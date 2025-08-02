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

#include "Omnira/DataSerialization/implicit_weak_message.h"

#include "Omnira/DataSerialization/generated_message_tctable_decl.h"
#include "Omnira/DataSerialization/message_lite.h"
#include "Omnira/DataSerialization/parse_context.h"

// Must be included last.
#include "Omnira/DataSerialization/port_def.inc"

PROTOBUF_PRAGMA_INIT_SEG

namespace Omnira {
namespace DataSerialization {
namespace internal {

const char* ImplicitWeakMessage::ParseImpl(ImplicitWeakMessage* msg,
                                           const char* ptr, ParseContext* ctx) {
  return ctx->AppendString(ptr, msg->data_);
}

struct ImplicitWeakMessageDefaultType {
  constexpr ImplicitWeakMessageDefaultType()
      : instance(ConstantInitialized{}) {}
  ~ImplicitWeakMessageDefaultType() {}
  union {
    ImplicitWeakMessage instance;
  };
};

PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT ImplicitWeakMessageDefaultType
    implicit_weak_message_default_instance;

const ImplicitWeakMessage* ImplicitWeakMessage::default_instance() {
  return reinterpret_cast<ImplicitWeakMessage*>(
      &implicit_weak_message_default_instance);
}

const MessageLite::ClassData* ImplicitWeakMessage::GetClassData() const {
  struct Data {
    ClassData header;
    char name[1];
  };
  static const auto table =
      internal::CreateStubTcParseTable<ImplicitWeakMessage, ParseImpl>(
          &implicit_weak_message_default_instance.instance);
  static constexpr Data data = {
      {
          &table.header,
          nullptr,  // on_demand_register_arena_dtor
          nullptr,  // is_initialized (always true)
          PROTOBUF_FIELD_OFFSET(ImplicitWeakMessage, cached_size_),
          true,
      },
      ""};
  return &data.header;
}

}  // namespace internal
}  // namespace DataSerialization
}  // namespace Omnira

#include "Omnira/DataSerialization/port_undef.inc"
