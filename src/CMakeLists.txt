cmake_minimum_required(VERSION 3.16)

project(AnyCapture VERSION 1.0 LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Multimedia WinExtras)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Multimedia WinExtras)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_DEBUG_POSTFIX "d")

#linux .so output
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/buildlib/${CMAKE_BUILD_TYPE}")
#win .dll output
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/buildlib/${CMAKE_BUILD_TYPE}")
#win .lib output
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/buildlib/${CMAKE_BUILD_TYPE}")

add_library(anycapture SHARED
    abstractcapturethread.cpp
    abstractcapturethread.h
    anycapture.cpp
    anycapture.h
    audiocapture.cpp
    audiocapture.h
    cameracapture.cpp
    cameracapture.h
    camerasurface.cpp
    camerasurface.h
    captureglobal.h
    d3dmanager.cpp
    d3dmanager.h
    dxgimanager.cpp
    dxgimanager.h
    libanycapture_global.h
    screencapture.cpp
    screencapture.h
)

target_compile_options(anycapture PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

target_include_directories(anycapture PRIVATE ${CMAKE_SOURCE_DIR}/libyuvconvertor)

if (WIN32)
    target_include_directories(anycapture PRIVATE ${CMAKE_SOURCE_DIR}/DirectXSDK/Include)
    target_link_libraries(anycapture PRIVATE Qt::WinExtras)
    #-lPropsys -lOle32 -lWinmm -lGdi32 -lUser32 -ld3d9 -lD3D11 -lDXGI
    target_link_libraries(anycapture
        PRIVATE Propsys
        PRIVATE Ole32
        PRIVATE Winmm
        PRIVATE Gdi32
        PRIVATE User32
        PRIVATE d3d9
        PRIVATE D3D11
        PRIVATE DXGI
    )
    target_link_directories(anycapture
        PRIVATE $<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,${CMAKE_SOURCE_DIR}/DirectXSDK/Lib/x64,${CMAKE_SOURCE_DIR}/DirectXSDK/Lib/x86>
    )
endif()

target_link_libraries(anycapture
    PRIVATE libyuvconvertor
    PRIVATE Qt::Core
    PRIVATE Qt::Multimedia
)

target_compile_definitions(anycapture PRIVATE LIBANYCAPTURE_LIBRARY)
