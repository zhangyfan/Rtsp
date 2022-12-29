#include "logger.h"
#include "RTSPClient.h"
#include "Decoder.h"
#include <iostream>

int main()
{
    Common::InitLogger();
    RTSP::ProxyRTSPClient client;
    Codec::Decoder *decoder = Codec::Decoder::createNew("H264");

    FILE *file, *file1;
    unsigned char NAL[] = {0, 0, 0, 1};

    file = fopen("out.h264", "wb");
    file1 = fopen("out.yuv", "wb");

    LOG_INFO("System starting ......");
    client.open("127.0.0.1", 8554, "testStream");
    client.setFrameCallback([file, file1, NAL, decoder](unsigned char *data, size_t length, const char *id) {
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

        fwrite(yuv, 1, yuv_size, file1);
        fflush(file1);
    });

    client.run();
    fclose(file);
    fclose(file1);
    LOG_INFO("System exiting ......");
}