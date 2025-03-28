# chuck-max

![chuck~ help](https://github.com/shakfu/chuck-max/blob/main/media/screenshot-min.png)

A project which embeds the [ChucK](https://chuck.stanford.edu) 1.5.5.1-dev (chai) engine in a Max/MSP external.

This repository is itself a Max package which includes one external (`chuck~`) with the following features and limitations:

- Generate and process audio by running chuck files and evaluating chuck code with `global` parameters controlled and adjusted in realtime by Max messages.

- Layer sounds from a single instance by running multiple chuck files concurrently.

- Add, remove, replace audio and audio processes on the fly using chuck messages via Max messages.

- Interact with Max via MIDI and OSC protocols.

The package also includes the following:

- The complete set of current chuck examples

- Most of the [base CCRMA chugins](https://github.com/ccrma/chugins) including `WarpBuf`, `Fauck` or `Faust` and  `Fluidsynth`, except `Ladspa`.

- Many Max patchers to test and demonstrate usage.

- Contributed patchers and code examples.

*For the impatient*: download the `chuck-max` package with pre-compiled externals and chugins from the the project's [Releases](https://github.com/shakfu/chuck-max/releases) section and check out the [cheatsheat](https://github.com/shakfu/chuck-max/blob/main/media/chuck-max-cheatsheat.pdf).

Note that `chuck-max` has a sibling in the [pd-chuck](https://github.com/shakfu/pd-chuck) project.

## Overview

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

## Usage

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

The `examples` directory contains all chuck examples from the chuck repo, and some additional folders: `chugins` containing chugin binaries, `fauck`, containing `faust.chug` examples, `faust`, containing the faust stdlib, `max` chuck files which are used by the max patchers, and `test`, chuck files used by max patcher test files.

In the `patchers` section, there are quite a few patchers demonstrating one feature or other in the `tests` folder, and the `contrib` folder contains excellents advanced examples of uses by `chuck-max` contributors.

## Known Unresolved Bugs

- If a chuck file contains a custom event and the Max patch sends a `clear vm` or `reset` before running the chuck file while Max audio is turned off, it may cause Max to crash. The case is demonstrated in the `patchers/tests/crash` section. See [github issue #11](https://github.com/shakfu/chuck-max/issues/11) for updates on this. The interim solution is to only run chuck files with Max audio on, and there's an attribute `run_needs_audio` to force this and prevent the crash. Thanks to @HighHarmonics2 for discovering this one.

## Credits

This project thanks the following:

- Professors GE Wang and Perry Cook and all chuck and chuggin contributors for creating the amazing ChucK language and chuggin ecosystem!

- Professor Perry Cook for co-authoring Chuck and creating the [Synthesis Toolkit](https://github.com/thestk/stk) which is integrated with chuck.

- Professor [Brad Garton](http://sites.music.columbia.edu/brad) for creating the original [chuck~](http://sites.music.columbia.edu/brad/chuck~) external for Max 5. My failure to build it and run it on Max 8 motivated me to start this project.

- David Braun, the author of the very cool and excellent [DawDreamer](https://github.com/DBraun/DawDreamer) project, for creating the excellent [ChucKDesigner](https://github.com/DBraun/ChucKDesigner) project which embeds chuck in a Touch Designer plugin. His project provided a super-clear blueprint on how to embed `libchuck` in another host or plugin system and was essential to this project.
