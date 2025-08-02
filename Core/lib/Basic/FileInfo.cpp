//===-- FileInfo.cpp ------------------------------------------------------===//
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

#include "builra/Basic/FileInfo.h"

#include "builra/Basic/Stat.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <cerrno>

using namespace builra;
using namespace builra::basic;

bool FileInfo::isDirectory() const {
  return (mode & S_IFDIR) != 0;
}

/// Get the information to represent the state of the given node in the file
/// system.
FileInfo FileInfo::getInfoForPath(const std::string& path, bool asLink) {
  FileInfo result;

  sys::StatStruct buf;
  auto statResult =
    asLink ? sys::lstat(path.c_str(), &buf) : sys::stat(path.c_str(), &buf);
  if (statResult != 0) {
    memset(&result, 0, sizeof(result));
    assert(result.isMissing());
    return result;
  }

  result.device = buf.st_dev;
  result.inode = buf.st_ino;
  result.mode = buf.st_mode;
  result.size = buf.st_size;
#if defined(__APPLE__)
  auto seconds = buf.st_mtimespec.tv_sec;
  auto nanoseconds = buf.st_mtimespec.tv_nsec;
#elif defined(_WIN32)
  auto seconds = buf.st_mtime;
  auto nanoseconds = 0;
#else
  auto seconds = buf.st_mtim.tv_sec;
  auto nanoseconds = buf.st_mtim.tv_nsec;
#endif
  result.modTime.seconds = seconds;
  result.modTime.nanoseconds = nanoseconds;

  // Enforce we never accidentally create our sentinel missing file value.
  if (result.isMissing()) {
    result.modTime.nanoseconds = 1;
    assert(!result.isMissing());
  }

  return result;
}

FileChecksum FileChecksum::getChecksumForPath(const std::string& path) {
  FileChecksum result;

  FileInfo fileInfo = FileInfo::getInfoForPath(path);
  if (fileInfo.isMissing()) {
    memset(result.bytes, 0, sizeof(result.bytes));
  } else if (fileInfo.isDirectory()) {
    result.bytes[0] = 1;
  } else {
    PlatformSpecificHasher hasher(path);
    if (hasher.readAndDigest()) {
      hasher.copy(result.bytes);
    } else {
      memset(result.bytes, 0, sizeof(result.bytes));
    }
  }

  return result;
}
