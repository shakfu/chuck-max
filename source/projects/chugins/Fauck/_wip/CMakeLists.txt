set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version" FORCE)

set(ROOT ${CMAKE_BINARY_DIR})

if (DEFINED ENV{FAUCK_VERSION})
    set (VERSION $ENV{FAUCK_VERSION})
else()
    set (VERSION "0.0.1")
endif()

project(FaucK VERSION ${VERSION})

set(PROJECT_NAME FaucK)

set(THIRDPARTY ${CMAKE_SOURCE_DIR}/build/thirdparty)

# where the chuck headers are
set(CK_SRC_PATH "../chuck/include/")
set(CK_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../chuck/include/chugin.h)


set(FAUST_DIR "${CMAKE_BINARY_DIR}/thirdparty/faust")
set(LIBFAUST_DIR "${CMAKE_BINARY_DIR}/thirdparty/libfaust")
set(FAUST_LIBRARIES_DIR ${LIBFAUST_DIR}/share/faust)

include_directories(${FAUST_DIR}/architecture)
include_directories(${FAUST_DIR}/compiler)
include_directories(${FAUST_DIR}/compiler/utils)

set(PROJECT_SRC
    Faust.cpp
    ${CK_SOURCES}
)

add_custom_target(fauck_deps
    # TARGET  ${CHUGIN_NAME} PRE_BUILD
    BYPRODUCTS  
        ${THIRDPARTY}/libfaust/lib/libfaustwithllvm.a
        ${THIRDPARTY}/install/lib/libsndfile.a
        ${FAUST_DIR}/architecture
        ${FAUST_DIR}/compiler
        ${FAUST_DIR}/compiler/utils
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_SOURCE_DIR}/source/scripts/install_fauck_deps.sh
)

add_library(${PROJECT_NAME}
    MODULE
    ${PROJECT_SRC}
)

add_dependencies(${PROJECT_NAME} fauck_deps)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

####################################
# Install - used for packaging
####################################

set (PACKDIR ${ROOT}/package)
set (DST fauck-${VERSION})
install ( TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${PACKDIR}/${DST}
    LIBRARY DESTINATION ${PACKDIR}/${DST}
    ARCHIVE DESTINATION ${PACKDIR}/${DST}
)

set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
add_compile_options(-fPIC)


set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS "-Wl,-U,_object_new_imp -Wl,-U,_object_method_imp"
    BUNDLE TRUE
    BUNDLE_EXTENSION chug
    OUTPUT_NAME Faust
    MACOSX_BUNDLE_GUI_IDENTIFIER edu.stanford.chuck.${PROJECT_NAME}
    MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
    MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION}
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
)

install(
    DIRECTORY ${FAUST_LIBRARIES_DIR}
    DESTINATION ${PACKDIR}/${DST}/Faust.chug/Contents/Resources
    PATTERN "*.lproj" EXCLUDE
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
    $<$<BOOL:${APPLE}>:__APPLE__>
    $<$<BOOL:${APPLE}>:__MACOSX_CORE__>
    $<$<BOOL:${APPLE}>:__PLATFORM_APPLE__>
    _USRDLL
    FAUCK_EXPORTS
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${THIRDPARTY}/install/include
    ${CK_SRC_PATH}
    ${FAUST_DIR}/architecture
    ${FAUST_DIR}/compiler
    ${FAUST_DIR}/compiler/utils
)

target_link_options(${PROJECT_NAME}
    PUBLIC
    $<$<CONFIG:RELEASE>:-s>
)

target_link_directories(${PROJECT_NAME}
    PUBLIC
    ${LIBFAUST_DIR}/lib
    ${THIRDPARTY}/install/lib
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    ${LIBFAUST_DIR}/lib/libfaustwithllvm.a
    ${THIRDPARTY}/install/lib/libsndfile.a
    "-framework CoreFoundation"
    "-framework CoreMIDI"
    "-framework CoreAudio"
)



# testing smaller 
# install(
#     DIRECTORY ${CMAKE_SOURCE_DIR}/examples/faust
#     DESTINATION ${PACKDIR}/${DST}/${CHUGIN_NAME}.chug/Contents/Resources
#     PATTERN "*.lproj" EXCLUDE
#     PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
# )

# install(
#     DIRECTORY ${FAUST_LIBRARIES_DIR}
#     DESTINATION ${PACKDIR}/${DST}/${CHUGIN_NAME}.chug/Contents/Resources
#     PATTERN "*.lproj" EXCLUDE
#     PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
# )

# filter recursive copy
# find . -name '*.lib' -or -name '*.dsp' -or -name '*.wav' | cpio -pdm ./dest


# Custom install command to copy the directory
set(SOURCE_DIR "${PACKDIR}/${DST}/Faust.chug")
set(DEST_DIR "${CMAKE_SOURCE_DIR}/examples/chugins/Faust.chug")
# set(DEST_DIR "$ENV{HOME}/.chuck/lib/Faust.chug")
install(CODE "
    message(\"Copying ${SOURCE_DIR} to ${DEST_DIR}\")
    file(MAKE_DIRECTORY ${DEST_DIR})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_directory \"${SOURCE_DIR}\" \"${DEST_DIR}\"
    )
")

