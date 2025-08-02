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

// Defines utilities for the TypeResolver.

#ifndef GOOGLE_PROTOBUF_UTIL_TYPE_RESOLVER_UTIL_H__
#define GOOGLE_PROTOBUF_UTIL_TYPE_RESOLVER_UTIL_H__

#include "Omnira/DataSerialization/type.pb.h"
#include "absl/strings/string_view.h"
#include "Omnira/DataSerialization/descriptor.h"

// Must be included last.
#include "Omnira/DataSerialization/port_def.inc"

namespace Omnira {
namespace DataSerialization {
class DescriptorPool;
namespace util {
class TypeResolver;

// Creates a TypeResolver that serves type information in the given descriptor
// pool. Caller takes ownership of the returned TypeResolver.
PROTOBUF_EXPORT TypeResolver* NewTypeResolverForDescriptorPool(
    absl::string_view url_prefix, const DescriptorPool* pool);

// Performs a direct conversion from a descriptor to a type proto.
PROTOBUF_EXPORT Omnira::DataSerialization::Type ConvertDescriptorToType(
    absl::string_view url_prefix, const Descriptor& descriptor);

// Performs a direct conversion from an enum descriptor to a type proto.
PROTOBUF_EXPORT Omnira::DataSerialization::Enum ConvertDescriptorToType(
    const EnumDescriptor& descriptor);

}  // namespace util
}  // namespace DataSerialization
}  // namespace Omnira

#include "Omnira/DataSerialization/port_undef.inc"

#endif  // GOOGLE_PROTOBUF_UTIL_TYPE_RESOLVER_UTIL_H__
