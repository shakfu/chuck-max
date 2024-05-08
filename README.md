# chuck-max

A project to minimally embed the [chuck](https://chuck.stanford.edu) engine in a Max/MSP external.

It currently has one external (`chuck~`) with the following features and limitations:

- Generate and process audio via an embedded chuck engine by running chuck files with `global` parameters controlled and adjusted in realtime by Max messages.

- Layer sounds from a single instance by running multiple chuck files concurrently.

- Add, remove, replace audio and audio processes on the fly using Chuck messages via Max messages.

- Includes most of the [base ccrma chugins](https://github.com/ccrma/chugins) including `WarpBuf` and `Fauck` or `Faust` except for the following:

  - Fluidsynth
  - Ladspa

- Note that `chuck-max` has a sibling in the [pd-chuck](https://github.com/shakfu/pd-chuck) project.

This project is currently built on the chuck 1.5.2.5-dev (chai) engine.

## Overview

- The `chuck~` object can take the following arguments:

  - `[chuck~]` : single channel in/out, no default chuck file
  - `[chuck~ <N>]` : N channel in/out, no default chuck file
  - `[chuck~ <filename>]` : single channel in/out with default chuck file
  - `[chuck~ <N> <filename>]` : N channels with default chuck file

- As of the current version, `chuck~` implements the core Chuck vm messages as Max messages:

| action                            | max msg                      | max msg (alias)              |
| :-------------------------------- | :--------------------------- | :--------------------------  |
| add shred                         | `add <file>`                 | `+ <filepath>`               |
| remove shred                      | `remove <shredID>`           | `- <shredID>`                |
| remove last shred                 | `remove last`                | `--`                         |
| remove all shreds                 | `remove all`                 |                              |
| replace shred                     | `replace <shredID> <file>`   | `= <shredID> <file>`         |
| vm status                         | `status`                     | `^`                          |
| clear vm                          | `clear vm`                   | `reset`                      |
| clear globals                     | `clear globals`              |                              |
| reset id                          | `reset id`                   |                              |
| time                              | `time`                       |                              |

It is worth reading [ChucK Language Specification's section on Concurrency and Shreds](https://chuck.cs.princeton.edu/doc/language/spork.html) to get a sense of what the above means. The first paragraph will be quoted here since it's quite informative:

> ChucK is able to run many processes concurrently (the process behave as if they are running in parallel). A ChucKian process is called a shred. To spork a shred means creating and adding a new process to the virtual machine. Shreds may be sporked from a variety of places, and may themselves spork new shreds.

- The core set of chuck vm messesages is also extended in `chuck-max` with the following utility messages:

| action                                  | max msg                      | 
| :-------------------------------------- | :--------------------------- | 
| set file attribute (does not run)       | `file <path>`                | 
| set full path to editor attribute       | `editor <path>`              |
| prevent running shreds when dsp is off  | `run_needs_audio`            |
| open file in external editor            | `edit <path>`                |
| probe chugins                           | `chugins`                    |
| list of running shreds                  | `info`                       |
| get/set loglevel (0-10)                 | `loglevel` & `loglevel <n>`  |
| get state of chuck vm                   | `vm`                         |
| launch chuck docs in a browser          | `docs`                       |


- Once a shred is running it makes sense to try to change it's parameters so to speak from outside of the process. To this end, ChucK makes available three mechanisms to do so: global variabls, global events, and callbacks which are triggered by the former. `chuck~` maps these chuck language elements to corresponding Max/MSP constructs as per the following table:

| action                            | chuck              | max msg                      |
| :-------------------------------- | :----------------  | :--------------------------  |
| change param value (untyped)      | global variable    | `<name>` `<value>`           |
  dump global variables to console. | global variable.   | `globals`                    |
| trigger named event               | global event       | `sig <name>`                 |
| trigger named event all shreds    | global event       | `broadcast <name>`           |


- This means you can change a global variable by sending a `<variable> <value>` message to a `chuck~` instance where the `value` can be an `int`, `float`, `string`, `array of ints` or `floats`, etc.. You can also trigger events by sending `sig` or signal messages, `broadcast` messages as per the above table.

- In addition to the above there is a extensive callback system which includes listening / stop listening for events associated with callbacks, triggering them via `sig` and `broadcast` messages and also setting typed global variables via messages and symmetrically getting their values via typed callbacks:

| example                           | chuck              | max msg                              |
| :-------------------------------- | :----------------  | :----------------------------------- |
| listen to event (one shot)        | global event       | `listen <name>` or `listen <name> 0` |
| listen to event (forever)         | global event       | `listen <name> 1`                    |
| stop listening to event           | global event       | `unlisten <name>`                    |
| trigger named callback            | global event       | `sig <name>`                         |
| trigger named callback all shreds | global event       | `broadcast <name>`                   |
| get int variable                  | global variable    | `get int <name>`                     |
| get float variable                | global variable    | `get float <name>`                   |
| get string variable               | global variable    | `get string <name>`                  |
| get int array                     | global variable    | `get int[] <name>`                   |
| get float array                   | global variable    | `get float[] <name>`                 |
| get int array indexed value       | global variable    | `get int[i] <name> <index>`          |
| get float array indexed value     | global variable    | `get float[i] <name> <index>`        |
| get int associative array value   | global variable    | `get int[k] <name> <key>`            |
| get float associative array value | global variable    | `get float[k] <name> <key>`          |
| set int variable                  | global variable    | `set int <name> <value>`             |
| set float variable                | global variable    | `set float <name> <value>`           |
| set string variable               | global variable    | `set string <name> <value>`          |
| set int array                     | global variable    | `set int[] <name> v1, v2, ..`        |
| set float array                   | global variable    | `set float[] <name> v1, v2, ..`      |
| set int array indexed value       | global variable    | `set int[i] <name> <index> <value>`  |
| set float array indexed value     | global variable    | `set float[i] <name> <index> <value>`|
| set int associative array value   | global variable    | `set int[k] <name> <key> <value>`    |
| set float associative array value | global variable    | `set float[k] <name> <key> <value>`  |

See `help/chuck~.maxhelp` and patchers in the `patchers/tests` directory for a demonstration of current features.

## Requirements

Note that this external is currently only developed and tested on macOS, although a Windows version is on the TODO list.

It requires the following to be available on your system:

1. Xcode from the App store or Xcode Command Line Tools via `xcode-select –install`

2. cmake

3. bison

4. flex

The last three can be installed using [Homebrew](https://brew.sh) as follows:

```bash

brew install cmake bison flex

```

## Compilation

Currently `chuck~` can only be built on macOS. Of course, any help to progrss a windows version variant would be welcome!

The buildsystem consists of a minimal Makefile frontend with CMake driving the build on the backend.

To get up and running:

```bash
git clone https://github.com/shakfu/chuck-max.git
cd chuck-max
make setup
```

Note: `make setup` does two things: (1) retrieve `max-sdk-base` via a git submodule and makes the package and its contents available to be used by Max by creating a symlink of the `chuck-max` folder in `$HOME/Documents/Max 8/Packages`.

From this point you have three options:

1. `make`: Base system: (external + base ccrma chugins excluding `Fauck` or `Faust` and `WarpBuf`)*

2. `make full`: Base system + `Faust` and `Warpbuf` chugins with full `libsndfile` format support

3. `make light`: Base system + `Faust` and `Warpbuf` chugins with `libsndfile` support only `.wav` files.

[`*`] The [Fauck](https://github.com/ccrma/fauck) or `Faust` chugin will be referred to by either of these names.

Also note that by default `make` builds the external according to the *native* architecture of the mac it is compiled on. You can build the base system with universal support by typing `make universal` instead.

If there's a need to update and re-build the external just type the following in the root of the project.

```bash
git pull
make
```

### Install the Warpbuf Chugin (Optional)

With `WarpBuf` you can time-stretch and independently transpose the pitch of an audio file.

This chugin can be built by `make full` instead of `make` in the build process above or if you are just using cmake then set option `-DENABLE_WARPBUF=ON`

### Install the Fauck Chugin (Optional)

The [fauck chugin](https://github.com/ccrma/fauck) contains the full llvm-based [faust](https://faust.grame.fr) engine and dsp platform which makes it very powerful and also quite large compared to other chugins (at around 45 MB stripped down). It requires at least 2 output channels to work properly.

This chugin can be built by `make full` and `make light` options instead of `make` in the build process above or if you are just using cmake then set option -DENABLE_FAUCK=ON

A future aim is to include a stripped down version of this fauck chugin which only supports .wav files and and only contains the faust standard library in an official `chuck-max` package.

CAVEAT: the Faust chugin has unresolved cleanup bug which may cause Max to crash after all patch windows are closed and Max is exited.

## Usage

Open the help file `help/chuck~.maxhelp` for a demo. Check out the `patchers` folders for further examples of use.


## Known Unresolved Bugs

1. If a chuck file contains a custom event and the max patch sends a `clear vm` or `reset` before running, and the chuck file is run before Max audio is turned on, it may cause Max to crash. See [github issue #11](https://github.com/shakfu/chuck-max/issues/11) for updates on this

2. Use of the `Fauck` or `Faust` chugin will cause Max to crash when the user quits Max even after all patch windows are closed.



## Credits

This project thanks the following:

- Professors GE Wang and Perry Cook and all chuck and chuggin contributors for creating the amazing ChucK language and chuggin ecosystem!

- Professor Perry Cook for co-authoring Chuck and creating the [Synthesis Toolkit](https://github.com/thestk/stk) which is integrated with chuck.

- Professor [Brad Garton](http://sites.music.columbia.edu/brad) for creating the original [chuck~](http://sites.music.columbia.edu/brad/chuck~) external for Max 5. My failure to build it and run it on Max 8 motivated me to start this project.

- David Braun, the author of the very cool and excellent [DawDreamer](https://github.com/DBraun/DawDreamer) project, for creating the excellent [ChucKDesigner](https://github.com/DBraun/ChucKDesigner) project which embeds chuck in a Touch Designer plugin. His project provided a super-clear blueprint on how to embed `libchuck` in another host or plugin system and was essential to this project.
