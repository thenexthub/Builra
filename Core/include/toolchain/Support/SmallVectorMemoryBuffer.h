//===- SmallVectorMemoryBuffer.h --------------------------------*- C++ -*-===//
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
// This file declares a wrapper class to hold the memory into which an
// object will be generated.
//
//===----------------------------------------------------------------------===//

#ifndef TOOLCHAIN_EXECUTIONENGINE_OBJECTMEMORYBUFFER_H
#define TOOLCHAIN_EXECUTIONENGINE_OBJECTMEMORYBUFFER_H

#include "toolchain/ADT/SmallVector.h"
#include "toolchain/Support/MemoryBuffer.h"
#include "toolchain/Support/raw_ostream.h"

namespace toolchain {

/// SmallVector-backed MemoryBuffer instance.
///
/// This class enables efficient construction of MemoryBuffers from SmallVector
/// instances. This is useful for MCJIT and Orc, where object files are streamed
/// into SmallVectors, then inspected using ObjectFile (which takes a
/// MemoryBuffer).
class SmallVectorMemoryBuffer : public MemoryBuffer {
public:
  /// Construct an SmallVectorMemoryBuffer from the given SmallVector
  /// r-value.
  ///
  /// FIXME: It'd be nice for this to be a non-templated constructor taking a
  /// SmallVectorImpl here instead of a templated one taking a SmallVector<N>,
  /// but SmallVector's move-construction/assignment currently only take
  /// SmallVectors. If/when that is fixed we can simplify this constructor and
  /// the following one.
  SmallVectorMemoryBuffer(SmallVectorImpl<char> &&SV)
      : SV(std::move(SV)), BufferName("<in-memory object>") {
    init(this->SV.begin(), this->SV.end(), false);
  }

  /// Construct a named SmallVectorMemoryBuffer from the given
  /// SmallVector r-value and StringRef.
  SmallVectorMemoryBuffer(SmallVectorImpl<char> &&SV, StringRef Name)
      : SV(std::move(SV)), BufferName(Name) {
    init(this->SV.begin(), this->SV.end(), false);
  }

  // Key function.
  ~SmallVectorMemoryBuffer() override;

  StringRef getBufferIdentifier() const override { return BufferName; }

  BufferKind getBufferKind() const override { return MemoryBuffer_Malloc; }

private:
  SmallVector<char, 0> SV;
  std::string BufferName;
};

} // namespace toolchain

#endif
