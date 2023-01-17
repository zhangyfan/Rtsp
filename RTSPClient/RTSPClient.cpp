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
    void setFrameCallback(const std::function<void(unsigned char *, size_t)> &callback);
    void run();
    int getVideoWidth();
    int getVideoHeight();

private:
    std::string makeURL(const std::string &addr, int port, const std::string &path, const std::string &user, const std::string &passwd);

private:
    std::function<void(unsigned char *, size_t)> onFrame_;
    AVFormatContext *fmtCtx_;
    AVCodecContext *codecCtx_;
    int vsIndex = 0;
};

ProxyRTSPClient::impl::impl() {
    fmtCtx_ = avformat_alloc_context();
    codecCtx_ = NULL;
}

ProxyRTSPClient::impl::~impl() {
}

std::string ProxyRTSPClient::impl::makeURL(const std::string &addr, int port, const std::string &path, const std::string &user, const std::string &passwd) {
    // rtsp://[username[:password]@]ip_address[:rtsp_port]/path
    std::stringstream ss;

    ss << "rtsp://";

    if (!user.empty() && !passwd.empty()) {
        ss << user << ":" << passwd << "@";
    }

    ss << addr << ":" << port << "/" << path;
    return ss.str();
}

bool ProxyRTSPClient::impl::open(const std::string &addr, int port, const std::string &path, const std::string &user, const std::string &passwd) {
    std::string url = addr; // makeURL(addr, port, path, user, passwd);
    int ret         = avformat_open_input(&fmtCtx_, url.c_str(), NULL, NULL);

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
    av_read_pause(fmtCtx_);
    return true;
}

void ProxyRTSPClient::impl::setFrameCallback(const std::function<void(unsigned char *, size_t)> &callback)
{
    onFrame_ = callback;
}

void ProxyRTSPClient::impl::run() 
{
    while (true) {    
        AVPacket packet;

        av_init_packet(&packet);
        int ret = av_read_frame(fmtCtx_, &packet);

        if (ret >= 0 && onFrame_ && packet.stream_index == vsIndex) {
            onFrame_(packet.data, packet.size);
        }

        av_free_packet(&packet);
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

void ProxyRTSPClient::setFrameCallback(const std::function<void(unsigned char *, size_t)> &callback) {
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
} // namespace RTSP