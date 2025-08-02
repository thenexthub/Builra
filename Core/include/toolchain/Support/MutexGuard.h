//===-- Support/MutexGuard.h - Acquire/Release Mutex In Scope ---*- C++ -*-===//
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
//
// This file defines a guard for a block of code that ensures a Mutex is locked
// upon construction and released upon destruction.
//
//===----------------------------------------------------------------------===//

#ifndef TOOLCHAIN_SUPPORT_MUTEXGUARD_H
#define TOOLCHAIN_SUPPORT_MUTEXGUARD_H

#include "toolchain/Support/Mutex.h"

namespace toolchain {
  /// Instances of this class acquire a given Mutex Lock when constructed and
  /// hold that lock until destruction. The intention is to instantiate one of
  /// these on the stack at the top of some scope to be assured that C++
  /// destruction of the object will always release the Mutex and thus avoid
  /// a host of nasty multi-threading problems in the face of exceptions, etc.
  /// Guard a section of code with a Mutex.
  class MutexGuard {
    sys::Mutex &M;
    MutexGuard(const MutexGuard &) = delete;
    void operator=(const MutexGuard &) = delete;
  public:
    MutexGuard(sys::Mutex &m) : M(m) { M.lock(); }
    ~MutexGuard() { M.unlock(); }
    /// holds - Returns true if this locker instance holds the specified lock.
    /// This is mostly used in assertions to validate that the correct mutex
    /// is held.
    bool holds(const sys::Mutex& lock) const { return &M == &lock; }
  };
}

#endif // TOOLCHAIN_SUPPORT_MUTEXGUARD_H
