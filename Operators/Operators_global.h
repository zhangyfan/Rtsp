/**
* @file Operators_global.h
* @author 张一帆
* @brief
* @version 0.1
* @date 2023-01-11
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _OPERATORS_GLOBAL_H_
#define _OPERATORS_GLOBAL_H_
#pragma once

#ifdef OPERATORS_LIBRARY
#ifdef _MSC_VER
#define OPERATORS_EXPORT __declspec(dllexport)
#else
#define OPERATORS_EXPORT
#endif
#else
#ifdef _MSC_VER
#define OPERATORS_EXPORT __declspec(dllimport)
#else
#define OPERATORS_EXPORT
#endif
#endif

#endif //_OPERATORS_GLOBAL_H_


