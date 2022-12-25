/**
 * @file RTSPClient_global.h
 * @author 张一帆
 * @brief  RTSP客户端导出
 * @version 0.1
 * @date 2022-12-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef RTSPCLIENT_GLOBAL_H_
#define RTSPCLIENT_GLOBAL_H_
#pragma once

#ifdef RTSPCLIENT_LIBRARY
#ifdef _MSC_VER
#define RTSPCLIENT_EXPORT __declspec(dllexport)
#else
#define RTSPCLIENT_EXPORT
#endif
#else
#ifdef _MSC_VER
#define RTSPCLIENT_EXPORT __declspec(dllimport)
#else
#define RTSPCLIENT_EXPORT
#endif
#endif

#endif // RTSPCLIENT_GLOBAL_H_