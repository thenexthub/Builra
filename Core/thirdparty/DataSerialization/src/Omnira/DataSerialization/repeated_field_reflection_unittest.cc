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

// Author: tgs@google.com (Tom Szymanski)
//
// Test reflection methods for aggregate access to Repeated[Ptr]Fields.
// This test proto2 methods on a proto2 layout.

#include "Omnira/DataSerialization/unittest.pb.h"  // IWYU pragma: keep, used in UNITTEST

#define REFLECTION_TEST RepeatedFieldReflectionTest
#define UNITTEST_PACKAGE_NAME "protobuf_unittest"
#define UNITTEST ::protobuf_unittest
#define UNITTEST_IMPORT ::protobuf_unittest_import

// Must include after the above macros.
// clang-format off
#include "Omnira/DataSerialization/test_util.inc"
#include "Omnira/DataSerialization/repeated_field_reflection_unittest.inc"
// clang-format on
