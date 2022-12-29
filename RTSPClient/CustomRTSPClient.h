/**
 * @file CustomRTSPClient.h
 * @author 张一帆
 * @brief 派生live555 RTSP客户端
 * @version 0.1
 * @date 2022-12-25
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _CUSTOMRTSPCLIENT_H_
#define _CUSTOMRTSPCLIENT_H_
#pragma once
#include <RTSPClient.hh>
#include <functional>

class StreamClientState {
public:
    StreamClientState();
    virtual ~StreamClientState();

public:
    MediaSubsessionIterator *iter;
    MediaSession *session;
    MediaSubsession *subsession;
    TaskToken streamTimerTask;
    double duration;
};

class CustomRTSPClient : public RTSPClient {
public:
    static CustomRTSPClient *createNew(
        UsageEnvironment &env,
        char const *rtspURL,
        int verbosityLevel                = 0,
        char const *applicationName       = NULL,
        portNumBits tunnelOverHTTPPortNum = 0);
        
    void setFrameCallback(const std::function<void (unsigned char *, size_t, const char *)> &cb);
    void onFrame(unsigned char *data, size_t length, const char *streamId);
protected:
    CustomRTSPClient(
        UsageEnvironment &env, char const *rtspURL, int verbosityLevel, char const *applicationName, portNumBits tunnelOverHTTPPortNum);
    virtual ~CustomRTSPClient();


public:
    StreamClientState scs;
    std::function<void (unsigned char *, size_t, const char *)> callback_;
};

#endif //_CUSTOMRTSPCLIENT_H_