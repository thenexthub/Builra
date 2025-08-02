//===- NativeFormatting.h - Low level formatting helpers ---------*- C++-*-===//
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

#ifndef TOOLCHAIN_SUPPORT_NATIVE_FORMATTING_H
#define TOOLCHAIN_SUPPORT_NATIVE_FORMATTING_H

#include "toolchain/ADT/Optional.h"
#include "toolchain/Support/raw_ostream.h"

#include <cstdint>

namespace toolchain {
enum class FloatStyle { Exponent, ExponentUpper, Fixed, Percent };
enum class IntegerStyle {
  Integer,
  Number,
};
enum class HexPrintStyle { Upper, Lower, PrefixUpper, PrefixLower };

size_t getDefaultPrecision(FloatStyle Style);

bool isPrefixedHexStyle(HexPrintStyle S);

void write_integer(raw_ostream &S, unsigned int N, size_t MinDigits,
                   IntegerStyle Style);
void write_integer(raw_ostream &S, int N, size_t MinDigits, IntegerStyle Style);
void write_integer(raw_ostream &S, unsigned long N, size_t MinDigits,
                   IntegerStyle Style);
void write_integer(raw_ostream &S, long N, size_t MinDigits,
                   IntegerStyle Style);
void write_integer(raw_ostream &S, unsigned long long N, size_t MinDigits,
                   IntegerStyle Style);
void write_integer(raw_ostream &S, long long N, size_t MinDigits,
                   IntegerStyle Style);

void write_hex(raw_ostream &S, uint64_t N, HexPrintStyle Style,
               Optional<size_t> Width = None);
void write_double(raw_ostream &S, double D, FloatStyle Style,
                  Optional<size_t> Precision = None);
}

#endif

