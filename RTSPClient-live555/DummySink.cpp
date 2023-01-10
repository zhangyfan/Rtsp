#include "DummySink.h"
#include "CustomRTSPClient.h"
#include "RTSPClient.hh"

#define DUMMY_SINK_RECEIVE_BUFFER_SIZE 512 * 1024

DummySink *DummySink::createNew(UsageEnvironment &env, MediaSubsession &subsession, RTSPClient *client) {
    return new DummySink(env, subsession, client);
}

DummySink::DummySink(UsageEnvironment &env, MediaSubsession &subsession, RTSPClient *client)
    : MediaSink(env), fSubsession(subsession), client_(client) {
    fStreamId      = strDup(client->url());
    fReceiveBuffer = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE];
}

DummySink::~DummySink() {
    delete[] fReceiveBuffer;
    delete[] fStreamId;
}

void DummySink::afterGettingFrame(void *clientData, unsigned frameSize, unsigned numTruncatedBytes,
    struct timeval presentationTime, unsigned durationInMicroseconds) {
    DummySink *sink = (DummySink *)clientData;
    sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
}

void DummySink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
    struct timeval presentationTime, unsigned /*durationInMicroseconds*/) {
    CustomRTSPClient *customClient = (CustomRTSPClient *)client_;
    

    customClient->onFrame(fReceiveBuffer, frameSize, fStreamId);
    // Then continue, to request the next frame of data:
    continuePlaying();
}

Boolean DummySink::continuePlaying() {
    if (fSource == NULL)
        return False; // sanity check (should not happen)

    // Request the next frame of data from our input source.  "afterGettingFrame()" will get called later, when it arrives:
    fSource->getNextFrame(
        fReceiveBuffer,
        DUMMY_SINK_RECEIVE_BUFFER_SIZE,
        afterGettingFrame,
        this,
        onSourceClosure,
        this);
    return True;
}
