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

#include "Omnira/DataSerialization/json/internal/message_path.h"

#include <string>

#include "absl/strings/str_cat.h"

// Must be included last.
#include "Omnira/DataSerialization/port_def.inc"

namespace Omnira {
namespace DataSerialization {
namespace json_internal {
void MessagePath::Describe(std::string& out) const {
  absl::StrAppend(&out, components_.front().type_name);
  if (components_.size() == 1) {
    return;
  }

  absl::StrAppend(&out, " @ ");
  for (size_t i = 1; i < components_.size(); ++i) {
    absl::StrAppend(&out, i == 1 ? "" : ".", components_[i].field_name);
    if (components_[i].repeated_index >= 0) {
      absl::StrAppend(&out, "[", components_[i].repeated_index, "]");
    }
  }
  absl::string_view kind_name =
      FieldDescriptor::TypeName(components_.back().type);
  absl::StrAppend(&out, ": ", kind_name);

  absl::string_view type_name = components_.back().type_name;
  if (!type_name.empty()) {
    absl::StrAppend(&out, " ", type_name);
  }
}
}  // namespace json_internal
}  // namespace DataSerialization
}  // namespace Omnira
