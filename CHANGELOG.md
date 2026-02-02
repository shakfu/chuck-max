# Changelog

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) and [Commons Changelog](https://common-changelog.org). This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## Types of Changes

- Added: for new features.
- Changed: for changes in existing functionality.
- Deprecated: for soon-to-be removed features.
- Removed: for now removed features.
- Fixed: for any bug fixes.
- Security: in case of vulnerabilities.

---

## [0.2.x]

### Changed

- Made `run_needs_audio` default to 1 to prevent issue #11 (loading files before audio)

### New Features

#### New ChucK API Messages

- Added `@tap` attribute and `tap` message for reading audio samples from global UGens
  - Enables tapping into global UGens declared in ChucK code for signal routing to Max
  - Supports mono and multichannel (up to 16 channels) tap configurations
  - Usage: `[chuck~ 2 @tap 2]` creates 2 audio channels + 2 tap outlets
  - Send `tap <ugen_name>` message to specify which global UGen to read from

- Added `param` message for querying and setting ChucK VM parameters
  - `param` (no args): lists all available parameters with their current values
  - `param <name>`: get specific parameter value
  - `param <name> <value>`: set parameter value
  - Supports integer, string, and string list parameter types

- Added `shreds` message for shred introspection
  - `shreds`: list all shreds with IDs, names, and state
  - `shreds ready`: list ready (running) shreds
  - `shreds blocked`: list blocked (waiting) shreds
  - `shreds <id>`: detailed info about specific shred
  - `shreds highest/last/next/count`: query shred ID info

- Added `removeall` message as a direct way to remove all shreds
  - Equivalent to `remove all` but more discoverable
  - Unlike `clear vm`, preserves VM state (globals, etc.)

- Added `adaptive` message for runtime control of adaptive block processing
  - `adaptive`: query current adaptive mode status
  - `adaptive <size>`: set adaptive mode (0 to disable, >1 to enable with max block size)
  - Can improve CPU performance for certain workloads

#### ChucK and Chugins Updates

- Updated `chuck` and examples to version `1.5.5.7-dev (chai)`

- Added `VST3` chugin

- Added `CLAP` chugin (macOS and Windows)

- Added `AudioUnit` chugin (macOS only)

- Added `AbletonLink` chugin for tempo synchronization with Link-enabled applications
  - Real-time tempo and beat grid synchronization across network
  - Compatible with Ableton Live, Max/MSP, and other Link-enabled software
  - Configurable quantum, resolution, and latency offset settings
  - Thread-safe implementation optimized for audio processing

#### Other

- Added improved auto-update script to make it easier to update examples

### Security and Stability Fixes

- Security
  - Fixed potential command injection vulnerability in `ck_edit` function by adding input validation for editor and file paths
  
  - Added path validation function `is_safe_path()` to prevent directory traversal attacks in file operations
  
  - Enhanced `ck_check_file()` with security validation to block unsafe path patterns (../, absolute paths, UNC paths)

- Fixed:
  - Memory management in `ck_free()` function now includes null checks before deletion to prevent crashes

  - Control flow errors in `ck_check_file()` by adding missing braces around multi-line if statements

  - Enhanced error handling in `ck_edit()` with proper exception handling and validation

  - AudioUnit chugin audio effects now properly process input audio instead of passing through unmodified
    - Added input sample storage and render callback for all AudioUnit types
    - Effects now receive actual audio input while instruments continue to receive silent input
    - Fixes issue where AudioUnit effects like AUDelay were not processing audio in Max/MSP context

  - CLAP chugin now compiles on Windows
    - Added cross-platform dynamic library loading (LoadLibrary/GetProcAddress on Windows, dlopen/dlsym on POSIX)
    - Added cross-platform directory enumeration (FindFirstFile/FindNextFile on Windows, opendir/readdir on POSIX)
    - Fixed CMakeLists.txt to only apply macOS-specific linker flags on Apple platforms

## [0.2.1]

- Fixed a bug in `ck_edit` function in `chuck_tilde.cpp` which caused the editor to fail to open a chuck file if the package was installed in Max packages directory (because that path has a space). Properly quoting the file path did the trick!

- Changed C++ standard used in this project from 14 to 17 to make available `std::filesystem::path`.

## [0.2.0] (Release)

- Added cross-platform package with prefix `-multi` which includes macos arm64 and windows amd64 support.

- Added additional support to building via the alternative `go-task` method.

- Added support for bundling chugins in the external's bundle via `BUNDLED=1` option

- Added support for building (for both MacOS and Windows) `ChuGL.chug`.

- Added new `icon.png`

- Added support for `Regex.chug` chugin for windows build.

- Added Chump as a cmake-compilable subproject.

- Added Windows support to build `chuck~` and standard chugins (except `Regex`).

- Updated ChucK and ChucK examples to `ChucK 1.5.5.3-dev (chai)`

## [0.1.3]

- Added `XML.chug`, last remaining CCRMA chugin.

- Added `Ladspa.chug`.

- `FluidSynth` now works! Can be built if dependencies are available (see README), also added a script to download a couple of standard soundfonts using `make install_sf2`

- Updated chuck to `chuck 1.5.5.1-dev (chai)`

- Updated chuck code base and examples to `chuck 1.5.4.5-dev (chai)`

- Defaults to Max 9 builds

- Updated chuck code base to `chuck 1.5.4.5-dev`

- Added `ConvRev.chug`, `FluidSynth.chug` and `Line.chug`

- Updated chuck code base and examples to `1.5.4.2-dev (chai)`

- Added MAX_VERSION env variable for MAX 9 compatibility

- Tested sucessfully on Max 9

- Updated chuck code base to `1.5.3.2-dev (chai)`

- Updated chuck code base to `1.5.2.6-dev (chai)`

- Added [pdf cheatsheet of chuck~ methods](https://github.com/shakfu/chuck-max/blob/main/media/chuck-max-cheatsheat.pdf)

- Changed `add` syntax: `add <filename:arg1:arg2:..:argN` is no longer allowed due to the ambiguity entailed by Max automatically adding volname prefixes such `Macintosh HD:`. Args must **only** be provided as Max-friendly space separated format: `add <filename> arg1 arg2 .. argN`

- Added `nomp3` build variant for a `full - mp3` build (works better on older macOS systems)

- Added Makefile-based packaging, codesigning and notarization.

## [0.1.2]

- Added a github workflow to automate building the external (no codesigning/notarizing yet)

- Added improved and simplified build guid in README

- Added improved build system using mostly homebrew via `make brew`

- Added test for .mp3 use by `WarpBuf`

- Added mp3 support to libsndfile-using chugins {`Fauck`, `WarpBuf`}

- Added alternative max-friendly style for `add` such that `<filename> arg1 arg2 .. argN` is interpretered as `<filename>:arg1:arg2:argN`

- Added tested for chuck `add` colon-separated args

- Added `ck_info`, `ck_warn`, `ck_debug` for more control and consistency wrt console logging.

- Removed `info` message and merged its functionality with `status` message.

- Reorganized `patchers/contrib` folder given prior bug fix.

- Fix for file searching bug bug which now makes it possible in the case of `[chuck~ 2 abc.ck]` for `abc.ck` to be in found in the same folder as the Max patch which references it.

- Thanks to Professor @gewang for [PR-16](https://github.com/shakfu/chuck-max/pull/16) which fixed the Faust 'crash-on-clone' issue!

- Added `eval` method for adding code from max messages and the textedit widget (see `test_eval.maxpat`).

## [0.1.1]

see [prerelease 0.1.1](https://github.com/shakfu/chuck-max/releases/tag/0.1.1)

- Added `run_needs_audio` attribute to ensure that adding or running shred can only happen when dsp is on.

- Added patcher directory to search paths for `add`ed chuck file.

- Added improved callback demos for global vars

- Changed `remove` message to able to take more than than one shred_id

- Added test for use of global variables and a practical test of its use in a musical patch.

- Fixed bug which prevented sending lists of atoms as chuck arrays.

- Improved path handling: run, add, replace, edit now all use same mechanism to check file validity, fix partial path if file is in `examples`, or otherwise search in Max search path.

- Changed / improved `chuck~.maxhelp` to make it clearer and more readable.

- Added `docs` message to open chuck documentation in browser

- Updated chuck code base to `1.5.2.5-dev (chai)`

- Added fallback search for file based on `locatefile_extended` if given path is not absolute or found in the `chuck-max/examples` folder.

- Added support for setting file and editor attributes, to facilitate use of external editor via setting inspector vars or via setting `editor <path>` and `edit <filename>`. Also double-clicking on the `chuck~` object will open the current file in the specified external editor.

- The`add` and `replace` messages now both handle chuck style file args:  `add <path>:<arg1>:<arg2>:<argN>`  While this is consistent without chuck treats file arguments, there is some thought to making this more readable and Max friendly in the future by dropping the colons to adopt the following style: `add <path> <arg1> <arg2> ... <argN>`

- Fixed `chuck~` object args to allow just the number of channels as args.

- Added midi player to .maxhelp to demonstrate midi file playback of `chuck~` from Max.

- Added `loglevel` / `loglevel <int>` message for setting chuck log level 0-10. Dropped redundant previous `debug` switch/attribute.

- Added a number of missing chuck messages: `add <filepath>`, `replace <shredID> <filepath>`, `reset id`, `clear vm`, `clear globals`, `time`, `status` along with their respective symbols: `+`, `-`, `--`, `=`, `^`, etc.

- Changed back to keeping all chugins in `chuck-max/examples/chugins` (alternative is to move chugins and faust stdlib to `support`)

- Added support for building the `Faust` or `Fauck` chugin with the `faust`
stdlib in `chuck-max/examples/faust`

- Added support for building the `WarpBuf` chugin

- Changed the content and structure of the `examples` folder to more closely resemble the updated chuck examples folder. See the `README.md` file in the folder for a more granular list of changes.

- For macOS, chugins are now installed in the external's `Resources/chugins` folder. The `chugins` folder in `examples` is still searched for chugins in any case

- Removed posix header includes and replaced with x-platform code in aid of eventual windows support

- Added callback mechanism and example

- Added examples for local and global event triggering

- Fixed the `sndbuf.ck` exmample (and added `sndbuf1.ck`) to demonstrate wav file playback via `SndBuf` with some samples

- Changed `signal` message name to `sig` as it conflicted with a global Max message `signal`

- Added optional channels argument which to set more channels at object creation

- Changed `info`: now shows `<object id> - <shred-id>`

- Added test for two object instances running simultaneously

- Added `info` message to list running shreds in the console

- Update to chuck 1.5.2.3-dev (chai)

## [0.1.0]

- Initial support for chuck 1.5.1.3

- Support for Linux

- Support for macOS
