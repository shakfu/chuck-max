# chuck-max

A project to minimally embed the [chuck](https://chuck.stanford.edu) engine in a Max/MSP external.

It currently has one external (`chuck~`) with the following features and limitations:

- Generate and process audio via an embedded chuck engine by running chuck files with `global` parameters controlled and adjusted in realtime by Max messages.

- Layer sounds from a single instance by running multiple chuck files concurrently.

- Add and remove audio and audio processes on the fly via Max messages.

- Includes most of the [standard ccrma chugins](https://github.com/ccrma/chugins) except for the following:

  - Fauck
  - Fluidsynth
  - Ladspa

- The `chuck~` object can take the following arguments:

  - `[chuck~]` : single channel in/out, no default chuck file
  - `[chuck~ <filename>]` : single channel in/out with default chuck file
  - `[chuck~ <N> <filename>]` : N channels with default chuck file

- The `chuck~` object has a single attribute `debug` which can be switched on for more verbose logging to the console.

- As of the current version, `chuck~` maps a few chuck language constructs to corresponding Max/MSP constructs as per the following table:

| action                            | chuck              | max                          |
| :-------------------------------- | :----------------  | :--------------------------  |
| change param value                | global variable    | (`<name>` `<value>`)  msg    |
| trigger named event               | global event       | (`sig <name>`) msg           |
| trigger named event all shreds    | global event       | (`broadcast <name>`) msg     |
| trigger named callback            | global event       | (`sig <name>`) msg           |
| trigger named callback all shreds | global event       | (`broadcast <name>`)  msg    |

See `help/chuck~.maxhelp` and patchers in the `patchers/tests` directory for a demonstration of current features.

Also note that `chuck-max` has a sibling in the [pd-chuck](https://github.com/shakfu/pd-chuck) project.

This project is currently built on the chuck 1.5.2.3-dev (chai) engine.

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

All dependencies are included in the repo, and the external is built 'statically' without any non-system dependencies.

The buildsystem consists of a minimal Makefile frontend with CMake driving the build on the backend.

To get up and running:

```bash
git clone https://github.com/shakfu/chuck-max.git
cd chuck-max
make
make setup
```

Note: `make` builds everything and `make setup` makes the package and its contents available to be used by Max by creating a symlink of the `chuck-max` folder in `$HOME/Documents/Max 8/Packages`.

Also note that by default `make` builds the external according to the
*native* architecture of the mac it is compiled on. If you'd rather build an  external with a universal architecture then do this instead:

```bash
make universal
```

If there is a need to update and re-build the external just type the following in the root of the project.

```bash
git pull
make
```

## Usage

Open the help file `help/chuck~.maxhelp` for a demo.

## Optional or Missing Chugins

There are four of the standard CCRMA chugins which are either optional or not yet supported:

1. `WarpBuf`: works well, can be built with cmake option: `-DENABLE_WARPBUF` or `make full`

2. `Fauck`: works well, provided `chuck~` configured with at least 2 channels.

3. `Ladspa`: not yet supported but known to compile ok.

4. `Fluidsynth`: not yet supported.

## Status

- [ ] add windows support
- [x] add support for fauck (faust chugin)
- [x] add support for WarpBuf chugin
- [x] add support for callbacks (if needed)
- [x] add support for events
- [x] build chugins with cmake
- [x] can set global param values
- [x] fix sound input!
- [x] fix sound output!
- [x] initial attempt to embed (no sound yet) without compilation or max/msp errors
- [x] compiled `chuck~` with `libchuck.a` without errors
- [x] created `libchuck.a`
- [x] converted chuck makefile to CMAKE
- [x] added support for `<<< msgs >>>`

## TODO

- [ ] Add Windows Support.

- [ ] Better examples

- [ ] Add editor support code editor: double-click to edit, etc.. or via filewatcher

- [ ] Flesh out API (add all messages). Learn from the miniAudicle code.

- [ ] Package chugins and scripts in a self-contained signed and notarized external bundle

- ...

## Credits

This project thanks the following:

- Professors GE Wang and Perry Cook and all chuck and chuggin contributors for creating the amazing ChucK language and chuggin ecosystem!

- Professor Perry Cook for co-authoring Chuck and creating the [Synthesis Toolkit](https://github.com/thestk/stk) which is integrated with chuck.

- Professor [Brad Garton](http://sites.music.columbia.edu/brad) for creating the original [chuck~](http://sites.music.columbia.edu/brad/chuck~) external for Max 5. My failure to build it and run it on Max 8 motivated me to start this project.

- David Braun, the author of the very cool and excellent [DawDreamer](https://github.com/DBraun/DawDreamer) project, for creating the excellent [ChucKDesigner](https://github.com/DBraun/ChucKDesigner) project which embeds chuck in a Touch Designer plugin. His project provided a super-clear blueprint on how to embed `libchuck` in another host or plugin system and was essential to this project.
