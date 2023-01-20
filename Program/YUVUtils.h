/**
* @file YUVUtils.h
* @author 张一帆
* @brief
* @version 0.1
* @date 2023-01-20
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _YUVUTILS_H_
#define _YUVUTILS_H_
#pragma once
#include <utility>

struct AVFrame;
extern std::pair<unsigned char*, size_t> toYUV420(AVFrame *frame);
extern std::pair<unsigned char *, size_t> toBGR888(AVFrame *frame);

#endif //_YUVUTILS_H_