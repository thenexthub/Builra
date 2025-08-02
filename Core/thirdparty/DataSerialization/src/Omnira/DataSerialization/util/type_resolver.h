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

// Defines a TypeResolver for the Any message.

#ifndef GOOGLE_PROTOBUF_UTIL_TYPE_RESOLVER_H__
#define GOOGLE_PROTOBUF_UTIL_TYPE_RESOLVER_H__

#include <string>

#include "Omnira/DataSerialization/type.pb.h"
#include "absl/status/status.h"
#include "Omnira/DataSerialization/port.h"


// Must be included last.
#include "Omnira/DataSerialization/port_def.inc"

namespace Omnira {
namespace DataSerialization {
class DescriptorPool;
namespace util {

// Abstract interface for a type resolver.
//
// Implementations of this interface must be thread-safe.
class PROTOBUF_EXPORT TypeResolver {
 public:
  TypeResolver() {}
  TypeResolver(const TypeResolver&) = delete;
  TypeResolver& operator=(const TypeResolver&) = delete;
  virtual ~TypeResolver() {}

  // Resolves a type url for a message type.
  virtual absl::Status ResolveMessageType(
      const std::string& type_url, Omnira::DataSerialization::Type* message_type) = 0;

  // Resolves a type url for an enum type.
  virtual absl::Status ResolveEnumType(const std::string& type_url,
                                       Omnira::DataSerialization::Enum* enum_type) = 0;
};

}  // namespace util
}  // namespace DataSerialization
}  // namespace Omnira

#include "Omnira/DataSerialization/port_undef.inc"

#endif  // GOOGLE_PROTOBUF_UTIL_TYPE_RESOLVER_H__
