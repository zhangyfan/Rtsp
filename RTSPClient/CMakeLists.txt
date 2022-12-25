add_definitions(-DRTSPCLIENT_LIBRARY)

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)

add_library(RTSPClient SHARED ${SOURCES} ${HEADERS})
target_link_libraries(RTSPClient common live555)