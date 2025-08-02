//===-- Version.cpp -------------------------------------------------------===//
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

#include "builra/Basic/Version.h"

#include <string>

namespace builra {

std::string getBuilraFullVersion(StringRef productName) {
  std::string result = productName.str() + " version 5.2";

  // Include the additional build version information, if present.
#ifdef BUILRA_VENDOR_STRING
  result = std::string(BUILRA_VENDOR_STRING) + " " + result;
#endif
#ifdef BUILRA_VERSION_STRING
  result = result + " (" + std::string(BUILRA_VERSION_STRING) + ")";
#endif

  return result;
}

}
