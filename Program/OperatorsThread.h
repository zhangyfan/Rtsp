/**
* @file OperatorsThread.h
* @author 张一帆
* @brief
* @version 0.1
* @date 2023-01-23
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _OPERATORSTHREAD_H_
#define _OPERATORSTHREAD_H_
#pragma once
#include <string_view>

/**
 * @brief 开始算法计算线程
 * 
 */
extern void startOperators(std::string_view cameraId);

/**
 * @brief 停止算法计算线程
 * 
 */
extern void stopOperators();

#endif //_OPERATORSTHREAD_H_