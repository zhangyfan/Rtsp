#include "RTSPClient.h"
#include "logger.h"
#include <sstream>
extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavformat/avio.h>
    #include <libswscale/swscale.h>
}

namespace RTSP {
class ProxyRTSPClient::impl {
public:
    impl();
    ~impl();

    bool open(const std::string &addr, int port, const std::string &path, const std::string &user, const std::string &passwd);
    bool close();
    void setFrameCallback(const std::function<void(AVPacket *)> &callback);
    void setErrorCallback(const std::function<void()> &func); 
    void run();
    int getVideoWidth();
    int getVideoHeight();
    double getFps();

private:
    static int InterruptCB(void *);
    int InterruptCB();
    void resetStartEpoch();

private:
    std::function<void(AVPacket *)> onFrame_;
    std::function<void()> onError_;
    AVFormatContext *fmtCtx_;
    int vsIndex = 0;
    std::string url_;
    std::chrono::system_clock::time_point tp_;
};

ProxyRTSPClient::impl::impl() {
}

ProxyRTSPClient::impl::~impl() {
}

//连接失败直接五秒后重试
int ProxyRTSPClient::impl::InterruptCB(void *ptr) {
    return ((ProxyRTSPClient::impl *)ptr)->InterruptCB();
}

int ProxyRTSPClient::impl::InterruptCB() {
    using namespace std::chrono;

    auto now = system_clock::now();
    auto dura = now - tp_;

    if (dura > std::chrono::seconds(5)) {
        return 1;
    }

    return 0;
}

void ProxyRTSPClient::impl::resetStartEpoch() {
    using namespace std::chrono;
    tp_ = system_clock::now();
}

bool ProxyRTSPClient::impl::open(const std::string &addr, int port, const std::string &path, const std::string &user, const std::string &passwd) {
    fmtCtx_ = avformat_alloc_context();
    url_    = addr;
    
    //初始化超时时间
    AVIOInterruptCB intCB;

    intCB.callback              = InterruptCB;
    intCB.opaque                = (void *)this;
    fmtCtx_->interrupt_callback = intCB;

    //设置开始时间
    resetStartEpoch();

    int ret                     = avformat_open_input(&fmtCtx_, url_.c_str(), nullptr, nullptr);

    // open RTSP
    if (ret != 0) {
        return false;
    }

    if (avformat_find_stream_info(fmtCtx_, NULL) < 0) {
        return false;
    }

    for (unsigned int i = 0; i < fmtCtx_->nb_streams; ++i) {
        const AVStream *stream = fmtCtx_->streams[i];
        
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            vsIndex = stream->index;
        }
    }

    return true;
}

bool ProxyRTSPClient::impl::close()
{
    avformat_free_context(fmtCtx_);
    return true;
}

void ProxyRTSPClient::impl::setFrameCallback(const std::function<void(AVPacket *)> &callback)
{
    onFrame_ = callback;
}

void ProxyRTSPClient::impl::setErrorCallback(const std::function<void()>& func) {
    onError_ = func;
}

void ProxyRTSPClient::impl::run() 
{
    while (true) {
        AVPacket *packet = av_packet_alloc();

        av_init_packet(packet);
        resetStartEpoch();

        int ret = av_read_frame(fmtCtx_, packet);

        if (ret < 0 && onError_) {
            onError_();
            av_packet_free(&packet);
            break;
        }

        if (ret >= 0 && onFrame_ && packet->stream_index == vsIndex) {
            onFrame_(packet);
        }

        av_packet_free(&packet);
    }
}

int ProxyRTSPClient::impl::getVideoWidth() {
    if (fmtCtx_->nb_streams > 0) {
        return fmtCtx_->streams[vsIndex]->codecpar->width;
    }

    return 0;
}

int ProxyRTSPClient::impl::getVideoHeight() {
    if (fmtCtx_->nb_streams > 0) {
        return fmtCtx_->streams[vsIndex]->codecpar->height;
    }

    return 0;
}

double ProxyRTSPClient::impl::getFps() {
    if (fmtCtx_->nb_streams > 0) {
        return av_q2d(fmtCtx_->streams[vsIndex]->r_frame_rate);
    }

    return 0.0;
}

//---------------------------------------------------------------------------------------------------------
ProxyRTSPClient::ProxyRTSPClient() {
    m_impl = new impl();
}

ProxyRTSPClient::~ProxyRTSPClient() {
    if (m_impl) {
        delete m_impl;
    }
}

bool ProxyRTSPClient::open(const std::string &addr, int port, const std::string &path, const std::string &user, const std::string &passwd) {
    return m_impl->open(addr, port, path, user, passwd);
}

bool ProxyRTSPClient::close() {
    return m_impl->close();
}

void ProxyRTSPClient::setFrameCallback(const std::function<void(AVPacket *)> &callback) {
    m_impl->setFrameCallback(callback);
}

void ProxyRTSPClient::run() {
    m_impl->run();
}

int ProxyRTSPClient::getVideoWidth() {
    return m_impl->getVideoWidth();
}

int ProxyRTSPClient::getVideoHeight() {
    return m_impl->getVideoHeight();
}

double ProxyRTSPClient::getFps() {
    return m_impl->getFps();
}

void ProxyRTSPClient::setErrorCallback(const std::function<void()> &func) {
    m_impl->setErrorCallback(func);
}
} // namespace RTSP