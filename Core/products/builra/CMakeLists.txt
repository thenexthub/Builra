add_executable(builra
  main.cpp)

target_link_libraries(builra PRIVATE
  builraCommands
  builraNinja
  builraBuildSystem
  builraCore
  builraBasic
  llvmSupport
  SQLite::SQLite3)

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    set_target_properties(builra PROPERTIES LINK_FLAGS "/INCREMENTAL:NO")
endif()

if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  target_link_libraries(builra PRIVATE
    curses)
endif()
