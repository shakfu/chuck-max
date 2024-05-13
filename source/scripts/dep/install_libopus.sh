# install full stack dependencies to build the WarpBuf and Fauck chugins

CWD=`pwd`
THIRDPARTY=${CWD}/build/thirdparty
PREFIX=${THIRDPARTY}/install
#FAUST_VERSION=2.69.3
FAUST_VERSION=2.72.14

function setup() {
	mkdir -p ${PREFIX}/include && \
	mkdir -p ${PREFIX}/lib
}

function install_libopus() {
	SRC=${THIRDPARTY}/libopus
	BUILD=${THIRDPARTY}/libopus/build
	if [ ! -f ${THIRDPARTY}/install/lib/libopus.a ]; then
		rm -rf ${THIRDPARTY}/libopus && \
		mkdir -p build/thirdparty && \
		git clone --depth=1 https://github.com/xiph/opus.git ${THIRDPARTY}/libopus && \
		mkdir -p ${BUILD} && \
		cd ${BUILD} && \
		cmake .. \
			-DCMAKE_VERBOSE_MAKEFILE=ON \
			-DCMAKE_C_FLAGS="-fPIC" \
			-DCMAKE_CXX_FLAGS="-fPIC" \
			-DBUILD_SHARED_LIBS=OFF \
			-DBUILD_TESTING=OFF \
			-DCMAKE_INSTALL_PREFIX=${PREFIX} && \
		cmake --build . --config Release && \
		cmake --build . --target install
	fi
}

setup && install_libopus
