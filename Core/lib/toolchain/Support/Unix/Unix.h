//===- toolchain/Support/Unix/Unix.h - Common Unix Include File -------*- C++ -*-===//
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
// This file defines things specific to Unix implementations.
//
//===----------------------------------------------------------------------===//

#ifndef TOOLCHAIN_LIB_SUPPORT_UNIX_UNIX_H
#define TOOLCHAIN_LIB_SUPPORT_UNIX_UNIX_H

//===----------------------------------------------------------------------===//
//=== WARNING: Implementation here must contain only generic UNIX code that
//===          is guaranteed to work on all UNIX variants.
//===----------------------------------------------------------------------===//

#include "toolchain/Config/config.h" // Get autoconf configuration settings
#include "toolchain/Support/Chrono.h"
#include "toolchain/Support/Errno.h"
#include <algorithm>
#include <assert.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif
#include <time.h>

#ifdef HAVE_DLFCN_H
# include <dlfcn.h>
#endif

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif

/// This function builds an error message into \p ErrMsg using the \p prefix
/// string and the Unix error number given by \p errnum. If errnum is -1, the
/// default then the value of errno is used.
/// Make an error message
///
/// If the error number can be converted to a string, it will be
/// separated from prefix by ": ".
static inline bool MakeErrMsg(
  std::string* ErrMsg, const std::string& prefix, int errnum = -1) {
  if (!ErrMsg)
    return true;
  if (errnum == -1)
    errnum = errno;
  *ErrMsg = prefix + ": " + toolchain::sys::StrError(errnum);
  return true;
}

namespace toolchain {
namespace sys {

/// Convert a struct timeval to a duration. Note that timeval can be used both
/// as a time point and a duration. Be sure to check what the input represents.
inline std::chrono::microseconds toDuration(const struct timeval &TV) {
  return std::chrono::seconds(TV.tv_sec) +
         std::chrono::microseconds(TV.tv_usec);
}

/// Convert a time point to struct timespec.
inline struct timespec toTimeSpec(TimePoint<> TP) {
  using namespace std::chrono;

  struct timespec RetVal;
  RetVal.tv_sec = toTimeT(TP);
  RetVal.tv_nsec = (TP.time_since_epoch() % seconds(1)).count();
  return RetVal;
}

/// Convert a time point to struct timeval.
inline struct timeval toTimeVal(TimePoint<std::chrono::microseconds> TP) {
  using namespace std::chrono;

  struct timeval RetVal;
  RetVal.tv_sec = toTimeT(TP);
  RetVal.tv_usec = (TP.time_since_epoch() % seconds(1)).count();
  return RetVal;
}

} // namespace sys
} // namespace toolchain

#endif
