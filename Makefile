CHUCK_PACKAGE := "$(HOME)/Documents/Max\ 8/Packages/chuck-max"
SCRIPTS := source/scripts
BUILD := build
CONFIG = Release
THIRDPARTY = $(BUILD)/thirdparty
LIB = $(THIRDPARTY)/install/lib
CHUCK = $(THIRDPARTY)/install/bin/chuck
DIST = $(BUILD)/dist/chuck-max


.PHONY: all native universal full light dev clean reset setup test   \
		test-fauck test-warpbuf install_deps install_deps_light brew

all: native

native:
	@mkdir -p build && \
		cd build && \
		cmake -GXcode .. && \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

universal:
	@mkdir -p build && \
		cd build && \
		cmake -GXcode -DC74_BUILD_FAT=ON .. && \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'


install_deps:
	./source/scripts/install_deps.sh

install_deps_light:
	./source/scripts/install_deps_light.sh


install_faust:
	./source/scripts/dep/install_faust.sh
	./source/scripts/dep/install_libfaust.sh

brew: install_faust
	@mkdir -p build && \
		cd build && \
		cmake -GXcode .. -DENABLE_HOMEBREW=ON -DENABLE_WARPBUF=ON -DENABLE_FAUCK=ON && \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'


full: install_deps
	@mkdir -p build && \
		cd build && \
		cmake -GXcode .. -DENABLE_WARPBUF=ON -DENABLE_FAUCK=ON && \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

light: install_deps_light
	@mkdir -p build && \
		cd build && \
		cmake -GXcode .. -DENABLE_WARPBUF=ON -DENABLE_FAUCK=ON && \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

package:
	@rm -rf $(DIST) && \
		mkdir -p $(DIST) && \
		cp -af examples $(DIST)/examples && \
		cp -af externals $(DIST)/externals && \
		cp -rf help $(DIST)/help && \
		cp -rf patchers $(DIST)/patchers && \
		cp -f package-info.json $(DIST)/package-info.json && \
		cp -f LICENSE $(DIST)/LICENSE && \
		cp -f CHANGELOG.md $(DIST)/CHANGELOG.md && \
		cp -f README.md $(DIST)/README.md && \
		find $(DIST) -name ".DS_Store" -delete && \
		cd build/dist && \
		zip -T -9 -r chuck-max.zip  chuck-max && \
		echo "DONE"


link:
	@cd examples && rm -f chuck && ln -s ../build/$(CONFIG)/chuck .

dev:
	@mkdir -p build && cd build && cmake .. && make

clean:
	@rm -rf \
		build/$(CONFIG) \
		build/*.ck \
		build/*.cmake \
		build/*.xcodeproj \
		build/build \
		build/CMake* \
		build/install* \
		build/package \
		build/source \
		externals

reset:
	@rm -rf externals
	@rm -rf build/CMakeCache.txt build/CMakeFiles build/CMakeScripts build/Release build/build build/sine.ck
	@rm -rf build/chuck-max.xcodeproj build/cmake_install.cmake build/install_manifest.txt build/source build/build
	@rm -rf build/thirdparty/faust build/thirdparty/install build/thirdparty/libfaust

setup:
	@git submodule init
	@git submodule update
	@if ! [ -L "$(CHUCK_PACKAGE)" ]; then \
		ln -s "$(shell pwd)" "$(CHUCK_PACKAGE)" ; \
		echo "... symlink created" ; \
	else \
		echo "... symlink already exists" ; \
	fi

test:
	@cd examples && ./chuck --chugin-path:chugins stk/rhodey-song.ck -v3

test-fauck:
	@cd examples && ./chuck --chugin-path:chugins fauck/test.ck -v3

test-warpbuf:
	@cd examples && ./chuck --chugin-path:chugins warpbuf/warpbuf_basic.ck -v3


