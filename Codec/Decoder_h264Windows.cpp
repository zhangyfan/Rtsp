#include "Decoder_h264.h"
#include "logger.h"
#include <mutex>
extern "C" {
    #include "libavcodec/avcodec.h"
    #include "libavutil/imgutils.h"
    #include "libswscale/swscale.h"
}

// Windows下的解码实现使用ffmpeg解码器，如果要编译可以先下载一个ffmpeg

using namespace Codec;

class DecoderH264::impl {
public:
    impl();
    ~impl();

    AVFrame *decode(unsigned char *src, size_t length);

private:
    void initHWContext();

private:
    AVCodec *codec_;
    AVCodecContext *context_;
    AVBufferRef *hwCtx_ = nullptr;
};

DecoderH264::impl::impl() {
#ifdef _MSC_VER
    codec_ = avcodec_find_decoder_by_name("h264");
#else
    codec_ = avcodec_find_decoder_by_name("h264");
#endif
    if (!codec_) {
        LOG_ERROR("Error on find h.264 decoder from ffmpeg!");
        return;
    }

    context_ = avcodec_alloc_context3(codec_);

    if (!context_) {
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

void DecoderH264::impl::initHWContext() {
    int err = 0;

    if ((err = av_hwdevice_ctx_create(&hwCtx_, AV_HWDEVICE_TYPE_DRM, "drm", NULL, 0)) < 0) {
        LOG_ERROR("Failed to create specified HW device [{}]", err);
        return;
    }

    context_->hw_device_ctx = av_buffer_ref(hwCtx_);
}

AVFrame *DecoderH264::impl::decode(unsigned char *src, size_t length) {
    AVPacket *pPacket = av_packet_alloc();
    AVFrame *pFrame   = av_frame_alloc();

    if (!pPacket || !pFrame) {
        LOG_ERROR("Error on alloc packet or frame");
        return nullptr;
    }

    av_init_packet(pPacket);
    
    pPacket->size = (int)length;
    pPacket->data = src;

    int ret       = avcodec_send_packet(context_, pPacket);

    if (ret < 0) {
        return nullptr;
    }

    // 解码
    ret = avcodec_receive_frame(context_, pFrame);
        
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF || ret < 0) {
        return nullptr;
    }

    return pFrame;
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

AVFrame * DecoderH264::decode(unsigned char *src, size_t length) {
    return impl_->decode(src, length);
}