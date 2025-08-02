set(SOURCES 
  BuildSystem-C-API.cpp
  C-API.cpp
  Core-C-API.cpp
  BuildDB-C-API.cpp
  BuildKey-C-API.cpp
  BuildValue-C-API.cpp
  Ninja-C-API.cpp)

add_builra_library(libbuilra
  ${SOURCES}
  STATIC
  OUTPUT_NAME builra)

set_property(TARGET libbuilra PROPERTY MACOSX_RPATH ON)

target_link_libraries(libbuilra PRIVATE
  builraBuildSystem
  builraCore
  builraBasic
  builraNinja
  llvmSupport
  SQLite::SQLite3)

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    set_target_properties(libbuilra PROPERTIES LINK_FLAGS "/INCREMENTAL:NO")
endif()

if(NOT CMAKE_SYSTEM_NAME STREQUAL Windows)
  target_link_libraries(libbuilra PRIVATE
    curses)
endif()

target_include_directories(libbuilra
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

include_directories(BEFORE
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(DIRECTORY include/
  DESTINATION include
  COMPONENT libbuilra
  FILES_MATCHING
  PATTERN "*.h")

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/
  DESTINATION include
  COMPONENT libbuilra
  FILES_MATCHING
  PATTERN "*.h")

install(TARGETS libbuilra
  ARCHIVE DESTINATION lib${BUILRA_LIBDIR_SUFFIX}
  LIBRARY DESTINATION lib${BUILRA_LIBDIR_SUFFIX}
  RUNTIME DESTINATION bin
  COMPONENT libbuilra)
set_property(GLOBAL APPEND PROPERTY Builra_EXPORTS libbuilra)

add_custom_target(install-libbuilra
                  DEPENDS libbuilra
                  COMMENT "Installing libbuilra..."
                  COMMAND "${CMAKE_COMMAND}"
                          -DCMAKE_INSTALL_COMPONENT=libbuilra
                          -P "${CMAKE_BINARY_DIR}/cmake_install.cmake")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # On OS X, add a target to create the builra.framework product.
  add_library(builra-framework SHARED ${SOURCES})
  set_target_properties(builra-framework PROPERTIES FRAMEWORK 1)
  set_target_properties(builra-framework PROPERTIES MACOSX_RPATH "@rpath")
  set_target_properties(builra-framework PROPERTIES OUTPUT_NAME builra)
  set_target_properties(builra-framework PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
  set_target_properties(builra-framework PROPERTIES LINK_FLAGS " -compatibility_version 1 -current_version 1")
  target_link_libraries(builra-framework PRIVATE
    builraBuildSystem
    builraCore
    builraBasic
    builraNinja
    llvmSupport
    SQLite::SQLite3
    curses)

  # Manually set up the remaining framework structure.
  set(BUILRA_FW_INPUTS)

  # Copy the public headers.
  file(GLOB headers include/builra/*.h)
  foreach(header ${headers})
    get_filename_component(name ${header} NAME)
    set(dst ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/builra.framework/Versions/A/Headers/${name})
    list(APPEND BUILRA_FW_INPUTS ${dst})
    add_custom_command(OUTPUT ${dst} COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/builra.framework/Versions/A/Headers/ COMMAND ${CMAKE_COMMAND} -E copy ${header} ${dst} DEPENDS ${header})
  endforeach()

  # Copy the framework module map.
  set(src ${CMAKE_CURRENT_SOURCE_DIR}/../builra-framework/builra-module.modulemap)
  set(dst ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/builra.framework/Versions/A/Modules/module.modulemap)
  list(APPEND BUILRA_FW_INPUTS ${dst})
  add_custom_command(OUTPUT ${dst} COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/builra.framework/Versions/A/Modules/ COMMAND ${CMAKE_COMMAND} -E copy ${src} ${dst} DEPENDS ${src})

  # Create the appropriate symlinks.
  set(dst ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/builra.framework/Headers)
  list(APPEND BUILRA_FW_INPUTS ${dst})
  add_custom_command(OUTPUT ${dst} COMMAND ${CMAKE_COMMAND} -E create_symlink Versions/Current/Headers ${dst})
  set(dst ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/builra.framework/Modules)
  list(APPEND BUILRA_FW_INPUTS ${dst})
  add_custom_command(OUTPUT ${dst} COMMAND ${CMAKE_COMMAND} -E create_symlink Versions/Current/Modules ${dst})

  # Add a custom target for the setup tasks.
  add_custom_target(builra-framework-setup DEPENDS ${BUILRA_FW_INPUTS})
  add_dependencies(builra-framework builra-framework-setup)
endif()
