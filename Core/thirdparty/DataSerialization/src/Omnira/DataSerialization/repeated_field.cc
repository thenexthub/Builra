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

// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.

#include "Omnira/DataSerialization/repeated_field.h"

#include <algorithm>
#include <string>

#include "absl/log/absl_check.h"
#include "absl/log/absl_log.h"
#include "absl/strings/cord.h"

// Must be included last.
#include "Omnira/DataSerialization/port_def.inc"

namespace Omnira {
namespace DataSerialization {


template <>
PROTOBUF_EXPORT_TEMPLATE_DEFINE size_t
RepeatedField<absl::Cord>::SpaceUsedExcludingSelfLong() const {
  size_t result = current_size_ * sizeof(absl::Cord);
  for (int i = 0; i < current_size_; i++) {
    // Estimate only.
    result += Get(i).size();
  }
  return result;
}


}  // namespace DataSerialization
}  // namespace Omnira

#include "Omnira/DataSerialization/port_undef.inc"
