/**
* @file WSServer_global.h
* @author 张一帆
* @brief
* @version 0.1
* @date 2023-01-19
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _WSSERVER_GLOBAL_H_
#define _WSSERVER_GLOBAL_H_
#pragma once

#ifdef WEBSOCKETSERVER_LIBRARY
#ifdef _MSC_VER
#define WEBSOCKETSERVER_EXPORT __declspec(dllexport)
#else
#define WEBSOCKETSERVER_EXPORT
#endif
#else
#ifdef _MSC_VER
#define WEBSOCKETSERVER_EXPORT __declspec(dllimport)
#else
#define WEBSOCKETSERVER_EXPORT
#endif
#endif

#endif //_WSSERVER_GLOBAL_H_