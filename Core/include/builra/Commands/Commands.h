//===- Commands.h -----------------------------------------------*- C++ -*-===//
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
// This header describes the interfaces in the Commands builra library, which
// contains all of the command line tool implementations.
//
//===----------------------------------------------------------------------===//

#ifndef BUILRA_COMMANDS_H
#define BUILRA_COMMANDS_H

#include "builra/Basic/toolchain.h"

#include "toolchain/ADT/StringRef.h"

#include <string>
#include <vector>

namespace builra {
namespace commands {

/// Register the program name.
void setProgramName(StringRef name);

/// Get the registered program name.
const char* getProgramName();

int executeNinjaCommand(const std::vector<std::string> &args);
int executeBuildEngineCommand(const std::vector<std::string> &args);
int executeBuildSystemCommand(const std::vector<std::string> &args);

}
}

#endif
