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
     */
    virtual std::vector<AVFrame *> decode(AVPacket *pkt);

private:
    class impl;
    impl *impl_;
};
}

#endif //_DECODE_H264_H_