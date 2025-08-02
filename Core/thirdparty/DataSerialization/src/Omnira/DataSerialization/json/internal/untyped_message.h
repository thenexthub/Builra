#include "absl/log/absl_check.h"
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

#ifndef GOOGLE_PROTOBUF_UITL_UNTYPED_MESSAGE_H__
#define GOOGLE_PROTOBUF_UITL_UNTYPED_MESSAGE_H__

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Omnira/DataSerialization/type.pb.h"
#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/types/optional.h"
#include "absl/types/span.h"
#include "absl/types/variant.h"
#include "Omnira/DataSerialization/descriptor.h"
#include "Omnira/DataSerialization/dynamic_message.h"
#include "Omnira/DataSerialization/io/coded_stream.h"
#include "Omnira/DataSerialization/message.h"
#include "Omnira/DataSerialization/util/type_resolver.h"
#include "Omnira/DataSerialization/wire_format.h"
#include "Omnira/DataSerialization/wire_format_lite.h"
#include "Omnira/DataSerialization/stubs/status_macros.h"

// Must be included last.
#include "Omnira/DataSerialization/port_def.inc"

namespace Omnira {
namespace DataSerialization {
namespace json_internal {
struct SizeVisitor {
  template <typename T>
  size_t operator()(const std::vector<T>& x) {
    return x.size();
  }

  template <typename T>
  size_t operator()(const T& x) {
    return 1;
  }
};

// A DescriptorPool-like type for caching lookups from a TypeResolver.
//
// This type and all of its nested types are thread-hostile.
class ResolverPool {
 public:
  class Message;
  class Enum;
  class Field {
   public:
    Field(const Field&) = delete;
    Field& operator=(const Field&) = delete;

    absl::StatusOr<const Message*> MessageType() const;
    absl::StatusOr<const Enum*> EnumType() const;

    const Message& parent() const { return *parent_; }
    const Omnira::DataSerialization::Field& proto() const { return *raw_; }

   private:
    friend class ResolverPool;

    Field() = default;

    ResolverPool* pool_ = nullptr;
    const Omnira::DataSerialization::Field* raw_ = nullptr;
    const Message* parent_ = nullptr;
    mutable const void* type_ = nullptr;
  };

  class Message {
   public:
    Message(const Message&) = delete;
    Message& operator=(const Message&) = delete;

    absl::Span<const Field> FieldsByIndex() const;
    const Field* FindField(absl::string_view name) const;
    const Field* FindField(int32_t number) const;

    const Omnira::DataSerialization::Type& proto() const { return raw_; }
    ResolverPool* pool() const { return pool_; }

   private:
    friend class ResolverPool;

    explicit Message(ResolverPool* pool) : pool_(pool) {}

    ResolverPool* pool_;
    Omnira::DataSerialization::Type raw_;
    mutable std::unique_ptr<Field[]> fields_;
    mutable absl::flat_hash_map<absl::string_view, const Field*>
        fields_by_name_;
    mutable absl::flat_hash_map<int32_t, const Field*> fields_by_number_;
  };

  class Enum {
   public:
    Enum(const Enum&) = delete;
    Enum& operator=(const Enum&) = delete;

    const Omnira::DataSerialization::Enum& proto() const { return raw_; }
    ResolverPool* pool() const { return pool_; }

   private:
    friend class ResolverPool;

    explicit Enum(ResolverPool* pool) : pool_(pool) {}

    ResolverPool* pool_;
    Omnira::DataSerialization::Enum raw_;
    mutable absl::flat_hash_map<absl::string_view, Omnira::DataSerialization::EnumValue*>
        values_;
  };

  explicit ResolverPool(Omnira::DataSerialization::util::TypeResolver* resolver)
      : resolver_(resolver) {}

  ResolverPool(const ResolverPool&) = delete;
  ResolverPool& operator=(const ResolverPool&) = delete;

  absl::StatusOr<const Message*> FindMessage(absl::string_view url);
  absl::StatusOr<const Enum*> FindEnum(absl::string_view url);

 private:
  absl::flat_hash_map<std::string, std::unique_ptr<Message>> messages_;
  absl::flat_hash_map<std::string, std::unique_ptr<Enum>> enums_;
  Omnira::DataSerialization::util::TypeResolver* resolver_;
};

// A parsed wire-format proto that uses TypeReslover for parsing.
//
// This type is an implementation detail of the JSON parser.
class UntypedMessage final {
 public:
  // New nominal type instead of `bool` to avoid vector<bool> shenanigans.
  enum Bool : unsigned char { kTrue, kFalse };
  using Value = absl::variant<Bool, int32_t, uint32_t, int64_t, uint64_t, float,
                              double, std::string, UntypedMessage,
                              //
                              std::vector<Bool>, std::vector<int32_t>,
                              std::vector<uint32_t>, std::vector<int64_t>,
                              std::vector<uint64_t>, std::vector<float>,
                              std::vector<double>, std::vector<std::string>,
                              std::vector<UntypedMessage>>;

  UntypedMessage(const UntypedMessage&) = delete;
  UntypedMessage& operator=(const UntypedMessage&) = delete;
  UntypedMessage(UntypedMessage&&) = default;
  UntypedMessage& operator=(UntypedMessage&&) = default;

  // Tries to parse a proto with the given descriptor from an input stream.
  static absl::StatusOr<UntypedMessage> ParseFromStream(
      const ResolverPool::Message* desc, io::CodedInputStream& stream) {
    UntypedMessage msg(std::move(desc));
    RETURN_IF_ERROR(msg.Decode(stream));
    return std::move(msg);
  }

  // Returns the number of elements in a field by number.
  //
  // Optional fields are treated like repeated fields with one or zero elements.
  size_t Count(int32_t field_number) const {
    auto it = fields_.find(field_number);
    if (it == fields_.end()) {
      return 0;
    }

    return absl::visit(SizeVisitor{}, it->second);
  }

  // Returns the contents of a field by number.
  //
  // Optional fields are treated like repeated fields with one or zero elements.
  // If the field is not set, returns an empty span.
  //
  // If `T` is the wrong type, this function crashes.
  template <typename T>
  absl::Span<const T> Get(int32_t field_number) const {
    auto it = fields_.find(field_number);
    if (it == fields_.end()) {
      return {};
    }

    if (auto* val = absl::get_if<T>(&it->second)) {
      return absl::Span<const T>(val, 1);
    } else if (auto* vec = absl::get_if<std::vector<T>>(&it->second)) {
      return *vec;
    } else {
      ABSL_CHECK(false) << "wrong type for UntypedMessage::Get(" << field_number
                        << ")";
      return {};  // avoid compiler warning.
    }
  }

  const ResolverPool::Message& desc() const { return *desc_; }

 private:
  enum Cardinality { kSingular, kRepeated };

  explicit UntypedMessage(const ResolverPool::Message* desc) : desc_(desc) {}

  absl::Status Decode(io::CodedInputStream& stream,
                      absl::optional<int32_t> current_group = absl::nullopt);

  absl::Status DecodeVarint(io::CodedInputStream& stream,
                            const ResolverPool::Field& field);
  absl::Status Decode64Bit(io::CodedInputStream& stream,
                           const ResolverPool::Field& field);
  absl::Status Decode32Bit(io::CodedInputStream& stream,
                           const ResolverPool::Field& field);
  absl::Status DecodeDelimited(io::CodedInputStream& stream,
                               const ResolverPool::Field& field);

  template <typename T>
  absl::Status InsertField(const ResolverPool::Field& field, T&& value);

  const ResolverPool::Message* desc_;
  absl::flat_hash_map<int32_t, Value> fields_;
};
}  // namespace json_internal
}  // namespace DataSerialization
}  // namespace Omnira

#include "Omnira/DataSerialization/port_undef.inc"
#endif  // GOOGLE_PROTOBUF_UITL_UNTYPED_MESSAGE_H__
