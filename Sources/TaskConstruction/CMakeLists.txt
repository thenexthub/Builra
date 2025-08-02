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

add_library(SWBTaskConstruction
  BuildDirectoryContext.code
  DiagnosticSupport.code
  ProductPlanning/BuildPlan.code
  ProductPlanning/BuildPlanDumping.code
  ProductPlanning/ProductPlan.code
  ProductPlanning/ProductPlanner.code
  StaleFileRemovalContext.code
  TaskPlanningDelegate.code
  TaskProducers/BuildPhaseTaskProducers/AppleScriptTaskProducer.code
  TaskProducers/BuildPhaseTaskProducers/BuildRuleTaskProducer.code
  TaskProducers/BuildPhaseTaskProducers/CopyFilesTaskProducer.code
  TaskProducers/BuildPhaseTaskProducers/FilesBasedBuildPhaseTaskProducer.code
  TaskProducers/BuildPhaseTaskProducers/HeadersTaskProducer.code
  TaskProducers/BuildPhaseTaskProducers/ResourcesTaskProducer.code
  TaskProducers/BuildPhaseTaskProducers/RezTaskProducer.code
  TaskProducers/BuildPhaseTaskProducers/ShellBasedTaskProducer.code
  TaskProducers/BuildPhaseTaskProducers/ShellScriptTaskProducer.code
  TaskProducers/BuildPhaseTaskProducers/SourcesTaskProducer.code
  TaskProducers/BuildPhaseTaskProducers/SwiftPackageCopyFilesTaskProducer.code
  TaskProducers/OtherTaskProducers/CopySwiftPackageResourcesTaskProducer.code
  TaskProducers/OtherTaskProducers/CustomTaskProducer.code
  TaskProducers/OtherTaskProducers/DevelopmentAssetsTaskProducer.code
  TaskProducers/OtherTaskProducers/DocumentationTaskProducer.code
  TaskProducers/OtherTaskProducers/ExternalTargetTaskProducer.code
  TaskProducers/OtherTaskProducers/GenerateAppPlaygroundAssetCatalogTaskProducer.code
  TaskProducers/OtherTaskProducers/GeneratedFilesTaskProducer.code
  TaskProducers/OtherTaskProducers/HeadermapTaskProducer.code
  TaskProducers/OtherTaskProducers/InfoPlistTaskProducer.code
  TaskProducers/OtherTaskProducers/ModuleMapTaskProducer.code
  TaskProducers/OtherTaskProducers/ModuleVerifierTaskProducer.code
  TaskProducers/OtherTaskProducers/ProductPostprocessingTaskProducer.code
  TaskProducers/OtherTaskProducers/ProductStructureTaskProducer.code
  TaskProducers/OtherTaskProducers/SanitizerTaskProducer.code
  TaskProducers/OtherTaskProducers/SDKStatCacheTaskProducer.code
  TaskProducers/OtherTaskProducers/SwiftFrameworkABICheckerTaskProducer.code
  TaskProducers/OtherTaskProducers/SwiftStandardLibrariesTaskProducer.code
  TaskProducers/OtherTaskProducers/TAPISymbolExtractorTaskProducer.code
  TaskProducers/OtherTaskProducers/TargetOrderTaskProducer.code
  TaskProducers/OtherTaskProducers/XCTestProductTypeTaskProducer.code
  TaskProducers/StandardTaskProducer.code
  TaskProducers/TaskProducer.code
  TaskProducers/TaskProducerExtensionPoint.code
  TaskProducers/WorkspaceTaskProducers/CreateBuildDirectoryTaskProducer.code
  TaskProducers/WorkspaceTaskProducers/HeadermapVFSTaskProducer.code
  TaskProducers/WorkspaceTaskProducers/IndexBuildVFSDirectoryRemapTaskProducer.code
  TaskProducers/WorkspaceTaskProducers/PCHModuleMapTaskProducer.code
  TaskProducers/WorkspaceTaskProducers/XCFrameworkTaskProducer.code
  TaskProducerSandboxing.code
  XCFrameworkContext.code)
set_target_properties(SWBTaskConstruction PROPERTIES
  Swift_LANGUAGE_VERSION 5)
target_link_libraries(SWBTaskConstruction PUBLIC
  SWBCore
  SWBUtil)

set_target_properties(SWBTaskConstruction PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_Swift_MODULE_DIRECTORY})

set_property(GLOBAL APPEND PROPERTY SWIFTBUILD_EXPORTS SWBTaskConstruction)

install(TARGETS SWBTaskConstruction
    ARCHIVE DESTINATION "${SwiftBuild_INSTALL_LIBDIR}")
