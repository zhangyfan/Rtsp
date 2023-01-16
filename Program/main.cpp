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

#ifdef _MSC_VER
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

    sws_freeContext(swsContext);
    return std::make_pair(data, length);
}
#else
#include "mpp_frame.h"

typedef struct
{
    MppFrame frame;
    AVBufferRef *decoder_ref;
} RKMPPFrameContext;
//Warning
// Linux下输出的是MPP结构，其内部数据就是YUV420的
// 如果解码返回多个AVFrame，好像是分成了一片一片的

MppFrame AVFrame2MppFrame(AVFrame *avframe) {
    MppBuffer mppbuff;

    if (avframe->format != AV_PIX_FMT_DRM_PRIME) {
        return NULL;
    }
    AVBufferRef *framecontextref = (AVBufferRef *)av_buffer_get_opaque(
        avframe->buf[0]);
    RKMPPFrameContext *framecontext =
        (RKMPPFrameContext *)framecontextref->data;

    return framecontext->frame;
}

std::pair<unsigned char *, size_t> toYUV420(AVFrame *frame) {
    MppFrame mppFrame      = AVFrame2MppFrame(frame);
    int width              = frame->width;
    int height             = frame->height;
    MppBuffer buffer       = mpp_frame_get_buffer(mppFrame);

    return std::make_pair((unsigned char *)mpp_buffer_get_ptr(buffer), width * height * 3 / 2);
}

std::pair<unsigned char *, size_t> toBGR888(AVFrame *frame) {
    AVPicture pFrameYUV, pFrameBGR;
    MppBuffer buffer = mpp_frame_get_buffer(frame);
    uint8_t * pYUV    = (uint8_t *)mpp_buffer_get_ptr(buffer);
    int width        = frame->width;
    int height       = frame->height;

    avpicture_fill(&pFrameYUV, pYUV, AV_PIX_FMT_YUV420P, width, height);

    // U,V互换
    uint8_t *ptmp     = pFrameYUV.data[1];
    pFrameYUV.data[1] = pFrameYUV.data[2];
    pFrameYUV.data[2] = ptmp;

    size_t size       = width * height * 3;
    uint8_t *pBGR24   = new uint8_t[size];

    avpicture_fill(&pFrameBGR, pBGR24, AV_PIX_FMT_BGR24,width, height); //BGR

    struct SwsContext *imgCtx = NULL;
    imgCtx                    = sws_getContext(width, height, AV_PIX_FMT_YUV420P, width, height, AV_PIX_FMT_BGR24, SWS_BILINEAR, 0, 0, 0);

    if (imgCtx != NULL) {
        sws_scale(imgCtx,pFrameYUV.data,pFrameYUV.linesize,0,height,pFrameBGR.data,pFrameBGR.linesize);

        if (imgCtx) {
            sws_freeContext(imgCtx);
            imgCtx = NULL;
        }
    } else {
        sws_freeContext(imgCtx);
        imgCtx = NULL;
    }

    sws_freeContext(imgCtx);
    return std::make_pair(pBGR24, size);
}


#endif // _MSC_VER


void onReceiveFrame(unsigned char *data, size_t length) {
    //解码并保存yuv数据
    std::vector<AVFrame *> frames = decoder->decode(data, length);

    for (auto avFrame: frames) {
        if (!avFrame) {
            continue;
        }

        //拷贝一份BGR888数据，给算法调用
        unsigned char *BGR888 = nullptr;
        size_t BGRSize = 0;
        unsigned char *YUV420 = nullptr;
        size_t YUVSize        = 0;

        //std::tie(BGR888, BGRSize) = toBGR888(avFrame);
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
#ifdef _MSC_VER
        free(YUV420); // Linux下不删除
#endif // _MSC_VER
        //free(BGR888);
        av_frame_free(&avFrame);
    }
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
    int width  = client->getVideoWidth();
    int height = client->getVideoHeight();

    encoder->init(width, height, 25);

    server = new RTMPServer();
    server->init(width, height, 25, argv[2]);
    server->start();

    LOG_INFO("Stream open success");
    client->setFrameCallback(onReceiveFrame);
    client->run();
    LOG_INFO("System exiting ......");
}