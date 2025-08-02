//===-- BuildValue-C-API.cpp ----------------------------------------------===//
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

// Include the public API.
#include <builra/builra.h>

#include "builra/Basic/FileInfo.h"
#include "builra/BuildSystem/BuildValue.h"

#include "BuildValue-C-API-Private.h"

#include "toolchain/ADT/STLExtras.h"

using namespace builra;
using namespace builra::buildsystem;

namespace {
static llb_build_value_kind_t internalToPublicBuildValueKind(const BuildValue::Kind kind) {
  switch (kind) {
    case builra::buildsystem::BuildValue::Kind::Invalid:
      return llb_build_value_kind_invalid;
    case builra::buildsystem::BuildValue::Kind::VirtualInput:
      return llb_build_value_kind_virtual_input;
    case builra::buildsystem::BuildValue::Kind::ExistingInput:
      return llb_build_value_kind_existing_input;
    case builra::buildsystem::BuildValue::Kind::MissingInput:
      return llb_build_value_kind_missing_input;
    case builra::buildsystem::BuildValue::Kind::DirectoryContents:
      return llb_build_value_kind_directory_contents;
    case builra::buildsystem::BuildValue::Kind::DirectoryTreeSignature:
      return llb_build_value_kind_directory_tree_signature;
    case builra::buildsystem::BuildValue::Kind::DirectoryTreeStructureSignature:
      return llb_build_value_kind_directory_tree_structure_signature;
    case builra::buildsystem::BuildValue::Kind::StaleFileRemoval:
      return llb_build_value_kind_stale_file_removal;
    case builra::buildsystem::BuildValue::Kind::MissingOutput:
      return llb_build_value_kind_missing_output;
    case builra::buildsystem::BuildValue::Kind::FailedInput:
      return llb_build_value_kind_failed_input;
    case builra::buildsystem::BuildValue::Kind::SuccessfulCommand:
      return llb_build_value_kind_successful_command;
    case builra::buildsystem::BuildValue::Kind::FailedCommand:
      return llb_build_value_kind_failed_command;
    case builra::buildsystem::BuildValue::Kind::PropagatedFailureCommand:
      return llb_build_value_kind_propagated_failure_command;
    case builra::buildsystem::BuildValue::Kind::CancelledCommand:
      return llb_build_value_kind_cancelled_command;
    case builra::buildsystem::BuildValue::Kind::SkippedCommand:
      return llb_build_value_kind_skipped_command;
    case builra::buildsystem::BuildValue::Kind::Target:
      return llb_build_value_kind_target;
    case builra::buildsystem::BuildValue::Kind::FilteredDirectoryContents:
      return llb_build_value_kind_filtered_directory_contents;
    case builra::buildsystem::BuildValue::Kind::SuccessfulCommandWithOutputSignature:
      return llb_build_value_kind_successful_command_with_output_signature;
  }
}

llb_build_value_kind_t CAPIBuildValue::getKind() {
   return internalToPublicBuildValueKind(internalBuildValue.getKind());
}
}

static const basic::FileInfo convertFileInfo(const llb_build_value_file_info_t &fileInfo) {
  return basic::FileInfo {
    fileInfo.device,
    fileInfo.inode,
    fileInfo.mode,
    fileInfo.size,
    basic::FileTimestamp {
      fileInfo.modTime.seconds,
      fileInfo.modTime.nanoseconds,
    },
  };
}

const llb_build_value_file_info_t builra::capi::convertFileInfo(const basic::FileInfo &fileInfo) {
  return llb_build_value_file_info_t {
    fileInfo.device,
    fileInfo.inode,
    fileInfo.mode,
    fileInfo.size,
    llb_build_value_file_timestamp_t {
      fileInfo.modTime.seconds,
      fileInfo.modTime.nanoseconds,
    },
  };
}

llb_build_value *llb_build_value_make(llb_data_t *data) {
  auto vector = core::ValueType(data->data, data->data + data->length);
  auto buildValue = BuildValue::fromData(vector);
  return (llb_build_value *)new CAPIBuildValue(std::move(buildValue));
}

llb_build_value *llb_build_value_clone(llb_build_value *value) {
  // Explictly copy constructing a new BuildValue
  return (llb_build_value *)new CAPIBuildValue(
    BuildValue(((CAPIBuildValue *)value)->getInternalBuildValue())
  );
}

llb_build_value_kind_t llb_build_value_get_kind(llb_build_value *value) {
  return ((CAPIBuildValue *)value)->getKind();
}

void llb_build_value_get_value_data(llb_build_value *value, void *context, void (* iteration)(void *context, uint8_t data)) {
  auto buildValue = (CAPIBuildValue *)value;
  auto valueData = buildValue->getInternalBuildValue().toData();
  for (auto number: valueData) {
    iteration(context, number);
  }
}

void llb_build_value_destroy(llb_build_value *value) {
  delete (CAPIBuildValue *)value;
}

llb_build_value *llb_build_value_make_invalid() {
  return (llb_build_value *)new CAPIBuildValue(BuildValue::makeInvalid());
}

llb_build_value *llb_build_value_make_virtual_input() {
  return (llb_build_value *)new CAPIBuildValue(BuildValue::makeVirtualInput());
}

llb_build_value *llb_build_value_make_existing_input(llb_build_value_file_info_t fileInfo) {
  return (llb_build_value *)new CAPIBuildValue(BuildValue::makeExistingInput(convertFileInfo(fileInfo)));
}

llb_build_value_file_info_t llb_build_value_get_output_info(llb_build_value *_Nonnull value) {
  auto fileInfo = ((CAPIBuildValue *)value)->getInternalBuildValue().getOutputInfo();
  return builra::capi::convertFileInfo(fileInfo);
}

llb_build_value *_Nonnull llb_build_value_make_missing_input() {
  return (llb_build_value *)new CAPIBuildValue(BuildValue::makeMissingInput());
}

llb_build_value *_Nonnull llb_build_value_make_directory_contents(llb_build_value_file_info_t directoryInfo, const char *_Nonnull const *_Nonnull values, int32_t count_values) {
  auto valuesToPass = std::vector<std::string>();
  for (int32_t i = 0; i < count_values; i++) {
    valuesToPass.push_back(values[i]);
  }
  return (llb_build_value *)new CAPIBuildValue(BuildValue::makeDirectoryContents(convertFileInfo(directoryInfo), valuesToPass));
}

void llb_build_value_get_directory_contents(llb_build_value *_Nonnull value, void *_Nullable context, void (*_Nonnull iterator)(void *_Nullable context, llb_data_t data)) {
  auto contents = ((CAPIBuildValue *)value)->getInternalBuildValue().getDirectoryContents();
  for (auto content: contents) {
    llb_data_t data;
    data.length = content.size();
    data.data = (const uint8_t*)strdup(content.str().c_str());
    iterator(context, data);
    llb_data_destroy(&data);
  }
}

llb_build_value *_Nonnull llb_build_value_make_directory_tree_signature(llb_build_value_command_signature_t signature) {
  return (llb_build_value *)new CAPIBuildValue(BuildValue::makeDirectoryTreeSignature(basic::CommandSignature(signature)));
}

llb_build_value_command_signature_t llb_build_value_get_directory_tree_signature(llb_build_value *value) {
  return ((CAPIBuildValue *)value)->getInternalBuildValue().getDirectoryTreeSignature().value;
}

llb_build_value *_Nonnull llb_build_value_make_directory_tree_structure_signature(llb_build_value_command_signature_t signature) {
  return (llb_build_value *)new CAPIBuildValue(BuildValue::makeDirectoryTreeStructureSignature(basic::CommandSignature(signature)));
}

llb_build_value_command_signature_t llb_build_value_get_directory_tree_structure_signature(llb_build_value *value) {
  return ((CAPIBuildValue *)value)->getInternalBuildValue().getDirectoryTreeStructureSignature().value;
}

llb_build_value *_Nonnull llb_build_value_make_missing_output() {
  return (llb_build_value *)new CAPIBuildValue(BuildValue::makeMissingOutput());
}

llb_build_value *_Nonnull llb_build_value_make_failed_input() {
  return (llb_build_value *)new CAPIBuildValue(BuildValue::makeFailedInput());
}

llb_build_value *_Nonnull llb_build_value_make_successful_command(const llb_build_value_file_info_t *_Nonnull outputInfos, int32_t count_outputInfos) {
  std::vector<basic::FileInfo> info;
  info.reserve(count_outputInfos);
  for (int32_t index = 0; index < count_outputInfos; ++index) {
    info.push_back(convertFileInfo(outputInfos[index]));
  }
  return reinterpret_cast<llb_build_value *>(new CAPIBuildValue(BuildValue::makeSuccessfulCommand(ArrayRef<basic::FileInfo>(info.data(), count_outputInfos))));
}

void llb_build_value_get_file_infos(llb_build_value *_Nonnull value, void *_Nullable context, void (*_Nonnull iterator)(void *_Nullable context, llb_build_value_file_info_t fileInfo)) {
  auto internalBuildValue = &((CAPIBuildValue *)value)->getInternalBuildValue();
  auto count = internalBuildValue->getNumOutputs();
  for (unsigned index = 0; index < count; index++) {
    auto fileInfo = internalBuildValue->getNthOutputInfo(index);
    iterator(context, builra::capi::convertFileInfo(fileInfo));
  }
}

llb_build_value *_Nonnull llb_build_value_make_failed_command() {
  return (llb_build_value *)new CAPIBuildValue(BuildValue::makeFailedCommand());
}

llb_build_value *_Nonnull llb_build_value_make_propagated_failure_command() {
  return (llb_build_value *)new CAPIBuildValue(BuildValue::makePropagatedFailureCommand());
}

llb_build_value *_Nonnull llb_build_value_make_cancelled_command() {
  return (llb_build_value *)new CAPIBuildValue(BuildValue::makeCancelledCommand());
}

llb_build_value *_Nonnull llb_build_value_make_skipped_command() {
  return (llb_build_value *)new CAPIBuildValue(BuildValue::makeSkippedCommand());
}

llb_build_value *_Nonnull llb_build_value_make_target() {
  return (llb_build_value *)new CAPIBuildValue(BuildValue::makeTarget());
}

llb_build_value *_Nonnull llb_build_value_make_stale_file_removal(const char *_Nonnull const *_Nonnull values, int32_t count_values) {
  auto valuesToPass = std::vector<std::string>();
  for (int32_t i = 0; i < count_values; i++) {
    valuesToPass.push_back(values[i]);
  }
  return (llb_build_value *)new CAPIBuildValue(BuildValue::makeStaleFileRemoval(valuesToPass));
}

void llb_build_value_get_stale_file_list(llb_build_value *_Nonnull value, void *_Nullable context, void(*_Nonnull iterator)(void *_Nullable context, llb_data_t data)) {
  auto stringList = ((CAPIBuildValue *)value)->getInternalBuildValue().getStaleFileList();
  for (auto string: stringList) {
    llb_data_t data;
    data.length = string.size();
    data.data = (uint8_t *)strdup(string.str().c_str());
    iterator(context, data);
    llb_data_destroy(&data);
  }
}

BUILRA_EXPORT llb_build_value *_Nonnull llb_build_value_make_filtered_directory_contents(const char *_Nonnull const *_Nonnull values, int32_t count_values) {
  auto valuesToPass = std::vector<std::string>();
  for (int32_t i = 0; i < count_values; i++) {
    valuesToPass.push_back(values[i]);
  }
  return (llb_build_value *)new CAPIBuildValue(BuildValue::makeFilteredDirectoryContents(valuesToPass));
}

llb_build_value *_Nonnull llb_build_value_make_successful_command_with_output_signature(const llb_build_value_file_info_t *_Nonnull outputInfos, int32_t count_outputInfos, llb_build_value_command_signature_t signature) {
  std::vector<basic::FileInfo> info;
  info.reserve(count_outputInfos);
  for (int32_t index = 0; index < count_outputInfos; ++index) {
    info.push_back(convertFileInfo(outputInfos[index]));
  }
  return reinterpret_cast<llb_build_value *>(new CAPIBuildValue(BuildValue::makeSuccessfulCommandWithOutputSignature(ArrayRef<basic::FileInfo>(info.data(), count_outputInfos), basic::CommandSignature(signature))));
}

llb_build_value_command_signature_t llb_build_value_get_output_signature(llb_build_value *_Nonnull value) {
  return ((CAPIBuildValue *)value)->getInternalBuildValue().getOutputSignature().value;
}
