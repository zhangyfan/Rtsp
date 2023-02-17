#include <string>
#include <list>
#include <mutex>
#include <thread>
#include <filesystem>
#include "logger.h"
extern "C" {
#include <libavformat/avformat.h>
}

typedef std::list<std::shared_ptr<AVPacket>> Queue;

static std::string camId_;
static Queue queue_;
static std::mutex mtx_;
static std::condition_variable cond_;
static std::atomic_bool flag_;
static int fps_;

//启动一个线程，启动后直接等待保存，避免反复开线程的代码
//同时避免再主线程保存文件导致的延迟
static void RecordThread() {
    int n = fps_ * 12; //只保存最近的10秒，即fps*10

    while (flag_) {
        //每秒保存一次
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (queue_.size() == 0) {
            continue;
        }

        std::unique_lock<std::mutex> guard(mtx_);
        Queue::iterator begin = queue_.begin();

        //队列中的帧大于10秒
        if (queue_.size() > n) {
            int skip = queue_.size() - n;
            std::advance(begin, skip);
        }

#ifdef _MSC_VER
        std::string dst   = fmt::format("{}.h264", camId_);
        std::string fName = fmt::format("{}.h264.tmp", camId_);
#else
        std::string dst   = fmt::format("/dev/shm/{}.h264", camId_);
        std::string fName = fmt::format("/dev/shm/{}.h264.tmp", camId_);
#endif

        //开始保存
        FILE *file = fopen(fName.c_str(), "wb");

        if (!file) {
            LOG_ERROR("Error on save record to {}", fName);
            break;
        }

        std::for_each(begin, queue_.end(), [file](std::shared_ptr<AVPacket> pkt) {
            uint8_t *data = pkt->data;
            size_t size   = pkt->size;

            fwrite(data, 1, size, file);
        });
        fclose(file);

        //删除10秒之前的帧，不要把这个操作提前，因为这样会导致迭代器失效
        queue_.erase(queue_.begin(), begin);

        //删除之前的文件，然后拷贝过去
        //因为会拷贝该文件，所以可能删除失败
        while (std::filesystem::exists(dst)) {
            if (std::filesystem::remove(dst)) {
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        std::filesystem::rename(fName, dst);
    }


}

void startRecord(std::string_view camId, int fps) {
    camId_ = camId;
    flag_  = true;
    fps_   = fps;

    std::thread thread(RecordThread);
    thread.detach();
}

void stopRecord() {
    flag_ = false;
}

void PushRecordPkt(AVPacket* packet) {
    auto pkt = av_packet_alloc();
    av_packet_ref(pkt, packet);

    //不加锁容易出现迭代器失效的情况
    std::unique_lock<std::mutex> guard(mtx_);
    queue_.push_back(std::shared_ptr<AVPacket>(pkt, [](AVPacket *pkt) {
        av_packet_free(&pkt);
    }));
}