# VST3 Chugin Examples

This directory contains discrete examples demonstrating how to use the VST3 chugin in ChucK.

## Quick Start with Makefile

The easiest way to run examples is using the provided Makefile:

```bash
make help           # Show all available examples
make list           # List all VST3 plugins on your system
make basic-effect   # Run basic effect example
make instrument     # Run MIDI instrument example
make all            # Run all examples in order
```

## Running Examples Manually

Before running these examples, you'll need:

1. A VST3 plugin installed on your system
2. The VST3 chugin compiled (VST3.chug in parent directory)

To run an example manually:
```bash
chuck --chugin:../VST3.chug 00-list-vst3.ck
```

## Examples Overview

### Api Mapping

Methods used in examples:
- `vst.load(path)` -> `vst3_load returns int` (line 818-821)
- `vst.close()` -> `vst3_close` (line 823-824)
- `vst.list()` -> `vst3_list` (line 826-827)
- `vst.paramCount()` -> `vst3_get_param_count returns int` (line 851-852)
- `vst.paramName(index)` -> `vst3_get_param_name returns string` (line 847-849)
- `vst.getParam(index)` -> `vst3_get_param returns float` (line 839-841)
- `vst.setParam(index, value)` -> `vst3_set_param` (line 829-832)
- `vst.setParamByName(name, value)` -> `vst3_set_param_by_name returns int` (line 834-837)
- `vst.getParamByName(name)` -> `vst3_get_param_by_name returns float` (line 843-845)
- `vst.bypass(1/0)` -> `vst3_bypass` (line 875-877)
- `vst.isInstrument()` -> `vst3_is_instrument returns int` (line 904-905)
- `vst.noteOn(pitch, velocity)` -> `vst3_note_on returns int` (line 886-889)
- `vst.noteOff(pitch)` -> `vst3_note_off returns int` (line 891-893)
- `vst.controlChange(cc, value)` -> `vst3_control_change returns int` (line 895-898)
- `vst.programChange(program)` -> `vst3_program_change returns int` (line 900-902)
- `vst.presetCount()` -> `vst3_get_preset_count returns int` (line 865-866)
- `vst.presetName(index)` -> `vst3_get_preset_name returns string` (line 868-870)
- `vst.setPreset(index)` -> `vst3_set_preset returns int` (line 854-856)
- `vst.setPresetByName(name)` -> `vst3_set_preset_by_name returns int` (line 858-860)
- `vst.getPreset()` -> `vst3_get_preset returns int` (line 862-863)
- `vst.listPresets()` -> `vst3_list_presets` (line 872-873)

### Discovery and Setup

- **00-list-vst3.ck** - List all available VST3 plugins on your system

### Basic Usage

- **01-basic-effect.ck** - Load and use a VST3 plugin as an audio effect
- **02-parameters.ck** - Discover and control plugin parameters
- **03-dynamic-control.ck** - Real-time parameter modulation with sine wave
- **04-bypass.ck** - Toggle bypass to compare processed vs unprocessed audio

### Instruments and MIDI

- **05-instrument.ck** - Use VST3 plugins as sound generators/instruments
- **06-midi-simple.ck** - Basic MIDI note control
- **12-midi-control.ck** - MIDI control change and program change messages

### Advanced Parameter Control

- **08-parameter-sweep.ck** - Various parameter modulation techniques (linear, sine, triangle, random)
- **09-parameter-by-name.ck** - Control parameters by name instead of index

### Presets

- **10-presets.ck** - Discover and select plugin presets
- **11-preset-by-name.ck** - Select presets by name instead of index

### Multiple Effects

- **07-multiple-effects.ck** - Chain multiple VST3 plugins in series

## Common VST3 Plugin Locations

### macOS
- `/Library/Audio/Plug-Ins/VST3/`
- `~/Library/Audio/Plug-Ins/VST3/`

### Windows
- `C:\Program Files\Common Files\VST3\`
- `C:\Program Files (x86)\Common Files\VST3\`

### Linux
- `/usr/lib/vst3/`
- `/usr/local/lib/vst3/`
- `~/.vst3/`

## Tips

1. **Start with 00-list-vst3.ck** to see what VST3 plugins are available on your system
2. **Update plugin paths** in the examples to match your actual VST3 plugins
3. **Check parameter names** using examples/02-parameters.ck before using parameter-by-name functions
4. **Use isInstrument()** to check if a plugin is an instrument before sending MIDI

## VST3 Plugin Recommendations

Free VST3 plugins that work well for testing:
- **Effects**: TAL-Reverb, Valhalla FreqEcho, Tokyo Dawn Labs plugins
- **Instruments**: Surge XT, Vital, Dexed

## Troubleshooting

- If a plugin fails to load, check the path with `00-list-vst3.ck`
- Parameter names are plugin-specific - list them first with `02-parameters.ck`
- MIDI functions only work with instrument plugins - use `isInstrument()` to check
- Some plugins may have no presets - check `presetCount()` first
