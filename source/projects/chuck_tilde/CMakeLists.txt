include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-pretarget.cmake)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CHUCK_INCLUDE ${CMAKE_SOURCE_DIR}/source/projects/chuck/core)

include_directories( 
    "${MAX_SDK_INCLUDES}"
    "${MAX_SDK_MSP_INCLUDES}"
    "${MAX_SDK_JIT_INCLUDES}"
)

file(GLOB PROJECT_SRC
    "*.h"
    "*.c"
    "*.cpp"
    "*.hpp"
)

add_library( 
    ${PROJECT_NAME} 
    MODULE
    ${PROJECT_SRC}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CHUCK_INCLUDE}
)

if(MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY
    # link statically to the multi-threaded runtime
    MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    "$<$<PLATFORM_ID:Darwin>:-framework CoreAudio>"
    "$<$<PLATFORM_ID:Darwin>:-framework CoreMIDI>"
    "$<$<PLATFORM_ID:Darwin>:-framework CoreFoundation>"    
    "$<$<PLATFORM_ID:Darwin>:-framework IOKit>"
    "$<$<PLATFORM_ID:Darwin>:-framework Carbon>"
    "$<$<PLATFORM_ID:Darwin>:-framework AppKit>"
    "$<$<PLATFORM_ID:Darwin>:-framework Foundation>"
    "$<$<PLATFORM_ID:Darwin>:-F/System/Library/PrivateFrameworks>"
    "$<$<PLATFORM_ID:Darwin>:-weak_framework MultitouchSupport>"
    chuck_lib
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
    "$<$<BOOL:${CM_MULTIOS_CHUGINS}>:CM_MULTIOS_CHUGINS>"
    "$<$<BOOL:${CM_MACOS_BUNDLED_CHUGINS}>:CM_MACOS_BUNDLED_CHUGINS>"
)




add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_SOURCE_DIR}/source/scripts/postprocess.py ${CMAKE_SOURCE_DIR}
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-posttarget.cmake)
