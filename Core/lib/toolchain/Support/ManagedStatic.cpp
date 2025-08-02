//===-- ManagedStatic.cpp - Static Global wrapper -------------------------===//
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
// This file implements the ManagedStatic class and llvm_shutdown().
//
//===----------------------------------------------------------------------===//

#include "toolchain/Support/ManagedStatic.h"
#include "toolchain/Config/config.h"
#include "toolchain/Support/Mutex.h"
#include "toolchain/Support/MutexGuard.h"
#include "toolchain/Support/Threading.h"
#include <cassert>
using namespace toolchain;

static const ManagedStaticBase *StaticList = nullptr;
static sys::Mutex *ManagedStaticMutex = nullptr;
static toolchain::once_flag mutex_init_flag;

static void initializeMutex() {
  ManagedStaticMutex = new sys::Mutex();
}

static sys::Mutex* getManagedStaticMutex() {
  toolchain::call_once(mutex_init_flag, initializeMutex);
  return ManagedStaticMutex;
}

void ManagedStaticBase::RegisterManagedStatic(void *(*Creator)(),
                                              void (*Deleter)(void*)) const {
  assert(Creator);
  if (llvm_is_multithreaded()) {
    MutexGuard Lock(*getManagedStaticMutex());

    if (!Ptr.load(std::memory_order_relaxed)) {
      void *Tmp = Creator();

      Ptr.store(Tmp, std::memory_order_release);
      DeleterFn = Deleter;

      // Add to list of managed statics.
      Next = StaticList;
      StaticList = this;
    }
  } else {
    assert(!Ptr && !DeleterFn && !Next &&
           "Partially initialized ManagedStatic!?");
    Ptr = Creator();
    DeleterFn = Deleter;

    // Add to list of managed statics.
    Next = StaticList;
    StaticList = this;
  }
}

void ManagedStaticBase::destroy() const {
  assert(DeleterFn && "ManagedStatic not initialized correctly!");
  assert(StaticList == this &&
         "Not destroyed in reverse order of construction?");
  // Unlink from list.
  StaticList = Next;
  Next = nullptr;

  // Destroy memory.
  DeleterFn(Ptr);

  // Cleanup.
  Ptr = nullptr;
  DeleterFn = nullptr;
}

/// llvm_shutdown - Deallocate and destroy all ManagedStatic variables.
void toolchain::llvm_shutdown() {
  MutexGuard Lock(*getManagedStaticMutex());

  while (StaticList)
    StaticList->destroy();
}
