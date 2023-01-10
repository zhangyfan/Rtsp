/**
 * @file DummySink.h
 * @author 张一帆
 * @brief live555
 * @version 0.1
 * @date 2022-12-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _DUMMYSINK_H_
#define _DUMMYSINK_H_
#pragma once
#include <liveMedia/MediaSession.hh>
#include <liveMedia/MediaSink.hh>

class RTSPClient;
class DummySink : public MediaSink {
public:
    static DummySink *createNew(UsageEnvironment &env, MediaSubsession &subsession, RTSPClient *client);

private:
    DummySink(UsageEnvironment &env, MediaSubsession &subsession, RTSPClient *client);
    // called only by "createNew()"
    virtual ~DummySink();

    static void afterGettingFrame(
        void *clientData, unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime, unsigned durationInMicroseconds);
    void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
        struct timeval presentationTime, unsigned durationInMicroseconds);

private:
    // redefined virtual functions:
    virtual Boolean continuePlaying();

private:
    u_int8_t *fReceiveBuffer;
    RTSPClient *client_;
    MediaSubsession &fSubsession;
    char *fStreamId;
};
#endif //_DUMMYSINK_H_