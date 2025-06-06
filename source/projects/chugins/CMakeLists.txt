project(chugins)

set (CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE Release)
set(BUILD_SHARED_LIBS ON)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/examples/chugins)
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/chugins)

if(APPLE)
	# set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/externals/chuck~.mxo/Contents/Resources" CACHE PATH "..." FORCE)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/examples" CACHE PATH "..." FORCE)
else()
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/examples" CACHE PATH "..." FORCE)
endif()


add_subdirectory(ABSaturator)
add_subdirectory(AmbPan)
add_subdirectory(Binaural)
add_subdirectory(Bitcrusher)
add_subdirectory(ConvRev)
add_subdirectory(Elliptic)
add_subdirectory(ExpDelay)
add_subdirectory(ExpEnv)
if(ENABLE_FAUCK)
    add_subdirectory(Fauck)
endif()
add_subdirectory(FIR)
if(ENABLE_FLUIDSYNTH)
    add_subdirectory(FluidSynth)
endif()
add_subdirectory(FoldbackSaturator)
add_subdirectory(GVerb)
add_subdirectory(KasFilter)
add_subdirectory(Ladspa)
add_subdirectory(Line)
add_subdirectory(MagicSine)
add_subdirectory(Mesh2D)
add_subdirectory(MIAP)
add_subdirectory(Multicomb)
add_subdirectory(NHHall)
add_subdirectory(Overdrive)
add_subdirectory(PanN)
add_subdirectory(Patch)
add_subdirectory(Perlin)
add_subdirectory(PitchTrack)
add_subdirectory(PowerADSR)
add_subdirectory(Random)
add_subdirectory(Range)
add_subdirectory(RegEx)
add_subdirectory(Sigmund)
add_subdirectory(Spectacle)
if(ENABLE_WARPBUF)
    add_subdirectory(WarpBuf)
endif()
add_subdirectory(Wavetable)
add_subdirectory(WinFuncEnv)
add_subdirectory(WPDiodeLadder)
add_subdirectory(WPKorg35)
add_subdirectory(XML)
