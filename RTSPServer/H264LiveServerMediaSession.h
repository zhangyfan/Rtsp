#pragma once
#include "LiveSource.h"
#include "OnDemandServerMediaSubsession.hh"
#include "liveMedia.hh"
#include <map>

class H264LiveServerMediaSession : public OnDemandServerMediaSubsession {
public:
    static H264LiveServerMediaSession *createNew(UsageEnvironment &env, bool reuseFirstSource);
    void checkForAuxSDPLine1();
    void afterPlayingDummy1();

    void addFrame(unsigned char *data, size_t length);

protected:
    H264LiveServerMediaSession(UsageEnvironment &env, bool reuseFirstSource);
    virtual ~H264LiveServerMediaSession(void);
    void setDoneFlag() { fDoneFlag = ~0; }

protected:
    virtual char const *getAuxSDPLine(RTPSink *rtpSink, FramedSource *inputSource);
    virtual FramedSource *createNewStreamSource(unsigned clientSessionId, unsigned &estBitrate);
    virtual RTPSink *createNewRTPSink(Groupsock *rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource *inputSource);

private:
    char *fAuxSDPLine;
    char fDoneFlag;
    RTPSink *fDummySink;
    LiveSource *frameSource_ = nullptr;
};