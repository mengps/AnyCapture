project(libyuvconvertor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_DEBUG_POSTFIX "d")

#linux .so output
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/buildlib/${CMAKE_BUILD_TYPE}")
#win .dll output
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/buildlib/${CMAKE_BUILD_TYPE}")
#win .lib output
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/buildlib/${CMAKE_BUILD_TYPE}")

file(GLOB_RECURSE SOURCES "libyuv/*.h" "libyuv/*.cc")

add_library(libyuvconvertor SHARED
    ${SOURCES}
    yuvconvertor.h
    yuvconvertor.cpp
)

target_include_directories(libyuvconvertor
    PRIVATE libyuv/include
)

target_compile_definitions(libyuvconvertor PRIVATE LIBYUVCONVERTOR_LIBRARY)
