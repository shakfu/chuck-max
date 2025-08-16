

## Build Guide

The base `chuck-max` system consists of a Max package with the `chuck~` external, the default [CCRMA chugins](https://github.com/ccrma/chugins) and extensive examples, tests and Max patchers.

To get up and running, type the following while in the Max `Packages` directory.

```sh
git clone --recursive https://github.com/shakfu/chuck-max.git
cd chuck-max
```

### Building on Windows

Building the base system on Windows requires [Visual Studio Community Edition](https://visualstudio.microsoft.com/vs/community/) or higher and [cmake](https://cmake.org) to be available on your system.

If `make.exe` is available in your PATH via something like [w64devkit](https://github.com/skeeto/w64devkit), then `make` will build the base system on Windows. Otherwise, type the following:

```sh
mkdir build
cd build
cmake ..
cmake --build . --config Release
cmake --install . --config Release
```

Note that the `Faust.chug`, `WarpBuf.chug` and `FluidSynth.chug` chugins are not yet implemented on Windows.


### Building on MacOS

Building `chuck-max` on MacOS requires the following to be available:

1. Xcode from the App store or Xcode Command Line Tools via `xcode-select –install`

2. The [Homebrew](https://brew.sh) package system[^1].

To build the base system, install the following dependencies using [Homebrew](https://brew.sh) as follows:

```sh
brew install cmake bison flex
```

If you've cloned the `chuck-max` package outside of `$HOME/Documents/Max [8 | 9]/Packages`, then `make setup` will create a symlink of the folder in the `Packages` directory and make it available to Max.

Now it should be possible to build the base system with either of the following two options:

- `make` or `make native`: builds the external using your machine's native architecture which is `arm64` for Apple Silicon Macs and `x86_64` for Intel Macs. This is the default build option.

- `make universal`: build the external as a `universal` binary making it compatible with both Mac architectural variants. This is useful if you want share the external with others in a custom Max package or standalone.

#### Advanced Chugins

A few chugins, `Faust.chug`, `WarpBuf.chug` and `FluidSynth.chug` are not built by default because they need additional dependencies. These are:

1. The [Faust](https://github.com/ccrma/fauck) chugin incorporates the full llvm-based [faust](https://faust.grame.fr) engine and DSP platform, making it exceptionally powerful and larger in size compared to other chugins, approximately 45 MB when stripped down. It necessitates at least two output channels to function correctly and utilizes the [libsndfile](https://github.com/libsndfile/libsndfile) library.

2. The [WarpBuf](https://github.com/ccrma/chugins/tree/main/WarpBuf) chugin enables time-stretching and independent transposition of an audio file's pitch. This functionality is achieved through the use of the [rubberband](https://github.com/breakfastquay/rubberband), [libsndfile](https://github.com/libsndfile/libsndfile), and [libsamplerate](https://github.com/libsndfile/libsamplerate) libraries.

3. The [FluidSynth](https://github.com/FluidSynth/fluidsynth?tab=readme-ov-file) chugin is a software synthesizer based on the SoundFont 2 specifications. On MacOS, it requires [libsndfile](https://github.com/libsndfile/libsndfile) as well.

Install the dependencies required to build these three chugins via [Homebrew](https://brew.sh) as follows:

```sh
brew install autogen automake fluidsynth rubberband libsamplerate
```

This is an abbreviated way to install all the dependencies, and equivalent to the longer:

```bash
brew install autoconf autogen automake fluidsynth libsndfile libtool pkg-config rubberband libsamplerate
```

![macos-adanced-deps](https://github.com/shakfu/chuck-max/blob/main/media/macos-adv-deps.svg)


After these are installed, type the following for a full `chuck-max` package:

```sh
make macos-full
```

[^1]: It is also possible to build `chuck-max` and some of the advanced chugins on MacOS using alternative methods. These are outlined in the [this document]()

