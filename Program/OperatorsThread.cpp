#include <thread>
#include <string>
#include <atomic>
#include "logger.h"
#include "Operators.h"
#include "YUVUtils.h"
#include "logger.h"
#include "Result.h"
#include "WSServer.h"
#include "json.hpp"
extern "C" {
#include <libavformat/avformat.h>
}

static std::atomic_bool flag = false;
static std::mutex mtx_;
static std::condition_variable cond_;
static std::shared_ptr<AVFrame> frame_;
static uint64_t pts_;
static std::string camId_;
static Websocket::WSServer *wsServer_;

static std::shared_ptr<AVFrame> getQueuedFrame() {
    std::unique_lock<std::mutex> guard(mtx_);

    if (!frame_) {
        cond_.wait(guard, [] { return (bool)frame_; });
    }

    auto ret = frame_;

    frame_.reset();
    return ret;
}

void wsSendJson(const std::vector<Result> &results) {
    nlohmann::json root;

    root["camId"] = camId_;
    root["pts"]   = pts_;

    std::vector<nlohmann::json> datas;
    for (auto &res : results) {
        nlohmann::json data;

        data["label"]   = res.label;
        data["conf"]    = res.conf;
        data["rectLTX"] = res.lx;
        data["rectLTY"] = res.ly;
        data["rectRBX"] = res.rx;
        data["rectRBY"] = res.ry;

        datas.push_back(data);
    }

    root["datas"] = datas;

    std::string json = root.dump();
    wsServer_->asyncSend("/smart", (unsigned char *)json.data(), json.size());
}

//Windows下开启该线程会应该会立刻结束
static void operatorThreadEntry(std::string dir, std::string_view cameraId) {
    Operators *operators = new Operators();
    
    //为了避免linux下崩溃的问题特殊处理
    if (*dir.rbegin() == '/') {
        dir = dir.substr(0, dir.size() - 1);
    }
    
    if (!operators->init(dir, cameraId)) {
        LOG_ERROR("Operators init error, thread is exiting!");
        return;
    }

    while (flag) {
        std::shared_ptr<AVFrame> pFrame = getQueuedFrame();
        AVFrame *frame                  = pFrame.get();
        int width                       = MPP_ALIGN(frame->width, 16);
        int height                      = MPP_ALIGN(frame->height, 16);
        unsigned char *BGR888;
        size_t size;

        std::tie(BGR888, size) = toBGR888(frame);
        auto results = operators->detect(BGR888, width, height);

        if (!results.empty()) {
            wsSendJson(results);
        }
        free(BGR888);
    }
}

void startOperators(std::string dir, std::string_view cameraId, Websocket::WSServer *wsServer) {
    flag = true;
    camId_ = cameraId;
    wsServer_ = wsServer;

    std::thread thread(operatorThreadEntry, dir, cameraId);
    thread.detach();
}

void stopOperators() {
    flag = false;
}

void setOperatorsFrame(std::shared_ptr<AVFrame> pFrame, uint64_t pts) {
    std::unique_lock<std::mutex> guard(mtx_);

    frame_ = pFrame;
    pts_   = pts;
    cond_.notify_all();
}