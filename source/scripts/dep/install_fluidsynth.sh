# install full stack dependencies to build the WarpBuf and Fauck chugins

CWD=`pwd`
THIRDPARTY=${CWD}/build/thirdparty
PREFIX=${THIRDPARTY}/install

function setup() {
	mkdir -p ${PREFIX}/include && \
	mkdir -p ${PREFIX}/lib
}


function install_fluidsynth() {
	SRC=${THIRDPARTY}/fluidsynth
	if [ ! -f ${THIRDPARTY}/install/lib/libfluidsynth.a ]; then
		rm -rf ${THIRDPARTY}/rubberband && \
		mkdir -p ${THIRDPARTY} && \
		git clone --depth=1 https://github.com/FluidSynth/fluidsynth.git ${THIRDPARTY}/fluidsynth && \
		cd ${SRC} && \
		mkdir build && cd build && \
		cmake .. \
			-DBUILD_SHARED_LIBS=OFF \
			-Denable-framework=OFF \
			-Denable-readline=OFF \
			-Denable-sdl2=OFF && \
		cmake --build . --config Release && \
		cmake --install . --prefix ${PREFIX}
	fi
}

setup && install_fluidsynth
