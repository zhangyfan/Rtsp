/**
* @file NetworkListener.h
* @author 张一帆
* @brief 网络接口
* @version 0.1
* @date 2023-01-17
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _NETWORKLISTENER_H_
#define _NETWORKLISTENER_H_
#pragma once
#include "Network_global.h"
#include <uchar.h>

namespace Network
{
class NetworkListener {
public:
    NetworkListener();
    ~NetworkListener();

    void run();
    bool connect();
    bool write(const char *data, size_t size);

private:
    class impl;
    impl *impl_;
};
}

#endif //_NETWORKLISTENER_H_