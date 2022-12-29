add_definitions(-DCODEC_LIBRARY)

set(SOURCES "")
file(GLOB HEADERS *.h)

option(WITH_FFMPEG "build with ffmpeg to decode and encode" OFF)

if(WITH_FFMPEG)
list(APPEND SOURCES "Decoder_h264Windows.cpp")
include_directories("E:/tools/ffmpeg-N-100367-g8d19b3c4a5-win64-gpl-shared-vulkan/include")
link_directories("E:/tools/ffmpeg-N-100367-g8d19b3c4a5-win64-gpl-shared-vulkan/lib")
else()
list(APPEND SOURCES "Decoder_h264MPP.cpp")
endif()

list(APPEND SOURCES "Decoder.cpp")
add_library(Codec SHARED ${SOURCES} ${HEADERS})
if(WITH_FFMPEG)
target_link_libraries(Codec PRIVATE common avcodec avutil avformat)
else()
endif()
set_target_properties(Codec PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}"
)