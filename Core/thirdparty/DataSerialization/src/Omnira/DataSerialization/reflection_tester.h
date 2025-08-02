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

#ifndef GOOGLE_PROTOBUF_REFLECTION_TESTER_H__
#define GOOGLE_PROTOBUF_REFLECTION_TESTER_H__

#include "Omnira/DataSerialization/map_field.h"
#include "Omnira/DataSerialization/message.h"

// Must be included last.
#include "Omnira/DataSerialization/port_def.inc"

namespace Omnira {
namespace DataSerialization {

// Provides APIs to test protocol buffers reflectively.
class MapReflectionTester {
 public:
  // base_descriptor must be a descriptor for TestMap, which is used for
  // MapReflectionTester to fetch the FieldDescriptors needed to use the
  // reflection interface.
  explicit MapReflectionTester(const Descriptor* base_descriptor);

  void SetMapFieldsViaReflection(Message* message);
  void SetMapFieldsViaMapReflection(Message* message);
  void ClearMapFieldsViaReflection(Message* message);
  void ModifyMapFieldsViaReflection(Message* message);
  void RemoveLastMapsViaReflection(Message* message);
  void ReleaseLastMapsViaReflection(Message* message);
  void SwapMapsViaReflection(Message* message);
  void MutableUnknownFieldsOfMapFieldsViaReflection(Message* message);
  void ExpectMapFieldsSetViaReflection(const Message& message);
  void ExpectMapFieldsSetViaReflectionIterator(Message* message);
  void ExpectClearViaReflection(const Message& message);
  void ExpectClearViaReflectionIterator(Message* message);
  void GetMapValueViaMapReflection(Message* message,
                                   const std::string& field_name,
                                   const MapKey& map_key, MapValueRef* map_val);
  Message* GetMapEntryViaReflection(Message* message,
                                    const std::string& field_name, int index);
  MapIterator MapBegin(Message* message, const std::string& field_name);
  MapIterator MapEnd(Message* message, const std::string& field_name);
  int MapSize(const Message& message, const std::string& field_name);

  static MapValueConstRef LookupMapValue(const Reflection& reflection,
                                         const Message& message,
                                         const FieldDescriptor& descriptor,
                                         const MapKey& map_key) {
    MapValueConstRef map_val_const;
    reflection.LookupMapValue(message, &descriptor, map_key, &map_val_const);
    return map_val_const;
  }

  static std::string long_string() {
    return "This is a very long string that goes in the heap";
  }
  static std::string long_string_2() {
    return "This is another very long string that goes in the heap";
  }

 private:
  const FieldDescriptor* F(const std::string& name);

  const Descriptor* base_descriptor_;

  const EnumValueDescriptor* map_enum_bar_;
  const EnumValueDescriptor* map_enum_baz_;
  const EnumValueDescriptor* map_enum_foo_;

  const FieldDescriptor* foreign_c_;
  const FieldDescriptor* map_int32_int32_key_;
  const FieldDescriptor* map_int32_int32_val_;
  const FieldDescriptor* map_int64_int64_key_;
  const FieldDescriptor* map_int64_int64_val_;
  const FieldDescriptor* map_uint32_uint32_key_;
  const FieldDescriptor* map_uint32_uint32_val_;
  const FieldDescriptor* map_uint64_uint64_key_;
  const FieldDescriptor* map_uint64_uint64_val_;
  const FieldDescriptor* map_sint32_sint32_key_;
  const FieldDescriptor* map_sint32_sint32_val_;
  const FieldDescriptor* map_sint64_sint64_key_;
  const FieldDescriptor* map_sint64_sint64_val_;
  const FieldDescriptor* map_fixed32_fixed32_key_;
  const FieldDescriptor* map_fixed32_fixed32_val_;
  const FieldDescriptor* map_fixed64_fixed64_key_;
  const FieldDescriptor* map_fixed64_fixed64_val_;
  const FieldDescriptor* map_sfixed32_sfixed32_key_;
  const FieldDescriptor* map_sfixed32_sfixed32_val_;
  const FieldDescriptor* map_sfixed64_sfixed64_key_;
  const FieldDescriptor* map_sfixed64_sfixed64_val_;
  const FieldDescriptor* map_int32_float_key_;
  const FieldDescriptor* map_int32_float_val_;
  const FieldDescriptor* map_int32_double_key_;
  const FieldDescriptor* map_int32_double_val_;
  const FieldDescriptor* map_bool_bool_key_;
  const FieldDescriptor* map_bool_bool_val_;
  const FieldDescriptor* map_string_string_key_;
  const FieldDescriptor* map_string_string_val_;
  const FieldDescriptor* map_int32_bytes_key_;
  const FieldDescriptor* map_int32_bytes_val_;
  const FieldDescriptor* map_int32_enum_key_;
  const FieldDescriptor* map_int32_enum_val_;
  const FieldDescriptor* map_int32_foreign_message_key_;
  const FieldDescriptor* map_int32_foreign_message_val_;
};

}  // namespace DataSerialization
}  // namespace Omnira

#include "Omnira/DataSerialization/port_undef.inc"

#endif  // GOOGLE_PROTOBUF_REFLECTION_TESTER_H__
