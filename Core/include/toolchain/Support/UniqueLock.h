//===- Support/UniqueLock.h - Acquire/Release Mutex In Scope ----*- C++ -*-===//
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

#ifndef TOOLCHAIN_SUPPORT_UNIQUE_LOCK_H
#define TOOLCHAIN_SUPPORT_UNIQUE_LOCK_H

#include <cassert>

namespace toolchain {

  /// A pared-down imitation of std::unique_lock from C++11. Contrary to the
  /// name, it's really more of a wrapper for a lock. It may or may not have
  /// an associated mutex, which is guaranteed to be locked upon creation
  /// and unlocked after destruction. unique_lock can also unlock the mutex
  /// and re-lock it freely during its lifetime.
  /// Guard a section of code with a mutex.
  template<typename MutexT>
  class unique_lock {
    MutexT *M = nullptr;
    bool locked = false;

  public:
    unique_lock() = default;
    explicit unique_lock(MutexT &m) : M(&m), locked(true) { M->lock(); }
    unique_lock(const unique_lock &) = delete;
     unique_lock &operator=(const unique_lock &) = delete;

    void operator=(unique_lock &&o) {
      if (owns_lock())
        M->unlock();
      M = o.M;
      locked = o.locked;
      o.M = nullptr;
      o.locked = false;
    }

    ~unique_lock() { if (owns_lock()) M->unlock(); }

    void lock() {
      assert(!locked && "mutex already locked!");
      assert(M && "no associated mutex!");
      M->lock();
      locked = true;
    }

    void unlock() {
      assert(locked && "unlocking a mutex that isn't locked!");
      assert(M && "no associated mutex!");
      M->unlock();
      locked = false;
    }

    bool owns_lock() { return locked; }
  };

} // end namespace toolchain

#endif // TOOLCHAIN_SUPPORT_UNIQUE_LOCK_H
