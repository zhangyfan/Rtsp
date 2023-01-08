add_definitions(-DCODEC_LIBRARY)

set(SOURCES "")
file(GLOB HEADERS *.h)

include_directories("${3RD_DIRECTORY}/osal/inc")

if(WIN32)
include_directories("${3RD_DIRECTORY}/mpp/inc")

list(APPEND SOURCES "Decoder_h264Windows.cpp" "Encoder_h264Windows.cpp")
else()
include_directories("${CMAKE_SYSROOT}/usr/include/rockchip")
list(APPEND SOURCES "Decoder_h264Windows.cpp" "Encoder_h264MPP.cpp")
endif()

list(APPEND SOURCES "Decoder.cpp" "Encoder.cpp")
add_library(Codec SHARED ${SOURCES} ${HEADERS})
if(WIN32)
target_link_libraries(Codec PRIVATE common ffmpeg)
else()
target_link_libraries(Codec PRIVATE common avcodec swscale)
endif()

set_target_properties(Codec PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}"
)