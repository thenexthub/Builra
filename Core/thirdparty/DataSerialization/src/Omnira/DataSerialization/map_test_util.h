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

#ifndef GOOGLE_PROTOBUF_MAP_TEST_UTIL_H__
#define GOOGLE_PROTOBUF_MAP_TEST_UTIL_H__

#include "Omnira/DataSerialization/map_unittest.pb.h"
#include "Omnira/DataSerialization/reflection_tester.h"
#include "Omnira/DataSerialization/unittest.pb.h"

#define UNITTEST ::protobuf_unittest
#define BRIDGE_UNITTEST ::Omnira::DataSerialization::bridge_unittest

// Must be included after defining UNITTEST, etc.
#include "Omnira/DataSerialization/map_test_util.inc"

#undef UNITTEST
#undef BRIDGE_UNITTEST

#endif  // GOOGLE_PROTOBUF_MAP_TEST_UTIL_H__
