add_builra_unittest(BuildSystemTests
  BuildSystemFrontendTest.cpp
  BuildSystemTaskTests.cpp
  BuildValueTest.cpp
  MockBuildSystemDelegate.cpp
  TempDir.cpp
  )

target_link_libraries(BuildSystemTests PRIVATE
  builraBuildSystem
  llvmSupport
  builraCore
  builraBasic
  llvmSupport
  SQLite::SQLite3)

if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  target_link_libraries(BuildSystemTests PRIVATE
    curses)
endif()
