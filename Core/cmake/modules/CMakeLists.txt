set(Builra_EXPORTS_FILE ${CMAKE_CURRENT_BINARY_DIR}/BuilraExports.cmake)
configure_file(BuilraConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/BuilraConfig.cmake)

get_property(Builra_EXPORTS GLOBAL PROPERTY Builra_EXPORTS)
export(TARGETS ${Builra_EXPORTS} FILE ${Builra_EXPORTS_FILE})
