include (${CMAKE_SOURCE_DIR}/3rdParty/nvtt3/nvtt.cmake)

add_library(astcenc_universal SHARED
	universal.hpp
	universal.cpp
	universal_avx.cpp
	universal_sse.cpp
    nvtt.cpp
    nvtt.hpp
    astc_server.cpp
    astc_server.h
)

set_target_properties(astcenc_universal
PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

target_link_libraries(astcenc_universal PRIVATE ext::nvtt3)

set_source_files_properties(
universal_avx.cpp
PROPERTIES
COMPILE_FLAGS
$<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
)

set_source_files_properties(
universal_sse.cpp
PROPERTIES
COMPILE_FLAGS
$<$<CXX_COMPILER_ID:MSVC>:/arch:SSE2>
)
target_include_directories(astcenc_universal PRIVATE ${CMAKE_SOURCE_DIR}/Source)

string(TIMESTAMP astcencoder_YEAR "%Y")

configure_file(
    ../Source/astcenccli_version.h.in
    astcenccli_version.h
    ESCAPE_QUOTES @ONLY)

add_executable(astcenc
    ../Source/astcenccli_error_metrics.cpp
    ../Source/astcenccli_image.cpp
    ../Source/astcenccli_image_external.cpp
    ../Source/astcenccli_image_load_store.cpp
    ../Source/astcenccli_platform_dependents.cpp
    ../Source/astcenccli_toplevel.cpp
    ../Source/astcenccli_toplevel_help.cpp
    ../Source/astcenc_mathlib_softfloat.cpp
)

target_link_libraries(astcenc PRIVATE astcenc_universal)

set_target_properties(astcenc
PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

target_include_directories(astcenc
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR})