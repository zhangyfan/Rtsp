/**
* @file RecordThread.h
* @author 张一帆
* @brief
* @version 0.1
* @date 2023-02-06
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _RECORDTHREAD_H_
#define _RECORDTHREAD_H_
#pragma once
#include <string>

struct AVPacket;

extern void startRecord(std::string_view camId, int fps);
extern void stopRecord();
extern void PushRecordPkt(AVPacket *packet);

#endif //_RECORDTHREAD_H_