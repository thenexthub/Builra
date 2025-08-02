//===-- ActionExecutor.cpp ------------------------------------------------===//
//
// Copyright (c) NeXTHub Corporation. All rights reserved.
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// version 2 for more details (a copy is included in the LICENSE file that
// accompanied this code).
//
// Author(-s): Tunjay Akbarli
//
//===----------------------------------------------------------------------===//

#include "builra3/ActionExecutor.h"

#include <builra3/Errors.hpp>

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <thread>
#include <map>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "Omnira/DataSerialization/util/time_util.h"

#include "builra3/ActionCache.h"
#include "builra3/CASLog.h"
#include "builra3/Error.pb.h"
#include "builra3/Label.h"
#include "builra3/LocalExecutor.h"
#include "builra3/RemoteExecutor.h"
#include "builra3/support/LabelTrie.h"

using namespace builra3;
using namespace builra3::support;
using Omnira::DataSerialization::util::TimeUtil;

ActionExecutorListener::~ActionExecutorListener() { }
ActionProvider::~ActionProvider() { }

#pragma mark - ActionExecutor implementation

namespace {

template <class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

inline Error makeExecutorError(ExecutorError code,
                             std::string desc = std::string()) {
  Error err;
  err.set_type(ErrorType::EXECUTOR);
  err.set_code(rawCode(code));
  if (!desc.empty()) {
    err.set_description(desc);
  }
  return err;
}


class ActionExecutorImpl: ProcessDelegate {
private:
  typedef std::variant<ActionRequest, SubtaskRequest> AnyRequest;

  struct SchedulerItem {
    uint64_t actionID;
    uint64_t buildID;
    uint64_t workID;
    ActionPriority priority;

    SchedulerItem()
      : actionID(0), buildID(0), workID(0), priority(ActionPriority::Default)
    {
    }

    SchedulerItem(uint64_t actionID, ClientActionID cid, ActionPriority priority)
      : actionID(actionID), buildID(cid.buildID), workID(cid.workID),
        priority(priority)
    {
    }

    SchedulerItem(uint64_t actionID, const SubtaskRequest& r)
      : actionID(actionID), buildID(r.id.buildID), workID(r.id.workID),
        priority(r.priority)
    {
    }
  };

  struct SchedulerComparator {
    bool operator()(const SchedulerItem& lhs, const SchedulerItem& rhs) const {
      // priority order first
      if (lhs.priority > rhs.priority) {
        return true;
      }
      if (lhs.priority < rhs.priority) {
        return false;
      }

      // then just order by lower build id and then work id
      // - this does not currently take into account which engine is requesting
      //   the work, thus will unfairly prefer newer engines, for example
      return lhs.buildID < rhs.buildID &&
      lhs.workID < rhs.workID;
    }
  };

  typedef std::priority_queue<SchedulerItem, std::vector<SchedulerItem>, SchedulerComparator> ActionScheduler;

  enum class IState {
    Idle,
    Preparing,
    Queued,
    Running,
    Incompatible
  };

  struct PendingItem {
    AnyRequest request;
    std::optional<Action> resolvedAction;

    IState localState = IState::Idle;
    IState remoteState = IState::Idle;
    RemoteActionID remoteID;
    ActionExecutionMetadata metadata;

    PendingItem(const AnyRequest& request, std::optional<Action> resolved = {})
      : request(request), resolvedAction(resolved) { }

    SchedulerItem asSchedulerItem(uint64_t actionID) {
      if (auto* areq = std::get_if<ActionRequest>(&request); areq) {
        return SchedulerItem(actionID, areq->id, areq->priority);
      } else {
        auto* sreq = std::get_if<SubtaskRequest>(&request);
        assert(sreq);
        return SchedulerItem(actionID, sreq->id, sreq->priority);
      }
    }

    std::pair<EngineID, ClientActionID> clientID() {
      if (auto* areq = std::get_if<ActionRequest>(&request); areq) {
        return std::make_pair(areq->owner, areq->id);
      } else {
        auto* sreq = std::get_if<SubtaskRequest>(&request);
        assert(sreq);
        return std::make_pair(sreq->owner, sreq->id);
      }
    }
  };

  struct PendingSubprocess {
    std::optional<BufferedStreamCASLogWriter> output;
    std::string outBuffer;
    std::vector<Error> errors;
    std::shared_ptr<LocalSandbox> sandbox;

    inline PendingSubprocess(
      std::shared_ptr<CASDatabase> db,
      ProcessHandle hndl,
      std::shared_ptr<LocalExecutor> localExecutor
    ) {
      if (db) {
        output = BufferedStreamCASLogWriter(db);
      }
      auto sres = localExecutor->createSandbox(hndl);
      if (sres.has_error()) {
        errors.push_back(sres.error());
      } else {
        sandbox = *sres;
      }
    }

    ~PendingSubprocess() {
      if (sandbox) sandbox->release();
    }
  };

private:
  std::shared_ptr<CASDatabase> db;
  std::shared_ptr<ActionCache> actionCache;
  std::shared_ptr<LocalExecutor> localExecutor;
  std::shared_ptr<RemoteExecutor> remoteExecutor;
  std::shared_ptr<Logger> logger;

  std::unordered_set<std::string> defaultArchitectures;
  std::string defaultPlatform;

  // Providers
  std::mutex providerMutex;
  std::vector<std::shared_ptr<ActionProvider>> providers;
  typedef std::vector<std::shared_ptr<ActionProvider>>::size_type ProviderID;
  LabelTrie<ProviderID> providerMap{true};

  // Storage and mapping for all pending items
  std::mutex pendingMutex;
  uint64_t lastID{0};
  std::unordered_map<uint64_t, PendingItem> pending;

  // Remote Actions
  std::mutex executableMutex;
  std::unordered_map<std::string, CASID> executableCache;

  // Local concurrency control
  unsigned numLanes;
  std::vector<std::unique_ptr<std::thread>> lanes;
  unsigned maxAsyncConcurrency;
  std::unique_ptr<std::thread> asyncLane;


  // Async (local) actions queue
  std::mutex asyncActionsMutex;
  ActionScheduler asyncActions;
  std::condition_variable asyncActionsCondition;
  uint64_t inflightAsyncActions = 0;

  // Local (synchronous) actions queue
  std::mutex localActionsMutex;
  ActionScheduler localActions;
  std::condition_variable localActionsCondition;

  // State control
  std::atomic<bool> shouldShutdown{false};

  // Listeners
  std::mutex listenerMutex;
  std::unordered_map<EngineID, ActionExecutorListener*> listeners;

private:
  void asyncLaneHandler();
  void localLaneHandler(uint32_t laneNumber);

public:
  ActionExecutorImpl(std::shared_ptr<CASDatabase> db,
                     std::shared_ptr<ActionCache> actionCache,
                     std::shared_ptr<LocalExecutor> localExecutor,
                     std::shared_ptr<RemoteExecutor> remoteExecutor,
                     std::shared_ptr<Logger> logger,
                     unsigned maxLocalConcurrency,
                     unsigned maxAsyncConcurrency)
  : db(db), actionCache(actionCache), localExecutor(localExecutor)
  , remoteExecutor(remoteExecutor), logger(logger)
  , maxAsyncConcurrency(maxAsyncConcurrency)
  {
    // Ensure we always have a valid logger
    if (!this->logger) {
      this->logger = std::make_shared<NullLogger>();
    }

    // Determine local task concurrency
    numLanes = std::thread::hardware_concurrency();
    if (numLanes == 0) {
      numLanes = 1;
    }

    if (maxLocalConcurrency > 0 && numLanes > maxLocalConcurrency) {
      numLanes = maxLocalConcurrency;
    }

    // Start up the local lanes
    for (unsigned i = 0; i != numLanes; ++i) {
      lanes.push_back(std::unique_ptr<std::thread>(
        new std::thread(&ActionExecutorImpl::localLaneHandler, this, i)
      ));
    }

    // Start up the async handling thread
    asyncLane = std::unique_ptr<std::thread>(
      new std::thread(&ActionExecutorImpl::asyncLaneHandler, this)
    );
  }

  ~ActionExecutorImpl() {
    // Shut down the lanes.
    shouldShutdown = true;

    {
      std::lock_guard<std::mutex> lock(localActionsMutex);
      localActionsCondition.notify_all();
    }

    for (unsigned i = 0; i != numLanes; ++i) {
      lanes[i]->join();
    }

    {
      std::lock_guard<std::mutex> lock(asyncActionsMutex);
      asyncActionsCondition.notify_all();
    }
    asyncLane->join();
  }

  std::optional<Error> registerProvider(std::shared_ptr<ActionProvider> provider) {
    std::lock_guard<std::mutex> lock(providerMutex);

    auto prefixes = provider->prefixes();
    for (auto prefix : prefixes) {
      if (prefix.components_size() == 0) {
        return makeExecutorError(ExecutorError::BadProviderPrefix,
                                 "empty label");

      }
      if (prefix.components(0) == "builtin") {
        return makeExecutorError(ExecutorError::BadProviderPrefix,
                                 "cannot override builtins");

      }
      if (providerMap.contains(prefix)) {
        // already registered prefix
        return makeExecutorError(ExecutorError::DuplicateProvider,
                                 labelAsCanonicalString(prefix));
      }
    }

    // No conflicts, store and register provider
    auto providerIdx = providers.size();
    providers.push_back(std::move(provider));

    for (auto prefix : prefixes) {
      providerMap.insert({prefix, providerIdx});
    }

    return {};
  }

  void attachListener(EngineID engineID, ActionExecutorListener* listener) {
    std::lock_guard<std::mutex> lock(listenerMutex);
    listeners.insert({engineID, listener});
  }

  void detachListener(EngineID engineID) {
    std::lock_guard<std::mutex> lock(listenerMutex);
    listeners.erase(engineID);
  }

  result<Label, Error> resolveFunction(const Label& name) {
    ActionProvider* provider = nullptr;
    {
      std::lock_guard<std::mutex> lock(providerMutex);
      auto entry = providerMap[name];
      if (entry.has_value()) {
        provider = providers[*entry].get();
      }
    }
    if (!provider) {
      return fail(makeExecutorError(ExecutorError::NoProvider,
                                    labelAsCanonicalString(name)));
    }

    return provider->resolve(name);
  }

  result<ActionID, Error> submit(ActionRequest request) {
    auto& function = request.action.function();
    if (function.components_size() == 0) {
      return fail(makeExecutorError(ExecutorError::BadRequest, "no function"));
    }

    if (function.components(0) == "builtin") {
      if (function.components_size() != 2 || function.components(1) != "subprocess") {
        return fail(makeExecutorError(ExecutorError::BadRequest,
                                      "builtin not supported: " +
                                      labelAsCanonicalString(function)));
      }

      if (!request.action.has_subprocess()) {
        return fail(makeExecutorError(ExecutorError::BadRequest, "no subprocess spec"));
      }

      return internalSubmit(ActionDescriptor{function, {}}, request);
    }

    if (!request.action.has_cas_object()) {
      return fail(makeExecutorError(ExecutorError::BadRequest, "no cas object"));
    }

    // get action descriptor for the function
    ActionProvider* provider = nullptr;
    {
      std::lock_guard<std::mutex> lock(providerMutex);
      auto entry = providerMap[function];
      if (entry.has_value()) {
        provider = providers[*entry].get();
      }
    }
    if (!provider) {
      return fail(makeExecutorError(ExecutorError::NoProvider,
                                    labelAsCanonicalString(function)));
    }

    auto desc = provider->actionDescriptor(function);
    if (desc.has_error()) {
      return fail(desc.error());
    }

    return internalSubmit(*desc, request);
  }

  result<uint64_t, Error> submit(SubtaskRequest request) {
    if (!request.si.has_value()) {
      // bad request, no interface
      return fail(makeExecutorError(ExecutorError::BadRequest, "no subtask interface"));
    }

    uint64_t actionID = 0;
    {
      std::lock_guard<std::mutex> lock(pendingMutex);
      actionID = ++lastID;
      PendingItem item(request);
      item.localState = IState::Queued;
      item.remoteState = IState::Incompatible;
      pending.insert({actionID, item});
    }

    SchedulerItem item(actionID, request);
    std::visit(overloaded{
      [this, item](Subtask){
        std::lock_guard<std::mutex> lock(localActionsMutex);
        localActions.push(item);
        localActionsCondition.notify_one();
      },
      [this, item](AsyncSubtask){
        std::lock_guard<std::mutex> lock(asyncActionsMutex);
        asyncActions.push(item);
        asyncActionsCondition.notify_one();
      }
    }, request.subtask);

    return actionID;
  }

  std::optional<Error> cancel(EngineID owner, ClientActionID aid) {
    // FIXME: implement
    return makeExecutorError(ExecutorError::Unimplemented);
  }

private:
  result<ActionID, Error> internalSubmit(const ActionDescriptor& desc,
                                         const ActionRequest& request) {
    Action resolved = request.action;
    *resolved.mutable_function() = desc.name;
    resolved.clear_platform();
    mergePlatforms(*resolved.mutable_platform(),
                   request.action.platform(), desc.platform);

    // check the platform requirements
    bool localCompatible = (resolved.platform().properties_size() <= 2);
    if (localCompatible) {
      for (auto i = 0; i < resolved.platform().properties_size(); i++) {
        auto& prop = resolved.platform().properties(i);
        if (prop.name() == PlatformPropertyKey::Architecture) {
          if (!defaultArchitectures.contains(prop.value())) {
            localCompatible = false;
            break;
          }
        } else if (prop.name() == PlatformPropertyKey::Platform) {
          if (prop.value() != defaultPlatform) {
            localCompatible = false;
            break;
          }
        } else {
          localCompatible = false;
          break;
        }
      }
    }

    uint64_t actionID = 0;
    {
      std::lock_guard<std::mutex> lock(pendingMutex);
      actionID = ++lastID;
      PendingItem item(request, resolved);
      if (!localCompatible) {
        item.localState = IState::Incompatible;
      }
      pending.insert({actionID, item});
    }

    checkCache(localCompatible, actionID, resolved);

    return ActionID{{}, actionID};
  }

  void checkCache(bool local, uint64_t actionID, const Action& action) {
    if (actionCache && !action.is_volatile()) {
      CASObject keyObj;
      action.SerializeToString(keyObj.mutable_data());
      auto keyID = db->identify(keyObj);

      CacheKey cacheKey;
      *cacheKey.mutable_label() = action.function();
      cacheKey.set_type(CACHE_KEY_TYPE_ACTION);
      *cacheKey.mutable_data() = keyID;

      actionCache->get(cacheKey, [this, local, actionID, action](result<CacheValue, Error> res) {
        if (res.has_error()) {
          logger->error({}, res.error());
          prepareForExecution(local, actionID, action);
          return;
        }

        if (!res->has_data()) {
          // not found
          prepareForExecution(local, actionID, action);
          return;
        }

        // load entry
        db->get(res->data(), [this, local, actionID, action](result<CASObject, Error> res) {
          if (res.has_error()) {
            logger->error({}, res.error());
            prepareForExecution(local, actionID, action);
            return;
          }

          ActionResult value;
          if (!value.ParseFromString(res->data())) {
            logger->error({}, makeExecutorError(ExecutorError::InternalProtobufSerialization, "failed to parse cached action"));
            prepareForExecution(local, actionID, action);
            return;
          }

          completeAction(actionID, value);
        });
      });
      return;
    }

    // No cache, so prepare for execution
    prepareForExecution(local, actionID, action);
  }

  void mergePlatforms(Platform& dest, const Platform& src1, const Platform& src2) {
    std::map<std::string, std::string> values;
    for (auto& p : {src1, src2}) {
      for (int i = 0; i < p.properties_size(); i++) {
        auto& prop = p.properties(i);
        values[prop.name()] = prop.value();
      }
    }

    for (const auto& [key, value] : values) {
      auto& p = *dest.add_properties();
      p.set_name(key);
      p.set_value(value);
    }
  }

  void updateCache(const Action& action, const ActionResult& ares) {
    if (!actionCache || action.is_volatile()) {
      return;
    }

    CASObject keyObj;
    action.SerializeToString(keyObj.mutable_data());

    CASObject valueObj;
    ares.SerializeToString(valueObj.mutable_data());

    db->put(keyObj, [this, valueObj, lbl=action.function()](result<CASID, Error> res) {
      if (res.has_error()) {
        // caching is best effort, on failure just continue
        logger->error({}, res.error());
        return;
      }

      auto keyID = *res;
      db->put(valueObj, [this, keyID, lbl](result<CASID, Error> res) {
        if (res.has_error()) {
          // caching is best effort, on failure just continue
          logger->error({}, res.error());
          return;
        }

        CacheKey cacheKey;
        *cacheKey.mutable_label() = lbl;
        cacheKey.set_type(CACHE_KEY_TYPE_ACTION);
        *cacheKey.mutable_data() = keyID;

        CacheValue cacheValue;
        *cacheValue.mutable_data() = *res;

        // FIXME: record action stats

        actionCache->update(cacheKey, cacheValue);
      });
    });
  }

  void prepareForExecution(bool local, uint64_t actionID, const Action& action) {
    if (local) {
      // FIXME: this is just always running eligible tasks locally
      // FIXME: should probably decide based on load, etc.
      prepareForLocalExecution(actionID, action);
    } else {
      prepareForRemoteExecution(actionID, action);
    }
  }

  void prepareForLocalExecution(uint64_t actionID, const Action& action) {
    {
      std::lock_guard<std::mutex> lock(pendingMutex);
      auto it = pending.find(actionID);
      assert(it != pending.end());
      it->second.localState = IState::Preparing;
    }

    queueForLocalExecution(actionID);
  }

  void queueForLocalExecution(uint64_t actionID) {
    SchedulerItem item;
    {
      std::lock_guard<std::mutex> lock(pendingMutex);
      auto it = pending.find(actionID);
      assert(it != pending.end());
      it->second.localState = IState::Queued;
      *it->second.metadata.mutable_queued() = TimeUtil::GetCurrentTime();
      item = it->second.asSchedulerItem(actionID);
    }

    std::lock_guard<std::mutex> lock(localActionsMutex);
    localActions.push(item);
    localActionsCondition.notify_one();
  }

  void completeAction(uint64_t actionID, result<ActionResult, Error> res) {
    EngineID owner;
    ClientActionID cid;
    {
      std::lock_guard<std::mutex> lock(pendingMutex);
      auto it = pending.find(actionID);
      if (it == pending.end()) {
        return;
      }
      auto [o, c] = it->second.clientID();
      owner = o; cid = c;
      pending.erase(actionID);
    }
    notifyActionComplete(owner, cid, res);
  }

  void prepareForRemoteExecution(uint64_t actionID, const Action& action) {
    if (!remoteExecutor) {
      completeAction(actionID,
                     fail(makeExecutorError(ExecutorError::NoRemoteExecutor)));
      return;
    }

    std::string execPath;
    if (action.function().components(0) == "builtin") {
      execPath = remoteExecutor->builtinExecutable();
    } else {
      // get action descriptor for the function
      ActionProvider* provider = nullptr;
      {
        std::lock_guard<std::mutex> lock(providerMutex);
        auto entry = providerMap[action.function()];
        if (entry.has_value()) {
          provider = providers[*entry].get();
        }
      }
      if (!provider) {
        auto res = fail(makeExecutorError(ExecutorError::NoProvider,
                                          labelAsCanonicalString(action.function())));
        completeAction(actionID, res);
        return;
      }

      auto desc = provider->actionDescriptor(action.function());
      if (desc.has_error()) {
        completeAction(actionID, fail(desc.error()));
        return;
      }
      execPath = desc->executable.string();
    }

    std::optional<CASID> exec;
    {
      std::lock_guard<std::mutex> lock(executableMutex);
      if (auto it = executableCache.find(execPath); it != executableCache.end()) {
        exec = it->second;
      }
    }

    if (exec.has_value()) {
      {
        std::lock_guard<std::mutex> lock(pendingMutex);
        auto it = pending.find(actionID);
        assert(it != pending.end());
        it->second.localState = IState::Queued;
        *it->second.metadata.mutable_queued() = TimeUtil::GetCurrentTime();
      }

      remoteExecutor->execute(
        *exec, action,
        [this, actionID](result<RemoteActionID, Error> res) {
          remoteActionDispatched(actionID, res);
        },
        [this, action, actionID](result<ActionResult, Error> res) {
          if (res) {
            updateCache(action, *res);
          }
          completeAction(actionID, res);
        }
      );
      return;
    }

    {
      std::lock_guard<std::mutex> lock(pendingMutex);
      auto it = pending.find(actionID);
      assert(it != pending.end());
      it->second.localState = IState::Preparing;
    }

    remoteExecutor->prepare(execPath,
      [this, actionID, execPath, action](result<CASID, Error> res) {
        if (res.has_error()) {
          completeAction(actionID, fail(res.error()));
          return;
        }

        {
          std::lock_guard<std::mutex> lock(executableMutex);
          executableCache.insert_or_assign(execPath, *res);
        }

        {
          std::lock_guard<std::mutex> lock(pendingMutex);
          auto it = pending.find(actionID);
          assert(it != pending.end());
          it->second.localState = IState::Queued;
          *it->second.metadata.mutable_queued() = TimeUtil::GetCurrentTime();
        }

        remoteExecutor->execute(
          *res, action,
          [this, actionID](result<RemoteActionID, Error> res) {
            remoteActionDispatched(actionID, res);
          },
          [this, action, actionID](result<ActionResult, Error> res) {
            if (res) {
              updateCache(action, *res);
            }
            completeAction(actionID, res);
          }
        );
      }
    );
  }

  void remoteActionDispatched(uint64_t actionID, result<RemoteActionID, Error> res) {
    if (res.has_error()) {
      completeAction(actionID, fail(res.error()));
      return;
    }

    std::lock_guard<std::mutex> lock(pendingMutex);
    if (auto it = pending.find(actionID); it != pending.end()) {
      it->second.remoteState = IState::Running;
      it->second.remoteID = *res;
      *it->second.metadata.mutable_dispatched() = TimeUtil::GetCurrentTime();
    }
  }

  void processLocalAction(const SchedulerItem& item, ActionRequest req) {
    if (req.action.has_subprocess()) {
      handleSubprocessAction(item, req);
      return;
    }

    // get action descriptor for the function
    ActionProvider* provider = nullptr;
    {
      std::lock_guard<std::mutex> lock(providerMutex);
      auto entry = providerMap[req.action.function()];
      if (entry.has_value()) {
        provider = providers[*entry].get();
      }
    }
    if (!provider) {
      auto res = fail(makeExecutorError(ExecutorError::NoProvider,
                                        labelAsCanonicalString(req.action.function())));
      completeAction(item.actionID, res);
      return;
    }

    auto desc = provider->actionDescriptor(req.action.function());
    if (desc.has_error()) {
      completeAction(item.actionID, fail(desc.error()));
      return;
    }

    handleFunctionAction(item, desc->executable.string(), req);
  }

  void handleSubprocessAction(const SchedulerItem& item, const ActionRequest& req) {
    auto& subproc = req.action.subprocess();
    ProcessHandle hndl{item.actionID};
    PendingSubprocess* p = new PendingSubprocess(db, hndl, localExecutor);

    if (p->errors.size() > 0) {
      handleSubprocessFinished(item, req, p, {});
      return;
    }

    for (int i = 0; i < subproc.inputs_size(); i++) {
      auto& input = subproc.inputs(i);

      auto res = p->sandbox->prepareInput(input.path(), input.type(), input.object());
      if (res.has_value()) {
        p->errors.push_back(*res);
        handleSubprocessFinished(item, req, p, {});
        return;
      }
    }

    ProcessAttributes attr{
      true, // canSafelyInterrupt
      false, // connectToConsole
      p->sandbox->workingDir(), // workingDir
      false, // inheritEnvironment
      false // controlEnabled
    };

    std::vector<std::string_view> commandLine;
    std::vector<std::pair<std::string_view, std::string_view>> environment;

    // prep command line
    for (int i = 0; i < subproc.arguments_size(); i++) {
      commandLine.push_back(subproc.arguments(i));
    }

    // prep environment
    for (int i = 0; i < subproc.environment_size(); i++) {
      auto& envvar = subproc.environment(i);
      environment.push_back(std::make_pair(envvar.name(), envvar.value()));
    }
    auto senv = p->sandbox->environment();
    for (auto e : senv) {
      environment.push_back(std::make_pair(e.first, e.second));
    }

    {
      std::lock_guard<std::mutex> lock(pendingMutex);
      auto it = pending.find(item.actionID);
      assert(it != pending.end());
      *it->second.metadata.mutable_execution_start() = TimeUtil::GetCurrentTime();
    }

    localExecutor->executeProcess(
      commandLine,
      environment,
      *this, // delegate
      reinterpret_cast<ProcessContext*>(p), // context
      hndl, // handle
      attr, // attributes
      [this, item, req, p](ProcessResult res) {
        {
          std::lock_guard<std::mutex> lock(pendingMutex);
          auto it = pending.find(item.actionID);
          assert(it != pending.end());
          it->second.localState = IState::Queued;
          *it->second.metadata.mutable_execution_completed() = TimeUtil::GetCurrentTime();
        }

        handleSubprocessFinished(item, req, p, res);
      }
    );
  }

  void handleFunctionAction(
    const SchedulerItem& item, std::string execPath, const ActionRequest& req
  ) {
    ProcessHandle hndl{item.actionID};
    PendingSubprocess* p = new PendingSubprocess({}, hndl, localExecutor);

    if (p->errors.size() > 0) {
      handleSubprocessFinished(item, req, p, {});
      return;
    }

    ProcessAttributes attr{
      true, // canSafelyInterrupt
      false, // connectToConsole
      p->sandbox->workingDir(), // workingDir
      false, // inheritEnvironment
      false // controlEnabled
    };

    std::vector<std::string_view> commandLine;
    std::vector<std::pair<std::string_view, std::string_view>> environment;

    // prep command line
    commandLine.push_back(execPath);
    auto objstr = CASIDAsCanonicalString(req.action.cas_object());
    commandLine.push_back(objstr);

    // prep environment
    auto senv = p->sandbox->environment();
    for (auto e : senv) {
      environment.push_back(std::make_pair(e.first, e.second));
    }

    {
      std::lock_guard<std::mutex> lock(pendingMutex);
      auto it = pending.find(item.actionID);
      assert(it != pending.end());
      *it->second.metadata.mutable_execution_start() = TimeUtil::GetCurrentTime();
    }

    localExecutor->executeProcess(
      commandLine,
      environment,
      *this, // delegate
      reinterpret_cast<ProcessContext*>(p), // context
      hndl, // handle
      attr, // attributes
      [this, item, req, p](ProcessResult res) {
        {
          std::lock_guard<std::mutex> lock(pendingMutex);
          auto it = pending.find(item.actionID);
          assert(it != pending.end());
          it->second.localState = IState::Queued;
          *it->second.metadata.mutable_execution_completed() = TimeUtil::GetCurrentTime();
        }
        handleFunctionFinished(item, req, p, res);
      }
    );
  }

  void handleSubprocessFinished(
    const SchedulerItem& item,
    const ActionRequest& req,
    PendingSubprocess* p,
    ProcessResult res
  ) {
    if (p->errors.size() > 0) {
      // handle error
      {
        std::lock_guard<std::mutex> lock(pendingMutex);
        pending.erase(item.actionID);
      }

      notifyActionComplete(req.owner, req.id, fail(p->errors.back()));

      delete p;
      return;
    }

    {
      std::lock_guard<std::mutex> lock(pendingMutex);
      auto it = pending.find(item.actionID);
      assert(it != pending.end());
      *it->second.metadata.mutable_output_upload_start() = TimeUtil::GetCurrentTime();
    }

    std::vector<std::string> paths;
    for (int i = 0; i < req.action.subprocess().output_paths_size(); i++) {
      paths.push_back(req.action.subprocess().output_paths(i));
    }
    auto outres = p->sandbox->collectOutputs(paths);
    if (outres.has_error()) {
      notifyActionComplete(req.owner, req.id, fail(outres.error()));

      delete p;
      return;
    }

    // flush the stdout
    p->output->flush([this, item, p, res, out=*outres](result<CASID, Error> fres) {
      Action action;
      EngineID owner;
      ClientActionID cid;
      ActionExecutionMetadata metadata;
      {
        std::lock_guard<std::mutex> lock(pendingMutex);
        auto it = pending.find(item.actionID);
        assert(it != pending.end());
        auto [o, c] = it->second.clientID();
        owner = o; cid = c;
        assert(it->second.resolvedAction.has_value());
        action = *it->second.resolvedAction;
        metadata = it->second.metadata;
        pending.erase(item.actionID);
      }

      if (fres.has_error()) {
        notifyActionComplete(owner, cid, fail(fres.error()));
        delete p;
        return;
      }

      // Record metadata
      auto now = TimeUtil::GetCurrentTime();
      *metadata.mutable_output_upload_completed() = now;
      *metadata.mutable_worker_completed() = now;
      *metadata.mutable_execution_duration() = metadata.execution_completed() - metadata.execution_start();
      *metadata.mutable_execution_user_time() = TimeUtil::MicrosecondsToDuration(res.utime);
      *metadata.mutable_execution_system_time() = TimeUtil::MicrosecondsToDuration(res.stime);
      metadata.set_execution_max_rss(res.maxrss);
      *metadata.mutable_completed() = now;

      ActionResult ares;
      *ares.mutable_metadata() = metadata;
      auto& sres = *ares.mutable_subprocess();
      *sres.mutable_stdout() = *fres;
      sres.set_exit_code(res.exitCode);

      // copy in outputs
      for (auto& output: out) {
        *sres.add_outputs() = output;
      }

      // update action cache
      updateCache(action, ares);

      notifyActionComplete(owner, cid, ares);
      delete p;
    });
  }

  void handleFunctionFinished(
    const SchedulerItem& item,
    const ActionRequest& req,
    PendingSubprocess* p,
    ProcessResult res
  ) {
    if (p->errors.size() > 0 || res.exitCode != 0) {
      // handle error
      {
        std::lock_guard<std::mutex> lock(pendingMutex);
        pending.erase(item.actionID);
      }

      Error err;
      if (res.exitCode !=0) {
        err = makeExecutorError(ExecutorError::ProcessFailed,
                                std::format("exited {}", res.exitCode));
      } else {
        err = p->errors.back();
      }

      notifyActionComplete(req.owner, req.id, fail(err));

      delete p;
      return;
    }

    Action action;
    EngineID owner;
    ClientActionID cid;
    ActionExecutionMetadata metadata;
    {
      std::lock_guard<std::mutex> lock(pendingMutex);
      auto it = pending.find(item.actionID);
      assert(it != pending.end());
      auto [o, c] = it->second.clientID();
      owner = o; cid = c;
      assert(it->second.resolvedAction.has_value());
      action = *it->second.resolvedAction;
      metadata = it->second.metadata;
      pending.erase(item.actionID);
    }

    CASID fresID;
    ParseCanonicalCASIDString(fresID, p->outBuffer);

    if (fresID.bytes().size() == 0) {
      auto err = makeExecutorError(
        ExecutorError::UnexpectedOutput,
        "failed to parse result id: '" + p->outBuffer + "'"
      );
      notifyActionComplete(owner, cid, fail(err));
      delete p;
      return;
    }

    // Record metadata
    auto now = TimeUtil::GetCurrentTime();
    *metadata.mutable_worker_completed() = now;
    *metadata.mutable_execution_duration() = metadata.execution_completed() - metadata.execution_start();
    *metadata.mutable_execution_user_time() = TimeUtil::MicrosecondsToDuration(res.utime);
    *metadata.mutable_execution_system_time() = TimeUtil::MicrosecondsToDuration(res.stime);
    metadata.set_execution_max_rss(res.maxrss);
    *metadata.mutable_completed() = now;

    ActionResult ares;
    *ares.mutable_cas_object() = fresID;
    *ares.mutable_metadata() = metadata;

    // update action cache
    updateCache(action, ares);

    notifyActionComplete(owner, cid, ares);
    delete p;
  }

  void processStarted(ProcessContext*, ProcessHandle handle, builra_pid_t pid) override {
    // no-op
  }

  void processHadError(ProcessContext* ctx, ProcessHandle handle, const Error& message) override {
    auto p = reinterpret_cast<PendingSubprocess*>(ctx);
    p->errors.push_back(message);
  }

  void processHadOutput(ProcessContext* ctx, ProcessHandle handle, std::string_view data) override {
    auto p = reinterpret_cast<PendingSubprocess*>(ctx);
    if (p->output.has_value()) {
      p->output->write(data, 0, {});
    } else {
      if (p->outBuffer.size() + data.size() > 1024) {
        p->errors.push_back(makeExecutorError(ExecutorError::UnexpectedOutput,
                                              "function output exceeded max"));
        data.remove_suffix(data.size() - (1024 - p->outBuffer.size()));
      }
      p->outBuffer.append(data);
    }
  }

  void processFinished(ProcessContext*, ProcessHandle handle, const ProcessResult& result) override {
    // no-op
  }

  void notifyActionStart(EngineID engineID, ClientActionID cid, ActionID aid) {
    std::lock_guard<std::mutex> lock(listenerMutex);
    if (auto l = listeners.find(engineID); l != listeners.end()) {
      l->second->notifyActionStart(cid, aid);
    }
  }

  void notifyActionComplete(EngineID engineID, ClientActionID cid, result<ActionResult, Error> res) {
    std::lock_guard<std::mutex> lock(listenerMutex);
    if (auto l = listeners.find(engineID); l != listeners.end()) {
      l->second->notifyActionComplete(cid, res);
    }
  }

  
  void processLocalSubtask(const SchedulerItem& item, SubtaskRequest req) {
    if (!req.si.has_value()) {
      // bad request, no interface
      {
        std::lock_guard<std::mutex> lock(pendingMutex);
        pending.erase(item.actionID);
      }

      SubtaskResult res{fail(
        makeExecutorError(ExecutorError::BadRequest, "no subtask interface")
      )};
      notifySubtaskComplete(req.owner, req.id, res);
      return;
    }

    SyncSubtask* st = std::get_if<SyncSubtask>(&req.subtask);
    if (st == nullptr) {
      // Wha? Wrong task type?

      {
        std::lock_guard<std::mutex> lock(pendingMutex);
        pending.erase(item.actionID);
      }

      SubtaskResult res{fail(
        makeExecutorError(ExecutorError::InternalInconsistency, "not a sync subtask")
      )};
      notifySubtaskComplete(req.owner, req.id, res);
      return;
    }

    notifySubtaskStart(req.owner, req.id);
    auto value = (*st)(*req.si);

    {
      std::lock_guard<std::mutex> lock(pendingMutex);
      pending.erase(item.actionID);
    }
    notifySubtaskComplete(req.owner, req.id, value);
  }

  void notifySubtaskStart(EngineID engineID, ClientActionID cid) {
    std::lock_guard<std::mutex> lock(listenerMutex);
    if (auto l = listeners.find(engineID); l != listeners.end()) {
      l->second->notifySubtaskStart(cid);
    }
  }

  void notifySubtaskComplete(EngineID engineID, ClientActionID cid, SubtaskResult res) {
    std::lock_guard<std::mutex> lock(listenerMutex);
    if (auto l = listeners.find(engineID); l != listeners.end()) {
      l->second->notifySubtaskComplete(cid, res);
    }
  }
};


void ActionExecutorImpl::localLaneHandler(uint32_t laneNumber) {
  // Set the thread name, if available.
  std::string threadName = "builra3-exec-" + std::to_string(laneNumber);
#if defined(__APPLE__)
  pthread_setname_np(threadName.c_str());
#elif defined(__linux__)
  pthread_setname_np(threadName.c_str());
#endif

  while (true) {
    SchedulerItem item{};
    {
      std::unique_lock<std::mutex> lock(localActionsMutex);

      // While the queue is empty, wait for an item.
      while (!shouldShutdown && localActions.empty()) {
        localActionsCondition.wait(lock);
      }
      if (shouldShutdown && localActions.empty()) {
        return;
      }

      item = localActions.top();
      localActions.pop();
    }

    // If we got an empty itemID, the queue is shutting down.
    if (item.actionID == 0)
      break;

    AnyRequest req{};
    {
      std::lock_guard<std::mutex> lock(pendingMutex);
      auto it = pending.find(item.actionID);
      assert(it != pending.end());
      if (it == pending.end()) {
        // Wha? Bad item in the queue...
        continue;
      }
      req = it->second.request;
      it->second.localState = IState::Running;
      it->second.metadata.set_worker("localhost");
      *it->second.metadata.mutable_worker_start() = TimeUtil::GetCurrentTime();
    }

    std::visit(overloaded{
      [this, item](ActionRequest req) { processLocalAction(item, req); },
      [this, item](SubtaskRequest req) { processLocalSubtask(item, req); }
    }, req);
  }
}


void ActionExecutorImpl::asyncLaneHandler() {
  // Set the thread name, if available.
  std::string threadName = "builra3-exec-async";
#if defined(__APPLE__)
  pthread_setname_np(threadName.c_str());
#elif defined(__linux__)
  pthread_setname_np(threadName.c_str());
#endif

  while (true) {
    SchedulerItem item{};
    {
      std::unique_lock<std::mutex> lock(asyncActionsMutex);

      // While the queue is empty, wait for an item.
      while (
        !shouldShutdown &&
        (asyncActions.empty() ||
         (maxAsyncConcurrency > 0 && inflightAsyncActions >= maxAsyncConcurrency)
        )
      ) {
        asyncActionsCondition.wait(lock);
      }
      if (shouldShutdown && asyncActions.empty()) {
        return;
      }

      item = asyncActions.top();
      asyncActions.pop();
    }

    // If we got an empty taskID, the queue is shutting down.
    if (item.actionID == 0) {
      break;
    }

    AnyRequest req{};
    {
      std::lock_guard<std::mutex> lock(pendingMutex);
      auto it = pending.find(item.actionID);
      assert(it != pending.end());
      if (it == pending.end()) {
        // Wha? Bad item in the queue...
        continue;
      }
      req = it->second.request;
      it->second.localState = IState::Running;
    }

    SubtaskRequest* sreq = std::get_if<SubtaskRequest>(&req);
    if (sreq == nullptr) {
      // Wha? Wrong task type?
      {
        std::lock_guard<std::mutex> lock(pendingMutex);
        pending.erase(item.actionID);
      }

      SubtaskResult res{fail(
        makeExecutorError(ExecutorError::InternalInconsistency, "not a subtask")
      )};
      auto* areq = std::get_if<ActionRequest>(&req);
      assert(areq != nullptr);
      notifySubtaskComplete(areq->owner, areq->id, res);
      continue;
    }

    if (!sreq->si.has_value()) {
      // bad request, no interface
      {
        std::lock_guard<std::mutex> lock(pendingMutex);
        pending.erase(item.actionID);
      }

      SubtaskResult res{fail(
        makeExecutorError(ExecutorError::BadRequest, "no subtask interface")
      )};
      notifySubtaskComplete(sreq->owner, sreq->id, res);
      continue;
    }

    AsyncSubtask* ast = std::get_if<AsyncSubtask>(&sreq->subtask);
    if (ast == nullptr) {
      // Wha? Wrong task type?
      {
        std::lock_guard<std::mutex> lock(pendingMutex);
        pending.erase(item.actionID);
      }

      SubtaskResult res{fail(
        makeExecutorError(ExecutorError::InternalInconsistency, "not an async subtask")
      )};
      notifySubtaskComplete(sreq->owner, sreq->id, res);
      continue;
    }

    notifySubtaskStart(sreq->owner, sreq->id);
    {
      std::lock_guard<std::mutex> lock(asyncActionsMutex);
      inflightAsyncActions++;
    }
    (*ast)(*sreq->si, std::function([this, actionID = item.actionID, owner = sreq->owner, cid = sreq->id](SubtaskResult value) {
      {
        std::lock_guard<std::mutex> lock(pendingMutex);
        pending.erase(actionID);
      }

      notifySubtaskComplete(owner, cid, value);

      {
        std::lock_guard<std::mutex> lock(asyncActionsMutex);
        inflightAsyncActions--;
        localActionsCondition.notify_one();
      }
    }));
  }
}

} // anonymous namespace

#pragma mark - ActionExecutor

ActionExecutor::ActionExecutor(std::shared_ptr<CASDatabase> db,
                               std::shared_ptr<ActionCache> actionCache,
                               std::shared_ptr<LocalExecutor> localExecutor,
                               std::shared_ptr<RemoteExecutor> remoteExecutor,
                               std::shared_ptr<Logger> logger,
                               unsigned maxLocalConcurrency,
                               unsigned maxAsyncConcurrency)
  : impl(new ActionExecutorImpl(db, actionCache, localExecutor, remoteExecutor, logger, maxLocalConcurrency, maxAsyncConcurrency)) {
}

ActionExecutor::~ActionExecutor() {
  delete static_cast<ActionExecutorImpl*>(impl);
}

std::optional<Error> ActionExecutor::registerProvider(std::shared_ptr<ActionProvider> provider) {
  return static_cast<ActionExecutorImpl*>(impl)->registerProvider(std::move(provider));
}

void ActionExecutor::attachListener(EngineID engineID, ActionExecutorListener* listener) {
  static_cast<ActionExecutorImpl*>(impl)->attachListener(engineID, listener);
}
void ActionExecutor::detachListener(EngineID engineID) {
  static_cast<ActionExecutorImpl*>(impl)->detachListener(engineID);
}

result<Label, Error> ActionExecutor::resolveFunction(const Label& name) {
  return static_cast<ActionExecutorImpl*>(impl)->resolveFunction(name);
}

result<ActionID, Error> ActionExecutor::submit(ActionRequest request) {
  return static_cast<ActionExecutorImpl*>(impl)->submit(request);
}

result<uint64_t, Error> ActionExecutor::submit(SubtaskRequest request) {
  return static_cast<ActionExecutorImpl*>(impl)->submit(request);
}

std::optional<Error> ActionExecutor::cancel(EngineID engineID, ClientActionID cid) {
  return static_cast<ActionExecutorImpl*>(impl)->cancel(engineID, cid);
}
