include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-pretarget.cmake)
set(CMAKE_VERBOSE_MAKEFILE ON)

#############################################################
# MAX EXTERNAL
#############################################################
# set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum OS X deployment version" FORCE)

add_subdirectory(core)
add_subdirectory(host)
add_subdirectory(host_embed)
add_subdirectory(demo)


set(CHUCK_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/core)

include_directories( 
    "${MAX_SDK_INCLUDES}"
    "${MAX_SDK_MSP_INCLUDES}"
    "${MAX_SDK_JIT_INCLUDES}"
    
)

file(GLOB PROJECT_SRC
    "*.h"
    "*.c"
    "*.cpp"
)

add_library( 
    ${PROJECT_NAME} 
    MODULE
    ${PROJECT_SRC}
)


target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CHUCK_INCLUDE}
)


target_link_libraries(${PROJECT_NAME}
    PUBLIC
    "-framework CoreAudio"
    "-framework CoreMIDI"
    "-framework CoreFoundation"
    "-framework IOKit"
    "-framework Carbon"
    "-framework AppKit"
    "-framework Foundation"
    "-F/System/Library/PrivateFrameworks"
    "-weak_framework MultitouchSupport"
    -lc++
    -lm
    chuck_lib
)



include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-posttarget.cmake)
