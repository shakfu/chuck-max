CWD=`pwd`
THIRDPARTY=${CWD}/build/thirdparty
PREFIX=${THIRDPARTY}/install

function setup() {
	mkdir -p ${PREFIX}/include && \
	mkdir -p ${PREFIX}/lib
}

function install_libsndfile() {
	BUILD=${THIRDPARTY}/libsndfile/build
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
}

function install_rubberband() {
	SRC=${THIRDPARTY}/rubberband
	mkdir -p ${THIRDPARTY} && \
	git clone --depth=1 https://github.com/breakfastquay/rubberband.git ${THIRDPARTY}/rubberband && \
	cd ${SRC} && \
	make -f otherbuilds/Makefile.macos && \
	cp lib/librubberband.a ${PREFIX}/lib && \
	cp -rf rubberband ${PREFIX}/include/
}

function install_libsamplerate() {
	SRC=${THIRDPARTY}/libsamplerate
	BUILD=${THIRDPARTY}/libsamplerate/build
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
}

setup
install_libsndfile
install_rubberband
install_libsamplerate

