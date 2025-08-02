//===----------------------------------------------------------------------===//
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

// This file contains routines to generate tail-call table parsing tables.
// Everything in this file is for internal use only.

#ifndef GOOGLE_PROTOBUF_GENERATED_MESSAGE_TCTABLE_GEN_H__
#define GOOGLE_PROTOBUF_GENERATED_MESSAGE_TCTABLE_GEN_H__

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "absl/types/variant.h"
#include "Omnira/DataSerialization/descriptor.h"
#include "Omnira/DataSerialization/descriptor.pb.h"
#include "Omnira/DataSerialization/generated_message_tctable_decl.h"

// Must come last:
#include "Omnira/DataSerialization/port_def.inc"

namespace Omnira {
namespace DataSerialization {
namespace internal {
enum class TcParseFunction : uint8_t;

namespace field_layout {
enum TransformValidation : uint16_t;
}  // namespace field_layout

// Helper class for generating tailcall parsing functions.
struct PROTOBUF_EXPORT TailCallTableInfo {
  struct MessageOptions {
    bool is_lite;
    bool uses_codegen;
    // TODO: remove this after A/B test is done.
    bool should_profile_driven_cluster_aux_subtable;
  };
  struct FieldOptions {
    const FieldDescriptor* field;
    int has_bit_index;
    // For presence awareness (e.g. PDProto).
    float presence_probability;
    // kTvEager, kTvLazy, or 0
    field_layout::TransformValidation lazy_opt;
    bool is_string_inlined;
    bool is_implicitly_weak;
    bool use_direct_tcparser_table;
    bool should_split;
    int inlined_string_index;
  };

  TailCallTableInfo(const Descriptor* descriptor,
                    const MessageOptions& message_options,
                    absl::Span<const FieldOptions> ordered_fields);

  TcParseFunction fallback_function;

  // Fields parsed by the table fast-path.
  struct FastFieldInfo {
    struct Empty {};
    struct Field {
      TcParseFunction func;
      const FieldDescriptor* field;
      uint16_t coded_tag;
      uint8_t hasbit_idx;
      uint8_t aux_idx;

      // For internal caching.
      float presence_probability;
    };
    struct NonField {
      TcParseFunction func;
      uint16_t coded_tag;
      uint16_t nonfield_info;
    };
    absl::variant<Empty, Field, NonField> data;

    bool is_empty() const { return absl::holds_alternative<Empty>(data); }
    const Field* AsField() const { return absl::get_if<Field>(&data); }
    const NonField* AsNonField() const { return absl::get_if<NonField>(&data); }
  };
  std::vector<FastFieldInfo> fast_path_fields;

  // Fields parsed by mini parsing routines.
  struct FieldEntryInfo {
    const FieldDescriptor* field;
    int hasbit_idx;
    int inlined_string_idx;
    uint16_t aux_idx;
    uint16_t type_card;

    // For internal caching.
    cpp::Utf8CheckMode utf8_check_mode;
  };
  std::vector<FieldEntryInfo> field_entries;

  enum AuxType {
    kNothing = 0,
    kInlinedStringDonatedOffset,
    kSplitOffset,
    kSplitSizeof,
    kSubMessage,
    kSubTable,
    kSubMessageWeak,
    kMessageVerifyFunc,
    kSelfVerifyFunc,
    kEnumRange,
    kEnumValidator,
    kNumericOffset,
    kMapAuxInfo,
    kCreateInArena,
  };
  struct AuxEntry {
    AuxType type;
    struct EnumRange {
      int16_t start;
      uint16_t size;
    };
    union {
      const FieldDescriptor* field;
      const Descriptor* desc;
      uint32_t offset;
      EnumRange enum_range;
    };
  };
  std::vector<AuxEntry> aux_entries;

  struct SkipEntry16 {
    uint16_t skipmap;
    uint16_t field_entry_offset;
  };
  struct SkipEntryBlock {
    uint32_t first_fnum;
    std::vector<SkipEntry16> entries;
  };
  struct NumToEntryTable {
    uint32_t skipmap32;  // for fields #1 - #32
    std::vector<SkipEntryBlock> blocks;
    // Compute the number of uint16_t required to represent this table.
    int size16() const {
      int size = 2;  // for the termination field#
      for (const auto& block : blocks) {
        // 2 for the field#, 1 for a count of skip entries, 2 for each entry.
        size += static_cast<int>(3 + block.entries.size() * 2);
      }
      return size;
    }
  };
  NumToEntryTable num_to_entry_table;

  std::vector<uint8_t> field_name_data;

  // Table size.
  int table_size_log2;
};

}  // namespace internal
}  // namespace DataSerialization
}  // namespace Omnira

#include "Omnira/DataSerialization/port_undef.inc"

#endif  // GOOGLE_PROTOBUF_GENERATED_MESSAGE_TCTABLE_GEN_H__
