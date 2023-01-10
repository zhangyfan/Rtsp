/**
* @file RTMPServer.h
* @author 张一帆
* @brief rtsp服务
* @version 0.1
* @date 2023-01-09
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _RTMPSERVER_H_
#define _RTMPSERVER_H_
#pragma once
#include "RTMPServer_global.h"
#include <string>

class RTMPSERVER_EXPORT RTMPServer
{
public:
    RTMPServer();
    ~RTMPServer();

    void init(int width, int height, int fps);

    /**
     * @brief 开始RTMP服务
     * 
     */
    void start();

    /**
     * @brief 添加视频帧
     * 
     * @param data   数据
     * @param length 数据长度
     * @return true 
     * @return false 
     */
    bool addFrame(unsigned char *data, size_t length);

private:
    class impl;
    impl *impl_;
};

#endif //_RTSPSERVER_H_