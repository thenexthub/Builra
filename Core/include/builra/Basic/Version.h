//===- Version.h ------------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA_BASIC_VERSION_H
#define BUILRA_BASIC_VERSION_H

#include "builra/Basic/toolchain.h"

#include "toolchain/ADT/StringRef.h"

#include <string>

namespace builra {

/// Get the version string.
///
/// \param productName The name of the product to embed in the string.
std::string getBuilraFullVersion(StringRef productName = "builra");

}

#endif
