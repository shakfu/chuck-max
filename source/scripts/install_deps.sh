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

function install_faust() {
	VERSION=${FAUST_VERSION}
	SRC=${THIRDPARTY}/faust
    if [ ! -d ${THIRDPARTY}/faust/architecture ]; then
    	rm -rf ${THIRDPARTY}/faust && \
		git clone -b "${VERSION}" --depth=1 https://github.com/grame-cncm/faust.git ${THIRDPARTY}/faust
	fi
}

function install_libfaust() {
	VERSION=${FAUST_VERSION}
	if [ "$(uname)" = "Darwin" ]; then
	    if [ ! -f ${THIRDPARTY}/libfaust/lib/libfaustwithllvm.a ]; then
	    	rm -rf ${THIRDPARTY}/libfaust
			if [ "$(uname -m)" = "arm64" ]; then
				if [ ! -f Faust-$VERSION-arm64.dmg ]; then
					curl -L https://github.com/grame-cncm/faust/releases/download/$VERSION/Faust-$VERSION-arm64.dmg -o Faust-$VERSION-arm64.dmg
					hdiutil attach Faust-$VERSION-arm64.dmg
					mkdir -p ${THIRDPARTY}/libfaust
					cp -Rf /Volumes/Faust-$VERSION/Faust-$VERSION/* ${THIRDPARTY}/libfaust/
					hdiutil detach /Volumes/Faust-$VERSION/
					rm -f Faust-$VERSION-arm64.dmg
				fi					
			else
				if [ ! -f Faust-$VERSION-x64.dmg ]; then
					curl -L https://github.com/grame-cncm/faust/releases/download/$VERSION/Faust-$VERSION-x64.dmg -o Faust-$VERSION-x64.dmg
					hdiutil attach Faust-$VERSION-x64.dmg
					mkdir -p ${THIRDPARTY}/libfaust
					cp -Rf /Volumes/Faust-$VERSION/Faust-$VERSION/* ${THIRDPARTY}/libfaust/
					hdiutil detach /Volumes/Faust-$VERSION/
					rm -f Faust-$VERSION-x64.dmg
				fi
			fi
		fi
	fi
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

# function install_libmpg123() {
# 	SRC=${THIRDPARTY}/libmpg123
# 	BUILD=${THIRDPARTY}/libmpg123/build
# 	if [ ! -f ${THIRDPARTY}/install/lib/libmpg123.a ]; then
# 		rm -rf ${THIRDPARTY}/libmpg123 && \
# 		mkdir -p build/thirdparty && \
# 		git clone --depth=1 https://github.com/gypified/libmpg123.git ${THIRDPARTY}/libmpg123 && \
# 		cd ${THIRDPARTY}/libmpg123 && \
# 		CFLAGS="-Os -s" ./configure \
# 			--with-cpu=generic  \
# 			--disable-id3v2 \
# 			--disable-lfs-alias \
# 			--disable-feature-report \
# 			--with-seektable=0 \
# 			--disable-16bit \
# 			--disable-32bit \
# 			--disable-8bit \
# 			--disable-messages \
# 			--disable-feeder \
# 			--disable-ntom \
# 			--disable-downsample \
# 			--disable-icy \
# 			--enable-static \
# 			--prefix=${PREFIX} && \
# 		make && \
# 		make install 
# 	fi
# }

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
			--prefix=${PREFIX} && \
		make && \
		make install 
	fi
}

# patch a per homebrew build
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

function install_libsamplerate() {
	SRC=${THIRDPARTY}/libsamplerate
	BUILD=${THIRDPARTY}/libsamplerate/build
	if [ ! -f ${THIRDPARTY}/install/lib/libsamplerate.a ]; then
		rm -rf ${THIRDPARTY}/libsamplerate && \
		mkdir -p ${THIRDPARTY} && \
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

function install_fluidsynth() {
	SRC=${THIRDPARTY}/fluidsynth
	if [ ! -f ${THIRDPARTY}/install/lib/libfluidsynth.a ]; then
		rm -rf ${THIRDPARTY}/fluidsynth && \
		mkdir -p ${THIRDPARTY} && \
		git clone --depth=1 https://github.com/FluidSynth/fluidsynth.git ${THIRDPARTY}/fluidsynth && \
		cd ${SRC} && \
		mkdir build && cd build && \
		cmake .. \
			-DBUILD_SHARED_LIBS=OFF \
			-Denable-framework=OFF \
			-Denable-readline=OFF \
			-Denable-sdl2=OFF && \
		cmake --build . --config Release && \
		cmake --install . --prefix ${PREFIX}
	fi
}

setup && \
	install_libmpg123 && \
	install_libmp3lame && \
	install_libopus && \
	install_libvorbis && \
	install_libflac && \
	install_libogg && \
	install_faust && \
	install_libfaust && \
	install_libsndfile && \
	install_rubberband && \
	install_libsamplerate && \
	install_fluidsynth
