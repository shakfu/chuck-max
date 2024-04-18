cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

set(ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0)

if (DEFINED ENV{FAUCK_VERSION})
set (VERSION $ENV{FAUCK_VERSION})
else()
set (VERSION "0.0.1")
endif()

project(FaucK VERSION ${VERSION})

# Set startup project for Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT FaucK)

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
    if(MSVC)
    set(LIBFAUST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libfaust/win64/Release")
    elseif(APPLE)
    message( FATAL_ERROR "LIBFAUST_DIR must be set on macOS." )
    else()
    set(LIBFAUST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libfaust/ubuntu-x86_64/Release")
    endif()
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

if(APPLE)
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
else()
set_target_properties (${PROJECT_NAME} PROPERTIES 
    SUFFIX ".chug"
    OUTPUT_NAME Faust
)
install(
    DIRECTORY ${FAUST_LIBRARIES_DIR}
    DESTINATION ${PACKDIR}/${DST}
    FILES_MATCHING PATTERN "*.*"
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
endif()

# Find sndfile and link it
if (MSVC)
    find_package(SndFile REQUIRED HINTS "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libsndfile-1.2.0-win64/cmake")
    target_link_libraries(${PROJECT_NAME} PRIVATE SndFile::sndfile)
else()
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libsndfile/install")
    find_package(PkgConfig REQUIRED)
    # We expect the user to have used brew to install the dependencies
    # for libsndfile, to have built libsndfile as a static lib,
    # and for it to be accessible via `pkg-config --static libsndfile`.
    if(APPLE)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
    list(APPEND PKG_CONFIG_EXECUTABLE "--static")  # append temporarily
    endif()
    pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)
    pkg_check_modules(FLAC REQUIRED IMPORTED_TARGET flac)
    pkg_check_modules(VORBIS REQUIRED IMPORTED_TARGET vorbis)
    pkg_check_modules(OGG REQUIRED IMPORTED_TARGET ogg)
    pkg_check_modules(OPUS REQUIRED IMPORTED_TARGET opus)
    pkg_check_modules(MPG123 REQUIRED IMPORTED_TARGET libmpg123)
    target_link_libraries (${PROJECT_NAME} PRIVATE PkgConfig::SNDFILE PkgConfig::FLAC PkgConfig::VORBIS PkgConfig::OGG PkgConfig::OPUS PkgConfig::MPG123)
endif()

# Link libfaust based on platform
target_link_directories(${PROJECT_NAME} PRIVATE ${LIBFAUST_DIR}/lib)
if(MSVC)
target_link_libraries(${PROJECT_NAME} PRIVATE libfaustwithllvm.lib)
else()
target_link_libraries(${PROJECT_NAME} PRIVATE libfaustwithllvm.a)
endif()

unset(FAUST_DIR CACHE)
unset(LIBFAUST_DIR CACHE)

# Platform-specific libraries and definitions
if(APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        __MACOSX_CORE__
        __APPLE__
        __PLATFORM_APPLE__
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework CoreFoundation" "-framework CoreMIDI" "-framework CoreAudio")
elseif(MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 winmm wsock32 dsound dinput8 dxguid)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        WIN32
        _WIN32
        _WINDOWS
        NOMINMAX
        _USRDLL
        __WINDOWS_DS__
        __PLATFORM_WINDOWS__
    )
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
        _GLIBCXX_USE_CXX11_ABI=0
        __PLATFORM_LINUX__
        )
endif()

# MSVC-specific options
if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Debug>:_DEBUG>$<$<CONFIG:Release>:NDEBUG>")
    target_link_options(${PROJECT_NAME} PRIVATE "/DEBUG;/SUBSYSTEM:WINDOWS;/INCREMENTAL:NO")
endif()

# Debug and Release configurations
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "_USRDLL;"
    "FAUCK_EXPORTS"
)

# Include directories
target_include_directories(FaucK PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cheaders/include>
    $<INSTALL_INTERFACE:FaucK> )

if (MSVC)
####################################
# install sndfile
install (
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libsndfile-1.2.0-win64/bin/sndfile.dll DESTINATION ${PACKDIR}/${DST}
)
endif()

# Custom install command to copy the directory
if(MSVC)
    set(SOURCE_FILE "${PACKDIR}/${DST}/Faust.chug")
    set(DEST_DIR $ENV{USERPROFILE}/Documents/ChucK/chugins)
    STRING(REGEX REPLACE "\\\\" "/" DEST_DIR ${DEST_DIR}) 
    install(CODE "
    message(\"Copying ${SOURCE_FILE} to ${DEST_DIR}\")
    file(MAKE_DIRECTORY ${DEST_DIR})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy \"${SOURCE_FILE}\" \"${DEST_DIR}\"
    )
    ")
    set(SOURCE_DIR "${PACKDIR}/${DST}/faust")
    set(DEST_DIR $ENV{USERPROFILE}/Documents/ChucK/chugins/faust)
    STRING(REGEX REPLACE "\\\\" "/" DEST_DIR ${DEST_DIR}) 
    install(CODE "
    message(\"Copying ${SOURCE_DIR} to ${DEST_DIR}\")
    file(MAKE_DIRECTORY ${DEST_DIR})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_directory \"${SOURCE_DIR}\" \"${DEST_DIR}\"
    )
    ")
else()
    set(SOURCE_DIR "${PACKDIR}/${DST}/Faust.chug")
    set(DEST_DIR "$ENV{HOME}/.chuck/lib/Faust.chug")
    install(CODE "
        message(\"Copying ${SOURCE_DIR} to ${DEST_DIR}\")
        file(MAKE_DIRECTORY ${DEST_DIR})
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy_directory \"${SOURCE_DIR}\" \"${DEST_DIR}\"
        )
    ")
endif()