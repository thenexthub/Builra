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

add_library(SWBCLibc
  libc.c)
target_include_directories(SWBCLibc INTERFACE
  include)

target_include_directories(SWBCLibc PUBLIC
  ${CMAKE_CURRENT_BINARY_DIR})

set_property(GLOBAL APPEND PROPERTY SWIFTBUILD_EXPORTS SWBCLibc)

install(TARGETS SWBCLibc
    ARCHIVE DESTINATION "${SwiftBuild_INSTALL_LIBDIR}")
