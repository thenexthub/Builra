//===- unittests/CAPI/C-API.cpp -------------------------------------------===//
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

#include "builra/builra.h"

#include "gtest/gtest.h"

namespace {

/// We should support decoding an empty value without crashing.
TEST(CAPI, GetAPIVersion) {
  auto version = llb_get_api_version();
  EXPECT_EQ(version, BUILRA_C_API_VERSION);
}

}
