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

namespace Codec
{
class CODEC_EXPORT Decoder
{
public:
    static Decoder *createNew(const std::string &codecName);
    virtual bool decode(unsigned char *src, size_t length, unsigned char *&dst, size_t &dstLength) = 0;
};
}

#endif //_DECODE_H_