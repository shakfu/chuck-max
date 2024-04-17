CWD=`pwd`
THIRDPARTY=${CWD}/build/thirdparty
PREFIX=${THIRDPARTY}/install

function setup() {
	mkdir -p ${PREFIX}/include && \
	mkdir -p ${PREFIX}/lib
}

function install_libsndfile() {
	SRC=${THIRDPARTY}/libsndfile
	BUILD=${THIRDPARTY}/libsndfile/build
	if [ ! -f ${THIRDPARTY}/install/lib/libsndfile.a ]; then
		mkdir -p build/thirdparty && \
		git clone --depth=1 https://github.com/libsndfile/libsndfile.git ${THIRDPARTY}/libsndfile && \
		mkdir -p ${BUILD} && \
		cd ${BUILD} && \
		cmake .. \
			-DCMAKE_VERBOSE_MAKEFILE=ON \
			-DCMAKE_C_FLAGS="-fPIC" \
			-DCMAKE_CXX_FLAGS="-fPIC" \
			-DBUILD_TESTING=OFF \
			-DENABLE_EXTERNAL_LIBS=OFF \
			-DENABLE_MPEG=OFF \
			-DBUILD_PROGRAMS=OFF \
			-DBUILD_EXAMPLES=OFF \
			-DENABLE_CPACK=OFF \
			-DCMAKE_INSTALL_PREFIX=${PREFIX} && \
		cmake --build . --config Release && \
		cmake --build . --target install
	fi
}

function install_rubberband() {
	SRC=${THIRDPARTY}/rubberband
	if [ ! -f ${THIRDPARTY}/install/lib/librubberband.a ]; then
		mkdir -p ${THIRDPARTY} && \
		git clone --depth=1 https://github.com/breakfastquay/rubberband.git ${THIRDPARTY}/rubberband && \
		cd ${SRC} && \
		make -f otherbuilds/Makefile.macos && \
		cp lib/librubberband.a ${PREFIX}/lib && \
		cp -rf rubberband ${PREFIX}/include/
	fi
}

function install_libsamplerate() {
	SRC=${THIRDPARTY}/libsamplerate
	BUILD=${THIRDPARTY}/libsamplerate/build
	if [ ! -f ${THIRDPARTY}/install/lib/libsamplerate.a ]; then
		mkdir -p build/thirdparty && \
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


setup
install_libsndfile
install_rubberband
install_libsamplerate

