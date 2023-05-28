# chuck-max

A project to embed the [chuck](https://chuck.stanford.edu) engine in a Max/Msp external.

It has the following features and limitations:

- Generate and process sound via an embedded chuck engine by running chuck files with the ability to set and control global parameters via Max messages. 

- Layer sounds by running multiple chuck files concurrently.

- Add and remove sounds on the fly via max messages.

- Most of the [standard ccrma chugins](https://github.com/ccrma/chugins) except for the following are part of the cmake build and are available for use by the external: 

	- Faust
	- Fluidsynth
	- MIAP
	- Ladspa
	- WarpBuf

- As of this version, there is no support for callbacks and events except via the `signal` and `broadcast` messages.

see `help/chuck~.maxhelp` for a demonstration of current features.

## Compilation

Note that this external is currently only developed and tested on macOS. All dependencies are included in the repo, and the external is built 'statically' without any non-system dependencies.

```bash
make
make setup
```

Note: `make setup` does the following:

```bash
ln -s $(shell pwd) "$(HOME)/Documents/Max 8/Packages/$(shell basename `pwd`)"
```


## Usage

Open the help file for a demo.


## Status

- [ ] add support for events
- [ ] add support for callbacks
- [x] build chugins with cmake
- [x] can set param values
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

- [ ] Package chugins and scripts in external bundle

- ...


## Credits

This project thanks the following:

- Professors GE Wang and Perry Cook and all chuck and chuggin contributors for creating the amazing ChucK language and the chuggin ecosystem!

- Professor Perry Cook for co-authoring Chuck and creating the [Synthesis Toolkit](https://github.com/thestk/stk) which is integrated with chuck.

- Professor [Brad Garton](http://sites.music.columbia.edu/brad) for creating the original [chuck~](http://sites.music.columbia.edu/brad/chuck~) external for Max 5. I tried to build and run it, but I couldn't get it to work with Max 8. This motivated me to try again.

- [David Braun] (the author of the very cool [DawDreamer](https://github.com/DBraun/DawDreamer) project which I am already a great fan of) for creating the excellent[ChucKDesigner](https://github.com/DBraun/ChucKDesigner) project which embeds chuck in a Touch Designer plugin. This project provides an excellent and super-clear blueprint on how to embed `libchuck` in another host or plugin system.

