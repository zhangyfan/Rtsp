#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <gflags/gflags.h>
#include <spdlog/fmt/fmt.h>
#include "Encoder.h"
#include "YUVUtils.h"
#include "logger.h"
extern "C" {
#include <libavformat/avformat.h>
}

std::atomic_bool flag;
static std::function<void()> callback_;
static std::mutex mtx_;
static std::condition_variable cond_;
static std::condition_variable triggerCond_;
static std::shared_ptr<AVFrame> frame_;

DECLARE_string(id);

static std::shared_ptr<AVFrame> getQueuedFrame() {
    std::unique_lock<std::mutex> guard(mtx_);

    if (!frame_) {
        cond_.wait(guard, [] { return (bool)frame_; });
    }

    auto ret = frame_;

    frame_.reset();
    return ret;
}

static void threadEntry(int width, int height) {
    std::string strPath = fmt::format("/dev/shm/{}.jpg", FLAGS_id);
    
    for (; flag;) {
        {
            std::unique_lock<std::mutex> guard(mtx_);
            triggerCond_.wait(guard);
        }

        std::shared_ptr<AVFrame> pFrame = getQueuedFrame();
        AVFrame *frame                  = pFrame.get();

        YUV2JPG(frame, strPath);
        if (callback_) {
            callback_();
        }
    }
}

void startSnap(int width, int height, const std::function<void()> &callback) {
    //要先设置标记
    flag = true;

    //设置回调
    callback_ = callback;

    std::thread thread(threadEntry, width, height);
    thread.detach();
}

void stopSnap() {
    flag = false;
}

void triggerSnap() {
    triggerCond_.notify_all();
}

void setSnapFrame(std::shared_ptr<AVFrame> pFrame) {
    std::unique_lock<std::mutex> guard(mtx_);

    frame_ = pFrame;
    cond_.notify_all();
}
