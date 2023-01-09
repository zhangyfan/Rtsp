#include "H264LiveServerMediaSession.h"
#include "logger.h"

H264LiveServerMediaSession *H264LiveServerMediaSession::createNew(UsageEnvironment &env, StreamReplicator *replicator)
{
    return new H264LiveServerMediaSession(env, replicator);
}

H264LiveServerMediaSession::H264LiveServerMediaSession(UsageEnvironment &env, StreamReplicator *replicator)
    : OnDemandServerMediaSubsession(env, False), m_replicator(replicator) {
}

H264LiveServerMediaSession::~H264LiveServerMediaSession(void) {
}

FramedSource *H264LiveServerMediaSession::createNewStreamSource(unsigned clientSessionId, unsigned &estBitrate) {
    FramedSource *source = m_replicator->createStreamReplica();
    return H264VideoStreamDiscreteFramer::createNew(envir(), source);
}

RTPSink *H264LiveServerMediaSession::createNewRTPSink(Groupsock *rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource *inputSource) {
    return H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
}
