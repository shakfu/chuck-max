CWD=`pwd`
THIRDPARTY=${CWD}/build/thirdparty
PREFIX=${THIRDPARTY}/install

function setup() {
	mkdir -p ${PREFIX}/include && \
	mkdir -p ${PREFIX}/lib
}

function install_libmpg123() {
	SRC=${THIRDPARTY}/libmpg123
	BUILD=${THIRDPARTY}/libmpg123/build
	if [ ! -f ${THIRDPARTY}/install/lib/libmpg123.a ]; then
		rm -rf ${THIRDPARTY}/libmpg123 && \
		mkdir -p build/thirdparty && \
		wget https://www.mpg123.de/download/mpg123-1.32.6.tar.bz2 && \
		tar xvf mpg123-1.32.6.tar.bz2 && \
		mv mpg123-1.32.6 ${THIRDPARTY}/libmpg123 && \
		rm -f mpg123-1.32.6.tar.bz2 && \
		cd ${THIRDPARTY}/libmpg123 && \
		CFLAGS="-Os -s" ./configure \
			--with-module-suffix=.so \
			--enable-static \
			--with-default-audio=coreaudio \
			--with-cpu=aarch64 \
			--prefix=${PREFIX} && \
		make && \
		make install 
	fi
}

setup && install_libmpg123
