set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version" FORCE)

set(path "${CMAKE_CURRENT_SOURCE_DIR}")
cmake_path(GET path STEM PARENT_DIR)

# chugin name
set(CHUGIN_NAME ${PARENT_DIR})

set(THIRDPARTY ${CMAKE_SOURCE_DIR}/build/thirdparty/install)

# all of the c/cpp files that compose this chugin
file(GLOB PROJECT_SRC
    src/AbletonClipInfo.h
    src/WarpBufChugin.h
    src/WarpBufChugin.cpp
    src/WarpBufChuginDLL.cpp
)

# where the chuck headers are
set(CK_SRC_PATH
    "../chuck/include/"
)

add_custom_target(warpbuf_deps
    # TARGET  ${CHUGIN_NAME} PRE_BUILD
    BYPRODUCTS  
        ${THIRDPARTY}/lib/librubberband.a
        ${THIRDPARTY}/lib/libsndfile.a
        ${THIRDPARTY}/lib/libsamplerate.a
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_SOURCE_DIR}/source/scripts/install_deps.sh
)

add_library(${CHUGIN_NAME}
    ${PROJECT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/portable_endian/include/portable_endian.h
)

add_dependencies(${CHUGIN_NAME} warpbuf_deps)

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
    __APPLE__
    $<$<BOOL:${APPLE}>:__MACOSX_CORE__>
    $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:__CK_DLL_STATIC__>
    HAVE_CONFIG_H
    HAVE_LIBSAMPLERATE
    HAVE_VDSP
    USE_PTHREADS
)

target_include_directories(${CHUGIN_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${THIRDPARTY}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/portable_endian/include
    ${CK_SRC_PATH}
)

target_link_options(${CHUGIN_NAME}
    PUBLIC
    -shared
    -lc++
)

target_link_directories(${CHUGIN_NAME}
    PUBLIC
    ${THIRDPARTY}/lib
)

target_link_libraries(${CHUGIN_NAME}
    PUBLIC
    ${THIRDPARTY}/lib/librubberband.a
    ${THIRDPARTY}/lib/libsndfile.a
    ${THIRDPARTY}/lib/libsamplerate.a
    "-framework CoreFoundation"
    "-framework CoreMIDI"
    "-framework CoreAudio"
    "-framework Accelerate"
)

# set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/examples")
install(
    TARGETS ${CHUGIN_NAME}
    LIBRARY DESTINATION chugins
    # LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/examples/chugins
)

set (DST ${CMAKE_SOURCE_DIR}/externals/chuck~.mxo/Contents/Resources/chugins)
# set (DST ${CMAKE_SOURCE_DIR}/examples/chugins)

install(
    CODE "execute_process (COMMAND codesign -vf -s - ${DST}/WarpBuf.chug)" 
)


