//===- libstdc++14-workaround.h ---------------------------------*- C++ -*-===//
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

#if defined(__cplusplus)

/* Workaround broken libstdc++ issue with C++14, see:
 *   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=51785
 *
 * We work around this by forcing basically everything to include libstdc++'s config.
 */
#include <initializer_list>
#undef _GLIBCXX_HAVE_GETS

#endif
