# chugin name
CHUGIN_NAME=ChuGL
# suffice
CHUGIN_SUFFIX=.chug

# ---------------------------------------------------------------------------- #
# you won't generally need to change anything below this line for a new chugin #
# ---------------------------------------------------------------------------- #

CHUG=$(addsuffix $(CHUGIN_SUFFIX),$(CHUGIN_NAME))

########################## DEFAULT MAKE TARGET ################################
# default target: print usage message and quit
current:
	@echo "[chugl build]: please use one of the following configurations:"
	@echo "   make mac, make linux, make win"

# creates build-release
build-release:
	cmake -B build-release -DCMAKE_BUILD_TYPE=Release

build-debug:
	cmake -B build-debug -DCMAKE_BUILD_TYPE=Debug

.PHONY: mac osx linux linux-oss linux-jack linux-alsa linux-all
mac osx linux linux-oss linux-jack linux-alsa linux-all: build-release
	cmake --build build-release
	cp build-release/ChuGL.chug .

# generate XCode project in build-xcode
xcode:
	cmake -G Xcode -B build-xcode

.PHONY: win win32 win64
win win32 win64: build-release
	@echo '------------'
	@echo '(windows) --> build ChuGL.chug in Visual Studio from build-release/chugl.sln'

debug: build-debug
	cmake --build build-debug
	cp build-debug/ChuGL.chug .

test: build-release
	test/test.py

# (mac/linux) remove system installed ChuGL and install local ChuGL
test-install:
	@echo "[removing installed ChuGL.chug...]"
ifneq (,$(wildcard /usr/local/lib/chuck/ChuGL.chug))
	sudo rm -f /usr/local/lib/chuck/ChuGL.chug
endif
ifneq (,$(wildcard ~/.chuck/lib/ChuGL.chug))
	rm -f ~/.chuck/lib/ChuGL.chug
endif
	@echo "[copying ChuGL.chug to local search path...]"
	cp ChuGL.chug ~/.chuck/lib
	@echo "[probing chugins...]"
	@chuck --color --chugin-probe 2>&1 >/dev/null | grep ChuGL.chug

clean:
ifneq ("$(wildcard build-release)","")
	cmake --build build-release --target clean
endif
ifneq ("$(wildcard build-debug)","")
	cmake --build build-debug --target clean
endif

clean-all: 
	rm -rf $(CHUG) build-release build-debug build-xcode
