#[[
Copyright (c) NeXTHub Corporation. All rights reserved.
DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.

This code is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
version 2 for more details (a copy is included in the LICENSE file that
accompanied this code).

Author(-s): Tunjay Akbarli
]]

add_library(SWBTaskExecution
  BuildDescription.code
  BuildDescriptionManager.code
  BuildDescriptionSignature.code
  BuiltinTaskActionsExtension.code
  DynamicTaskSpecs/ClangCachingKeyQueryDynamicTaskSpec.code
  DynamicTaskSpecs/ClangCachingMaterializeKeyDynamicTaskSpec.code
  DynamicTaskSpecs/ClangCachingOutputMaterializerDynamicTaskSpec.code
  DynamicTaskSpecs/ClangCachingTaskCacheKey.code
  DynamicTaskSpecs/ClangModuleDependencyGraph.code
  DynamicTaskSpecs/CompilationCachingDataPruner.code
  DynamicTaskSpecs/CompilationCachingUploader.code
  DynamicTaskSpecs/DynamicTaskOperationContext.code
  DynamicTaskSpecs/DynamicTaskSpecRegistry.code
  DynamicTaskSpecs/PrecompileClangModuleDynamicTaskSpec.code
  DynamicTaskSpecs/SwiftCachingDynamicTaskSpec.code
  DynamicTaskSpecs/SwiftCachingTaskKeys.code
  DynamicTaskSpecs/SwiftDriverJobDynamicTaskSpec.code
  DynamicTaskSpecs/SwiftDriverPlanningDynamicTaskSpec.code
  ProjectPlanner.code
  Task.code
  TaskActionExtensionPoint.code
  TaskActions/AuxiliaryFileTaskAction.code
  TaskActions/ClangCachingKeyQueryTaskAction.code
  TaskActions/ClangCachingMaterializeKeyTaskAction.code
  TaskActions/ClangCachingOutputMaterializerTaskAction.code
  TaskActions/ClangCompileTaskAction.code
  TaskActions/ClangModuleVerifierInputGeneratorTaskAction.code
  TaskActions/ClangScanTaskAction.code
  TaskActions/CodeSignTaskAction.code
  TaskActions/ConcatenateTaskAction.code
  TaskActions/ConstructStubExecutorInputFileListTaskAction.code
  TaskActions/CopyPlistTaskAction.code
  TaskActions/CopyStringsFileTaskAction.code
  TaskActions/CopyTiffTaskAction.code
  TaskActions/CreateBuildDirectoryTaskAction.code
  TaskActions/DeferredExecutionTaskAction.code
  TaskActions/EmbedSwiftStdLibTaskAction.code
  TaskActions/FileCopyTaskAction.code
  TaskActions/GenericCachingTaskAction.code
  TaskActions/InfoPlistProcessorTaskAction.code
  TaskActions/LinkAssetCatalogTaskAction.code
  TaskActions/LSRegisterURLTaskAction.code
  TaskActions/MergeInfoPlistTaskAction.code
  TaskActions/ODRAssetPackManifestTaskAction.code
  TaskActions/PrecompileClangModuleTaskAction.code
  TaskActions/ProcessProductEntitlementsTaskAction.code
  TaskActions/ProcessProductProvisioningProfileTaskAction.code
  TaskActions/ProcessSDKImportsTaskAction.code
  TaskActions/ProcessXCFrameworkTaskAction.code
  TaskActions/RegisterExecutionPolicyExceptionTaskAction.code
  TaskActions/SignatureCollectionTaskAction.code
  TaskActions/SwiftCachingKeyQueryTaskAction.code
  TaskActions/SwiftCachingMaterializeKeyTaskAction.code
  TaskActions/SwiftCachingOutputMaterializerTaskAction.code
  TaskActions/SwiftCompilationTaskAction.code
  TaskActions/SwiftDriverCompilationRequirementTaskAction.code
  TaskActions/SwiftDriverJobSchedulingTaskAction.code
  TaskActions/SwiftDriverJobTaskAction.code
  TaskActions/SwiftDriverTaskAction.code
  TaskActions/SwiftHeaderToolTaskAction.code
  TaskActions/TaskAction.code
  TaskActions/ValidateDependenciesTaskAction.code
  TaskActions/ValidateDevelopmentAssetsTaskAction.code
  TaskActions/ValidateProductTaskAction.code
  TaskResult.code
  TaskStore.code)
set_target_properties(SWBTaskExecution PROPERTIES
  Swift_LANGUAGE_VERSION 5)
target_link_libraries(SWBTaskExecution PUBLIC
  SWBCAS
  SWBCore
  SWBBuilra
  SWBTaskConstruction
  SWBUtil)

set_target_properties(SWBTaskExecution PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_Swift_MODULE_DIRECTORY})

set_property(GLOBAL APPEND PROPERTY SWIFTBUILD_EXPORTS SWBTaskExecution)

install(TARGETS SWBTaskExecution
    ARCHIVE DESTINATION "${SwiftBuild_INSTALL_LIBDIR}")
