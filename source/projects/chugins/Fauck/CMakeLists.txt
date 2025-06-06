set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version" FORCE)
set(BUILD_SHARED_LIBS ON)

set(path "${CMAKE_CURRENT_SOURCE_DIR}")
cmake_path(GET path STEM PARENT_DIR)

# chugin name
# set(CHUGIN_NAME ${PARENT_DIR})
set(CHUGIN_NAME "Faust")

set(THIRDPARTY ${CMAKE_SOURCE_DIR}/build/thirdparty)

# where the chuck headers are
set(CK_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../chuck/include)
set(CK_SOURCE ${CK_SRC_PATH}/chugin.h)

set(FAUST_DIR "${CMAKE_BINARY_DIR}/thirdparty/faust")
set(LIBFAUST_DIR "${CMAKE_BINARY_DIR}/thirdparty/libfaust")
set(FAUST_LIBRARIES_DIR ${LIBFAUST_DIR}/share/faust)


set(PROJECT_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Faust.cpp
    ${CK_SOURCE}
)

add_library(${CHUGIN_NAME}
    MODULE
    ${PROJECT_SRC}
)

# add_dependencies(${CHUGIN_NAME} fauck_deps)

set_target_properties(${CHUGIN_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${CHUGIN_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(BUILD_SHARED_LIBS)
    set_target_properties(${CHUGIN_NAME}
        PROPERTIES
        PREFIX ""
        SUFFIX ".chug"
    )
endif()

target_compile_options(${CHUGIN_NAME}
    PUBLIC
    -fPIC
    $<IF:$<CONFIG:Debug>,-g,-O3>
)

target_compile_definitions(${CHUGIN_NAME}
    PUBLIC
    NOBUNDLE
    $<$<PLATFORM_ID:Darwin>:__MACOSX_CORE__>
    $<$<PLATFORM_ID:Darwin>:__PLATFORM_APPLE__>
    $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:__CK_DLL_STATIC__>
    _USRDLL
    FAUCK_EXPORTS
)

target_include_directories(${CHUGIN_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${local_prefix}/include
    # ${THIRDPARTY}/install/include
    ${CK_SRC_PATH}
    ${FAUST_DIR}/architecture
    ${FAUST_DIR}/compiler
    ${FAUST_DIR}/compiler/utils
)

target_link_options(${CHUGIN_NAME}
    PUBLIC
    $<$<CONFIG:RELEASE>:-s>
    # -shared
)


target_link_directories(${CHUGIN_NAME}
    PUBLIC
    ${LIBFAUST_DIR}/lib
    # ${THIRDPARTY}/install/lib
    ${local_prefix}/lib
)

target_link_libraries(${CHUGIN_NAME}
    PUBLIC
    ${LIBFAUST_DIR}/lib/libfaustwithllvm.a
    $<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libFLAC.a>
    $<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libogg.a>
    $<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libvorbis.a>
    $<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libvorbisenc.a>
    $<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libvorbisfile.a>
    $<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libopus.a>
    $<$<BOOL:${ENABLE_EXTRA_FORMATS}>:${local_prefix}/lib/libmpg123.a>
    $<$<BOOL:${ENABLE_MP3}>:${local_prefix}/lib/libmp3lame.a>    
    ${local_prefix}/lib/libsndfile.a
    "-framework CoreFoundation"
    "-framework CoreMIDI"
    "-framework CoreAudio"
)


# set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/examples")
install(
    TARGETS ${CHUGIN_NAME}
    # LIBRARY DESTINATION chugins
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/examples/chugins
)

# set (DST ${CMAKE_SOURCE_DIR}/externals/chuck~.mxo/Contents/Resources/chugins)
set (DST ${CMAKE_SOURCE_DIR}/examples/chugins)

install(
    CODE "execute_process (COMMAND codesign -vf -s - ${DST}/${CHUGIN_NAME}.chug)" 
)
