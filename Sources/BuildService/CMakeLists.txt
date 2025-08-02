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

add_library(SWBBuildService
  BuildDependencyInfo.code
  BuildOperationMessages.code
  BuildService.code
  BuildServiceEntryPoint.code
  ClientExchangeDelegate.code
  DependencyGraphMessages.code
  DocumentationInfo.code
  LocalizationInfo.code
  Messages.code
  PlanningOperation.code
  PreviewInfo.code
  ProjectDescriptor.code
  Session.code
  Tools.code)
set_target_properties(SWBBuildService PROPERTIES
  Swift_LANGUAGE_VERSION 5)
target_link_libraries(SWBBuildService PUBLIC
  SWBBuildSystem
  SWBServiceCore
  SWBTaskExecution
  SWBAndroidPlatform
  SWBApplePlatform
  SWBGenericUnixPlatform
  SWBQNXPlatform
  SWBUniversalPlatform
  SWBWebAssemblyPlatform
  SWBWindowsPlatform
  $<$<NOT:$<PLATFORM_ID:Darwin>>:SwiftSystem::SystemPackage>)

set_target_properties(SWBBuildService PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_Swift_MODULE_DIRECTORY})

set_property(GLOBAL APPEND PROPERTY SWIFTBUILD_EXPORTS SWBBuildService)

install(TARGETS SWBBuildService
    ARCHIVE DESTINATION "${SwiftBuild_INSTALL_LIBDIR}")
