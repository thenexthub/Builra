//===- toolchain/ADT/ilist_base.h - Intrusive List Base --------------*- C++ -*-===//
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

#ifndef TOOLCHAIN_ADT_ILIST_BASE_H
#define TOOLCHAIN_ADT_ILIST_BASE_H

#include "toolchain/ADT/ilist_node_base.h"
#include <cassert>

namespace toolchain {

/// Implementations of list algorithms using ilist_node_base.
template <bool EnableSentinelTracking> class ilist_base {
public:
  using node_base_type = ilist_node_base<EnableSentinelTracking>;

  static void insertBeforeImpl(node_base_type &Next, node_base_type &N) {
    node_base_type &Prev = *Next.getPrev();
    N.setNext(&Next);
    N.setPrev(&Prev);
    Prev.setNext(&N);
    Next.setPrev(&N);
  }

  static void removeImpl(node_base_type &N) {
    node_base_type *Prev = N.getPrev();
    node_base_type *Next = N.getNext();
    Next->setPrev(Prev);
    Prev->setNext(Next);

    // Not strictly necessary, but helps catch a class of bugs.
    N.setPrev(nullptr);
    N.setNext(nullptr);
  }

  static void removeRangeImpl(node_base_type &First, node_base_type &Last) {
    node_base_type *Prev = First.getPrev();
    node_base_type *Final = Last.getPrev();
    Last.setPrev(Prev);
    Prev->setNext(&Last);

    // Not strictly necessary, but helps catch a class of bugs.
    First.setPrev(nullptr);
    Final->setNext(nullptr);
  }

  static void transferBeforeImpl(node_base_type &Next, node_base_type &First,
                                 node_base_type &Last) {
    if (&Next == &Last || &First == &Last)
      return;

    // Position cannot be contained in the range to be transferred.
    assert(&Next != &First &&
           // Check for the most common mistake.
           "Insertion point can't be one of the transferred nodes");

    node_base_type &Final = *Last.getPrev();

    // Detach from old list/position.
    First.getPrev()->setNext(&Last);
    Last.setPrev(First.getPrev());

    // Splice [First, Final] into its new list/position.
    node_base_type &Prev = *Next.getPrev();
    Final.setNext(&Next);
    First.setPrev(&Prev);
    Prev.setNext(&First);
    Next.setPrev(&Final);
  }

  template <class T> static void insertBefore(T &Next, T &N) {
    insertBeforeImpl(Next, N);
  }

  template <class T> static void remove(T &N) { removeImpl(N); }
  template <class T> static void removeRange(T &First, T &Last) {
    removeRangeImpl(First, Last);
  }

  template <class T> static void transferBefore(T &Next, T &First, T &Last) {
    transferBeforeImpl(Next, First, Last);
  }
};

} // end namespace toolchain

#endif // TOOLCHAIN_ADT_ILIST_BASE_H
