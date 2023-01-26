CONFIG -= qt

TEMPLATE = lib

CONFIG += c++17

DEFINES += LIBYUVCONVERTOR_LIBRARY

TARGET = $$qtLibraryTarget(libyuvconvertor)

DESTDIR = $$PWD/buildlib

INCLUDEPATH += $$PWD/libyuv/include

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    libyuv/source/compare.cc \
    libyuv/source/compare_common.cc \
    libyuv/source/compare_gcc.cc \
    libyuv/source/compare_mmi.cc \
    libyuv/source/compare_msa.cc \
    libyuv/source/compare_neon.cc \
    libyuv/source/compare_neon64.cc \
    libyuv/source/compare_win.cc \
    libyuv/source/convert.cc \
    libyuv/source/convert_argb.cc \
    libyuv/source/convert_from.cc \
    libyuv/source/convert_from_argb.cc \
    libyuv/source/convert_jpeg.cc \
    libyuv/source/convert_to_argb.cc \
    libyuv/source/convert_to_i420.cc \
    libyuv/source/cpu_id.cc \
    libyuv/source/mjpeg_decoder.cc \
    libyuv/source/mjpeg_validate.cc \
    libyuv/source/planar_functions.cc \
    libyuv/source/rotate.cc \
    libyuv/source/rotate_any.cc \
    libyuv/source/rotate_argb.cc \
    libyuv/source/rotate_common.cc \
    libyuv/source/rotate_gcc.cc \
    libyuv/source/rotate_lsx.cc \
    libyuv/source/rotate_mmi.cc \
    libyuv/source/rotate_msa.cc \
    libyuv/source/rotate_neon.cc \
    libyuv/source/rotate_neon64.cc \
    libyuv/source/rotate_win.cc \
    libyuv/source/row_any.cc \
    libyuv/source/row_common.cc \
    libyuv/source/row_gcc.cc \
    libyuv/source/row_lasx.cc \
    libyuv/source/row_lsx.cc \
    libyuv/source/row_mmi.cc \
    libyuv/source/row_msa.cc \
    libyuv/source/row_neon.cc \
    libyuv/source/row_neon64.cc \
    libyuv/source/row_win.cc \
    libyuv/source/scale.cc \
    libyuv/source/scale_any.cc \
    libyuv/source/scale_argb.cc \
    libyuv/source/scale_common.cc \
    libyuv/source/scale_gcc.cc \
    libyuv/source/scale_lsx.cc \
    libyuv/source/scale_mmi.cc \
    libyuv/source/scale_msa.cc \
    libyuv/source/scale_neon.cc \
    libyuv/source/scale_neon64.cc \
    libyuv/source/scale_rgb.cc \
    libyuv/source/scale_uv.cc \
    libyuv/source/scale_win.cc \
    libyuv/source/video_common.cc \
    yuvconvertor.cpp

HEADERS += \
    libyuv/include/libyuv.h \
    libyuv/include/libyuv/basic_types.h \
    libyuv/include/libyuv/compare.h \
    libyuv/include/libyuv/compare_row.h \
    libyuv/include/libyuv/convert.h \
    libyuv/include/libyuv/convert_argb.h \
    libyuv/include/libyuv/convert_from.h \
    libyuv/include/libyuv/convert_from_argb.h \
    libyuv/include/libyuv/cpu_id.h \
    libyuv/include/libyuv/loongson_intrinsics.h \
    libyuv/include/libyuv/macros_msa.h \
    libyuv/include/libyuv/mjpeg_decoder.h \
    libyuv/include/libyuv/planar_functions.h \
    libyuv/include/libyuv/rotate.h \
    libyuv/include/libyuv/rotate_argb.h \
    libyuv/include/libyuv/rotate_row.h \
    libyuv/include/libyuv/row.h \
    libyuv/include/libyuv/scale.h \
    libyuv/include/libyuv/scale_argb.h \
    libyuv/include/libyuv/scale_rgb.h \
    libyuv/include/libyuv/scale_row.h \
    libyuv/include/libyuv/scale_uv.h \
    libyuv/include/libyuv/version.h \
    libyuv/include/libyuv/video_common.h \
    yuvconvertor.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
