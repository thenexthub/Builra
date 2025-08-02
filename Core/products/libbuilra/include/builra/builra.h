//===- builra.h --------------------------------------------------*- C -*-===//
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
// These are the C API interfaces to the builra library.
//
//===----------------------------------------------------------------------===//

#ifndef BUILRA_PUBLIC_BUILRA_H
#define BUILRA_PUBLIC_BUILRA_H

#include "builra-defines.h"

/// Get the full version of the builra library.
BUILRA_EXPORT const char* llb_get_full_version_string(void);

/// Get the C API version number.
BUILRA_EXPORT int llb_get_api_version(void);

// The Core component.
#include "core.h"

// The BuildSystem component.
#include "buildsystem.h"

// The Database component.
#include "db.h"

#include "buildkey.h"
#include "buildvalue.h"

#include "ninja.h"

#endif
