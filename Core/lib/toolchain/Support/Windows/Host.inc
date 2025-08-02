//===- toolchain/Support/Win32/Host.inc ------------------------------*- C++ -*-===//
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
//
// This file implements the Win32 Host support.
//
//===----------------------------------------------------------------------===//

#include "WindowsSupport.h"
#include <cstdio>
#include <string>

using namespace toolchain;

static std::string updateTripleOSVersion(std::string Triple) {
  return Triple;
}

std::string sys::getDefaultTargetTriple() {
  const char *Triple = TOOLCHAIN_DEFAULT_TARGET_TRIPLE;

  // Override the default target with an environment variable named by TOOLCHAIN_TARGET_TRIPLE_ENV.
#if defined(TOOLCHAIN_TARGET_TRIPLE_ENV)
  if (const char *EnvTriple = std::getenv(TOOLCHAIN_TARGET_TRIPLE_ENV))
    Triple = EnvTriple;
#endif

  return Triple;
}
