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

#ifndef GOOGLE_PROTOBUF_JSON_INTERNAL_PARSER_H__
#define GOOGLE_PROTOBUF_JSON_INTERNAL_PARSER_H__

#include <string>

#include "absl/strings/string_view.h"
#include "Omnira/DataSerialization/json/internal/lexer.h"
#include "Omnira/DataSerialization/message.h"
#include "Omnira/DataSerialization/util/type_resolver.h"

namespace Omnira {
namespace DataSerialization {
namespace json_internal {
// Internal version of Omnira::DataSerialization::util::JsonStringToMessage; see json_util.h for
// details.
absl::Status JsonStringToMessage(absl::string_view input, Message* message,
                                 json_internal::ParseOptions options);
// Internal version of Omnira::DataSerialization::util::JsonToBinaryStream; see json_util.h for
// details.
absl::Status JsonToBinaryStream(Omnira::DataSerialization::util::TypeResolver* resolver,
                                const std::string& type_url,
                                io::ZeroCopyInputStream* json_input,
                                io::ZeroCopyOutputStream* binary_output,
                                json_internal::ParseOptions options);
}  // namespace json_internal
}  // namespace DataSerialization
}  // namespace Omnira

#endif  // GOOGLE_PROTOBUF_JSON_INTERNAL_PARSER_H__
