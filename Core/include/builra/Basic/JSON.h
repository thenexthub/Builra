//===- JSON.h ---------------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA_BASIC_JSON_H
#define BUILRA_BASIC_JSON_H

#include <iomanip>
#include <string>
#include <sstream>

namespace builra {
namespace basic {

  template <typename String>
  std::string escapeForJSON(String const& in) {
    std::ostringstream ss;
    for (auto i = in.begin(); i != in.end(); i++) {
      switch (*i) {
      case '\\': ss << "\\\\"; break;
      case '"': ss << "\\\""; break;
      case '\b': ss << "\\b"; break;
      case '\n': ss << "\\n"; break;
      case '\f': ss << "\\f"; break;
      case '\r': ss << "\\r"; break;
      case '\t': ss << "\\t"; break;
      default:
        if ('\x00' <= *i && *i <= '\x1f') {
          ss << "\\u" << std::hex << std::setw(4) << std::setfill('0')
             << (int)*i;
        } else {
          ss << *i;
        }
      }
    }
    return ss.str();
  }

}
}

#endif
