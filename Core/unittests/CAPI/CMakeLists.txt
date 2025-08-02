
include_directories(BEFORE
  ${CMAKE_SOURCE_DIR}/products/libbuilra/include)

add_builra_unittest(CAPITests
  C-API.cpp
  BuildSystem-C-API.cpp
  )

target_link_libraries(CAPITests PRIVATE
  llvmSupport
  builraBasic
  libbuilra
  SQLite::SQLite3)

if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  target_link_libraries(CAPITests PRIVATE
    curses)
endif()
