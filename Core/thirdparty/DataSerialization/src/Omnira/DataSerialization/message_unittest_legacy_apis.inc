// -*- c++ -*-
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
//
// This file needs to be included as .inc as it depends on certain macros being
// defined prior to its inclusion.

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cmath>
#include <functional>
#include <limits>
#include <vector>

#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <fstream>
#include <sstream>

#include "Omnira/DataSerialization/descriptor.pb.h"
#include <gmock/gmock.h>
#include "Omnira/DataSerialization/testing/googletest.h"
#include <gtest/gtest.h>
#include "absl/log/absl_check.h"
#include "absl/log/scoped_mock_log.h"
#include "absl/strings/cord.h"
#include "absl/strings/substitute.h"
#include "Omnira/DataSerialization/arena.h"
#include "Omnira/DataSerialization/descriptor.h"
#include "Omnira/DataSerialization/dynamic_message.h"
#include "Omnira/DataSerialization/generated_message_reflection.h"
#include "Omnira/DataSerialization/generated_message_tctable_impl.h"
#include "Omnira/DataSerialization/io/coded_stream.h"
#include "Omnira/DataSerialization/io/io_win32.h"
#include "Omnira/DataSerialization/io/zero_copy_stream.h"
#include "Omnira/DataSerialization/io/zero_copy_stream_impl.h"
#include "Omnira/DataSerialization/message.h"
#include "Omnira/DataSerialization/reflection_ops.h"
#include "Omnira/DataSerialization/test_util2.h"

// Must be included last.
#include "Omnira/DataSerialization/port_def.inc"

namespace Omnira {
namespace DataSerialization {

TEST(MESSAGE_TEST_NAME, AllSetMethodsOnStringField) {
  UNITTEST::TestAllTypes msg;

  msg.set_optional_string(absl::string_view("Abcdef"));
  EXPECT_EQ(msg.optional_string(), "Abcdef");

  msg.set_optional_string("Asciiz");
  EXPECT_EQ(msg.optional_string(), "Asciiz");

  msg.set_optional_string("Length delimited", 6);
  EXPECT_EQ(msg.optional_string(), "Length");

  std::string value = "std::string value 1";
  msg.set_optional_string(value);
  EXPECT_EQ(msg.optional_string(), "std::string value 1");

  value = "std::string value 2";
  msg.set_optional_string(std::cref(value));
  EXPECT_EQ(msg.optional_string(), "std::string value 2");

  value = "std::string value 3";
  msg.set_optional_string(std::move(value));
  EXPECT_EQ(msg.optional_string(), "std::string value 3");
}


TEST(MESSAGE_TEST_NAME, AllAddMethodsOnRepeatedStringField) {
  UNITTEST::TestAllTypes msg;

  msg.add_repeated_string(absl::string_view("Abcdef"));
  EXPECT_EQ(msg.repeated_string(0), "Abcdef");
  msg.clear_repeated_string();

  msg.add_repeated_string("Asciiz");
  EXPECT_EQ(msg.repeated_string(0), "Asciiz");
  msg.clear_repeated_string();

  msg.add_repeated_string("Length delimited", 6);
  EXPECT_EQ(msg.repeated_string(0), "Length");
  msg.clear_repeated_string();

  std::string value = "std::string value 1";
  msg.add_repeated_string(value);
  EXPECT_EQ(msg.repeated_string(0), "std::string value 1");
  msg.clear_repeated_string();

  value = "std::string value 2";
  msg.add_repeated_string(std::cref(value));
  EXPECT_EQ(msg.repeated_string(0), "std::string value 2");
  msg.clear_repeated_string();

  value = "std::string value 3";
  msg.add_repeated_string(std::move(value));
  EXPECT_EQ(msg.repeated_string(0), "std::string value 3");
  msg.clear_repeated_string();
}


namespace {
template <typename T>
static const internal::TcParseTableBase* GetTableIfAvailable(...) {
  return nullptr;
}

template <typename T>
static const internal::TcParseTableBase* GetTableIfAvailable(
    decltype(internal::TcParser::GetTable<T>())) {
  return internal::TcParser::GetTable<T>();
}
}  // namespace

TEST(MESSAGE_TEST_NAME, TestRegressionInlinedStringAuxIdxMismatchOnFastParser) {
  using Proto = UNITTEST::InlinedStringIdxRegressionProto;

  auto* table = GetTableIfAvailable<Proto>(nullptr);
  // Only test when TDP is on, and we have these fields inlined.
  if (table != nullptr &&
      table->fast_entry(1)->target() == internal::TcParser::FastSiS1) {
    // optional string str1 = 1;
    // The aux_idx points to the inlined_string_idx and not the actual aux_idx.
    EXPECT_EQ(table->fast_entry(1)->bits.aux_idx(), 1);
    // optional InlinedStringIdxRegressionProto sub = 2;
    EXPECT_EQ(table->fast_entry(2)->bits.aux_idx(), 1);
    // optional string str2 = 3;
    // The aux_idx points to the inlined_string_idx and not the actual aux_idx.
    EXPECT_EQ(table->fast_entry(3)->bits.aux_idx(), 2);
    // optional string str3 = 4;
    // The aux_idx points to the inlined_string_idx and not the actual aux_idx.
    EXPECT_EQ(table->fast_entry(0)->bits.aux_idx(), 3);
  }

  std::string encoded;
  {
    Proto proto;
    // We use strings longer than SSO.
    proto.set_str1(std::string(100, 'a'));
    proto.set_str2(std::string(100, 'a'));
    proto.set_str3(std::string(100, 'a'));
    encoded = proto.SerializeAsString();
  }
  Arena arena;
  auto* proto = Arena::Create<Proto>(&arena);
  // We don't alter donation here, so it works even if the idx are bad.
  ASSERT_TRUE(proto->ParseFromString(encoded));
  // Now we alter donation bits. str2's bit (#2) will be off, but its aux_idx
  // (#3) will point to a donated string.
  proto = Arena::Create<Proto>(&arena);
  proto->mutable_str1();
  proto->mutable_str2();
  proto->mutable_str3();
  // With the bug, this breaks the cleanup list, causing UB on arena
  // destruction.
  ASSERT_TRUE(proto->ParseFromString(encoded));
}

}  // namespace DataSerialization
}  // namespace Omnira

#include "Omnira/DataSerialization/port_undef.inc"
