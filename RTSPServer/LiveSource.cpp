#include "LiveSource.h"
#include "logger.h"

#if defined(_WIN32)
#include <chrono>

int gettimeofday(struct timeval *tp, struct timezone *tzp) {
    namespace sc                 = std::chrono;
    sc::system_clock::duration d = sc::system_clock::now().time_since_epoch();
    sc::seconds s                = sc::duration_cast<sc::seconds>(d);
    tp->tv_sec                   = (long)s.count();
    tp->tv_usec                  = (long)sc::duration_cast<sc::microseconds>(d - s).count();

    return 0;
}
#else
#include <sys/time.h>
#endif // _WIN32


LiveSource *LiveSource::createNew(UsageEnvironment &env) {
    return new LiveSource(env);
}

LiveSource::LiveSource(UsageEnvironment &env)
    : FramedSource(env), frame_(nullptr, 0) {
    triggerId_ = envir().taskScheduler().createEventTrigger(LiveSource::deliverFrameStub);
}

LiveSource::~LiveSource(void) {
}

void LiveSource::doStopGettingFrames() {
    FramedSource::doStopGettingFrames();
}

void LiveSource::doGetNextFrame() {
    deliverFrame();
}

void LiveSource::deliverFrame() {
    if (!isCurrentlyAwaitingData()) {
        return;
    }

    unsigned char *data = nullptr;
    size_t length       = 0;

    std::tie(data, length) = getFrame();
    
    //去掉NAL
    int trancate = 0;

    if (length >= 4 && data[0] == 0 && data[1] == 0 && data[2] == 0 && data[3] == 1) {
        trancate = 4;
    } else if (length >= 3 && data[0] == 0 && data[1] == 0 && data[2] == 1) {
        trancate = 3;
    }

    if (length - trancate > fMaxSize) {
        fFrameSize         = fMaxSize;
        fNumTruncatedBytes = (unsigned int)length - trancate - fMaxSize;
    } else {
        fFrameSize = (unsigned int)length - trancate;
    }
    
    gettimeofday(&fPresentationTime, NULL);
    memmove(fTo, data + trancate, fFrameSize);

    FramedSource::afterGetting(this);

    //清理
    std::unique_lock<std::mutex> guard(mtx_);
}

void LiveSource::addFrame(unsigned char *data, size_t length) {
    unsigned char *buffer = new unsigned char[length];
    memcpy(buffer, data, length);

    std::unique_lock<std::mutex> guard(mtx_);

    if (frame_.first) {
        delete[] frame_.first;
    }

    frame_ = std::make_pair(buffer, length);

    cond_.notify_all();
    envir().taskScheduler().triggerEvent(triggerId_, this);
}

std::pair<unsigned char*, size_t> LiveSource::getFrame() {
    std::pair<unsigned char *, size_t> res;
    std::unique_lock<std::mutex> guard(mtx_);

    if (!frame_.first) {
        cond_.wait(guard, [this]() { return frame_.first; });
    }

    return frame_;
}

void LiveSource::deliverFrameStub(void *clientData) {
    ((LiveSource *)clientData)->deliverFrame();
};