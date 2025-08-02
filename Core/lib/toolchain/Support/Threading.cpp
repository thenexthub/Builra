//===-- toolchain/Support/Threading.cpp- Control multithreading mode --*- C++ -*-==//
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
// This file defines helper functions for running TOOLCHAIN in a multi-threaded
// environment.
//
//===----------------------------------------------------------------------===//

#include "toolchain/Support/Threading.h"
#include "toolchain/Config/config.h"
#include "toolchain/Support/Host.h"

#include <cassert>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

using namespace toolchain;

//===----------------------------------------------------------------------===//
//=== WARNING: Implementation here must contain only TRULY operating system
//===          independent code.
//===----------------------------------------------------------------------===//

bool toolchain::llvm_is_multithreaded() {
#if TOOLCHAIN_ENABLE_THREADS != 0
  return true;
#else
  return false;
#endif
}

#if TOOLCHAIN_ENABLE_THREADS == 0 ||                                                \
    (!defined(_WIN32) && !defined(HAVE_PTHREAD_H))
// Support for non-Win32, non-pthread implementation.
void toolchain::llvm_execute_on_thread(void (*Fn)(void *), void *UserData,
                                  unsigned RequestedStackSize) {
  (void)RequestedStackSize;
  Fn(UserData);
}

unsigned toolchain::heavyweight_hardware_concurrency() { return 1; }

unsigned toolchain::hardware_concurrency() { return 1; }

uint64_t toolchain::get_threadid() { return 0; }

uint32_t toolchain::get_max_thread_name_length() { return 0; }

void toolchain::set_thread_name(const Twine &Name) {}

void toolchain::get_thread_name(SmallVectorImpl<char> &Name) { Name.clear(); }

#else

#include <thread>
unsigned toolchain::heavyweight_hardware_concurrency() {
  // Since we can't get here unless TOOLCHAIN_ENABLE_THREADS == 1, it is safe to use
  // `std::thread` directly instead of `toolchain::thread` (and indeed, doing so
  // allows us to not define `thread` in the toolchain namespace, which conflicts
  // with some platforms such as FreeBSD whose headers also define a struct
  // called `thread` in the global namespace which can cause ambiguity due to
  // ADL.
  int NumPhysical = sys::getHostNumPhysicalCores();
  if (NumPhysical == -1)
    return std::thread::hardware_concurrency();
  return NumPhysical;
}

unsigned toolchain::hardware_concurrency() {
#if defined(HAVE_SCHED_GETAFFINITY) && defined(HAVE_CPU_COUNT)
  cpu_set_t Set;
  if (sched_getaffinity(0, sizeof(Set), &Set))
    return CPU_COUNT(&Set);
#endif
  // Guard against std::thread::hardware_concurrency() returning 0.
  if (unsigned Val = std::thread::hardware_concurrency())
    return Val;
  return 1;
}

// Include the platform-specific parts of this class.
#ifdef TOOLCHAIN_ON_UNIX
#include "Unix/Threading.inc"
#endif
#ifdef _WIN32
#include "Windows/Threading.inc"
#endif

#endif
