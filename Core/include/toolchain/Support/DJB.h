//===-- toolchain/Support/DJB.h ---DJB Hash --------------------------*- C++ -*-===//
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
// This file contains support for the DJ Bernstein hash function.
//
//===----------------------------------------------------------------------===//

#ifndef TOOLCHAIN_SUPPORT_DJB_H
#define TOOLCHAIN_SUPPORT_DJB_H

#include "toolchain/ADT/StringRef.h"

namespace toolchain {

/// The Bernstein hash function used by the DWARF accelerator tables.
inline uint32_t djbHash(StringRef Buffer, uint32_t H = 5381) {
  for (unsigned char C : Buffer.bytes())
    H = (H << 5) + H + C;
  return H;
}

/// Computes the Bernstein hash after folding the input according to the Dwarf 5
/// standard case folding rules.
uint32_t caseFoldingDjbHash(StringRef Buffer, uint32_t H = 5381);
} // namespace toolchain

#endif // TOOLCHAIN_SUPPORT_DJB_H
