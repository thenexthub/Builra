//===-- Valgrind.cpp - Implement Valgrind communication ---------*- C++ -*-===//
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
//  Defines Valgrind communication methods, if HAVE_VALGRIND_VALGRIND_H is
//  defined.  If we have valgrind.h but valgrind isn't running, its macros are
//  no-ops.
//
//===----------------------------------------------------------------------===//

#include "toolchain/Support/Valgrind.h"
#include "toolchain/Config/config.h"
#include <cstddef>

#if HAVE_VALGRIND_VALGRIND_H
#include <valgrind/valgrind.h>

static bool InitNotUnderValgrind() {
  return !RUNNING_ON_VALGRIND;
}

// This bool is negated from what we'd expect because code may run before it
// gets initialized.  If that happens, it will appear to be 0 (false), and we
// want that to cause the rest of the code in this file to run the
// Valgrind-provided macros.
static const bool NotUnderValgrind = InitNotUnderValgrind();

bool toolchain::sys::RunningOnValgrind() {
  if (NotUnderValgrind)
    return false;
  return RUNNING_ON_VALGRIND;
}

void toolchain::sys::ValgrindDiscardTranslations(const void *Addr, size_t Len) {
  if (NotUnderValgrind)
    return;

  VALGRIND_DISCARD_TRANSLATIONS(Addr, Len);
}

#else  // !HAVE_VALGRIND_VALGRIND_H

bool toolchain::sys::RunningOnValgrind() {
  return false;
}

void toolchain::sys::ValgrindDiscardTranslations(const void *Addr, size_t Len) {
}

#endif  // !HAVE_VALGRIND_VALGRIND_H
