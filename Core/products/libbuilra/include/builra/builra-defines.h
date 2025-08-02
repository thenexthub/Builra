//===- builra-defines.h ------------------------------------------*- C -*-===//
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

#ifndef BUILRA_PUBLIC_BUILRA_DEFINES_H
#define BUILRA_PUBLIC_BUILRA_DEFINES_H

#if !defined(BUILRA_PUBLIC_BUILRA_H) && !defined(__clang_tapi__)
#error Clients must include the "builra.h" umbrella header.
#endif

#if defined(__cplusplus)
#define BUILRA_EXTERN extern "C"
#else
#define BUILRA_EXTERN extern
#endif

#if defined(__ELF__) || (defined(__APPLE__) && defined(__MACH__))
#define BUILRA_EXPORT BUILRA_EXTERN __attribute__((__visibility__("default")))
#else
// asume PE/COFF
#if defined(_WINDLL)
#if defined(libbuilra_EXPORTS)
#define BUILRA_EXPORT BUILRA_EXTERN __declspec(dllexport)
#else
#define BUILRA_EXPORT BUILRA_EXTERN __declspec(dllimport)
#endif
#else
#define BUILRA_EXPORT BUILRA_EXTERN
#endif
#endif

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#if __has_attribute(swift_name)
# define BUILRA_SWIFT_NAME(_name) __attribute__((swift_name(#_name)))
#else
# define BUILRA_SWIFT_NAME(_name)
#endif

#if __has_attribute(enum_extensibility)
#define BUILRA_ENUM_ATTRIBUTES __attribute__((enum_extensibility(open)))
#else
#define BUILRA_ENUM_ATTRIBUTES
#endif

#ifndef __has_feature
# define __has_feature(x) 0
#endif
#if !__has_feature(nullability)
# ifndef _Nullable
#  define _Nullable
# endif
# ifndef _Nonnull
#  define _Nonnull
# endif
#endif

#if __has_feature(assume_nonnull)
#define BUILRA_ASSUME_NONNULL_BEGIN _Pragma("clang assume_nonnull begin")
#define BUILRA_ASSUME_NONNULL_END   _Pragma("clang assume_nonnull end")
#else
#define BUILRA_ASSUME_NONNULL_BEGIN
#define BUILRA_ASSUME_NONNULL_END
#endif

/// A monotonically increasing indicator of the builra API version.
///
/// The builra API is *not* stable. This value allows clients to conditionally
/// compile for multiple versions of the API.
///
/// Version History:
/// 19: Added isResultValid API with a fallback to CAPIExternalCommand.
///
/// 18: Added support for configuring outputs of dynamic tasks via the C API.
///
/// 17: Added `llb_buildsystem_dependency_data_format_makefile_ignoring_subsequent_outputs`
///
/// 16: Added more efficient C API for working with custom tasks
///
/// 15: Added `determined_rule_needs_to_run` delegate method
///
/// 14: Added configure API to CAPIExternalCommand
///
/// 13: Update command status for custom tasks as well
///
/// 12: Invoke provideValue on ExternalCommand for all build values
///
/// 11: Added QualityOfService field to llb_buildsystem_invocation_t
///
/// 10: Changed to a llb_task_interface_t copies instead of pointers
///
/// 9: Changed the API for build keys to use bridged opaque pointers with access functions
///
/// 8: Move scheduler algorithm and lanes into llb_buildsystem_invocation_t
///
/// 7: Added destroy_context task delegate method.
///
/// 6: Added delegate methods for specific diagnostics.
///
/// 5: Added `llb_buildsystem_command_extended_result_t`, changed command_process_finished signature.
///
/// 4: Added llb_buildsystem_build_node.
///
/// 3: Added command_had_error, command_had_note and command_had_warning delegate methods.
///
/// 2: Added `llb_buildsystem_command_result_t` parameter to command_finished.
///
/// 1: Added `environment` parameter to llb_buildsystem_invocation_t.
///
/// 0: Pre-history
#define BUILRA_C_API_VERSION 19

#endif
