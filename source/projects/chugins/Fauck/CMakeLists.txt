set(CHUGIN_NAME "Faust")
set(FAUST_DIR "${CMAKE_BINARY_DIR}/thirdparty/faust")
set(LIBFAUST_DIR "${CMAKE_BINARY_DIR}/thirdparty/libfaust")

add_chugin(
    NAME ${CHUGIN_NAME}
    CODESIGN
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Faust.cpp
    COMPILE_DEFINITIONS
        "NOBUNDLE"
        $<$<PLATFORM_ID:Darwin>:__MACOSX_CORE__>
        $<$<PLATFORM_ID:Darwin>:__PLATFORM_APPLE__>
        "_USRDLL"
        FAUCK_EXPORTS
    INCLUDE_DIRS
        ${local_prefix}/include
        ${FAUST_DIR}/architecture
        ${FAUST_DIR}/compiler
        ${FAUST_DIR}/compiler/utils
    LINK_OPTIONS
        $<$<CONFIG:RELEASE>:-s>
    LINK_DIRS
        ${local_prefix}/lib
        ${LIBFAUST_DIR}/lib
    LINK_LIBS
        $<$<PLATFORM_ID:Darwin>:${LIBFAUST_DIR}/lib/libfaustwithllvm.a>
        $<$<PLATFORM_ID:Darwin>:$<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libFLAC.a>>
        $<$<PLATFORM_ID:Darwin>:$<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libogg.a>>
        $<$<PLATFORM_ID:Darwin>:$<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libvorbis.a>>
        $<$<PLATFORM_ID:Darwin>:$<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libvorbisenc.a>>
        $<$<PLATFORM_ID:Darwin>:$<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libvorbisfile.a>>
        $<$<PLATFORM_ID:Darwin>:$<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libopus.a>>
        $<$<PLATFORM_ID:Darwin>:$<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libmpg123.a>>
        $<$<PLATFORM_ID:Darwin>:$<$<BOOL:${ENABLE_MP3}>:${local_prefix}/lib/libmp3lame.a>>
        $<$<PLATFORM_ID:Darwin>:${local_prefix}/lib/libsndfile.a>
        "$<$<PLATFORM_ID:Darwin>:-framework CoreFoundation>"
        "$<$<PLATFORM_ID:Darwin>:-framework CoreMIDI>"
        "$<$<PLATFORM_ID:Darwin>:-framework CoreAudio>"
)

set_target_properties(${CHUGIN_NAME} PROPERTIES CXX_STANDARD 17)
