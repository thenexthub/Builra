//===--- Compiler.h ---------------------------------------------*- C++ -*-===//
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
//
// This file contains a variety of feature test macros copied from
// include/toolchain/Support/Compiler.h so that TOOLCHAINDemangle does not need to take
// a dependency on TOOLCHAINSupport.
//===----------------------------------------------------------------------===//

#ifndef TOOLCHAIN_DEMANGLE_COMPILER_H
#define TOOLCHAIN_DEMANGLE_COMPILER_H

#ifdef _MSC_VER
// snprintf is implemented in VS 2015
#if _MSC_VER < 1900
#define snprintf _snprintf_s
#endif
#endif

#ifndef __has_feature
#define __has_feature(x) 0
#endif

#ifndef __has_cpp_attribute
#define __has_cpp_attribute(x) 0
#endif

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

#ifndef TOOLCHAIN_GNUC_PREREQ
#if defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
#define TOOLCHAIN_GNUC_PREREQ(maj, min, patch)                                      \
  ((__GNUC__ << 20) + (__GNUC_MINOR__ << 10) + __GNUC_PATCHLEVEL__ >=          \
   ((maj) << 20) + ((min) << 10) + (patch))
#elif defined(__GNUC__) && defined(__GNUC_MINOR__)
#define TOOLCHAIN_GNUC_PREREQ(maj, min, patch)                                      \
  ((__GNUC__ << 20) + (__GNUC_MINOR__ << 10) >= ((maj) << 20) + ((min) << 10))
#else
#define TOOLCHAIN_GNUC_PREREQ(maj, min, patch) 0
#endif
#endif

#if __has_attribute(used) || TOOLCHAIN_GNUC_PREREQ(3, 1, 0)
#define TOOLCHAIN_ATTRIBUTE_USED __attribute__((__used__))
#else
#define TOOLCHAIN_ATTRIBUTE_USED
#endif

#if __has_builtin(__builtin_unreachable) || TOOLCHAIN_GNUC_PREREQ(4, 5, 0)
#define TOOLCHAIN_BUILTIN_UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#define TOOLCHAIN_BUILTIN_UNREACHABLE __assume(false)
#endif

#if __has_attribute(noinline) || TOOLCHAIN_GNUC_PREREQ(3, 4, 0)
#define TOOLCHAIN_ATTRIBUTE_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define TOOLCHAIN_ATTRIBUTE_NOINLINE __declspec(noinline)
#else
#define TOOLCHAIN_ATTRIBUTE_NOINLINE
#endif

#if !defined(NDEBUG) || defined(TOOLCHAIN_ENABLE_DUMP)
#define TOOLCHAIN_DUMP_METHOD TOOLCHAIN_ATTRIBUTE_NOINLINE TOOLCHAIN_ATTRIBUTE_USED
#else
#define TOOLCHAIN_DUMP_METHOD TOOLCHAIN_ATTRIBUTE_NOINLINE
#endif

#if __cplusplus > 201402L && __has_cpp_attribute(fallthrough)
#define TOOLCHAIN_FALLTHROUGH [[fallthrough]]
#elif __has_cpp_attribute(gnu::fallthrough)
#define TOOLCHAIN_FALLTHROUGH [[gnu::fallthrough]]
#elif !__cplusplus
// Workaround for toolchain.org/PR23435, since clang 3.6 and below emit a spurious
// error when __has_cpp_attribute is given a scoped attribute in C mode.
#define TOOLCHAIN_FALLTHROUGH
#elif __has_cpp_attribute(clang::fallthrough)
#define TOOLCHAIN_FALLTHROUGH [[clang::fallthrough]]
#else
#define TOOLCHAIN_FALLTHROUGH
#endif

#endif
