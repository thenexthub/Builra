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

add_library(SWBUniversalPlatform
  CopyPlistFile.code
  CopyStringsFile.code
  CppTool.code
  DiffTool.code
  LexCompiler.code
  TestEntryPointGenerationTaskAction.code
  TestEntryPointGenerationTool.code
  TestEntryPointTaskProducer.code
  YaccCompiler.code
  Plugin.code)
SwiftBuild_Bundle(MODULE SWBUniversalPlatform FILES
  Specs/BuiltInBuildRules.xcbuildrules
  Specs/BuiltInCompilers.xcspec
  Specs/BuiltInFileTypes.xcspec
  Specs/Clang.xcspec
  Specs/ClangModuleVerifierInputGenerator.xcspec
  Specs/ClangStatCache.xcspec
  Specs/ClangSymbolExtractor.xcspec
  Specs/ClangVerifier.xcspec
  Specs/CodeSign.xcspec
  Specs/CopyPlistFile.xcspec
  Specs/CopyStringsFile.xcspec
  Specs/Cpp.xcspec
  Specs/DefaultCompiler.xcspec
  Specs/Documentation.xcspec
  Specs/Ld.xcspec
  Specs/Lex.xcspec
  Specs/Libtool.xcspec
  Specs/PackageTypes.xcspec
  Specs/PBXCp.xcspec
  Specs/ProductTypes.xcspec
  Specs/ProductTypeValidationTool.xcspec
  Specs/StandardFileTypes.xcspec
  Specs/StripSymbols.xcspec
  Specs/swift-stdlib-tool.xcspec
  Specs/Swift.xcspec
  Specs/SwiftBuildSettings.xcspec
  Specs/TAPI.xcspec
  Specs/TestEntryPointGenerator.xcspec
  Specs/Unifdef.xcspec
  Specs/Yacc.xcspec)
target_link_libraries(SWBUniversalPlatform PUBLIC
  SWBCore
  SWBMacro
  SWBUtil
  SWBTaskConstruction
  SWBTaskExecution
  ArgumentParser)

set_target_properties(SWBUniversalPlatform PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_Swift_MODULE_DIRECTORY})

set_property(GLOBAL APPEND PROPERTY SWIFTBUILD_EXPORTS SWBUniversalPlatform)

install(TARGETS SWBUniversalPlatform
    ARCHIVE DESTINATION "${SwiftBuild_INSTALL_LIBDIR}")
