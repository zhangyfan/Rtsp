#include "logger.h"
#include "RTSPClient.h"
#include <iostream>

int main()
{
    Common::InitLogger();
    RTSP::ProxyRTSPClient client;

    LOG_INFO("System starting ......");
    client.open("127.0.0.1", 8554, "1");
    client.setFrameCallback([](unsigned char *data, size_t length, char *id) {

    });

    LOG_INFO("System exiting ......");
}