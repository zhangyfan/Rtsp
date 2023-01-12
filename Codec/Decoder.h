/**
* @file Decode.h
* @author 张一帆
* @brief 解码器定义
* @version 0.1
* @date 2022-12-27
*
* @copyright Copyright (c) 2022
*
*/
#ifndef _DECODE_H_
#define _DECODE_H_
#pragma once
#include "Codec_global.h"
#include <string>
#include <vector>

struct AVFrame;

namespace Codec {
class CODEC_EXPORT Decoder
{
public:
    static Decoder *createNew(const std::string &codecName);
    virtual std::vector<AVFrame *> decode(unsigned char *src, size_t length) = 0;
};
}

#endif //_DECODE_H_