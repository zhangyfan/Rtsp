add_definitions(-DNETWORKLISTENER_LIBRARY)

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)

include_directories("${3RD_DIRECTORY}/asio/")
add_library(Networklistener SHARED ${SOURCES} ${HEADERS})
target_link_libraries(Networklistener common)
set_target_properties(Networklistener PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}"
)