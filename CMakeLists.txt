cmake_minimum_required(VERSION 3.16)

project(AnyCapture VERSION 1.0)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})

add_subdirectory(libyuvconvertor)

add_subdirectory(src)
