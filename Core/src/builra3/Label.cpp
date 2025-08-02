//===- Label.cpp ------------------------------------------------*- C++ -*-===//
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

#include "builra3/Label.h"

namespace builra3 {

std::string labelAsCanonicalString(const Label& label) {
  std::string result("//");

  // FIXME: Should this allow empty components?

  for (int i = 0; i < label.components_size(); i++) {
    auto component = label.components(i);

    // add separator for subsequent components
    if (i > 0) {
      result.append("/");
    }
    result.append(component);
  }

  if (label.name().size() > 0) {
    result.append(":");
    result.append(label.name());
  }

  return result;
}

} // namespace builra3
