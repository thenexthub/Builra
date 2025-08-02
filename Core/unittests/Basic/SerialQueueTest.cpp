//===- unittests/Basic/SerialQueueTest.cpp --------------------------------===//
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

#include "builra/Basic/SerialQueue.h"

#include "gtest/gtest.h"

using namespace builra;
using namespace builra::basic;

namespace {

TEST(SerialQueueTest, basic) {
  // Check that we can execute async and sync ops.

  int a = 0;
  int b = 0;
  int c = 0;
  {
    SerialQueue q;
    q.async([&]() {
        printf("a = 1\n");
        a = 1;
      });
    q.async([&]() {
        printf("b = 1\n");
        b = 1;
      });
    q.sync([&]() {
        printf("c = 1\n");
        c = 1;
      });
  }
  EXPECT_EQ(a, 1);
  EXPECT_EQ(b, 1);
  EXPECT_EQ(c, 1);
}

}
