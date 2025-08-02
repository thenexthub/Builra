//===- Mutex.cpp - Mutual Exclusion Lock ------------------------*- C++ -*-===//
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
// This file implements the toolchain::sys::Mutex class.
//
//===----------------------------------------------------------------------===//

#include "toolchain/Support/Mutex.h"
#include "toolchain/Config/config.h"
#include "toolchain/Support/ErrorHandling.h"

//===----------------------------------------------------------------------===//
//=== WARNING: Implementation here must contain only TRULY operating system
//===          independent code.
//===----------------------------------------------------------------------===//

#if !defined(TOOLCHAIN_ENABLE_THREADS) || TOOLCHAIN_ENABLE_THREADS == 0
// Define all methods as no-ops if threading is explicitly disabled
namespace toolchain {
using namespace sys;
MutexImpl::MutexImpl( bool recursive) { }
MutexImpl::~MutexImpl() { }
bool MutexImpl::acquire() { return true; }
bool MutexImpl::release() { return true; }
bool MutexImpl::tryacquire() { return true; }
}
#else

#if defined(HAVE_PTHREAD_H) && defined(HAVE_PTHREAD_MUTEX_LOCK)

#include <cassert>
#include <pthread.h>
#include <stdlib.h>

namespace toolchain {
using namespace sys;

// Construct a Mutex using pthread calls
MutexImpl::MutexImpl( bool recursive)
  : data_(nullptr)
{
  // Declare the pthread_mutex data structures
  pthread_mutex_t* mutex =
    static_cast<pthread_mutex_t*>(safe_malloc(sizeof(pthread_mutex_t)));

  pthread_mutexattr_t attr;

  // Initialize the mutex attributes
  int errorcode = pthread_mutexattr_init(&attr);
  assert(errorcode == 0); (void)errorcode;

  // Initialize the mutex as a recursive mutex, if requested, or normal
  // otherwise.
  int kind = ( recursive  ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_NORMAL );
  errorcode = pthread_mutexattr_settype(&attr, kind);
  assert(errorcode == 0); (void)errorcode;

  // Initialize the mutex
  errorcode = pthread_mutex_init(mutex, &attr);
  assert(errorcode == 0); (void)errorcode;

  // Destroy the attributes
  errorcode = pthread_mutexattr_destroy(&attr);
  assert(errorcode == 0); (void)errorcode;

  // Assign the data member
  data_ = mutex;
}

// Destruct a Mutex
MutexImpl::~MutexImpl()
{
  pthread_mutex_t* mutex = static_cast<pthread_mutex_t*>(data_);
  assert(mutex != nullptr);
  pthread_mutex_destroy(mutex);
  free(mutex);
}

bool
MutexImpl::acquire()
{
  pthread_mutex_t* mutex = static_cast<pthread_mutex_t*>(data_);
  assert(mutex != nullptr);

  int errorcode = pthread_mutex_lock(mutex);
  return errorcode == 0;
}

bool
MutexImpl::release()
{
  pthread_mutex_t* mutex = static_cast<pthread_mutex_t*>(data_);
  assert(mutex != nullptr);

  int errorcode = pthread_mutex_unlock(mutex);
  return errorcode == 0;
}

bool
MutexImpl::tryacquire()
{
  pthread_mutex_t* mutex = static_cast<pthread_mutex_t*>(data_);
  assert(mutex != nullptr);

  int errorcode = pthread_mutex_trylock(mutex);
  return errorcode == 0;
}

}

#elif defined(TOOLCHAIN_ON_UNIX)
#include "Unix/Mutex.inc"
#elif defined( _WIN32)
#include "Windows/Mutex.inc"
#else
#warning Neither TOOLCHAIN_ON_UNIX nor _WIN32 was set in Support/Mutex.cpp
#endif
#endif
