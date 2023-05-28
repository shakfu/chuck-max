# chuck-max

A project to minimally embed the [chuck](https://chuck.stanford.edu) engine in a Max/Msp external.

It currently has the following features and limitations:

- Generate and process audio via an embedded chuck engine by running chuck files with `global` parameters adjusted and controlled in realtime via Max messages.

- Layer sounds by running multiple chuck files concurrently.

- Add and remove audio and audio processes on the fly via max messages.

- Includes most of the [standard ccrma chugins](https://github.com/ccrma/chugins) except for the following: 

	- Faust
	- Fluidsynth
	- MIAP
	- Ladspa
	- WarpBuf

- As of this version, there is no support for callbacks and events except via the `signal` and `broadcast` messages.

see `help/chuck~.maxhelp` for a demonstration of current features.

## Requirements

Note that this external is currently only developed and tested on macOS.

It requires the following to be available on your system:

1. Xcode from the App store or Xcode Command Line Tools via `xcode-select –install`

2. cmake

3. bison

4. flex

The last three can be installed via Homebrew as follows:

```bash

brew install cmake bison flex

```


## Compilation

All dependencies are included in the repo, and the external is built 'statically' without any non-system dependencies.

Just clone the the repo and then

```bash
cd chuck-max
make
make setup
```

Note: `make` builds everything and the `make setup` does the following:

```bash
ln -s $(shell pwd) "$(HOME)/Documents/Max 8/Packages/$(shell basename `pwd`)"
```

This makes the package and it's contents available in Max


## Usage

Open the help file `help/chuck~.maxhelp` for a demo.


## Status

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


## TODO

- [ ] More error checking!!! 

- [ ] Add editor support for double-click to edit, etc.. or via filewatcher

- [ ] Flesh out API (add all messages). Learn from the miniAudicle code.

- [ ] Add support for events and callbacks: ChucKDesigner has python callbacks, perhaps use lua or js via an extension?

- [ ] Convert `chuginate` to python3

- [ ] Package chugins and scripts in a self-contained signed and notarized external bundle

- ...


## Credits

This project thanks the following:

- Professors GE Wang and Perry Cook and all chuck and chuggin contributors for creating the amazing ChucK language and chuggin ecosystem!

- Professor Perry Cook for co-authoring Chuck and creating the [Synthesis Toolkit](https://github.com/thestk/stk) which is integrated with chuck.

- Professor [Brad Garton](http://sites.music.columbia.edu/brad) for creating the original [chuck~](http://sites.music.columbia.edu/brad/chuck~) external for Max 5. My failure to build and run on Max 8 his external on Max 8 motivated me to start this project.

- [David Braun] (the author of the very cool [DawDreamer](https://github.com/DBraun/DawDreamer) project which I am already a great fan of) for creating the excellent[ChucKDesigner](https://github.com/DBraun/ChucKDesigner) project which embeds chuck in a Touch Designer plugin. His project provided a super-clear blueprint on how to embed `libchuck` in another host or plugin system and was essential to this project.

