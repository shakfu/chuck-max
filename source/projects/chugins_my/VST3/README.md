# VST3 Chugin

A ChucK chugin that allows loading and using VST3 plugins in ChucK.

## Important Note

The VST3 chugin currently has **linking issues with the VST3 SDK**. While the CLAP chugin works perfectly (no SDK required!), this VST3 chugin needs additional work to properly link against the VST3 SDK libraries.

**We recommend using the CLAP chugin instead** - it:
- Requires no external SDK
- Has a cleaner, modern API
- Is fully open source
- Works great with many plugins (Surge XT, Vital, etc.)

See `../CLAP/` for a working alternative!

## Status

This chugin compiles but has unresolved symbol errors during linking. The issues are:
- VST3 SDK libraries need to be rebuilt as universal binaries (x86_64 + arm64)
- Some SDK components may need to be compiled directly into the chugin
- The VST3 hosting API is complex and may require additional wrapper code

## Building (Not Currently Working)

### Requirements

1. Download the VST3 SDK from: https://www.steinberg.net/vst3sdk
2. Extract and build the SDK:

```bash
cd /path/to/vst3sdk
mkdir build && cd build
cmake ..
make
```

3. Set the SDK path:

```bash
export VST3_SDK_PATH=/path/to/vst3sdk
```

### Build Attempt

```bash
cd VST3
make mac
# Currently fails with linking errors
```

## Why Use CLAP Instead?

| Feature | CLAP | VST3 |
|---------|------|------|
| SDK Required | No (headers only) | Yes (complex build) |
| Licensing | Free, open | Free but restricted |
| Build Complexity | Simple | Complex |
| Status in ChucK | **Working** | Linking issues |
| Plugin Availability | Growing rapidly | Widely available |

## Contributing

If you'd like to help fix the VST3 linking issues, the main problems are in:
- `/Users/sa/projects/chugins/VST3/makefile.mac` - Library linking
- `/Users/sa/projects/chugins/VST3/VST3.cpp` - May need to compile SDK sources directly

Pull requests welcome!

## See Also

- **[CLAP chugin](../CLAP/)** - **Recommended working alternative**
- [AudioUnit chugin](../AudioUnit/) - macOS-only, but working
- [VST3 SDK Documentation](https://steinbergmedia.github.io/vst3_dev_portal/)
