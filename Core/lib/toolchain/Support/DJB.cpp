//===-- Support/DJB.cpp ---DJB Hash -----------------------------*- C++ -*-===//
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

#include "toolchain/Support/DJB.h"
#include "toolchain/ADT/ArrayRef.h"
#include "toolchain/Support/Compiler.h"
#include "toolchain/Support/ConvertUTF.h"
#include "toolchain/Support/Unicode.h"

using namespace toolchain;

static UTF32 chopOneUTF32(StringRef &Buffer) {
  UTF32 C;
  const UTF8 *const Begin8Const =
      reinterpret_cast<const UTF8 *>(Buffer.begin());
  const UTF8 *Begin8 = Begin8Const;
  UTF32 *Begin32 = &C;

  // In lenient mode we will always end up with a "reasonable" value in C for
  // non-empty input.
  assert(!Buffer.empty());
  ConvertUTF8toUTF32(&Begin8, reinterpret_cast<const UTF8 *>(Buffer.end()),
                     &Begin32, &C + 1, lenientConversion);
  Buffer = Buffer.drop_front(Begin8 - Begin8Const);
  return C;
}

static StringRef toUTF8(UTF32 C, MutableArrayRef<UTF8> Storage) {
  const UTF32 *Begin32 = &C;
  UTF8 *Begin8 = Storage.begin();

  // The case-folded output should always be a valid unicode character, so use
  // strict mode here.
  ConversionResult CR = ConvertUTF32toUTF8(&Begin32, &C + 1, &Begin8,
                                           Storage.end(), strictConversion);
  assert(CR == conversionOK && "Case folding produced invalid char?");
  (void)CR;
  return StringRef(reinterpret_cast<char *>(Storage.begin()),
                   Begin8 - Storage.begin());
}

static UTF32 foldCharDwarf(UTF32 C) {
  // DWARF v5 addition to the unicode folding rules.
  // Fold "Latin Small Letter Dotless I" and "Latin Capital Letter I With Dot
  // Above" into "i".
  if (C == 0x130 || C == 0x131)
    return 'i';
  return sys::unicode::foldCharSimple(C);
}

static uint32_t caseFoldingDjbHashCharSlow(StringRef &Buffer, uint32_t H) {
  UTF32 C = chopOneUTF32(Buffer);

  C = foldCharDwarf(C);

  std::array<UTF8, UNI_MAX_UTF8_BYTES_PER_CODE_POINT> Storage;
  StringRef Folded = toUTF8(C, Storage);
  return djbHash(Folded, H);
}

uint32_t toolchain::caseFoldingDjbHash(StringRef Buffer, uint32_t H) {
  while (!Buffer.empty()) {
    unsigned char C = Buffer.front();
    if (TOOLCHAIN_LIKELY(C <= 0x7f)) {
      // US-ASCII, encoded as one character in utf-8.
      // This is by far the most common case, so handle this specially.
      if (C >= 'A' && C <= 'Z')
        C = 'a' + (C - 'A'); // fold uppercase into lowercase
      H = (H << 5) + H + C;
      Buffer = Buffer.drop_front();
      continue;
    }
    H = caseFoldingDjbHashCharSlow(Buffer, H);
  }
  return H;
}
