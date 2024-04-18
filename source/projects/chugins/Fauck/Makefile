# Adapted from https://github.com/ccrma/chugl/blob/main/src/makefile

# chugin name
CHUGIN_NAME=Faust
# suffix
CHUGIN_SUFFIX=.chug

CMAKE_OSX_ARCHITECTURES ?= 

# Determine the OS and architecture
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# Set LIBFAUST_DIR based on OS and architecture
ifeq ($(UNAME_S), Darwin)
    ifeq ($(UNAME_M), arm64)
        LIBFAUST_DIR ?= $(PWD)/thirdparty/libfaust/darwin-arm64/Release
    else
        LIBFAUST_DIR ?= $(PWD)/thirdparty/libfaust/darwin-x64/Release
    endif
else ifeq ($(OS),Windows_NT)
	LIBFAUST_DIR ?= thirdparty/libfaust/win64/Release
else
    LIBFAUST_DIR ?= $(PWD)/thirdparty/libfaust/ubuntu-x86_64/Release
endif

CHUG=$(addsuffix $(CHUGIN_SUFFIX),$(CHUGIN_NAME))

########################## DEFAULT MAKE TARGET ################################
# default target: print usage message and quit
current:
	@echo "[fauck build]: please use one of the following configurations:"
	@echo "   make mac, make linux, make win"

.PHONY: libfaust
libfaust:
	@OS="`uname`"; \
	if echo "$$OS" | grep -qE 'CYGWIN_NT|MINGW32_NT|MINGW64_NT'; then \
		cd thirdparty/libfaust && cmd /C download_libfaust.bat; \
	else \
		cd thirdparty/libfaust && sh download_libfaust.sh; \
	fi

.PHONY: libsndfile
libsndfile:
	@OS="`uname`"; \
	if echo "$$OS" | grep -qE 'CYGWIN_NT|MINGW32_NT|MINGW64_NT'; then \
		cd thirdparty && cmd /C download_libsndfile.bat; \
	else \
		cd thirdparty/libsndfile && \
		cmake -Bbuild -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_OSX_ARCHITECTURES=$(CMAKE_OSX_ARCHITECTURES) -DCMAKE_C_FLAGS="-fPIC" -DCMAKE_CXX_FLAGS="-fPIC" -DBUILD_TESTING=OFF -DCMAKE_INSTALL_PREFIX="./install" && \
		cmake --build build --config Release && \
		cmake --build build --target install; \
	fi

build-release: libfaust libsndfile
	cmake -DLIBFAUST_DIR=$(LIBFAUST_DIR) -B build-release -DCMAKE_BUILD_TYPE=Release

build-debug: libfaust libsndfile
	cmake -DLIBFAUST_DIR=$(LIBFAUST_DIR) -B build-debug -DCMAKE_BUILD_TYPE=Debug

.PHONY: mac osx linux windows win win32 win64
mac osx linux windows win win32 win64: build-release
	cmake --build build-release --config Release --target install

debug: build-debug
	cmake --build build-debug
	cp build-debug/Faust.chug .

# (mac/linux) remove system installed FaucK and install local FaucK
test-install:
	@echo "[removing installed Faust.chug...]"
ifneq (,$(wildcard /usr/local/lib/chuck/Faust.chug))
	sudo rm -rf /usr/local/lib/chuck/Faust.chug
endif
ifneq (,$(wildcard ~/.chuck/lib/Faust.chug))
	rm -rf ~/.chuck/lib/Faust.chug
endif
	@echo "[copying Faust.chug to local search path...]"
	cp -r Faust.chug ~/.chuck/lib
	@echo "[probing chugins...]"
	@chuck --color --chugin-probe 2>&1 >/dev/null | grep Faust.chug

clean:
ifneq ("$(wildcard build-release)","")
	cmake --build build-release --target clean
endif
ifneq ("$(wildcard build-debug)","")
	cmake --build build-debug --target clean
endif

clean-all: 
	rm -rf $(CHUG) build-release build-debug