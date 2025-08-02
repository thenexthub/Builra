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

add_library(SwiftBuild
  CompatibilityShims.code
  ConsoleCommands/SWBServiceConsoleBuildCommand.code
  ConsoleCommands/SWBServiceConsoleBuildCommandProtocol.code
  ConsoleCommands/SWBServiceConsoleCreateXCFrameworkCommand.code
  ConsoleCommands/SWBServiceConsoleGeneralCommands.code
  ConsoleCommands/SWBServiceConsoleSessionCommands.code
  ConsoleCommands/SWBServiceConsoleXcodeCommands.code
  ProjectModel/BuildConfig.code
  ProjectModel/BuildFile.code
  ProjectModel/BuildPhases.code
  ProjectModel/BuildRule.code
  ProjectModel/BuildSettings.code
  ProjectModel/CustomTask.code
  ProjectModel/ImpartedBuildProperties.code
  ProjectModel/PlatformFilter.code
  ProjectModel/Project.code
  ProjectModel/ProjectModel.code
  ProjectModel/References.code
  ProjectModel/SandboxingOverride.code
  ProjectModel/TargetDependency.code
  ProjectModel/Targets.code
  SWBBuildAction.code
  SWBBuildOperation.code
  SWBBuildOperationBacktraceFrame.code
  SWBBuildParameters.code
  SWBBuildRequest.code
  SWBBuildService.code
  SWBBuildServiceConnection.code
  SWBBuildServiceConsole.code
  SWBBuildServiceSession.code
  SWBChannel.code
  SWBClientExchangeSupport.code
  SWBDocumentationSupport.code
  SWBIndexingSupport.code
  SWBLocalizationSupport.code
  SWBMacroEvaluation.code
  SWBPreviewSupport.code
  SWBProductPlannerSupport.code
  SWBPropertyList.code
  SWBProvisioningTaskInputs.code
  SWBSystemInfo.code
  SWBTargetGUID.code
  SWBTerminal.code
  SWBuildMessage+Protocol.code
  SWBUserInfo.code
  SWBWorkspaceInfo.code
  SwiftBuild.code
  SwiftBuildVersion.code
  TerminalAttributes.code)
set_target_properties(SwiftBuild PROPERTIES
  Swift_LANGUAGE_VERSION 5)
target_link_libraries(SwiftBuild PUBLIC
  SWBCSupport
  SWBCore
  SWBProtocol
  SWBUtil
  SWBProjectModel)

set_target_properties(SwiftBuild PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_Swift_MODULE_DIRECTORY})

set_property(GLOBAL APPEND PROPERTY SWIFTBUILD_EXPORTS SwiftBuild)

install(TARGETS SwiftBuild
    ARCHIVE DESTINATION "${SwiftBuild_INSTALL_LIBDIR}")
