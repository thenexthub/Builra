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

add_library(SWBWebAssemblyPlatform
  Plugin.code)
SwiftBuild_Bundle(MODULE SWBWebAssemblyPlatform FILES
  Specs/WasmCompile.xcspec
  Specs/WasmLd.xcspec
  Specs/WasmLibtool.xcspec
  Specs/WebAssembly.xcspec)
target_link_libraries(SWBWebAssemblyPlatform PUBLIC
  SWBCore
  SWBMacro
  SWBUtil)
set_target_properties(SWBWebAssemblyPlatform PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_Swift_MODULE_DIRECTORY})

set_property(GLOBAL APPEND PROPERTY SWIFTBUILD_EXPORTS SWBWebAssemblyPlatform)

install(TARGETS SWBWebAssemblyPlatform
    ARCHIVE DESTINATION "${SwiftBuild_INSTALL_LIBDIR}")
