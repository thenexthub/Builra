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

add_executable(SWBBuildServiceBundle
  main.code)
set_target_properties(SWBBuildServiceBundle PROPERTIES
  Swift_LANGUAGE_VERSION 6)
target_link_libraries(SWBBuildServiceBundle PRIVATE
  SWBBuildService
  SWBBuildSystem
  SWBServiceCore
  SWBUtil
  SWBCore)

install(TARGETS SWBBuildServiceBundle)

set_property(GLOBAL APPEND PROPERTY SWIFTBUILD_EXPORTS SWBBuildServiceBundle)
