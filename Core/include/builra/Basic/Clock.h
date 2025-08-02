//===- Clock.h --------------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA_BASIC_CLOCK_h
#define BUILRA_BASIC_CLOCK_h

#include "builra/Basic/Compiler.h"

#include <algorithm>
#include <chrono>

namespace builra {
namespace basic {

class Clock {
public:
  /// A timestamp is the number of seconds since the clock's epoch time.
  typedef double Timestamp;
  
  Clock() BUILRA_DELETED_FUNCTION;
  
  /// Returns a global timestamp that represents the current time in seconds since a reference data.
  /// *NOTE*: This function uses a monotonic clock, so don't compare between systems.
  inline static Timestamp now() {
    // steady_clock is monotonic
    auto now = std::chrono::steady_clock::now();
    auto difference = std::chrono::duration_cast<std::chrono::duration<double>>(now.time_since_epoch());
    return difference.count();
  }
};

}
}

#endif
