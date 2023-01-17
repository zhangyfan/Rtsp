/**
* @file Network_global.h
* @author 张一帆
* @brief
* @version 0.1
* @date 2023-01-17
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _NETWORK_GLOBAL_H_
#define _NETWORK_GLOBAL_H_
#pragma once

#ifdef NETWORKLISTENER_LIBRARY
#ifdef _MSC_VER
#define NETWORKLISTENER_EXPORT __declspec(dllexport)
#else
#define NETWORKLISTENER_EXPORT
#endif
#else
#ifdef _MSC_VER
#define NETWORKLISTENER_EXPORT __declspec(dllimport)
#else
#define NETWORKLISTENER_EXPORT
#endif
#endif

#endif //_NETWORK_GLOBAL_H_


