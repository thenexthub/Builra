//===- toolchain/Support/Unix/Host.inc -------------------------------*- C++ -*-===//
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
//
// This file implements the UNIX Host support.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//=== WARNING: Implementation here must contain only generic UNIX code that
//===          is guaranteed to work on *all* UNIX variants.
//===----------------------------------------------------------------------===//

#include "Unix.h"
#include "toolchain/ADT/StringRef.h"
#include "toolchain/Config/config.h"
#include <cctype>
#include <string>
#include <sys/utsname.h>

using namespace toolchain;

static std::string getOSVersion() {
  struct utsname info;

  if (uname(&info))
    return "";

  return info.release;
}

static std::string updateTripleOSVersion(std::string TargetTripleString) {
  // On darwin, we want to update the version to match that of the target.
  std::string::size_type DarwinDashIdx = TargetTripleString.find("-darwin");
  if (DarwinDashIdx != std::string::npos) {
    TargetTripleString.resize(DarwinDashIdx + strlen("-darwin"));
    TargetTripleString += getOSVersion();
    return TargetTripleString;
  }
  std::string::size_type MacOSDashIdx = TargetTripleString.find("-macos");
  if (MacOSDashIdx != std::string::npos) {
    TargetTripleString.resize(MacOSDashIdx);
    // Reset the OS to darwin as the OS version from `uname` doesn't use the
    // macOS version scheme.
    TargetTripleString += "-darwin";
    TargetTripleString += getOSVersion();
  }
  return TargetTripleString;
}

std::string sys::getDefaultTargetTriple() {
  std::string TargetTripleString =
      updateTripleOSVersion(TOOLCHAIN_DEFAULT_TARGET_TRIPLE);

  // Override the default target with an environment variable named by
  // TOOLCHAIN_TARGET_TRIPLE_ENV.
#if defined(TOOLCHAIN_TARGET_TRIPLE_ENV)
  if (const char *EnvTriple = std::getenv(TOOLCHAIN_TARGET_TRIPLE_ENV))
    TargetTripleString = EnvTriple;
#endif

  return TargetTripleString;
}
