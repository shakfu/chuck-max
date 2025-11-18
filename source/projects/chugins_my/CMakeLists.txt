project(chugins_my)

set (CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE Release)
set(BUILD_SHARED_LIBS ON)

if(CM_MACOS_BUNDLED_CHUGINS AND CMAKE_HOST_APPLE)
    set(CMAKE_INSTALL_PREFIX "${EXTERNAL_RESOURCES_DIR}" CACHE PATH "..." FORCE)
else()
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/examples" CACHE PATH "..." FORCE)
endif()

add_subdirectory(AbletonLink)
if(CMAKE_HOST_APPLE)
    add_subdirectory(AudioUnit)
endif()
add_subdirectory(CLAP)
if(CM_VST3)
    add_subdirectory(VST3)
endif()

