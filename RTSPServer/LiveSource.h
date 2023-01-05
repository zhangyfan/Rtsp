#pragma once
#include <liveMedia/FramedSource.hh>
#include <queue>
#include <mutex>
#include <condition_variable>

class LiveSource : public FramedSource {
public:
    static LiveSource *createNew(UsageEnvironment &env);
    static EventTriggerId eventTriggerId;
    
    void addFrame(unsigned char *data, size_t length);
protected:
    LiveSource(UsageEnvironment &env);
    virtual ~LiveSource(void);

private:
    virtual void doGetNextFrame();
    static void deliverFrame0(void *clientData);
    void deliverFrame();
    std::pair<unsigned char *, size_t> getFrame();

private:
    static unsigned referenceCount;
    timeval currentTime_;
    std::mutex mtx_;
    std::condition_variable cond_;
    std::queue<std::pair<unsigned char *, size_t>> queue_;
};