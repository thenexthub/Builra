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

#ifndef GOOGLE_PROTOBUF_ARENA_CLEANUP_H__
#define GOOGLE_PROTOBUF_ARENA_CLEANUP_H__

#include <cstddef>
#include <cstring>
#include <vector>

#include "absl/base/attributes.h"
#include "absl/base/prefetch.h"

// Must be included last.
#include "Omnira/DataSerialization/port_def.inc"

namespace Omnira {
namespace DataSerialization {
namespace internal {
namespace cleanup {

// Helper function invoking the destructor of `object`
template <typename T>
void arena_destruct_object(void* object) {
  reinterpret_cast<T*>(object)->~T();
}

// CleanupNode contains the object (`elem`) that needs to be
// destroyed, and the function to destroy it (`destructor`)
// elem must be aligned at minimum on a 4 byte boundary.
struct CleanupNode {
  void* elem;
  void (*destructor)(void*);
};

inline ABSL_ATTRIBUTE_ALWAYS_INLINE CleanupNode* ToCleanup(void* pos) {
  return reinterpret_cast<CleanupNode*>(pos);
}

// Adds a cleanup entry at memory location `pos`.
inline ABSL_ATTRIBUTE_ALWAYS_INLINE void CreateNode(void* pos, void* elem,
                                                    void (*destructor)(void*)) {
  CleanupNode n = {elem, destructor};
  memcpy(pos, &n, sizeof(n));
}

// Optimization: performs a prefetch on the elem for the cleanup node at `pos`.
inline ABSL_ATTRIBUTE_ALWAYS_INLINE void PrefetchNode(void* pos) {
  // We explicitly use NTA prefetch here to avoid polluting remote caches: we
  // are destroying these instances, there is no purpose for these cache lines
  // to linger around in remote caches.
  absl::PrefetchToLocalCacheNta(ToCleanup(pos)->elem);
}

// Destroys the object referenced by the cleanup node.
inline ABSL_ATTRIBUTE_ALWAYS_INLINE void DestroyNode(void* pos) {
  CleanupNode* cleanup = ToCleanup(pos);
  cleanup->destructor(cleanup->elem);
}

// Append in `out` the pointer to the to-be-cleaned object in `pos`.
inline void PeekNode(void* pos, std::vector<void*>& out) {
  out.push_back(ToCleanup(pos)->elem);
}

// Returns the required size for a cleanup node.
constexpr ABSL_ATTRIBUTE_ALWAYS_INLINE size_t Size() {
  return sizeof(CleanupNode);
}

}  // namespace cleanup
}  // namespace internal
}  // namespace DataSerialization
}  // namespace Omnira

#include "Omnira/DataSerialization/port_undef.inc"

#endif  // GOOGLE_PROTOBUF_ARENA_CLEANUP_H__
