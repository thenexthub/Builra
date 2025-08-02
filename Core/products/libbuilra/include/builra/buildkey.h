//===- buildkey.h -------------------------------------------------*- C -*-===//
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

#ifndef BUILRA_PUBLIC_BUILDKEY_H
#define BUILRA_PUBLIC_BUILDKEY_H

#if !defined(BUILRA_PUBLIC_BUILRA_H) && !defined(__clang_tapi__)
#error Clients must include the "builra.h" umbrella header.
#endif

#include "core.h"
#include <stddef.h>

BUILRA_ASSUME_NONNULL_BEGIN

typedef enum BUILRA_ENUM_ATTRIBUTES {
  /// A key used to identify a command.
  llb_build_key_kind_command = 0,

  /// A key used to identify a custom task.
  llb_build_key_kind_custom_task BUILRA_SWIFT_NAME(customTask) = 1,

  /// A key used to identify directory contents.
  llb_build_key_kind_directory_contents BUILRA_SWIFT_NAME(directoryContents) = 2,

  /// A key used to identify the signature of a complete directory tree.
  llb_build_key_kind_directory_tree_signature BUILRA_SWIFT_NAME(directoryTreeSignature) = 3,

  /// A key used to identify a node.
  llb_build_key_kind_node = 4,

  /// A key used to identify a target.
  llb_build_key_kind_target = 5,

  /// An invalid key kind.
  llb_build_key_kind_unknown = 6,

  /// A key used to identify the signature of a complete directory tree.
  llb_build_key_kind_directory_tree_structure_signature BUILRA_SWIFT_NAME(directoryTreeStructureSignature) = 7,

  /// A key used to identify filtered directory contents.
  llb_build_key_kind_filtered_directory_contents BUILRA_SWIFT_NAME(filteredDirectoryContents) = 8,

  /// A key used to identify a node.
  llb_build_key_kind_stat = 10,
} llb_build_key_kind_t BUILRA_SWIFT_NAME(BuildKeyKind);

typedef struct llb_build_key_t_ llb_build_key_t;

BUILRA_EXPORT llb_build_key_t *llb_build_key_make(const llb_data_t *data);

BUILRA_EXPORT bool llb_build_key_equal(llb_build_key_t *key1, llb_build_key_t *key2);

BUILRA_EXPORT size_t llb_build_key_hash(llb_build_key_t *key);

BUILRA_EXPORT void llb_build_key_get_key_data(llb_build_key_t *key, void *_Nonnull context, void (*_Nonnull iteration)(void *context, uint8_t *data, size_t count));

BUILRA_EXPORT llb_build_key_kind_t llb_build_key_get_kind(llb_build_key_t *key);

BUILRA_EXPORT void llb_build_key_destroy(llb_build_key_t *key);

BUILRA_EXPORT char llb_build_key_identifier_for_kind(llb_build_key_kind_t kind);
BUILRA_EXPORT llb_build_key_kind_t llb_build_key_kind_for_identifier(char identifier);

// Command
BUILRA_EXPORT llb_build_key_t *llb_build_key_make_command(const char *name);
BUILRA_EXPORT void llb_build_key_get_command_name(llb_build_key_t *key, llb_data_t *out_name);

// Custom Task
BUILRA_EXPORT llb_build_key_t *llb_build_key_make_custom_task(const char *name, const char *taskData);
BUILRA_EXPORT llb_build_key_t *llb_build_key_make_custom_task_with_data(const char *name, llb_data_t data);
BUILRA_EXPORT void llb_build_key_get_custom_task_name(llb_build_key_t *key, llb_data_t *out_name);
BUILRA_EXPORT void llb_build_key_get_custom_task_data(llb_build_key_t *key, llb_data_t *out_task_data);
BUILRA_EXPORT void llb_build_key_get_custom_task_data_no_copy(llb_build_key_t *key, llb_data_t *out_task_data);


// Directory Contents
BUILRA_EXPORT llb_build_key_t *llb_build_key_make_directory_contents(const char *path);
BUILRA_EXPORT void llb_build_key_get_directory_path(llb_build_key_t *key, llb_data_t *out_path);

typedef void (*IteratorFunction)(void *_Nullable context, llb_data_t data);

// Filtered Directory Contents
BUILRA_EXPORT llb_build_key_t *llb_build_key_make_filtered_directory_contents(const char *path, const char *_Nonnull const *_Nonnull filters, int32_t count_filters);
BUILRA_EXPORT void llb_build_key_get_filtered_directory_path(llb_build_key_t *key, llb_data_t *out_path);
BUILRA_EXPORT void llb_build_key_get_filtered_directory_filters(llb_build_key_t *key, void *_Nullable context, IteratorFunction  iterator);

// Directory Tree Signature
BUILRA_EXPORT llb_build_key_t *llb_build_key_make_directory_tree_signature(const char *path, const char *_Nonnull const *_Nonnull filters, int32_t count_filters);
BUILRA_EXPORT void llb_build_key_get_directory_tree_signature_path(llb_build_key_t *key, llb_data_t *out_path);
BUILRA_EXPORT void llb_build_key_get_directory_tree_signature_filters(llb_build_key_t *key, void *_Nullable context, IteratorFunction  iterator);

// Directory Tree Structure Signature
BUILRA_EXPORT llb_build_key_t *llb_build_key_make_directory_tree_structure_signature(const char *path, const char *_Nonnull const *_Nonnull filters, int32_t count_filters);
BUILRA_EXPORT void llb_build_key_get_directory_tree_structure_signature_path(llb_build_key_t *key, llb_data_t *out_path);
BUILRA_EXPORT void llb_build_key_get_directory_tree_structure_signature_filters(llb_build_key_t *key, void *_Nullable context, IteratorFunction  iterator);

// Node
BUILRA_EXPORT llb_build_key_t *llb_build_key_make_node(const char *path);
BUILRA_EXPORT void llb_build_key_get_node_path(llb_build_key_t *key, llb_data_t *out_path);

// Stat
BUILRA_EXPORT llb_build_key_t *llb_build_key_make_stat(const char *path);
BUILRA_EXPORT void llb_build_key_get_stat_path(llb_build_key_t *key, llb_data_t *out_path);

// Target
BUILRA_EXPORT llb_build_key_t *llb_build_key_make_target(const char *name);
BUILRA_EXPORT void llb_build_key_get_target_name(llb_build_key_t *key, llb_data_t *out_name);

BUILRA_ASSUME_NONNULL_END

#endif
