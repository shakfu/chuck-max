# CLAP Chugin Examples

This directory contains examples demonstrating various features of the CLAP chugin.

## Running Examples

### Using the Makefile (Recommended)

The examples directory includes a Makefile for easy execution:

```bash
# Show all available examples
make help

# Run a specific example
make basic-effect
make parameters
make multi-plugin

# Run all CLAP2 examples in order
make all

# Run all legacy CLAP examples
make legacy
```

### Running Manually

To run any example directly:
```bash
chuck examples/01-basic-effect.ck
```

Make sure the CLAP chugin is installed in your ChucK search path, or specify it explicitly:
```bash
chuck --chugin:../CLAP.chug 01-basic-effect.ck
```

## Example Overview

### CLAP2 Examples (Numbered Series)

These examples demonstrate the full feature set of CLAP2:

#### Basic Usage
- **00-list-clap.ck** - List all available CLAP plugins on your system
  - `make list`
- **01-basic-effect.ck** - Load and use a CLAP plugin as an audio effect
  - `make basic-effect`
- **02-parameters.ck** - Discover and control CLAP plugin parameters
  - `make parameters`

#### Parameter Control
- **03-dynamic-control.ck** - Real-time parameter modulation with sine wave
  - `make dynamic-control`
- **08-parameter-sweep.ck** - Various parameter modulation techniques (linear, sine, triangle, random)
  - `make parameter-sweep`
- **09-parameter-by-name.ck** - Control parameters by name instead of index (more readable)
  - `make parameter-by-name`

#### Advanced Features
- **04-bypass.ck** - Toggle CLAP bypass to compare processed vs unprocessed audio
  - `make bypass`
- **05-instrument.ck** - Using CLAP plugins as sound generators/instruments
  - `make instrument`
- **06-midi-simple.ck** - Simple MIDI control of CLAP instruments
  - `make midi-simple`
- **07-multiple-effects.ck** - Chain multiple CLAP plugins in series
  - `make multiple-effects`
- **10-multi-plugin.ck** - **Multi-plugin bundle support** - enumerate and switch between plugins
  - `make multi-plugin`

### Legacy CLAP Examples

These examples are from the original CLAP implementation and work with CLAP2:

- **Clap-midi-demo.ck** - Comprehensive MIDI demonstration
  - `make demo`
- **Clap-test.ck** - General CLAP functionality test suite
  - `make clap-test`
- **TAL-test.ck** - TAL plugin-specific test
  - `make tal`
- **test-midi.ck** - Basic MIDI test
  - `make midi`
- **test-midi-melody.ck** - MIDI melody playback test
  - `make melody`

## Prerequisites

- **ChucK** - Version 1.5.0.0 or higher recommended
- **CLAP plugins** - Install free CLAP plugins (see below)

## CLAP Plugin Locations

CLAP plugins are typically installed in these locations:

### macOS
- `/Library/Audio/Plug-Ins/CLAP/` (system-wide)
- `~/Library/Audio/Plug-Ins/CLAP/` (user)

### Windows
- `C:\Program Files\Common Files\CLAP\`
- `%LOCALAPPDATA%\Programs\Common\CLAP\`

### Linux
- `/usr/lib/clap/`
- `/usr/local/lib/clap/`
- `~/.clap/`

### Environment Variable
You can also set the `CLAP_PATH` environment variable to add additional search directories.

## Free CLAP Plugins to Try

Here are some excellent free CLAP plugins you can use with these examples:

### Instruments
- **[Surge XT](https://surge-synthesizer.github.io/)** - Hybrid synthesizer with wavetable, subtractive, and FM synthesis
- **[Vital](https://vital.audio/)** - Modern wavetable synthesizer
- **[Dexed](https://asb2m10.github.io/dexed/)** - FM synthesizer (Yamaha DX7 clone)
- **[Cardinal](https://cardinal.kx.studio/)** - Modular synthesizer (VCV Rack)

### Effects
- **[Airwindows](https://www.airwindows.com/)** - Large collection of unique effects
- **[Chow Tape Model](https://chowdsp.com/products.html)** - Analog tape emulation
- **[Dragonfly Reverb](https://michaelwillis.github.io/dragonfly-reverb/)** - High-quality reverb
- **[Surge XT Effects](https://surge-synthesizer.github.io/)** - Individual effects from Surge XT

## Troubleshooting

**No plugins found:**
- Run `00-list-clap.ck` to see what CLAP plugins are detected
- Install CLAP plugins in standard locations listed above
- Set `CLAP_PATH` environment variable to your plugin directory

**Plugin fails to load:**
- Verify the plugin path is correct and the `.clap` file exists
- Make sure the plugin is compatible with your system architecture
- Check console output for specific error messages

**No audio output from instrument:**
- Send MIDI note on messages: `clap.noteOn(60, 100)`
- Check if `clap.isInstrument()` returns true
- Set gain: `0.5 => clap.gain`

**Parameters don't work as expected:**
- Different plugins have different parameter ranges and names
- Use `02-parameters.ck` to discover parameter names and current values
- Use `setParamByName()` and `getParamByName()` for more readable code (see `09-parameter-by-name.ck`)

## Tips

1. **Discover before using**: Run `00-list-clap.ck` first to see what's available
2. **Check parameters**: Use `02-parameters.ck` as a template to explore any CLAP plugin's parameters
3. **Start simple**: Begin with `01-basic-effect.ck` before moving to complex chains
4. **MIDI instruments**: When using CLAP instruments, always set gain (e.g., `0.5 => clap.gain`) to hear output
5. **Test MIDI first**: Use `06-midi-simple.ck` to verify MIDI is working before trying complex examples
6. **Prefer names over indices**: Use `setParamByName()` for more maintainable code
7. **Experiment**: Try different CLAP plugins, parameter values, and modulation techniques

## Updating Plugin Paths

Most examples use placeholder paths. Update them to match your system:

```chuck
// Replace this:
"/Library/Audio/Plug-Ins/CLAP/ExampleDelay.clap" => string pluginPath;

// With your actual plugin path:
"/Library/Audio/Plug-Ins/CLAP/SurgeXT.clap" => string pluginPath;
```

Run `00-list-clap.ck` to find the correct paths on your system.

## Further Reading

See the main README.md in the CLAP directory for complete API reference and additional documentation.
