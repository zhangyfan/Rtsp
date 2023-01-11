#include "logger.h"
#include "RTSPClient.h"
#include "RTMPServer.h"
#include "Decoder.h"
#include "Encoder.h"
#include "Operators.h"
#include <iostream>
#include <fstream>
extern "C" {
#include <libavformat/avformat.h>
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}

RTSP::ProxyRTSPClient *client;
RTMPServer *server;
Codec::Decoder *decoder;
Codec::Encoder *encoder;
FILE *file;
Operators *operators = nullptr;

void onDecodedYUV(unsigned char *BGR888, unsigned char *YUV420, int width, int height);
std::pair<unsigned char *, size_t> reencode(unsigned char *YUV420, size_t length);

std::pair<unsigned char*, size_t> toYUV420(AVFrame *frame) {
    int width  = frame->width;
    int height = frame->height;
    size_t length = width * height * 3;
    unsigned char *data = (unsigned char *)malloc(length);

    av_image_copy_to_buffer(data, (int)length, frame->data, frame->linesize, AV_PIX_FMT_YUV420P, width, height, 1);
    return std::make_pair(data, length);
}

std::pair<unsigned char *, size_t> toBGR888(AVFrame *frame) {
    int width              = frame->width;
    int height             = frame->height;
    SwsContext *swsContext = sws_getContext(
        width, height, AV_PIX_FMT_YUV420P, width, height, AV_PIX_FMT_BGR24, NULL, NULL, NULL, NULL);
    int linesize[8]        = {frame->linesize[0] * 3};
    int numBytes           = av_image_get_buffer_size(AV_PIX_FMT_BGR24, width, height, 1);
    auto buffer            = (uint8_t *)malloc(numBytes * sizeof(uint8_t));
    uint8_t *bgrBuffer[8]  = {buffer};

    sws_scale(swsContext, frame->data, frame->linesize, 0, height, bgrBuffer, linesize);

    size_t length = width * height * 3;
    unsigned char *data = (unsigned char *)malloc(length); 

    memcpy(data, bgrBuffer[0], length);
    free(buffer);

    return std::make_pair(data, length);
}

void onReceiveFrame(unsigned char *data, size_t length) {
    //解码并保存yuv数据
    AVFrame *avFrame = decoder->decode(data, length);
    if (!avFrame) {
        return;
    }
    //拷贝一份BGR888数据，给算法调用
    unsigned char *BGR888 = nullptr;
    size_t BGRSize = 0;
    unsigned char *YUV420 = nullptr;
    size_t YUVSize        = 0;

    std::tie(BGR888, BGRSize) = toBGR888(avFrame);
    std::tie(YUV420, YUVSize) = toYUV420(avFrame);

    onDecodedYUV(BGR888, YUV420, avFrame->width, avFrame->height);

    //重新编码
    unsigned char *h264Data = nullptr;
    size_t h264Length       = 0;

    std::tie(h264Data, h264Length) = reencode(YUV420, YUVSize);

    if (h264Data) {
        server->addFrame(h264Data, h264Length);
    }

    delete[] h264Data;
    free(BGR888);
    free(YUV420);
    av_frame_free(&avFrame);
}

void onDecodedYUV(unsigned char *BGR888, unsigned char *YUV420, int width, int height) {
    //调用算法
    if (operators) {
        operators->detect(BGR888, width, height, YUV420);
    }
}

std::pair<unsigned char*, size_t> reencode(unsigned char *YUV420, size_t length) {
    //编码
    unsigned char *encoded = nullptr;
    size_t encodedSize = 0;

    encoder->encode(YUV420, length, encoded, encodedSize);
    return std::make_pair(encoded, encodedSize);
}

int main(int argc, char *argv[]) {
    Common::InitLogger();    
    decoder = Codec::Decoder::createNew("H264");
    encoder = Codec::Encoder::createNew("H264");

    client  = new RTSP::ProxyRTSPClient();

    if (!client->open(argv[1], 0, "")) {
        LOG_ERROR("stream can not open!!!!");
        return 0;
    }

    operators = new Operators();
    operators->init();
    
    //初始化编码器，解码器因为有PPS/SPS所有不需要额外的信息
    int width  = 1920; // client->getVideoWidth();
    int height = 1080; // client->getVideoHeight();

    encoder->init(width, height, 25);

    server = new RTMPServer();
    server->init(width, height, 25, argv[2]);
    server->start();

    LOG_INFO("Stream open success");
    client->setFrameCallback(onReceiveFrame);
    client->run();
    LOG_INFO("System exiting ......");
}