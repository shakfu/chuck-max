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

It is recommended to choose 2 channels for stereo configuration. If a `<filename>` argument is given it will be searched for according to the following search rules:

1. Assume it's an absolute path, accept it if it exists

2. Assume that it's a partial path with the package's `examples` folder as a prefix. So if `stk/flute.ck` is given the `<filename>` arg, The absolute path of the package `examples` folder is prepended to the filename and if the resulting path exists, it is accepted.

3. Assume the `<filename>` exists in the parent patcher's directory. If so, accept it. This is useful if you want to package patchers and chuck files together.

4. Use Max's `locatefile_extended` search function to search for the `<filename>` in the Max search path. The first successul result will be used.


### Core Messages

As of the current version, `chuck~` implements the core Chuck vm messages as Max messages:

| Action                            | Max msg                      | Max msg (alias)              |
| :-------------------------------- | :--------------------------- | :--------------------------  |
| Add shred                         | `add <file>`                 | `+ <filepath>`               |
| Remove shred                      | `remove <shredID>`           | `- <shredID>`                |
| Remove last shred                 | `remove last`                | `--`                         |
| Remove all shreds                 | `remove all`                 |                              |
| Replace shred                     | `replace <shredID> <file>`   | `= <shredID> <file>`         |
| VM status                         | `status`                     | `^`                          |
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
| List of running shreds                  | `info`                       |
| Get/set loglevel (0-10)                 | `loglevel` & `loglevel <n>`  |
| Get state of chuck vm                   | `vm`                         |
| Launch chuck docs in a browser          | `docs`                       |

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

In order to customize the current set of callbacks (which currently just post the value of the parameters to Max console), an advanced user may want to modify a callback function to do something other than the default and then re-compile the external.

```c++
void cb_get_int(const char* name, long val)
{
     post("cb_get_int: name: %s value: %d", name, val);
}
```

*Development Note*: In practice, callbacks in chuck-max are constrained by what their function signature allows, and to do something useful in Max's c-api one will typically want to access the pointer to the object instance of `chuck~` which is not available with any of the callbacks. While it is theoretically possible to make this work if one limits onself to one `chuck~` instance and global variables, it not a *natural* way of accessing the potential of this feature. If callbacks change from function pointers to `std::function` callables, which allow for capturing context, this situation will likely change for the better.


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

1. If a chuck file contains a custom event and the Max patch sends a `clear vm` or `reset` before running the chuck file while Max audio is turned off, it may cause Max to crash. See [github issue #11](https://github.com/shakfu/chuck-max/issues/11) for updates on this. The interim solution is to only run chuck files with Max audio on, and there's an attribute `run_needs_audio` to force this and prevent the crash. Thanks to @HighHarmonics2 for discovering this one.

2. Use of the `Fauck` or `Faust` chugin will cause Max to crash when the user quits Max after all patch windows are closed.

## Credits

This project thanks the following:

- Professors GE Wang and Perry Cook and all chuck and chuggin contributors for creating the amazing ChucK language and chuggin ecosystem!

- Professor Perry Cook for co-authoring Chuck and creating the [Synthesis Toolkit](https://github.com/thestk/stk) which is integrated with chuck.

- Professor [Brad Garton](http://sites.music.columbia.edu/brad) for creating the original [chuck~](http://sites.music.columbia.edu/brad/chuck~) external for Max 5. My failure to build it and run it on Max 8 motivated me to start this project.

- David Braun, the author of the very cool and excellent [DawDreamer](https://github.com/DBraun/DawDreamer) project, for creating the excellent [ChucKDesigner](https://github.com/DBraun/ChucKDesigner) project which embeds chuck in a Touch Designer plugin. His project provided a super-clear blueprint on how to embed `libchuck` in another host or plugin system and was essential to this project.
