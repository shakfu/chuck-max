
project(chump_cli_project)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(chump
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_include_directories(chump
	PUBLIC
	${CMAKE_SOURCE_DIR}/source/projects/chump/include
	${CMAKE_SOURCE_DIR}/source/projects/chugins/chuck/include
	$<$<PLATFORM_ID:Darwin>:${local_prefix}/include>
	$<$<PLATFORM_ID:Darwin>:${local_prefix}/include/minizip-ng>
)

target_link_directories(chump
	PUBLIC
	$<$<PLATFORM_ID:Darwin>:${local_prefix}/lib>
)

target_link_libraries(chump
	PUBLIC
	chump_lib
	$<$<PLATFORM_ID:Darwin>:${local_prefix}/lib/libssl.a>
	$<$<PLATFORM_ID:Darwin>:${local_prefix}/lib/libcrypto.a>
	$<$<PLATFORM_ID:Darwin>:${local_prefix}/lib/libminizip-ng.a>
	$<$<PLATFORM_ID:Darwin>:${local_prefix}/lib/libzstd.a>
	$<$<PLATFORM_ID:Darwin>:${local_prefix}/lib/liblzma.a>
	$<$<PLATFORM_ID:Darwin>:-lcurl>
	$<$<PLATFORM_ID:Darwin>:-lz>
	$<$<PLATFORM_ID:Darwin>:-liconv>
	$<$<PLATFORM_ID:Darwin>:-lbz2>
	"$<$<PLATFORM_ID:Darwin>:-framework Security>"
)
