add_builra_library(builraNinja STATIC
  Lexer.cpp
  Manifest.cpp
  ManifestLoader.cpp
  Parser.cpp
  )

target_link_libraries(builraNinja PRIVATE
  builraBasic
  llvmSupport)

set_property(GLOBAL APPEND PROPERTY Builra_EXPORTS builraNinja)
