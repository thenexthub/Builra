//===- Tool.h ---------------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA_BUILDSYSTEM_TOOL_H
#define BUILRA_BUILDSYSTEM_TOOL_H

#include "builra/Basic/Compiler.h"

#include "toolchain/ADT/StringRef.h"

#include <utility>

namespace builra {
namespace buildsystem {

class BuildKey;
class Command;

/// Context for information that may be needed for a configuration action.
//
// FIXME: This is currently commingled with the build file loading, even though
// it should ideally be possible to create a build description decoupled
// entirely from the build file representation.
struct ConfigureContext;

/// Abstract tool definition used by the build file.
class Tool {
  // DO NOT COPY
  Tool(const Tool&) BUILRA_DELETED_FUNCTION;
  void operator=(const Tool&) BUILRA_DELETED_FUNCTION;
  Tool &operator=(Tool&& rhs) BUILRA_DELETED_FUNCTION;
    
  std::string name;

public:
  explicit Tool(StringRef name) : name(name) {}
  virtual ~Tool();

  StringRef getName() const { return name; }

  /// Called by the build file loader to configure a specified tool property.
  virtual bool configureAttribute(const ConfigureContext&, StringRef name,
                                  StringRef value) = 0;
  /// Called by the build file loader to configure a specified tool property.
  virtual bool configureAttribute(const ConfigureContext&, StringRef name,
                                  ArrayRef<StringRef> values) = 0;
  /// Called by the build file loader to configure a specified node property.
  virtual bool configureAttribute(
      const ConfigureContext&, StringRef name,
      ArrayRef<std::pair<StringRef, StringRef>> values) = 0;

  /// Called by the build file loader to create a command which uses this tool.
  ///
  /// \param name - The name of the command.
  virtual std::unique_ptr<Command> createCommand(StringRef name) = 0;

  /// Called by the build system to create a custom command with the given name.
  ///
  /// The tool should return null if it does not understand how to create the
  /// a custom command for the given key.
  ///
  /// \param key - The custom build key to create a command for.
  /// \returns The command to use, or null.
  virtual std::unique_ptr<Command> createCustomCommand(const BuildKey& key);
};

}
}

#endif
