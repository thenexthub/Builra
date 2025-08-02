//===-- toolchain/Support/ManagedStatic.h - Static Global wrapper ----*- C++ -*-===//
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
// This file defines the ManagedStatic class and the llvm_shutdown() function.
//
//===----------------------------------------------------------------------===//

#ifndef TOOLCHAIN_SUPPORT_MANAGEDSTATIC_H
#define TOOLCHAIN_SUPPORT_MANAGEDSTATIC_H

#include <atomic>
#include <cstddef>

namespace toolchain {

/// object_creator - Helper method for ManagedStatic.
template <class C> struct object_creator {
  static void *call() { return new C(); }
};

/// object_deleter - Helper method for ManagedStatic.
///
template <typename T> struct object_deleter {
  static void call(void *Ptr) { delete (T *)Ptr; }
};
template <typename T, size_t N> struct object_deleter<T[N]> {
  static void call(void *Ptr) { delete[](T *)Ptr; }
};

/// ManagedStaticBase - Common base class for ManagedStatic instances.
class ManagedStaticBase {
protected:
  // This should only be used as a static variable, which guarantees that this
  // will be zero initialized.
  mutable std::atomic<void *> Ptr;
  mutable void (*DeleterFn)(void*);
  mutable const ManagedStaticBase *Next;

  void RegisterManagedStatic(void *(*creator)(), void (*deleter)(void*)) const;

public:
  /// isConstructed - Return true if this object has not been created yet.
  bool isConstructed() const { return Ptr != nullptr; }

  void destroy() const;
};

/// ManagedStatic - This transparently changes the behavior of global statics to
/// be lazily constructed on demand (good for reducing startup times of dynamic
/// libraries that link in TOOLCHAIN components) and for making destruction be
/// explicit through the llvm_shutdown() function call.
///
template <class C, class Creator = object_creator<C>,
          class Deleter = object_deleter<C>>
class ManagedStatic : public ManagedStaticBase {
public:
  // Accessors.
  C &operator*() {
    void *Tmp = Ptr.load(std::memory_order_acquire);
    if (!Tmp)
      RegisterManagedStatic(Creator::call, Deleter::call);

    return *static_cast<C *>(Ptr.load(std::memory_order_relaxed));
  }

  C *operator->() { return &**this; }

  const C &operator*() const {
    void *Tmp = Ptr.load(std::memory_order_acquire);
    if (!Tmp)
      RegisterManagedStatic(Creator::call, Deleter::call);

    return *static_cast<C *>(Ptr.load(std::memory_order_relaxed));
  }

  const C *operator->() const { return &**this; }
};

/// llvm_shutdown - Deallocate and destroy all ManagedStatic variables.
void llvm_shutdown();

/// llvm_shutdown_obj - This is a simple helper class that calls
/// llvm_shutdown() when it is destroyed.
struct llvm_shutdown_obj {
  llvm_shutdown_obj() = default;
  ~llvm_shutdown_obj() { llvm_shutdown(); }
};

} // end namespace toolchain

#endif // TOOLCHAIN_SUPPORT_MANAGEDSTATIC_H
