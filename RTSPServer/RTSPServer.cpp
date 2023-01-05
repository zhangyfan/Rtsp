#include "RTSPServer.h"
#include "logger.h"
#include "H264LiveServerMediaSession.h"
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>
#include <liveMedia.hh>

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

namespace RTSP {
//定义rtp和rtcp端口
constexpr int rtpPortNum  = 38888;
constexpr int rtcpPortNum = rtpPortNum + 1;
constexpr int ttl         = 255;

class RTSPServer::impl {
public:
    impl();
    ~impl();

    void start(int port, const std::string &stream);
    bool addFrame(unsigned char *data, size_t length);

private:
    TaskScheduler *taskSchedular_;
    BasicUsageEnvironment *env_ = nullptr;
    ::RTSPServer *server_;
    H264LiveServerMediaSession *liveSubSession_;
};

RTSPServer::impl::impl() {
    //初始化通用环境
    OutPacketBuffer::maxSize = 4 * 1024 * 1024;
    taskSchedular_ = BasicTaskScheduler::createNew();
    env_           = BasicUsageEnvironment::createNew(*taskSchedular_);
}

RTSPServer::impl::~impl() {
}

void RTSPServer::impl::start(int port, const std::string &stream) {
    server_ = ::RTSPServer::createNew(*env_, port);

    if (server_ == NULL) {
        LOG_ERROR("Failed to create RTSP server: {}", env_->getResultMsg());
        return;
    }

    ServerMediaSession *sms = ServerMediaSession::createNew(
        *env_, 
        stream.c_str(), 
        NULL,
        "Session streamed by \"testH264VideoStreamer\"",
        True);

    liveSubSession_ = H264LiveServerMediaSession::createNew(*env_, True);
    sms->addSubsession(liveSubSession_);
    server_->addServerMediaSession(sms);

    //打印日志
    char *url = server_->rtspURL(sms);
    LOG_INFO("Started rtsp server URL [{}]", url);
    delete[] url;

    //开启单独的线程处理
    std::thread thread([this]() {
        taskSchedular_->doEventLoop();
    });

    thread.detach();
}

bool RTSPServer::impl::addFrame(unsigned char* data, size_t length) {
    if (liveSubSession_->getSource()) {
        liveSubSession_->getSource()->addFrame(data, length);
        return true;
    }

    return false;
}

//--------------------------------------------------------------
RTSPServer::RTSPServer() {
    impl_ = new impl();
}

RTSPServer::~RTSPServer() {
    if (impl_) {
        delete impl_;
    }
}

void RTSPServer::start(int port, const std::string &stream) {
    impl_->start(port, stream);
}

bool RTSPServer::addFrame(unsigned char *data, size_t length) {
    return impl_->addFrame(data, length);
}
} // namespace RTSP