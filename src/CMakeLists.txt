cmake_minimum_required(VERSION 3.16)

project(AnyCapture VERSION 1.0 LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Multimedia WinExtras)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Multimedia WinExtras)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)

add_library(libanycapture SHARED
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

target_compile_options(libanycapture PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8;/Zi>")

target_include_directories(libanycapture PRIVATE ${CMAKE_SOURCE_DIR}/libyuvconvertor)

if (WIN32)
    target_include_directories(libanycapture PRIVATE ${CMAKE_SOURCE_DIR}/DirectXSDK/Include)
    target_link_libraries(libanycapture PRIVATE Qt::WinExtras)
    #-lPropsys -lOle32 -lWinmm -lGdi32 -lUser32 -ld3d9 -lD3D11 -lDXGI
    target_link_libraries(libanycapture PRIVATE Propsys Ole32 Winmm Gdi32 User32 d3d9 D3D11 DXGI)
    target_link_directories(libanycapture
        PRIVATE $<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,${CMAKE_SOURCE_DIR}/DirectXSDK/Lib/x64,${CMAKE_SOURCE_DIR}/DirectXSDK/Lib/x86>
    )
endif()

target_link_libraries(libanycapture
    PRIVATE libyuvconvertor
    PRIVATE Qt::Core
    PRIVATE Qt::Multimedia
)

target_compile_definitions(libanycapture PRIVATE LIBANYCAPTURE_LIBRARY)

set(EXPORT_HEADER
    anycapture.h
    libanycapture_global.h
)

set_target_properties(libanycapture PROPERTIES
    DEBUG_POSTFIX "d"
    PUBLIC_HEADER "${EXPORT_HEADER}"
    PDB_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/buildlib/${CMAKE_BUILD_TYPE}
)

install(TARGETS libanycapture
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/buildlib/include
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/buildlib/${CMAKE_BUILD_TYPE}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/buildlib/${CMAKE_BUILD_TYPE}
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/buildlib/${CMAKE_BUILD_TYPE}
)

