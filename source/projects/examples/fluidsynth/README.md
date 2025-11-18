# Getting Fluidsynth to run

1. Make sure that `@import "FluidSynth";` is at the top of you `.ck` file.

2. Make sure that relevant `.sf2` or `.sf3` file is findable. So for example, in `fluidsynth-test.ck` we specify make it discoverable in a `sf2` directory of the chuck file's parent folder:

```c
f.open(me.dir() + "/sf2/FluidR3_GM.sf2");
```

Note that `FluidR3_GM.sf2` and `FluidR3_GS.sf2` are downloaded from github and installed in `examples/fluidsynth/sf2` via `make install_sf2`.


## Testing via the chuck exectuable

To test via the `chuck` executable:


```sh
cd examples
./chuck --chugin-path:`pwd`/chugins fluidsynth/fluidsynth-test.ck
```
