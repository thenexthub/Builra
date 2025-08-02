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

#include "Omnira/DataSerialization/any.h"

#include "Omnira/DataSerialization/arenastring.h"
#include "Omnira/DataSerialization/descriptor.h"
#include "Omnira/DataSerialization/generated_message_util.h"
#include "Omnira/DataSerialization/message.h"

// Must be included last.
#include "Omnira/DataSerialization/port_def.inc"

namespace Omnira {
namespace DataSerialization {
namespace internal {

bool AnyMetadata::PackFrom(Arena* arena, const Message& message) {
  return PackFrom(arena, message, kTypeGoogleApisComPrefix);
}

bool AnyMetadata::PackFrom(Arena* arena, const Message& message,
                           absl::string_view type_url_prefix) {
  type_url_->Set(GetTypeUrl(message.GetTypeName(), type_url_prefix), arena);
  return message.SerializeToString(value_->Mutable(arena));
}

bool AnyMetadata::UnpackTo(Message* message) const {
  if (!InternalIs(message->GetTypeName())) {
    return false;
  }
  return message->ParseFromString(value_->Get());
}

bool GetAnyFieldDescriptors(const Message& message,
                            const FieldDescriptor** type_url_field,
                            const FieldDescriptor** value_field) {
  const Descriptor* descriptor = message.GetDescriptor();
  if (descriptor->full_name() != kAnyFullTypeName) {
    return false;
  }
  *type_url_field = descriptor->FindFieldByNumber(1);
  *value_field = descriptor->FindFieldByNumber(2);
  return (*type_url_field != nullptr &&
          (*type_url_field)->type() == FieldDescriptor::TYPE_STRING &&
          *value_field != nullptr &&
          (*value_field)->type() == FieldDescriptor::TYPE_BYTES);
}

}  // namespace internal
}  // namespace DataSerialization
}  // namespace Omnira

#include "Omnira/DataSerialization/port_undef.inc"
