/**
* @file Encoder_JPEG.h
* @author 张一帆
* @brief
* @version 0.1
* @date 2023-01-18
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _ENCODER_JPEG_H_
#define _ENCODER_JPEG_H_
#pragma once
#include "Encoder.h"

namespace Codec
{
class EncoderJPEG: public Encoder
{
public:
    EncoderJPEG();
    ~EncoderJPEG();

    virtual void init(int width, int height, int fps);
    
    /**
     * @brief 编码
     * 
     * @param src    yuv数据
     * @param length yuv数据大小
     * @param dst    h.264数据输出
     * @param dstLength 
     * @return true 
     * @return false 
     */
    virtual bool encode(unsigned char *src, size_t length, unsigned char *&dst, size_t &dstLength);

private:
    class impl;
    impl *impl_;
};
}
#endif //_ENCODER_JPEG_H_