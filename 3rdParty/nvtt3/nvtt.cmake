get_filename_component(install_prefix "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)

add_library(ext::nvtt3 SHARED IMPORTED)

if(WIN32)
    #set_property(TARGET ext::nvtt3 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
    #set_property(TARGET ext::nvtt3 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
    set_target_properties(ext::nvtt3 PROPERTIES 
        IMPORTED_LOCATION "${install_prefix}/bin/nvtt30106.dll"
        IMPORTED_IMPLIB "${install_prefix}/lib/x64-v142/nvtt30106.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${install_prefix}/include"
    )
    #set(nvtt3_DLL "${install_prefix}/bin/nvtt30106.dll")
endif()