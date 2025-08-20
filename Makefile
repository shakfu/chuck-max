# os of current system
PLATFORM = $(shell uname)
MAX_VERSION := 9
CHUCK_PACKAGE := "$(HOME)/Documents/Max\ $(MAX_VERSION)/Packages/chuck-max"
SCRIPTS := source/scripts
BUILD := build
CONFIG = Release
THIRDPARTY = $(BUILD)/thirdparty
LIB = $(THIRDPARTY)/install/lib
CHUCK = $(THIRDPARTY)/install/bin/chuck
DIST = $(BUILD)/dist/chuck-max
ARCH=$(shell uname -m)
DMG=chuck-max-$(VERSION)-$(ARCH).dmg
ENTITLEMENTS = source/scripts/entitlements.plist
VERSION=0.1.2

ifeq ($(PLATFORM), Darwin)
GENERATOR ?= "-GXcode"
endif

.PHONY: all native universal full light brew brew2 nomp3 dev setup \
		clean reset test test-fauck test-warpbuf test-fluidsynth \
		install_deps install_deps_light install_deps_nomp3 \
		full2 install_fs_deps chump \
		sign package dmg sign-dmg notarize staple sign-dist \
		release dist-release

all: native

native: 
	@mkdir -p build && \
		cd build && \
		cmake $(GENERATOR) .. && \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

universal: 
	@mkdir -p build && \
		cd build && \
		cmake $(GENERATOR) -DBUILD_UNIVERSAL=ON .. && \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

install_deps:
	./source/scripts/install_deps.sh

install_deps_nomp3:
	./source/scripts/install_deps_nomp3.sh

install_deps_light:
	./source/scripts/install_deps_light.sh

install_faust: 
	./source/scripts/dep/install_faust.sh
	./source/scripts/dep/install_libfaust.sh

install_soundfonts:
	./source/scripts/download_sf2.sh

install_fs_deps:
	./source/scripts/install_fluidsynth_deps.sh

brew-lite: 
	@mkdir -p build && \
		cd build && \
		cmake $(GENERATOR) .. \
			-DENABLE_HOMEBREW=ON \
			-DENABLE_EXTRA_FORMATS=ON \
			-DENABLE_MP3=ON \
			-DENABLE_WARPBUF=ON \
			-DENABLE_FLUIDSYNTH=ON \
			&& \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

brew: install_faust 
	@mkdir -p build && \
		cd build && \
		cmake $(GENERATOR) .. \
			-DENABLE_HOMEBREW=ON \
			-DENABLE_EXTRA_FORMATS=ON \
			-DENABLE_MP3=ON \
			-DENABLE_WARPBUF=ON \
			-DENABLE_FAUCK=ON \
			-DENABLE_FLUIDSYNTH=ON \
			&& \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

full: install_deps
	@mkdir -p build && \
		cd build && \
		cmake $(GENERATOR) .. \
			-DENABLE_EXTRA_FORMATS=ON \
			-DENABLE_MP3=ON \
			-DENABLE_WARPBUF=ON \
			-DENABLE_FAUCK=ON
			&& \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

full2: install_deps install_fs_deps 
	@mkdir -p build && \
		cd build && \
		cmake $(GENERATOR) .. \
			-DENABLE_EXTRA_FORMATS=ON \
			-DENABLE_MP3=ON \
			-DENABLE_WARPBUF=ON \
			-DENABLE_FAUCK=ON \
			-DENABLE_FLUIDSYNTH=ON \
			&& \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

nomp3: install_deps_nomp3
	@mkdir -p build && \
		cd build && \
		cmake $(GENERATOR) .. \
			-DENABLE_EXTRA_FORMATS=ON \
			-DENABLE_MP3=OFF \
			-DENABLE_WARPBUF=ON \
			-DENABLE_FAUCK=ON && \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

light: install_deps_light
	@mkdir -p build && \
		cd build && \
		cmake $(GENERATOR) .. \
			-DENABLE_EXTRA_FORMATS=OFF \
			-DENABLE_MP3=OFF \
			-DENABLE_WARPBUF=ON \
			-DENABLE_FAUCK=ON && \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

chump:
	@mkdir -p build && cd build && \
		cmake $(GENERATOR) .. \
			-DENABLE_HOMEBREW=ON \
			-DBUILD_CHUMP=ON && \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

strip:
	@strip -u -r externals/chuck\~.mxo/Contents/MacOS/chuck\~
	@strip -x examples/chugins/*.chug


sign:
	@codesign --sign 'Developer ID Application: $(DEV_ID)' \
		--timestamp --deep --force externals/chuck\~.mxo/Contents/MacOS/chuck\~ && \
		codesign --sign 'Developer ID Application: $(DEV_ID)' \
			--timestamp --deep --force --options runtime \
			--entitlements $(ENTITLEMENTS) externals/chuck\~.mxo && \
		codesign --verify externals/chuck\~.mxo && \
		codesign --verify externals/chuck\~.mxo/Contents/MacOS/chuck\~ && \
		codesign --sign 'Developer ID Application: $(DEV_ID)' \
			--timestamp --deep --force examples/chugins/*.chug && \
		codesign --verify examples/chugins/*.chug

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
		rm -f $(DIST)/examples/chuck && \
		find $(DIST) -name ".DS_Store" -delete && \
		echo "DONE"

sign-dist:
	@codesign --sign 'Developer ID Application: $(DEV_ID)' \
		--timestamp --deep --force $(DIST)/externals/chuck\~.mxo/Contents/MacOS/chuck\~ && \
		codesign --sign 'Developer ID Application: $(DEV_ID)' \
			--timestamp --deep --force --options runtime \
			--entitlements $(ENTITLEMENTS) $(DIST)/externals/chuck\~.mxo && \
		codesign --verify $(DIST)/externals/chuck\~.mxo && \
		codesign --verify $(DIST)/externals/chuck\~.mxo/Contents/MacOS/chuck\~ && \
		codesign --sign 'Developer ID Application: $(DEV_ID)' \
			--timestamp --deep --force $(DIST)/examples/chugins/*.chug && \
		codesign --verify $(DIST)/examples/chugins/*.chug

dmg:
	@hdiutil create -volname CHUCK-MAX -srcfolder $(BUILD)/dist -ov -format UDBZ $(DMG)

sign-dmg:
	@codesign --sign "Developer ID Application: $(DEV_ID)" --deep --force --verbose --options runtime "$(DMG)" && \
		codesign --verify --verbose $(DMG)

notarize:
	@xcrun notarytool submit "$(DMG)" --keychain-profile "$(KEYCHAIN_PROFILE)" --wait

staple:
	@xcrun stapler staple "$(DMG)"

release: strip sign package dmg sign-dmg notarize staple
	@echo "DONE"

dist-release: sign-dist dmg sign-dmg notarize staple
	@echo "DONE"


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
	@rm -rf externals examples/chugins/*.chug
	@rm -rf build/CMakeCache.txt build/CMakeFiles build/CMakeScripts build/Release build/build build/sine.ck
	@rm -rf build/chuck-max.xcodeproj build/cmake_install.cmake build/install_manifest.txt build/source build/build
	@rm -rf build/thirdparty/faust build/thirdparty/install build/thirdparty/libfaust build/dist

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

test-fluidsynth:
	@cd examples && ./chuck --chugin-path:chugins fluidsynth/FluidSynth-test.ck -v3

