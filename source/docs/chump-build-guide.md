# Chump Build Guide

[chump](https://github.com/ccrma/chump) is experimentally included as subproject in this project and can be built with `cmake`.

It currently only works on MacOS and requires [Homebrew](https://brew.sh) to install the following dependencies:

```sh
brew install nlohmann-json openssl@3 minizip-ng zstd xz
```

## Building

```sh
make chump
```
