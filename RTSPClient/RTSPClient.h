/**
 * @file RTSPClient.h
 * @author 张一帆
 * @brief  RTSP客户端对象定义
 * @version 0.1
 * @date 2022-12-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef _RTSPCLIENT_H_
#define _RTSPCLIENT_H_
#pragma once
#include "RTSPClient_global.h"
#include <string>
#include <functional>

namespace RTSP
{
class RTSPCLIENT_EXPORT RTSPClient
{
public:
    RTSPClient();
    ~RTSPClient();

    bool open(const std::string &addr, int port, const std::string &user = "", const std::string &passwd = "");
    bool close();

    void setFrameCallback(const std::function<void (unsigned char *, size_t, int)>);
};
}

#endif //_RTSPCLIENT_H_