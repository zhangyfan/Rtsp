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
class RTSPCLIENT_EXPORT ProxyRTSPClient
{
public:
    ProxyRTSPClient();
    ~ProxyRTSPClient();

    /**
     * @brief 打开RTSP流
     * 
     * @param addr 地址
     * @param port 端口
     * @param path 路径
     * @param user 用户名
     * @param passwd 密码
     * @return true 
     * @return false 
     */
    bool open(const std::string &addr, int port, const std::string &path, const std::string &user = "", const std::string &passwd = "");

    /**
     * @brief 关闭RTSP流
     * 
     * @return true 
     * @return false 
     */
    bool close();

    /**
     * @brief 设置获得帧时的回调
     * 
     */
    void setFrameCallback(const std::function<void (unsigned char *, size_t, char *)> &);

private:
    class impl;
    impl *m_impl;
};
}

#endif //_RTSPCLIENT_H_