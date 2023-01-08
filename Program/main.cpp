#include "logger.h"
#include "RTSPClient.h"
#include "RTSPServer.h"
#include "Decoder.h"
#include "Encoder.h"
#include <iostream>
#include <fstream>

int main()
{
    Common::InitLogger();
    RTSP::RTSPServer server;
    RTSP::ProxyRTSPClient client;
    
    Codec::Decoder *decoder = Codec::Decoder::createNew("H264");
    Codec::Encoder *encoder = Codec::Encoder::createNew("H264");
    unsigned char NAL[] = {0, 0, 0, 1};

    FILE *file = fopen("enc.h264", "wb");

    server.start(8554, "1");

    LOG_INFO("System starting ......");

    {
        unsigned char *data;
        size_t length;
        
        encoder->readSPS(data, length);
        
        LOG_INFO("SPS read {}", length);
        fwrite(data, 1, length, file);
        fflush(file);
    }

    client.open("192.168.1.107", 8554, "testStream");
    client.setFrameCallback([&encoder, &decoder, NAL, &server, file](unsigned char *data, size_t length, const char *id) {
        //还原NAL
        unsigned char *dataNAL = new unsigned char[length + 4];

        memcpy(dataNAL, NAL, 4);
        memcpy(dataNAL + 4, data, length);

        //解码并保存yuv数据
        unsigned char *yuv = nullptr;
        size_t yuv_size = 0;

        decoder->decode(dataNAL, length + 4, yuv, yuv_size);

        if (yuv_size != 0) {
            unsigned char *encoded = nullptr;
            size_t encoded_size = 0;        

            encoder->encode(yuv, yuv_size, encoded, encoded_size);

            fwrite(encoded, 1, encoded_size, file);
            fflush(file);
            //server.addFrame(encoded, encoded_size);
            delete[] encoded;
        }

        //server.addFrame(data, length);
        delete[] dataNAL;
        delete[] yuv;
        std::this_thread::sleep_for(std::chrono::microseconds(45));
    });

    std::thread clientThread([&client]() {
        client.run();
    });

    clientThread.join();
    LOG_INFO("System exiting ......");
}