CHUCK_PACKAGE := "$(HOME)/Documents/Max\ 8/Packages/chuck-max"
SCRIPTS := source/scripts
BUILD := build


.PHONY: all native universal dev clean setup 

all: native

native:
	@mkdir -p build && cd build && cmake -GXcode .. && cmake --build . --config 'Release' && cmake --install . --config 'Release'

universal:
	@mkdir -p build && cd build && cmake -GXcode -DC74_BUILD_FAT=ON .. && cmake --build . --config 'Release' && cmake --install . --config 'Release'

dev:
	@mkdir -p build && cd build && cmake .. && make

clean:
	@rm -rf build

setup:
	@git submodule init
	@git submodule update
	@if ! [ -L "$(CHUCK_PACKAGE)" ]; then \
		ln -s "$(shell pwd)" "$(CHUCK_PACKAGE)" ; \
		echo "... symlink created" ; \
	else \
		echo "... symlink already exists" ; \
	fi
