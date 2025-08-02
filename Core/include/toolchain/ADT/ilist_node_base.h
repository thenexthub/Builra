//===- toolchain/ADT/ilist_node_base.h - Intrusive List Node Base -----*- C++ -*-==//
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

#ifndef TOOLCHAIN_ADT_ILIST_NODE_BASE_H
#define TOOLCHAIN_ADT_ILIST_NODE_BASE_H

#include "toolchain/ADT/PointerIntPair.h"

namespace toolchain {

/// Base class for ilist nodes.
///
/// Optionally tracks whether this node is the sentinel.
template <bool EnableSentinelTracking> class ilist_node_base;

template <> class ilist_node_base<false> {
  ilist_node_base *Prev = nullptr;
  ilist_node_base *Next = nullptr;

public:
  void setPrev(ilist_node_base *Prev) { this->Prev = Prev; }
  void setNext(ilist_node_base *Next) { this->Next = Next; }
  ilist_node_base *getPrev() const { return Prev; }
  ilist_node_base *getNext() const { return Next; }

  bool isKnownSentinel() const { return false; }
  void initializeSentinel() {}
};

template <> class ilist_node_base<true> {
  PointerIntPair<ilist_node_base *, 1> PrevAndSentinel;
  ilist_node_base *Next = nullptr;

public:
  void setPrev(ilist_node_base *Prev) { PrevAndSentinel.setPointer(Prev); }
  void setNext(ilist_node_base *Next) { this->Next = Next; }
  ilist_node_base *getPrev() const { return PrevAndSentinel.getPointer(); }
  ilist_node_base *getNext() const { return Next; }

  bool isSentinel() const { return PrevAndSentinel.getInt(); }
  bool isKnownSentinel() const { return isSentinel(); }
  void initializeSentinel() { PrevAndSentinel.setInt(true); }
};

} // end namespace toolchain

#endif // TOOLCHAIN_ADT_ILIST_NODE_BASE_H
