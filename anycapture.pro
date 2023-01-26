QT += multimedia

TEMPLATE = lib
DEFINES += LIBANYCAPTURE_LIBRARY

CONFIG += c++17 debug_and_release

TARGET = $$qtLibraryTarget(libanycapture)

DESTDIR = $$PWD/buildlib

win32 {
    QT += winextras
    LIBS += -lPropsys -lOle32 -lwinmm -lGdi32 -lUser32 -ld3d9 -lD3D11 -lDXGI

    INCLUDEPATH += $$PWD/DirectXSDK/Include

    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$PWD/DirectXSDK/Lib/x64 -ld3dx9
    } else {
        LIBS += -L$$PWD/DirectXSDK/Lib/x86 -ld3dx9
    }
}

INCLUDEPATH += $$PWD/libyuvconvertor

LIBS += -L$$PWD/libyuvconvertor/buildlib

CONFIG(debug, debug|release) {
    LIBS += -llibyuvconvertord
} else {
    LIBS += -llibyuvconvertor
}

copy_files.files += \
    $$files($$PWD/libyuvconvertor/buildlib/*.dll)

copy_files.path = $$DESTDIR

COPIES += copy_files

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/abstractcapturethread.cpp \
    src/anycapture.cpp \
    src/audiocapture.cpp \
    src/cameracapture.cpp \
    src/camerasurface.cpp \
    src/d3dmanager.cpp \
    src/dxgimanager.cpp \
    src/screencapture.cpp

HEADERS += \
    src/abstractcapturethread.h \
    src/anycapture.h \
    src/audiocapture.h \
    src/cameracapture.h \
    src/camerasurface.h \
    src/captureglobal.h \
    src/d3dmanager.h \
    src/dxgimanager.h \
    src/libanycapture_global.h \
    src/screencapture.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
