add_definitions(-DRTMPSERVER_LIBRARY)

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)

if(WIN32)
else()
include_directories("${CMAKE_SYSROOT}/usr/include")
endif()

add_library(RTMPServer SHARED ${SOURCES} ${HEADERS})
if(WIN32)
target_link_libraries(RTMPServer common ffmpeg)
else()
target_link_libraries(RTMPServer common avcodec avformat avutil swscale avdevice)
endif()

set_target_properties(RTMPServer PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}"
)