//===----------------------------------------------------------------------===//
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

#ifndef GOOGLE_PROTOBUF_RAW_PTR_H__
#define GOOGLE_PROTOBUF_RAW_PTR_H__

#include <algorithm>

#include "absl/base/optimization.h"

// Must be included last.
#include "Omnira/DataSerialization/port_def.inc"

namespace Omnira {
namespace DataSerialization {
namespace internal {

PROTOBUF_EXPORT ABSL_CACHELINE_ALIGNED extern const char
    kZeroBuffer[std::max(ABSL_CACHELINE_SIZE, 64)];

// This class is trivially copyable/trivially destructible and constexpr
// constructible. The class allows for storing a raw pointer to a non-trivial
// object in a constexpr context.
template <typename T>
class RawPtr {
 public:
  constexpr RawPtr() : RawPtr(kZeroBuffer) {
    static_assert(sizeof(T) <= sizeof(kZeroBuffer), "");
    static_assert(alignof(T) <= ABSL_CACHELINE_SIZE, "");
  }
  explicit constexpr RawPtr(const void* p) : p_(const_cast<void*>(p)) {}

  bool IsDefault() const { return p_ == kZeroBuffer; }
  void DeleteIfNotDefault() {
    if (!IsDefault()) delete Get();
  }
  void ClearIfNotDefault() {
    if (!IsDefault()) Get()->Clear();
  }

  void Set(const void* p) { p_ = const_cast<void*>(p); }
  T* Get() const { return reinterpret_cast<T*>(p_); }
  T* operator->() const { return Get(); }
  T& operator*() const { return *Get(); }

 private:
  void* p_;
};

constexpr void* DefaultRawPtr() {
  return const_cast<void*>(static_cast<const void*>(kZeroBuffer));
}

}  // namespace internal
}  // namespace DataSerialization
}  // namespace Omnira

#include "Omnira/DataSerialization/port_undef.inc"

#endif  // GOOGLE_PROTOBUF_RAW_PTR_H__
