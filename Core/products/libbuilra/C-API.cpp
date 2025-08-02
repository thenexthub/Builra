//===-- C-API.cpp ---------------------------------------------------------===//
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

// Include the public API.
#include <builra/builra.h>

#include "builra/Basic/Version.h"

using namespace builra;

/* Misc API */

const char* llb_get_full_version_string(void) {
  // Use a static local to store the version string, to avoid lifetime issues.
  static std::string versionString = getBuilraFullVersion();

  return versionString.c_str();
}

int llb_get_api_version(void) {
    return BUILRA_C_API_VERSION;
}
