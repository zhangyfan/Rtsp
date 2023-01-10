#include "Encoder_h264.h"
#include "logger.h"
#include <mutex>
extern "C" {
    #include "libavcodec/avcodec.h"
    #include "libavutil/imgutils.h"
}

// Windows下的解码实现使用ffmpeg编码器，如果要编译可以先下载一个ffmpeg

using namespace Codec;

#define CHECK_FFMPEG(line, error)                                   \
    {                                                               \
        int ret = line;                                             \
        if (ret < 0) {                                              \
            char *szStr = new char[1024];                           \
            memset(szStr, 0 , 1024);                                \
            av_strerror(ret, szStr, 1024);                          \
            LOG_ERROR(error " code [{}] error [{}]", ret, szStr);   \
            delete[] szStr;                                         \
            return false;                                           \
        }                                                           \
    }

class EncoderH264::impl {
public:
    impl();
    ~impl();

    void init(int width, int height, int fps);
    bool encode(unsigned char *src, size_t length, unsigned char *&dst, size_t &dstLength);

private:
    AVCodec *codec_;
    AVCodecContext *context_;
};

EncoderH264::impl::impl() {
 
}

EncoderH264::impl::~impl() {
}

void EncoderH264::impl::init(int width, int height, int fps) {
    codec_ = avcodec_find_encoder(AV_CODEC_ID_H264);

    if (!codec_) {
        LOG_ERROR("Error on find h.264 encoder from ffmpeg!");
        return;
    }

    context_ = avcodec_alloc_context3(codec_);

    if (!context_) {
        LOG_ERROR("Error on avcodec_alloc_context3!");
        return;
    }

    context_->width        = width;
    context_->height       = height;
    context_->time_base    = {1, fps};
    context_->framerate    = {fps, 1};
    context_->gop_size     = fps;
    context_->max_b_frames = 1;
    context_->pix_fmt      = AV_PIX_FMT_YUV420P;

    if (avcodec_open2(context_, codec_, nullptr) < 0) {
        LOG_ERROR("Error on avcodec_open2!");
        return;
    }
}

bool EncoderH264::impl::encode(unsigned char *src, size_t length, unsigned char *&dst, size_t &dstLength) {
    static int i      = 0;

    AVPacket *pPacket = av_packet_alloc();
    AVFrame *pFrame   = av_frame_alloc();

    if (!pPacket || !pFrame) {
        LOG_ERROR("Error on alloc packet or frame");
        return false;
    }

    pFrame->format = context_->pix_fmt;
    pFrame->width  = context_->width;
    pFrame->height = context_->height;
    pFrame->pts    = i++;

    CHECK_FFMPEG(av_frame_get_buffer(pFrame, 0), "Error on av_frame_get_buffer");
    CHECK_FFMPEG(av_frame_make_writable(pFrame), "Error on av_frame_make_writable");
    CHECK_FFMPEG(av_image_fill_arrays(pFrame->data, pFrame->linesize, src, context_->pix_fmt, context_->width, context_->height, 1), 
        "Error on av_image_fill_arrays");
    CHECK_FFMPEG(avcodec_send_frame(context_, pFrame), "Error on avcodec_send_frame");
    CHECK_FFMPEG(avcodec_receive_packet(context_, pPacket), "Error on avcodec_receive_packet");

    dstLength = pPacket->size;
    dst = new unsigned char[pPacket->size];
    memcpy(dst, pPacket->data, pPacket->size);

    return true;
}

//----------------------------------------------------------------------------------------------------
EncoderH264::EncoderH264() {
    impl_ = new impl();
}

EncoderH264::~EncoderH264() {
    if (impl_) {
        delete impl_;
    }
}

void EncoderH264::init(int width, int height, int fps) {
    impl_->init(width, height, fps);
}

bool EncoderH264::encode(unsigned char *src, size_t length, unsigned char *&dst, size_t &dstLength) {
    return impl_->encode(src, length, dst, dstLength);
}