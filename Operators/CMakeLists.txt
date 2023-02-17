add_definitions(-DOPERATORS_LIBRARY)

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)

include_directories(
    "${CMAKE_SYSROOT}/usr/include"
    "${3RD_DIRECTORY}/alg_inc"
    "${3RD_DIRECTORY}/json"
)

add_library(Operators SHARED ${SOURCES} ${HEADERS})
if(UNIX)
target_link_libraries(Operators common rwalg_core rwalg_reasoning)
else()
target_link_libraries(Operators common)
endif()

set_target_properties(Operators PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}"
)