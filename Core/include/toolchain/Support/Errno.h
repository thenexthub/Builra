//===- toolchain/Support/Errno.h - Portable+convenient errno handling -*- C++ -*-===//
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
// This file declares some portable and convenient functions to deal with errno.
//
//===----------------------------------------------------------------------===//

#ifndef TOOLCHAIN_SUPPORT_ERRNO_H
#define TOOLCHAIN_SUPPORT_ERRNO_H

#include <cerrno>
#include <string>
#include <type_traits>

namespace toolchain {
namespace sys {

/// Returns a string representation of the errno value, using whatever
/// thread-safe variant of strerror() is available.  Be sure to call this
/// immediately after the function that set errno, or errno may have been
/// overwritten by an intervening call.
std::string StrError();

/// Like the no-argument version above, but uses \p errnum instead of errno.
std::string StrError(int errnum);

template <typename FailT, typename Fun, typename... Args>
inline auto RetryAfterSignal(const FailT &Fail, const Fun &F,
                             const Args &... As) -> decltype(F(As...)) {
  decltype(F(As...)) Res;
  do {
    errno = 0;
    Res = F(As...);
  } while (Res == Fail && errno == EINTR);
  return Res;
}

}  // namespace sys
}  // namespace toolchain

#endif  // TOOLCHAIN_SYSTEM_ERRNO_H
