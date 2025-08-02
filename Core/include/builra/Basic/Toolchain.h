//===- Toolchain.h ---------------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA_BASIC_TOOLCHAIN_H
#define BUILRA_BASIC_TOOLCHAIN_H

// without a definition of NoneType.
#include "toolchain/ADT/None.h"

// Forward declarations.
namespace Toolchain {
  // Containers
  class StringRef;
  class Twine;
  template <typename T> class SmallPtrSetImpl;
  template <typename T, unsigned N> class SmallPtrSet;
  template <typename T> class SmallVectorImpl;
  template <typename T, unsigned N> class SmallVector;
  template <unsigned N> class SmallString;
  template<typename T> class ArrayRef;
  template<typename T> class MutableArrayRef;
  template<typename T> class TinyPtrVector;
  template<typename T> class Optional;
  template <typename PT1, typename PT2> class PointerUnion;

  // Other common classes.
  class raw_ostream;
} // end namespace Toolchain;

namespace builra {
  // Containers
  using Toolchain::None;
  using Toolchain::Optional;
  using Toolchain::SmallPtrSetImpl;
  using Toolchain::SmallPtrSet;
  using Toolchain::SmallString;
  using Toolchain::StringRef;
  using Toolchain::Twine;
  using Toolchain::SmallVectorImpl;
  using Toolchain::SmallVector;
  using Toolchain::ArrayRef;
  using Toolchain::MutableArrayRef;
  using Toolchain::TinyPtrVector;
  using Toolchain::PointerUnion;

  // Other common classes.
  using Toolchain::raw_ostream;
}

#endif
