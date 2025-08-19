# Source files
set(SR_WEBCAM_SOURCES
    src/sr_webcam_internal.h
    src/sr_webcam.c
)

# Detect the system and add corresponding source files
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(SR_WEBCAM_SOURCES ${SR_WEBCAM_SOURCES} src/sr_webcam_win.cpp)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin") # aka macOS
    set(SR_WEBCAM_SOURCES ${SR_WEBCAM_SOURCES} src/sr_webcam_mac.m)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(SR_WEBCAM_SOURCES ${SR_WEBCAM_SOURCES} src/sr_webcam_lin.c)
else()
    message(FATAL_ERROR "sr_webcam library not supported on this platform")
endif()

# Create the static library
add_library(sr_webcam STATIC ${SR_WEBCAM_SOURCES})
target_include_directories(sr_webcam PUBLIC .)

# link libraries
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # On Windows, this should link against mfplat, mf, mfuuid, Mfreadwriteand Shlwapi
    target_link_libraries(sr_webcam PRIVATE mf mfplat mfuuid Mfreadwrite Shlwapi)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin") # aka macOS
    # On macOS, this should link against AVFoundation.framework, CoreMedia.framework and Accelerate.framework
    target_link_libraries(sr_webcam PRIVATE "-framework AVFoundation" "-framework CoreMedia" "-framework Accelerate")
    # Embed the Info.plist (which requests camera access rights) in the binary
    # Note: This section might be ignored when running with Xcode attached, you'll have to copy the 
    # Info.plist in the binary directory while debugging, but it will work when running the binary directly
    set_target_properties(sr_webcam PROPERTIES 
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # On Linux, this should link against pthread
    target_link_libraries(sr_webcam PRIVATE pthread)
endif()
