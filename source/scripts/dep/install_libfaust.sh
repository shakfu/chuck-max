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


setup && install_libfaust