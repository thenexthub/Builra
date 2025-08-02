//===-- WindowsError.h - Support for mapping windows errors to posix-------===//
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

#ifndef TOOLCHAIN_SUPPORT_WINDOWSERROR_H
#define TOOLCHAIN_SUPPORT_WINDOWSERROR_H

#include <system_error>

namespace toolchain {
std::error_code mapWindowsError(unsigned EV);
}

#endif
