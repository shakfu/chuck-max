
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

execute_process(
	COMMAND bison -dv -b chuck chuck.y
	COMMAND flex -ochuck.yy.c chuck.lex
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set(C_CORE
	chuck.tab.c
	chuck.yy.c
	util_math.c
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
	chuck_utils.cpp
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
	lo/server_thread.c
	lo/timetag.c
)

# include_directories( 
#     "${MAX_SDK_INCLUDES}"
#     "${MAX_SDK_MSP_INCLUDES}"
#     "${MAX_SDK_JIT_INCLUDES}"   
# )

add_library(chuck_lib
	STATIC
	util_sndfile.c
	${C_CORE}
	${CXX_CORE}
	${LO_C_CORE}
)


target_compile_options(chuck_lib
	PUBLIC
	-O3
)

target_compile_definitions(chuck_lib
	PUBLIC
	__MACOSX_CORE__
	HAVE_CONFIG_H

)


target_include_directories(chuck_lib
	PUBLIC
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/lo
)

set_property(TARGET chuck_lib PROPERTY OUTPUT_NAME chuck)


