//===- MakefileDepsParser.h -------------------------------------*- C++ -*-===//
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

#ifndef BUILRA_CORE_MAKEFILEDEPSPARSER_H
#define BUILRA_CORE_MAKEFILEDEPSPARSER_H

#include "builra/Basic/toolchain.h"

#include "toolchain/ADT/StringRef.h"

#include <cstdint>

namespace builra {
namespace core {

/// Interface for parsing compiler-style dependencies output, which is a
/// restricted subset of the Makefile syntax.
class MakefileDepsParser {
public:
  /// Delegate interface for parser behavior.
  struct ParseActions {
    virtual ~ParseActions();

    /// Called if an error is encountered in parsing the input.
    ///
    /// \param message A message including information on the error.
    ///
    /// \param position The approximate position of the error in the input
    /// buffer.
    virtual void error(StringRef message, uint64_t position) = 0;

    /// Called when a new rule is encountered.
    ///
    /// \param name - The rule name.
    ///
    /// \param unescapedWord - An unescaped version of the name.
    virtual void actOnRuleStart(StringRef name, StringRef unescapedWord) = 0;

    /// Called when a new dependency is found for the current rule.
    ///
    /// This is only called between paired calls to \see actOnRuleStart() and
    /// \see actOnRuleEnd().
    ///
    /// \param dependency - The dependency string start.
    ///
    /// \param unescapedWord - An unescaped version of the dependency.
    virtual void actOnRuleDependency(StringRef dependency,
                                     StringRef unescapedWord) = 0;

    /// Called when a rule is complete.
    virtual void actOnRuleEnd() = 0;
  };

  StringRef data;
  ParseActions& actions;
  bool ignoreSubsequentOutputs;

public:
  MakefileDepsParser(StringRef data, ParseActions& actions, bool ignoreSubsequentOutputs)
    : data(data), actions(actions), ignoreSubsequentOutputs(ignoreSubsequentOutputs) {}

  void parse();
};

}
}

#endif
