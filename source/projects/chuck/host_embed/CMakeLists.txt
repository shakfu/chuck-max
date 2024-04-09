set(CHUCK_DIR ${CMAKE_SOURCE_DIR}/source/projects/chuck~)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


set(CXX_HOST_EMBED
	chuck-embed.cpp
	RtAudio/RtAudio.cpp
)

add_executable(chuck_embed
	${CXX_HOST_EMBED}
)

target_compile_options(chuck_embed
	PUBLIC
	-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk
	-O3
)

target_compile_definitions(chuck_embed
	PUBLIC
	$<$<BOOL:${APPLE}>:__MACOSX_CORE__>
	HAVE_CONFIG_H
)

target_include_directories(chuck_embed
	PUBLIC
	${CHUCK_DIR}/core
	${CHUCK_DIR}/host
	${CHUCK_DIR}/host/RtAudio
	${CHUCK_DIR}/host/RtAudio/include
)

target_link_libraries(chuck_embed
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

file(COPY sine.ck DESTINATION ${CMAKE_BINARY_DIR})
