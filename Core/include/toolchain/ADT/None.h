//===-- None.h - Simple null value for implicit construction ------*- C++ -*-=//
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
//  This file provides None, an enumerator for use in implicit constructors
//  of various (usually templated) types to make such construction more
//  terse.
//
//===----------------------------------------------------------------------===//

#ifndef TOOLCHAIN_ADT_NONE_H
#define TOOLCHAIN_ADT_NONE_H

namespace toolchain {
/// A simple null object to allow implicit construction of Optional<T>
/// and similar types without having to spell out the specialization's name.
// (constant value 1 in an attempt to workaround MSVC build issue... )
enum class NoneType { None = 1 };
const NoneType None = NoneType::None;
}

#endif
