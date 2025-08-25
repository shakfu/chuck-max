CWD=`pwd`
THIRDPARTY=${CWD}/build/thirdparty
PREFIX=${THIRDPARTY}/install

function setup() {
	mkdir -p ${PREFIX}/include && \
	mkdir -p ${PREFIX}/lib
}


function install_libogg() {
	SRC=${THIRDPARTY}/libogg
	BUILD=${THIRDPARTY}/libogg/build
	if [ ! -f ${THIRDPARTY}/install/lib/libogg.a ]; then
		rm -rf ${THIRDPARTY}/libogg && \
		mkdir -p build/thirdparty && \
		git clone --depth=1 https://github.com/xiph/ogg.git ${THIRDPARTY}/libogg && \
		mkdir -p ${BUILD} && \
		cd ${BUILD} && \
		cmake .. \
			-DCMAKE_VERBOSE_MAKEFILE=ON \
			-DCMAKE_C_FLAGS="-fPIC" \
			-DCMAKE_CXX_FLAGS="-fPIC" \
			-DBUILD_SHARED_LIBS=OFF \
			-DBUILD_TESTING=OFF \
			-DINSTALL_DOCS=OFF \
			-DINSTALL_MANPAGES=OFF \
			-DINSTALL_PKG_CONFIG_MODULE=OFF \
			-DINSTALL_CMAKE_PACKAGE_MODULE=OFF \
			-DCMAKE_INSTALL_PREFIX=${PREFIX} && \
		cmake --build . --config Release && \
		cmake --build . --target install
	fi
}


setup && install_libogg 
