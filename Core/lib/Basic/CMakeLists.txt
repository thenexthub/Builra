add_builra_library(builraBasic STATIC
  ExecutionQueue.cpp
  FileInfo.cpp
  FileSystem.cpp
  Hashing.cpp
  LaneBasedExecutionQueue.cpp
  PlatformUtility.cpp
  SerialQueue.cpp
  Subprocess.cpp
  Tracing.cpp
  Version.cpp
  ShellUtility.cpp
  )

target_link_libraries(builraBasic PRIVATE
  llvmSupport
  Threads::Threads)

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
  target_link_libraries(builraBasic PUBLIC
    ShLwApi.lib)
endif()

set_property(GLOBAL APPEND PROPERTY Builra_EXPORTS builraBasic)
