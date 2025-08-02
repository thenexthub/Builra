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

#include "Omnira/DataSerialization/raw_ptr.h"

#include "absl/base/attributes.h"
#include "absl/base/optimization.h"

// Must be included last.
#include "Omnira/DataSerialization/port_def.inc"

namespace Omnira {
namespace DataSerialization {
namespace internal {

ABSL_CONST_INIT PROTOBUF_EXPORT
    ABSL_CACHELINE_ALIGNED const char kZeroBuffer[ABSL_CACHELINE_SIZE] = {};

}  // namespace internal
}  // namespace DataSerialization
}  // namespace Omnira
