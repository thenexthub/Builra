add_builra_library(llvmDemangle STATIC
    ItaniumDemangle.cpp
    MicrosoftDemangle.cpp
)

set_property(GLOBAL APPEND PROPERTY Builra_EXPORTS llvmDemangle)
