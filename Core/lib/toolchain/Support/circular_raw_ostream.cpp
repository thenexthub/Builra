//===- circular_raw_ostream.cpp - Implement circular_raw_ostream ----------===//
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
// This implements support for circular buffered streams.
//
//===----------------------------------------------------------------------===//

#include "toolchain/Support/circular_raw_ostream.h"
#include <algorithm>
using namespace toolchain;

void circular_raw_ostream::write_impl(const char *Ptr, size_t Size) {
  if (BufferSize == 0) {
    TheStream->write(Ptr, Size);
    return;
  }

  // Write into the buffer, wrapping if necessary.
  while (Size != 0) {
    unsigned Bytes =
      std::min(unsigned(Size), unsigned(BufferSize - (Cur - BufferArray)));
    memcpy(Cur, Ptr, Bytes);
    Size -= Bytes;
    Cur += Bytes;
    if (Cur == BufferArray + BufferSize) {
      // Reset the output pointer to the start of the buffer.
      Cur = BufferArray;
      Filled = true;
    }
  }
}

void circular_raw_ostream::flushBufferWithBanner() {
  if (BufferSize != 0) {
    // Write out the buffer
    TheStream->write(Banner, std::strlen(Banner));
    flushBuffer();
  }
}
