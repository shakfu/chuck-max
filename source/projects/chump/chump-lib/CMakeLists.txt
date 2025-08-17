
project(chump_lib_project)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CHUMP_SOURCES
	chuck_version.cpp
	fetch.cpp
	manager.cpp
	package.cpp
	package_list.cpp
	util.cpp
)

add_library(chump_lib
	STATIC
	${CHUMP_SOURCES}
)

target_include_directories(chump_lib
	PUBLIC
	${CMAKE_SOURCE_DIR}/source/projects/chump/include
	${CMAKE_SOURCE_DIR}/source/projects/chugins/chuck/include
	$<$<PLATFORM_ID:Darwin>:${local_prefix}/include>
	$<$<PLATFORM_ID:Darwin>:${local_prefix}/include/minizip-ng>
)

target_link_directories(chump_lib
	PUBLIC
	$<$<PLATFORM_ID:Darwin>:${local_prefix}/lib>
)

target_link_libraries(chump_lib
	PUBLIC
	$<$<PLATFORM_ID:Darwin>:${local_prefix}/lib/libssl.a>
	$<$<PLATFORM_ID:Darwin>:${local_prefix}/lib/libcrypto.a>
	$<$<PLATFORM_ID:Darwin>:${local_prefix}/lib/libminizip-ng.a>
	$<$<PLATFORM_ID:Darwin>:-lcurl>
	$<$<PLATFORM_ID:Darwin>:-lz>
)

set_property(TARGET chump_lib PROPERTY OUTPUT_NAME chump)
