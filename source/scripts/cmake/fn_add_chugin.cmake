
function(add_chugin)
    unset(CMAKE_OSX_DEPLOYMENT_TARGET)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "requires >= 10.15" FORCE)

    set(CMAKE_EXPORT_COMPILE_COMMANDS True)

    set(options
        DEBUG
        CODESIGN
    )
    set(oneValueArgs
        NAME
        CHUCK_HEADER_PATH
    )
    set(multiValueArgs
        SOURCES
        OTHER_SOURCES
        INCLUDE_DIRS
        LINK_LIBS
        LINK_DIRS
        COMPILE_DEFINITIONS
        COMPILE_OPTIONS
        LINK_OPTIONS
    )

    cmake_parse_arguments(
        CHUGIN
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    set(path "${CMAKE_CURRENT_SOURCE_DIR}")
    cmake_path(GET path STEM PARENT_DIR)

    string(TOLOWER ${CMAKE_HOST_SYSTEM_PROCESSOR} ARCH)

    if(CMAKE_HOST_APPLE)
        set(CHUGINS_DIR ${CMAKE_SOURCE_DIR}/examples/chugins/darwin-${ARCH})
    else()
        set(CHUGINS_DIR ${CMAKE_SOURCE_DIR}/examples/chugins/windows-${ARCH})
    endif()

    if(NOT DEFINED CHUGIN_NAME)
        set(CHUGIN_NAME ${PARENT_DIR})
    endif()

    if(NOT DEFINED CHUGIN_CHUCK_HEADER_PATH)
        set(CHUGIN_CHUCK_HEADER_PATH ${CMAKE_SOURCE_DIR}/source/projects/chugins/chuck/include)
    endif()

    message(STATUS "CHUGIN_NAME: ${CHUGIN_NAME}")
    if(CHUGIN_DEBUG)
        message(STATUS "CHUCK_HEADER_PATH: ${CHUGIN_CHUCK_HEADER_PATH}")
        message(STATUS "CHUGIN_SOURCES: ${CHUGIN_SOURCES}")
        message(STATUS "OTHER_SOURCES: ${CHUGIN_OTHER_SOURCES}")
        message(STATUS "INCLUDE_DIRS: ${CHUGIN_INCLUDE_DIRS}")
        message(STATUS "COMPILE_DEFINITIONS: ${CHUGIN_COMPILE_DEFINITIONS}")
        message(STATUS "COMPILE_OPTIONS: ${CHUGIN_COMPILE_OPTIONS}")
        message(STATUS "LINK_LIBS: ${CHUGIN_LINK_LIBS}")
        message(STATUS "LINK_DIRS: ${CHUGIN_LINK_DIRS}")
        message(STATUS "LINK_OPTIONS: ${CHUGIN_LINK_OPTIONS}")
    endif()

    if(NOT CHUGIN_SOURCES)
        file(GLOB CHUGIN_SOURCES
            "*.h"
            "*.c"
            "*.cpp"
        )
    endif()

    add_library( 
        ${CHUGIN_NAME} 
        MODULE
        ${CHUGIN_SOURCES}
        ${CHUGIN_OTHER_SOURCES}
    )

    if(CMAKE_HOST_APPLE AND SKIP_WARNING_SHORTEN_64_TO_32)
        set_source_files_properties(
            ${CHUGIN_SOURCES}
            ${CHUGIN_OTHER_SOURCES}
            PROPERTIES
            COMPILE_OPTIONS "-Wno-shorten-64-to-32"
        )
    endif()

    set_target_properties(${CHUGIN_NAME}
        PROPERTIES
        PREFIX ""
        SUFFIX ".chug"
        POSITION_INDEPENDENT_CODE ON
    )

    target_include_directories(
        ${CHUGIN_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CHUGIN_INCLUDE_DIRS}
        ${CHUGIN_CHUCK_HEADER_PATH}
    )

    target_compile_definitions(
        ${CHUGIN_NAME}
        PRIVATE
        ${CHUGIN_COMPILE_DEFINITIONS}
        HAVE_CONFIG_H
        $<$<CONFIG:Release>:NDEBUG>
        $<$<PLATFORM_ID:Darwin>:__MACOSX_CORE__>
        $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:__CK_DLL_STATIC__>
    )

    target_compile_options(
        ${CHUGIN_NAME}
        PRIVATE
        ${CHUGIN_COMPILE_OPTIONS}
        $<$<PLATFORM_ID:Darwin>:-fPIC>
    )

    target_link_directories(
        ${CHUGIN_NAME} 
        PRIVATE
        ${CHUGIN_LINK_DIRS}
    )

    target_link_options(
        ${CHUGIN_NAME}
        PRIVATE
        ${CHUGIN_LINK_OPTIONS}
        # -shared
    )

    target_link_libraries(
        ${CHUGIN_NAME} 
        PRIVATE
        ${CHUGIN_LINK_LIBS}
    )

if(CMAKE_HOST_APPLE)
    install(
        TARGETS ${CHUGIN_NAME}
        LIBRARY DESTINATION chugins/darwin-${ARCH}
    )

    if(CHUGIN_CODESIGN)
        install(
            CODE "execute_process (COMMAND codesign -vf -s - ${CHUGINS_DIR}/${CHUGIN_NAME}.chug)" 
        )
    endif()
else()
    install(
        TARGETS ${CHUGIN_NAME}
        LIBRARY DESTINATION chugins/windows-${ARCH}
    )
endif()

endfunction()
