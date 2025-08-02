//===-- StringSaver.cpp ---------------------------------------------------===//
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

#include "toolchain/Support/StringSaver.h"

using namespace toolchain;

StringRef StringSaver::save(StringRef S) {
  char *P = Alloc.Allocate<char>(S.size() + 1);
  memcpy(P, S.data(), S.size());
  P[S.size()] = '\0';
  return StringRef(P, S.size());
}

StringRef UniqueStringSaver::save(StringRef S) {
  auto R = Unique.insert(S);
  if (R.second)                 // cache miss, need to actually save the string
    *R.first = Strings.save(S); // safe replacement with equal value
  return *R.first;
}
