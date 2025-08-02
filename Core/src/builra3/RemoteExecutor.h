//===- RemoteExecutor.h -----------------------------------------*- C++ -*-===//
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

#ifndef BUILRA3_REMOTEEXECUTOR_H
#define BUILRA3_REMOTEEXECUTOR_H

#include <functional>

#include <builra3/Result.hpp>

#include "builra3/ActionExecutor.h"
#include "builra3/CAS.pb.h"
#include "builra3/Common.h"
#include "builra3/Error.pb.h"

namespace builra3 {

typedef uuids::uuid RemoteActionID;

class RemoteExecutor {
public:
  RemoteExecutor() { }
  virtual ~RemoteExecutor() = 0;

  virtual std::string builtinExecutable() const = 0;

  virtual void prepare(std::string execPath,
                       std::function<void(result<CASID, Error>)> res) = 0;

  virtual void execute(
    const CASID& functionID,
    const Action& action,
    std::function<void(result<RemoteActionID, Error>)> dispatchedFn,
    std::function<void(result<ActionResult, Error>)> resultFn
  ) = 0;
};

}

#endif
