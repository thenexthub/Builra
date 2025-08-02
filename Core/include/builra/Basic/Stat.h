//===- Stat.h -------------------------------------------------------------===//
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

#ifndef BUILRA_BASIC_STAT_H
#define BUILRA_BASIC_STAT_H

#include <sys/stat.h>

#include "toolchain/Support/FileSystem.h"

namespace builra {
namespace basic {
namespace sys {

#if !defined(S_IFBLK)
#define S_IFBLK 0060000
#endif
#if !defined(S_IFIFO)
#define S_IFIFO 0010000
#endif
#if !defined(S_IFSOCK)
#define S_IFSOCK 00140000
#endif
#if !defined(S_IFLNK)
#define S_IFLNK 0120000
#endif

#if !defined(S_ISREG)
#define S_ISREG(mode) (((mode) & _S_IFMT) == S_IFREG)
#endif

#if !defined(S_ISDIR)
#define S_ISDIR(mode) (((mode) & _S_IFMT) == S_IFDIR)
#endif

#if !defined(S_ISBLK)
#define S_ISBLK(mode) (((mode) & _S_IFMT) == S_IFBLK)
#endif

#if !defined(S_ISCHR)
#define S_ISCHR(mode) ((mode) & _S_IFMT) == S_IFCHR
#endif

#if !defined(S_ISFIFO)
#define S_ISFIFO(mode) ((mode) & _S_IFMT) == S_IFIFO
#endif

#if !defined(S_ISSOCK)
#define S_ISSOCK(mode) ((mode) & _S_IFMT) == S_IFSOCK
#endif

#if !defined(S_ISLNK)
#define S_ISLNK(mode) ((mode) & _S_IFMT) == S_IFLNK
#endif

#if defined(_WIN32)
using StatStruct = struct ::_stat;
#else
using StatStruct = struct ::stat;
#endif

int lstat(const char *fileName, StatStruct *buf);
int stat(const char *fileName, StatStruct *buf);
}
}
}

#endif // BUILRA_BASIC_STAT_H
