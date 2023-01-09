#pragma once
#include "LiveSource.h"
#include "OnDemandServerMediaSubsession.hh"
#include "liveMedia.hh"
#include <map>
#include <StreamReplicator.hh>

class H264LiveServerMediaSession : public OnDemandServerMediaSubsession {
public:
    static H264LiveServerMediaSession *createNew(UsageEnvironment &env, StreamReplicator *replicator);

protected:
    H264LiveServerMediaSession(UsageEnvironment &env, StreamReplicator *replicator);
    virtual ~H264LiveServerMediaSession(void);

    
    virtual FramedSource *createNewStreamSource(unsigned clientSessionId, unsigned &estBitrate);
    virtual RTPSink *createNewRTPSink(Groupsock *rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource *inputSource);

private:
    StreamReplicator *m_replicator;
};