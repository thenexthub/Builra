//===- PlatformUtility.h ----------------------------------------*- C++ -*-===//
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
// This file defines cross platform definitions.
//
//===----------------------------------------------------------------------===//

#ifndef CrossPlatformCompatibility_h
#define CrossPlatformCompatibility_h

#if defined(_WIN32)
// Ignore the conflicting min/max defined in windows.h
#define NOMINMAX
#include <windows.h>
#else
#include <inttypes.h>
#if __has_include(<sys/cdefs.h>)
#include <sys/cdefs.h>
#endif
#include <sys/resource.h>
#include <unistd.h>
#if defined(__linux__) || defined(__GNU__)
#include <termios.h>
#else
#include <sys/types.h>
#endif // defined(__linux__) || defined(__GNU__)
#endif // _WIN32

#if defined(_WIN32)
typedef HANDLE builra_pid_t;
typedef HANDLE FD;
typedef int builra_rlim_t;
#define PATH_MAX MAX_PATH
#else
typedef pid_t builra_pid_t;
typedef int FD;
typedef rlim_t builra_rlim_t;
#endif

#endif /* CrossPlatformCompatibility_h */
