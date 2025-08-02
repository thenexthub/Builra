//===- Memory.cpp - Memory Handling Support ---------------------*- C++ -*-===//
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
// This file defines some helpful functions for allocating memory and dealing
// with memory mapped files
//
//===----------------------------------------------------------------------===//

#include "toolchain/Support/Memory.h"
#include "toolchain/Config/toolchain-config.h"
#include "toolchain/Support/Valgrind.h"

// Include the platform-specific parts of this class.
#ifdef TOOLCHAIN_ON_UNIX
#include "Unix/Memory.inc"
#endif
#ifdef _WIN32
#include "Windows/Memory.inc"
#endif
