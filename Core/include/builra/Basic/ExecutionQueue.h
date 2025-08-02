//===- ExecutionQueue.h -----------------------------------------*- C++ -*-===//
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

#ifndef BUILRA_BASIC_EXECUTIONQUEUE_H
#define BUILRA_BASIC_EXECUTIONQUEUE_H

#include "builra/Basic/Compiler.h"
#include "builra/Basic/toolchain.h"
#include "builra/Basic/Subprocess.h"

#include <cstdint>

namespace builra {
  namespace basic {

    /// MARK: Execution Queue
    
    class ExecutionQueueDelegate;

    // Default shell system path
    const std::string DefaultShellPath = "/bin/sh";

    /// Description of the queue job, used for scheduling and diagnostics.
    class JobDescriptor {
    public:
      JobDescriptor() {}
      virtual ~JobDescriptor();

      /// Get a name used for ordering this job
      virtual StringRef getOrdinalName() const = 0;

      /// Get a short description of the command, for use in status reporting.
      virtual void getShortDescription(SmallVectorImpl<char> &result) const = 0;

      /// Get a verbose description of the command, for use in status reporting.
      virtual void getVerboseDescription(SmallVectorImpl<char> &result) const = 0;
    };


    /// Opaque type which allows the queue implementation to maintain additional
    /// state and associate subsequent requests (e.g., \see executeProcess())
    /// with the dispatching job.
    class QueueJobContext {
    public:
      virtual ~QueueJobContext();
      virtual unsigned laneID() const = 0;
    };

    /// Wrapper for individual pieces of work that are added to the execution
    /// queue.
    class QueueJob {
      JobDescriptor* desc = nullptr;

      /// The function to execute to do the work.
      typedef std::function<void(QueueJobContext*)> work_fn_ty;
      work_fn_ty work;

    public:
      /// Default constructor, for use as a sentinel.
      QueueJob() {}

      /// General constructor.
      QueueJob(JobDescriptor* desc, work_fn_ty work)
      : desc(desc), work(work) {}

      JobDescriptor* getDescriptor() const { return desc; }

      void execute(QueueJobContext* context) { work(context); }
    };

    enum class QueueJobPriority {
      Normal,
      High
    };

    /// This abstact class encapsulates the interface needed for contributing
    /// work which needs to be executed.
    class ExecutionQueue {
      // DO NOT COPY
      ExecutionQueue(const ExecutionQueue&) BUILRA_DELETED_FUNCTION;
      void operator=(const ExecutionQueue&) BUILRA_DELETED_FUNCTION;
      ExecutionQueue& operator=(ExecutionQueue&&) BUILRA_DELETED_FUNCTION;

      ExecutionQueueDelegate& delegate;

    public:
      ExecutionQueue(ExecutionQueueDelegate& delegate);
      virtual ~ExecutionQueue();

      /// @name Accessors
      /// @{

      ExecutionQueueDelegate& getDelegate() { return delegate; }
      const ExecutionQueueDelegate& getDelegate() const { return delegate; }

      /// @}

      /// Add a job to be executed.
      virtual void addJob(QueueJob job, QueueJobPriority priority = QueueJobPriority::Normal) = 0;

      /// Cancel all jobs and subprocesses of this queue.
      virtual void cancelAllJobs() = 0;


      /// @name Execution Interfaces
      ///
      /// These are additional interfaces provided by the execution queue which
      /// can be invoked by the individual \see QueueJob::execute() to perform
      /// particular actions. The benefit of delegating to the execution queue
      /// to perform these actions is that the queue can potentially do a better
      /// job of scheduling activities.
      ///
      /// @{

      /// Execute the given command line.
      ///
      /// This will launch and execute the given command line and wait for it to
      /// complete or release its execution lane.
      ///
      /// \param context The context object passed to the job's worker function.
      ///
      /// \param commandLine The command line to execute.
      ///
      /// \param environment The environment to launch with.
      ///
      /// \param completionFn An optional function that, if supplied, will be
      /// run following the completion of the process. This may be run
      /// asynchronously from another thread if the executed process asks the
      /// system to release its execution lane. Callers should put cleanup and
      /// notification work here.
      ///
      /// \param attributes Additional attributes for the process to be spawned.
      //
      // FIXME: This interface will need to get more complicated, and provide the
      // command result and facilities for dealing with the output.
      virtual void
      executeProcess(QueueJobContext* context,
                     ArrayRef<StringRef> commandLine,
                     ArrayRef<std::pair<StringRef, StringRef>> environment,
                     ProcessAttributes attributes = {true},
                     toolchain::Optional<ProcessCompletionFn> completionFn = {toolchain::None},
                     ProcessDelegate* delegate = nullptr) = 0;

      /// @}

      /// Execute the given command, using an inherited environment.
      ProcessStatus executeProcess(QueueJobContext* context,
                                   ArrayRef<StringRef> commandLine);

      /// Execute the given command using "/bin/sh".
      ///
      /// This will launch and execute the given command line and wait for it to
      /// complete.
      ///
      /// \param context The context object passed to the job's worker function.
      /// \param command The command to execute.
      /// \returns True on success.
      //
      // FIXME: This interface will need to get more complicated, and provide the
      // command result and facilities for dealing with the output.
      bool executeShellCommand(QueueJobContext* context, StringRef command);

    };

    /// Delegate interface for execution queue status.
    ///
    /// All delegate interfaces are invoked synchronously by the execution queue,
    /// and should defer any long running operations to avoid blocking the queue
    /// unnecessarily.
    ///
    /// NOTE: The delegate *MUST* be thread-safe with respect to all calls, which
    /// will arrive concurrently and without any specified thread.
    class ExecutionQueueDelegate : public ProcessDelegate {
      // DO NOT COPY
      ExecutionQueueDelegate(const ExecutionQueueDelegate&)
          BUILRA_DELETED_FUNCTION;
      void operator=(const ExecutionQueueDelegate&) BUILRA_DELETED_FUNCTION;
      ExecutionQueueDelegate &operator=(ExecutionQueueDelegate&& rhs)
          BUILRA_DELETED_FUNCTION;

    public:
      ExecutionQueueDelegate() {}
      virtual ~ExecutionQueueDelegate();

      /// Called when a command's job has been started.
      ///
      /// The queue guarantees that any jobStarted() call will be paired with
      /// exactly one \see jobFinished() call.
      //
      // FIXME: We may eventually want to allow the individual job to provide
      // some additional context here, for complex commands.
      //
      // FIXME: Design a way to communicate the "lane" here, for use in "super
      // console" like UIs.
      virtual void queueJobStarted(JobDescriptor*) = 0;

      /// Called when a command's job has been finished.
      ///
      /// NOTE: This callback is invoked by the queue without any understanding
      /// of how the command is tied to the engine. In particular, it is almost
      /// always the case that the command will have already completed from the
      /// perspective of the low-level engine (and its dependents may have
      /// started executing). Clients which want to understand when a command is
      /// complete before the engine has been notified as such should use \see
      /// BuildSystem::commandFinished().
      virtual void queueJobFinished(JobDescriptor*) = 0;
    };

    // MARK: Lane Based Execution Queue

    enum class SchedulerAlgorithm {
      /// Name priority queue based scheduling [default]
      NamePriority = 0,

      /// First in, first out
      FIFO = 1
    };

    /// Create an execution queue that schedules jobs to individual lanes with a
    /// capped limit on the number of concurrent lanes.
    ExecutionQueue* createLaneBasedExecutionQueue(
        ExecutionQueueDelegate& delegate, int numLanes, SchedulerAlgorithm alg,
        QualityOfService qos, const char* const* environment);

    /// Create an execution queue that executes all tasks serially on a single
    /// thread.
    std::unique_ptr<ExecutionQueue> createSerialQueue(
        ExecutionQueueDelegate& delegate, const char* const* environment);
  }
}

#endif
