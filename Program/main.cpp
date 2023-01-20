#include "logger.h"
#include "RTSPClient.h"
#include "RTMPServer.h"
#include "RTSPServer.h"
#include "Decoder.h"
#include "Encoder.h"
#include "Operators.h"
#include "WSServer.h"
#include "YUVUtils.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <chrono>
extern "C" {
    #include <libavformat/avformat.h>
    #include "libswscale/swscale.h"
    #include "libavutil/imgutils.h"
}

RTSP::ProxyRTSPClient *client;
//RTMPServer *server;
RTSP::RTSPServer *rtspServer = nullptr;
Codec::Decoder *decoder;
//Codec::Encoder *encoder;
Codec::Encoder *JPEGEncoder;
FILE *file;
Operators *operators = nullptr;
Websocket::WSServer *WSServer = nullptr;
bool proxyOnly = false;
AVPacket *queuedPacket = nullptr;
std::mutex mtx;
std::condition_variable cond;

void onDecodedYUV(unsigned char *BGR888, unsigned char *YUV420, int width, int height);
std::pair<unsigned char *, size_t> reencode(unsigned char *YUV420, size_t length);

void onReceiveFrame(AVPacket *packet) {
    uint8_t *data = packet->data;
    size_t size = packet->size;

    rtspServer->addFrame(data, size);
    
    if (WSServer) {
        unsigned char *buffer = new unsigned char[size + 8];
        int64_t pts = packet->pts;

        *((int64_t *)buffer)  = pts;
        memcpy(buffer + 8, data, size);
        WSServer->asyncSend("/data", buffer, size + 8);
        delete buffer;
    }
    
    //转发流 
    if (!proxyOnly) {
        //解码需要一直调用，否则有可能会漏掉PPS/SPS，甚至没有从I帧开始导致无法解码
        AVPacket *pkt = av_packet_alloc();
        
        av_packet_ref(pkt, packet);
        queuedPacket = pkt;
    }
}

//将解码和后续处理放到不同线程中，否则会导致解码丢帧
void EncodeThreadEntry() {
    while (true) {
        //等待packet不为空
        if (!queuedPacket) {
            {
                std::unique_lock<std::mutex> guard(mtx);
                cond.wait(guard, []()->bool { return queuedPacket != nullptr;});
            }
        }
        
        uint8_t *data = queuedPacket->data;
        size_t size = queuedPacket->size;
        
        auto decodedFrames = decoder->decode(data, size);

        for (auto avFrame: decodedFrames) {
            
       //拷贝一份BGR888数据，给算法调用
       unsigned char *BGR888 = nullptr;
       size_t BGRSize        = 0;
       unsigned char *YUV420 = nullptr;
       size_t YUVSize        = 0;

       // std::tie(BGR888, BGRSize) = toBGR888(avFrame);
       std::tie(YUV420, YUVSize) = toYUV420(avFrame);
       onDecodedYUV(BGR888, YUV420, avFrame->width, avFrame->height);

        {
            unsigned char *jpeg            = nullptr;
            size_t size                    = 0;

            JPEGEncoder->encode(YUV420, YUVSize, jpeg, size);
            //FILE *f = fopen("/dev/shm/1.jpeg", "wb");
            //fwrite(jpeg, 1, size, f);
            //fclose(f);

            delete jpeg;
        }

        //重新编码
        //unsigned char *h264Data = nullptr;
        //size_t h264Length       = 0;

        //std::tie(h264Data, h264Length) = reencode(YUV420, YUVSize);
        //if (h264Data) {
        //    server->addFrame(h264Data, h264Length);
        //    delete[] h264Data;
        //}

#ifdef _MSC_VER
       free(YUV420); // Linux下不删除
#endif                // _MSC_VER
       //free(BGR888);
        av_frame_free(&avFrame);

        }
        
        av_packet_unref(queuedPacket);
    }
}

void snapThreadEntry() {

}

void onDecodedYUV(unsigned char *BGR888, unsigned char *YUV420, int width, int height) {
    //调用算法
    if (operators) {
        operators->detect(BGR888, width, height, YUV420);
    }
}

// std::pair<unsigned char*, size_t> reencode(unsigned char *YUV420, size_t length) {
//     //编码
//     unsigned char *encoded = nullptr;
//     size_t encodedSize = 0;

//     encoder->encode(YUV420, length, encoded, encodedSize);
//     return std::make_pair(encoded, encodedSize);
// }

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Invalid command line arguments:");
        printf("    RtspProxy INPUT_URL DEST_URL [IS_PROXY]");

        return 0;
    }

    Common::InitLogger();    
    decoder = Codec::Decoder::createNew("H264");
    //encoder = Codec::Encoder::createNew("H264");
    JPEGEncoder = Codec::Encoder::createNew("JPEG");

    client  = new RTSP::ProxyRTSPClient();

    if (!client->open(argv[1], 0, "")) {
        LOG_ERROR("stream can not open!!!!");
        return 0;
    }

    operators = new Operators();
    operators->init();
    
    //初始化编码器，解码器因为有PPS/SPS所有不需要额外的信息
    int width  = client->getVideoWidth();
    int height = client->getVideoHeight();

    //encoder->init(width, height, 25);
    JPEGEncoder->init(width, height, 25);

    //RTSP服务
    rtspServer = new RTSP::RTSPServer();
    //server->init(width, height, 25, argv[2]);
    rtspServer->start(8554, argv[2]);

    //Websocket服务
    WSServer = new Websocket::WSServer();
    WSServer->init(9000);

    LOG_INFO("Stream open success");

    if (argc == 4 && (strcmp(argv[3], "1") == 0)) {
        proxyOnly = true;
    } else {
        proxyOnly = false;
        
        //开启编码线程
        std::thread encThread(&EncodeThreadEntry);
        encThread.detach();
    }

    client->setFrameCallback(onReceiveFrame);
    client->run();
    LOG_INFO("System exiting ......");
}