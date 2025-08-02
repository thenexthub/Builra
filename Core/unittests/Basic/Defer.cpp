//===- unittests/Basic/Defer.cpp ------------------------------------------===//
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

#include "builra/Basic/Defer.h"

#include "gtest/gtest.h"

using namespace builra;
using namespace builra::basic;

namespace {

TEST(DeferTest, basic) {
  int defer_count = 0;

  // Test basic macro use
  {
    builra_defer { defer_count++; };
    EXPECT_EQ(0, defer_count);
  }
  EXPECT_EQ(1, defer_count);

  // Test multiple defers
  defer_count = 0;
  {
    builra_defer { defer_count += 1; };
    EXPECT_EQ(0, defer_count);
    builra_defer { defer_count += 2; };
    EXPECT_EQ(0, defer_count);
  }
  EXPECT_EQ(3, defer_count);

  // Test/show direct use of RAII builder
  defer_count = 0;
  {
    auto deferred = makeScopeDefer([&](){ defer_count += 1; });
    EXPECT_EQ(0, defer_count);
  }
  EXPECT_EQ(1, defer_count);

  // Test/show direct use of ScopeDefer template
  defer_count = 0;
  {
    auto deferred = ScopeDefer<std::function<void()>>([&](){ defer_count += 1; });
    EXPECT_EQ(0, defer_count);
  }
  EXPECT_EQ(1, defer_count);
}

}
