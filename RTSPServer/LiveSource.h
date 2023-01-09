#pragma once
#include <liveMedia/FramedSource.hh>
#include <mutex>
#include <queue>
#include <condition_variable>
#include "readerwriterqueue.h"

class LiveSource : public FramedSource {
public:
    static LiveSource *createNew(UsageEnvironment &env);
    static void deliverFrameStub(void *clientData);

    LiveSource(UsageEnvironment &env);
    virtual ~LiveSource(void);

    void addFrame(unsigned char *data, size_t length);

private:
    virtual void doGetNextFrame();
    virtual void doStopGettingFrames();
    void deliverFrame();
    std::pair<unsigned char *, size_t> getFrame();

private:
    using FrameType = std::pair<unsigned char *, size_t>;
    
    std::queue<FrameType> queue_;
    std::mutex mtx_;
    std::condition_variable cond_;
    EventTriggerId triggerId_;
};