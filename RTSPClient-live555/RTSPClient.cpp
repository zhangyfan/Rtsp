#include "RTSPClient.h"
#include "CustomRTSPClient.h"
#include "DummySink.h"
#include "logger.h"
#include <UsageEnvironment/UsageEnvironment.hh>
#include <BasicUsageEnvironment/BasicUsageEnvironment.hh>
#include <memory>
#include <sstream>

#define REQUEST_STREAMING_OVER_TCP False

namespace RTSP {
void shutdownStream(RTSPClient *rtspClient, int exitCode = 1);
void setupNextSubsession(RTSPClient *rtspClient);

void subsessionAfterPlaying(void *clientData) // called when a stream's subsession (e.g., audio or video substream) ends
{
    MediaSubsession *subsession = (MediaSubsession *)clientData;
    RTSPClient *rtspClient      = (RTSPClient *)(subsession->miscPtr);

    // Begin by closing this subsession's stream:
    Medium::close(subsession->sink);
    subsession->sink = NULL;

    // Next, check whether *all* subsessions' streams have now been closed:
    MediaSession &session = subsession->parentSession();
    MediaSubsessionIterator iter(session);
    while ((subsession = iter.next()) != NULL) {
        if (subsession->sink != NULL)
            return; // this subsession is still active
    }

    // All subsessions' streams have now been closed, so shutdown the client:
    shutdownStream(rtspClient);
}

void subsessionByeHandler(void *clientData) {
    MediaSubsession *subsession = (MediaSubsession *)clientData;
    RTSPClient *rtspClient      = (RTSPClient *)subsession->miscPtr;
    UsageEnvironment &env       = rtspClient->envir(); // alias

    LOG_DEBUG("Received RTCP \"BYE\" on subsession");
    // Now act as if the subsession had closed:
    subsessionAfterPlaying(subsession);
}

void streamTimerHandler(void *clientData) {
    CustomRTSPClient *rtspClient = (CustomRTSPClient *)clientData;
    StreamClientState &scs    = rtspClient->scs; // alias

    scs.streamTimerTask       = NULL;

    // Shut down the stream:
    shutdownStream(rtspClient);
}

void continueAfterSETUP(RTSPClient *rtspClient, int resultCode, char *resultString) {
    do {
        UsageEnvironment &env  = rtspClient->envir();                   // alias
        StreamClientState &scs = ((CustomRTSPClient *)rtspClient)->scs; // alias

        if (resultCode != 0) {
            LOG_ERROR("Failed to set up the subsession: {}", resultString);
            break;
        }

        LOG_DEBUG("Set up the subsession (client port {})", scs.subsession->clientPortNum());

        // Having successfully setup the subsession, create a data sink for it, and call "startPlaying()" on it.
        // (This will prepare the data sink to receive data; the actual flow of data from the client won't start happening until later,
        // after we've sent a RTSP "PLAY" command.)
        scs.subsession->sink = DummySink::createNew(env, *scs.subsession, rtspClient);
        // perhaps use your own custom "MediaSink" subclass instead
        if (scs.subsession->sink == NULL) {
            LOG_ERROR("Failed to create a data sink for the subsession: {}", env.getResultMsg());
            break;
        }

        LOG_DEBUG("Created a data sink for the subsession");
        scs.subsession->miscPtr = rtspClient; // a hack to let subsession handler functions get the "RTSPClient" from the subsession
        scs.subsession->sink->startPlaying(*(scs.subsession->readSource()), subsessionAfterPlaying, scs.subsession);

        // Also set a handler to be called if a RTCP "BYE" arrives for this subsession:
        if (scs.subsession->rtcpInstance() != NULL) {
            scs.subsession->rtcpInstance()->setByeHandler(subsessionByeHandler, scs.subsession);
        }
    } while (0);
    delete[] resultString;

    // Set up the next subsession, if any:
    setupNextSubsession(rtspClient);
}

void continueAfterPLAY(RTSPClient *rtspClient, int resultCode, char *resultString) {
    Boolean success = False;

    do {
        UsageEnvironment &env  = rtspClient->envir();                   // alias
        StreamClientState &scs = ((CustomRTSPClient *)rtspClient)->scs; // alias

        if (resultCode != 0) {
            LOG_ERROR("Failed to start playing session: {}", resultString);
            break;
        }

        // Set a timer to be handled at the end of the stream's expected duration (if the stream does not already signal its end
        // using a RTCP "BYE").  This is optional.  If, instead, you want to keep the stream active - e.g., so you can later
        // 'seek' back within it and do another RTSP "PLAY" - then you can omit this code.
        // (Alternatively, if you don't want to receive the entire stream, you could set this timer for some shorter value.)
        if (scs.duration > 0) {
            unsigned const delaySlop = 2; // number of seconds extra to delay, after the stream's expected duration.  (This is optional.)
            scs.duration += delaySlop;
            unsigned uSecsToDelay = (unsigned)(scs.duration * 1000000);
            scs.streamTimerTask   = env.taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc *)streamTimerHandler, rtspClient);
        }

        LOG_INFO("Started playing session");
        if (scs.duration > 0) {
            env << " (for up to " << scs.duration << " seconds)";
        }
        env << "...\n";

        success = True;
    } while (0);
    delete[] resultString;

    if (!success) {
        // An unrecoverable error occurred with this stream.
        shutdownStream(rtspClient);
    }
}

void setupNextSubsession(RTSPClient *rtspClient) {
    UsageEnvironment &env  = rtspClient->envir();                   // alias
    StreamClientState &scs = ((CustomRTSPClient *)rtspClient)->scs; // alias

    scs.subsession         = scs.iter->next();
    if (scs.subsession != NULL) {
        if (!scs.subsession->initiate()) {
            LOG_ERROR("Failed to initiate the subsession: {}", env.getResultMsg());
            setupNextSubsession(rtspClient); // give up on this subsession; go to the next one
        } else {
            LOG_DEBUG("initiate the subsession {} (client port {})", env.getResultMsg(), scs.subsession->clientPortNum());

            // Continue setting up this subsession, by sending a RTSP "SETUP" command:
            rtspClient->sendSetupCommand(*scs.subsession, continueAfterSETUP, False, REQUEST_STREAMING_OVER_TCP);
        }
        return;
    }

    // We've finished setting up all of the subsessions.  Now, send a RTSP "PLAY" command to start the streaming:
    if (scs.session->absStartTime() != NULL) {
        // Special case: The stream is indexed by 'absolute' time, so send an appropriate "PLAY" command:
        rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY, scs.session->absStartTime(), scs.session->absEndTime());
    } else {
        scs.duration = scs.session->playEndTime() - scs.session->playStartTime();
        rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY);
    }
}

void shutdownStream(RTSPClient *rtspClient, int exitCode) {
    UsageEnvironment &env  = rtspClient->envir();                   // alias
    StreamClientState &scs = ((CustomRTSPClient *)rtspClient)->scs; // alias

    // First, check whether any subsessions have still to be closed:
    if (scs.session != NULL) {
        Boolean someSubsessionsWereActive = False;
        MediaSubsessionIterator iter(*scs.session);
        MediaSubsession *subsession;

        while ((subsession = iter.next()) != NULL) {
            if (subsession->sink != NULL) {
                Medium::close(subsession->sink);
                subsession->sink = NULL;

                if (subsession->rtcpInstance() != NULL) {
                    subsession->rtcpInstance()->setByeHandler(NULL, NULL); // in case the server sends a RTCP "BYE" while handling "TEARDOWN"
                }

                someSubsessionsWereActive = True;
            }
        }

        if (someSubsessionsWereActive) {
            // Send a RTSP "TEARDOWN" command, to tell the server to shutdown the stream.
            // Don't bother handling the response to the "TEARDOWN".
            rtspClient->sendTeardownCommand(*scs.session, NULL);
        }
    }

    Medium::close(rtspClient);
}

// 获得SDP
void continueAfterDESCRIBE(RTSPClient *rtspClient, int resultCode, char *resultString) {
    UsageEnvironment &env  = rtspClient->envir();
    StreamClientState &scs = ((CustomRTSPClient *)rtspClient)->scs;
    std::unique_ptr<char> autoResult(resultString);

    if (resultCode != 0) {
        LOG_ERROR("Failed to get a SDP description: {}", resultString);
        shutdownStream(rtspClient);
        return;
    } else {
        LOG_DEBUG("Got a SDP description:\n {}", resultString);
    }

    // 创建媒体会话
    scs.session = MediaSession::createNew(env, resultString);

    if (scs.session == NULL) {
        LOG_ERROR("Failed to create a MediaSession object from the SDP description: {}", env.getResultMsg());
        shutdownStream(rtspClient);
        return;
    } else if (!scs.session->hasSubsessions()) {
        LOG_DEBUG("This session has no media subsessions (i.e., no \"m=\" lines)\n");
        shutdownStream(rtspClient);
        return;
    }

    scs.iter = new MediaSubsessionIterator(*scs.session);
    setupNextSubsession(rtspClient);
}

class ProxyRTSPClient::impl {
public:
    impl();
    ~impl();

    bool open(const std::string &addr, int port, const std::string &path, const std::string &user, const std::string &passwd);
    bool close();
    void setFrameCallback(const std::function<void(unsigned char *, size_t)> &callback);
    void run();
    int getVideoWidth();
    int getVideoHeight();

private:
    std::string makeURL(const std::string &addr, int port, const std::string &path, const std::string &user, const std::string &passwd);

private:
    std::function<void(unsigned char *, size_t)> m_onFrame;
    TaskScheduler *scheduler_ = nullptr;
    UsageEnvironment *env_ = nullptr;
    CustomRTSPClient *client_ = nullptr;
    char eventLoopWatchVariable_ = 0;
    std::mutex mtx_;
};

ProxyRTSPClient::impl::impl() {
    scheduler_ = BasicTaskScheduler::createNew();
    env_       = BasicUsageEnvironment::createNew(*scheduler_);
}

ProxyRTSPClient::impl::~impl() {
}

std::string ProxyRTSPClient::impl::makeURL(const std::string &addr, int port, const std::string &path, const std::string &user, const std::string &passwd) {
    // rtsp://[username[:password]@]ip_address[:rtsp_port]/path
    std::stringstream ss;

    ss << "rtsp://";

    if (!user.empty() && !passwd.empty()) {
        ss << user << ":" << passwd << "@";
    }

    ss << addr << ":" << port << "/" << path;
    return ss.str();
}

bool ProxyRTSPClient::impl::open(const std::string &addr, int port, const std::string &path, const std::string &user, const std::string &passwd) {
    std::string url        = makeURL(addr, port, path, user, passwd);

    std::lock_guard<std::mutex> guard(mtx_);
    client_ = CustomRTSPClient::createNew(*env_, url.c_str(), 1, "RTSPProxy");

    if (!client_) {
        LOG_ERROR("Fail to open rtsp url [{}], error [{}] ", url, env_->getResultMsg());
        return false;
    }

    client_->sendDescribeCommand(continueAfterDESCRIBE);
    return true;
}

bool ProxyRTSPClient::impl::close()
{
    std::lock_guard<std::mutex> guard(mtx_);

    shutdownStream(client_);
    client_ = nullptr;
    return true;
}

void ProxyRTSPClient::impl::setFrameCallback(const std::function<void(unsigned char *, size_t)> &callback)
{
    client_->setFrameCallback(callback);
}

void ProxyRTSPClient::impl::run() 
{
    env_->taskScheduler().doEventLoop(&eventLoopWatchVariable_);
}

int ProxyRTSPClient::impl::getVideoWidth() {
    return client_->getVideoWidth();
}

int ProxyRTSPClient::impl::getVideoHeight() {
    return client_->getVideoHeight();
}
//---------------------------------------------------------------------------------------------------------
ProxyRTSPClient::ProxyRTSPClient() {
    m_impl = new impl();
}

ProxyRTSPClient::~ProxyRTSPClient() {
    if (m_impl) {
        delete m_impl;
    }
}

bool ProxyRTSPClient::open(const std::string &addr, int port, const std::string &path, const std::string &user, const std::string &passwd) {
    return m_impl->open(addr, port, path, user, passwd);
}

bool ProxyRTSPClient::close() {
    return m_impl->close();
}

void ProxyRTSPClient::setFrameCallback(const std::function<void(unsigned char *, size_t)> &callback) {
    m_impl->setFrameCallback(callback);
}

void ProxyRTSPClient::run() {
    m_impl->run();
}

int ProxyRTSPClient::getVideoWidth() {
    return m_impl->getVideoWidth();
}

int ProxyRTSPClient::getVideoHeight() {
    return m_impl->getVideoHeight();
}
} // namespace RTSP