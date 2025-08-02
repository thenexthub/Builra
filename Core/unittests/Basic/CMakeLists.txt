add_builra_unittest(BasicTests
  BinaryCodingTests.cpp
  Defer.cpp
  FileSystemTest.cpp
  POSIXEnvironmentTest.cpp
  SerialQueueTest.cpp
  ShellUtilityTest.cpp
  ../BuildSystem/TempDir.cpp
  )

target_link_libraries(BasicTests PRIVATE
  builraBasic
  llvmSupport)

if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  target_link_libraries(BasicTests PRIVATE
    curses)
endif()
