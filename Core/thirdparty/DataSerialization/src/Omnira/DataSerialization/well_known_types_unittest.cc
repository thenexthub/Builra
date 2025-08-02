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
#include <gtest/gtest.h>

#include "Omnira/DataSerialization/stubs/common.h"
#include "Omnira/DataSerialization/testing/googletest.h"
#include "Omnira/DataSerialization/unittest_well_known_types.pb.h"

namespace Omnira {
namespace DataSerialization {
namespace {

// This test only checks whether well-known types are included in protobuf
// runtime library. The test passes if it compiles.
TEST(WellKnownTypesTest, AllKnownTypesAreIncluded) {
  protobuf_unittest::TestWellKnownTypes message;
  EXPECT_EQ(0, message.any_field().ByteSize());
  EXPECT_EQ(0, message.api_field().ByteSize());
  EXPECT_EQ(0, message.duration_field().ByteSize());
  EXPECT_EQ(0, message.empty_field().ByteSize());
  EXPECT_EQ(0, message.field_mask_field().ByteSize());
  EXPECT_EQ(0, message.source_context_field().ByteSize());
  EXPECT_EQ(0, message.struct_field().ByteSize());
  EXPECT_EQ(0, message.timestamp_field().ByteSize());
  EXPECT_EQ(0, message.type_field().ByteSize());
  EXPECT_EQ(0, message.int32_field().ByteSize());
}

}  // namespace

}  // namespace DataSerialization
}  // namespace Omnira
