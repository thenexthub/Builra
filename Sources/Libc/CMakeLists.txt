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

add_library(SWBLibc
  libc.code)
set_target_properties(SWBLibc PROPERTIES
  Swift_LANGUAGE_VERSION 6)
target_link_libraries(SWBLibc PUBLIC
  SWBCLibc)

# FIXME: why is this needed? We should be wiring up the Swift_MODULE_DIRECTORY
# without this explicit configuration.
target_include_directories(SWBLibc PUBLIC
  ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(SWBLibc PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_Swift_MODULE_DIRECTORY})

set_property(GLOBAL APPEND PROPERTY SWIFTBUILD_EXPORTS SWBLibc)

install(TARGETS SWBLibc
    ARCHIVE DESTINATION "${SwiftBuild_INSTALL_LIBDIR}")
