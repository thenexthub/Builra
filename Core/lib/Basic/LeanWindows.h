//===- LeanWindows.h --------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA_BASIC_LEANWINDOWS_H
#define BUILRA_BASIC_LEANWINDOWS_H

#if !defined(_WIN32)
#error "LeanWindows.h should only be included on Windows."
#endif

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif
