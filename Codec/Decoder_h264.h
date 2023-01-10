/**
* @file Decode_h264.h
* @author 张一帆
* @brief h.264解码
* @version 0.1
* @date 2022-12-27
*
* @copyright Copyright (c) 2022
*
*/
#ifndef _DECODE_H264_H_
#define _DECODE_H264_H_
#pragma once
#include "Decoder.h"

namespace Codec
{
class DecoderH264: public Decoder
{
public:
    DecoderH264();
    ~DecoderH264();

    /**
     * @brief 解码
     * 
     * @warning 需要调用方删除dst内存
     * @param src       源数据
     * @param length    源数据大小
     * @param dst       输出数据
     * @param dstLength 输出数据大小
     * @return true 
     * @return false 
     */
    virtual AVFrame * decode(unsigned char *src, size_t length);

private:
    class impl;
    impl *impl_;
};
}

#endif //_DECODE_H264_H_