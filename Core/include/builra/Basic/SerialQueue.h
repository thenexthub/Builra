//===- SerialQueue.h --------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA_BASIC_SERIALQUEUE_H
#define BUILRA_BASIC_SERIALQUEUE_H

#include <functional>

namespace builra {
namespace basic {

/// A basic serial operation queue.
class SerialQueue {
  void *impl;

public:
  SerialQueue();
  ~SerialQueue();

  /// Add an operation and wait for it to complete.
  void sync(std::function<void(void)> fn);

  /// Add an operation to the queue and return.
  void async(std::function<void(void)> fn);
};

}
}

#endif
