file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)

include_directories("E:/source/boost_1_66_0")
include_directories("${3RD_DIRECTORY}")
include_directories("${3RD_DIRECTORY}/json")
link_directories("E:/source/boost_1_66_0/stage/lib")

add_executable(WSServer ${SOURCES} ${HEADERS})
target_link_libraries(WSServer ffmpeg)