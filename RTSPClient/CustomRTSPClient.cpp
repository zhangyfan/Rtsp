#include "CustomRTSPClient.h"

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

StreamClientState::StreamClientState()
    : iter(NULL), session(NULL), subsession(NULL), streamTimerTask(NULL), duration(0.0)
{

}

StreamClientState::~StreamClientState()
{
  delete iter;
  if (session != NULL) {
    // We also need to delete "session", and unschedule "streamTimerTask" (if set)
    UsageEnvironment& env = session->envir(); // alias

    env.taskScheduler().unscheduleDelayedTask(streamTimerTask);
    Medium::close(session);
  }
}

CustomRTSPClient* CustomRTSPClient::createNew(
    UsageEnvironment& env, char const* rtspURL, int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum) 
{
    return new CustomRTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum);
}

CustomRTSPClient::CustomRTSPClient(
  UsageEnvironment& env, char const* rtspURL, int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum)
    : RTSPClient(env,rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum, -1)
{
}

CustomRTSPClient::~CustomRTSPClient() 
{
}