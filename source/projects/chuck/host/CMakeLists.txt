
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CHUCK_DIR ${CMAKE_SOURCE_DIR}/source/projects/chuck~)

set(CXX_HOST
	chuck_main.cpp
	chuck_console.cpp
	chuck_audio.cpp
	RtAudio/RtAudio.cpp
)

add_executable(chuck
	${CXX_HOST}
)

if(CMAKE_HOST_APPLE)
target_compile_options(chuck
	PUBLIC
	-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk
)
endif()

target_compile_definitions(chuck
	PUBLIC
	$<$<BOOL:${APPLE}>:__MACOSX_CORE__>
	$<$<BOOL:${WIN32}>:__WINDOWS_DS__>
	$<$<BOOL:${WIN32}>:__WINDOWS_ASIO__>
	$<$<BOOL:${WIN32}>:__WINDOWS_WASAPI__>
	$<$<BOOL:${WIN32}>:WIN32>
	$<$<BOOL:${WIN32}>:NDEBUG>
	$<$<BOOL:${WIN32}>:_CONSOLE>
	HAVE_CONFIG_H
)



target_include_directories(chuck
	PUBLIC
	${CHUCK_DIR}/core
	${CHUCK_DIR}/host
	${CHUCK_DIR}/host/RtAudio
	${CHUCK_DIR}/host/RtAudio/include
)

target_link_libraries(chuck
	PUBLIC
	"$<$<BOOL:${APPLE}>:-framework CoreAudio>"
	"$<$<BOOL:${APPLE}>:-framework CoreMIDI>"
	"$<$<BOOL:${APPLE}>:-framework CoreFoundation>"
	"$<$<BOOL:${APPLE}>:-framework IOKit>"
	"$<$<BOOL:${APPLE}>:-framework Carbon>"
	"$<$<BOOL:${APPLE}>:-framework AppKit>"
	"$<$<BOOL:${APPLE}>:-framework Foundation>"
	"$<$<BOOL:${APPLE}>:-F/System/Library/PrivateFrameworks>"
	"$<$<BOOL:${APPLE}>:-weak_framework MultitouchSupport>"
	-lc++
	-lm
	chuck_lib
)
