set(path "${CMAKE_CURRENT_SOURCE_DIR}")
cmake_path(GET path STEM PARENT_DIR)

# chugin name
set(CHUGIN_NAME ${PARENT_DIR})


set(GENLIB_SRC
	genlib/FFTReal.cpp
	genlib/Obucket.cpp
	genlib/Odelay.cpp
	genlib/Offt.cpp
	genlib/Ooscil.cpp
	genlib/RandGen.cpp
)

# all of the c/cpp files that compose this chugin
file(GLOB PROJECT_SRC
    "*.h"
    "*.c"
    "*.cpp"
)

# where the chuck headers are
set(CK_SRC_PATH
	"../chuck/include/"
)

add_library(${CHUGIN_NAME}
	${GENLIB_SRC}
	${PROJECT_SRC}
)

if(BUILD_SHARED_LIBS)
	set_target_properties(${CHUGIN_NAME}
		PROPERTIES
		PREFIX ""
		SUFFIX ".chug"
	)
endif()

target_compile_options(${CHUGIN_NAME}
	PUBLIC
	-fPIC
	$<IF:$<CONFIG:Debug>,-g,-O3>

)

target_compile_definitions(${CHUGIN_NAME}
	PUBLIC
	__MACOSX_CORE__
	HAVE_CONFIG_H
	$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:__CK_DLL_STATIC__>
)

target_include_directories(${CHUGIN_NAME}
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/genlib
	${CK_SRC_PATH}
)

target_link_options(${CHUGIN_NAME}
	PUBLIC
	-shared
	-lc++
)

# target_link_libraries(${CHUGIN_NAME}
# 	PUBLIC
# 	-lm
# )

install(
	TARGETS ${CHUGIN_NAME}
	LIBRARY DESTINATION	chugins
)
