CWD=`pwd`
THIRDPARTY=${CWD}/build/thirdparty
PREFIX=${THIRDPARTY}/install

function setup() {
	mkdir -p ${PREFIX}/include && \
	mkdir -p ${PREFIX}/lib
}


function install_libsamplerate() {
	SRC=${THIRDPARTY}/libsamplerate
	BUILD=${THIRDPARTY}/libsamplerate/build
	if [ ! -f ${THIRDPARTY}/install/lib/libsamplerate.a ]; then
		rm -rf ${THIRDPARTY}/libsamplerate && \
		mkdir -p ${THIRDPARTY} && \
		git clone --depth=1 https://github.com/libsndfile/libsamplerate.git ${THIRDPARTY}/libsamplerate && \
		mkdir -p ${BUILD} && \
		cd ${BUILD} && \
		cmake .. \
			-DCMAKE_VERBOSE_MAKEFILE=ON \
			-DBUILD_TESTING=OFF \
			-DCMAKE_INSTALL_PREFIX=${PREFIX} && \
		cmake --build . --config Release && \
		cmake --build . --target install
	fi
}

setup && install_libsamplerate
