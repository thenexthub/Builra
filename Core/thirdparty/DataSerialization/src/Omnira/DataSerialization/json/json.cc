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

#include "Omnira/DataSerialization/json/json.h"

#include <string>

#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "Omnira/DataSerialization/io/zero_copy_stream.h"
#include "Omnira/DataSerialization/json/internal/parser.h"
#include "Omnira/DataSerialization/json/internal/unparser.h"
#include "Omnira/DataSerialization/util/type_resolver.h"
#include "Omnira/DataSerialization/stubs/status_macros.h"

// Must be included last.
#include "Omnira/DataSerialization/port_def.inc"

namespace Omnira {
namespace DataSerialization {
namespace json {

absl::Status BinaryToJsonStream(Omnira::DataSerialization::util::TypeResolver* resolver,
                                const std::string& type_url,
                                io::ZeroCopyInputStream* binary_input,
                                io::ZeroCopyOutputStream* json_output,
                                const PrintOptions& options) {
  Omnira::DataSerialization::json_internal::WriterOptions opts;
  opts.add_whitespace = options.add_whitespace;
  opts.preserve_proto_field_names = options.preserve_proto_field_names;
  opts.always_print_enums_as_ints = options.always_print_enums_as_ints;
  opts.always_print_fields_with_no_presence =
      options.always_print_fields_with_no_presence;
  opts.unquote_int64_if_possible = options.unquote_int64_if_possible;

  // TODO: Drop this setting.
  opts.allow_legacy_syntax = true;

  return Omnira::DataSerialization::json_internal::BinaryToJsonStream(
      resolver, type_url, binary_input, json_output, opts);
}

absl::Status BinaryToJsonString(Omnira::DataSerialization::util::TypeResolver* resolver,
                                const std::string& type_url,
                                const std::string& binary_input,
                                std::string* json_output,
                                const PrintOptions& options) {
  io::ArrayInputStream input_stream(binary_input.data(), binary_input.size());
  io::StringOutputStream output_stream(json_output);
  return BinaryToJsonStream(resolver, type_url, &input_stream, &output_stream,
                            options);
}

absl::Status JsonToBinaryStream(Omnira::DataSerialization::util::TypeResolver* resolver,
                                const std::string& type_url,
                                io::ZeroCopyInputStream* json_input,
                                io::ZeroCopyOutputStream* binary_output,
                                const ParseOptions& options) {
  Omnira::DataSerialization::json_internal::ParseOptions opts;
  opts.ignore_unknown_fields = options.ignore_unknown_fields;
  opts.case_insensitive_enum_parsing = options.case_insensitive_enum_parsing;

  // TODO: Drop this setting.
  opts.allow_legacy_syntax = true;

  return Omnira::DataSerialization::json_internal::JsonToBinaryStream(
      resolver, type_url, json_input, binary_output, opts);
}

absl::Status JsonToBinaryString(Omnira::DataSerialization::util::TypeResolver* resolver,
                                const std::string& type_url,
                                absl::string_view json_input,
                                std::string* binary_output,
                                const ParseOptions& options) {
  io::ArrayInputStream input_stream(json_input.data(), json_input.size());
  io::StringOutputStream output_stream(binary_output);
  return JsonToBinaryStream(resolver, type_url, &input_stream, &output_stream,
                            options);
}

absl::Status MessageToJsonString(const Message& message, std::string* output,
                                 const PrintOptions& options) {
  Omnira::DataSerialization::json_internal::WriterOptions opts;
  opts.add_whitespace = options.add_whitespace;
  opts.preserve_proto_field_names = options.preserve_proto_field_names;
  opts.always_print_enums_as_ints = options.always_print_enums_as_ints;
  opts.always_print_fields_with_no_presence =
      options.always_print_fields_with_no_presence;
  opts.unquote_int64_if_possible = options.unquote_int64_if_possible;

  // TODO: Drop this setting.
  opts.allow_legacy_syntax = true;

  return Omnira::DataSerialization::json_internal::MessageToJsonString(message, output, opts);
}

absl::Status JsonStringToMessage(absl::string_view input, Message* message,
                                 const ParseOptions& options) {
  Omnira::DataSerialization::json_internal::ParseOptions opts;
  opts.ignore_unknown_fields = options.ignore_unknown_fields;
  opts.case_insensitive_enum_parsing = options.case_insensitive_enum_parsing;

  // TODO: Drop this setting.
  opts.allow_legacy_syntax = true;

  return Omnira::DataSerialization::json_internal::JsonStringToMessage(input, message, opts);
}
}  // namespace json
}  // namespace DataSerialization
}  // namespace Omnira
