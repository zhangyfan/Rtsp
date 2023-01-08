#include "H264LiveServerMediaSession.h"
#include "logger.h"

H264LiveServerMediaSession *H264LiveServerMediaSession::createNew(UsageEnvironment &env, bool reuseFirstSource) {
    return new H264LiveServerMediaSession(env, reuseFirstSource);
}

H264LiveServerMediaSession::H264LiveServerMediaSession(UsageEnvironment &env, bool reuseFirstSource)
    : OnDemandServerMediaSubsession(env, reuseFirstSource), fAuxSDPLine(NULL), fDoneFlag(0), fDummySink(NULL) {
}

H264LiveServerMediaSession::~H264LiveServerMediaSession(void) {
    delete[] fAuxSDPLine;
}

static void afterPlayingDummy(void *clientData) {
    H264LiveServerMediaSession *session = (H264LiveServerMediaSession *)clientData;
    session->afterPlayingDummy1();
}

void H264LiveServerMediaSession::afterPlayingDummy1() {
    envir().taskScheduler().unscheduleDelayedTask(nextTask());
    setDoneFlag();
}

static void checkForAuxSDPLine(void *clientData) {
    H264LiveServerMediaSession *session = (H264LiveServerMediaSession *)clientData;
    session->checkForAuxSDPLine1();
}

void H264LiveServerMediaSession::checkForAuxSDPLine1() {
    char const *dasl;
    if (fAuxSDPLine != NULL) {
        setDoneFlag();
    } else if (fDummySink != NULL && (dasl = fDummySink->auxSDPLine()) != NULL) {
        fAuxSDPLine = strDup(dasl);
        fDummySink  = NULL;
        setDoneFlag();
    } else {
        int uSecsDelay = 100000;
        nextTask()     = envir().taskScheduler().scheduleDelayedTask(uSecsDelay, (TaskFunc *)checkForAuxSDPLine, this);
    }
}

char const *H264LiveServerMediaSession::getAuxSDPLine(RTPSink *rtpSink, FramedSource *inputSource) {
    if (fAuxSDPLine != NULL)
        return fAuxSDPLine;
    if (fDummySink == NULL) {
        fDummySink = rtpSink;
        fDummySink->startPlaying(*inputSource, afterPlayingDummy, this);
        checkForAuxSDPLine(this);
    }

    envir().taskScheduler().doEventLoop(&fDoneFlag);
    return fAuxSDPLine;
}

FramedSource *H264LiveServerMediaSession::createNewStreamSource(unsigned clientSessionID, unsigned &estBitRate) {
    estBitRate   = 90000;
    frameSource_ = LiveSource::createNew(envir());

    LOG_ERROR("source created {:x}", uint64_t(frameSource_));
    return H264VideoStreamDiscreteFramer::createNew(envir(), frameSource_);
}

RTPSink *H264LiveServerMediaSession::createNewRTPSink(Groupsock *rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource *inputSource) {
    return H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
}

void H264LiveServerMediaSession::addFrame(unsigned char* data, size_t length) {
    if (frameSource_) {
        frameSource_->addFrame(data,length);
    }
}