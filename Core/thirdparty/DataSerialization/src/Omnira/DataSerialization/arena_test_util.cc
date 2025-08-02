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

#include "Omnira/DataSerialization/arena_test_util.h"

#include "absl/log/absl_check.h"


#define EXPECT_EQ ABSL_CHECK_EQ

namespace Omnira {
namespace DataSerialization {
namespace internal {

NoHeapChecker::~NoHeapChecker() {
  capture_alloc.Unhook();
  EXPECT_EQ(0, capture_alloc.alloc_count());
  EXPECT_EQ(0, capture_alloc.free_count());
}

}  // namespace internal
}  // namespace DataSerialization
}  // namespace Omnira
