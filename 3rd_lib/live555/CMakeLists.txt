if (WIN32)

add_library(BasicUsageEnvironment UNKNOWN IMPORTED GLOBAL)
set_target_properties(BasicUsageEnvironment PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include/BasicUsageEnvironment"
    IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/win32/lib/Debug/BasicUsageEnvironment.lib"
    IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/win32/lib/Release/BasicUsageEnvironment.lib"
)

add_library(groupsock UNKNOWN IMPORTED GLOBAL)
set_target_properties(groupsock PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include/groupsock"
    IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/win32/lib/Debug/groupsock.lib"
    IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/win32/lib/Release/groupsock.lib"
)

add_library(liveMedia UNKNOWN IMPORTED GLOBAL)
set_target_properties(liveMedia PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include/liveMedia"
    IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/win32/lib/Debug/liveMedia.lib"
    IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/win32/lib/Release/liveMedia.lib"
)

add_library(UsageEnvironment UNKNOWN IMPORTED GLOBAL)
set_target_properties(UsageEnvironment PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include/UsageEnvironment"
    IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/win32/lib/Debug/UsageEnvironment.lib"
    IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/win32/lib/Release/UsageEnvironment.lib"
)

add_library(live555 INTERFACE IMPORTED GLOBAL)
set_property(TARGET live555 PROPERTY
  INTERFACE_LINK_LIBRARIES BasicUsageEnvironment groupsock liveMedia UsageEnvironment)
endif()
