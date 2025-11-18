# AudioUnit Chugin Examples

This directory contains examples demonstrating various features of the AudioUnit chugin.

## Running Examples

To run any example:
```bash
chuck examples/01-basic-effect.ck
```

Make sure the AudioUnit chugin is installed in your ChucK search path, or specify it explicitly:
```bash
chuck --chugin:/path/to/AudioUnit.chug examples/01-basic-effect.ck
```

## Example Overview

### Basic Usage

- **00-list-audiounit.ck** - List all available AudioUnits on your system
- **01-basic-effect.ck** - Load and use an AudioUnit as an audio effect
- **02-load-by-fourcc.ck** - Load AudioUnits using four-character codes

### Parameter Control

- **03-parameters.ck** - Discover and control AudioUnit parameters
- **04-dynamic-control.ck** - Real-time parameter modulation with sine wave
- **11-parameter-sweep.ck** - Various parameter modulation techniques (linear, sine, triangle, random)
- **12-parameter-by-name.ck** - Control parameters by name instead of index (more readable)

### Preset Control

- **13-presets.ck** - Discover and select AudioUnit factory presets by index (uses AUReverb with 13 presets)
- **14-preset-by-name.ck** - Select presets by name instead of index (uses AUDistortion with 22 presets)

### Advanced Features

- **05-bypass.ck** - Toggle AudioUnit bypass to compare processed vs unprocessed audio
- **06-generator.ck** - Using AudioUnits as sound generators/instruments

### MIDI Control

- **07-midi.ck** - **Option A:** Direct programmatic MIDI control
  - Send MIDI messages directly to AudioUnit instruments from ChucK code
  - Demonstrates noteOn, noteOff, controlChange, programChange, and sendMIDI
  - Immediate, programmatic control without external MIDI devices
  - **Working example with audio output**

- **07-midi-simple.ck** - Minimal MIDI example
  - Simple 6-note melody demonstrating basic MIDI control
  - Good starting point for learning MIDI with AudioUnits
  - Verified working with audio output

- **07-midi-routing.ck** - **Option C:** Virtual MIDI destination
  - Demonstrates the automatically-created virtual MIDI destination
  - Shows how to route external MIDI or ChucK's MidiIn/MidiOut to AudioUnits
  - Enables integration with MIDI controllers, DAWs, and other software

### Multi-Effect Usage

- **08-multiple-effects.ck** - Chain multiple AudioUnits in series
- **09-switch-effects.ck** - Dynamically switch between different AudioUnits
- **10-live-input.ck** - Process live audio input through multiple effects

## Prerequisites

- **macOS** - AudioUnits are only available on macOS
- **ChucK** - Version 1.5.0.0 or higher recommended
- **AudioUnit plugins** - Built-in Apple AudioUnits should work out of the box

## Common AudioUnits

Most examples use these Apple AudioUnits which are available on all macOS systems:

### Effects
- `AUDelay` - Delay effect
- `AULowpass` - Low-pass filter
- `AUHighpass` - High-pass filter
- `AUBandpass` - Band-pass filter
- `AUReverb` - Reverb effect (13 factory presets)
- `AUDistortion` - Distortion effect (22 factory presets)
- `AUDynamicsProcessor` - Compressor/limiter/expander/gate
- `AUPeakLimiter` - Peak limiter
- `AUNBandEQ` - Multi-band parametric EQ

### Instruments
- `DLSMusicDevice` - Apple's DLS-based synthesizer

## Troubleshooting

**No sound output:**
- Check that your audio interface is properly configured
- Verify the AudioUnit loaded successfully (check console messages)
- For input-based examples, ensure your microphone/line-in is connected

**AudioUnit fails to load:**
- Run `00-list-audiounit.ck` to see available AudioUnits
- Some third-party AudioUnits may have specific requirements
- Verify the AudioUnit name or four-character code is correct

**Parameters don't work as expected:**
- Different AudioUnits have different parameter ranges
- Use `03-parameters.ck` to discover parameter names and current values
- Some parameters may be read-only
- Use `setParamByName()` and `getParamByName()` for more readable code (see `12-parameter-by-name.ck`)

**Presets not available:**
- Many AudioUnits (including Apple's built-in effects) don't define factory presets
- This is normal behavior - not all AudioUnits provide presets
- Third-party synthesizers and effects are more likely to have factory presets
- Use `13-presets.ck` or call `listPresets()` to see what's available for any AudioUnit

## Tips

1. **Discover before using**: Run `00-list-audiounit.ck` first to see what's available
2. **Check parameters**: Use `03-parameters.ck` as a template to explore any AudioUnit's parameters
3. **Check presets**: Use `13-presets.ck` or `listPresets()` to discover factory presets (if available)
4. **Start simple**: Begin with `01-basic-effect.ck` before moving to complex chains
5. **MIDI instruments**: When using MusicDevice AudioUnits, always set gain (e.g., `0.5 => synth.gain`) to hear output
6. **Test MIDI first**: Use `07-midi-simple.ck` to verify MIDI is working before trying complex examples
7. **Prefer names over indices**: Use `setParamByName()` and `setPresetByName()` for more maintainable code
8. **Experiment**: Try different AudioUnits, parameter values, and presets to find interesting sounds

## Further Reading

See the main README.md in the AudioUnit directory for complete API reference and additional documentation.