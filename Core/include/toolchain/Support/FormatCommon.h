//===- FormatAdapters.h - Formatters for common TOOLCHAIN types -----*- C++ -*-===//
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

#ifndef TOOLCHAIN_SUPPORT_FORMATCOMMON_H
#define TOOLCHAIN_SUPPORT_FORMATCOMMON_H

#include "toolchain/ADT/SmallString.h"
#include "toolchain/Support/FormatVariadicDetails.h"
#include "toolchain/Support/raw_ostream.h"

namespace toolchain {
enum class AlignStyle { Left, Center, Right };

struct FmtAlign {
  detail::format_adapter &Adapter;
  AlignStyle Where;
  size_t Amount;
  char Fill;

  FmtAlign(detail::format_adapter &Adapter, AlignStyle Where, size_t Amount,
           char Fill = ' ')
      : Adapter(Adapter), Where(Where), Amount(Amount), Fill(Fill) {}

  void format(raw_ostream &S, StringRef Options) {
    // If we don't need to align, we can format straight into the underlying
    // stream.  Otherwise we have to go through an intermediate stream first
    // in order to calculate how long the output is so we can align it.
    // TODO: Make the format method return the number of bytes written, that
    // way we can also skip the intermediate stream for left-aligned output.
    if (Amount == 0) {
      Adapter.format(S, Options);
      return;
    }
    SmallString<64> Item;
    raw_svector_ostream Stream(Item);

    Adapter.format(Stream, Options);
    if (Amount <= Item.size()) {
      S << Item;
      return;
    }

    size_t PadAmount = Amount - Item.size();
    switch (Where) {
    case AlignStyle::Left:
      S << Item;
      fill(S, PadAmount);
      break;
    case AlignStyle::Center: {
      size_t X = PadAmount / 2;
      fill(S, X);
      S << Item;
      fill(S, PadAmount - X);
      break;
    }
    default:
      fill(S, PadAmount);
      S << Item;
      break;
    }
  }

private:
  void fill(toolchain::raw_ostream &S, uint32_t Count) {
    for (uint32_t I = 0; I < Count; ++I)
      S << Fill;
  }
};
}

#endif
