#ifndef YUVCONVERTOR_H
#define YUVCONVERTOR_H

#include "libyuvconvertor_global.h"

class LIBYUVCONVERTOR_EXPORT YUVConvertor
{
public:
    static void yuy2ToI420(unsigned char *src_data, int width, int height, unsigned char *dst_data);
    static void rgb24ToI420(unsigned char *src_data, int width, int height, unsigned char *dst_data);
    static void argb32ToRgb24(unsigned char *src_data, int width, int height, unsigned char *dst_data);
    static void argb32ToI420(unsigned char *src_data, int width, int height, unsigned char *dst_data);
    static void argb32ToNv12(unsigned char *src_data, int width, int height, unsigned char *dst_data);
};

#endif // YUVCONVERTOR_H
