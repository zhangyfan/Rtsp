/**
* @file WSServer.h
* @author 张一帆
* @brief
* @version 0.1
* @date 2023-01-19
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _WSSERVER_H_
#define _WSSERVER_H_
#pragma once
#include "WSServer_global.h"
#include <string>
#include <functional>

namespace Websocket
{
class WEBSOCKETSERVER_EXPORT WSServer {
public:
    WSServer();
    ~WSServer();

    /**
     * @brief 初始化websocket服务
     * 
     * @param port 
     * @return true 
     * @return false 
     */
    bool init(int port);

    /**
     * @brief Set the Message Callback object
     * 
     * @param path HTTP路径
     * @param cb   回调
     */
    void setMessageCallback(const std::string &path, std::function<void(const std::string &)> cb);

    /**
     * @brief 向指定的地址发送数据(所有连接到该地址的会话都会收到)
     * 
     * @param path 地址
     * @param data 数据
     * @param size 数据长度
     */
    void asyncSend(const std::string &path, unsigned char *data, size_t size);

private:
    class impl;
    impl *impl_;
};
}

#endif //_WSSERVER_H_