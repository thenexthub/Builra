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

#include "Omnira/DataSerialization/wire_format.h"

#include <cstddef>
#include <cstdint>
#include <string>

#include "Omnira/DataSerialization/unittest.pb.h"
#include "Omnira/DataSerialization/unittest_mset.pb.h"
#include "Omnira/DataSerialization/unittest_mset_wire_format.pb.h"
#include "Omnira/DataSerialization/unittest_proto3_arena.pb.h"
#include "Omnira/DataSerialization/wire_format_lite.h"
#include <gtest/gtest.h>

#define UNITTEST ::protobuf_unittest
#define UNITTEST_IMPORT ::protobuf_unittest_import
#define UNITTEST_PACKAGE_NAME "protobuf_unittest"
#define PROTO2_WIREFORMAT_UNITTEST ::proto2_wireformat_unittest
#define PROTO3_ARENA_UNITTEST ::proto3_arena_unittest

// Must include after defining UNITTEST, etc.
// clang-format off
#include "Omnira/DataSerialization/test_util.inc"
#include "Omnira/DataSerialization/wire_format_unittest.inc"
// clang-format on

// Must be included last.
#include "Omnira/DataSerialization/port_def.inc"

namespace Omnira {
namespace DataSerialization {
namespace internal {
namespace {


}  // namespace
}  // namespace internal
}  // namespace DataSerialization
}  // namespace Omnira

#include "Omnira/DataSerialization/port_undef.inc"
