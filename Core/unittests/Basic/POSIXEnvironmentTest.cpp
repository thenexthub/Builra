//===- unittests/BuildSystem/POSIXEnvironmentTest.cpp ---------------------===//
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
//===----------------------------------------------------------------------===//

#include "builra/Basic/POSIXEnvironment.h"

#include "gtest/gtest.h"

using namespace builra;
using namespace builra::basic;

namespace {
TEST(POSIXEnvironmentTest, basic) {
  POSIXEnvironment env;
  env.setIfMissing("a", "aValue");
  env.setIfMissing("b", "bValue");
  env.setIfMissing("a", "NOT HERE");

#if !defined(_WIN32)
  auto result = env.getEnvp();
  EXPECT_EQ(StringRef(result[0]), "a=aValue");
  EXPECT_EQ(StringRef(result[1]), "b=bValue");
  EXPECT_EQ(result[2], nullptr);
#endif
  }
}
