project(libyuvconvertor VERSION 1.0 LANGUAGES CXX)

file(GLOB_RECURSE LIBYUV_SOURCES "libyuv/*.h" "libyuv/*.cc")

add_library(libyuvconvertor SHARED
    ${LIBYUV_SOURCES}
    libyuvconvertor_global.h
    yuvconvertor.h
    yuvconvertor.cpp
)

target_include_directories(libyuvconvertor
    PRIVATE libyuv/include
)
target_compile_options(libyuvconvertor PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8;/Zi>")

target_compile_definitions(libyuvconvertor PRIVATE LIBYUVCONVERTOR_LIBRARY)

set(EXPORT_HEADER
    yuvconvertor.h
    libyuvconvertor_global.h
)

set_target_properties(libyuvconvertor PROPERTIES
    DEBUG_POSTFIX "d"
    PUBLIC_HEADER "${EXPORT_HEADER}"
    PDB_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/buildlib/${CMAKE_BUILD_TYPE}
)

install(TARGETS libyuvconvertor
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/buildlib/include
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/buildlib/${CMAKE_BUILD_TYPE}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/buildlib/${CMAKE_BUILD_TYPE}
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/buildlib/${CMAKE_BUILD_TYPE}
)
