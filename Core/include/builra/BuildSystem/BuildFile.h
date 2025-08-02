//===- BuildFile.h ----------------------------------------------*- C++ -*-===//
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
//===----------------------------------------------------------------------===//

#ifndef BUILRA_BUILDSYSTEM_BUILDFILE_H
#define BUILRA_BUILDSYSTEM_BUILDFILE_H

#include "builra/BuildSystem/BuildSystem.h"
#include "builra/Basic/Compiler.h"
#include "builra/Basic/toolchain.h"

#include "toolchain/ADT/StringRef.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace builra {
namespace basic {

class FileSystem;

}

namespace buildsystem {

/// The type used to pass parsed properties to the delegate.
typedef std::vector<std::pair<std::string, std::string>> property_list_type;

class BuildDescription;
class BuildFileDelegate;
class BuildKey;
class BuildSystem;
class BuildValue;
class Command;
class Node;
class Target;
class Tool;

/// Minimal token object representing the range where a diagnostic occurred.
struct BuildFileToken {
  const char* start;
  unsigned length;
};

/// Context for information that may be needed for a configuration action.
struct ConfigureContext {
  /// The file delegate, to use for error reporting, etc.
  BuildFileDelegate& delegate;

  /// The file the configuration request originated from.
  StringRef filename;

  /// The token to use in error reporting.
  BuildFileToken at;

public:
  BuildFileDelegate& getDelegate() const { return delegate; }

  void error(const Twine& message) const;
};

class BuildFileDelegate {
public:
  virtual ~BuildFileDelegate();

  /// Get an interned string.
  virtual StringRef getInternedString(StringRef value) = 0;
  
  /// Get the file system to use for access.
  virtual basic::FileSystem& getFileSystem() = 0;
  
  /// Called by the build file loader to register the current file contents.
  //
  // FIXME: This is a total hack, and should be cleaned up.
  virtual void setFileContentsBeingParsed(StringRef buffer) = 0;

  /// Called by the build file loader to report an error.
  ///
  /// \param filename The file the error occurred in.
  ///
  /// \param at The token at which the error occurred. The token will be null if
  /// no location is associated.
  ///
  /// \param message The diagnostic message.
  virtual void error(StringRef filename,
                     const BuildFileToken& at,
                     const Twine& message) = 0;

  /// Called by the build file loader when ownership analysis determines
  /// multiple producers exist for node.
  virtual void cannotLoadDueToMultipleProducers(Node *output,
                                                std::vector<Command*> commands) = 0;

  /// Called by the build file loader after the 'client' file section has been
  /// loaded.
  ///
  /// \param name The expected client name.
  /// \param version The client version specified in the file.
  /// \param properties The list of additional properties passed to the client.
  ///
  /// \returns True on success.
  virtual bool configureClient(const ConfigureContext&, StringRef name,
                               uint32_t version,
                               const property_list_type& properties) = 0;

  /// Called by the build file loader to get a tool definition.
  ///
  /// \param name The name of the tool to lookup.
  /// \returns The tool to use on success, or otherwise Nothing.
  virtual std::unique_ptr<Tool> lookupTool(StringRef name) = 0;

  /// Called by the build file loader to inform the client that a target
  /// definition has been loaded.
  virtual void loadedTarget(StringRef name, const Target& target) = 0;

  /// Called by the build file loader to inform the client that a default
  /// target has been loaded.
  virtual void loadedDefaultTarget(StringRef target) = 0;

  /// Called by the build file loader to inform the client that a command
  /// has been fully loaded.
  virtual void loadedCommand(StringRef name, const Command& command) = 0;

  /// Called by the build file loader to get a node.
  ///
  /// \param name The name of the node to lookup.
  ///
  /// \param isImplicit Whether the node is an implicit one (created as a side
  /// effect of being declared by a command).
  virtual std::unique_ptr<Node> createNode(StringRef name,
                                           bool isImplicit=false) = 0;
};

/// The BuildFile class supports the "builra"-native build description file
/// format.
class BuildFile {
private:
  void *impl;

public:
  /// Create a build file with the given delegate.
  ///
  /// \arg mainFilename The path of the main build file.
  explicit BuildFile(StringRef mainFilename,
                     BuildFileDelegate& delegate);
  ~BuildFile();

  /// Return the file delegate the engine was configured with.
  BuildFileDelegate* getDelegate();

  /// Load the build file from the provided filename.
  ///
  /// \returns A non-null build description on success.
  std::unique_ptr<BuildDescription> load();
};

}
}

#endif
