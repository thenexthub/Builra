//===- FileSystem.h ---------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA_BASIC_FILESYSTEM_H
#define BUILRA_BASIC_FILESYSTEM_H

#include "builra/Basic/Compiler.h"
#include "builra/Basic/FileInfo.h"
#include "builra/Basic/toolchain.h"

#include "toolchain/Support/ErrorOr.h"

#include <memory>
#ifndef _WIN32
#include <unistd.h>
#endif

namespace toolchain {

class MemoryBuffer;

}

namespace builra {
namespace basic {

// Abstract interface for interacting with a file system. This allows mocking of
// operations for testing, and for clients to provide virtualized interfaces.
class FileSystem  {
  // DO NOT COPY
  FileSystem(const FileSystem&) BUILRA_DELETED_FUNCTION;
  void operator=(const FileSystem&) BUILRA_DELETED_FUNCTION;
  FileSystem &operator=(FileSystem&& rhs) BUILRA_DELETED_FUNCTION;

public:
  FileSystem() {}
  virtual ~FileSystem();

  /// Create the given directory if it does not exist.
  ///
  /// \returns True on success (the directory was created, or already exists).
  virtual bool
  createDirectory(const std::string& path) = 0;

  /// Create the given directory (recursively) if it does not exist.
  ///
  /// \returns True on success (the directory was created, or already exists).
  virtual bool
  createDirectories(const std::string& path);

  /// Get a memory buffer for a given file on the file system.
  ///
  /// \returns The file contents, on success, or null on error.
  virtual std::unique_ptr<toolchain::MemoryBuffer>
  getFileContents(const std::string& path) = 0;

  /// Remove the file or directory at the given path.
  ///
  /// Directory removal is recursive.
  ///
  /// \returns True if the item was removed, false otherwise.
  virtual bool remove(const std::string& path) = 0;

  /// Get checksum of the given path in the file system.
  ///
  /// \returns The FileChecksum for the given path, which will be missing if the
  /// path does not exist (or any error was encountered).
  virtual FileChecksum getFileChecksum(const std::string& path) = 0;

  /// Get the information to represent the state of the given path in the file
  /// system.
  ///
  /// \returns The FileInfo for the given path, which will be missing if the
  /// path does not exist (or any error was encountered).
  virtual FileInfo getFileInfo(const std::string& path) = 0;

  /// Get the information to represent the state of the given path in the file
  /// system, without looking through symbolic links.
  ///
  /// \returns The FileInfo for the given path, which will be missing if the
  /// path does not exist (or any error was encountered).
  virtual FileInfo getLinkInfo(const std::string& path) = 0;

  /// Create a symbolic link
  ///
  /// \returns True on success (the symlink was created)
  virtual bool createSymlink(const std::string& src, const std::string& target) = 0;
};

/// Create a FileSystem instance suitable for accessing the local filesystem.
std::unique_ptr<FileSystem> createLocalFileSystem();


/// Device/inode agnostic filesystem wrapper
class DeviceAgnosticFileSystem : public FileSystem {
private:
  std::unique_ptr<FileSystem> impl;

public:
  explicit DeviceAgnosticFileSystem(std::unique_ptr<FileSystem> fs)
    : impl(std::move(fs))
  {
  }

  DeviceAgnosticFileSystem(const FileSystem&) BUILRA_DELETED_FUNCTION;
  void operator=(const DeviceAgnosticFileSystem&) BUILRA_DELETED_FUNCTION;
  DeviceAgnosticFileSystem &operator=(DeviceAgnosticFileSystem&& rhs) BUILRA_DELETED_FUNCTION;

  static std::unique_ptr<FileSystem> from(std::unique_ptr<FileSystem> fs);


  virtual bool
  createDirectory(const std::string& path) override {
    return impl->createDirectory(path);
  }

  virtual bool
  createDirectories(const std::string& path) override {
    return impl->createDirectories(path);
  }

  virtual std::unique_ptr<toolchain::MemoryBuffer>
  getFileContents(const std::string& path) override;

  virtual bool remove(const std::string& path) override {
    return impl->remove(path);
  }

  virtual FileChecksum getFileChecksum(const std::string& path) override {
    return impl->getFileChecksum(path);
  }

  virtual FileInfo getFileInfo(const std::string& path) override {
    auto info = impl->getFileInfo(path);

    // Device and inode numbers may (will) change when relocating files to
    // another device. Here we explicitly, unconditionally override them with 0,
    // enabling an entire build tree to be relocated or copied yet retain the
    // ability to perform incremental builds.
    info.device = 0;
    info.inode = 0;

    return info;
  }

  virtual FileInfo getLinkInfo(const std::string& path) override {
    auto info = impl->getLinkInfo(path);

    info.device = 0;
    info.inode = 0;

    return info;
  }

  virtual bool createSymlink(const std::string& src, const std::string& target) override {
    return impl->createSymlink(src, target);
  }
};

/// Checksum-only filesystem wrapper
class ChecksumOnlyFileSystem : public FileSystem {
private:
  std::unique_ptr<FileSystem> impl;

public:
  explicit ChecksumOnlyFileSystem(std::unique_ptr<FileSystem> fs)
    : impl(std::move(fs))
  {
  }

  ChecksumOnlyFileSystem(const FileSystem&) BUILRA_DELETED_FUNCTION;
  void operator=(const ChecksumOnlyFileSystem&) BUILRA_DELETED_FUNCTION;
  ChecksumOnlyFileSystem &operator=(ChecksumOnlyFileSystem&& rhs) BUILRA_DELETED_FUNCTION;

  static std::unique_ptr<FileSystem> from(std::unique_ptr<FileSystem> fs);


  virtual bool
  createDirectory(const std::string& path) override {
    return impl->createDirectory(path);
  }

  virtual bool
  createDirectories(const std::string& path) override {
    return impl->createDirectories(path);
  }

  virtual std::unique_ptr<toolchain::MemoryBuffer>
  getFileContents(const std::string& path) override;

  virtual bool remove(const std::string& path) override {
    return impl->remove(path);
  }

  virtual FileChecksum getFileChecksum(const std::string& path) override {
    return impl->getFileChecksum(path);
  }

  virtual FileInfo getFileInfo(const std::string& path) override {
    auto info = impl->getFileInfo(path);

    info.device = 0;
    info.inode = 0;
    info.modTime = FileTimestamp();
    info.modTime.seconds = 0;
    info.modTime.nanoseconds = 0;

    info.checksum = impl->getFileChecksum(path);

    return info;
  }

  virtual FileInfo getLinkInfo(const std::string& path) override {
    auto info = impl->getLinkInfo(path);

    info.device = 0;
    info.inode = 0;
    info.modTime = FileTimestamp();
    info.modTime.seconds = 0;
    info.modTime.nanoseconds = 0;

#ifndef _WIN32
    char buff[PATH_MAX];
    ssize_t len = ::readlink(path.c_str(), buff, sizeof(buff)-1);
    if (len != -1) {
      buff[len] = '\0';
      PlatformSpecificHasher(std::string(buff)).readPathStringAndDigest(info.checksum);
    } else {
      info.checksum = {0};
    }
#else
    info.checksum = impl->getFileChecksum(path);
#endif
    return info;
  }

  virtual bool createSymlink(const std::string& src, const std::string& target) override {
    return impl->createSymlink(src, target);
  }
};

}
}

#endif
