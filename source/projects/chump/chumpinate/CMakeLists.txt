project(chump_chugin_project)

add_chugin(
	NAME "Chumpinate"
	CODESIGN
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Chumpinate.cpp
    INCLUDE_DIRS
    	${CMAKE_SOURCE_DIR}/source/projects/chump/include
    	$<$<PLATFORM_ID:Darwin>:${local_prefix}/include>
		$<$<PLATFORM_ID:Darwin>:${local_prefix}/include/minizip-ng>
    LINK_LIBS
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