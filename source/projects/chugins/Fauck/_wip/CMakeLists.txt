cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

set(ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0)

if (DEFINED ENV{FAUCK_VERSION})
set (VERSION $ENV{FAUCK_VERSION})
else()
set (VERSION "0.0.1")
endif()

project(FaucK VERSION ${VERSION})

# Collect ChucK sources
FILE(GLOB CK_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cheaders/include/*.h)
source_group("ChucK" FILES ${CK_SOURCES})

# Collect FaucK sources
set(Sources
    "${PROJECT_SOURCE_DIR}/Faust.cpp"
)
source_group("Sources" FILES ${Sources})

# Combine all sources
set(ALL_FILES
    ${CK_SOURCES}
    ${Sources}
)

#######################################
# faust libraries

# Include directories for libfaust
if (NOT DEFINED FAUST_DIR)
    set(FAUST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/faust")
endif()
include_directories(${FAUST_DIR}/architecture)
include_directories(${FAUST_DIR}/compiler)
include_directories(${FAUST_DIR}/compiler/utils)

if (NOT DEFINED LIBFAUST_DIR)
    message( FATAL_ERROR "LIBFAUST_DIR must be set on macOS." )
endif()

set(FAUST_LIBRARIES_DIR ${LIBFAUST_DIR}/share/faust)

# Create shared library
add_library(FaucK MODULE ${ALL_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

####################################
# Install - used for packaging
####################################
set (PACKDIR ${ROOT}/package)
set (DST fauck-${VERSION})
install ( TARGETS FaucK
    RUNTIME DESTINATION ${PACKDIR}/${DST}
    LIBRARY DESTINATION ${PACKDIR}/${DST}
    ARCHIVE DESTINATION ${PACKDIR}/${DST}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

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
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist
)
install(
    DIRECTORY ${FAUST_LIBRARIES_DIR}
    DESTINATION ${PACKDIR}/${DST}/Faust.chug/Contents/Resources
    PATTERN "*.lproj" EXCLUDE
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)


# Find sndfile and link it
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libsndfile/install")
find_package(PkgConfig REQUIRED)
# We expect the user to have used brew to install the dependencies
# for libsndfile, to have built libsndfile as a static lib,
# and for it to be accessible via `pkg-config --static libsndfile`.
set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
list(APPEND PKG_CONFIG_EXECUTABLE "--static")  # append temporarily
pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)
pkg_check_modules(FLAC REQUIRED IMPORTED_TARGET flac)
pkg_check_modules(VORBIS REQUIRED IMPORTED_TARGET vorbis)
pkg_check_modules(OGG REQUIRED IMPORTED_TARGET ogg)
pkg_check_modules(OPUS REQUIRED IMPORTED_TARGET opus)
pkg_check_modules(MPG123 REQUIRED IMPORTED_TARGET libmpg123)
target_link_libraries (${PROJECT_NAME} PRIVATE PkgConfig::SNDFILE PkgConfig::FLAC PkgConfig::VORBIS PkgConfig::OGG PkgConfig::OPUS PkgConfig::MPG123)


# Link libfaust based on platform
target_link_directories(${PROJECT_NAME} PRIVATE ${LIBFAUST_DIR}/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE libfaustwithllvm.a)

unset(FAUST_DIR CACHE)
unset(LIBFAUST_DIR CACHE)

# Platform-specific libraries and definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    __MACOSX_CORE__
    __APPLE__
    __PLATFORM_APPLE__
)
target_link_libraries(${PROJECT_NAME} PRIVATE "-framework CoreFoundation" "-framework CoreMIDI" "-framework CoreAudio")

# Debug and Release configurations
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "_USRDLL;"
    "FAUCK_EXPORTS"
)

# Include directories
target_include_directories(FaucK PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cheaders/include>
    $<INSTALL_INTERFACE:FaucK> )


set(SOURCE_DIR "${PACKDIR}/${DST}/Faust.chug")
set(DEST_DIR "$ENV{HOME}/.chuck/lib/Faust.chug")
install(CODE "
    message(\"Copying ${SOURCE_DIR} to ${DEST_DIR}\")
    file(MAKE_DIRECTORY ${DEST_DIR})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_directory \"${SOURCE_DIR}\" \"${DEST_DIR}\"
    )
")
