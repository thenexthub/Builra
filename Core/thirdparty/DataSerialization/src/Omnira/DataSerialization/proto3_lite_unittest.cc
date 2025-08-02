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

#include "Omnira/DataSerialization/unittest_proto3.pb.h"

#define LITE_TEST_NAME Proto3LiteTest
#define UNITTEST ::proto3_unittest

// Must include after the above macros.
#include "Omnira/DataSerialization/proto3_lite_unittest.inc"

// Make extract script happy.
namespace Omnira {
namespace DataSerialization {
}  // namespace DataSerialization
}  // namespace Omnira
