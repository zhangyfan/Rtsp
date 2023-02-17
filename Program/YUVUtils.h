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

#define MPP_ALIGN(x, a) (((x) + (a)-1) & ~((a)-1))

struct AVFrame;
extern std::pair<unsigned char*, size_t> toYUV420(AVFrame *frame);
extern std::pair<unsigned char *, size_t> toBGR888(AVFrame *frame);
extern bool YUV2JPG(AVFrame *frame, std::string_view path);
#endif //_YUVUTILS_H_