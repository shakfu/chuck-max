CHUCK_PACKAGE := "$(HOME)/Documents/Max\ 8/Packages/chuck-max"
SCRIPTS := source/scripts
BUILD := build
CONFIG = Release
THIRDPARTY = $(BUILD)/thirdparty
LIB = $(THIRDPARTY)/install/lib
CHUCK = $(THIRDPARTY)/install/bin/chuck



.PHONY: all native universal full light fauck warpbuf dev clean reset setup test \
		test-fauck test-warpbuf install_deps install_deps_light install_fauck_deps \
		install_warpbuf_deps

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

install_fauck_deps:
	./source/scripts/install_fauck_deps.sh

fauck: install_fauck_deps
	@mkdir -p build && \
		cd build && \
		cmake -GXcode .. -DENABLE_FAUCK=ON && \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

install_warpbuf_deps:
	./source/scripts/install_warpbuf_deps.sh

warbuf: install_warpbuf_deps
	@mkdir -p build && \
		cd build && \
		cmake -GXcode .. -DENABLE_WARPBUF=ON && \
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


