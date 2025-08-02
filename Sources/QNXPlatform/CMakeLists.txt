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

add_library(SWBQNXPlatform
  Plugin.code
  QNXSDP.code)
SwiftBuild_Bundle(MODULE SWBQNXPlatform FILES
  Specs/QNX.xcspec
  Specs/QNXCompile.xcspec
  Specs/QNXLd.xcspec
  Specs/QNXLibtool.xcspec)
target_link_libraries(SWBQNXPlatform PUBLIC
  SWBCore
  SWBMacro
  SWBUtil)

set_target_properties(SWBQNXPlatform PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_Swift_MODULE_DIRECTORY})

set_property(GLOBAL APPEND PROPERTY SWIFTBUILD_EXPORTS SWBQNXPlatform)

install(TARGETS SWBQNXPlatform
    ARCHIVE DESTINATION "${SwiftBuild_INSTALL_LIBDIR}")
