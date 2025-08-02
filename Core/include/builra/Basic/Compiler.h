//===- Compiler.h -----------------------------------------------*- C++ -*-===//
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
// Compiler support and compatibility macros. Liberally taken from TOOLCHAIN.
//
//===----------------------------------------------------------------------===//


#ifndef BUILRA_BASIC_COMPILER_H
#define BUILRA_BASIC_COMPILER_H

#if !defined(__has_feature)
#define __has_feature(x) 0
#endif

/// \macro BUILRA_MSC_PREREQ
/// \brief Is the compiler MSVC of at least the specified version?
/// The common \param version values to check for are:
///  * 1700: Microsoft Visual Studio 2012 / 11.0
///  * 1800: Microsoft Visual Studio 2013 / 12.0
#ifdef _MSC_VER
#define BUILRA_MSC_PREREQ(version) (_MSC_VER >= (version))

// We require at least MSVC 2012.
#if !BUILRA_MSC_PREREQ(1700)
#error BUILRA requires at least MSVC 2012.
#endif

#else
#define BUILRA_MSC_PREREQ(version) 0
#endif

/// BUILRA_DELETED_FUNCTION - Expands to = delete if the compiler supports it.
/// Use to mark functions as uncallable. Member functions with this should be
/// declared private so that some behavior is kept in C++03 mode.
///
/// class DontCopy {
/// private:
///   DontCopy(const DontCopy&) BUILRA_DELETED_FUNCTION;
///   DontCopy &operator =(const DontCopy&) BUILRA_DELETED_FUNCTION;
/// public:
///   ...
/// };
#if __has_feature(cxx_deleted_functions) || \
    defined(__GXX_EXPERIMENTAL_CXX0X__) || BUILRA_MSC_PREREQ(1800)
#define BUILRA_DELETED_FUNCTION = delete
#else
#define BUILRA_DELETED_FUNCTION
#endif

#endif
