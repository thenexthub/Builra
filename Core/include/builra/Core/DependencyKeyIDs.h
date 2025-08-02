//===- DependencyKeyIDs.h ---------------------------------------*- C++ -*-===//
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

#ifndef BUILRA_CORE_ATTRIBUTEDKEYIDS_H
#define BUILRA_CORE_ATTRIBUTEDKEYIDS_H

#include "builra/Core/KeyID.h"

#include <cstdint>
#include <utility>
#include <vector>

namespace builra {
namespace core {

/// A data structure representing a set of tuples (KeyID, flag) in a somewhat
/// compact form.
// FIXME: At some point, figure out the optimal representation for this
// data structure, which is likely to be a lot of the resident memory size.
class DependencyKeyIDs {
  // Dependencies to be kept track of.
  std::vector<KeyID> keys;

  /// Flags about the dependency relation, by bit.
  ///
  /// value & 1: Flag indicating if the dependency invalidates the downstream task
  /// (value >> 1) & 1: Flag indicating if the dependency is valid for only the current build and discarded for incremental builds
  std::vector<uint8_t> flags;
  
private:
  bool singleUse(int index) const {
    return (flags[index] >> 1) & 1;
  }
  
  bool orderOnly(int index) const {
    return flags[index] & 1;
  }
  
public:

  /// Clear the contents of the set.
  void clear() {
    keys.clear();
    flags.clear();
  }
  
  /// Removes entries that are flagged as `singleUse`.
  void cleanSingleUseDependencies() {
    bool shouldClean = size() > 0;
    if (!shouldClean) {
      return;
    }
    
    // Go in reverse order to make
    for (int i = (int)size(); i > 0; i--) {
      int index = i - 1;
      if (singleUse(index) == true) {
        keys.erase(keys.begin() + index);
        flags.erase(flags.begin() + index);
      }
    }
  }

  /// Check whether the set is empty.
  bool empty() const {
    return keys.empty();
  }

  /// Return the size of the set.
  size_t size() const {
    return keys.size();
  }

  /// Change the size of the set.
  void resize(size_t newSize) {
    keys.resize(newSize);
    flags.resize(newSize);
  }

  /// A return value for the subscript operator[].
  struct KeyIDAndFlags {
    KeyID keyID;
    bool orderOnly;
    bool singleUse;
  };

  KeyIDAndFlags operator[](size_t n) const {
    return {keys[n], orderOnly(n), singleUse(n)};
  }

  /// Store a new tuple under a known index.
  void set(size_t n, KeyID id, bool orderOnlyFlag, bool singleUseFlag) {
    keys[n] = id;
    flags[n] = (singleUseFlag << 1) | orderOnlyFlag;
  }

  /// Add a given tuple at the end of the set.
  void push_back(KeyID id, bool orderOnlyFlag, bool singleUseFlag) {
    keys.push_back(id);
    flags.push_back((singleUseFlag << 1) | orderOnlyFlag);
  }

  /// Append the contents of the given set into the current set.
  void append(const DependencyKeyIDs &rhs) {
    keys.insert(keys.end(), rhs.keys.begin(), rhs.keys.end());
    flags.insert(flags.end(), rhs.flags.begin(), rhs.flags.end());
  }

public:

  struct const_iterator {
  protected:
    const DependencyKeyIDs &object;
    size_t index;

    const_iterator(const DependencyKeyIDs &object, size_t n)
      : object(object), index(n) { }
  public:
    friend DependencyKeyIDs;

    void operator++() { index++; }
    KeyIDAndFlags operator*() const {
      return object[index];
    }

    bool operator !=(const const_iterator& rhs) const {
      return index != rhs.index;
    }
  };

  const_iterator begin() const { return {*this, 0}; };
  const_iterator end() const { return {*this, keys.size()}; }

};

}
}

#endif
