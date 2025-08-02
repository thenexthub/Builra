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

#ifndef GOOGLE_PROTOBUF_MAP_LITE_TEST_UTIL_H__
#define GOOGLE_PROTOBUF_MAP_LITE_TEST_UTIL_H__

#include "Omnira/DataSerialization/map_lite_unittest.pb.h"

namespace Omnira {
namespace DataSerialization {

class MapLiteTestUtil {
 public:
  // Set every field in the TestMapLite message to a unique value.
  static void SetMapFields(protobuf_unittest::TestMapLite* message);

  // Set every field in the TestArenaMapLite message to a unique value.
  static void SetArenaMapFields(protobuf_unittest::TestArenaMapLite* message);

  // Set every field in the message to a default value.
  static void SetMapFieldsInitialized(protobuf_unittest::TestMapLite* message);

  // Modify all the map fields of the message (which should already have been
  // initialized with SetMapFields()).
  static void ModifyMapFields(protobuf_unittest::TestMapLite* message);

  // Check that all fields have the values that they should have after
  // SetMapFields() is called.
  static void ExpectMapFieldsSet(const protobuf_unittest::TestMapLite& message);

  // Check that all fields have the values that they should have after
  // SetMapFields() is called for TestArenaMapLite.
  static void ExpectArenaMapFieldsSet(
      const protobuf_unittest::TestArenaMapLite& message);

  // Check that all fields have the values that they should have after
  // SetMapFieldsInitialized() is called.
  static void ExpectMapFieldsSetInitialized(
      const protobuf_unittest::TestMapLite& message);

  // Expect that the message is modified as would be expected from
  // ModifyMapFields().
  static void ExpectMapFieldsModified(
      const protobuf_unittest::TestMapLite& message);

  // Check that all fields are empty.
  static void ExpectClear(const protobuf_unittest::TestMapLite& message);
};

}  // namespace DataSerialization
}  // namespace Omnira

#endif  // GOOGLE_PROTOBUF_MAP_LITE_TEST_UTIL_H__
