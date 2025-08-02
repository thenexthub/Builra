//===- toolchain/ADT/EpochTracker.h - ADT epoch tracking --------------*- C++ -*-==//
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
// This file defines the DebugEpochBase and DebugEpochBase::HandleBase classes.
// These can be used to write iterators that are fail-fast when TOOLCHAIN is built
// with asserts enabled.
//
//===----------------------------------------------------------------------===//

#ifndef TOOLCHAIN_ADT_EPOCH_TRACKER_H
#define TOOLCHAIN_ADT_EPOCH_TRACKER_H

#include "toolchain/Config/abi-breaking.h"

#include <cstdint>

namespace toolchain {

#if TOOLCHAIN_ENABLE_ABI_BREAKING_CHECKS

/// A base class for data structure classes wishing to make iterators
/// ("handles") pointing into themselves fail-fast.  When building without
/// asserts, this class is empty and does nothing.
///
/// DebugEpochBase does not by itself track handles pointing into itself.  The
/// expectation is that routines touching the handles will poll on
/// isHandleInSync at appropriate points to assert that the handle they're using
/// is still valid.
///
class DebugEpochBase {
  uint64_t Epoch;

public:
  DebugEpochBase() : Epoch(0) {}

  /// Calling incrementEpoch invalidates all handles pointing into the
  /// calling instance.
  void incrementEpoch() { ++Epoch; }

  /// The destructor calls incrementEpoch to make use-after-free bugs
  /// more likely to crash deterministically.
  ~DebugEpochBase() { incrementEpoch(); }

  /// A base class for iterator classes ("handles") that wish to poll for
  /// iterator invalidating modifications in the underlying data structure.
  /// When TOOLCHAIN is built without asserts, this class is empty and does nothing.
  ///
  /// HandleBase does not track the parent data structure by itself.  It expects
  /// the routines modifying the data structure to call incrementEpoch when they
  /// make an iterator-invalidating modification.
  ///
  class HandleBase {
    const uint64_t *EpochAddress;
    uint64_t EpochAtCreation;

  public:
    HandleBase() : EpochAddress(nullptr), EpochAtCreation(UINT64_MAX) {}

    explicit HandleBase(const DebugEpochBase *Parent)
        : EpochAddress(&Parent->Epoch), EpochAtCreation(Parent->Epoch) {}

    /// Returns true if the DebugEpochBase this Handle is linked to has
    /// not called incrementEpoch on itself since the creation of this
    /// HandleBase instance.
    bool isHandleInSync() const { return *EpochAddress == EpochAtCreation; }

    /// Returns a pointer to the epoch word stored in the data structure
    /// this handle points into.  Can be used to check if two iterators point
    /// into the same data structure.
    const void *getEpochAddress() const { return EpochAddress; }
  };
};

#else

class DebugEpochBase {
public:
  void incrementEpoch() {}

  class HandleBase {
  public:
    HandleBase() = default;
    explicit HandleBase(const DebugEpochBase *) {}
    bool isHandleInSync() const { return true; }
    const void *getEpochAddress() const { return nullptr; }
  };
};

#endif // TOOLCHAIN_ENABLE_ABI_BREAKING_CHECKS

} // namespace toolchain

#endif
