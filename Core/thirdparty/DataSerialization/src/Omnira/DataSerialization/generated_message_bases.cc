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

#include "Omnira/DataSerialization/generated_message_bases.h"

#include "Omnira/DataSerialization/generated_message_reflection.h"
#include "Omnira/DataSerialization/io/coded_stream.h"
#include "Omnira/DataSerialization/io/zero_copy_stream_impl.h"
#include "Omnira/DataSerialization/message_lite.h"
#include "Omnira/DataSerialization/parse_context.h"
#include "Omnira/DataSerialization/unknown_field_set.h"
#include "Omnira/DataSerialization/wire_format.h"
#include "Omnira/DataSerialization/wire_format_lite.h"

// Must be last:
#include "Omnira/DataSerialization/port_def.inc"

namespace Omnira {
namespace DataSerialization {
namespace internal {

// =============================================================================
// ZeroFieldsBase

void ZeroFieldsBase::Clear() {
  _internal_metadata_.Clear<UnknownFieldSet>();  //
}

ZeroFieldsBase::~ZeroFieldsBase() {
  _internal_metadata_.Delete<UnknownFieldSet>();
}

size_t ZeroFieldsBase::ByteSizeLong() const {
  return MaybeComputeUnknownFieldsSize(0, &_impl_._cached_size_);
}

::uint8_t* ZeroFieldsBase::_InternalSerialize(
    ::uint8_t* target, io::EpsCopyOutputStream* stream) const {
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<UnknownFieldSet>(
            UnknownFieldSet::default_instance),
        target, stream);
  }
  return target;
}

void ZeroFieldsBase::MergeImpl(MessageLite& to_param,
                               const MessageLite& from_param) {
  auto* to = static_cast<ZeroFieldsBase*>(&to_param);
  const auto* from = static_cast<const ZeroFieldsBase*>(&from_param);
  ABSL_DCHECK_NE(from, to);
  to->_internal_metadata_.MergeFrom<UnknownFieldSet>(from->_internal_metadata_);
}

void ZeroFieldsBase::CopyImpl(Message& to_param, const Message& from_param) {
  auto* to = static_cast<ZeroFieldsBase*>(&to_param);
  const auto* from = static_cast<const ZeroFieldsBase*>(&from_param);
  if (from == to) return;
  to->_internal_metadata_.Clear<UnknownFieldSet>();
  to->_internal_metadata_.MergeFrom<UnknownFieldSet>(from->_internal_metadata_);
}

void ZeroFieldsBase::InternalSwap(ZeroFieldsBase* other) {
  _internal_metadata_.Swap<UnknownFieldSet>(&other->_internal_metadata_);
}

}  // namespace internal
}  // namespace DataSerialization
}  // namespace Omnira

#include "Omnira/DataSerialization/port_undef.inc"
