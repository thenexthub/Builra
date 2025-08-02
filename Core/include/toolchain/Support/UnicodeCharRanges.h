//===--- UnicodeCharRanges.h - Types and functions for character ranges ---===//
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
#ifndef TOOLCHAIN_SUPPORT_UNICODECHARRANGES_H
#define TOOLCHAIN_SUPPORT_UNICODECHARRANGES_H

#include "toolchain/ADT/ArrayRef.h"
#include "toolchain/ADT/SmallPtrSet.h"
#include "toolchain/Support/Compiler.h"
#include "toolchain/Support/Debug.h"
#include "toolchain/Support/Mutex.h"
#include "toolchain/Support/MutexGuard.h"
#include "toolchain/Support/raw_ostream.h"
#include <algorithm>

#define DEBUG_TYPE "unicode"

namespace toolchain {
namespace sys {

/// Represents a closed range of Unicode code points [Lower, Upper].
struct UnicodeCharRange {
  uint32_t Lower;
  uint32_t Upper;
};

inline bool operator<(uint32_t Value, UnicodeCharRange Range) {
  return Value < Range.Lower;
}
inline bool operator<(UnicodeCharRange Range, uint32_t Value) {
  return Range.Upper < Value;
}

/// Holds a reference to an ordered array of UnicodeCharRange and allows
/// to quickly check if a code point is contained in the set represented by this
/// array.
class UnicodeCharSet {
public:
  typedef ArrayRef<UnicodeCharRange> CharRanges;

  /// Constructs a UnicodeCharSet instance from an array of
  /// UnicodeCharRanges.
  ///
  /// Array pointed by \p Ranges should have the lifetime at least as long as
  /// the UnicodeCharSet instance, and should not change. Array is validated by
  /// the constructor, so it makes sense to create as few UnicodeCharSet
  /// instances per each array of ranges, as possible.
#ifdef NDEBUG

  // FIXME: This could use constexpr + static_assert. This way we
  // may get rid of NDEBUG in this header. Unfortunately there are some
  // problems to get this working with MSVC 2013. Change this when
  // the support for MSVC 2013 is dropped.
  constexpr UnicodeCharSet(CharRanges Ranges) : Ranges(Ranges) {}
#else
  UnicodeCharSet(CharRanges Ranges) : Ranges(Ranges) {
    assert(rangesAreValid());
  }
#endif

  /// Returns true if the character set contains the Unicode code point
  /// \p C.
  bool contains(uint32_t C) const {
    return std::binary_search(Ranges.begin(), Ranges.end(), C);
  }

private:
  /// Returns true if each of the ranges is a proper closed range
  /// [min, max], and if the ranges themselves are ordered and non-overlapping.
  bool rangesAreValid() const {
    uint32_t Prev = 0;
    for (CharRanges::const_iterator I = Ranges.begin(), E = Ranges.end();
         I != E; ++I) {
      if (I != Ranges.begin() && Prev >= I->Lower) {
        TOOLCHAIN_DEBUG(dbgs() << "Upper bound 0x");
        TOOLCHAIN_DEBUG(dbgs().write_hex(Prev));
        TOOLCHAIN_DEBUG(dbgs() << " should be less than succeeding lower bound 0x");
        TOOLCHAIN_DEBUG(dbgs().write_hex(I->Lower) << "\n");
        return false;
      }
      if (I->Upper < I->Lower) {
        TOOLCHAIN_DEBUG(dbgs() << "Upper bound 0x");
        TOOLCHAIN_DEBUG(dbgs().write_hex(I->Lower));
        TOOLCHAIN_DEBUG(dbgs() << " should not be less than lower bound 0x");
        TOOLCHAIN_DEBUG(dbgs().write_hex(I->Upper) << "\n");
        return false;
      }
      Prev = I->Upper;
    }

    return true;
  }

  const CharRanges Ranges;
};

} // namespace sys
} // namespace toolchain

#undef DEBUG_TYPE // "unicode"

#endif // TOOLCHAIN_SUPPORT_UNICODECHARRANGES_H
