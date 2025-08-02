//===-- toolchain/ADT/APSInt.cpp - Arbitrary Precision Signed Int ---*- C++ -*--===//
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
// This file implements the APSInt class, which is a simple class that
// represents an arbitrary sized integer that knows its signedness.
//
//===----------------------------------------------------------------------===//

#include "toolchain/ADT/APSInt.h"
#include "toolchain/ADT/FoldingSet.h"
#include "toolchain/ADT/StringRef.h"

using namespace toolchain;

APSInt::APSInt(StringRef Str) {
  assert(!Str.empty() && "Invalid string length");

  // (Over-)estimate the required number of bits.
  unsigned NumBits = ((Str.size() * 64) / 19) + 2;
  APInt Tmp(NumBits, Str, /*Radix=*/10);
  if (Str[0] == '-') {
    unsigned MinBits = Tmp.getMinSignedBits();
    if (MinBits > 0 && MinBits < NumBits)
      Tmp = Tmp.trunc(MinBits);
    *this = APSInt(Tmp, /*IsUnsigned=*/false);
    return;
  }
  unsigned ActiveBits = Tmp.getActiveBits();
  if (ActiveBits > 0 && ActiveBits < NumBits)
    Tmp = Tmp.trunc(ActiveBits);
  *this = APSInt(Tmp, /*IsUnsigned=*/true);
}

void APSInt::Profile(FoldingSetNodeID& ID) const {
  ID.AddInteger((unsigned) (IsUnsigned ? 1 : 0));
  APInt::Profile(ID);
}
