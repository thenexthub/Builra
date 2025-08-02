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

add_library(SWBCore
  ActivityReporting.code
  Apple/DeviceFamily.code
  Apple/InterfaceBuilderShared.code
  BuildFileFilteringContext.code
  BuildFileResolution.code
  BuildParameters.code
  BuildRequest.code
  BuildRequestContext.code
  BuildRuleAction.code
  BuildRuleCondition.code
  BuildRuleSet.code
  CapturedBuildInfo.code
  ClangModuleVerifier/ModuleVerifierFilenameMap.code
  ClangModuleVerifier/ModuleVerifierFramework.code
  ClangModuleVerifier/ModuleVerifierHeader.code
  ClangModuleVerifier/ModuleVerifierLanguage.code
  ClangModuleVerifier/ModuleVerifierModuleMap.code
  ClangModuleVerifier/ModuleVerifierModuleMapFileVerifier.code
  ClangModuleVerifier/ModuleVerifierTarget.code
  ClangModuleVerifierOutputParser.code
  ClangSerializedDiagnostics.code
  CommandLineArgument.code
  ConfiguredTarget.code
  Core.code
  CustomTaskTypeDescription.code
  Dependencies.code
  DependencyInfoEditPayload.code
  DependencyResolution.code
  DiagnosticSupport.code
  EnvironmentBindings.code
  ExecutableTask.code
  Extensions/DeveloperDirectoryExtension.code
  Extensions/DiagnosticToolingExtension.code
  Extensions/EnvironmentExtension.code
  Extensions/FeatureAvailabilityExtension.code
  Extensions/InputFileGroupingStrategyExtension.code
  Extensions/PlatformInfoExtension.code
  Extensions/SDKRegistryExtension.code
  Extensions/SDKVariantInfoExtension.code
  Extensions/SettingsBuilderExtension.code
  Extensions/SpecificationsExtension.code
  Extensions/ToolchainRegistryExtension.code
  FileSystemSignatureBasedCache.code
  FileToBuild.code
  LibclangVendored/ArrayExtensions.code
  LibclangVendored/CStringArray.code
  LibclangVendored/Libclang.code
  LibSwiftDriver/LibSwiftDriver.code
  LibSwiftDriver/PlannedBuild.code
  LinkageDependencyResolver.code
  MacCatalystInfo.code
  MacroConfigFileLoader.code
  MacroEvaluationExtensions.code
  MacroExpressionSourceExtensions.code
  OnDemandResources.code
  OptimizationRemarks.code
  PlannedNode.code
  PlannedTask.code
  PlannedTaskAction.code
  PlatformEnvironment.code
  PlatformFiltering.code
  PlatformRegistry.code
  Process.code
  ProcessExecutionCache.code
  ProductTypeIdentifier.code
  ProjectModel/BuildConfiguration.code
  ProjectModel/BuildFile.code
  ProjectModel/BuildPhase.code
  ProjectModel/BuildRule.code
  ProjectModel/DependencyInfoFormat.code
  ProjectModel/FilePathResolver.code
  ProjectModel/FileTextEncoding.code
  ProjectModel/ImpartedBuildProperties.code
  ProjectModel/PIFLoader.code
  ProjectModel/PlatformFilter.code
  ProjectModel/Project.code
  ProjectModel/ProjectModelItem.code
  ProjectModel/Reference.code
  ProjectModel/ReferenceLookupContext.code
  ProjectModel/Target.code
  ProjectModel/Workspace.code
  ProjectModel/WorkspaceHeaderIndex.code
  ProvisionalTask.code
  Provisioning.code
  ProvisioningTaskInputs.code
  SDKRegistry.code
  Settings/BuildRuleFile.code
  Settings/BuiltinMacros.code
  Settings/CASOptions.code
  Settings/RecursiveSearchPathResolver.code
  Settings/Settings.code
  Settings/StackedSearchPaths.code
  ShellScript.code
  SigningSupport.code
  SwiftSDK.code
  SpecImplementations/CommandLineToolSpec.code
  SpecImplementations/CompilerSpec.code
  Specs/CoreBuildSystem.xcspec
  Specs/ExternalBuildSystem.xcspec
  SpecImplementations/FileTypes.code
  SpecImplementations/LinkerSpec.code
  Specs/NativeBuildSystem.xcspec
  SpecImplementations/ProductTypes.code
  SpecImplementations/PropertyDomainSpec.code
  SpecImplementations/RegisterSpecs.code
  SpecImplementations/SpecParser.code
  SpecImplementations/SpecRegistry.code
  SpecImplementations/Specs.code
  SpecImplementations/Tools/AppShortcutStringsMetadataCompiler.code
  SpecImplementations/Tools/CCompiler.code
  SpecImplementations/Tools/ClangModuleVerifierInputGenerator.code
  SpecImplementations/Tools/ClangStatCache.code
  SpecImplementations/Tools/CodeSign.code
  SpecImplementations/Tools/ConcatenateTool.code
  SpecImplementations/Tools/ConstructStubExecutorFileListTool.code
  SpecImplementations/Tools/CopyTool.code
  SpecImplementations/Tools/CreateAssetPackManifestTool.code
  SpecImplementations/Tools/CreateBuildDirectory.code
  SpecImplementations/Tools/DocumentationCompiler.code
  SpecImplementations/Tools/DsymutilTool.code
  SpecImplementations/Tools/Gate.code
  SpecImplementations/Tools/GCCCompatibleCompilerSupport.code
  SpecImplementations/Tools/GenerateAppPlaygroundAssetCatalog.code
  SpecImplementations/Tools/InfoPlistTool.code
  SpecImplementations/Tools/LaunchServicesRegisterTool.code
  SpecImplementations/Tools/LinkerTools.code
  SpecImplementations/Tools/Lipo.code
  SpecImplementations/Tools/MergeInfoPlist.code
  SpecImplementations/Tools/MkdirTool.code
  SpecImplementations/Tools/ModulesVerifierTool.code
  SpecImplementations/Tools/PLUtilTool.code
  SpecImplementations/Tools/PrelinkedObjectLink.code
  SpecImplementations/Tools/ProcessSDKImports.code
  SpecImplementations/Tools/ProcessXCFrameworkLibrary.code
  SpecImplementations/Tools/ProductPackaging.code
  SpecImplementations/Tools/RegisterExecutionPolicyException.code
  SpecImplementations/Tools/SetAttributes.code
  SpecImplementations/Tools/ShellScriptTool.code
  SpecImplementations/Tools/SignatureCollection.code
  SpecImplementations/Tools/StripTool.code
  SpecImplementations/Tools/SwiftABICheckerTool.code
  SpecImplementations/Tools/SwiftABIGenerationTool.code
  SpecImplementations/Tools/SwiftCompiler.code
  SpecImplementations/Tools/SwiftHeaderTool.code
  SpecImplementations/Tools/SwiftStdLibTool.code
  SpecImplementations/Tools/SwiftSymbolExtractor.code
  SpecImplementations/Tools/SymlinkTool.code
  SpecImplementations/Tools/TAPISymbolExtractor.code
  SpecImplementations/Tools/TAPITools.code
  SpecImplementations/Tools/TiffUtilTool.code
  SpecImplementations/Tools/TouchTool.code
  SpecImplementations/Tools/UnifdefTool.code
  SpecImplementations/Tools/ValidateDependencies.code
  SpecImplementations/Tools/ValidateDevelopmentAssets.code
  SpecImplementations/Tools/ValidateEmbeddedBinaryTool.code
  SpecImplementations/Tools/ValidateProductTool.code
  SpecImplementations/Tools/WriteFile.code
  SWBFeatureFlag.code
  TargetDependencyResolver.code
  TargetPlatformDiagnostics.code
  TaskGeneration.code
  TaskResult.code
  ToolchainRegistry.code
  ToolInfo/ClangToolInfo.code
  Tuning.code
  WorkspaceContext.code
  WorkspaceSettingsCache.code
  XCFramework.code)
SwiftBuild_Bundle(MODULE SWBCore FILES
  Specs/CoreBuildSystem.xcspec
  Specs/ExternalBuildSystem.xcspec
  Specs/NativeBuildSystem.xcspec)
set_target_properties(SWBCore PROPERTIES
  Swift_LANGUAGE_VERSION 5)
target_link_libraries(SWBCore PUBLIC
  SWBMacro
  SWBProtocol
  SWBServiceCore
  SWBUtil
  SWBCAS
  SWBBuilra
  SwiftDriver)

set_target_properties(SWBCore PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_Swift_MODULE_DIRECTORY})

set_property(GLOBAL APPEND PROPERTY SWIFTBUILD_EXPORTS SWBCore)

install(TARGETS SWBCore
    ARCHIVE DESTINATION "${SwiftBuild_INSTALL_LIBDIR}")
