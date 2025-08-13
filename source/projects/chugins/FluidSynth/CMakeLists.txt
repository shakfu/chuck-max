set(THIRDPARTY ${CMAKE_SOURCE_DIR}/build/thirdparty)

add_chugin(
    CODESIGN
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/FluidSynth.cpp
    COMPILE_DEFINITIONS
        "USE_PTHREADS"
    INCLUDE_DIRS
        ${local_prefix}/include
    LINK_DIRS
        ${local_prefix}/lib
    LINK_LIBS
        $<$<PLATFORM_ID:Darwin>:-liconv>
        $<$<PLATFORM_ID:Darwin>:-lreadline>

        $<$<PLATFORM_ID:Darwin>:${local_prefix}/lib/libfluidsynth.a>
        $<$<PLATFORM_ID:Darwin>:${local_prefix}/lib/libportaudio.a>
        $<$<PLATFORM_ID:Darwin>:${local_prefix}/lib/libportaudiocpp.a>
        $<$<PLATFORM_ID:Darwin>:${local_prefix}/lib/libglib-2.0.a>
        $<$<PLATFORM_ID:Darwin>:${local_prefix}/lib/libgthread-2.0.a>
        $<$<PLATFORM_ID:Darwin>:${local_prefix}/lib/libintl.a>

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
        "$<$<PLATFORM_ID:Darwin>:-framework CoreServices>"
        "$<$<PLATFORM_ID:Darwin>:-framework CoreAudio>"
        "$<$<PLATFORM_ID:Darwin>:-framework AudioUnit>"
        "$<$<PLATFORM_ID:Darwin>:-framework AudioToolbox>"
        "$<$<PLATFORM_ID:Darwin>:-framework AppKit>"
        "$<$<PLATFORM_ID:Darwin>:-framework Accelerate>"       
)
