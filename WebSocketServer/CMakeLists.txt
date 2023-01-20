add_definitions(-DWEBSOCKETSERVER_LIBRARY)

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)

add_library(WebsocketServer SHARED ${SOURCES} ${HEADERS})
target_link_libraries(WebsocketServer common hv)
set_target_properties(WebsocketServer PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}"
)