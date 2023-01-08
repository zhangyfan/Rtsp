/**
* @file Encoder_h264.h
* @author 张一帆
* @brief H264编码器
* @version 0.1
* @date 2022-12-30
*
* @copyright Copyright (c) 2022
*
*/
#ifndef _ENCODER_H264_H_
#define _ENCODER_H264_H_
#pragma once

#include "Encoder.h"

namespace Codec
{
class EncoderH264: public Encoder
{
public:
    EncoderH264();
    ~EncoderH264();

    virtual bool readSPS(unsigned char *&dst, size_t &dstLength);

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

#endif //_ENCODER_H264_H_