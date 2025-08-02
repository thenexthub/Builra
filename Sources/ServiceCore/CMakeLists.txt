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

add_library(SWBServiceCore
  Request.code
  Service.code
  ServiceEntryPoint.code
  ServiceExtensionPoint.code
  ServiceHostConnection.code)
set_target_properties(SWBServiceCore PROPERTIES
  Swift_LANGUAGE_VERSION 6)
target_link_libraries(SWBServiceCore PUBLIC
  SWBProtocol)

set_target_properties(SWBServiceCore PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_Swift_MODULE_DIRECTORY})

set_property(GLOBAL APPEND PROPERTY SWIFTBUILD_EXPORTS SWBServiceCore)

install(TARGETS SWBServiceCore
    ARCHIVE DESTINATION "${SwiftBuild_INSTALL_LIBDIR}")
