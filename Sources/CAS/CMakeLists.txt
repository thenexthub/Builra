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

add_library(SWBCAS
  CASFSNode.code
  CASProtocol.code
  Errors.code
  plugin_api_t.code
  ToolchainCASPlugin.code)
set_target_properties(SWBCAS PROPERTIES
  Swift_LANGUAGE_VERSION 6)
target_link_libraries(SWBCAS PUBLIC
  SWBUtil
  SWBCSupport)

set_target_properties(SWBCAS PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_Swift_MODULE_DIRECTORY})

set_property(GLOBAL APPEND PROPERTY SWIFTBUILD_EXPORTS SWBCAS)

install(TARGETS SWBCAS
    ARCHIVE DESTINATION "${SwiftBuild_INSTALL_LIBDIR}")
