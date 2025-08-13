
set(CHUGIN_NAME WarpBuf)

add_chugin(
    NAME ${CHUGIN_NAME}
    CODESIGN
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/AbletonClipInfo.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/WarpBufChugin.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/WarpBufChugin.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/WarpBufChuginDLL.cpp
    OTHER_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/portable_endian/include/portable_endian.h
    COMPILE_DEFINITIONS
        $<$<PLATFORM_ID:Darwin>:__APPLE__>
        "HAVE_LIBSAMPLERATE"
        "HAVE_VDSP"
        "USE_PTHREADS"
    INCLUDE_DIRS
        $<$<PLATFORM_ID:Darwin>:${local_prefix}/include>
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/portable_endian/include
    LINK_OPTIONS
        $<$<CONFIG:RELEASE>:-s>
    LINK_DIRS
        $<$<PLATFORM_ID:Darwin>:${local_prefix}/lib>
    LINK_LIBS
        $<$<PLATFORM_ID:Darwin>:$<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libFLAC.a>>
        $<$<PLATFORM_ID:Darwin>:$<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libogg.a>>
        $<$<PLATFORM_ID:Darwin>:$<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libvorbis.a>>
        $<$<PLATFORM_ID:Darwin>:$<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libvorbisenc.a>>
        $<$<PLATFORM_ID:Darwin>:$<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libvorbisfile.a>>
        $<$<PLATFORM_ID:Darwin>:$<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libopus.a>>
        $<$<PLATFORM_ID:Darwin>:$<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libmpg123.a>>
        $<$<PLATFORM_ID:Darwin>:$<$<BOOL:${ENABLE_MP3}>:${local_prefix}/lib/libmp3lame.a>>
        $<$<PLATFORM_ID:Darwin>:${local_prefix}/lib/libsndfile.a>
        $<$<PLATFORM_ID:Darwin>:${local_prefix}/lib/librubberband.a>
        $<$<PLATFORM_ID:Darwin>:${local_prefix}/lib/libsamplerate.a>
        "$<$<PLATFORM_ID:Darwin>:-framework CoreFoundation>"
        "$<$<PLATFORM_ID:Darwin>:-framework CoreMIDI>"
        "$<$<PLATFORM_ID:Darwin>:-framework CoreAudio>"
        "$<$<PLATFORM_ID:Darwin>:-framework Accelerate>"
)

set_target_properties(${CHUGIN_NAME} PROPERTIES CXX_STANDARD 17)
