file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)

include_directories("${3RD_DIRECTORY}/json")

add_executable(RtspProxy ${SOURCES} ${HEADERS})
if(WIN32)
include_directories("${3RD_DIRECTORY}/osal/inc")
target_link_libraries(RtspProxy common RTSPClient RTSPServer RTMPServer Codec ffmpeg WebsocketServer gflags Shlwapi Operators)
else()
include_directories("${CMAKE_SYSROOT}/usr/include/rockchip" "${3RD_DIRECTORY}/alg_inc")

target_link_libraries(RtspProxy common RTSPClient RTSPServer RTMPServer Codec Operators avcodec avformat avutil swscale avdevice rockchip_mpp WebsocketServer gflags opencv2 rga rwalg_core rwalg_reasoning)
endif()