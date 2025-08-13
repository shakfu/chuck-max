
add_chugin(
    OTHER_SOURCES
    	${CMAKE_CURRENT_SOURCE_DIR}/genlib/FFTReal.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/genlib/Obucket.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/genlib/Odelay.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/genlib/Offt.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/genlib/Ooscil.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/genlib/RandGen.cpp
    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/genlib
)
