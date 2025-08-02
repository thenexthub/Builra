//===- unittests/BuildSystem/TempDir.cpp --------------------------------===//
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

#include "TempDir.h"

#include "builra/Basic/FileSystem.h"
#include "builra/Basic/PlatformUtility.h"

#include "toolchain/Support/FileSystem.h"
#include "toolchain/Support/Path.h"
#include "toolchain/Support/SourceMgr.h"

#if defined(_WIN32)
#include "toolchain/Support/ConvertUTF.h"
#include <windows.h>
#endif

builra::TmpDir::TmpDir(toolchain::StringRef namePrefix) {
  toolchain::SmallString<256> tempDirPrefix;
  toolchain::sys::path::system_temp_directory(true, tempDirPrefix);
  toolchain::sys::path::append(tempDirPrefix, namePrefix);

  std::error_code ec =
      toolchain::sys::fs::createUniqueDirectory(tempDirPrefix.str(), tempDir);
  assert(!ec);
  (void)ec;
}

builra::TmpDir::~TmpDir() {
#if defined(_WIN32)
  // On windows you can't delete a directory is it's your current working
  // directory. So if we're in the directory we're trying to delete, move out
  // of it first.
  wchar_t wCurrPath[MAX_PATH];
  GetCurrentDirectoryW(MAX_PATH, wCurrPath);
  toolchain::SmallVector<toolchain::UTF16, 20> wTmpPath;
  toolchain::convertUTF8ToUTF16String(str(), wTmpPath);
  if (lstrcmpW(wCurrPath, (LPCWSTR)wTmpPath.data()) == 0) {
    std::string currPath = str();
    StringRef parent = toolchain::sys::path::parent_path(currPath);
    builra::basic::sys::chdir(parent.str().c_str());
  }
#endif
  auto fs = basic::createLocalFileSystem();
  bool result = fs->remove(tempDir.c_str());
  assert(result);
  (void)result;
}

const char *builra::TmpDir::c_str() { return tempDir.c_str(); }
std::string builra::TmpDir::str() const { return tempDir.str(); }
