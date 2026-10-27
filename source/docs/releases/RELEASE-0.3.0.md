## Overview

chuck-max `v0.3.0` adds a reply outlet, six host classes bound into every VM with `ChucK::bind()`, a `@verbose` attribute, an `abort` message and multichannel tapping. It updates the engine to `ChucK 1.5.5.9-dev (chai)`. It also ships everything recorded under 0.2.1 and 0.2.2 in `CHANGELOG.md`, which were never released: `@ntap`, `param`, `shreds`, `adaptive`, the `VST3`, `CLAP`, `AudioUnit` and `AbletonLink` chugins, and the fix for issue #11.

## What's New in v0.3.0

### Reply Outlet

A new rightmost, non-signal outlet reports data to the patch instead of only to the Max console.

- `val <name> <value...>` answers `get`.
- `event <name>` fires while a `listen` is active.
- `shred add <id>` and `shred remove <id>` report shred changes.
- `global <name> <type>` answers `globals`.
- `msg <name> <args...>` carries `Host.msg()` calls from ChucK.

The selector is always one of these five words, so no ChucK name can collide with a control message. Route with `[route val event shred global msg]`.

### Host Classes

`chuck~` binds these classes into every VM. The names are shared with pd-chuck and numchuck, so one `.ck` file runs in each host.

- `Host.msg(name, ...)` sends `msg <name> <args...>` to the reply outlet. Overloads take nothing, a float, a string or a `float[]`.
- `Host.sendBang`, `sendFloat`, `sendSymbol` and `sendList` deliver to every `[receive <name>]`, as `[send]` does.
- `HostIn` / `HostMsg` receive the new `msg <name> <args...>` inlet message, with mixed-type arguments, in the style of `OscIn` / `OscMsg`.
- `HostBuffer` reads and writes a named `buffer~`. `HostBufPlay` is a UGen that plays one channel of it, with `rate`, `loop`, `pos`, `interp` and `channel`.
- `HostClock` extends `Event` and exposes the Max transport: `ticks()`, `tempo()`, `running()`, `beat()`, `toNextBeat()`, `toNext()`, `bar()`, `beatInBar()` and `unit()`. It broadcasts on start, stop, tempo or time signature change, and jumps.

Examples: `examples/max/max_*.ck` and `patchers/tests/test_max_*.maxpat`. Design notes: `source/docs/dev/bind.md`. Usage: the Host Classes section of `README.md`.

### Tap Outlets

- `@ntap <M>` adds `M` outlets that read samples from global UGens.
- `tap <ugen>`, `tap <outlet> <ugen>`, `tap <outlet>` and `tap` assign and clear outlets.
- `tap <outlet> <ugen> <nchannels>` spreads a multichannel UGen such as `Pan2` across consecutive outlets. Each sub-channel needs its own `buffered` flag.

### Messages and Attributes

- `abort` aborts the shred executing inside the VM. It reaches a shred that loops without advancing time, which `remove` cannot.
- `@verbose` (0 quiet, 1 normal, 2 debug) sets this object's reporting, separately from the VM's `loglevel`. `loglevel` is now process-global, as `ChucK::setLogLevel()` is. Warnings and query answers (`status`, `vm`, `chugins`, `loglevel`) now always report.
- `param`, `param <name>` and `param <name> <value>` query and set VM parameters. `CHUCK_PARAM_OTF_PRINT_WARNINGS` is included.
- `shreds`, `shreds ready|blocked`, `shreds <id>` and `shreds highest|last|next|count` introspect shreds.
- `removeall` removes every shred and reports the count. Global UGens survive it; use `reset` to clear them.
- `adaptive` and `adaptive <size>` control adaptive block processing.
- `run_needs_audio` defaults to 1, as a safeguard for issue #11.

### Fixes

- Sample rate changes in Max now reach the VM. The rate was captured once at object creation, so a later change made pitch and timing drift.
- `set float[i]` and `set float[k]` keep the fractional part of each value. It was truncated through a `long`.
- `ChucK::globalCleanup()` runs once, when the last `chuck~` is freed. It shut down HID, serial and keyboard input for every instance whenever any one was freed.
- `get` replies and event notifications reach the object that asked. The callback-id overloads are now used.
- `ck_edit` opens files from paths that contain spaces, such as the Max `Packages` directory.
- 13 format string mismatches, mostly `%d` applied to 64-bit types.
- `HostClock.bar()`, `beatInBar()` and `unit()` on an exact beat boundary answered for the previous beat.
- Crash when running files with custom events after `clear vm` (issue #11, chuck #537).
- `AudioUnit` effects process their input instead of passing it through. `CLAP` compiles on Windows.
- Command injection and path traversal hardening in `ck_edit` and `ck_check_file`.

### Documentation

- `help/chuck~.maxhelp` has a new `advanced` tab covering the reply outlet, taps, typed `get` and `set`, events and VM introspection. The previous content is the `main` tab.
- `README.md` documents the reply outlet, the host classes, taps, `param`, `shreds` and `adaptive`.
- `source/docs/logging.md` documents the console and outlet reporting channels.
- `patchers/tests/test_chuck_tilde.maxpat` and `examples/test/help_features.ck` demonstrate the current message set.

## Technical Improvements

- ChucK core and examples updated to `1.5.5.9-dev (chai)`.
- C++17, for `std::filesystem::path`.
- `source/scripts/update.sh` restores the `examples/max/max_*.ck` bind examples from `source/projects/examples/max/` and applies `source/scripts/patches/` after each upstream update.
- `CHUCK_PARAM_IS_REALTIME_AUDIO_HINT` is set at startup.
- `chout` and `cherr` are captured per instance, so `<<< >>>` output reaches the Max console.
- Compile-time `printf` format checking on the logging helpers.

## Known Issues

- `Faust.chug`, `WarpBuf.chug` and `FluidSynth.chug` are not available on Windows.
- `Host.msg` and `Host.send*` deliver on the main thread. They are for control data, not for clocking.
- `HostClock` reads the global transport only. Named transports are not supported.
- `HostBufPlay` is mono. Stereo takes two instances.

## Additional Resources

- **Documentation**: `README.md` and `help/chuck~.maxhelp`
- **Examples**: the `examples/` directory; host class examples in `examples/max/`
- **Community**: the chuck-max channel of the ChucK discord and the project's GitHub repository
- **Cheatsheet**: the PDF cheatsheet in the `media/` folder

## Information about the Release Build Variants

All macOS externals and binaries are codesigned and notarized and can be used in other packages and standalones.

### MacOS Releases

- [chuck-max-0.3.0-macos-arm64.dmg](https://github.com/shakfu/chuck-max/releases/download/0.3.0/chuck-max-0.3.0-macos-arm64.dmg) - Full Max package for Apple Silicon Macs: the `chuck~` external, ChucK examples, all CCRMA chugins, and `Faust.chug`, `WarpBuf.chug` and `FluidSynth.chug` in the `examples/chugins` folder.

- [chuck-max-0.3.0-macos-arm64-bundled.dmg](https://github.com/shakfu/chuck-max/releases/download/0.3.0/chuck-max-0.3.0-macos-arm64-bundled.dmg) - The `BUNDLED=1` variant: as above, with the chugins embedded in the external's `Resources` folder.

- [chuck-max-0.3.0-macos-universal.dmg](https://github.com/shakfu/chuck-max/releases/download/0.3.0/chuck-max-0.3.0-macos-universal.dmg) - The `UNIVERSAL=1` build with arm64 and x86_64 binaries.

- [chuck-max-0.3.0-macos-x86_64.dmg](https://github.com/shakfu/chuck-max/releases/download/0.3.0/chuck-max-0.3.0-macos-x86_64.dmg) - A build for Intel Macs.

### Windows Releases

- [chuck-max-0.3.0-windows-amd64.zip](https://github.com/shakfu/chuck-max/releases/download/0.3.0/chuck-max-0.3.0-windows-amd64.zip) - Base Max package for Windows, without `Faust.chug`, `WarpBuf.chug` and `FluidSynth.chug`.

### Cross-Platform Releases

- [chuck-max-0.3.0-multi.zip](https://github.com/shakfu/chuck-max/releases/download/0.3.0/chuck-max-0.3.0-multi.zip) - A cross-platform Max package with the macOS arm64 and Windows externals, ChucK examples, CCRMA chugins for both platforms, and `Faust.chug`, `WarpBuf.chug` and `FluidSynth.chug` for the macOS external.
