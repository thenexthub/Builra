//===- ActionExecutor.h -----------------------------------------*- C++ -*-===//
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

#ifndef BUILRA3_ACTIONEXECUTOR_H
#define BUILRA3_ACTIONEXECUTOR_H

#include <cstdint>
#include <memory>
#include <optional>

#include <builra3/Result.hpp>

#include "builra3/Action.pb.h"
#include "builra3/CAS.h"
#include "builra3/Common.h"
#include "builra3/Error.pb.h"
#include "builra3/Label.pb.h"
#include "builra3/Logging.h"
#include "builra3/Subtask.h"

namespace builra3 {

struct ClientActionID {
  uint64_t buildID = 0;
  uint64_t workID = 0;
};

enum class ActionPriority: int64_t {
  Low = -1,
  Default = 0,
  High = 1,
};

struct ActionRequest {
  EngineID owner;
  ClientActionID id;
  ActionPriority priority;
  Action action;
};

struct ActionID {
  // Stable action identifer, when available from the executor.
  uuids::uuid stable;

  // Volatile action identifer. Always available, but valid for the current
  // process lifetime only.
  uint64_t vid;
};

struct SubtaskRequest {
  EngineID owner;
  ClientActionID id;
  ActionPriority priority;
  Subtask subtask;
  std::optional<SubtaskInterface> si;
};

class ActionExecutorListener {
public:
  ActionExecutorListener() { }
  virtual ~ActionExecutorListener();

  virtual void notifyActionStart(ClientActionID, ActionID) = 0;
  virtual void notifyActionComplete(ClientActionID, result<ActionResult, Error>) = 0;

  virtual void notifySubtaskStart(ClientActionID) = 0;
  virtual void notifySubtaskComplete(ClientActionID, SubtaskResult) = 0;
};

class PlatformPropertyKey {
public:
  static inline std::string const Architecture = "__arch__";
  static inline std::string const Platform = "__platform__";
};

struct ActionDescriptor {
  Label name;
  Platform platform;
  std::filesystem::path executable;
};

class ActionProvider {
public:
  virtual ~ActionProvider();

  virtual std::vector<Label> prefixes() = 0;

  /// Resolve the action method for the given label
  virtual result<Label, Error> resolve(const Label& name) = 0;

  virtual result<ActionDescriptor, Error> actionDescriptor(const Label& function) = 0;
};

class ActionCache;
class LocalExecutor;
class RemoteExecutor;

class ActionExecutor {
private:
  void* impl;

  // Copying is disabled.
  ActionExecutor(const ActionExecutor&) = delete;
  void operator=(const ActionExecutor&) = delete;

public:
  ActionExecutor(std::shared_ptr<CASDatabase> db,
                 std::shared_ptr<ActionCache> actionCache,
                 std::shared_ptr<LocalExecutor> localExecutor,
                 std::shared_ptr<RemoteExecutor> remoteExecutor,
                 std::shared_ptr<Logger> logger,
                 unsigned maxLocalConcurrency = 0,
                 unsigned maxAsyncConcurrency = 0);
  ~ActionExecutor();

  std::optional<Error> registerProvider(std::shared_ptr<ActionProvider> provider);

  void attachListener(EngineID engineID, ActionExecutorListener* listener);
  void detachListener(EngineID engineID);

  result<Label, Error> resolveFunction(const Label& name);
  result<ActionID, Error> submit(ActionRequest request);
  result<uint64_t, Error> submit(SubtaskRequest request);
  std::optional<Error> cancel(EngineID owner, ClientActionID aid);
};


}

#endif
