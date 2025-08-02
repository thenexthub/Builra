//===- CommandUtil.h --------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA_NINJA_COMMANDUTIL_H
#define BUILRA_NINJA_COMMANDUTIL_H

#include "builra/Basic/toolchain.h"

#include "toolchain/ADT/StringRef.h"
#include "toolchain/Support/Error.h"

#include <memory>
#include <string>
#include <utility>

namespace toolchain {
class MemoryBuffer;
}

namespace builra {
namespace ninja {

class Parser;
struct Token;

}

// FIXME: Move all of these things to a real home.
namespace commands {
namespace util {

std::string escapedString(StringRef str);

void emitError(StringRef filename, StringRef message,
               const ninja::Token& at, const ninja::Parser* parser);

void emitError(StringRef filename, StringRef message,
               const char* position, unsigned length,
               StringRef buffer);

/// Load the contents of the given file. Relative files will be resolved using
/// the current working directory of the process.
toolchain::Expected<std::unique_ptr<toolchain::MemoryBuffer>> readFileContents(
    StringRef filename);

}
}
}

#endif
