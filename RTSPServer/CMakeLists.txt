add_definitions(-DRTSPSERVER_LIBRARY)

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)

if(WIN32)
include_directories("${3RD_DIRECTORY}/live555/include/")
else()
#基于sysroot的路径
include_directories("${CMAKE_SYSROOT}/usr/include/groupsock")
include_directories("${CMAKE_SYSROOT}/usr/include/liveMedia")
include_directories("${CMAKE_SYSROOT}/usr/include/UsageEnvironment")
include_directories("${CMAKE_SYSROOT}/usr/include/BasicUsageEnvironment")
endif()

add_library(RTSPServer SHARED ${SOURCES} ${HEADERS})
if(WIN32)
target_link_libraries(RTSPServer common live555)
else()
target_link_libraries(RTSPServer common BasicUsageEnvironment liveMedia groupsock UsageEnvironment)
endif()
set_target_properties(RTSPServer PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}"
)