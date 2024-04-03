# chuck-max

A project to minimally embed the [chuck](https://chuck.stanford.edu) engine in a Max/MSP external. 

It currently has one external (`chuck~`) with the following features and limitations:

- Generate and process audio via an embedded chuck engine by running chuck files with `global` parameters controlled and adjusted in realtime by Max messages.

- Layer sounds from a single instance by running multiple chuck files concurrently.

- Add and remove audio and audio processes on the fly via Max messages.

- Includes most of the [standard ccrma chugins](https://github.com/ccrma/chugins) except for the following: 

	- Faust
	- Fluidsynth
	- MIAP
	- Ladspa
	- WarpBuf

- As of this version, there is no support for callbacks and events except via the `signal` and `broadcast` messages.

see `help/chuck~.maxhelp` for a demonstration of current features.

Also note that `chuck-max` has a sibling in the [pd-chuck](https://github.com/shakfu/pd-chuck) project.

This project is currently built on chuck 1.5.2.3-dev (chai).

## Requirements

Note that this external is currently only developed and tested on macOS.

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

If you want to update and re-build the external if there are changes in the repo just type the following in the root of the project.

```bash
git pull
make
```

## Usage

Open the help file `help/chuck~.maxhelp` for a demo.


## Status

- [ ] add windows support
- [ ] add support for events
- [ ] add support for callbacks
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

- [ ] More error checking!!! 

- [ ] Add editor support for double-click to edit, etc.. or via filewatcher

- [ ] Flesh out API (add all messages). Learn from the miniAudicle code.

- [ ] Add support for events and callbacks: ChucKDesigner has python callbacks, perhaps use lua or js via an extension?

- [ ] Package chugins and scripts in a self-contained signed and notarized external bundle

- ...


## Credits

This project thanks the following:

- Professors GE Wang and Perry Cook and all chuck and chuggin contributors for creating the amazing ChucK language and chuggin ecosystem!

- Professor Perry Cook for co-authoring Chuck and creating the [Synthesis Toolkit](https://github.com/thestk/stk) which is integrated with chuck.

- Professor [Brad Garton](http://sites.music.columbia.edu/brad) for creating the original [chuck~](http://sites.music.columbia.edu/brad/chuck~) external for Max 5. My failure to build it and run it on Max 8 motivated me to start this project.

- David Braun, the author of the very cool and excellent [DawDreamer](https://github.com/DBraun/DawDreamer) project, for creating the excellent [ChucKDesigner](https://github.com/DBraun/ChucKDesigner) project which embeds chuck in a Touch Designer plugin. His project provided a super-clear blueprint on how to embed `libchuck` in another host or plugin system and was essential to this project.

