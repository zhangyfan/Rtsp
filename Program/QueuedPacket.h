/*****************************************************************//**
 * \file   QueuedPacket.h
 * \brief  
 * 
 * \author zyf01
 * \date   January 2023
 *********************************************************************/
#ifndef _QUEUEDPACKET_H_
#define _QUEUEDPACKET_H_
#pragma once

struct AVPacket;
struct AVFrame;

/**
 * @brief 设置队列中的包
 */
extern void setQueuedPacket(AVPacket *);

/**
 * @brief 获取队列中的包（可能会阻塞）
 */
extern AVPacket *getQueuedPacket();

extern void setQueuedFrame(AVFrame *);
extern AVFrame *getQueuedFrame();

#endif // !_QUEUEDPACKET_H_


