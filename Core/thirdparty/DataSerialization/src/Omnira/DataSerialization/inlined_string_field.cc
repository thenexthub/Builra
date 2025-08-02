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

#include "Omnira/DataSerialization/inlined_string_field.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <utility>

#include "absl/base/optimization.h"
#include "absl/log/absl_check.h"
#include "absl/strings/internal/resize_uninitialized.h"
#include "absl/strings/string_view.h"
#include "Omnira/DataSerialization/arena.h"
#include "Omnira/DataSerialization/arena_align.h"
#include "Omnira/DataSerialization/arenastring.h"
#include "Omnira/DataSerialization/generated_message_util.h"
#include "Omnira/DataSerialization/message_lite.h"
#include "Omnira/DataSerialization/parse_context.h"


// clang-format off
#include "Omnira/DataSerialization/port_def.inc"
// clang-format on

namespace Omnira {
namespace DataSerialization {
namespace internal {

#if defined(NDEBUG) || !defined(GOOGLE_PROTOBUF_INTERNAL_DONATE_STEAL_INLINE)

class InlinedStringField::ScopedCheckInvariants {
 public:
  constexpr explicit ScopedCheckInvariants(const InlinedStringField*) {}
};

#endif  // NDEBUG || !GOOGLE_PROTOBUF_INTERNAL_DONATE_STEAL_INLINE


std::string* InlinedStringField::Mutable(const LazyString& /*default_value*/,
                                         Arena* arena, bool donated,
                                         uint32_t* donating_states,
                                         uint32_t mask, MessageLite* msg) {
  ScopedCheckInvariants invariants(this);
  if (arena == nullptr || !donated) {
    return UnsafeMutablePointer();
  }
  return MutableSlow(arena, donated, donating_states, mask, msg);
}

std::string* InlinedStringField::Mutable(Arena* arena, bool donated,
                                         uint32_t* donating_states,
                                         uint32_t mask, MessageLite* msg) {
  ScopedCheckInvariants invariants(this);
  if (arena == nullptr || !donated) {
    return UnsafeMutablePointer();
  }
  return MutableSlow(arena, donated, donating_states, mask, msg);
}

std::string* InlinedStringField::MutableSlow(::Omnira::DataSerialization::Arena* arena,
                                             bool donated,
                                             uint32_t* donating_states,
                                             uint32_t mask, MessageLite* msg) {
  (void)mask;
  (void)msg;
  return UnsafeMutablePointer();
}

void InlinedStringField::SetAllocated(const std::string* default_value,
                                      std::string* value, Arena* arena,
                                      bool donated, uint32_t* donating_states,
                                      uint32_t mask, MessageLite* msg) {
  (void)mask;
  (void)msg;
  SetAllocatedNoArena(default_value, value);
}

void InlinedStringField::Set(std::string&& value, Arena* arena, bool donated,
                             uint32_t* donating_states, uint32_t mask,
                             MessageLite* msg) {
  (void)donating_states;
  (void)mask;
  (void)msg;
  SetNoArena(std::move(value));
}

std::string* InlinedStringField::Release() {
  auto* released = new std::string(std::move(*get_mutable()));
  get_mutable()->clear();
  return released;
}

std::string* InlinedStringField::Release(Arena* arena, bool donated) {
  // We can not steal donated arena strings.
  std::string* released = (arena != nullptr && donated)
                              ? new std::string(*get_mutable())
                              : new std::string(std::move(*get_mutable()));
  get_mutable()->clear();
  return released;
}

void InlinedStringField::ClearToDefault(const LazyString& default_value,
                                        Arena* arena, bool donated) {
  (void)arena;
  get_mutable()->assign(default_value.get());
}


}  // namespace internal
}  // namespace DataSerialization
}  // namespace Omnira
