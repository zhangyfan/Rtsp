#include <thread>
#include <mutex>
#include <condition_variable>
#include "Encoder.h"
#include "QueuedPacket.h"
#include "YUVUtils.h"
extern "C" {
#include <libavformat/avformat.h>
}


static std::mutex mtx_;
static std::condition_variable cond_;

static void threadEntry(int width, int height) {
    Codec::Encoder *JPEGEncoder = Codec::Encoder::createNew("JPEG");

    JPEGEncoder->init(width, height, 25);
    for (;true;) {
        std::unique_lock<std::mutex> guard(mtx_);
        cond_.wait_for(guard, std::chrono::seconds(5));
        AVFrame *frame = getQueuedFrame();
        unsigned char *YUV = nullptr;
        size_t YUVSize = 0;

        std::tie(YUV, YUVSize) = toYUV420(frame);
        
        unsigned char *jpeg            = nullptr;
        size_t size                    = 0;
        
        JPEGEncoder->encode(YUV, YUVSize, jpeg, size);
        delete jpeg;

#ifdef _MSC_VER
        free(YUV);
#endif // _MSC_VER
        av_frame_unref(frame);
    }
}

void startSnap(int width, int height) {
    std::thread thread(threadEntry, width, height);
    thread.detach();
}

void stopSnap() {

}

void triggerSnap() {
    cond_.notify_all();
}