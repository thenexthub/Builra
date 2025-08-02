//===- Defer.h --------------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA_BASIC_DEFER_H
#define BUILRA_BASIC_DEFER_H

#include <functional>
#include <utility>
#include <type_traits>

namespace builra {
namespace basic {

template <typename T>
class ScopeDefer {
  T deferredWork;
  void operator=(ScopeDefer&) = delete;
public:
  ScopeDefer(T&& work) : deferredWork(std::move(work)) { }
  ~ScopeDefer() { deferredWork(); }
};

template <typename T>
ScopeDefer<T> makeScopeDefer(T&& work) {
  return ScopeDefer<typename std::decay<T>::type>(std::forward<T>(work));
}

namespace impl {
  struct ScopeDeferTask {};
  template<typename T>
  ScopeDefer<typename std::decay<T>::type> operator+(ScopeDeferTask, T&& work) {
    return ScopeDefer<typename std::decay<T>::type>(std::forward<T>(work));
  }
}

}
}

// These generate a unique variable name for each use of defer in the
// translation unit.
#define DEFER_VAR_NAME(C) _defer_##C
#define DEFER_INTERMEDIATE(C) DEFER_VAR_NAME(C)
#define DEFER_UNIQUE_VAR_NAME DEFER_INTERMEDIATE(__COUNTER__)

/// Runs the following function/lambda body when the current scope exits.
/// Typical use looks like:
///
///   builra_defer {
///     deferred work
///   };
///
#define builra_defer \
  auto DEFER_UNIQUE_VAR_NAME = builra::basic::impl::ScopeDeferTask() + [&]()

#endif
