//===-- Process.cpp - Implement OS Process Concept --------------*- C++ -*-===//
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
//
//  This file implements the operating system Process concept.
//
//===----------------------------------------------------------------------===//

#include "toolchain/Support/Process.h"
#include "toolchain/ADT/STLExtras.h"
#include "toolchain/ADT/StringExtras.h"
#include "toolchain/Config/toolchain-config.h"
#include "toolchain/Support/FileSystem.h"
#include "toolchain/Support/Path.h"
#include "toolchain/Support/Program.h"

using namespace toolchain;
using namespace sys;

//===----------------------------------------------------------------------===//
//=== WARNING: Implementation here must contain only TRULY operating system
//===          independent code.
//===----------------------------------------------------------------------===//

Optional<std::string> Process::FindInEnvPath(StringRef EnvName,
                                             StringRef FileName) {
  return FindInEnvPath(EnvName, FileName, {});
}

Optional<std::string> Process::FindInEnvPath(StringRef EnvName,
                                             StringRef FileName,
                                             ArrayRef<std::string> IgnoreList) {
  assert(!path::is_absolute(FileName));
  Optional<std::string> FoundPath;
  Optional<std::string> OptPath = Process::GetEnv(EnvName);
  if (!OptPath.hasValue())
    return FoundPath;

  const char EnvPathSeparatorStr[] = {EnvPathSeparator, '\0'};
  SmallVector<StringRef, 8> Dirs;
  SplitString(OptPath.getValue(), Dirs, EnvPathSeparatorStr);

  for (StringRef Dir : Dirs) {
    if (Dir.empty())
      continue;

    if (any_of(IgnoreList, [&](StringRef S) { return fs::equivalent(S, Dir); }))
      continue;

    SmallString<128> FilePath(Dir);
    path::append(FilePath, FileName);
    if (fs::exists(Twine(FilePath))) {
      FoundPath = FilePath.str();
      break;
    }
  }

  return FoundPath;
}


#define COLOR(FGBG, CODE, BOLD) "\033[0;" BOLD FGBG CODE "m"

#define ALLCOLORS(FGBG,BOLD) {\
    COLOR(FGBG, "0", BOLD),\
    COLOR(FGBG, "1", BOLD),\
    COLOR(FGBG, "2", BOLD),\
    COLOR(FGBG, "3", BOLD),\
    COLOR(FGBG, "4", BOLD),\
    COLOR(FGBG, "5", BOLD),\
    COLOR(FGBG, "6", BOLD),\
    COLOR(FGBG, "7", BOLD)\
  }

static const char colorcodes[2][2][8][10] = {
 { ALLCOLORS("3",""), ALLCOLORS("3","1;") },
 { ALLCOLORS("4",""), ALLCOLORS("4","1;") }
};

// This is set to true when Process::PreventCoreFiles() is called.
static bool coreFilesPrevented = false;

bool Process::AreCoreFilesPrevented() {
  return coreFilesPrevented;
}

// Include the platform-specific parts of this class.
#ifdef TOOLCHAIN_ON_UNIX
#include "Unix/Process.inc"
#endif
#ifdef _WIN32
#include "Windows/Process.inc"
#endif
