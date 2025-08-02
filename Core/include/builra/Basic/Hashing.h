//===- Hashing.h ------------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA_BASIC_HASHING_H
#define BUILRA_BASIC_HASHING_H

#include "builra/Basic/BinaryCoding.h"
#include "builra/Basic/toolchain.h"

#include "toolchain/ADT/Hashing.h"
#include "toolchain/ADT/StringRef.h"

namespace builra {
namespace basic {

uint64_t hashString(StringRef value);

class CommandSignature {
public:
  CommandSignature() = default;
  CommandSignature(StringRef string) {
    value = size_t(toolchain::hash_value(string));
  }
  explicit CommandSignature(uint64_t sig) : value(sig) {}
  CommandSignature(const CommandSignature& other) = default;
  CommandSignature(CommandSignature&& other) = default;
  CommandSignature& operator=(const CommandSignature& other) = default;
  CommandSignature& operator=(CommandSignature&& other) = default;

  bool isNull() const { return value == 0; }

  bool operator==(const CommandSignature& other) const { return value == other.value; }
  bool operator!=(const CommandSignature& other) const { return value != other.value; }

  CommandSignature& combine(StringRef string) {
    // FIXME: Use a more appropriate hashing infrastructure.
    value = toolchain::hash_combine(value, string);
    return *this;
  }

  CommandSignature& combine(const std::string &string) {
    // FIXME: Use a more appropriate hashing infrastructure.
    value = toolchain::hash_combine(value, string);
    return *this;
  }

  CommandSignature& combine(bool b) {
    // FIXME: Use a more appropriate hashing infrastructure.
    value = toolchain::hash_combine(value, b);
    return *this;
  }

  template <typename T>
  CommandSignature& combine(const std::vector<T>& list) {
    for (const auto& v: list) {
      combine(v);
    }
    return *this;
  }

  uint64_t value = 0;
};

template<>
struct BinaryCodingTraits<CommandSignature> {
  static inline void encode(const CommandSignature& value, BinaryEncoder& coder) {
    coder.write(value.value);
  }
  static inline void decode(CommandSignature& value, BinaryDecoder& coder) {
    coder.read(value.value);
  }
};

}
}

#endif
