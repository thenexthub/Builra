//===- unittests/Basic/ShellUtilityTest.cpp -------------------------------===//
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

#include "builra/Basic/ShellUtility.h"

#include "gtest/gtest.h"

using namespace builra;
using namespace builra::basic;

namespace {

std::string quoted(std::string s) {
#if defined(_WIN32)
  std::string quote = "\"";
#else
  std::string quote = "'";
#endif
  return quote + s + quote;
}

TEST(UtilityTest, basic) {
  // No escapable char.
  std::string output = shellEscaped("input01");
  EXPECT_EQ(output, "input01");

  // Space.
  output = shellEscaped("input A");
#if defined(_WIN32)
  std::string quote = "\"";
#else
  std::string quote = "'";
#endif

  EXPECT_EQ(output, quoted("input A"));

  // Two spaces.
  output = shellEscaped("input A B");
  EXPECT_EQ(output, quoted("input A B"));

  // Double Quote.
  output = shellEscaped("input\"A");
#if defined(_WIN32)
  EXPECT_EQ(output, quoted("input") + "\\" + quote + quoted("A"));
#else
  EXPECT_EQ(output, quoted("input\"A"));
#endif

  // Single Quote.
  output = shellEscaped("input'A");
#if defined(_WIN32)
  EXPECT_EQ(output, quoted("input'A"));
#else
  EXPECT_EQ(output, "'input'\\''A'");
#endif

  // Question Mark.
  output = shellEscaped("input?A");
  EXPECT_EQ(output, quoted("input?A"));

  // New line.
  output = shellEscaped("input\nA");
  EXPECT_EQ(output, quoted("input\nA"));

  // Multiple special chars.
#if defined(_WIN32)
  output = shellEscaped("input\nA'B C>D*[$;()^><");
  EXPECT_EQ(output, quoted("input\nA'B C>D*[$;()^><"));
#else
  output = shellEscaped("input\nA\"B C>D*[$;()^><");
  EXPECT_EQ(output, quoted("input\nA\"B C>D*[$;()^><"));
#endif
}

}
