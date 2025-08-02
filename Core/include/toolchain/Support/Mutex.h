//===- toolchain/Support/Mutex.h - Mutex Operating System Concept -----*- C++ -*-===//
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
// This file declares the toolchain::sys::Mutex class.
//
//===----------------------------------------------------------------------===//

#ifndef TOOLCHAIN_SUPPORT_MUTEX_H
#define TOOLCHAIN_SUPPORT_MUTEX_H

#include "toolchain/Config/toolchain-config.h"
#include "toolchain/Support/Compiler.h"
#include "toolchain/Support/Threading.h"
#include <cassert>

namespace toolchain
{
  namespace sys
  {
    /// Platform agnostic Mutex class.
    class MutexImpl
    {
    /// @name Constructors
    /// @{
    public:

      /// Initializes the lock but doesn't acquire it. if \p recursive is set
      /// to false, the lock will not be recursive which makes it cheaper but
      /// also more likely to deadlock (same thread can't acquire more than
      /// once).
      /// Default Constructor.
      explicit MutexImpl(bool recursive = true);

      /// Releases and removes the lock
      /// Destructor
      ~MutexImpl();

    /// @}
    /// @name Methods
    /// @{
    public:

      /// Attempts to unconditionally acquire the lock. If the lock is held by
      /// another thread, this method will wait until it can acquire the lock.
      /// @returns false if any kind of error occurs, true otherwise.
      /// Unconditionally acquire the lock.
      bool acquire();

      /// Attempts to release the lock. If the lock is held by the current
      /// thread, the lock is released allowing other threads to acquire the
      /// lock.
      /// @returns false if any kind of error occurs, true otherwise.
      /// Unconditionally release the lock.
      bool release();

      /// Attempts to acquire the lock without blocking. If the lock is not
      /// available, this function returns false quickly (without blocking). If
      /// the lock is available, it is acquired.
      /// @returns false if any kind of error occurs or the lock is not
      /// available, true otherwise.
      /// Try to acquire the lock.
      bool tryacquire();

    //@}
    /// @name Platform Dependent Data
    /// @{
    private:
#if defined(TOOLCHAIN_ENABLE_THREADS) && TOOLCHAIN_ENABLE_THREADS != 0
      void* data_; ///< We don't know what the data will be
#endif

    /// @}
    /// @name Do Not Implement
    /// @{
    private:
      MutexImpl(const MutexImpl &) = delete;
      void operator=(const MutexImpl &) = delete;
    /// @}
    };


    /// SmartMutex - A mutex with a compile time constant parameter that
    /// indicates whether this mutex should become a no-op when we're not
    /// running in multithreaded mode.
    template<bool mt_only>
    class SmartMutex {
      MutexImpl impl;
      unsigned acquired;
      bool recursive;
    public:
      explicit SmartMutex(bool rec = true) :
        impl(rec), acquired(0), recursive(rec) { }

      bool lock() {
        if (!mt_only || llvm_is_multithreaded()) {
          return impl.acquire();
        } else {
          // Single-threaded debugging code.  This would be racy in
          // multithreaded mode, but provides not sanity checks in single
          // threaded mode.
          assert((recursive || acquired == 0) && "Lock already acquired!!");
          ++acquired;
          return true;
        }
      }

      bool unlock() {
        if (!mt_only || llvm_is_multithreaded()) {
          return impl.release();
        } else {
          // Single-threaded debugging code.  This would be racy in
          // multithreaded mode, but provides not sanity checks in single
          // threaded mode.
          assert(((recursive && acquired) || (acquired == 1)) &&
                 "Lock not acquired before release!");
          --acquired;
          return true;
        }
      }

      bool try_lock() {
        if (!mt_only || llvm_is_multithreaded())
          return impl.tryacquire();
        else return true;
      }

      private:
        SmartMutex(const SmartMutex<mt_only> & original);
        void operator=(const SmartMutex<mt_only> &);
    };

    /// Mutex - A standard, always enforced mutex.
    typedef SmartMutex<false> Mutex;

    template<bool mt_only>
    class SmartScopedLock  {
      SmartMutex<mt_only>& mtx;

    public:
      SmartScopedLock(SmartMutex<mt_only>& m) : mtx(m) {
        mtx.lock();
      }

      ~SmartScopedLock() {
        mtx.unlock();
      }
    };

    typedef SmartScopedLock<false> ScopedLock;
  }
}

#endif
