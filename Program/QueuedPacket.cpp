#include <mutex>
#include <condition_variable>
extern "C" {
#include <libavformat/avformat.h>
}

static AVPacket *queuedPacket = nullptr;
static AVFrame *queuedFrame = nullptr;

static std::mutex PktMtx;
static std::condition_variable PktCond;


void setQueuedPacket(AVPacket *packet) {
    std::unique_lock<std::mutex> guard(PktMtx);

    if (queuedPacket) {
        av_packet_unref(queuedPacket);
    }

    queuedPacket = av_packet_alloc();
    av_packet_ref(queuedPacket, packet);

    PktCond.notify_all();
}

AVPacket *getQueuedPacket() {
    std::unique_lock<std::mutex> guard(PktMtx);

    if (!queuedPacket) {
        PktCond.wait(guard, [] { return (bool)queuedPacket; });
    }

    auto ret = av_packet_alloc();

    av_packet_ref(ret, queuedPacket);
    return ret;
}

static std::mutex FrmMtx;
static std::condition_variable FrmCond;

void setQueuedFrame(AVFrame *frame) {
    std::unique_lock<std::mutex> guard(FrmMtx);

    if (queuedFrame) {
        av_frame_unref(queuedFrame);
    }

    queuedFrame = av_frame_alloc();
    av_frame_ref(queuedFrame, frame);

    FrmCond.notify_all();
}

AVFrame *getQueuedFrame() {
    std::unique_lock<std::mutex> guard(FrmMtx);

    if (!queuedFrame) {
        FrmCond.wait(guard, [] { return (bool)queuedFrame; });
    }

    auto ret = av_frame_alloc();

    av_frame_ref(ret, queuedFrame);
    return ret;
}