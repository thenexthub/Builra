//===- ShellUtility.h -------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA_BASIC_SHELLUTILITY_H
#define BUILRA_BASIC_SHELLUTILITY_H

#include "toolchain/ADT/StringRef.h"
#include "toolchain/Support/raw_ostream.h"
#include <vector>

namespace builra {
namespace basic {

#if defined(_WIN32)
/// Formats a command line using the Windows command line escaping rules a la a
/// reverse CommandLineToArgVW
///
/// \param args The arugments to escape
///
std::string formatWindowsCommandString(std::vector<std::string> args);
#endif

/// Appends a shell escaped string to an output stream.
/// For e.g. hello -> hello, hello$world -> 'hello$world', input A -> 'input A'
///
/// \param os Reference of the output stream to append to.
///
/// \param string The string to be escaped and appended.
///
void appendShellEscapedString(toolchain::raw_ostream& os, toolchain::StringRef string);

/// Creates and returns a shell escaped string of the input.
///
/// \param string The string to be escaped.
///
/// \returns escaped string.
std::string shellEscaped(toolchain::StringRef string);

}
}

#endif
