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
    FILE *file = fopen("out.yuv", "wb");

    server.start(8556, "1");

    if (!client.open("127.0.0.1", 8554, "1")) {
        LOG_ERROR("stream can not open!!!!");
        return 0;
    }

    LOG_INFO("Stream open success");

    client.setFrameCallback([&encoder, &decoder, &server, file](unsigned char *data, size_t length) {
        //解码并保存yuv数据
        unsigned char *yuv = nullptr;
        size_t yuv_size = 0;

        decoder->decode(data, length, yuv, yuv_size);

        if (yuv_size != 0) {
            unsigned char *encoded = nullptr;
            size_t encoded_size = 0;        

            encoder->encode(yuv, yuv_size, encoded, encoded_size);

            if (encoded) {
                server.addFrame(encoded, encoded_size);
                delete[] encoded;
            }
        }

        delete[] yuv;
        std::this_thread::sleep_for(std::chrono::microseconds(45));
    });

    client.run();
    std::cin.get();
    LOG_INFO("System exiting ......");
}