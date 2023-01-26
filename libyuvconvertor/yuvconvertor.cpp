#include "yuvconvertor.h"

#include "libyuv/convert.h"
#include "libyuv/convert_from_argb.h"

void YUVConvertor::yuy2ToI420(unsigned char *src_data, int width, int height, unsigned char *dst_data)
{
    unsigned char *dst_i420_y_data = dst_data;
    unsigned char *dst_i420_u_data = dst_data + width * height;
    unsigned char *dst_i420_v_data = dst_i420_u_data + width * height / 4;

    libyuv::YUY2ToI420(src_data, width * 2
                       , dst_i420_y_data, width
                       , dst_i420_u_data, width >> 1
                       , dst_i420_v_data, width >> 1
                       , width, height);
}

void YUVConvertor::rgb24ToI420(unsigned char *src_data, int width, int height, unsigned char *dst_data)
{
    unsigned char *dst_i420_y_data = dst_data;
    unsigned char *dst_i420_u_data = dst_data + width * height;
    unsigned char *dst_i420_v_data = dst_i420_u_data + width * height / 4;

    libyuv::RAWToI420(src_data, width * 3
                      , dst_i420_y_data, width
                      , dst_i420_u_data, width >> 1
                      , dst_i420_v_data, width >> 1
                      , width, height);
}

void YUVConvertor::argb32ToRgb24(unsigned char *src_data, int width, int height, unsigned char *dst_data)
{
    libyuv::ARGBToRAW(src_data, width * 4, dst_data, width * 3, width, height);
}

void YUVConvertor::argb32ToI420(unsigned char *src_data, int width, int height, unsigned char *dst_data)
{
    unsigned char *dst_i420_y_data = dst_data;
    unsigned char *dst_i420_u_data = dst_data + width * height;
    unsigned char *dst_i420_v_data = dst_i420_u_data + width * height / 4;

    libyuv::ARGBToI420(src_data, width * 4
                       , dst_i420_y_data, width
                       , dst_i420_u_data, width >> 1
                       , dst_i420_v_data, width >> 1
                       , width, height);
}

void YUVConvertor::argb32ToNv12(unsigned char *src_data, int width, int height, unsigned char *dst_data)
{
    unsigned char *dst_i420_y_data = dst_data;
    unsigned char *dst_i420_uv_data = dst_data + width * height;

    libyuv::ARGBToNV12(src_data, width * 4
                       , dst_i420_y_data, width
                       , dst_i420_uv_data, width
                       , width, height);
}
