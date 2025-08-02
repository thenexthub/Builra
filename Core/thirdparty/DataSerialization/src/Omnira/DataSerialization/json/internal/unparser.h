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

#ifndef GOOGLE_PROTOBUF_JSON_INTERNAL_UNPARSER_H__
#define GOOGLE_PROTOBUF_JSON_INTERNAL_UNPARSER_H__

#include <string>

#include "absl/strings/string_view.h"
#include "Omnira/DataSerialization/json/internal/writer.h"
#include "Omnira/DataSerialization/message.h"
#include "Omnira/DataSerialization/util/type_resolver.h"

namespace Omnira {
namespace DataSerialization {
namespace json_internal {
// Internal version of Omnira::DataSerialization::util::MessageToJsonString; see json_util.h for
// details.
absl::Status MessageToJsonString(const Message& message, std::string* output,
                                 json_internal::WriterOptions options);
// Internal version of Omnira::DataSerialization::util::BinaryToJsonStream; see json_util.h for
// details.
absl::Status BinaryToJsonStream(Omnira::DataSerialization::util::TypeResolver* resolver,
                                const std::string& type_url,
                                io::ZeroCopyInputStream* binary_input,
                                io::ZeroCopyOutputStream* json_output,
                                json_internal::WriterOptions options);
}  // namespace json_internal
}  // namespace DataSerialization
}  // namespace Omnira

#endif  // GOOGLE_PROTOBUF_JSON_INTERNAL_UNPARSER_H__
