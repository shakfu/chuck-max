CWD=`pwd`
THIRDPARTY=${CWD}/build/thirdparty
PREFIX=${THIRDPARTY}/install

function setup() {
	mkdir -p ${PREFIX}/include && \
	mkdir -p ${PREFIX}/lib
}


function install_libflac() {
	SRC=${THIRDPARTY}/libflac
	BUILD=${THIRDPARTY}/libflac/build
	if [ ! -f ${THIRDPARTY}/install/lib/libFLAC.a ]; then
		rm -rf ${THIRDPARTY}/libflac && \
		mkdir -p build/thirdparty && \
		git clone --depth=1 https://github.com/xiph/flac.git ${THIRDPARTY}/libflac && \
		mkdir -p ${BUILD} && \
		cd ${BUILD} && \
		cmake .. \
			-DCMAKE_VERBOSE_MAKEFILE=ON \
			-DCMAKE_C_FLAGS="-fPIC" \
			-DCMAKE_CXX_FLAGS="-fPIC" \
			-DBUILD_PROGRAMS=OFF \
			-DBUILD_EXAMPLES=OFF \
			-DBUILD_TESTING=OFF \
			-DBUILD_DOCS=OFF \
			-DINSTALL_MANPAGES=OFF \
			-DINSTALL_PKGCONFIG_MODULES=OFF \
			-DINSTALL_CMAKE_CONFIG_MODULE=OFF \
			-DWITH_OGG=OFF \
			-DBUILD_SHARED_LIBS=OFF \
			-DENABLE_MULTITHREADING=ON \
			-DCMAKE_INSTALL_PREFIX=${PREFIX} && \
		cmake --build . --config Release && \
		cmake --build . --target install
	fi
}

setup && install_libflac

