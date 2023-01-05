#include "logger.h"
#include "RTSPClient.h"
#include "RTSPServer.h"
#include "Decoder.h"
#include "Encoder.h"
#include <iostream>
#include <fstream>
//
//int main()
//{
//    Common::InitLogger();
//    RTSP::RTSPServer server;
//    RTSP::ProxyRTSPClient client;
//    FILE *file;
//
//    file = fopen("encode.h264", "rb");
//    server.start(8556, "1");
//    LOG_INFO("System starting ......");
//
//    std::thread thread([&client]() {
//        client.open("127.0.0.1", 8554, "testStream");
//        client.setFrameCallback([](unsigned char *data, size_t length, const char *id) {
//            LOG_DEBUG(length);
//        });
//        client.run();
//    });
//    thread.detach();
//
//    //读取帧
//    unsigned char *buffer = new unsigned char[ 30 * 1024 * 1024];
//    unsigned char *frame  = new unsigned char[ 256 * 1024];
//    size_t end;
//
//    {
//        end = fread(buffer, 1, 30 * 1024 * 1024, file);
//    }
//
//    unsigned char *start = buffer;
//    auto findNAL = [](unsigned char *start, unsigned char *end) {
//        int i = 0;
//
//        for (; (start + i) != end; start++) {
//            unsigned char _1 = *(start);
//            unsigned char _2 = *(start + 1);
//            unsigned char _3 = *(start + 2);
//            unsigned char _4 = *(start + 3);
//
//            if (_1 == 0 && _2 == 0 && _3 == 0 && _4 == 1) {
//                break;
//            }
//        }
//
//        return start + i;
//    };
//
//    while (true) {
//        unsigned char *frameEnd = findNAL(start + 4, buffer + end);
//
//        size_t length             = frameEnd - start;
//
//        memset(frame, 0, 256 * 1024);
//        memcpy(frame, start, length);
//
//        if (frameEnd == buffer + end) {
//            start = buffer;
//        } else {
//            start = frameEnd;
//        }
//
//        server.addFrame(frame, length);
//        std::this_thread::sleep_for(std::chrono::microseconds(40));
//    }
//
//    fclose(file);
//    LOG_INFO("System exiting ......");
//}

int main()
{
    Common::InitLogger();
    RTSP::ProxyRTSPClient client;
    RTSP::RTSPServer server;
    Codec::Decoder *decoder = Codec::Decoder::createNew("H264");
    Codec::Encoder *encoder = Codec::Encoder::createNew("H264");

    FILE *file, *file1, *file2;
    unsigned char NAL[] = {0, 0, 0, 1};

    file = fopen("out.h264", "wb");
    file1 = fopen("out.yuv", "wb");
    file2 = fopen("encode.h264", "wb");

    server.start(8556, "1");

    LOG_INFO("System starting ......");
    client.open("127.0.0.1", 8554, "testStream");
    client.setFrameCallback([file, file1, file2, NAL, decoder, encoder, &server](unsigned char *data, size_t length, const char *id) {
        //还原NAL
        unsigned char *dataNAL = new unsigned char[length + 4];

        memcpy(dataNAL, NAL, 4);
        memcpy(dataNAL + 4, data, length);

        fwrite(dataNAL, 1, length + 4, file);
        fflush(file);

        //解码并保存yuv数据
        unsigned char *yuv = nullptr;
        size_t yuv_size = 0;

        decoder->decode(dataNAL, length + 4, yuv, yuv_size);

        //fwrite(yuv, 1, yuv_size, file1);
        //fflush(file1);

        if (yuv_size != 0) {
            unsigned char *encoded = nullptr;
            size_t encoded_size = 0;

            encoder->encode(yuv, yuv_size, encoded, encoded_size);

            if (encoded_size != 0) {            
                fwrite(encoded, 1, encoded_size, file2);
                fflush(file2);
            }

            server.addFrame(encoded, encoded_size);
            delete[] encoded;
        }

        delete[] dataNAL;
        delete[] yuv;
        
    });

    client.run();
    fclose(file);
    fclose(file1);
    LOG_INFO("System exiting ......");
}