//===-- DependencyInfoParser.cpp ------------------------------------------===//
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

#include "builra/Core/DependencyInfoParser.h"

using namespace builra;
using namespace builra::core;

DependencyInfoParser::ParseActions::~ParseActions() {}

#pragma mark - DependencyInfoParser Implementation

namespace {

// This format isn't really documented anywhere other than the ld64 source,
// see, e.g.:
//
//   http://www.opensource.apple.com/source/ld64/ld64-253.3/src/ld/Options.h
//
// and:
//
//   http://www.opensource.apple.com/source/ld64/ld64-253.3/src/ld/Options.cpp

// The format itself is very simple, it is just a sequence of (<opcode>,
// <C-string operand>). The version record is required to be the first version
// in the file.

/// The known set of opcodes.
enum class Opcode : uint8_t {
  /// Version information of the producer.
  Version = 0x00,

  /// An input path.
  Input = 0x10,

  /// A missing file.
  Missing = 0x11,
    
  /// An output path,
  Output = 0x40,
};

}

void DependencyInfoParser::parse() {
  // Validate that the file ends with a null byte, if not it is malformed and we
  // just bail on parsing.
  if (!data.endswith(StringRef("\0", 1))) {
    actions.error("missing null terminator", data.size());
    return;
  }

  // Validate that the file starts with the version record.
  if (Opcode(data[0]) != Opcode::Version) {
    actions.error("missing version record", 0);
    return;
  }

  // Parse the records in turn.
  const char* cur = data.begin();
  const char* end = data.end();
  while (cur != end) {
    const char* opcodeStart = cur;
    auto opcode = Opcode(*cur++);
    const char* operandStart = cur;
    while (*cur != '\0') {
      ++cur;
      // We require the file have a terminating null, so we can never scan past
      // the end.
      assert (cur < end);
    }
    const char* operandEnd = cur;
    assert(*operandEnd == '\x00' && cur < end);
    ++cur;

    // Diagnose empty operands (missing isn't possible.
    auto operand = StringRef(operandStart, operandEnd - operandStart);
    if (operand.empty()) {
      actions.error("empty operand", opcodeStart - data.data());
      break;
    }

    // Handle the opcode.
    switch (opcode) {
    case Opcode::Version: {
      // Diagnose duplicate version records.
      if (opcodeStart != data.begin()) {
        actions.error("invalid duplicate version", opcodeStart - data.data());
        break;
      }
      actions.actOnVersion(operand);
      break;
    }

    case Opcode::Input: {
      actions.actOnInput(operand);
      break;
    }

    case Opcode::Missing: {
      actions.actOnMissing(operand);
      break;
    }

    case Opcode::Output: {
      actions.actOnOutput(operand);
      break;
    }

    default:
      actions.error("unknown opcode in file", opcodeStart - data.data());
      break;
    }
  }
}
