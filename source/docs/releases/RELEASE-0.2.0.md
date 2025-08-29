## Overview

chuck-max `v0.2.0` is a major milestone release which makes ChucK accessible to the Windows Max/MSP community while maintaining and enhancing the existing macOS experience. This release represents a significant step forward in cross-platform compatibility and includes an update to `ChucK v1.5.5.3`,  numerous enhancements to the build system, chugin support, overall functionality as well as a new package logo.

## What's New in v0.2.0

### Windows Support

- **Full Windows Build Support**: Complete Windows compatibility for building `chuck~` and all standard CCRMA chugins.
- **Cross-Platform Package**: New `-multi` package variant that includes both macOS (arm64) and Windows (amd64) support.
- **Visual Studio Integration**: Native support for Visual Studio Community Edition builds.
- **CMake Windows Builds**: Comprehensive CMake-based build system for Windows environments.

### Enhanced Build System

- **Alternative Build Method**: Added support for building via [go-task](https://taskfile.dev), a fast, cross-platform build tool inspired by Make.
- **Improved Github Actions**: Added support for building both windows and macOs packages as well as cross-platform packages using Github actions.
- **Bundled Chugins**: New `BUNDLED=1` macOS-only option for bundling chugins directly in the external's bundle
- **Chump Integration**: Chump is now available as a cmake-compilable subproject as a first step to further integration.

### Chugin Enhancements

- **Chugin Windows Support**: Added Windows support for the complete CCRMA chugin collection

## Technical Improvements

### Core Engine Updates

- **ChucK Version**: Updated to ChucK 1.5.5.3-dev (chai) - the latest development version
- **Examples Update**: All ChucK examples updated to match the latest engine version
- **Build System Modernization**: Improved CMake configuration and build scripts

### Platform-Specific Enhancements

- **macOS**: Continued support for arm64 and x64 architectures
- **Windows**: Full build pipeline from source to distribution
- **Cross-Platform**: Unified build system that works across all supported platforms

## Known Issues

- Advanced chugins (`Faust.chug`, `WarpBuf.chug`, `FluidSynth.chug`) are not yet implemented on Windows
- Some platform-specific optimizations may vary between macOS and Windows builds

## Additional Resources

- **Documentation**: See `help/chuck~.maxhelp` for detailed usage information
- **Examples**: Explore the `examples/` directory for practical demonstrations
- **Community**: Join discussions on chuck-max channel of the ChucK discord and also the project's GitHub repository
- **Cheatsheet**: Download the PDF cheatsheet from the `media/` folder

## Information about the Release Build Variants

Note that all MacOS externals and binaries are fully codesigned and notarized and can be used widely in other packages and standalones.

### MacOS Releases

- [chuck-max-0.2.0-macos-arm64.dmg](https://github.com/shakfu/chuck-max/releases/download/0.2.0/chuck-max-0.2.0-macos-arm64.dmg) - Full Max Package Release for Apple Silicon Macs, includes `chuck~` external, ChucK examples and all CCRMA chugines and also `Faust.chug`, `Warpbuf.chug` and `Fluidsynth.chug`, in the `examples/chugins` folder.

- [chuck-max-0.2.0-macos-arm64-bundled.dmg](https://github.com/shakfu/chuck-max/releases/download/0.2.0/chuck-max-0.2.0-macos-arm64-bundled.dmg) - The `BUNDLED=1` build variant is essentially the same as above but the chugins are embedded in the external's `Resource` folder.

- [chuck-max-0.2.0-macos-universal.dmg](https://github.com/shakfu/chuck-max/releases/download/0.2.0/chuck-max-0.2.0-macos-universal.dmg) - The `UNIVERSAL=1` build includes universal binaries for both arm64 and x86_64 architectures.

- [chuck-max-0.2.0-macos-x86_64.dmg](https://github.com/shakfu/chuck-max/releases/download/0.2.0/chuck-max-0.2.0-macos-x86_64.dmg) - A build specifically for intel Macs.

### Windows Releases

- [chuck-max-0.2.0-windows-amd64.zip](https://github.com/shakfu/chuck-max/releases/download/0.2.0/chuck-max-0.2.0-windows-amd64.zip) - Base Max Package Release for Windows machines, essentially similar same as the more mature MacOS release but without support for `Faust.chug`, `Warpbuf.chug` and `Fluidsynth.chug` chugins.

### Cross-Platform Releases

- [chuck-max-0.2.0-multi.zip](https://github.com/shakfu/chuck-max/releases/download/0.2.0/chuck-max-0.2.0-multi.zip) - The `MULTI-1` build is a cross-platform Max Package which includes both arm64 MacOS and Windows externals, ChucK examples and both MacOs and Windows versions of CCRMA Chugins, as well as `Faust.chug`, `Warpbuf.chug` and `Fluidsynth.chug` support for the MacOS external.
