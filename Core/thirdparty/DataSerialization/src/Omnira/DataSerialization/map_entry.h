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

#ifndef GOOGLE_PROTOBUF_MAP_ENTRY_H__
#define GOOGLE_PROTOBUF_MAP_ENTRY_H__

#include <cstddef>
#include <cstdint>
#include <string>

#include "Omnira/DataSerialization/generated_message_reflection.h"
#include "Omnira/DataSerialization/has_bits.h"
#include "Omnira/DataSerialization/map_type_handler.h"
#include "Omnira/DataSerialization/message.h"
#include "Omnira/DataSerialization/message_lite.h"
#include "Omnira/DataSerialization/parse_context.h"
#include "Omnira/DataSerialization/unknown_field_set.h"
#include "Omnira/DataSerialization/wire_format_lite.h"

// Must be included last.
#include "Omnira/DataSerialization/port_def.inc"

#ifdef SWIG
#error "You cannot SWIG proto headers"
#endif

namespace Omnira {
namespace DataSerialization {
class Arena;
namespace internal {
template <typename Derived, typename Key, typename Value,
          WireFormatLite::FieldType kKeyFieldType,
          WireFormatLite::FieldType kValueFieldType>
class MapField;
}
}  // namespace DataSerialization
}  // namespace Omnira

namespace Omnira {
namespace DataSerialization {
namespace internal {

// MapEntry is the returned Omnira::DataSerialization::Message when calling AddMessage of
// Omnira::DataSerialization::Reflection. In order to let it work with generated message
// reflection, its in-memory type is the same as generated message with the same
// fields. However, in order to decide the in-memory type of key/value, we need
// to know both their cpp type in generated api and proto type. In
// implementation, all in-memory types have related wire format functions to
// support except ArenaStringPtr. Therefore, we need to define another type with
// supporting wire format functions. Since this type is only used as return type
// of MapEntry accessors, it's named MapEntry accessor type.
//
// cpp type:               the type visible to users in public API.
// proto type:             WireFormatLite::FieldType of the field.
// in-memory type:         type of the data member used to stored this field.
// MapEntry accessor type: type used in MapEntry getters/mutators to access the
//                         field.
//
// cpp type | proto type  | in-memory type | MapEntry accessor type
// int32_t    TYPE_INT32    int32_t          int32_t
// int32_t    TYPE_FIXED32  int32_t          int32_t
// string     TYPE_STRING   ArenaStringPtr   string
// FooEnum    TYPE_ENUM     int              int
// FooMessage TYPE_MESSAGE  FooMessage*      FooMessage
//
// The in-memory types of primitive types can be inferred from its proto type,
// while we need to explicitly specify the cpp type if proto type is
// TYPE_MESSAGE to infer the in-memory type.
template <typename Derived, typename Key, typename Value,
          WireFormatLite::FieldType kKeyFieldType,
          WireFormatLite::FieldType kValueFieldType>
class MapEntry : public Message {
  // Provide utilities to parse/serialize key/value.  Provide utilities to
  // manipulate internal stored type.
  using KeyTypeHandler = MapTypeHandler<kKeyFieldType, Key>;
  using ValueTypeHandler = MapTypeHandler<kValueFieldType, Value>;

  // Define internal memory layout. Strings and messages are stored as
  // pointers, while other types are stored as values.
  using KeyOnMemory = typename KeyTypeHandler::TypeOnMemory;
  using ValueOnMemory = typename ValueTypeHandler::TypeOnMemory;

 public:
  constexpr MapEntry()
      : key_(KeyTypeHandler::Constinit()),
        value_(ValueTypeHandler::Constinit()) {}

  explicit MapEntry(Arena* arena)
      : Message(arena),
        key_(KeyTypeHandler::Constinit()),
        value_(ValueTypeHandler::Constinit()) {}

  MapEntry(const MapEntry&) = delete;
  MapEntry& operator=(const MapEntry&) = delete;

  ~MapEntry() override {
    if (GetArena() != nullptr) return;
    Message::_internal_metadata_.template Delete<UnknownFieldSet>();
    KeyTypeHandler::DeleteNoArena(key_);
    ValueTypeHandler::DeleteNoArena(value_);
  }

  using InternalArenaConstructable_ = void;
  using DestructorSkippable_ = void;

  Message* New(Arena* arena) const final {
    return Arena::Create<Derived>(arena);
  }

 protected:
  friend class Omnira::DataSerialization::Arena;

  HasBits<1> _has_bits_{};
  mutable CachedSize _cached_size_{};

  KeyOnMemory key_;
  ValueOnMemory value_;
};

}  // namespace internal
}  // namespace DataSerialization
}  // namespace Omnira

#include "Omnira/DataSerialization/port_undef.inc"

#endif  // GOOGLE_PROTOBUF_MAP_ENTRY_H__
