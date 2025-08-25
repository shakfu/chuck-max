CWD=`pwd`
THIRDPARTY=${CWD}/build/thirdparty
PREFIX=${THIRDPARTY}/install
#FAUST_VERSION=2.69.3
FAUST_VERSION=2.72.14
# FAUST_VERSION=2.81.2 	# (latest)

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

setup && install_faust
