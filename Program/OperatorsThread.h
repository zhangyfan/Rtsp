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

namespace Websocket {
class WSServer;
}

/**
 * @brief 开始算法计算线程
 * 
 */
extern void startOperators(std::string dir, std::string_view cameraId, Websocket::WSServer *server);

/**
 * @brief 停止算法计算线程
 * 
 */
extern void stopOperators();

/**
 * @brief 设置要截图的帧
 */
extern void setOperatorsFrame(std::shared_ptr<AVFrame>, uint64_t pts);
#endif //_OPERATORSTHREAD_H_