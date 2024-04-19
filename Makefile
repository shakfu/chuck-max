CHUCK_PACKAGE := "$(HOME)/Documents/Max\ 8/Packages/chuck-max"
SCRIPTS := source/scripts
BUILD := build
CONFIG = Release
THIRDPARTY = $(BUILD)/thirdparty
LIB = $(THIRDPARTY)/install/lib
CHUCK = $(THIRDPARTY)/install/bin/chuck



.PHONY: all native full universal fauck dev clean reset setup test test-fauck test-warpbuf

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

full: install_deps
	@mkdir -p build && \
		cd build && \
		cmake -GXcode .. -DENABLE_WARPBUF=ON -DENABLE_FAUCK=ON && \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

fauck:
	@mkdir -p build && \
		cd build && \
		cmake -GXcode .. -DENABLE_FAUCK=ON && \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

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
	@rm -rf build externals

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
	@cd examples && ./chuck --chugin-path:chugins stk/rhodey-song.ck

test-fauck:
	@cd examples && ./chuck --chugin-path:chugins fauck/test.ck

test-warpbuf:
	@cd examples && ./chuck --chugin-path:chugins warpbuf/warpbuf_basic.ck


