/**
* @file RTSPServer.h
* @author 张一帆
* @brief rtsp服务
* @version 0.1
* @date 2023-01-01
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _RTSPSERVER_H_
#define _RTSPSERVER_H_
#pragma once
#include "RTSPServer_global.h"
#include <string>

namespace RTSP
{
class RTSPSERVER_EXPORT RTSPServer
{
public:
    RTSPServer();
    ~RTSPServer();

    /**
     * @brief 开始RTSP服务
     * 
     * @param port   服务端口
     * @param stream 流名称
     */
    void start(int port, const std::string &stream);

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
}

#endif //_RTSPSERVER_H_