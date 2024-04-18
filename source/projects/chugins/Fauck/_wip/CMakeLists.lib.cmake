set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version" FORCE)

set(path "${CMAKE_CURRENT_SOURCE_DIR}")
cmake_path(GET path STEM PARENT_DIR)

# chugin name
set(CHUGIN_NAME ${PARENT_DIR})

set(THIRDPARTY ${CMAKE_SOURCE_DIR}/build/thirdparty/install)

# where the chuck headers are
set(CK_SRC_PATH "../chuck/include/")

set(FAUST_DIR "${CMAKE_BINARY_DIR}/thirdparty/faust")
set(LIBFAUST_DIR "${CMAKE_BINARY_DIR}/thirdparty/libfaust")
set(FAUST_LIBRARIES_DIR ${LIBFAUST_DIR}/share/faust)

include_directories(${FAUST_DIR}/architecture)
include_directories(${FAUST_DIR}/compiler)
include_directories(${FAUST_DIR}/compiler/utils)


set(PROJECT_SRC
    Faust.cpp
)


add_custom_target(fauck_deps
    # TARGET  ${CHUGIN_NAME} PRE_BUILD
    BYPRODUCTS  
        ${THIRDPARTY}/libfaust/lib/libfaustwithllvm.a
        ${THIRDPARTY}/install/lib/libsndfile.a
        ${THIRDPARTY}/faust/architecture
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_SOURCE_DIR}/source/scripts/install_fauck_deps.sh
)


add_library(${CHUGIN_NAME}
    ${PROJECT_SRC}
)

add_dependencies(${CHUGIN_NAME} fauck_deps)

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
    $<$<BOOL:${APPLE}>:__APPLE__>
    $<$<BOOL:${APPLE}>:__MACOSX_CORE__>
    $<$<BOOL:${APPLE}>:__PLATFORM_APPLE__>
    $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:__CK_DLL_STATIC__>
    _USRDLL
    FAUCK_EXPORTS
)

target_include_directories(${CHUGIN_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${THIRDPARTY}/include
    ${CK_SRC_PATH}
    ${FAUST_DIR}/architecture
    ${FAUST_DIR}/compiler
    ${FAUST_DIR}/compiler/utils
)

target_link_options(${CHUGIN_NAME}
    PUBLIC
    $<$<CONFIG:RELEASE>:-s>
    -shared
    -lc++
)

target_link_directories(${CHUGIN_NAME}
    PUBLIC
    ${LIBFAUST_DIR}/lib
    ${THIRDPARTY}/lib
)

target_link_libraries(${CHUGIN_NAME}
    PUBLIC
    ${LIBFAUST_DIR}/lib/libfaustwithllvm.a
    ${THIRDPARTY}/lib/libsndfile.a
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


