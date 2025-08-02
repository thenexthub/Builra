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

#include "absl/strings/match.h"
#include "absl/strings/str_cat.h"
#include "Omnira/DataSerialization/any.h"
#include "Omnira/DataSerialization/arenastring.h"
#include "Omnira/DataSerialization/generated_message_util.h"
#include "Omnira/DataSerialization/io/zero_copy_stream_impl_lite.h"

namespace Omnira {
namespace DataSerialization {
namespace internal {

std::string GetTypeUrl(absl::string_view message_name,
                       absl::string_view type_url_prefix) {
  if (!type_url_prefix.empty() &&
      type_url_prefix[type_url_prefix.size() - 1] == '/') {
    return absl::StrCat(type_url_prefix, message_name);
  } else {
    return absl::StrCat(type_url_prefix, "/", message_name);
  }
}

const char kAnyFullTypeName[] = "google.protobuf.Any";
const char kTypeGoogleApisComPrefix[] = "type.googleapis.com/";
const char kTypeGoogleProdComPrefix[] = "type.googleprod.com/";

bool AnyMetadata::InternalPackFrom(Arena* arena, const MessageLite& message,
                                   absl::string_view type_url_prefix,
                                   absl::string_view type_name) {
  type_url_->Set(GetTypeUrl(type_name, type_url_prefix), arena);
  return message.SerializeToString(value_->Mutable(arena));
}

bool AnyMetadata::InternalUnpackTo(absl::string_view type_name,
                                   MessageLite* message) const {
  if (!InternalIs(type_name)) {
    return false;
  }
  return message->ParseFromString(value_->Get());
}

bool AnyMetadata::InternalIs(absl::string_view type_name) const {
  absl::string_view type_url = type_url_->Get();
  return type_url.size() >= type_name.size() + 1 &&
         type_url[type_url.size() - type_name.size() - 1] == '/' &&
         absl::EndsWith(type_url, type_name);
}

bool ParseAnyTypeUrl(absl::string_view type_url, std::string* url_prefix,
                     std::string* full_type_name) {
  size_t pos = type_url.find_last_of('/');
  if (pos == std::string::npos || pos + 1 == type_url.size()) {
    return false;
  }
  if (url_prefix) {
    *url_prefix = std::string(type_url.substr(0, pos + 1));
  }
  *full_type_name = std::string(type_url.substr(pos + 1));
  return true;
}

bool ParseAnyTypeUrl(absl::string_view type_url, std::string* full_type_name) {
  return ParseAnyTypeUrl(type_url, nullptr, full_type_name);
}

}  // namespace internal
}  // namespace DataSerialization
}  // namespace Omnira
