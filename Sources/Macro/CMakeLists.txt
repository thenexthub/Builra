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

add_library(SWBMacro
  MacroCondition.code
  MacroConditionExpression.code
  MacroConditionParameter.code
  MacroConditionSet.code
  MacroConfigFileDiagnostic.code
  MacroConfigFileParser.code
  MacroDeclaration.code
  MacroEvaluationProgram.code
  MacroEvaluationScope.code
  MacroExpression.code
  MacroExpressionDiagnostic.code
  MacroExpressionParsing.code
  MacroNamespace.code
  MacroType.code
  MacroValueAssignmentTable.code)
set_target_properties(SWBMacro PROPERTIES
  Swift_LANGUAGE_VERSION 6)
target_link_libraries(SWBMacro PUBLIC
  SWBUtil
  SwiftDriver
  TSCBasic)

set_target_properties(SWBMacro PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_Swift_MODULE_DIRECTORY})

set_property(GLOBAL APPEND PROPERTY SWIFTBUILD_EXPORTS SWBMacro)

install(TARGETS SWBMacro
    ARCHIVE DESTINATION "${SwiftBuild_INSTALL_LIBDIR}")
