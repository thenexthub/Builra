//===-- ExecutionQueue.cpp ------------------------------------------------===//
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

#include "builra/Basic/ExecutionQueue.h"

#include "toolchain/ADT/ArrayRef.h"
#include "toolchain/ADT/SmallString.h"
#include "toolchain/ADT/StringRef.h"

#include <future>
#include <vector>

using namespace builra;
using namespace builra::basic;


JobDescriptor::~JobDescriptor() {
}

QueueJobContext::~QueueJobContext() {
}

ExecutionQueue::ExecutionQueue(ExecutionQueueDelegate& delegate)
  : delegate(delegate)
{
}

ExecutionQueue::~ExecutionQueue() {
}

ProcessStatus ExecutionQueue::executeProcess(QueueJobContext* context,
                                             ArrayRef<StringRef> commandLine) {
  std::promise<ProcessStatus> p;
  auto result = p.get_future();
  executeProcess(context, commandLine, {}, {true},
                 {[&p](ProcessResult result) mutable {
    p.set_value(result.status);
  }});
  return result.get();
}

bool ExecutionQueue::executeShellCommand(QueueJobContext* context,
                                         StringRef command) {
  SmallString<1024> commandStorage(command);
  std::vector<StringRef> commandLine(
#if defined(_WIN32)
      {"C:\\windows\\system32\\cmd.exe", "/C", commandStorage.c_str()});
#else
      {DefaultShellPath, "-c", commandStorage.c_str()});
#endif
  return executeProcess(context, commandLine) == ProcessStatus::Succeeded;
}

ExecutionQueueDelegate::~ExecutionQueueDelegate() {
}


