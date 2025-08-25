CWD=`pwd`
THIRDPARTY=${CWD}/build/thirdparty
PREFIX=${THIRDPARTY}/install
DOWNLOADS=${THIRDPARTY}/downloads
#FAUST_VERSION=2.69.3
FAUST_VERSION=2.72.14
#FAUST_VERSION=2.81.2

function setup() {
	mkdir -p ${DOWNLOADS} && \
	mkdir -p ${PREFIX}/include && \
	mkdir -p ${PREFIX}/lib
}


function install_libfaust() {
	VERSION=${FAUST_VERSION}
	if [ "$(uname)" = "Darwin" ]; then
	    if [ ! -f ${THIRDPARTY}/libfaust/lib/libfaustwithllvm.a ]; then
	    	rm -rf ${THIRDPARTY}/libfaust
			if [ "$(uname -m)" = "arm64" ]; then
				if [ ! -f ${DOWNLOADS}/Faust-$VERSION-arm64.dmg ]; then
					curl -L https://github.com/grame-cncm/faust/releases/download/$VERSION/Faust-$VERSION-arm64.dmg -o ${DOWNLOADS}/Faust-$VERSION-arm64.dmg
				fi
				hdiutil attach ${DOWNLOADS}/Faust-$VERSION-arm64.dmg
				mkdir -p ${THIRDPARTY}/libfaust
				cp -Rf /Volumes/Faust-$VERSION/Faust-$VERSION/* ${THIRDPARTY}/libfaust/
				hdiutil detach /Volumes/Faust-$VERSION/
				# rm -f Faust-$VERSION-arm64.dmg
			else
				if [ ! -f ${DOWNLOADS}/Faust-$VERSION-x64.dmg ]; then
					curl -L https://github.com/grame-cncm/faust/releases/download/$VERSION/Faust-$VERSION-x64.dmg -o ${DOWNLOADS}/Faust-$VERSION-x64.dmg
				fi
				hdiutil attach ${DOWNLOADS}/Faust-$VERSION-x64.dmg
				mkdir -p ${THIRDPARTY}/libfaust
				cp -Rf /Volumes/Faust-$VERSION/Faust-$VERSION/* ${THIRDPARTY}/libfaust/
				hdiutil detach /Volumes/Faust-$VERSION/
				# rm -f Faust-$VERSION-x64.dmg
			fi
		fi
	fi
}


setup && install_libfaust