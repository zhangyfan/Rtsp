#include "Decoder_h264.h"
#include "logger.h"
#include <mutex>
#include <thread>
extern "C" {
    #include "libavcodec/avcodec.h"
    #include "libavutil/imgutils.h"
    #include "libswscale/swscale.h"
    #include "libavutil/hwcontext_drm.h"
}

// Windows下的解码实现使用ffmpeg解码器，如果要编译可以先下载一个ffmpeg

using namespace Codec;

class DecoderH264::impl {
public:
    impl();
    ~impl();

    std::vector<AVFrame *> decode(AVPacket *pkt);

private:
    void receiveFrames(std::vector<AVFrame *> &vec);

private:
    AVCodec *codec_;
    AVCodecContext *context_;
    AVBufferRef *hwCtx_ = nullptr;
};

DecoderH264::impl::impl() {
#ifdef _MSC_VER
    codec_ = avcodec_find_decoder_by_name("h264");
#else
    codec_ = avcodec_find_decoder_by_name("h264_rkmpp");
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

void DecoderH264::impl::receiveFrames(std::vector<AVFrame*>& vec) {
    while (true) {
        AVFrame *pFrame = av_frame_alloc();
        int ret         = avcodec_receive_frame(context_, pFrame);

        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF || ret < 0) {
            av_frame_free(&pFrame);
            break;
        }
        vec.push_back(pFrame);
    }
}

std::vector<AVFrame *> DecoderH264::impl::decode(AVPacket *pkt) {
    std::vector<AVFrame *> res;
    int ret       = avcodec_send_packet(context_, pkt);

    if (ret == -11) {
        receiveFrames(res);
        avcodec_send_packet(context_, pkt);
    }

    if (ret < 0) {
        return std::vector<AVFrame *>();
    }

    receiveFrames(res);
    return res;
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

std::vector<AVFrame *> DecoderH264::decode(AVPacket *pkt) {
    return impl_->decode(pkt);
}