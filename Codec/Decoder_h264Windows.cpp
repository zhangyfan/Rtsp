#include "Decoder_h264.h"
#include "logger.h"
#include <mutex>
extern "C" {
    #include "libavcodec/avcodec.h"
}

// Windows下的解码实现使用ffmpeg解码器，如果要编译可以先下载一个ffmpeg

using namespace Codec;

class DecoderH264::impl {
public:
    impl();
    ~impl();

    bool decode(unsigned char *src, size_t length, unsigned char *&dst, size_t &dstLength);

private:
    AVCodec *codec_;
    AVCodecParserContext *parseContext_;
    AVCodecContext *context_;
};

DecoderH264::impl::impl() {
    codec_ = avcodec_find_decoder(AV_CODEC_ID_H264);

    if (!codec_) {
        LOG_ERROR("Error on find h.264 decoder from ffmpeg!");
        return;
    }

    parseContext_ = av_parser_init(codec_->id);

    if (!parseContext_) {
        LOG_ERROR("Error on av_parser_init!");
        return;
    }

    context_ = avcodec_alloc_context3(codec_);

    if (!parseContext_) {
        LOG_ERROR("Error on avcodec_alloc_context3!");
        return;
    }

    if (avcodec_open2(context_, codec_, nullptr) < 0) {
        LOG_ERROR("Error on avcodec_open2!");
        return;
    }
}

DecoderH264::impl::~impl() {
}

bool DecoderH264::impl::decode(unsigned char *src, size_t length, unsigned char *&dst, size_t &dstLength) {
    AVPacket *pPacket = av_packet_alloc();
    AVFrame *pFrame   = av_frame_alloc();

    if (!pPacket || !pFrame) {
        LOG_ERROR("Error on alloc packet or frame");
        return false;
    }

    pPacket->size = length;
    pPacket->data = src;

    int ret       = avcodec_send_packet(context_, pPacket);

    if (ret < 0) {
        return false;
    }

    int outLength = 0, gotFrame = 0;

    // 解码
    outLength = avcodec_decode_video2(context_, pFrame, &gotFrame, pPacket);

    if (!gotFrame) {
        return false;
    }
    int width        = pFrame->width;
    int height       = pFrame->height;
    int linesize_y   = pFrame->linesize[0];
    int linesize_uv  = pFrame->linesize[1];

    dst              = (unsigned char *)malloc(width * height * 3 / 2);
    unsigned char *y = dst, *u = y + height * width, *v = u + height / 2 * width / 2;

    // 分别拷贝yuv数据
    for (int i = 0; i < height; i++) {
        memcpy(y + i * width, pFrame->data[0] + i * linesize_y, width);
    }
    for (int i = 0; i < height / 2; i++) {
        memcpy(u + i * width / 2, pFrame->data[1] + i * linesize_uv, width / 2);
    }
    for (int i = 0; i < height / 2; i++) {
        memcpy(v + i * width / 2, pFrame->data[2] + i * linesize_uv, width / 2);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
DecoderH264::DecoderH264() {
    impl_ = new impl();
}

DecoderH264::~DecoderH264() {
    if (impl_) {
        delete impl_;
    }
}

bool DecoderH264::decode(unsigned char *src, size_t length, unsigned char *&dst, size_t &dstLength) {
    return impl_->decode(src, length, dst, dstLength);
}