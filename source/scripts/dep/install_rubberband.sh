CWD=`pwd`
THIRDPARTY=${CWD}/build/thirdparty
PREFIX=${THIRDPARTY}/install


function setup() {
	mkdir -p ${PREFIX}/include && \
	mkdir -p ${PREFIX}/lib
}


function install_rubberband() {
	SRC=${THIRDPARTY}/rubberband
	if [ ! -f ${THIRDPARTY}/install/lib/librubberband.a ]; then
		rm -rf ${THIRDPARTY}/rubberband && \
		mkdir -p ${THIRDPARTY} && \
		git clone --depth=1 https://github.com/breakfastquay/rubberband.git ${THIRDPARTY}/rubberband && \
		cd ${SRC} && \
		make -f otherbuilds/Makefile.macos && \
		cp lib/librubberband.a ${PREFIX}/lib && \
		cp -rf rubberband ${PREFIX}/include/
	fi
}

setup && install_rubberband
