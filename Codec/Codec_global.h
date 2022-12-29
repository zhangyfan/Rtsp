/**
* @file Codec_global.h
* @author 张一帆
* @brief 编解码模块
* @version 0.1
* @date 2022-12-27
*
* @copyright Copyright (c) 2022
*
*/
#ifndef _CODEC_GLOBAL_H_
#define _CODEC_GLOBAL_H_
#pragma once

#ifdef CODEC_LIBRARY
#ifdef _MSC_VER
#define CODEC_EXPORT __declspec(dllexport)
#else
#define CODEC_EXPORT
#endif
#else
#ifdef _MSC_VER
#define CODEC_EXPORT __declspec(dllimport)
#else
#define CODEC_EXPORT
#endif
#endif

#endif //_CODEC_GLOBAL_H_