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

    bool decode(unsigned char *src, size_t length, unsigned char *&dst, size_t &dstLength);

private:
    AVCodec *codec_;
    AVCodecContext *context_;
};

DecoderH264::impl::impl() {
#ifdef _MSC_VER
    codec_ = avcodec_find_decoder(AV_CODEC_ID_H264);
#else
    codec_ = avcodec_find_decoder_by_name("h264"/*"h264_rkmpp"*/);
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

bool DecoderH264::impl::decode(unsigned char *src, size_t length, unsigned char *&dst, size_t &dstLength) {
    AVPacket *pPacket = av_packet_alloc();
    AVFrame *pFrame   = av_frame_alloc();

    if (!pPacket || !pFrame) {
        LOG_ERROR("Error on alloc packet or frame");
        return false;
    }

    av_init_packet(pPacket);
    
    pPacket->size = length;
    pPacket->data = src;

    int ret       = avcodec_send_packet(context_, pPacket);

    if (ret < 0) {
        return false;
    }

    // 解码
    ret = avcodec_receive_frame(context_, pFrame);
        
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF || ret < 0) {
        return false;
    }

    int width        = pFrame->width;
    int height       = pFrame->height;

    dstLength = width * height * 3;
    dst       = (unsigned char *)malloc(dstLength);

    //转换为BGR24
    SwsContext* swsContext = swsContext = sws_getContext(pFrame->width, pFrame->height, AV_PIX_FMT_YUV420P,pFrame->width, pFrame->height, AV_PIX_FMT_BGR24, NULL, NULL, NULL, NULL);
    int linesize[8] = {pFrame->linesize[0] * 3};
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_BGR24, pFrame->width, pFrame->height, 1);
    auto buffer = (uint8_t*) malloc(numBytes * sizeof(uint8_t));
    uint8_t* bgrBuffer[8] = {buffer};

    sws_scale(swsContext, pFrame->data, pFrame->linesize, 0, pFrame->height, bgrBuffer, linesize);
    memcpy(dst, bgrBuffer[0], dstLength);

    free(buffer);
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