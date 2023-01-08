#pragma once
#include <liveMedia/FramedSource.hh>
#include <mutex>
#include <queue>
#include <condition_variable>
#include "readerwriterqueue.h"

class LiveSource : public FramedSource {
public:
    static LiveSource *createNew(UsageEnvironment &env);
    static EventTriggerId eventTriggerId;
    
    static void addFrame(unsigned char *data, size_t length);
protected:
    LiveSource(UsageEnvironment &env);
    virtual ~LiveSource(void);

private:
    virtual void doGetNextFrame();
    static void deliverFrame0(void *clientData);
    void deliverFrame();

private:
    static unsigned referenceCount;
    timeval currentTime_;
};