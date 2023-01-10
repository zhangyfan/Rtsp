add_definitions(-DRTMPSERVER_LIBRARY)

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)

add_library(RTMPServer SHARED ${SOURCES} ${HEADERS})
if(WIN32)
target_link_libraries(RTMPServer common rtmp Winmm)
else()
target_link_libraries(RTMPServer common rtmp)
endif()

set_target_properties(RTMPServer PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}"
)