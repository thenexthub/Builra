//===- BuildSystemHandlers.h ------------------------------------*- C++ -*-===//
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

#ifndef BUILRA_BUILDSYSTEM_BUILDSYSTEMHANDLERS_H
#define BUILRA_BUILDSYSTEM_BUILDSYSTEMHANDLERS_H

#include "builra/Basic/Subprocess.h"

#include <memory>

namespace builra {
namespace basic {
  class QueueJobContext;
}
namespace core {
  class TaskInterface;
}

namespace buildsystem {

class ShellCommand;

class HandlerState {
public:
  explicit HandlerState() {}
  virtual ~HandlerState();
};
  
class ShellCommandHandler {
public:
  explicit ShellCommandHandler() {}
  virtual ~ShellCommandHandler();
  
  virtual std::unique_ptr<HandlerState>
  start(core::TaskInterface, ShellCommand* command) const = 0;

  virtual void
  execute(HandlerState*, ShellCommand* command, core::TaskInterface ti,
          basic::QueueJobContext* context, basic::ProcessCompletionFn) const = 0;
};

}
}

#endif
