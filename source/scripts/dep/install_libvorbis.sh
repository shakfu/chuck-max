CWD=`pwd`
THIRDPARTY=${CWD}/build/thirdparty
PREFIX=${THIRDPARTY}/install

function setup() {
	mkdir -p ${PREFIX}/include && \
	mkdir -p ${PREFIX}/lib
}


function install_libvorbis() {
	SRC=${THIRDPARTY}/libvorbis
	BUILD=${THIRDPARTY}/libvorbis/build
	if [ ! -f ${THIRDPARTY}/install/lib/libvorbis.a ]; then
		rm -rf ${THIRDPARTY}/libvorbis && \
		mkdir -p build/thirdparty && \
		git clone --depth=1 https://github.com/xiph/vorbis.git ${THIRDPARTY}/libvorbis && \
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


setup && install_libvorbis 
