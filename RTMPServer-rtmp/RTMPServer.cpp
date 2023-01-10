#include "RTMPServer.h"
#include "logger.h"
#include "rtmp.h"

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

    void init(int width, int height, int fps);
    void start();
    bool addFrame(unsigned char *data, size_t length);

private:
    int width_ = 1920;
    int height_ = 816;
    int pts = 0;
    std::string rtmpURL = "rtmp://192.168.1.107:1935/1";
    ::RTMP *rtmp_;
    std::atomic_bool connected_ = false;
    uint32_t timeStamp = 0;
};

RTMPServer::impl::impl() {
}

RTMPServer::impl::~impl() {
}

void RTMPServer::impl::init(int width, int height, int fps) {
    rtmp_ = RTMP_Alloc();

    if (!rtmp_) {
        LOG_ERROR("Error on RTMP_Alloc");
        return;
    }

    RTMP_Init(rtmp_);

    int ret = RTMP_SetupURL(rtmp_, rtmpURL.data());

    if (ret <= 0) {
        LOG_ERROR("Error on RTMP_SetupURL [{}]", ret);
        return;
    }

    RTMP_EnableWrite(rtmp_);
}

void RTMPServer::impl::start() {
    std::thread thread([this]() {
        while (true) {
            int err = RTMP_Connect(rtmp_, nullptr);

            if (err <= 0) {
                LOG_ERROR("Error on RTMP_Connect [{}]", err);
                std::this_thread::sleep_for(std::chrono::microseconds(20));
                continue;
            }

            err = RTMP_ConnectStream(rtmp_, 0);
            
            if (err <= 0) {
                std::this_thread::sleep_for(std::chrono::microseconds(20));
                LOG_ERROR("Error on RTMP_ConnectStream [{}]", err);
                continue;
            }

            connected_ = true;
            break;
            //线程会退出
        }
    });

    thread.detach();
}

bool RTMPServer::impl::addFrame(unsigned char* data, size_t length) {
    if (!connected_) {
        return false;//未连接到RTMP接收方，不发送否则会报错
    }

    RTMPPacket packet;

    RTMPPacket_Reset(&packet);
    RTMPPacket_Alloc(&packet, (int)length);

    packet.m_packetType     = RTMP_PACKET_TYPE_VIDEO;
    packet.m_nBodySize      = length;
    packet.m_nTimeStamp     = timeStamp++;
    packet.m_nChannel       = 0x04;
    packet.m_headerType     = RTMP_PACKET_SIZE_LARGE;
    packet.m_nInfoField2    = rtmp_->m_stream_id;
    memcpy(packet.m_body, data, length);

    int err = RTMP_SendPacket(rtmp_, &packet, 1);

    if (err <= 0) {
        LOG_ERROR("Error on send RTMP packet [{}]", err);
        RTMPPacket_Free(&packet);
        return false;
    }

    RTMPPacket_Free(&packet);
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

void RTMPServer::init(int width, int height, int fps) {
    impl_->init(width, height, fps);
}