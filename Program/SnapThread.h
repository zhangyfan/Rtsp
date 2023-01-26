/**
* @file SnapThread.h
* @author 张一帆
* @brief
* @version 0.1
* @date 2023-01-22
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _SNAPTHREAD_H_
#define _SNAPTHREAD_H_
#pragma once

/**
 * @brief 开启截图线程（每五秒一次）
 */
extern void startSnap(int width, int height);

/**
 * @brief 停止截图线程
 * 
 */
extern void stopSnap();

/**
 * @brief 出发一次截图
 */
extern void triggerSnap();

#endif //_SNAPTHREAD_H_