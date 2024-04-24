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

## [0.1.x]

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
