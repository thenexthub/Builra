//===- BuildSystemExtensions.h ----------------------------------*- C++ -*-===//
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

#ifndef BUILRA_BUILDSYSTEM_BUILDSYSTEMEXTENSIONS_H
#define BUILRA_BUILDSYSTEM_BUILDSYSTEMEXTENSIONS_H

#include "builra/BuildSystem/BuildSystemHandlers.h"

#include "toolchain/ADT/StringMap.h"
#include "toolchain/ADT/StringRef.h"

#include <memory>
#include <mutex>

namespace builra {
namespace buildsystem {

class BuildSystemExtension;
class ShellCommand;

/// Management of the loading and registration of build system extensions;
///
/// NOTE: This class *is* thread-safe.
class BuildSystemExtensionManager {
  /// Mutex to protect extensions map.
  std::mutex extensionsLock;

  /// The map of discovered extensions (or nullptr, for negative lookups).
  toolchain::StringMap<std::unique_ptr<BuildSystemExtension>> extensions;
  
public:
  BuildSystemExtensionManager() {}

  /// Find a registered extension for the given command path.
  BuildSystemExtension* lookupByCommandPath(StringRef path);
};

/// A concrete build system extension.
///
/// NOTE: This class *must* be thread-safe.
class BuildSystemExtension {
public:
  explicit BuildSystemExtension() {}
  virtual ~BuildSystemExtension();

  /// Instantiate a shell command handler for the given toolpah.
  ///
  /// The build system will always create a unique handler for each build
  /// invocation; the extension should cache the handler if necessary for
  /// performance.
  virtual std::unique_ptr<ShellCommandHandler>
  createShellCommandHandler(StringRef toolPath) = 0;
};

}
}

#endif
