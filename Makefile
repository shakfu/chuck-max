CHUCK_PACKAGE := "$(HOME)/Documents/Max\ 8/Packages/chuck-max"
SCRIPTS := source/scripts
BUILD := build


.PHONY: cmake clean setup 

all: cmake

cmake:
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