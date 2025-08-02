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

add_library(SWBProtocol
  AsyncSequence.code
  BuildAction.code
  BuildOperationMessages.code
  BuildSettingsInfoMessageTypes.code
  ClientExchangeMessages.code
  DependencyClosureMessages.code
  DependencyGraphMessages.code
  DocumentationMessages.code
  IndexingMessages.code
  LocalizationMessages.code
  MacroEvaluationMessages.code
  Message.code
  MessageSupport.code
  PIFKeyConstants.code
  PlanningOperationMessages.code
  PreviewMessages.code
  ProjectDescriptorMessages.code
  ProjectDescriptorTypes.code
  ProjectModel/BuildConfiguration.code
  ProjectModel/BuildFile.code
  ProjectModel/BuildPhase.code
  ProjectModel/BuildRule.code
  ProjectModel/CustomTask.code
  ProjectModel/DependencyInfo.code
  ProjectModel/ImpartedBuildProperties.code
  ProjectModel/MacroExpressionSource.code
  ProjectModel/PIFObject.code
  ProjectModel/PlatformFilter.code
  ProjectModel/Project.code
  ProjectModel/ProvisioningSourceData.code
  ProjectModel/Reference.code
  ProjectModel/SourceTree.code
  ProjectModel/Target.code
  ProjectModel/TargetDependency.code
  ProjectModel/Workspace.code)
set_target_properties(SWBProtocol PROPERTIES
  Swift_LANGUAGE_VERSION 6)
target_link_libraries(SWBProtocol PUBLIC
  SWBUtil)

set_target_properties(SWBProtocol PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_Swift_MODULE_DIRECTORY})

set_property(GLOBAL APPEND PROPERTY SWIFTBUILD_EXPORTS SWBProtocol)

install(TARGETS SWBProtocol
    ARCHIVE DESTINATION "${SwiftBuild_INSTALL_LIBDIR}")
