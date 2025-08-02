//===- DependencyInfoParser.h -----------------------------------*- C++ -*-===//
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

#ifndef BUILRA_CORE_DEPENDENCYINFOPARSER_H
#define BUILRA_CORE_DEPENDENCYINFOPARSER_H

#include "builra/Basic/toolchain.h"

#include "toolchain/ADT/StringRef.h"

#include <cstdint>

namespace builra {
namespace core {

/// Interface for parsing the "dependency info" format used by Darwin tools.
class DependencyInfoParser {
public:
  /// Delegate interface for parser behavior.
  struct ParseActions {
    virtual ~ParseActions();

    /// Called if an error is encountered in parsing the input.
    ///
    /// \param message A C-string text message including information on the
    /// error.
    ///
    /// \param position The approximate position of the error in the input
    /// buffer.
    virtual void error(const char* message, uint64_t position) = 0;

    /// Called when the version information is found.
    ///
    /// There can only ever be one version info record in the file.
    virtual void actOnVersion(StringRef) = 0;

    /// Called when an input is found.
    virtual void actOnInput(StringRef) = 0;

    /// Called when an output is found.
    virtual void actOnOutput(StringRef) = 0;

    /// Called when a missing file entry is found.
    ///
    /// These entries indicate a file which was looked for by the tool, but not
    /// found, and can be used to track anti-dependencies.
    virtual void actOnMissing(StringRef) = 0;
  };

  StringRef data;
  ParseActions& actions;
  
public:
  DependencyInfoParser(StringRef data, ParseActions& actions)
    : data(data), actions(actions) {}

  void parse();
};

}
}

#endif
