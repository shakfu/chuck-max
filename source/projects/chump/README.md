![CI/CD Badge](https://github.com/ccrma/chump/actions/workflows/build-unit-tests.yml/badge.svg)

# chump

![chump_logo](https://github.com/user-attachments/assets/6e0b9c20-de6b-4854-9e41-2285f1c24ca1)

## [As in sucker](https://www.merriam-webster.com/thesaurus/chump)

The ChucK Manager of Packages

## Building & Testing

### Linux
```
make linux # build
make test # run unit and integration tests
make install # install chump (can require sudo)
```

### Macs
```
make mac # build
make test # run unit and integration tests
make install # install chump (can require sudo)
```

To build a universal binary:
```
make mac-universal
```

### Windows
```
# configure build dir
meson setup builddir --backend vs
# Go to build dir
cd builddir
# Compile the project
meson compile
# Run unit tests
meson test

# If you want to run chump.exe, you need to run it in a meson devenv:
meson devenv -C .\builddir\
.\chump-cli\chump.exe

```
