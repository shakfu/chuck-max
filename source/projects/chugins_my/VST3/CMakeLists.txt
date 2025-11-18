# set(VST3_SDK_ROOT ${CMAKE_SOURCE_DIR}/thirdparty/vst3sdk)
set(VST3_SDK_ROOT ${CMAKE_BINARY_DIR}/thirdparty/vst3sdk)
set(VST3_BRANCH "v3.8.0_build_66")

set(CMAKE_OSX_ARCHITECTURES ${CMAKE_SYSTEM_PROCESSOR} CACHE STRING "macOS architecture" FORCE)
set(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES OFF CACHE STRING "Enable VST 3 Hosting Examples" FORCE)
set(SMTG_ENABLE_VST3_HOSTING_EXAMPLES OFF CACHE STRING "macOS architecture" FORCE)
set(SMTG_ENABLE_VSTGUI_SUPPORT OFF CACHE STRING "Enable VSTGUI Support" FORCE)
set(SMTG_ENABLE_WAYLAND_SUPPORT OFF CACHE STRING "Enable Wayland Support (Linux only)" FORCE)


if (NOT EXISTS ${VST3_SDK_ROOT})
	execute_process (
		COMMAND mkdir -p ${CMAKE_BINARY_DIR}/thirdparty
		COMMAND git clone -b ${VST3_BRANCH} --depth=1 --recursive https://github.com/steinbergmedia/vst3sdk ${VST3_SDK_ROOT}
	)
endif()

add_subdirectory(${VST3_SDK_ROOT} ${CMAKE_BINARY_DIR}/vst3-build)

# VST3 Chugin CMakeLists.txt

# Platform-specific VST3 hosting module sources
if(CMAKE_HOST_APPLE)
    set(VST3_MODULE_SOURCE "${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/module_mac.mm")
    set_source_files_properties(
        "${VST3_MODULE_SOURCE}" PROPERTIES
        COMPILE_FLAGS "-fobjc-arc"
    )
elseif(CMAKE_HOST_LINUX)
    set(VST3_MODULE_SOURCE "${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/module_linux.cpp")
elseif(CMAKE_HOST_WIN32)
    set(VST3_MODULE_SOURCE "${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/module_win32.cpp")
endif()

add_chugin(
	OTHER_SOURCES
		${VST3_MODULE_SOURCE}
		"${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/plugprovider.cpp"
		"${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/hostdataexchangehandler.cpp"
	COMPILE_DEFINITIONS
		HAVE_VST3_SDK
	LINK_LIBS
		base
		sdk
		sdk_common
		pluginterfaces
		sdk_hosting
        "$<$<PLATFORM_ID:Darwin>:-framework CoreFoundation>"
        "$<$<PLATFORM_ID:Darwin>:-framework CoreServices>"
)
