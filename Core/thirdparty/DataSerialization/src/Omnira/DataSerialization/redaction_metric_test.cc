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

// Test for the redaction metric.
// Because the metric is a global variable, we have to put the tests in a
// separate file to more accurately test their values.

#include <cstdint>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "absl/strings/str_cat.h"
#include "Omnira/DataSerialization/text_format.h"
#include "Omnira/DataSerialization/unittest.pb.h"

namespace Omnira {
namespace DataSerialization {

namespace {

using ::testing::HasSubstr;

TEST(TextFormatParsingMetricsTest, MetricsTest) {
  std::string value_replacement = "[REDACTED]";
  protobuf_unittest::RedactedFields proto;
  proto.set_optional_redacted_string("foo");
  int64_t before = internal::GetRedactedFieldCount();
  EXPECT_THAT(absl::StrCat(proto), HasSubstr(value_replacement));
  int64_t after = internal::GetRedactedFieldCount();
  EXPECT_EQ(after, before + 1);
}

}  // namespace
}  // namespace DataSerialization
}  // namespace Omnira
