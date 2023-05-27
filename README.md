# chuck-max

A project to embed [chuck](https://chuck.stanford.edu) in a Max/Msp external.

Currently, can both generate and process sound via the chuck engine by running chuck files with the ability to set and control global parameters via Max messages. Running files concurrently is possible to layer sound as well as various options for removing shreds.

But there as of this initial version, there is no support for callbacks and events except via the `signal` and `broadcast` messages.

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

- [ ] Add support for events and callbacks: ChucKDesigner has python callbacks, perhaps lua or js via an extension?

- [ ] Package chugins and scripts in external bundle

- [ ] Convert `chuginate` to python3 

- etc..


## Credits

Some time before this project started I was persuaded to try to get [Brad Garton's](http://sites.music.columbia.edu/brad) original [chuck~](http://sites.music.columbia.edu/brad/chuck~) external for Max 5 running, but I couldn't get it to work in Max 8.

Then I unsuccessfully attempted to create a puredata external against the latest Chuck apis in the [pd-chuck](https://github.com/shakfu/pd-chuck) project.

In this last attempt, I couldn't get Chuck to work with puredata's audio model. I asked for help on chuck's github issue tracker and received none. I asked for help in the chuck electro-music forum and received only one post which was appreciative of the effort but none too helpful. So, I left it for another day.

I'm not sure why I tried again with Max/MSP but it turned out to be a good idea because this time I just searched for prior art in Github, and found David Braun's excellent[ChucKDesigner](https://github.com/DBraun/ChucKDesigner) project which embeds chuck in a Touch Designer plugin.

David is a [CCRMA](https://github.com/CCRMA) (the home of chuck) alum and the author of the fantastic [DawDreamer](https://github.com/DBraun/DawDreamer) project which I am already a great fan of, and in this case, he had solved the same problem for Touch Designer that I was trying to solve for pd and Max/MSP which made his implementation invaluable to get this project going.

So in summary, this project owes a big thanks to two people: Brad Garton, for inspiring me with the original `chuck~` external which demonstrated that it could be done and for making his implementantion available to read and learn from.

And finally, to David Braun, for creating and making the code to ChucKDesigner available. It has been the best guide to help me implement this external.

