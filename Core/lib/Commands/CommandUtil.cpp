//===-- CommandUtil.cpp ---------------------------------------------------===//
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

#include "CommandUtil.h"
#include "builra/Commands/Commands.h"

#include "builra/Basic/toolchain.h"
#include "builra/Ninja/ManifestLoader.h"
#include "builra/Ninja/Parser.h"

#include "toolchain/ADT/STLExtras.h"
#include "toolchain/ADT/Twine.h"
#include "toolchain/Support/FileSystem.h"
#include "toolchain/Support/MemoryBuffer.h"
#include "toolchain/Support/raw_ostream.h"

#include <iostream>

using namespace builra;
using namespace builra::commands;

static std::string programName;

void commands::setProgramName(StringRef name) {
  assert(programName.empty());
  programName = name;
}

const char* commands::getProgramName() {
  if (programName.empty())
    return nullptr;
  
  return programName.c_str();
}

static char hexdigit(unsigned input) {
  return (input < 10) ? '0' + input : 'A' + input - 10;
}

std::string util::escapedString(StringRef str) {
  std::string result;
  toolchain::raw_string_ostream resultStream(result);
  for (unsigned i = 0; i < str.size(); ++i) {
    char c = str[i];
    if (c == '"') {
      resultStream << "\\\"";
    } else if (isprint(c)) {
      resultStream << c;
    } else if (c == '\n') {
      resultStream << "\\n";
    } else {
      resultStream << "\\x"
             << hexdigit(((unsigned char) c >> 4) & 0xF)
             << hexdigit((unsigned char) c & 0xF);
    }
  }
  resultStream.flush();
  return result;
}

static void emitError(StringRef filename, StringRef message,
                      const char* position, unsigned length,
                      int line, int column,
                      StringRef buffer) {
  assert(position >= buffer.begin() && position <= buffer.end() &&
         "invalid position");
  assert(position + length <= buffer.end() && "invalid length");

  // Compute the line and column, if not provided.
  //
  // FIXME: This is not very efficient, if there are a lot of diagnostics.
  if (line == -1) {
    line = 1;
    column = 0;
    for (const char *c = buffer.begin(); c != position; ++c) {
      if (*c == '\n') {
        ++line;
        column = 0;
      } else {
        ++column;
      }
    }
  }

  std::cerr.write(filename.data(), filename.size());
  std::cerr << ":" << line << ":" << column << ": error: ";
  std::cerr.write(message.data(), message.size());
  std::cerr << "\n";

  // Skip carat diagnostics on EOF token.
  if (position == buffer.end())
    return;

  // Simple caret style diagnostics.
  const char *lineBegin = position, *lineEnd = position;
  const char *bufferBegin = buffer.begin(), *bufferEnd = buffer.end();

  // Run line pointers forward and back.
  while (lineBegin > bufferBegin &&
         lineBegin[-1] != '\r' && lineBegin[-1] != '\n')
    --lineBegin;
  while (lineEnd < bufferEnd &&
         lineEnd[0] != '\r' && lineEnd[0] != '\n')
    ++lineEnd;

  // Show the line, indented by 2.
  std::cerr << "  " << std::string(lineBegin, lineEnd) << "\n";

  // Show the caret or squiggly, making sure to print back spaces the same.
  std::cerr << "  ";
  for (const char* s = lineBegin; s != position; ++s)
    std::cerr << (isspace(*s) ? *s : ' ');
  if (length > 1) {
    for (unsigned i = 0; i != length; ++i)
      std::cerr << '~';
  } else {
    std::cerr << '^';
  }
  std::cerr << '\n';
}

void util::emitError(StringRef filename, StringRef message,
                     const ninja::Token& at, const ninja::Parser* parser) {
  ::emitError(filename, message, at.start, at.length, at.line, at.column,
            parser->getLexer().getBuffer());
}

void util::emitError(StringRef filename, StringRef message,
                     const char* position, unsigned length,
                     StringRef buffer) {
  ::emitError(filename, message, position, length, -1, -1, buffer);
}

toolchain::Expected<std::unique_ptr<toolchain::MemoryBuffer>> util::readFileContents(
    StringRef filename) {
  SmallString<256> path(filename);
  toolchain::sys::fs::make_absolute(path);

  auto bufferOrError = toolchain::MemoryBuffer::getFile(path);
  if (!bufferOrError) {
    auto ec = bufferOrError.getError();
    return toolchain::make_error<toolchain::StringError>(
        Twine("unable to read input \"") + util::escapedString(path) +
        "\" (" + ec.message() + ")", ec);
  }
  return std::move(*bufferOrError);
}
