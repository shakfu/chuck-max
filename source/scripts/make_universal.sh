#!/usr/bin/env sh

CWD=`pwd`
BUILD=${CWD}/build


function get_dmg() {
	ARCH=$1
	VER=$2
	mkdir -p ${BUILD}
	if [ ! -f ${BUILD}/chuck-max-${VER}-darwin-${ARCH}.dmg ]; then
		curl -L  https://github.com/shakfu/chuck-max/releases/download/${VER}/chuck-max-${VER}-darwin-${ARCH}.dmg -o ${BUILD}/chuck-max-${VER}-darwin-${ARCH}.dmg
	fi
	hdiutil attach ${BUILD}/chuck-max-${VER}-darwin-${ARCH}.dmg
	rm -rf ${BUILD}/darwin-${ARCH}
	mkdir -p ${BUILD}/darwin-${ARCH}
	cp -R /Volumes/chuck-max-${VER}-darwin-${ARCH}/chuck-max ${BUILD}/darwin-${ARCH}/chuck-max
	hdiutil detach /Volumes/chuck-max-${VER}-darwin-${ARCH}
}

function mk_universal_chugin() {
	echo "making universal chugin $1"
	lipo \
		${BUILD}/darwin-arm64/chuck-max/examples/chugins/$1 \
		${BUILD}/darwin-x86_64/chuck-max/examples/chugins/$1 -create -output \
		${BUILD}/darwin-universal/chuck-max/examples/chugins/$1
}

function mk_universal_external() {
	echo "making universal external chuck~.mxo"
	lipo \
		${BUILD}/darwin-arm64/chuck-max/externals/chuck\~.mxo/Contents/MacOS/chuck\~  \
		${BUILD}/darwin-x86_64/chuck-max/externals/chuck\~.mxo/Contents/MacOS/chuck\~  -create -output \
		${BUILD}/darwin-universal/chuck-max/externals/chuck\~.mxo/Contents/MacOS/chuck\~ 
}

function mk_universal_package() {
	VER=$1
	get_dmg arm64 ${VER} && \
	get_dmg x86_64 ${VER} && \
	rm -rf ${BUILD}/darwin-universal && \
	cp -af ${BUILD}/darwin-arm64 ${BUILD}/darwin-universal && \
	mk_universal_chugin ABSaturator.chug && \
	mk_universal_chugin AmbPan.chug && \
	mk_universal_chugin Binaural.chug && \
	mk_universal_chugin Bitcrusher.chug && \
	mk_universal_chugin Elliptic.chug && \
	mk_universal_chugin ExpDelay.chug && \
	mk_universal_chugin ExpEnv.chug && \
	mk_universal_chugin FIR.chug && \
	mk_universal_chugin Faust.chug && \
	mk_universal_chugin FoldbackSaturator.chug && \
	mk_universal_chugin GVerb.chug && \
	mk_universal_chugin KasFilter.chug && \
	mk_universal_chugin MIAP.chug && \
	mk_universal_chugin MagicSine.chug && \
	mk_universal_chugin Mesh2D.chug && \
	mk_universal_chugin Multicomb.chug && \
	mk_universal_chugin NHHall.chug && \
	mk_universal_chugin Overdrive.chug && \
	mk_universal_chugin PanN.chug && \
	mk_universal_chugin Patch.chug && \
	mk_universal_chugin Perlin.chug && \
	mk_universal_chugin PitchTrack.chug && \
	mk_universal_chugin PowerADSR.chug && \
	mk_universal_chugin Random.chug && \
	mk_universal_chugin Range.chug && \
	mk_universal_chugin RegEx.chug && \
	mk_universal_chugin Sigmund.chug && \
	mk_universal_chugin Spectacle.chug && \
	mk_universal_chugin WPDiodeLadder.chug && \
	mk_universal_chugin WPKorg35.chug && \
	mk_universal_chugin WarpBuf.chug && \
	mk_universal_chugin Wavetable.chug && \
	mk_universal_chugin WinFuncEnv.chug && \
	mk_universal_external
}

#mk_universal_chugin 0.1.1

mk_universal_chugin $1

echo "NOTE: the resulting universal package need to be re-codesigned and notarized!"

