PROJECT_NAME = chuck-max
EXTERNAL_NAME = chuck\~
VERSION = 0.2.0

PLATFORM = $(shell uname)
MAX_VERSION := 9
CHUCK_PACKAGE := "$(HOME)/Documents/Max\ $(MAX_VERSION)/Packages/$(PROJECT_NAME)"
CONFIG = Release
ROOT := $(PWD)
BUILD := $(ROOT)/build
SCRIPTS := $(ROOT)/source/scripts
THIRDPARTY = $(BUILD)/thirdparty
LIB = $(THIRDPARTY)/install/lib
CHUCK = $(THIRDPARTY)/install/bin/chuck
DIST = $(BUILD)/dist/$(PROJECT_NAME)
ARCH=$(shell uname -m)
ENTITLEMENTS = $(SCRIPTS)/entitlements.plist
CHUGINS_DIR = examples/chugins

# variants
BUNDLED=0
MULTI=0
UNIVERSAL=0

-DCM_VST3=ON 
ifeq ($(PLATFORM), Darwin)
OS = "macos"
GENERATOR ?= "-GXcode"
ifeq ($(UNIVERSAL), 0)
EXTRA_OPTIONS += -DCM_VST3=ON
endif
ifeq ($(UNIVERSAL), 1)
DIST_NAME = $(PROJECT_NAME)-$(VERSION)-macos-universal
EXTRA_OPTIONS += -DCM_MACOS_UNIVERSAL=ON
endif
ifeq ($(BUNDLED), 1)
EXTRA_OPTIONS += -DCM_MACOS_BUNDLED_CHUGINS=ON
CHUGINS_DIR = externals/$(EXTERNAL_NAME).mxo/Contents/Resources/chugins
endif
else
OS = "windows"
endif

ifeq ($(MULTI), 1)
EXTRA_OPTIONS += -DCM_MULTIPLATFORM_CHUGINS=ON
DIST_NAME = $(PROJECT_NAME)-$(VERSION)
CHUGIN_GLOB = **/*.chug
else
CHUGIN_GLOB = *.chug
endif

DIST_NAME = $(PROJECT_NAME)-$(VERSION)-$(OS)-$(ARCH)
DMG = $(DIST_NAME).dmg
ZIP = $(DIST_NAME).zip


.PHONY: all native multi universal full light brew brew-lite nomp3 dev setup \
		clean reset example example-fauck example-warpbuf example-fluidsynth example-audiounit \
		install_deps install_deps_light install_deps_nomp3 \
		full2 install_fs_deps chump \
		sign package dmg zip-dist sign-dmg notarize staple sign-dist sign-bundle \
		release release-zip test

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
	@strip -u -r externals/$(EXTERNAL_NAME).mxo/Contents/MacOS/$(EXTERNAL_NAME)
	@strip -x $(CHUGINS_DIR)/$(CHUGIN_GLOB)

sign:
	@codesign --sign 'Developer ID Application: $(DEV_ID)' \
			--timestamp --deep --force $(CHUGINS_DIR)/$(CHUGIN_GLOB) && \
		codesign --verify $(CHUGINS_DIR)/$(CHUGIN_GLOB) && \
		codesign --sign 'Developer ID Application: $(DEV_ID)' \
				--timestamp --deep --force externals/$(EXTERNAL_NAME).mxo/Contents/MacOS/$(EXTERNAL_NAME) && \
				codesign --sign 'Developer ID Application: $(DEV_ID)' \
					--timestamp --deep --force --options runtime \
					--entitlements $(ENTITLEMENTS) externals/$(EXTERNAL_NAME).mxo && \
		codesign --verify externals/$(EXTERNAL_NAME).mxo && \
		codesign --verify externals/$(EXTERNAL_NAME).mxo/Contents/MacOS/$(EXTERNAL_NAME)

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

sign-dist:
	@codesign --sign 'Developer ID Application: $(DEV_ID)' \
		--timestamp --deep --force $(DIST)/externals/$(EXTERNAL_NAME).mxo/Contents/MacOS/$(EXTERNAL_NAME) && \
		codesign --sign 'Developer ID Application: $(DEV_ID)' \
			--timestamp --deep --force --options runtime \
			--entitlements $(ENTITLEMENTS) $(DIST)/externals/$(EXTERNAL_NAME).mxo && \
		codesign --verify $(DIST)/externals/$(EXTERNAL_NAME).mxo && \
		codesign --verify $(DIST)/externals/$(EXTERNAL_NAME).mxo/Contents/MacOS/$(EXTERNAL_NAME) && \
		codesign --sign 'Developer ID Application: $(DEV_ID)' \
			--timestamp --deep --force $(DIST)/$(CHUGINS_DIR)/$(CHUGIN_GLOB) && \
		codesign --verify $(DIST)/$(CHUGINS_DIR)/$(CHUGIN_GLOB)

zip-dist-pre:
	@ditto -c -k --keepParent $(DIST) $(BUILD)/$(PROJECT_NAME)-to-notarize.zip

notarize-zip:
	@xcrun notarytool submit "$(BUILD)/$(PROJECT_NAME)-to-notarize.zip" --keychain-profile "$(KEYCHAIN_PROFILE)" --wait

staple-dist:
	@xcrun stapler staple $(DIST)/externals/$(EXTERNAL_NAME).mxo

zip-dist:
	@dot_clean -m $(DIST)
	@cd build/dist && zip -r $(PROJECT_NAME)-$(VERSION)-multi.zip chuck-max
	@mv build/dist/$(PROJECT_NAME)-$(VERSION)-multi.zip $(ROOT)
# 	@ditto -c -k --keepParent $(DIST) $(ZIP)

release-zip: sign-dist zip-dist-pre notarize-zip staple-dist zip-dist
	@echo done

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
	@rm -rf externals $(CHUGINS_DIR)/$(CHUGIN_GLOB) $(CHUGINS_DIR)/macos-* $(CHUGINS_DIR)/windows-*
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

example:
	@cd examples && timeout 10 ./chuck --chugin-path:chugins stk/rhodey-song.ck -v3 || true

example-fauck:
	@cd examples && timeout 10 ./chuck --chugin-path:chugins fauck/test.ck -v3 || true

example-warpbuf:
	@cd examples && timeout 10 ./chuck --chugin-path:chugins warpbuf/warpbuf_basic.ck -v3 || true

example-fluidsynth:
	@cd examples && timeout 10 ./chuck --chugin-path:chugins fluidsynth/FluidSynth-test.ck -v3 || true

example-audiounit:
	@cd examples && timeout 10 ./chuck --chugin-path:chugins audiounit/00-list-audiounit.ck -v3 || true

example-audiounit1:
	@cd examples && timeout 10 ./chuck --chugin-path:chugins audiounit/07-midi.ck -v3 || true
