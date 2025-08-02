//===- MemAlloc.h - Memory allocation functions -----------------*- C++ -*-===//
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
/// \file
///
/// This file defines counterparts of C library allocation functions defined in
/// the namespace 'std'. The new allocation functions crash on allocation
/// failure instead of returning null pointer.
///
//===----------------------------------------------------------------------===//

#ifndef TOOLCHAIN_SUPPORT_MEMALLOC_H
#define TOOLCHAIN_SUPPORT_MEMALLOC_H

#include "toolchain/Support/Compiler.h"
#include "toolchain/Support/ErrorHandling.h"
#include <cstdlib>

namespace toolchain {

TOOLCHAIN_ATTRIBUTE_RETURNS_NONNULL inline void *safe_malloc(size_t Sz) {
  void *Result = std::malloc(Sz);
  if (Result == nullptr)
    report_bad_alloc_error("Allocation failed");
  return Result;
}

TOOLCHAIN_ATTRIBUTE_RETURNS_NONNULL inline void *safe_calloc(size_t Count,
                                                        size_t Sz) {
  void *Result = std::calloc(Count, Sz);
  if (Result == nullptr)
    report_bad_alloc_error("Allocation failed");
  return Result;
}

TOOLCHAIN_ATTRIBUTE_RETURNS_NONNULL inline void *safe_realloc(void *Ptr, size_t Sz) {
  void *Result = std::realloc(Ptr, Sz);
  if (Result == nullptr)
    report_bad_alloc_error("Allocation failed");
  return Result;
}

}
#endif
