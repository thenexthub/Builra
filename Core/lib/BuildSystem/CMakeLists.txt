add_builra_library(builraBuildSystem STATIC
  BuildDescription.cpp
  BuildFile.cpp
  BuildKey.cpp
  BuildNode.cpp
  BuildSystem.cpp
  BuildSystemExtensionManager.cpp
  BuildSystemFrontend.cpp
  BuildValue.cpp
  ExternalCommand.cpp
  ShellCommand.cpp
  )

target_link_libraries(builraBuildSystem PRIVATE
  builraCore
  builraBasic
  llvmSupport)

set_property(GLOBAL APPEND PROPERTY Builra_EXPORTS builraBuildSystem)
