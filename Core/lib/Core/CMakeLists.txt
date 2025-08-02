add_builra_library(builraCore STATIC
  BuildDB.cpp
  BuildEngine.cpp
  BuildEngineTrace.cpp
  DependencyInfoParser.cpp
  MakefileDepsParser.cpp
  SQLiteBuildDB.cpp
)

target_link_libraries(builraCore PRIVATE
  builraBasic
  llvmSupport
  SQLite::SQLite3)

set_property(GLOBAL APPEND PROPERTY Builra_EXPORTS builraCore)
