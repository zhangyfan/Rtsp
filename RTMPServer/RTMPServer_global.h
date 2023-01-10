/**
* @file RTMPServer_global.h
* @author 张一帆
* @brief
* @version 0.1
* @date 2023-01-09
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _RTMPSERVER_GLOBAL_H_
#define _RTMPSERVER_GLOBAL_H_
#pragma once

#ifdef RTMPSERVER_LIBRARY
#ifdef _MSC_VER
#define RTMPSERVER_EXPORT __declspec(dllexport)
#else
#define RTMPSERVER_EXPORT
#endif
#else
#ifdef _MSC_VER
#define RTMPSERVER_EXPORT __declspec(dllimport)
#else
#define RTMPSERVER_EXPORT
#endif
#endif

#endif //_RTMPSERVER_GLOBAL_H_