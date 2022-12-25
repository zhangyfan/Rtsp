add_definitions(-DCOMMON_LIBRARY)

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)

add_library(common SHARED ${SOURCES} ${HEADERS})
target_link_libraries(common spdlog)
target_include_directories(common
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)