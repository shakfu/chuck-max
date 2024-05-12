#!/bin/sh

CWD=`pwd`
BUILD=${CWD}/build
THIRDPARTY=${BUILD}/thirdparty
VERSION=2.72.14

mkdir -p ${BUILD}

if [ "$(uname)" = "Darwin" ]; then
    echo "You are running macOS"
    if [ ! -f ${THIRDPARTY}/darwin-arm64/Release/lib/libfaustwithllvm.a ]; then
        curl -L https://github.com/grame-cncm/faust/releases/download/$VERSION/Faust-$VERSION-arm64.dmg -o ${BUILD}/Faust-$VERSION-arm64.dmg
        hdiutil attach ${BUILD}/Faust-$VERSION-arm64.dmg
        mkdir -p "${THIRDPARTY}/darwin-arm64/Release"
        cp -R /Volumes/Faust-$VERSION/Faust-$VERSION/* ${THIRDPARTY}/darwin-arm64/Release/
        hdiutil detach /Volumes/Faust-$VERSION/
    fi
    
    if [ ! -f ${THIRDPARTY}/darwin-x64/Release/lib/libfaustwithllvm.a ]; then
        curl -L https://github.com/grame-cncm/faust/releases/download/$VERSION/Faust-$VERSION-x64.dmg -o ${BUILD}/Faust-$VERSION-x64.dmg
        hdiutil attach ${BUILD}/Faust-$VERSION-x64.dmg
        mkdir -p "${THIRDPARTY}/darwin-x64/Release"
        cp -R /Volumes/Faust-$VERSION/Faust-$VERSION/* ${THIRDPARTY}/darwin-x64/Release/
        hdiutil detach /Volumes/Faust-$VERSION/
    fi
elif [ "$(expr substr $(uname -s) 1 5)" = "Linux" ]; then
    echo "You are running Linux"
    if [ ! -f libfaust-ubuntu-x86_64.zip ]; then
        curl -L https://github.com/grame-cncm/faust/releases/download/$VERSION/libfaust-ubuntu-x86_64.zip -o libfaust-ubuntu-x86_64.zip
        mkdir -p "${THIRDPARTY}/ubuntu-x86_64/Release"
        unzip libfaust-ubuntu-x86_64.zip -d ubuntu-x86_64/Release
    fi
elif [ "$(expr substr $(uname -s) 1 10)" = "MINGW32_NT" ] || [ "$(expr substr $(uname -s) 1 10)" = "MINGW64_NT" ]; then
    echo "You are running Windows. You should run \"call download_libfaust.bat\"" >&2
    exit 1
else
    echo "Unknown operating system" >&2
    exit 1
fi

if [ ! -f ${THIRDPARTY}/darwin-universal/Release/lib/libfaustwithllvm.a ]; then
    # copy arm64 into universal just to use the headers
    rm -rf ${THIRDPARTY}/darwin-universal
    cp -r ${THIRDPARTY}/darwin-arm64 ${THIRDPARTY}/darwin-universal

    # combine the static libfaust.a from x64 and arm64 into a universal library.
    lipo \
        ${THIRDPARTY}/darwin-x64/Release/lib/libfaustwithllvm.a \
        ${THIRDPARTY}/darwin-arm64/Release/lib/libfaustwithllvm.a -create -output \
        ${THIRDPARTY}/darwin-universal/Release/lib/libfaustwithllvm.a
fi