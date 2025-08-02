//===- Subtask.h ------------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA3_SUBTASK_H
#define BUILRA3_SUBTASK_H

#include <any>
#include <functional>
#include <unordered_map>
#include <variant>

#include <builra3/Result.hpp>

#include "builra3/Error.pb.h"

namespace builra3 {

class CASDatabase;

class SubtaskInterface {
  friend class ExtTaskInterface;
private:
  void* impl;
  uint64_t ctx;

public:
  SubtaskInterface(void* impl, uint64_t ctx) : impl(impl), ctx(ctx) {}

  std::shared_ptr<CASDatabase> cas();
};

typedef std::function<result<std::any, Error> (SubtaskInterface)> SyncSubtask;

typedef std::function<void(result<std::any, Error>)> SubtaskCallback;
typedef std::function<void (SubtaskInterface, SubtaskCallback)> AsyncSubtask;

typedef std::variant<SyncSubtask, AsyncSubtask> Subtask;

typedef result<std::any, Error> SubtaskResult;

typedef std::unordered_map<uint64_t, SubtaskResult> SubtaskResults;

}

#endif
