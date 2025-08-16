# chuck-max

## Build Requirements and Options

Please note that this external is currently only developed and tested on macOS, although a Windows version is on the TODO list (any help on this front would be much appreciated).

A number of build variants have been made available to address platform-specific changes and library compatibility issues and also to provide for different levels of usage.

The following matrix shows feature coverage and also differences in compatibility among the different macOS versions:

| feature/variant     | native | universal | brew | full | nomp3 | light |
| :--------------     | :----: | :-------: | :--: | :--: | :---: | :---: |
| External            | x      | x         | x    | x    | x     | x     |
| Base chugins        | x      | x         | x    | x    | x     | x     |
| Faust.chug          |        |           | x    | x    | x     | x     |
| WarpBuf.chug        |        |           | x    | x    | x     | x     |
| FluidSynth.chug     | x      | x         | x    |      |       |       |
| .wav                | x      | x         | x    | x    | x     | x     |
| .mp3                |        |           | x    | x    |       |       |
| .flac               |        |           | x    | x    | x     |       |
| .ogg                |        |           | x    | x    | x     |       |
| .opus               |        |           | x    | x    | x     |       |
| .vorbis             |        |           | x    | x    | x     |       |
| macOS 15 (Sequoia)  | x      | x         | x    | x    | x     | x     |
| macOS 14 (Sonoma)   | x      | x         | x    | x    | x     | x     |
| macOS 13 (Ventura)  | x      | x         | x    | x    | x     | x     |
| macOS 12 (Monterey) | x      | x         | x    | x    | x     | x     |
| macOS 11 (Big Sur)  | x      | x         |      | x    | x     | x     |


Note the the `native` variant will build `FluidSynth.chug` if its dependencies are available.

### A. The Base System

The base `chuck-max` system consists of a Max package with the `chuck~` external, the base [CCRMA chugins](https://github.com/ccrma/chugins) and extensive examples, tests and Max patchers.

Building it requires the following to be available on your machine:

1. Xcode from the App store or Xcode Command Line Tools via `xcode-select –install`

2. cmake

3. bison

4. flex

The last three can be installed using [Homebrew](https://brew.sh) as follows:

```bash
brew install cmake bison flex
```

The buildsystem consists of a minimal Makefile frontend with CMake driving the build on the backend.

To get up and running:

```bash
git clone https://github.com/shakfu/chuck-max.git
cd chuck-max
make setup
```

Note that `make setup` does two things: (1) retrieves `max-sdk-base` as a git submodule and makes the package and its contents available to be used by Max by creating a symlink of the `chuck-max` folder in `$HOME/Documents/Max 9/Packages`.

Now it should be possible to build the base system with either of the following two options:

- `make` or `make native`: builds the external using your machine's native architecture which is `arm64` for Apple Silicon Macs and `x86_64` for Intel Macs. This is the default build option.

- `make universal`: build the external as a `universal` binary making it compatible with both Mac architectural variants. This is useful if you want share the external with others in a custom Max package or standalone.

### B. The Advanced System

The advanced system consists of the base system + two advanced chugins, `Faust.chug` and `WarpBuf.chug`:

1. The [Fauck](https://github.com/ccrma/fauck) chugin contains the full llvm-based [faust](https://faust.grame.fr) engine and dsp platform which makes it quite powerful and also quite large compared to other chugins (at around 45 MB stripped down). It requires at least 2 output channels to work properly. It also uses the [libsndfile](https://github.com/libsndfile/libsndfile) library.

2. The [WarpBuf](https://github.com/ccrma/chugins/tree/main/WarpBuf) chugin makes it possible to time-stretch and independently transpose the pitch of an audio file. It uses the [rubberband](https://github.com/breakfastquay/rubberband), [libsndfile](https://github.com/libsndfile/libsndfile), and [libsamplerate](https://github.com/libsndfile/libsamplerate) libraries.

3. The [FluidSynth](https://github.com/FluidSynth/fluidsynth?tab=readme-ov-file) chugin is a software synthesizer based on the SoundFont 2 specifications. On MacOS, it requires `brew install fluidsynth pkg-config`

To build these three chugins, you will need some additional dependencies which can also be installed via `Homebrew` as follows:

```bash
brew install autoconf autogen automake fluidsynth flac libogg libtool libvorbis opus mpg123 lame pkg-config rubberband libsamplerate
```

After these are installed, it should be possible, subject to the compatibility matrix above, to build the advanced system with one of the following options:

- `make brew`: build the external using the previously installed homebrew dependencies, as well as downloaded `faust` headers and a downloaded pre-compiled `libfaust` (`libfaustwithllvm`) library, and some standard soundfonts to run the tests. This is the newer, faster, recommended way of getting a full chuck-max system up and running.

- `make full`: build the external by manually building all of the dependencies except for `libfaust` from source. This is the previous way of building an advanced system. It is currently only for advanced developers who want maximum flexibility in their builds.

- `make nomp3`: same as `make full` except without support for the .mp3 format. Try this variant if you are unable to build using `make full` on Intel Macs or on older macOS versions.

- `make light`: Same as `make full` except for withouth `libsndfile` multi-file format support. This means that (.mp3, flac, vorbis, opus, ogg) formats are not supported in this build. Only `.wav` files can be used.
