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
		rm -rf ${THIRDPARTY}/libsndfile && \
		mkdir -p build/thirdparty && \
		git clone --depth=1 https://github.com/libsndfile/libsndfile.git ${THIRDPARTY}/libsndfile && \
		mkdir -p ${BUILD} && \
		cd ${BUILD} && \
		cmake .. \
			-DCMAKE_VERBOSE_MAKEFILE=ON \
			-DCMAKE_C_FLAGS="-fPIC" \
			-DCMAKE_CXX_FLAGS="-fPIC" \
			-DBUILD_TESTING=OFF \
			-DENABLE_EXTERNAL_LIBS=ON \
			-DENABLE_MPEG=ON \
			-DBUILD_PROGRAMS=OFF \
			-DBUILD_EXAMPLES=OFF \
			-DENABLE_CPACK=OFF \
			-DCMAKE_INSTALL_PREFIX=${PREFIX} && \
		cmake --build . --config Release && \
		cmake --build . --target install
	fi
}

setup && install_libsndfile
