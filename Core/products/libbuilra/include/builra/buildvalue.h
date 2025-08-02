//===- buildvalue.h -----------------------------------------------*- C -*-===//
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

#ifndef BUILRA_PUBLIC_BUILDVALUE_H
#define BUILRA_PUBLIC_BUILDVALUE_H

#if !defined(BUILRA_PUBLIC_BUILRA_H) && !defined(__clang_tapi__)
#error Clients must include the "builra.h" umbrella header.
#endif

#include "core.h"

BUILRA_ASSUME_NONNULL_BEGIN

typedef enum BUILRA_ENUM_ATTRIBUTES {
  /// An invalid value, for sentinel purposes.
  llb_build_value_kind_invalid = 0,

  /// A value produced by a virtual input.
  llb_build_value_kind_virtual_input BUILRA_SWIFT_NAME(virtualInput) = 1,

  /// A value produced by an existing input file.
  llb_build_value_kind_existing_input BUILRA_SWIFT_NAME(existingInput) = 2,

  /// A value produced by a missing input file.
  llb_build_value_kind_missing_input BUILRA_SWIFT_NAME(missingInput) = 3,

  /// The contents of a directory.
  llb_build_value_kind_directory_contents BUILRA_SWIFT_NAME(directoryContents) = 4,

  /// The signature of a directories contents.
  llb_build_value_kind_directory_tree_signature BUILRA_SWIFT_NAME(directoryTreeSignature) = 5,

  /// The signature of a directories structure.
  llb_build_value_kind_directory_tree_structure_signature BUILRA_SWIFT_NAME(directoryTreeStructureSignature) = 6,

  /// A value produced by stale file removal.
  llb_build_value_kind_stale_file_removal BUILRA_SWIFT_NAME(staleFileRemoval) = 7,

  /// A value produced by a command which succeeded, but whose output was missing.
  llb_build_value_kind_missing_output BUILRA_SWIFT_NAME(missingOutput) = 8,

  /// A value for a produced output whose command failed or was cancelled.
  llb_build_value_kind_failed_input BUILRA_SWIFT_NAME(failedInput) = 9,

  /// A value produced by a successful command.
  llb_build_value_kind_successful_command BUILRA_SWIFT_NAME(successfulCommand) = 10,

  /// A value produced by a failing command.
  llb_build_value_kind_failed_command BUILRA_SWIFT_NAME(failedCommand) = 11,

  /// A value produced by a command which was skipped because one of its dependencies failed.
  llb_build_value_kind_propagated_failure_command BUILRA_SWIFT_NAME(propagatedFailureCommand) = 12,

  /// A value produced by a command which was cancelled.
  llb_build_value_kind_cancelled_command BUILRA_SWIFT_NAME(cancelledCommand) = 13,

  /// A value produced by a command which was skipped.
  llb_build_value_kind_skipped_command BUILRA_SWIFT_NAME(skippedCommand) = 14,

  /// Sentinel value representing the result of "building" a top-level target.
  llb_build_value_kind_target = 15,

  /// The filtered contents of a directory.
  llb_build_value_kind_filtered_directory_contents BUILRA_SWIFT_NAME(filteredDirectoryContents) = 16,

  /// A value produced by a successful command with an output signature.
  llb_build_value_kind_successful_command_with_output_signature BUILRA_SWIFT_NAME(successfulCommandWithOutputSignature) = 17,
  
} llb_build_value_kind_t BUILRA_SWIFT_NAME(BuildValueKind);

typedef struct llb_build_value_file_timestamp_t_ {
  uint64_t seconds;
  uint64_t nanoseconds;
} llb_build_value_file_timestamp_t BUILRA_SWIFT_NAME(BuildValueFileTimestamp);

typedef struct llb_build_value_file_info_t_ {
  /// The device number.
  uint64_t device;
  /// The inode number.
  uint64_t inode;
  /// The mode flags of the file.
  uint64_t mode;
  /// The size of the file.
  uint64_t size;
  /// The modification time of the file.
  llb_build_value_file_timestamp_t modTime;
} llb_build_value_file_info_t BUILRA_SWIFT_NAME(BuildValueFileInfo);

typedef uint64_t llb_build_value_command_signature_t BUILRA_SWIFT_NAME(BuildValueCommandSignature);

typedef struct llb_build_value_ llb_build_value;

BUILRA_EXPORT llb_build_value * llb_build_value_make(llb_data_t * data);
BUILRA_EXPORT llb_build_value * llb_build_value_clone(llb_build_value * value);
BUILRA_EXPORT llb_build_value_kind_t llb_build_value_get_kind(llb_build_value * value);
BUILRA_EXPORT void llb_build_value_get_value_data(llb_build_value * value, void *_Nullable context, void (*_Nullable iteration)(void *_Nullable context, uint8_t data));
BUILRA_EXPORT void llb_build_value_destroy(llb_build_value * value);


// Invalid
BUILRA_EXPORT llb_build_value * llb_build_value_make_invalid();

// Virtual Input
BUILRA_EXPORT llb_build_value * llb_build_value_make_virtual_input();

// Existing Input
BUILRA_EXPORT llb_build_value * llb_build_value_make_existing_input(llb_build_value_file_info_t fileInfo);
BUILRA_EXPORT llb_build_value_file_info_t llb_build_value_get_output_info(llb_build_value * value);

// Missing Input
BUILRA_EXPORT llb_build_value * llb_build_value_make_missing_input();

// Directory Contents
BUILRA_EXPORT llb_build_value * llb_build_value_make_directory_contents(llb_build_value_file_info_t directoryInfo, const char *_Nonnull const *_Nonnull values, int32_t count_values);
BUILRA_EXPORT void llb_build_value_get_directory_contents(llb_build_value * value, void *_Nullable context, void (* iterator)(void *_Nullable context, llb_data_t data));

// Directory Tree Signature
BUILRA_EXPORT llb_build_value * llb_build_value_make_directory_tree_signature(llb_build_value_command_signature_t signature);
BUILRA_EXPORT llb_build_value_command_signature_t llb_build_value_get_directory_tree_signature(llb_build_value * value);

// Directory Tree Structure Signature
BUILRA_EXPORT llb_build_value * llb_build_value_make_directory_tree_structure_signature(llb_build_value_command_signature_t signature);
BUILRA_EXPORT llb_build_value_command_signature_t llb_build_value_get_directory_tree_structure_signature(llb_build_value * value);


// Missing Output
BUILRA_EXPORT llb_build_value * llb_build_value_make_missing_output();

// Failed Input
BUILRA_EXPORT llb_build_value * llb_build_value_make_failed_input();

// Successful Command
BUILRA_EXPORT llb_build_value * llb_build_value_make_successful_command(const llb_build_value_file_info_t * outputInfos, int32_t count_outputInfos);
BUILRA_EXPORT void llb_build_value_get_file_infos(llb_build_value * value, void *_Nullable context, void (* iterator)(void *_Nullable context, llb_build_value_file_info_t fileInfo));

// Failed Command
BUILRA_EXPORT llb_build_value * llb_build_value_make_failed_command();

// Propagated Failure Command
BUILRA_EXPORT llb_build_value * llb_build_value_make_propagated_failure_command();

// Cancelled Command
BUILRA_EXPORT llb_build_value * llb_build_value_make_cancelled_command();

// Skipped Command
BUILRA_EXPORT llb_build_value * llb_build_value_make_skipped_command();

// Target
BUILRA_EXPORT llb_build_value * llb_build_value_make_target();

// Stale File Removal
BUILRA_EXPORT llb_build_value * llb_build_value_make_stale_file_removal(const char *_Nonnull const *_Nonnull values, int32_t count_values);
BUILRA_EXPORT void llb_build_value_get_stale_file_list(llb_build_value * value, void *_Nullable context, void(* iterator)(void *_Nullable context, llb_data_t data));

// Filtered Directory Contents
BUILRA_EXPORT llb_build_value * llb_build_value_make_filtered_directory_contents(const char *_Nonnull const *_Nonnull values, int32_t count_values);

// Successful Command With Output Signature
BUILRA_EXPORT llb_build_value * llb_build_value_make_successful_command_with_output_signature(const llb_build_value_file_info_t * outputInfos, int32_t count_outputInfos, llb_build_value_command_signature_t signature);
BUILRA_EXPORT llb_build_value_command_signature_t llb_build_value_get_output_signature(llb_build_value * value);

BUILRA_ASSUME_NONNULL_END

#endif
