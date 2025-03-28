CWD=`pwd`
THIRDPARTY=${CWD}/build/thirdparty
PREFIX=${THIRDPARTY}/install

function install_brew_static_dep() {
	base=`brew --prefix $1`
	lib=${base}/lib
	include=${base}/include
	cp ${lib}/*.a ${PREFIX}/lib/
	cp -rf ${include}/* ${PREFIX}/include/
}

install_brew_static_dep portaudio
install_brew_static_dep glib
install_brew_static_dep gettext
