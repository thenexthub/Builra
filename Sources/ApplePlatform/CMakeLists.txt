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

add_library(SWBApplePlatform
  AppIntentsMetadataCompiler.code
  AppIntentsMetadataTaskProducer.code
  AppIntentsSSUTrainingCompiler.code
  AssetCatalogCompiler.code
  CopyTiffFile.code
  CoreDataCompiler.code
  CoreMLCompiler.code
  DittoTool.code
  ExtensionPointExtractorTaskProducer.code
  ExtensionPointsCompiler.code
  EXUtil.code
  IIGCompiler.code
  InstrumentsPackageBuilderSpec.code
  IntentsCompiler.code
  InterfaceBuilderCompiler.code
  InterfaceBuilderShared.code
  MetalCompiler.code
  MiGCompiler.code
  OpenCLCompiler.code
  RealityAssetsCompilerSpec.code
  ReferenceObjectCompiler.code
  ResMergerLinkerSpec.code
  SceneKitToolSpec.code
  StoryboardLinker.code
  XCStringsCompiler.code
  ActoolInputFileGroupingStrategy.code
  AssetCatalogCompilerOutputParser.code
  CMakeLists.txt
  DevelopmentAssetsTaskProducer.code
  ImageScaleFactorsInputFileGroupingStrategy.code
  InterfaceBuilderCompilerOutputParser.code
  LocalizationInputFileGroupingStrategy.code
  MacCatalystInfoExtension.code
  Plugin.code
  RealityAssetsTaskProducer.code
  StringCatalogCompilerOutputParser.code
  StubBinaryTaskProducer.code
  XCStringsInputFileGroupingStrategy.code)
SwiftBuild_Bundle(MODULE SWBApplePlatform FILES
  Specs/AppIntentsMetadata.xcspec
  Specs/AppIntentsNLTraining.xcspec
  Specs/AppShortcutStringsMetadata.xcspec
  Specs/AssetCatalogCompiler.xcspec
  Specs/CompileSkybox.xcspec
  Specs/CopyPNGFile.xcspec
  Specs/CopyTiffFile.xcspec
  Specs/CoreData.xcspec
  Specs/CoreML.xcspec
  Specs/DarwinPackageTypes.xcspec
  Specs/DarwinProductTypes.xcspec
  Specs/DriverKit.xcspec
  Specs/DTrace.xcspec
  Specs/Embedded-Device.xcspec
  Specs/Embedded-Shared.xcspec
  Specs/Embedded-Simulator.xcspec
  Specs/EmbeddedBinaryValidationUtility.xcspec
  Specs/EXUtil.xcspec
  Specs/GenerateAppPlaygroundAssetCatalog.xcspec
  Specs/GenerateTextureAtlas.xcspec
  Specs/IBCompiler.xcspec
  Specs/IBPostprocessor.xcspec
  Specs/IBStoryboardCompiler.xcspec
  Specs/IBStoryboardLinker.xcspec
  Specs/IBStoryboardPostprocessor.xcspec
  Specs/Iconutil.xcspec
  Specs/Iig.xcspec
  Specs/InfoPlistUtility.xcspec
  Specs/InstrumentsPackage.xcspec
  Specs/Intents.xcspec
  Specs/InterfaceBuilderFileTypes.xcspec
  Specs/iOSDevice.xcspec
  Specs/iOSShared.xcspec
  Specs/iOSSimulator.xcspec
  Specs/KernelExtension.xcspec
  Specs/Lipo.xcspec
  Specs/LSRegisterURL.xcspec
  Specs/macOSArchitectures.xcspec
  Specs/macOSCoreBuildSystem.xcspec
  Specs/macOSNativeBuildSystem.xcspec
  Specs/macOSPackageTypes.xcspec
  Specs/macOSProductTypes.xcspec
  Specs/MetalCompiler.xcspec
  Specs/MetalFileTypes.xcspec
  Specs/MetalLinker.xcspec
  Specs/MetalPackageTypes.xcspec
  Specs/MetalProductTypes.xcspec
  Specs/MiG.xcspec
  Specs/OpenCL.xcspec
  Specs/OSACompile.xcspec
  Specs/RCFileTypes.xcspec
  Specs/RealityAssets.xcspec
  Specs/ReferenceObject.xcspec
  Specs/ResMerger.xcspec
  Specs/Rez.xcspec
  Specs/SceneKitFileTypes.xcspec
  Specs/SceneKitTools.xcspec
  Specs/SpriteKitFileTypes.xcspec
  Specs/TiffUtil.xcspec
  Specs/tvOSDevice.xcspec
  Specs/tvOSShared.xcspec
  Specs/tvOSSimulator.xcspec
  Specs/WatchKit1ProductTypes.xcspec
  Specs/watchOSDevice.xcspec
  Specs/watchOSShared.xcspec
  Specs/watchOSSimulator.xcspec
  Specs/XCAppExtensionPoints.xcspec
  Specs/XCStrings.xcspec
  Specs/xrOSDevice.xcspec
  Specs/xrOSShared.xcspec
  Specs/xrOSSimulator.xcspec)
set_target_properties(SWBApplePlatform PROPERTIES
  Swift_LANGUAGE_VERSION 6)
target_link_libraries(SWBApplePlatform PUBLIC
  SWBCore
  SWBMacro
  SWBUtil
  SWBProtocol
  SWBTaskConstruction)

set_target_properties(SWBApplePlatform PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_Swift_MODULE_DIRECTORY})

set_property(GLOBAL APPEND PROPERTY SWIFTBUILD_EXPORTS SWBApplePlatform)

install(TARGETS SWBApplePlatform
    ARCHIVE DESTINATION "${SwiftBuild_INSTALL_LIBDIR}")
