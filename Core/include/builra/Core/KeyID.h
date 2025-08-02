//===- KeyID.h -----------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA_CORE_KEYID_H
#define BUILRA_CORE_KEYID_H

#include "builra/Basic/Compiler.h"

#include "toolchain/ADT/DenseMapInfo.h"
#include "toolchain/ADT/StringRef.h"

namespace builra {
namespace core {

/// The scalar identifier has the following properties:
///  1. Has to be able to store a pointer value.
///  2. Can be used to identify something, as well as point to something.
struct KeyID {
  typedef uint64_t ValueTy;

private:
  ValueTy _value;

  // Some values at the top of the pointer range are reserved.
  static constexpr ValueTy MaxValidID = (~(uint64_t)0) << 4;

public:

  /// Get the identifying part of the key (without the flags).
  uint64_t value() const {
    return _value;
  }

  /// Implicit conversion to the identifying part of the key.
  operator uint64_t() const {
    return value();
  }

  /// Default initialization to a completely bogus value.
  constexpr KeyID() : _value(0) { }

  /// Store a pointer value.
  explicit KeyID(const void * value_ptr) : _value((uintptr_t)value_ptr) {
    // Some values at the top of the pointer range are reserved.
    assert(_value < MaxValidID);
  }

  /// Check if two KeyIDs are equivalent.
  constexpr bool operator==(const KeyID &other) const {
    return (_value == other._value);
  }

  /// Used for tests to support comparisons with explicit number literals.
  constexpr bool operator ==(int rhs) const {
    return rhs >= 0 && value() == (uint64_t)rhs;
  }

  /// Representation that doesn't denote a user-supplied value.
  static constexpr KeyID novalue() { return KeyID(); }

  /// Representation that denotes an explicitly invalid value.
  static constexpr KeyID invalid() {
    KeyID k;
    k._value = ~0UL;
    return k;
  }

};

}
}

namespace std {
  template <> struct hash<builra::core::KeyID> {
    std::size_t operator()(const builra::core::KeyID & k) const {
      return std::hash<uint64_t>()(k.value());
    }
  };
}

// Provide DenseMapInfo for KeyIDs.
namespace toolchain {
  using namespace builra::core;
  template<> struct DenseMapInfo<builra::core::KeyID> {
    static inline KeyID getEmptyKey() { return KeyID::novalue(); }
    static inline KeyID getTombstoneKey() { return KeyID::invalid(); }
    static unsigned getHashValue(const KeyID& Val) {
      return (unsigned)(Val.value() * 37UL);
    }
    static bool isEqual(const KeyID& LHS, const KeyID& RHS) {
      return LHS == RHS;
    }
  };
}

#endif
