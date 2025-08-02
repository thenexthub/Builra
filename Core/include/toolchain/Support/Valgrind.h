//===- toolchain/Support/Valgrind.h - Communication with Valgrind ----*- C++ -*-===//
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
// Methods for communicating with a valgrind instance this program is running
// under.  These are all no-ops unless TOOLCHAIN was configured on a system with the
// valgrind headers installed and valgrind is controlling this process.
//
//===----------------------------------------------------------------------===//

#ifndef TOOLCHAIN_SUPPORT_VALGRIND_H
#define TOOLCHAIN_SUPPORT_VALGRIND_H

#include <cstddef>

namespace toolchain {
namespace sys {
  // True if Valgrind is controlling this process.
  bool RunningOnValgrind();

  // Discard valgrind's translation of code in the range [Addr .. Addr + Len).
  // Otherwise valgrind may continue to execute the old version of the code.
  void ValgrindDiscardTranslations(const void *Addr, size_t Len);
} // namespace sys
} // end namespace toolchain

#endif // TOOLCHAIN_SUPPORT_VALGRIND_H
