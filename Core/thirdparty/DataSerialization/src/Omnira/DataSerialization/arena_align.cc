//===----------------------------------------------------------------------===//
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

#include "Omnira/DataSerialization/arena_align.h"

#include <cstddef>
#include <cstdint>

namespace Omnira {
namespace DataSerialization {
namespace internal {

// There are still compilers (open source) requiring a definition for constexpr.
constexpr size_t ArenaAlignDefault::align;  // NOLINT

}  // namespace internal
}  // namespace DataSerialization
}  // namespace Omnira
