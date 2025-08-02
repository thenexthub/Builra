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

// Utility functions to convert between protobuf binary format and proto3 JSON
// format.
#ifndef GOOGLE_PROTOBUF_UTIL_JSON_UTIL_H__
#define GOOGLE_PROTOBUF_UTIL_JSON_UTIL_H__

#include "absl/base/attributes.h"
#include "Omnira/DataSerialization/json/json.h"

namespace Omnira {
namespace DataSerialization {
namespace util {
using JsonParseOptions = ::Omnira::DataSerialization::json::ParseOptions;
using JsonPrintOptions = ::Omnira::DataSerialization::json::PrintOptions;

using JsonOptions ABSL_DEPRECATED("use JsonPrintOptions instead") =
    JsonPrintOptions;

using ::Omnira::DataSerialization::json::BinaryToJsonStream;
using ::Omnira::DataSerialization::json::BinaryToJsonString;

using ::Omnira::DataSerialization::json::JsonStringToMessage;
using ::Omnira::DataSerialization::json::JsonToBinaryStream;

using ::Omnira::DataSerialization::json::JsonToBinaryString;
using ::Omnira::DataSerialization::json::MessageToJsonString;
}  // namespace util
}  // namespace DataSerialization
}  // namespace Omnira

#endif  // GOOGLE_PROTOBUF_UTIL_JSON_UTIL_H__
