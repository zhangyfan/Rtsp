add_definitions(-DCOMMON_LIBRARY)

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)

add_library(common SHARED ${SOURCES} ${HEADERS})
if(WIN32)
target_link_libraries(common spdlog)
else()
target_link_libraries(common spdlog pthread stdc++fs)
endif()
target_include_directories(common
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)