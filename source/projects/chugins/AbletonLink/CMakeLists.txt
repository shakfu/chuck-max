# AbletonLink Chugin CMakeLists.txt

# Include Ableton Link directory
set(LINK_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/link)

# Check if the Link library exists
if(NOT EXISTS ${LINK_ROOT}/include/ableton/Link.hpp)
    message(WARNING "Ableton Link headers not found at ${LINK_ROOT}/include. Skipping AbletonLink chugin.")
    return()
endif()

# Platform-specific configuration
if(APPLE)
    set(LINK_LIBRARIES "-framework CoreFoundation" "-framework CoreServices")
    set(PLATFORM_DEFS "LINK_PLATFORM_MACOSX=1")
elseif(WIN32)
    set(LINK_LIBRARIES "ws2_32" "winmm" "iphlpapi")
    set(PLATFORM_DEFS "LINK_PLATFORM_WINDOWS=1" "_WIN32_WINNT=0x0601")
else()
    set(LINK_LIBRARIES "pthread")
    set(PLATFORM_DEFS "LINK_PLATFORM_LINUX=1")
endif()

# Add the chugin target
add_chugin(
    SOURCES
        AbletonLink.cpp
        abl_link_instance.cpp
    INCLUDE_DIRS
        ${CMAKE_SOURCE_DIR}/source/projects/chuck/core
        ${LINK_ROOT}/include
        ${LINK_ROOT}/modules/asio-standalone/asio/include
    COMPILE_DEFINITIONS
        ASIO_STANDALONE
        ASIO_HEADER_ONLY
        ${PLATFORM_DEFS}
    LINK_LIBS
        ${LINK_LIBRARIES}
    COMPILE_OPTIONS
        -w
)
