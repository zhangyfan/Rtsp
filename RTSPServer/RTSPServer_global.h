/**
* @file RTSPServer_global.h
* @author 张一帆
* @brief
* @version 0.1
* @date 2023-01-01
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _RTSPSERVER_GLOBAL_H_
#define _RTSPSERVER_GLOBAL_H_
#pragma once

#ifdef RTSPSERVER_LIBRARY
#ifdef _MSC_VER
#define RTSPSERVER_EXPORT __declspec(dllexport)
#else
#define RTSPSERVER_EXPORT
#endif
#else
#ifdef _MSC_VER
#define RTSPSERVER_EXPORT __declspec(dllimport)
#else
#define RTSPSERVER_EXPORT
#endif
#endif

#endif //_RTSPSERVER_GLOBAL_H_