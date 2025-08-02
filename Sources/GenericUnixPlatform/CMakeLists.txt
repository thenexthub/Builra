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

add_library(SWBGenericUnixPlatform
  Plugin.code)
SwiftBuild_Bundle(MODULE SWBGenericUnixPlatform FILES
  Specs/Unix.xcspec
  Specs/UnixCompile.xcspec
  Specs/UnixLd.xcspec
  Specs/UnixLibtool.xcspec)
target_link_libraries(SWBGenericUnixPlatform PUBLIC
  SWBCore
  SWBUtil)

set_target_properties(SWBGenericUnixPlatform PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_Swift_MODULE_DIRECTORY})

set_property(GLOBAL APPEND PROPERTY SWIFTBUILD_EXPORTS SWBGenericUnixPlatform)

install(TARGETS SWBGenericUnixPlatform
    ARCHIVE DESTINATION "${SwiftBuild_INSTALL_LIBDIR}")
