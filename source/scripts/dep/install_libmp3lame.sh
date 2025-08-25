CWD=`pwd`
THIRDPARTY=${CWD}/build/thirdparty
PREFIX=${THIRDPARTY}/install

function setup() {
	mkdir -p ${PREFIX}/include && \
	mkdir -p ${PREFIX}/lib
}


function install_libmp3lame() {
	SRC=${THIRDPARTY}/libmp3lame
	BUILD=${THIRDPARTY}/libmp3lame/build
	if [ ! -f ${THIRDPARTY}/install/lib/libmp3lame.a ]; then
		rm -rf ${THIRDPARTY}/libmp3lame && \
		mkdir -p build/thirdparty && \
		wget https://downloads.sourceforge.net/project/lame/lame/3.100/lame-3.100.tar.gz && \
		tar xvf lame-3.100.tar.gz && \
		mv lame-3.100 ${THIRDPARTY}/libmp3lame && \
		rm -f lame-3.100.tar.gz && \
		patch -u ${THIRDPARTY}/libmp3lame/include/libmp3lame.sym \
			  -i ${CWD}/source/scripts/patch/libmp3lame_sym.patch && \
		cd ${THIRDPARTY}/libmp3lame && \
		CFLAGS="-Os -s" ./configure \
			--disable-dependency-tracking \
			--disable-debug \
			--enable-nasm \
			--prefix=${PREFIX} && \
		make && \
		make install 
	fi
}


setup && install_libmp3lame
