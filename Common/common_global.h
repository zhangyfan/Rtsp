/**
 * @file common_global.h
 * @author 张一帆
 * @brief  common工程导出宏
 * @version 0.1
 * @date 2022-12-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef COMMON_GLOBAL_H_
#define COMMON_GLOBAL_H_
#pragma once

#ifdef COMMON_LIBRARY
#ifdef _MSC_VER
#define COMMON_EXPORT __declspec(dllexport)
#else
#define COMMON_EXPORT
#endif
#else
#ifdef _MSC_VER
#define COMMON_EXPORT __declspec(dllimport)
#else
#define COMMON_EXPORT
#endif
#endif

#endif // COMMON_GLOBAL_H_