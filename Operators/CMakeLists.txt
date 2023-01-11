add_definitions(-DOPERATORS_LIBRARY)

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)

if(WIN32)
include_directories("${3RD_DIRECTORY}/alg_reasoning")
else()
include_directories("${CMAKE_SYSROOT}/usr/include")
include_directories("${3RD_DIRECTORY}/alg_reasoning")
endif()

add_library(Operators SHARED ${SOURCES} ${HEADERS})
if(WIN32)
target_link_libraries(Operators common)
else()
target_link_libraries(Operators common)
endif()

set_target_properties(Operators PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}"
)