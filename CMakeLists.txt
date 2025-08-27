cmake_minimum_required(VERSION 3.19)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version" FORCE)
string(REGEX REPLACE "(.*)/" "" THIS_FOLDER_NAME "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_HOST_WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

project(${THIS_FOLDER_NAME})

# general macos options
option(CM_MACOS_HOMEBREW "On MacOS, build using Homebrew dependencies")
option(CM_MACOS_UNIVERSAL "On MacOS, build universal architecture externals")
option(CM_HOST_EMBED_EXAMPLE "Build embedded host example")

# general chugin options
option(CM_MULTIPLATFORM_CHUGINS "Create os-specific subfolders in chugins folder")

# macos chugin options
option(CM_MACOS_BUNDLED_CHUGINS "On MacOS, Embed chugins in external bundle")
option(CM_WARPBUF "Build WarpBuf chugin")
option(CM_FAUST "Build Faust chugin")
option(CM_MP3 "Enable MP3 support for chugins using libsndfile")
option(CM_EXTRA_FORMATS "Enable flac, vorbis, ogg and opus support for chugins using libsndfile")
option(CM_CHUMP "Build experimental chump executable and chugin")

# warnings options
option(CM_SKIP_WARNINGS "skip a set of warnings" ON)

# use ccache if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Found ccache in ${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

if (APPLE)
    if (${CMAKE_GENERATOR} MATCHES "Xcode")
            if (${XCODE_VERSION} VERSION_LESS 10)
                message(STATUS "Xcode 10 or higher is required. Please install from the Mac App Store.")
                return ()
            endif ()
    endif ()

    if (NOT CMAKE_OSX_ARCHITECTURES)
        if(CM_MACOS_UNIVERSAL)
            set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "macOS architecture" FORCE)
        else()
            set(CMAKE_OSX_ARCHITECTURES ${CMAKE_SYSTEM_PROCESSOR} CACHE STRING "macOS architecture" FORCE)
        endif()
        message("CMAKE_OSX_ARCHITECTURES set to ${CMAKE_OSX_ARCHITECTURES}")
    endif()
endif()

if(CMAKE_HOST_APPLE) # i.e if is_macos and have_homebrew then get homebrew_prefix
    execute_process(
        COMMAND brew --prefix
        OUTPUT_VARIABLE homebrew_prefix
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(DEFINED homebrew_prefix AND CM_MACOS_HOMEBREW)
        set(local_prefix ${homebrew_prefix})
    else()
        set(local_prefix ${CMAKE_SOURCE_DIR}/build/thirdparty/install)
    endif()

    if(CM_MACOS_BUNDLED_CHUGINS)
        set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/externals/chuck~.mxo")
        set(EXTERNAL_RESOURCES_DIR "${EXTERNAL_DIR}/Contents/Resources")
    endif()
endif()
message(STATUS "local_prefix: ${local_prefix}")

include(${CMAKE_SOURCE_DIR}/source/scripts/cmake/fn_add_chugin.cmake)

MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

# Generate a project for every folder in the "source/category" folder
SUBDIRLIST(CATEGORY_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/source)
foreach (cat_dir ${CATEGORY_DIRS})
  SUBDIRLIST(PROJECT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/source/${cat_dir})
  foreach (project_dir ${PROJECT_DIRS})
    set(project_path ${CMAKE_CURRENT_SOURCE_DIR}/source/${cat_dir}/${project_dir})
    message("checking in: ${project_path}")
    if (EXISTS "${project_path}/CMakeLists.txt")
      message("Generating: ${project_dir}")
      add_subdirectory(${project_path})
    endif ()
  endforeach ()
endforeach ()
