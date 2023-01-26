#include "logger.h"
#include "RTSPClient.h"
#include "RTMPServer.h"
#include "RTSPServer.h"
#include "Decoder.h"
#include "Encoder.h"
#include "Operators.h"
#include "WSServer.h"
#include "YUVUtils.h"
#include "QueuedPacket.h"
#include "DecodeThread.h"
#include "OperatorsThread.h"
#include "SnapThread.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <chrono>
extern "C" {
    #include <libavformat/avformat.h>
    #include "libswscale/swscale.h"
    #include "libavutil/imgutils.h"
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Invalid command line arguments:");
        printf("    RtspProxy INPUT_URL DEST_URL [IS_PROXY]");

        return 0;
    }

    Common::InitLogger();

    RTSP::ProxyRTSPClient *client = new RTSP::ProxyRTSPClient();

    if (!client->open(argv[1], 0, "")) {
        LOG_ERROR("stream can not open!!!!");
        return 0;
    }

    // RTSP服务
    auto rtspServer = new RTSP::RTSPServer();

    // server->init(width, height, 25, argv[2]);
    rtspServer->start(8554, argv[2]);

    // Websocket服务
    auto WSServer = new Websocket::WSServer();
    WSServer->init(9000);

    //是否仅转发流
    std::atomic_bool proxyOnly = false;

    if (argc == 4 && (strcmp(argv[3], "1") == 0)) {
        proxyOnly = true;
    } else {
        //初始化编码器，解码器因为有PPS/SPS所有不需要额外的信息
        int width  = client->getVideoWidth();
        int height = client->getVideoHeight();

        startDecode();
        startOperators("");
        startSnap(width, height);
    }

    LOG_INFO("Stream open success");
    client->setFrameCallback([rtspServer, WSServer, &proxyOnly](AVPacket *packet) {
        uint8_t *data = packet->data;
        size_t size   = packet->size;

        if (rtspServer) {
            rtspServer->addFrame(data, size);
        }

        if (WSServer) {
            unsigned char *buffer = new unsigned char[size + 8];
            int64_t pts           = packet->pts;

            *((int64_t *)buffer)  = pts;
            memcpy(buffer + 8, data, size);
            WSServer->asyncSend("/data", buffer, size + 8);
            delete buffer;
        }
        //转发流
        if (!proxyOnly) {
            setQueuedPacket(packet);
        }
    });
    client->run();
    LOG_INFO("System exiting ......");
}