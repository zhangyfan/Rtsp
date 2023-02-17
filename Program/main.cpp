#include "logger.h"
#include "RTSPClient.h"
#include "RTMPServer.h"
#include "RTSPServer.h"
#include "Decoder.h"
#include "Encoder.h"
#include "WSServer.h"
#include "YUVUtils.h"
#include "OperatorsThread.h"
#include "SnapThread.h"
#include "Config.h"
#include "RecordThread.h"
#include <iostream>
#include <fstream>
#include <list>
#include <chrono>
#include <gflags/gflags.h>
extern "C" {
    #include <libavformat/avformat.h>
    #include "libswscale/swscale.h"
    #include "libavutil/imgutils.h"
}

DEFINE_string(id, "", "Set working camera id.");
DEFINE_bool(proxy, false, "Optional; Set this 'true' to make program pass through h264 frame.");
#ifdef _MSC_VER
DEFINE_string(directory, ".\\",
    "Optional; Set camera configuration directory, default directory is working diretory.");
#else
DEFINE_string(directory, "/tmp/jobs/",
    "Optional; Set camera configuration directory, default directory is '/tmp/jobs/'.");
#endif // _MSC_VER

inline uint64_t GetMilliSecond() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void connectCamera(RTSP::ProxyRTSPClient *client, std::string_view url) {
    while (true) {
        if (client->open(std::string(url), 0, "")) {
            break;
        }
        LOG_ERROR("Can not connect to {}, retry after 5 seconds", url);
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

int main(int argc, char *argv[]) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    std::cout << gflags::CommandlineFlagsIntoString() << std::endl;

    if (FLAGS_id.empty()) {
        gflags::ShowUsageWithFlags(argv[0]);
        return 0;
    }

    Common::InitLogger(FLAGS_id);

    if (!loadConfig(FLAGS_id)) {
        LOG_ERROR("Can not load camera config file {}", FLAGS_id);
        return 0;
    }
    
    std::atomic_bool state   = false;
    auto pConfig = getConfig();

    // Websocket服务
    auto WSServer = new Websocket::WSServer();
    WSServer->init(pConfig->websocketPort);

    WSServer->setMessageCallback("/snap", [](const std::string &msg) {
        triggerSnap();
    });
    WSServer->setMessageCallback("/state", [WSServer, &state](const std::string &) {
        WSServer->asyncSend("/state", (unsigned char *)(state ? "1" : "0"), 1);
    });

    //开始连接摄像头
    RTSP::ProxyRTSPClient *client = new RTSP::ProxyRTSPClient();
    
    connectCamera(client, pConfig->inputURL);

    state = true;

    // RTSP服务
    auto rtspServer = new RTSP::RTSPServer();

    rtspServer->start(pConfig->rtspPort, pConfig->outputPath);

    int width  = client->getVideoWidth();
    int height = client->getVideoHeight();
    double fps = client->getFps();

    if (!FLAGS_proxy) {
         startOperators(FLAGS_directory, FLAGS_id, WSServer);
         startSnap(width, height, [WSServer]() {
             //出发截图后，发message到snap
             WSServer->asyncSend("/snap", (unsigned char *)"1", 1);
         });   
        //录像线程
        startRecord(FLAGS_id, (int)std::round(fps));
    }

    LOG_INFO("Stream open success");

    WSServer->setMessageCallback("/meta", [WSServer, width, height, fps](const std::string &) {
        std::string json;

        json = fmt::format("{{\"width\":{}, \"height\":{}, \"fps\":{}}}", width, height, fps);
        WSServer->asyncSend("/meta", (unsigned char *)json.data(), json.size());
    });

    Codec::Decoder *decoder = Codec::Decoder::createNew("H264");
 
    client->setErrorCallback([client, pConfig, &state]() {
        LOG_ERROR("{} is disconnected, trying to reconnect!", pConfig->inputURL);

        state = false;
        client->close();
        connectCamera(client, pConfig->inputURL);
        state = true;
    });

    client->setFrameCallback([rtspServer, WSServer, decoder](AVPacket *packet) {
        uint8_t *data = packet->data;
        size_t size   = packet->size;
        int64_t pts   = GetMilliSecond();
        
         if (rtspServer) {
             rtspServer->addFrame(data, size);
         }
         if (WSServer) {
             unsigned char *buffer = new unsigned char[size + 8];

             *((int64_t *)buffer)  = pts;
             memcpy(buffer + 8, data, size);
             WSServer->asyncSend("/h264", buffer, size + 8);
             delete buffer;
         }
        //非转发流
        if (!FLAGS_proxy) {
            auto decodedFrames = decoder->decode(packet);

             if (!decodedFrames.empty()) {
                 AVFrame *lastFrame = *decodedFrames.rbegin();

                 //释放其他帧
                 for (auto avFrame : decodedFrames) {
                     if (avFrame != lastFrame) {
                         av_frame_free(&avFrame);
                     }
                 }

                 //将最后一针放入处理线程
                 std::shared_ptr<AVFrame> pFrame(lastFrame, [](AVFrame *frame) {
                     av_frame_free(&frame);
                 });

                 setSnapFrame(pFrame);
                 setOperatorsFrame(pFrame, pts);
             }

             PushRecordPkt(packet);
        }
    });

    //有可能会断连，之后会自动重试，run会返回之后重新跑
    while (true) {
        client->run();
    }
    LOG_INFO("System exiting ......");
}