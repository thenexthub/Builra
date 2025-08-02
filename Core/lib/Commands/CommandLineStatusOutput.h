//===- CommandLineStatusOutput.h --------------------------------*- C++ -*-===//
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

#ifndef BUILRA_COMMANDS_COMMANDLINESTATUSOUTPUT_H
#define BUILRA_COMMANDS_COMMANDLINESTATUSOUTPUT_H

#include <string>

namespace builra {
namespace commands {

/// Utility class for writing out progress or status information to a terminal
/// which abstracts out support for ANSI compliant terminals.
class CommandLineStatusOutput {
  void *impl;

public:
  CommandLineStatusOutput();

  ~CommandLineStatusOutput();

  /// Open the output stream for writing.
  bool open(std::string* error_out);

  /// Close the output stream and clear any incomplete output.
  bool close(std::string* error_out);

  /// Check if the attached output device can update (rewrite) the current line.
  bool canUpdateCurrentLine() const;

  /// Clear the current output.
  ///
  /// This requires that \see canUpdateCurrentLine() is true.
  void clearOutput();

  /// Update the current line of output text.
  void setCurrentLine(const std::string& text);

  /// Update the current line of output text, if possible, otherwise simply
  /// write it out.
  ///
  /// The text should be a single line with no newlines or carriage returns.
  void setOrWriteLine(const std::string& text);

  /// Finish writing the current line, if necessary.
  void finishLine();

  /// Write a non-overwritable block of text to the output.
  ///
  /// Any text written by this method should always end with a newline.
  void writeText(std::string&& text);

  /// Destructively strip the ANSI codes if the terminal can not support them.
  void stripColorCodes(std::string& str);
};

}
}

#endif
