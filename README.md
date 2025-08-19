# chuck-max

![chuck~ help](https://github.com/shakfu/chuck-max/blob/main/media/screenshot-min.png)

This cross-platform project combines the power of [ChucK](https://chuck.stanford.edu) audio programming with the flexibility of [Max/MSP](https://cycling74.com). It includes a custom Max/MSP object, `chuck~`, that runs version `1.5.5.3-dev (chai)` of the ChucK engine for real-time sound synthesis and audio processing.

## Quick Start

**For the impatient**: Download the pre-compiled `chuck-max` package from [Releases](https://github.com/shakfu/chuck-max/releases) and check out the [cheatsheet](https://github.com/shakfu/chuck-max/blob/main/media/chuck-max-cheatsheat.pdf).

## Overview

`chuck-max` is a cross-platform open-source Max package that embeds the [ChucK](https://chuck.stanford.edu) audio programming language engine in a Max/MSP external (`chuck~`). This powerful integration allows you to:

- **Generate and process audio** by running ChucK files and evaluating code with real-time control of parameters.
- **Layer multiple sounds** from a single instance by running multiple ChucK files concurrently  
- **Modify audio processes on-the-fly** using ChucK messages via Max messages.
- **Interact with Max** via MIDI and OSC protocols

### Key Features

- **Complete ChucK Examples**: Includes a full set of current ChucK examples
- **Rich ChucK plugin (Chugin) Ecosystem**: Includes default [CCRMA chugins](https://github.com/ccrma/chugins) as well as (for macOS):
  - `WarpBuf` - Time-stretching and pitch-shifting
  - `Faust` - LLVM-based DSP platform
  - `FluidSynth` - Software synthesizer with SoundFont support
- **Max Integration**: Comprehensive Max patchers for testing and demonstration
- **Community Contributions**: User-contributed patchers and code examples

## Build Guide

The base `chuck-max` system consists of a Max package with the `chuck~` external, the default set of [CCRMA chugins](https://github.com/ccrma/chugins) and extensive examples, tests and Max patchers.

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

Note that the advanced chugins --`Faust.chug`, `WarpBuf.chug` and `FluidSynth.chug` -- are not yet implemented on Windows.

### Building on MacOS

Building `chuck-max` on MacOS requires the following:

1. Xcode from the App store or Xcode Command Line Tools via `xcode-select –install`

2. The [Homebrew](https://brew.sh) package system[^1].

#### Base MacOS Build

To build the base system, install the following dependencies using [Homebrew](https://brew.sh) as follows:

```sh
brew install cmake bison flex
```

If you've cloned the `chuck-max` package outside of `$HOME/Documents/Max [8 | 9]/Packages`, then `make setup` will create a symlink of the folder in the `Packages` directory and make it available to Max.

Now it should be possible to build the base system with either of the following two options:

- `make` or `make native`: builds the external using your machine's native architecture which is `arm64` for Apple Silicon Macs and `x86_64` for Intel Macs. This is the default build option.

- `make universal`: build the external as a `universal` binary making it compatible with both Mac architectural variants. This is useful if you want share the external with others in a custom Max package or standalone.

#### Full MacOS Build

A few advanced chugins, `Faust.chug`, `WarpBuf.chug` and `FluidSynth.chug` are not included in the Base system by default, these are:

1. The [Faust](https://github.com/ccrma/fauck) chugin incorporates the full llvm-based [faust](https://faust.grame.fr) engine and DSP platform, making it exceptionally powerful and larger in size compared to other chugins, approximately 45 MB when stripped down. It necessitates at least two output channels to function correctly and utilizes the [libsndfile](https://github.com/libsndfile/libsndfile) library.

2. The [WarpBuf](https://github.com/ccrma/chugins/tree/main/WarpBuf) chugin enables time-stretching and independent transposition of an audio file's pitch. This functionality is achieved through the use of the [rubberband](https://github.com/breakfastquay/rubberband), [libsndfile](https://github.com/libsndfile/libsndfile), and [libsamplerate](https://github.com/libsndfile/libsamplerate) libraries.

3. The [FluidSynth](https://github.com/FluidSynth/fluidsynth?tab=readme-ov-file) chugin is a software synthesizer based on the SoundFont 2 specifications. On MacOS, it requires [libsndfile](https://github.com/libsndfile/libsndfile) as well.

For a full MacOS build which includes the above chugins, use `Homebrew` again to install the following dependencies:

```sh
brew install autogen automake fluidsynth rubberband libsamplerate
```

This is actually an abbreviated way to install all the dependencies and sub-dependencies:

![macos-adv-deps](https://github.com/shakfu/chuck-max/blob/main/media/macos-adv-deps.svg)

After these are installed, type the following for a full `chuck-max` build[^2]:

```sh
make brew
```

[^1]: It is also possible to build `chuck-max` and some of the advanced chugins on MacOS using alternative methods. These are outlined in the [alternative-build-guide](https://github.com/shakfu/chuck-max/blob/main/source/docs/alternative-build-guide.md)).

[^2]: There is experimental support for building [chump](https://github.com/ccrma/chump) via `make chump`. See the [chump-build-guide](https://github.com/shakfu/chuck-max/blob/main/source/docs/chump-build-guide.md) for further details about install requirements.


## User Guide

- The `chuck~` object can take the following arguments:

  - `[chuck~]` : single channel in/out, no default chuck file
  - `[chuck~ <N>]` : N channel in/out, no default chuck file
  - `[chuck~ <filename>]` : single channel in/out with default chuck file
  - `[chuck~ <N> <filename>]` : N channels with default chuck file

It's recommended to choose 2 channels for stereo audio.

If a `<filename>` argument is given it will be searched for according to the following rules:

1. Assume it's an absolute path, use it if it exists.

2. Assume that it's a partial path with the package's `examples` folder as a prefix. So if `stk/flute.ck` is given as `<filename>`, the absolute path of the package `examples` folder is prepended to it and if the resulting path exists, it is used.

3. Assume `<filename>` exists in the parent patcher's directory. Test that this is the case and if it is, use it. This is useful if you want to package patchers and chuck files together.

4. Use Max's `locatefile_extended` search function to search for the `<filename>` in the Max search path. The first successul result will be used.

### Core Messages

As of the current version, `chuck~` implements the core Chuck vm messages as Max messages:

| Action                            | Max msg                      | Max msg (alias)              |
| :-------------------------------- | :--------------------------- | :--------------------------  |
| Add shred from file               | `add <file> [arg1 arg2 .. ]` | `+ <filepath> [args]`        |
| Run chuck file (save last used)   | `run <file>`                 |                              |
| Eval code as shred                | `eval <code>`                |                              |
| Remove shred                      | `remove <shredID>`           | `- <shredID>`                |
| Remove last shred                 | `remove last`                |                              |
| Remove all shreds                 | `remove all`                 |                              |
| Replace shred                     | `replace <shredID> <file>`   | `= <shredID> <file>`         |
| List running shreds               | `status`                     |                              |
| Clear vm                          | `clear vm`                   | `reset`                      |
| Clear globals                     | `clear globals`              |                              |
| Reset id                          | `reset id`                   |                              |
| Time                              | `time`                       |                              |

It's worth reading the [ChucK Language Specification's section on Concurrency and Shreds](https://chuck.cs.princeton.edu/doc/language/spork.html) to get a sense of what the above means. The first paragraph will be quoted here since it's quite informative:

> ChucK is able to run many processes concurrently (the process behave as if they are running in parallel). A ChucKian process is called a shred. To spork a shred means creating and adding a new process to the virtual machine. Shreds may be sporked from a variety of places, and may themselves spork new shreds.

### Utility Messages

The core set of chuck vm messesages is also extended in `chuck-max` with the following utility messages:

| Action                                  | Max msg                      |
| :-------------------------------------- | :--------------------------- |
| Set file attribute (does not run)       | `file <path>`                |
| Set full path to editor attribute       | `editor <path>`              |
| Prevent running shreds when dsp is off  | `run_needs_audio`            |
| Open file in external editor            | `edit <path>`                |
| Probe chugins                           | `chugins`                    |
| Get/set loglevel (0-10)                 | `loglevel` & `loglevel <n>`  |
| Get state of chuck vm                   | `vm`                         |
| Launch chuck docs in a browser          | `docs`                       |
| Clear Max console                       | `clear console`              |

### Parameter Messages

Once a shred is running you can change its parameters by sending values from Max to the `chuck~` object. To this end, ChucK makes available three mechanisms: global variables, global events, and callbacks which are triggered by events. `chuck~` maps these chuck language elements to corresponding Max/MSP constructs as per the following table:

| Action                            | ChucK              | Max msg                      |
| :-------------------------------- | :----------------  | :--------------------------  |
| Change param value (untyped)      | global variable    | `<name>` `<value>`           |
| Dump global variables to console  | global variable    | `globals`                    |
| Trigger named event               | global event       | `sig <name>`                 |
| Trigger named event all shreds    | global event       | `broadcast <name>`           |

You change a global variable by sending a `<variable-name> <value>` message to a `chuck~` instance where the `value` can be an `int`, `float`, `string`, `array of ints` or `floats`, etc. You can also trigger events by sending `sig` or signal messages, `broadcast` messages as per the above table.

*Note*: You can't use the ChucK types of `dur` or `time` in Max. Also, while in the above case, the Max msg seems untyped, it must match the type of the chuck global variable. So if you connect a Max number or flownum object to a message box, it needs to match the type of the global variable (int/float).

See `help/chuck~.maxhelp` and patchers in the `patchers/tests` directory for a demonstration of current features.

### Parameter Messages using Callbacks (Advanced Usage)

In addition to the typical way of changing parameters there is also an extensive callback system which includes listening / stop-listening for events associated with callbacks, triggering them via `sig` and `broadcast` messages and also setting typed global variables via messages and symmetrically getting their values via typed callbacks:

| Action                            | ChucK              | Max msg                              |
| :-------------------------------- | :----------------  | :----------------------------------- |
| Listen to event (one shot)        | global event       | `listen <name>` or `listen <name> 0` |
| Listen to event (forever)         | global event       | `listen <name> 1`                    |
| Stop listening to event           | global event       | `unlisten <name>`                    |
| Trigger named callback            | global event       | `sig <name>`                         |
| Trigger named callback all shreds | global event       | `broadcast <name>`                   |
| Get int variable                  | global variable    | `get int <name>`                     |
| Get float variable                | global variable    | `get float <name>`                   |
| Get string variable               | global variable    | `get string <name>`                  |
| Get int array                     | global variable    | `get int[] <name>`                   |
| Get float array                   | global variable    | `get float[] <name>`                 |
| Get int array indexed value       | global variable    | `get int[i] <name> <index>`          |
| Get float array indexed value     | global variable    | `get float[i] <name> <index>`        |
| Get int associative array value   | global variable    | `get int[k] <name> <key>`            |
| Get float associative array value | global variable    | `get float[k] <name> <key>`          |
| Set int variable                  | global variable    | `set int <name> <value>`             |
| Set float variable                | global variable    | `set float <name> <value>`           |
| Set string variable               | global variable    | `set string <name> <value>`          |
| Set int array                     | global variable    | `set int[] <name> v1, v2, ..`        |
| Set float array                   | global variable    | `set float[] <name> v1, v2, ..`      |
| Set int array indexed value       | global variable    | `set int[i] <name> <index> <value>`  |
| Set float array indexed value     | global variable    | `set float[i] <name> <index> <value>`|
| Set int associative array value   | global variable    | `set int[k] <name> <key> <value>`    |
| Set float associative array value | global variable    | `set float[k] <name> <key> <value>`  |

### Package Structure

The `chuck-max` package consists of the following folders:

```text
chuck-max
├── examples
│    ├── ai
│    ├── analysis
│    ├── ...
│    ├── **chugins**
│    ├── ...
│    ├── **fauck**
│    ├── **faust**
│    ├── ...
│    ├── **max**
│    ├── ...
│    ├── **test**
│    ├── ...
│    └── warpbuf
├── externals
│    └── chuck~.mxo
├── help
├── media
└── patchers
    ├── abstractions
    ├── contrib
    └── tests
```

Start with the `chuck~.maxhelp` file in the `help` folder for an overview of the external's features. The `media` folder also has a [pdf cheatsheet](https://github.com/shakfu/chuck-max/blob/main/media/chuck-max-cheatsheat.pdf) of available `chuck~` methods.

The `examples` directory contains all chuck examples from the chuck repo, and some additional folders: `chugins` containing chugin binaries, `fauck`, containing `faust.chug` examples, `faust`, containing the faust stdlib, `fluidsynth`, containing fluidsynth examples and soundfonts, `max`, containing chuck files which are used by the max patchers, and `test`, chuck files used by max patcher test files.

In the `patchers` section, there are quite a few patchers demonstrating one feature or other in the `tests` folder, and the `contrib` folder contains excellents advanced examples of uses by `chuck-max` contributors.

## Known Unresolved Bugs

- If a chuck file contains a custom event and the Max patch sends a `clear vm` or `reset` before running the chuck file while Max audio is turned off, it may cause Max to crash. The case is demonstrated in the `patchers/tests/crash` section. See [github issue #11](https://github.com/shakfu/chuck-max/issues/11) for updates on this. The interim solution is to only run chuck files with Max audio on, and there's an attribute `run_needs_audio` to force this and prevent the crash. Thanks to @HighHarmonics2 for discovering this one.

## Contributing

We welcome contributions! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## Related Projects

- **Sibling Project**: [pd-chuck](https://github.com/shakfu/pd-chuck) for Pure Data
- [Chai](https://chuck.stanford.edu/chai): ChucK for AI.
- [ChucKDesigner](https://github.com/DBraun/ChucKDesigner) for ChucK audio integration with TouchDesigner.
- [WebChuck](https://chuck.cs.princeton.edu/webchuck) for Web-based computer music programming with ChucK.

## Credits

This project thanks the following:

- Professors GE Wang and Perry Cook and all chuck and chuggin contributors for creating the amazing ChucK language and chugin ecosystem!

- Professor Perry Cook for co-authoring Chuck and creating the [Synthesis Toolkit](https://github.com/thestk/stk) which is integrated with chuck.

- Professor [Brad Garton](http://sites.music.columbia.edu/brad) for creating the original [chuck~](http://sites.music.columbia.edu/brad/chuck~) external for Max 5. My failure to build it and run it on Max 8 motivated me to start this project.

- David Braun, the author of the very cool and excellent [DawDreamer](https://github.com/DBraun/DawDreamer) project, for creating the excellent [ChucKDesigner](https://github.com/DBraun/ChucKDesigner) project which embeds chuck in a Touch Designer plugin. His project provided a super-clear blueprint on how to embed `libchuck` in another host or plugin system and was essential to this project.

