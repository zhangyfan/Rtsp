#include "RTMPServer.h"
#include "logger.h"
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/time.h"
}


#ifdef _MSC_VER
#pragma comment(lib, "ws2_32")
#endif // _MSC_VER


bool isIdrFrame(uint8_t* buf, int len) {
	switch (buf[0] & 0x1f) {
	case 7: // SPS
		return true;
	case 8: // PPS
		return true;
	case 5:
		return true;
	case 1:
		return false;
	default:
		return false;
	}
 
	return false;
}

class RTMPServer::impl {
public:
    impl();
    ~impl();

    void init(int width, int height, int fps, const std::string &url);
    void start();
    bool addFrame(unsigned char *data, size_t length);

private:
    int pts = 0;
    std::string rtmpURL = "rtmp://127.0.0.1:1935/1";
    AVFormatContext *fmtCtx_;
    std::atomic_bool connected_ = false;
    AVCodec *codec_;
    AVCodecContext *codec_context_;
    AVStream *vstream_;
};

RTMPServer::impl::impl() {
}

RTMPServer::impl::~impl() {
}

void RTMPServer::impl::init(int width, int height, int fps, const std::string &url) {
    av_register_all();
    avformat_network_init();
    rtmpURL = url;

    int ret                    = avformat_alloc_output_context2(&fmtCtx_, NULL, "flv", rtmpURL.c_str());
#ifdef _MSC_VER
    codec_ = avcodec_find_encoder(AV_CODEC_ID_H264);
#else
    codec_ = avcodec_find_encoder_by_name("libx264");
#endif // _MSC_VER

    codec_context_             = avcodec_alloc_context3(codec_);
    vstream_                   = avformat_new_stream(fmtCtx_, NULL);

    vstream_->id               = 0;
    vstream_->codec            = codec_context_;

    codec_context_->codec_id   = codec_->id;
    codec_context_->codec_type = AVMEDIA_TYPE_VIDEO;
    codec_context_->pix_fmt    = AV_PIX_FMT_YUV420P;
    codec_context_->bit_rate   = 4000000;
    codec_context_->width      = width;
    codec_context_->height     = height;

    AVRational time_base       = {1, fps};
    vstream_->time_base        = time_base;
    codec_context_->time_base  = time_base;
    codec_context_->codec_tag  = 0;

    if (fmtCtx_->oformat->flags & AVFMT_GLOBALHEADER) {
        codec_context_->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    ret = avcodec_open2(codec_context_, codec_, NULL);
    if (ret < 0) {
        LOG_ERROR("open video codec failed !");
    }

    vstream_->codecpar = avcodec_parameters_alloc();

    if (avcodec_parameters_from_context(vstream_->codecpar, codec_context_) < 0) {
        LOG_ERROR("acvodec_parameters_from_context failed!\n");
    }

    av_dump_format(fmtCtx_, 0, rtmpURL.c_str(), 1);
}
    
void RTMPServer::impl::start() {
    std::thread thread([this]() {
        int ret = avio_open(&fmtCtx_->pb, rtmpURL.c_str(), AVIO_FLAG_WRITE);

        if (ret < 0) {
            return;
        }

        ret = avformat_write_header(fmtCtx_, 0);
        if (ret < 0) {
            return;
        }

        connected_ = true;
    });

    thread.detach();
}

bool RTMPServer::impl::addFrame(unsigned char* data, size_t length) {
    if (!connected_) {
        return false;//未连接到RTMP接收方，不发送否则会报错
    }
    int isI           = 0;
    AVPacket pkt      = {0};

    av_init_packet(&pkt);

    isI = isIdrFrame((uint8_t *)data, length);
    pkt.flags |= isI ? AV_PKT_FLAG_KEY : 0;
    pkt.data             = (uint8_t *)data;
    pkt.size             = length;

    AVRational time_base = {1, 1000};
    pkt.pts              = av_rescale_q(pts++, time_base, vstream_->time_base);

    enum AVRounding rnd  = (enum AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
    pkt.dts              = av_rescale_q_rnd(pkt.dts, vstream_->time_base, vstream_->time_base, rnd);
    pkt.duration         = av_rescale_q(pkt.duration, vstream_->time_base, vstream_->time_base);
    pkt.pos              = -1;

    // rescale output packet timestamp values from  codec to stream timebase
    av_packet_rescale_ts(&pkt, time_base, vstream_->time_base);
    pkt.stream_index = vstream_->index;

    int ret = av_write_frame(fmtCtx_, &pkt);
    if (ret < 0) {
        LOG_ERROR("av_write_frame error");
    }
    return true;
}

//--------------------------------------------------------------
RTMPServer::RTMPServer() {
    impl_ = new impl();
}

RTMPServer::~RTMPServer() {
    if (impl_) {
        delete impl_;
    }
}

void RTMPServer::start() {
    impl_->start();
}

bool RTMPServer::addFrame(unsigned char *data, size_t length) {
    return impl_->addFrame(data, length);
}

void RTMPServer::init(int width, int height, int fps, const std::string &url) {
    return impl_->init(width, height, fps, url);
}