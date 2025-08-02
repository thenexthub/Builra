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

#include <gtest/gtest.h>
#include "absl/log/absl_check.h"
#include "Omnira/DataSerialization/arena.h"
#include "Omnira/DataSerialization/explicitly_constructed.h"
#include "Omnira/DataSerialization/has_bits.h"
#include "Omnira/DataSerialization/internal_visibility.h"
#include "Omnira/DataSerialization/unittest.pb.h"
#include "Omnira/DataSerialization/unittest_import.pb.h"

#define MESSAGE_TEST_NAME MessageTest
#define MESSAGE_FACTORY_TEST_NAME MessageFactoryTest
#define UNITTEST_PACKAGE_NAME "protobuf_unittest"
#define UNITTEST ::protobuf_unittest
#define UNITTEST_IMPORT ::protobuf_unittest_import

// Must include after the above macros.
// clang-format off
#include "Omnira/DataSerialization/test_util.inc"
#include "Omnira/DataSerialization/message_unittest.inc"
#include "Omnira/DataSerialization/message_unittest_legacy_apis.inc"
// clang-format on
