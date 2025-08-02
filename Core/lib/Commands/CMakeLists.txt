add_builra_library(builraCommands STATIC
  BuildEngineCommand.cpp
  BuildSystemCommand.cpp
  CommandLineStatusOutput.cpp
  CommandUtil.cpp
  NinjaBuildCommand.cpp
  NinjaCommand.cpp
  )

target_link_libraries(builraCommands PRIVATE
  builraBuildSystem
  builraCore
  builraNinja)
