//===- ManifestLoader.h ----------------------------------------*- C++ -*-===//
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

#ifndef BUILRA_NINJA_MANIFESTLOADER_H
#define BUILRA_NINJA_MANIFESTLOADER_H

#include "builra/Basic/toolchain.h"
#include "builra/Ninja/Manifest.h"

#include "toolchain/ADT/StringRef.h"

#include <memory>

namespace toolchain {
class MemoryBuffer;
}

namespace builra {
namespace ninja {

class Manifest;
class ManifestLoader;
class Parser;
struct Token;

/// Delegate interface for loader behavior.
class ManifestLoaderActions {
public:
  virtual ~ManifestLoaderActions();

  /// Called at the beginning of loading, to register the manifest loader.
  virtual void initialize(ManifestLoader* loader) = 0;

  virtual void error(StringRef filename, StringRef message, const Token& at) = 0;

  /// Called by the loader to request the contents of a manifest file be loaded.
  ///
  /// \param path Absolute path of the file to load.
  ///
  /// \param forFilename If non-empty, the name of the file triggering the file
  /// load (for use in diagnostics).
  ///
  /// \param forToken If non-null, the token triggering the file load (for use
  /// in diagnostics).
  ///
  /// \returns The loaded file on success, or a nullptr. On failure, the action
  /// is assumed to have produced an appropriate error.
  virtual std::unique_ptr<toolchain::MemoryBuffer> readFile(
      StringRef path, StringRef forFilename, const Token* forToken) = 0;
};

/// Interface for loading Ninja build manifests.
class ManifestLoader {
  class ManifestLoaderImpl;
  std::unique_ptr<ManifestLoaderImpl> impl;

public:
  ManifestLoader(StringRef workingDirectory, StringRef mainFilename,
                 ManifestLoaderActions& actions);
  ~ManifestLoader();

  /// Load the manifest.
  std::unique_ptr<Manifest> load();

  /// Get the current underlying manifest parser.
  const Parser* getCurrentParser() const;
};

}
}

#endif
