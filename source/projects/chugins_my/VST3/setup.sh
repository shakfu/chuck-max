CWD=`pwd`
BRANCH="v3.8.0_build_66"
VST_SDK_DIR=VST_SDK/vst3sdk

# cd ${CWD}

if [ ! -d "VST_SDK" ]; then
	mkdir -p VST_SDK
fi

cd VST_SDK/

if [ ! -d "vst3sdk" ]; then
	git clone -b ${BRANCH} --depth=1 --recursive https://github.com/steinbergmedia/vst3sdk
fi

cd ${CWD}


# cd vst3sdk && mkdir build && cd build && \
# cmake .. -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" && \
# cmake --build . --config Release
