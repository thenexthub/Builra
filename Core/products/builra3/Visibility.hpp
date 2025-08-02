//===- Visibility.hpp -------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA3_VISIBILITY_H
#define BUILRA3_VISIBILITY_H

#if defined(__ELF__) || (defined(__APPLE__) && defined(__MACH__))
#define BUILRA3_EXPORT __attribute__((__visibility__("default")))
#else
// asume PE/COFF
#if defined(_WINDLL)
#define BUILRA3_EXPORT __declspec(dllexport)
#else
#define BUILRA3_EXPORT
#endif
#endif

#endif
