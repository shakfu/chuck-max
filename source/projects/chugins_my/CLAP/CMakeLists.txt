# CLAP Chugin CMakeLists.txt

set(MAX_SDK_BASE "${CMAKE_SOURCE_DIR}/source/max-sdk-base")
set(MAX_SDK_INCLUDES "${MAX_SDK_BASE}/c74support/max-includes")
file (STRINGS "${MAX_SDK_BASE}/script/max-linker-flags.txt" C74_SYM_MAX_LINKER_FLAGS)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${C74_SYM_MAX_LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${C74_SYM_MAX_LINKER_FLAGS}")

add_chugin(
    INCLUDE_DIRS
        ${MAX_SDK_INCLUDES}
        ${CMAKE_CURRENT_SOURCE_DIR}/clap-headers/include
    LINK_LIBS
        "$<$<PLATFORM_ID:Darwin>:-framework CoreFoundation>"
        "$<$<PLATFORM_ID:Darwin>:-framework CoreMIDI>"
        "$<$<PLATFORM_ID:Darwin>:-framework AudioToolbox>" 
)
