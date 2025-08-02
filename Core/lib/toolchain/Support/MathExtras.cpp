//===-- MathExtras.cpp - Implement the MathExtras header --------------===//
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
// This file implements the MathExtras.h header
//
//===----------------------------------------------------------------------===//

#include "toolchain/Support/MathExtras.h"

#ifdef _MSC_VER
#include <limits>
#else
#include <math.h>
#endif

namespace toolchain {

#if defined(_MSC_VER)
  // Visual Studio defines the HUGE_VAL class of macros using purposeful
  // constant arithmetic overflow, which it then warns on when encountered.
  const float huge_valf = std::numeric_limits<float>::infinity();
#else
  const float huge_valf = HUGE_VALF;
#endif

}
