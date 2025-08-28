VERSION=0.2.0

PLATFORM = $(shell uname)
MAX_VERSION := 9
CHUCK_PACKAGE := "$(HOME)/Documents/Max\ $(MAX_VERSION)/Packages/chuck-max"
SCRIPTS := source/scripts
ROOT := $(PWD)
BUILD := build
CONFIG = Release
THIRDPARTY = $(BUILD)/thirdparty
LIB = $(THIRDPARTY)/install/lib
CHUCK = $(THIRDPARTY)/install/bin/chuck
DIST = $(BUILD)/dist/chuck-max
ARCH=$(shell uname -m)
ZIP=chuck-max-$(VERSION)-$(ARCH).zip
DMG=chuck-max-$(VERSION)-$(ARCH).dmg
ENTITLEMENTS = source/scripts/entitlements.plist
CHUGINS_DIR = examples/chugins

# variants
BUNDLED=0
MULTI=0
UNIVERSAL=0

ifeq ($(PLATFORM), Darwin)
GENERATOR ?= "-GXcode"
ifeq ($(UNIVERSAL), 1)
EXTRA_OPTIONS += -DCM_MACOS_UNIVERSAL=ON
endif
ifeq ($(BUNDLED), 1)
EXTRA_OPTIONS += -DCM_MACOS_BUNDLED_CHUGINS=ON
CHUGINS_DIR = externals/chuck\~.mxo/Contents/Resources/chugins
endif
endif

ifeq ($(MULTI), 1)
EXTRA_OPTIONS += -DCM_MULTIPLATFORM_CHUGINS=ON
endif


.PHONY: all native multi universal full light brew brew-lite nomp3 dev setup \
		clean reset test test-fauck test-warpbuf test-fluidsynth \
		install_deps install_deps_light install_deps_nomp3 \
		full2 install_fs_deps chump \
		sign package dmg zip-dist sign-dmg notarize staple sign-dist sign-bundle \
		release dist-release

all: native

native: 
	@mkdir -p build && \
		cd build && \
		cmake $(GENERATOR) .. $(EXTRA_OPTIONS) && \
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
		cmake $(GENERATOR) .. $(EXTRA_OPTIONS) \
			-DCM_MACOS_HOMEBREW=ON \
			-DCM_EXTRA_FORMATS=ON \
			-DCM_MP3=ON \
			-DCM_WARPBUF=ON \
			-DCM_FLUIDSYNTH=ON \
			&& \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

brew: install_faust 
	@mkdir -p build && \
		cd build && \
		cmake $(GENERATOR) .. $(EXTRA_OPTIONS) \
			-DCM_MACOS_HOMEBREW=ON \
			-DCM_EXTRA_FORMATS=ON \
			-DCM_MP3=ON \
			-DCM_WARPBUF=ON \
			-DCM_FAUST=ON \
			-DCM_FLUIDSYNTH=ON \
			&& \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

full: install_deps
	@mkdir -p build && \
		cd build && \
		cmake $(GENERATOR) .. $(EXTRA_OPTIONS) \
			-DCM_EXTRA_FORMATS=ON \
			-DCM_MP3=ON \
			-DCM_WARPBUF=ON \
			-DCM_FAUST=ON
			&& \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

full2: install_deps install_fs_deps 
	@mkdir -p build && \
		cd build && \
		cmake $(GENERATOR) .. $(EXTRA_OPTIONS) \
			-DCM_EXTRA_FORMATS=ON \
			-DCM_MP3=ON \
			-DCM_WARPBUF=ON \
			-DCM_FAUST=ON \
			-DCM_FLUIDSYNTH=ON \
			&& \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

nomp3: install_deps_nomp3
	@mkdir -p build && \
		cd build && \
		cmake $(GENERATOR) .. \
			-DCM_EXTRA_FORMATS=ON \
			-DCM_MP3=OFF \
			-DCM_WARPBUF=ON \
			-DCM_FAUST=ON && \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

light: install_deps_light
	@mkdir -p build && \
		cd build && \
		cmake $(GENERATOR) .. \
			-DCM_EXTRA_FORMATS=OFF \
			-DCM_MP3=OFF \
			-DCM_WARPBUF=ON \
			-DCM_FAUST=ON && \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

chump:
	@mkdir -p build && cd build && \
		cmake $(GENERATOR) .. \
			-DCM_MACOS_HOMEBREW=ON \
			-DCM_CHUMP=ON && \
		cmake --build . --config '$(CONFIG)' && \
		cmake --install . --config '$(CONFIG)'

strip:
	@strip -u -r externals/chuck\~.mxo/Contents/MacOS/chuck\~
	@strip -x $(CHUGINS_DIR)/*.chug

sign:
	@codesign --sign 'Developer ID Application: $(DEV_ID)' \
			--timestamp --deep --force $(CHUGINS_DIR)/*.chug && \
		codesign --verify $(CHUGINS_DIR)/*.chug && \
		codesign --sign 'Developer ID Application: $(DEV_ID)' \
				--timestamp --deep --force externals/chuck\~.mxo/Contents/MacOS/chuck\~ && \
				codesign --sign 'Developer ID Application: $(DEV_ID)' \
					--timestamp --deep --force --options runtime \
					--entitlements $(ENTITLEMENTS) externals/chuck\~.mxo && \
		codesign --verify externals/chuck\~.mxo && \
		codesign --verify externals/chuck\~.mxo/Contents/MacOS/chuck\~

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
			--timestamp --deep --force $(DIST)/$(CHUGINS_DIR)/**/*.chug && \
		codesign --verify $(DIST)/$(CHUGINS_DIR)/**/*.chug

zip-dist-pre:
	@zip -r chuck-max-to-notarize.zip $(DIST)

notarize-zip:
	@xcrun notarytool submit "chuck-max-to-notarize.zip" --keychain-profile "$(KEYCHAIN_PROFILE)" --wait

staple-dist:
	@xcrun stapler staple $(DIST)/externals/chuck\~.mxo

zip-dist:
	@cd build/dist && \
		zip -r chuck-max-$(VERSION).zip chuck-max && \
		mv chuck-max-$(VERSION).zip $(ROOT)

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
	@rm -rf externals $(CHUGINS_DIR)/**/*.chug
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

