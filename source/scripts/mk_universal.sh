#!/usr/bin/env sh

# reset
rm -rf build/merge/arm64
rm -rf build/merge/universal

mkdir -p build/merge/arm64
mkdir -p build/merge/x64
mkdir -p build/merge/universal

cp -a externals/chuck\~.mxo build/merge/arm64
cp -a examples/chugins/*.chug build/merge/arm64
cp -a externals/chuck\~.mxo build/merge/universal

function mk_universal_chugin() {
	echo "making universal chugin $1"
	lipo \
		build/merge/arm64/$1 \
		build/merge/x64/$1 -create -output \
		build/merge/universal/$1
}

function mk_universal_external() {
	echo "making universal external chuck~.mxo"
	lipo \
		build/merge/arm64/chuck\~.mxo/Contents/MacOS/chuck\~  \
		build/merge/x64/chuck\~.mxo/Contents/MacOS/chuck\~  -create -output \
		build/merge/universal/chuck\~.mxo/Contents/MacOS/chuck\~ 
}

for path in build/merge/arm64/*.chug; do
	name=`basename ${path}`
	mk_universal_chugin ${name}
	echo "created universal: ${name}"
done

echo "verify --------------------"

for path in build/merge/universal/*.chug; do
	lipo -info ${path}
done

mk_universal_external 
lipo -info build/merge/universal/chuck\~.mxo/Contents/MacOS/chuck\~


