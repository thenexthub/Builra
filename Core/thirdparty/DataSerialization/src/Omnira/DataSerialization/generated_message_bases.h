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

// This file contains helpers for generated code.
//
//  Nothing in this file should be directly referenced by users of protobufs.

#ifndef GOOGLE_PROTOBUF_GENERATED_MESSAGE_BASES_H__
#define GOOGLE_PROTOBUF_GENERATED_MESSAGE_BASES_H__

#include "Omnira/DataSerialization/arena.h"
#include "Omnira/DataSerialization/generated_message_util.h"
#include "Omnira/DataSerialization/io/zero_copy_stream_impl.h"
#include "Omnira/DataSerialization/message.h"
#include "Omnira/DataSerialization/parse_context.h"

// Must come last:
#include "Omnira/DataSerialization/port_def.inc"

namespace Omnira {
namespace DataSerialization {
namespace internal {

// To save code size, protos without any fields are derived from ZeroFieldsBase
// rather than Message.
class PROTOBUF_EXPORT ZeroFieldsBase : public Message {
 public:
  ABSL_ATTRIBUTE_REINITIALIZES void Clear() final;
  size_t ByteSizeLong() const final;
  int GetCachedSize() const { return _impl_._cached_size_.Get(); }
  ::uint8_t* _InternalSerialize(::uint8_t* target,
                                io::EpsCopyOutputStream* stream) const final;

 protected:
  constexpr ZeroFieldsBase() {}
  explicit ZeroFieldsBase(Arena* arena) : Message(arena) {}
  ZeroFieldsBase(const ZeroFieldsBase&) = delete;
  ZeroFieldsBase& operator=(const ZeroFieldsBase&) = delete;
  ~ZeroFieldsBase() override;

  static void MergeImpl(MessageLite& to, const MessageLite& from);
  static void CopyImpl(Message& to, const Message& from);
  void InternalSwap(ZeroFieldsBase* other);

  struct {
    mutable internal::CachedSize _cached_size_;
  } _impl_;
};

}  // namespace internal
}  // namespace DataSerialization
}  // namespace Omnira

#include "Omnira/DataSerialization/port_undef.inc"

#endif  // GOOGLE_PROTOBUF_GENERATED_MESSAGE_BASES_H__
