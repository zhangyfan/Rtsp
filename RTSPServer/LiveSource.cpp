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

using FrameType = std::pair<unsigned char *, size_t>;
static std::queue<FrameType> g_queue;
std::mutex mtx_;
std::condition_variable cond_;

void addFrame(unsigned char *data, size_t length) {
    unsigned char *buffer = new unsigned char[length];
    memcpy(buffer, data, length);

    std::unique_lock<std::mutex> guard(mtx_);
    g_queue.push(std::make_pair(buffer, length));
    cond_.notify_all();
}

std::pair<unsigned char *, size_t> getFrame() {
    std::pair<unsigned char *, size_t> res;
    std::unique_lock<std::mutex> guard(mtx_);

    if (g_queue.empty()) {
        cond_.wait(guard, []() { return !g_queue.empty(); });
    }

    res = g_queue.front();
    g_queue.pop();
    return res;
}

LiveSource *LiveSource::createNew(UsageEnvironment &env) {
    return new LiveSource(env);
}

EventTriggerId LiveSource::eventTriggerId = 0;

unsigned LiveSource::referenceCount       = 0;

LiveSource::LiveSource(UsageEnvironment &env)
    : FramedSource(env) {
    if (referenceCount == 0) {

    }
    ++referenceCount;

    if (eventTriggerId == 0) {
        eventTriggerId = envir().taskScheduler().createEventTrigger(deliverFrame0);
    }
}

LiveSource::~LiveSource(void) {
    --referenceCount;
    envir().taskScheduler().deleteEventTrigger(eventTriggerId);
    eventTriggerId = 0;
}

void LiveSource::deliverFrame0(void *clientData) {
    ((LiveSource *)clientData)->deliverFrame();
}

void LiveSource::doGetNextFrame() {
    gettimeofday(&currentTime_, NULL);
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
        fNumTruncatedBytes = length - trancate - fMaxSize;
    } else {
        fFrameSize = length - trancate;
    }

    fPresentationTime = currentTime_;
    memmove(fTo, data + trancate, fFrameSize);
    FramedSource::afterGetting(this);
}

void LiveSource::addFrame(unsigned char *data, size_t length) {
    ::addFrame(data, length);
}