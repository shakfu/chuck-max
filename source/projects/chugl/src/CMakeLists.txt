include(FetchContent)

# cmake minimum version
cmake_minimum_required(VERSION 3.16)

# set(CMAKE_VERBOSE_MAKEFILE ON)

# force universal binary on osx (must go BEFORE project call)
# set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15 CACHE STRING "macOS Deployment Target")

project(
    ChuGL
    VERSION 0.1.0
    LANGUAGES CXX C
)

if(LINUX OR EMSCRIPTEN)
    add_compile_options(-fPIC)  # needed for emcripten
    #set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined" )
endif()


set(
    CORE 
    core/log.c
    core/hashmap.c
    core/memory.cpp
)

set(
    VENDOR
    implementations.cpp
    vendor/mikktspace/mikktspace.c

    # cimgui (no freetype )
    vendor/cimgui/cimgui.cpp
    vendor/cimgui/imgui/imgui.cpp
    vendor/cimgui/imgui/imgui_draw.cpp
    vendor/cimgui/imgui/imgui_demo.cpp
    vendor/cimgui/imgui/imgui_widgets.cpp
	vendor/cimgui/imgui/imgui_tables.cpp
    vendor/cimgui/imgui/misc/cpp/imgui_stdlib.cpp
    vendor/cimgui/imgui/backends/imgui_impl_glfw.cpp
    vendor/cimgui/imgui/backends/imgui_impl_wgpu.cpp
)

set(
    RENDERER_TESTS
    # tests/obj.cpp
    tests/gltf.cpp
    tests/imgui_test.cpp
    tests/test_box2d.cpp
    tests/test_lines2d.cpp
)

if (CHUGL_BUILD_RENDERER_TESTS)
    message(STATUS "Building Renderer Tester")
    add_executable(
        ChuGL-Renderer-Tester
        main.cpp
        ${CORE}
        ${VENDOR}
        ${RENDERER_TESTS}
    )

    # benchamrks TODO: put behind option flag, link with compile options 
    # add_executable(
    #     ChuGL-Benchmarks
    #     bench/hashmap.cpp
    #     ${CORE}
    # )

    set_target_properties(ChuGL-Renderer-Tester PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS OFF
        # COMPILE_WARNING_AS_ERROR ON
        VS_DEBUGGER_ENVIRONMENT "DAWN_DEBUG_BREAK_ON_ERROR=1"
    )

    # IDE setup
    if(XCODE)
        set_target_properties(ChuGL-Renderer-Tester PROPERTIES
            XCODE_GENERATE_SCHEME ON
            XCODE_SCHEME_ENABLE_GPU_FRAME_CAPTURE_MODE "Metal"
            XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        )
    endif()
endif()

# chugl library
if (EMSCRIPTEN)
    # emcmake can't handle shared libraries. SIDE_LOAD executable is the only option
    add_executable(
        ${PROJECT_NAME} 
        ChuGL.cpp 
        ${CORE}
        ${VENDOR}
    )
else()
    add_library(
        ${PROJECT_NAME} SHARED 
        ChuGL.cpp 
        ${CORE}
        ${VENDOR}
    )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17 
    CXX_EXTENSIONS OFF
    # COMPILE_WARNING_AS_ERROR ON
    VS_DEBUGGER_ENVIRONMENT "DAWN_DEBUG_BREAK_ON_ERROR=1"
    POSITION_INDEPENDENT_CODE ON
    PREFIX ""
    OUTPUT_NAME ${PROJECT_NAME}
    SUFFIX ".chug"
)


if(XCODE)

    # create entitlements file for camera access
    # file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/ChuGL.entitlements
    # "<?xml version=\"1.0\" encoding=\"UTF-8\"?>
    # <!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">
    # <plist version=\"1.0\">
    # <dict>
    #     <key>com.apple.security.device.camera</key>
    #     <true/>
    #     <key>com.apple.security.device.audio-input</key>
    #     <true/>
    # </dict>
    # </plist>
    # ")

    set_target_properties(${PROJECT_NAME} PROPERTIES
        XCODE_GENERATE_SCHEME ON
        # XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS ${CMAKE_CURRENT_BINARY_DIR}/ChuGL.entitlements
        XCODE_SCHEME_ENABLE_GPU_FRAME_CAPTURE_MODE "Metal"
        XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        XCODE_SCHEME_EXECUTABLE "/usr/local/bin/chuck"
        XCODE_SCHEME_ARGUMENTS "--chugin:./build-xcode/Debug/ChuGL.chug"
        # XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES
        # MACOSX_BUNDLE TRUE
        # MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/vendor/sr_webcam/Info.plist
    )
endif()

# =================================================================================
# interface library for CMake properties
# =================================================================================
add_library(chugl_shared_properties INTERFACE)

# Defines
target_compile_definitions(chugl_shared_properties INTERFACE 
    $<$<CONFIG:Debug>:CHUGL_DEBUG>
    $<$<CONFIG:Release>:CHUGL_RELEASE>
    GLM_FORCE_DEPTH_ZERO_TO_ONE # glm force depth range to 0-1
    GLM_ENABLE_EXPERIMENTAL
    LOG_USE_COLOR
)

if (CHUGL_FAST_COMPILE)
    target_compile_definitions(chugl_shared_properties INTERFACE CHUGL_FAST_COMPILE)
endif()

# increasing warning levels
if (MSVC)
    target_compile_options(chugl_shared_properties INTERFACE /W4)
else()
    target_compile_options(chugl_shared_properties INTERFACE -Wall -Wextra -pedantic)

    # disable warnings
    target_compile_options(chugl_shared_properties INTERFACE 
        -Wno-unused-parameter
        # in release mode only, allowed unused params (to get around ASSERT)
        $<$<CONFIG:Release>:-Wno-unused-variable>
        -Wno-unused-function

        -Wno-deprecated-declarations
        -Wno-sign-compare
        -Wno-unused-but-set-variable
        -Wno-unused-value
    )

    if (LINUX)
        target_compile_options(chugl_shared_properties INTERFACE
            # gcc ignores
            -Wno-strict-aliasing
            -Wno-stringop-truncation
            -Wno-type-limits
            -Wno-sequence-point
            -Wno-uninitialized
            -Wno-unused-variable
            -Wno-implicit-fallthrough
            -Wno-format-truncation
            -Wno-stringop-overflow
            -Wno-format
            -Wno-class-memaccess
            -Wno-strict-aliasing
        )
         
    
    endif()
endif()

# Ignore Windows warnings
if (MSVC)
	target_compile_options(
        chugl_shared_properties INTERFACE
        # Ignore a warning that GLM requires to bypass
        # Disable warning C4201: nonstandard extension used: nameless struct/union
        /wd4201
        # Required for GLM lookat()
        # Disable warning C4127: conditional expression is constant
        /wd4127
        # Disable warning C4305: truncation from 'int' to 'bool' in 'if' condition
        /wd4305
        # Ignore a warning that stb_image requires to bypass
        # Disable warning C4244: conversion from 'int' to 'short', possible loss of data
        /wd4244
        # ignore fopen warning for cgltf
        /wd4996
        # ignore warning for using 'bitwise and' to check if flag is set
        /wd26813
        # ignore hiding of previous local declaration
        /wd4456
        # ignore unused local variables
        /wd4189
        # ignore uninitialized member variables
        /wd26495 
        # ignore possible loss of data in integer type conversion
        /wd4267
        # ignore signed/unsigned mismatch
        /wd4389
    )
    target_compile_options(
        ${PROJECT_NAME} PRIVATE
        # allow unused variables for the ChuGin DLL Query
        /wd4100
    )

    # static linking with CRT
    target_compile_options(${PROJECT_NAME} PRIVATE "/MT$<$<CONFIG:Debug>:d>") 
endif (MSVC)


if ( MSVC )
    # Set the startup project to be the actual project instead of ALL_BUILD 
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

    set(CHUCK_EXECUTABLE_DEBUG "${CMAKE_SOURCE_DIR}/chuck-debug.exe")
    set(CHUCK_EXECUTABLE_RELEASE "${CMAKE_SOURCE_DIR}/chuck-release.exe")
    set(CHUGIN_PATH_DEBUG "--chugin:${CMAKE_TARGET_DIR}./build/Debug/ChuGL.chug")
    set(CHUGIN_PATH_RELEASE "--chugin:${CMAKE_TARGET_DIR}./build/Release/ChuGL.chug")
    set_target_properties( ${PROJECT_NAME} PROPERTIES
        # VS_DEBUGGER_COMMAND $<$<CONFIG:Debug>:${CHUCK_EXECUTABLE_DEBUG}>$<$<CONFIG:Release>:${CHUCK_EXECUTABLE_RELEASE}>
        VS_DEBUGGER_COMMAND "C:/Program Files/ChucK/chuck.exe"

        VS_DEBUGGER_COMMAND_ARGUMENTS "$<$<CONFIG:Debug>:${CHUGIN_PATH_DEBUG}>$<$<CONFIG:Release>:${CHUGIN_PATH_RELEASE}> --chugin-probe"

        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") # static linking with CRT
endif()

# add include path
target_include_directories(chugl_shared_properties 
    INTERFACE .
    INTERFACE vendor
    INTERFACE vendor/cimgui
    INTERFACE vendor/cimgui/imgui
    INTERFACE vendor/cimgui/imgui
    INTERFACE vendor/box2d/include
)

# vendor dependencies ==========================================================

# host our own distribution of WebGPU on CCRMA
# switching to static library instead of dynami b/c
# chuck cannot find build/Debug/webgpu_native.dll.
# throws Error [126]: The specified module could not be found.
# i.e. ChuGL DLL needs wgpu DLL which cannot be not found.
# will need to fix this to implement ChuGLins
FetchContent_Declare(
  webgpu
  GIT_REPOSITORY https://github.com/ccrma/webgpu-distribution/
  GIT_TAG        wgpu-static-22.1.0.5
)
FetchContent_MakeAvailable(webgpu)

if (NOT EMSCRIPTEN)
    add_subdirectory(vendor/glfw)
endif()
add_subdirectory(vendor/glfw3webgpu)
add_subdirectory(vendor/sr_webcam)

# FreeType font rendering
# set default freetype options
set(FT_DISABLE_BROTLI   ON  CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2    ON  CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON  CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG      ON  CACHE BOOL "" FORCE)
set(FT_DISABLE_ZLIB     ON  CACHE BOOL "" FORCE)
set(FT_ENABLE_ERROR_STRINGS ON CACHE BOOL "" FORCE)
add_subdirectory(vendor/freetype)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
	option(BOX2D_AVX2 "Enable AVX2 (faster)" ON)
endif()

# Needed for samples.exe to find box2d.dll
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# The Box2D library uses simde https://github.com/simd-everywhere/simde
add_subdirectory(vendor/simde)
add_subdirectory(vendor/box2d/src)

# linking
# target_link_libraries(${PROJECT_NAME} PRIVATE webgpu glfw glfw3webgpu)
target_link_libraries(chugl_shared_properties INTERFACE webgpu glfw glfw3webgpu box2d freetype sr_webcam)
target_link_libraries(${PROJECT_NAME} PRIVATE chugl_shared_properties)
if (CHUGL_BUILD_RENDERER_TESTS)
    target_link_libraries(ChuGL-Renderer-Tester PRIVATE chugl_shared_properties)
endif()

# emscripten specific options =================================================
if (EMSCRIPTEN)
    # know to rebuild when assets/ directory changes
    # set(SHELL_FILE shell_minimal.html)
    # set_property(
    #     TARGET ${PROJECT_NAME}
    #     PROPERTY LINK_DEPENDS
    #     ${CMAKE_CURRENT_SOURCE_DIR}/${SHELL_FILE}
    # )
    # set_target_properties(${PROJECT_NAME} PROPERTIES LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
    target_compile_options(${PROJECT_NAME} PRIVATE -pthread)

    target_link_options(${PROJECT_NAME} PRIVATE -sENVIRONMENT=worker)
    target_link_options(${PROJECT_NAME} PRIVATE -pthread)

    # Add Emscripten-specific link options
    target_link_options(${PROJECT_NAME} PRIVATE
        -sUSE_GLFW=3 # Use Emscripten-provided GLFW
        -sUSE_WEBGPU # Handle WebGPU symbols
        -sALLOW_MEMORY_GROWTH # allow memory to grow dynamically
        -sSIDE_MODULE=1  # needed for chuck to dynamically load the chugin

        # add thread support
        # -pthread

        # removing because ASYNCIFY not supported in side module
        # using infinite loop instead
        # -sASYNCIFY # Required for emscripten_sleep in the async wgpu adapter and device request

        # -sDISABLE_EXCEPTION_CATCHING=0

        # uncomment to include local files in the build
        # https://emscripten.org/docs/porting/files/packaging_files.html
        # https://emscripten.org/docs/getting_started/Tutorial.html#tutorial-files
        # To include files outside of the current working directory
        # use the `--preload-file srcpath@dstpath` syntax to explicitly 
        # specify the target location
        # --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@./assets

        # Can specify custom html template
        # https://github.com/emscripten-core/emscripten/blob/main/shell_minimal.html
        # --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/${SHELL_FILE}
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".wasm")

    # emscripten only generates .html properly if target is a MAIN_MODULE
    # set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
endif (EMSCRIPTEN)

string(TOLOWER ${CMAKE_HOST_SYSTEM_PROCESSOR} ARCH)

if(CMAKE_HOST_APPLE)
    set(CHUGINS_DIR ${CMAKE_SOURCE_DIR}/examples/chugins/darwin-${ARCH})
else()
    set(CHUGINS_DIR ${CMAKE_SOURCE_DIR}/examples/chugins/windows-${ARCH})
endif()

if(CMAKE_HOST_APPLE)
    install(
        TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION chugins/darwin-${ARCH}
    )

    if(CHUGIN_CODESIGN)
        install(
            CODE "execute_process (COMMAND codesign -vf -s - ${CHUGINS_DIR}/${CHUGIN_NAME}.chug)" 
        )
    endif()
else()
    install(
        TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION chugins/windows-${ARCH}
    )
endif()
