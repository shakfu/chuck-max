
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (CMAKE_HOST_APPLE)
execute_process(
	COMMAND bison -dv -b chuck chuck.y
	COMMAND flex -ochuck.yy.c chuck.lex
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
endif()

set(C_CORE
	$<$<PLATFORM_ID:Darwin>:chuck.tab.c>
	$<$<PLATFORM_ID:Darwin>:chuck.yy.c>
	$<$<PLATFORM_ID:Windows>:chuck_yacc.c>
	util_network.c
	util_raw.c
	util_xforms.c
)

set(CXX_CORE
	chuck.cpp
	chuck_absyn.cpp
	chuck_errmsg.cpp
	chuck_frame.cpp
	chuck_parse.cpp
	chuck_carrier.cpp
	chuck_compile.cpp
	chuck_dl.cpp
	chuck_emit.cpp
	chuck_globals.cpp
	chuck_instr.cpp
	chuck_io.cpp
	chuck_lang.cpp
	chuck_oo.cpp
	chuck_otf.cpp
	chuck_scan.cpp
	chuck_shell.cpp
	chuck_stats.cpp
	chuck_symbol.cpp
	chuck_table.cpp
	chuck_type.cpp
	chuck_ugen.cpp
	chuck_vm.cpp
	hidio_sdl.cpp
	midiio_rtmidi.cpp
	rtmidi.cpp
	uana_extract.cpp
	uana_xform.cpp
	ugen_filter.cpp
	ugen_osc.cpp
	ugen_stk.cpp
	ugen_xxx.cpp
	ulib_ai.cpp
	ulib_doc.cpp
	ulib_machine.cpp
	ulib_math.cpp
	ulib_opsc.cpp
	ulib_std.cpp
	util_buffers.cpp
	util_console.cpp
	util_hid.cpp
	util_math.cpp
	util_platforms.cpp
	util_opsc.cpp
	util_serial.cpp
	util_string.cpp
	util_thread.cpp
)

set(LO_C_CORE
	lo/address.c
	lo/blob.c
	lo/bundle.c
	lo/message.c
	lo/method.c
	lo/pattern_match.c
	lo/send.c
	lo/server.c
	$<$<PLATFORM_ID:Darwin>:lo/server_thread.c>
	lo/timetag.c
)


add_library(chuck_lib
	STATIC
	util_sndfile.c
	${C_CORE}
	${CXX_CORE}
	${LO_C_CORE}
)

if (CMAKE_HOST_APPLE AND CM_SKIP_WARNINGS)
	set_source_files_properties(
		util_sndfile.c
		util_hid.cpp
		ulib_opsc.cpp
		ulib_doc.cpp
		ugen_stk.cpp
		chuck.yy.c
		PROPERTIES
		COMPILE_OPTIONS "-Wno-shorten-64-to-32"
	)
endif()


# target_compile_options(chuck_lib
# 	PUBLIC
# 	-stdlib=libc++
# 	-O3
# )

target_compile_definitions(chuck_lib
	PUBLIC
	$<$<PLATFORM_ID:Darwin>:__MACOSX_CORE__>
	$<$<PLATFORM_ID:Windows>:__PLATFORM_WINDOWS__>
	$<$<PLATFORM_ID:Windows>:__PLATFORM_WIN32__>
	$<$<CONFIG:Debug>:__CHUCK_DEBUG__>
	HAVE_CONFIG_H
)


target_include_directories(chuck_lib
	PUBLIC
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/lo
)

target_link_libraries(chuck_lib
	PUBLIC
	$<$<PLATFORM_ID:Windows>:kernel32>
	$<$<PLATFORM_ID:Windows>:user32>
	$<$<PLATFORM_ID:Windows>:gdi32>
	$<$<PLATFORM_ID:Windows>:winspool>
	$<$<PLATFORM_ID:Windows>:comdlg32>
	$<$<PLATFORM_ID:Windows>:advapi32>
	$<$<PLATFORM_ID:Windows>:shell32>
	$<$<PLATFORM_ID:Windows>:ole32>
	$<$<PLATFORM_ID:Windows>:oleaut32>
	$<$<PLATFORM_ID:Windows>:uuid>
	$<$<PLATFORM_ID:Windows>:odbc32>
	$<$<PLATFORM_ID:Windows>:odbccp32>
	# $<$<PLATFORM_ID:Windows>:/subsystem:console>
	# $<$<PLATFORM_ID:Windows>:/machine:I386>
	$<$<PLATFORM_ID:Windows>:dsound>
	# $<$<PLATFORM_ID:Windows>:dinput>
	$<$<PLATFORM_ID:Windows>:dinput8>
	$<$<PLATFORM_ID:Windows>:dxguid>
	$<$<PLATFORM_ID:Windows>:wsock32>
	$<$<PLATFORM_ID:Windows>:ws2_32>
	$<$<PLATFORM_ID:Windows>:winmm>
	$<$<PLATFORM_ID:Windows>:iphlpapi>
)

set_property(TARGET chuck_lib PROPERTY OUTPUT_NAME chuck)


