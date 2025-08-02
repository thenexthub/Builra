//===-- FileSystem.cpp ----------------------------------------------------===//
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

#include "builra/Basic/FileSystem.h"
#include "builra/Basic/PlatformUtility.h"
#include "builra/Basic/Stat.h"

#include "toolchain/ADT/STLExtras.h"
#include "toolchain/Support/FileSystem.h"
#include "toolchain/Support/Path.h"
#include "toolchain/Support/MemoryBuffer.h"

#include <cassert>
#include <cstring>

// Cribbed from toolchain, where it's been since removed.
namespace {
  using namespace std;
  using namespace toolchain;
  using namespace toolchain::sys::fs;

  static std::error_code fillStatus(int StatRet, const builra::basic::sys::StatStruct &Status,
                                    file_status &Result) {
    if (StatRet != 0) {
      std::error_code ec(errno, std::generic_category());
      if (ec == errc::no_such_file_or_directory)
        Result = file_status(file_type::file_not_found);
      else
        Result = file_status(file_type::status_error);
      return ec;
    }

    file_type Type = file_type::type_unknown;

    if (S_ISDIR(Status.st_mode))
      Type = file_type::directory_file;
    else if (S_ISREG(Status.st_mode))
      Type = file_type::regular_file;
    else if (S_ISBLK(Status.st_mode))
      Type = file_type::block_file;
    else if (S_ISCHR(Status.st_mode))
      Type = file_type::character_file;
    else if (S_ISFIFO(Status.st_mode))
      Type = file_type::fifo_file;
    else if (S_ISSOCK(Status.st_mode))
      Type = file_type::socket_file;
    else if (S_ISLNK(Status.st_mode))
      Type = file_type::symlink_file;

#if defined(_WIN32)
    Result = file_status(Type);
#else
    perms Perms = static_cast<perms>(Status.st_mode);
    Result =
      file_status(Type, Perms, Status.st_dev, Status.st_nlink, Status.st_ino, Status.st_atime, 0, Status.st_mtime, 0,
                Status.st_uid, Status.st_gid, Status.st_size);
#endif

    return std::error_code();
  }

  std::error_code link_status(const Twine &Path, file_status &Result) {
    SmallString<128> PathStorage;
    StringRef P = Path.toNullTerminatedStringRef(PathStorage);

    builra::basic::sys::StatStruct Status;
    int StatRet = builra::basic::sys::lstat(P.begin(), &Status);
    return fillStatus(StatRet, Status, Result);
  }

  error_code _remove_all_r(StringRef path, file_type ft, uint32_t &count) {
    if (ft == file_type::directory_file) {
      error_code ec;
      // For removal purposes, we should not follow through symlinks. Rather,
      // we just want to remove the symlink itself.
      directory_iterator i(path, ec, /* FollowSymlinks */ false);
      if (ec)
        return ec;

      for (directory_iterator e; i != e; i.increment(ec)) {
        if (ec)
          return ec;

        file_status st;

        if (error_code ec = link_status(i->path(), st))
          return ec;

        if (error_code ec = _remove_all_r(i->path(), st.type(), count))
          return ec;
      }

      if (error_code ec = remove(path, false))
        return ec;

      ++count; // Include the directory itself in the items removed.
    } else {
      if (error_code ec = remove(path, false))
        return ec;

      ++count;
    }

    return error_code();
  }
}

using namespace builra;
using namespace builra::basic;

FileSystem::~FileSystem() {}

bool FileSystem::createDirectories(const std::string& path) {
  // Attempt to create the final directory first, to optimize for the common
  // case where we don't need to recurse.
  if (createDirectory(path))
    return true;

  // If that failed, attempt to create the parent.
  StringRef parent = toolchain::sys::path::parent_path(path);
  if (parent.empty())
    return false;
  return createDirectories(parent) && createDirectory(path);
}


std::unique_ptr<toolchain::MemoryBuffer>
DeviceAgnosticFileSystem::getFileContents(const std::string& path) {
  return impl->getFileContents(path);
}

std::unique_ptr<toolchain::MemoryBuffer>
ChecksumOnlyFileSystem::getFileContents(const std::string& path) {
  return impl->getFileContents(path);
}
namespace {

class LocalFileSystem : public FileSystem {
public:
  LocalFileSystem() {}

  virtual bool
  createDirectory(const std::string& path) override {
    if (!builra::basic::sys::mkdir(path.c_str())) {
      if (errno != EEXIST) {
        return false;
      }

      // Target exists, check that it is actually a directory
      builra::basic::sys::StatStruct statbuf;
      if (builra::basic::sys::lstat(path.c_str(), &statbuf) != 0) {
        return false;
      }
      if (!S_ISDIR(statbuf.st_mode)) {
        return false;
      }
    }
    return true;
  }

  virtual std::unique_ptr<toolchain::MemoryBuffer>
  getFileContents(const std::string& path) override {
    auto result = toolchain::MemoryBuffer::getFile(path);
    if (result.getError()) {
      return nullptr;
    }
    return std::unique_ptr<toolchain::MemoryBuffer>(result->release());
  }

  bool rm_tree(const char* path) {
    uint32_t count = 0;
    return !_remove_all_r(path, file_type::directory_file, count);
  }

  virtual bool remove(const std::string& path) override {
    // Assume `path` is a regular file.
    if (builra::basic::sys::unlink(path.c_str()) == 0) {
      return true;
    }

#if defined(_WIN32)
    // Windows sets EACCES if the file is a directory
    if (errno != EACCES) {
      return false;
    }
#else
    // Error can't be that `path` is actually a directory (on Linux `EISDIR` will be returned since 2.1.132).
    if (errno != EPERM && errno != EISDIR) {
      return false;
    }
#endif

    // Check if `path` is a directory.
    builra::basic::sys::StatStruct statbuf;
    if (builra::basic::sys::lstat(path.c_str(), &statbuf) != 0) {
      return false;
    }

    if (S_ISDIR(statbuf.st_mode)) {
      if (builra::basic::sys::rmdir(path.c_str()) == 0) {
        return true;
      } else {
        return rm_tree(path.c_str());
      }
    }

    return false;
  }

  virtual FileChecksum getFileChecksum(const std::string& path) override {
    return FileChecksum::getChecksumForPath(path);
  }

  virtual FileInfo getFileInfo(const std::string& path) override {
    return FileInfo::getInfoForPath(path);
  }
  
  virtual FileInfo getLinkInfo(const std::string& path) override {
    return FileInfo::getInfoForPath(path, /*isLink:*/ true);
  }

  virtual bool createSymlink(const std::string& src,
                             const std::string& target) override {
    return (builra::basic::sys::symlink(src.c_str(), target.c_str()) == 0);
  }
};
  
}

std::unique_ptr<FileSystem> basic::createLocalFileSystem() {
  return toolchain::make_unique<LocalFileSystem>();
}

std::unique_ptr<FileSystem>
basic::DeviceAgnosticFileSystem::from(std::unique_ptr<FileSystem> fs) {
  return toolchain::make_unique<DeviceAgnosticFileSystem>(std::move(fs));
}

std::unique_ptr<FileSystem>
basic::ChecksumOnlyFileSystem::from(std::unique_ptr<FileSystem> fs) {
  return toolchain::make_unique<ChecksumOnlyFileSystem>(std::move(fs));
}
