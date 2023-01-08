/**
* @file Encoder.h
* @author 张一帆
* @brief  编码器
* @version 0.1
* @date 2022-12-30
*
* @copyright Copyright (c) 2022
*
*/
#ifndef _ENCODER_H_
#define _ENCODER_H_
#pragma once

#include "Codec_global.h"
#include <string>

namespace Codec
{
class CODEC_EXPORT Encoder
{
public:
    static Encoder *createNew(const std::string &codecName);
    virtual bool readSPS(unsigned char *&dst, size_t &dstLength) = 0;
    virtual bool encode(unsigned char *src, size_t length, unsigned char *&dst, size_t &dstLength) = 0;
};
}

#endif //_ENCODER_H_