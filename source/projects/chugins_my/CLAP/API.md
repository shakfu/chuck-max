# CLAP Chugin API Reference

The CLAP chugin enables loading and using CLAP (CLever Audio Plugin) format plugins in ChucK. CLAP is an open-source plugin standard supporting both effects and instruments.

## Table of Contents

- [Plugin Loading](#plugin-loading)
- [Multi-Plugin Support](#multi-plugin-support)
- [Parameter Control](#parameter-control)
- [MIDI Support](#midi-support)
- [Preset Management](#preset-management)
- [System Discovery](#system-discovery)
- [Audio Chain Setup](#audio-chain-setup)
- [Return Values](#return-values)

---

## Plugin Loading

### `load(string path) -> int`

Load a CLAP plugin from a file path.

**Parameters:**
- `path` - Absolute or relative path to the CLAP plugin file

**Returns:**
- `1` on success
- `0` on failure

**Notes:**
- On macOS, you can pass either the bundle path (`/path/to/Plugin.clap`) or the binary path (`/path/to/Plugin.clap/Contents/MacOS/Plugin`). The chugin automatically resolves bundle paths.
- For multi-plugin bundles, the first plugin (index 0) is auto-activated by default.

**Example:**
```chuck
Clap clap;
clap.load("/Library/Audio/Plug-Ins/CLAP/TAL-Reverb-4.clap") => int success;
if (!success) {
    <<< "Failed to load plugin" >>>;
}
```

### `close() -> void`

Close the currently loaded CLAP plugin and free all resources.

**Example:**
```chuck
clap.close();
```

---

## Multi-Plugin Support

Some CLAP files contain multiple plugins in a single bundle. These methods manage multi-plugin bundles.

### `pluginCount() -> int`

Get the number of plugins in the loaded bundle.

**Returns:** Number of plugins (typically 1)

**Example:**
```chuck
clap.pluginCount() => int count;
<<< "Bundle contains", count, "plugin(s)" >>>;
```

### `listPlugins() -> int`

List all plugins in the currently loaded bundle to console.

**Returns:** Number of plugins found

**Example:**
```chuck
clap.listPlugins() => int count;
// Prints:
// [CLAP]: Plugins in loaded bundle:
// ----------------------------------------
// [0] Plugin Name
//      Plugin description
// ----------------------------------------
// Total: 1 plugin(s)
```

### `pluginName(int index) -> string`

Get the name of a plugin by index.

**Parameters:**
- `index` - Plugin index (0-based)

**Returns:** Plugin name, or empty string if index is invalid

**Example:**
```chuck
for (0 => int i; i < clap.pluginCount(); i++) {
    <<< "Plugin", i, ":", clap.pluginName(i) >>>;
}
```

### `activatePlugin(int index) -> int`

Activate a specific plugin by index. Deactivates the currently active plugin.

**Parameters:**
- `index` - Plugin index (0-based)

**Returns:**
- `1` on success
- `0` on failure

**Notes:**
- Only needed for multi-plugin bundles
- The first plugin is auto-activated when loading

**Example:**
```chuck
clap.activatePlugin(1) => int success;  // Switch to second plugin
```

---

## Parameter Control

CLAP supports three methods for parameter access: by index (fastest), by parameter ID, or by name (most readable).

### `paramCount() -> int`

Get the number of parameters available.

**Returns:** Number of parameters

**Example:**
```chuck
clap.paramCount() => int count;
<<< "Plugin has", count, "parameters" >>>;
```

### `paramName(int index) -> string`

Get a parameter name by index.

**Parameters:**
- `index` - Parameter index (0-based)

**Returns:** Parameter name, or empty string if invalid

**Example:**
```chuck
for (0 => int i; i < clap.paramCount(); i++) {
    <<< i, ":", clap.paramName(i), "=", clap.getParam(i) >>>;
}
```

### `setParam(int index, float value) -> void`

Set a parameter value by index (fastest method).

**Parameters:**
- `index` - Parameter index (0-based)
- `value` - Normalized parameter value (typically 0.0 to 1.0)

**Example:**
```chuck
clap.setParam(0, 0.5);  // Set first parameter to 0.5
```

### `getParam(int index) -> float`

Get a parameter value by index.

**Parameters:**
- `index` - Parameter index (0-based)

**Returns:** Normalized parameter value, or 0.0 if invalid

**Example:**
```chuck
clap.getParam(0) => float value;
<<< "Parameter 0 value:", value >>>;
```

### `setParamByName(string name, float value) -> int`

Set a parameter value by name (most readable method).

**Parameters:**
- `name` - Parameter name (exact match required)
- `value` - Normalized parameter value

**Returns:**
- `1` on success
- `0` if parameter not found

**Example:**
```chuck
clap.setParamByName("Cutoff", 0.75) => int success;
clap.setParamByName("Resonance", 0.3);
```

### `getParamByName(string name) -> float`

Get a parameter value by name.

**Parameters:**
- `name` - Parameter name (exact match required)

**Returns:** Parameter value, or 0.0 if not found

**Example:**
```chuck
clap.getParamByName("Wet") => float wetLevel;
<<< "Wet level:", wetLevel >>>;
```

### `bypass(int bypass) -> void`

Bypass the CLAP plugin (pass audio through unprocessed).

**Parameters:**
- `bypass` - `1` to bypass, `0` to activate

**Example:**
```chuck
clap.bypass(1);  // Bypass plugin
1::second => now;
clap.bypass(0);  // Re-activate plugin
```

---

## MIDI Support

CLAP plugins marked as instruments can receive MIDI messages. Check `isInstrument()` before using MIDI methods.

### `isInstrument() -> int`

Check if the loaded CLAP plugin is an instrument.

**Returns:**
- `1` if plugin is an instrument/synthesizer
- `0` if plugin is an effect

**Example:**
```chuck
if (clap.isInstrument()) {
    <<< "This is an instrument plugin" >>>;
}
```

### `noteOn(int pitch, int velocity) -> int`

Send MIDI note-on message to the plugin (channel 0).

**Parameters:**
- `pitch` - MIDI note number (0-127)
- `velocity` - Note velocity (0-127)

**Returns:**
- `1` on success
- `0` on failure (not an instrument or plugin not loaded)

**Example:**
```chuck
clap.noteOn(60, 100);  // Middle C, forte
500::ms => now;
clap.noteOff(60);
```

### `noteOff(int pitch) -> int`

Send MIDI note-off message to the plugin (channel 0).

**Parameters:**
- `pitch` - MIDI note number (0-127)

**Returns:**
- `1` on success
- `0` on failure

**Example:**
```chuck
clap.noteOff(60);  // Release middle C
```

### `controlChange(int cc, int value) -> int`

Send MIDI control change message (channel 0).

**Parameters:**
- `cc` - Controller number (0-127)
- `value` - Controller value (0-127)

**Returns:**
- `1` on success
- `0` on failure

**Example:**
```chuck
clap.controlChange(1, 64);  // Modulation wheel to center
```

### `sendMIDI(int status, int data1, int data2) -> int`

Send raw MIDI message to the plugin.

**Parameters:**
- `status` - MIDI status byte (includes channel in lower 4 bits)
- `data1` - First data byte
- `data2` - Second data byte

**Returns:**
- `1` on success
- `0` on failure

**Notes:**
- Status byte format: `0xMN` where M is message type, N is channel
- Note On: `0x90` | channel
- Note Off: `0x80` | channel
- Control Change: `0xB0` | channel

**Example:**
```chuck
clap.sendMIDI(0x90, 60, 100);  // Note On, channel 0, C4, velocity 100
```

---

## Preset Management

CLAP plugins can support presets through the preset discovery system. Not all plugins provide presets.

### `presetCount() -> int`

Get the number of available presets.

**Returns:** Number of discovered presets (may be 0)

**Example:**
```chuck
clap.presetCount() => int count;
if (count > 0) {
    <<< "Found", count, "presets" >>>;
}
```

### `presetName(int index) -> string`

Get the name of a preset by index.

**Parameters:**
- `index` - Preset index (0-based)

**Returns:** Preset name, or empty string if invalid

**Example:**
```chuck
for (0 => int i; i < clap.presetCount(); i++) {
    <<< i, ":", clap.presetName(i) >>>;
}
```

### `loadPreset(int index) -> int`

Load a preset by index.

**Parameters:**
- `index` - Preset index (0-based)

**Returns:**
- `1` on success
- `0` on failure (invalid index, plugin doesn't support presets, or load failed)

**Example:**
```chuck
clap.loadPreset(0) => int success;  // Load first preset
if (success) {
    <<< "Preset loaded successfully" >>>;
}
```

### `loadPresetByName(string name) -> int`

Load a preset by name.

**Parameters:**
- `name` - Preset name (exact match required)

**Returns:**
- `1` on success
- `0` on failure (not found or load failed)

**Example:**
```chuck
clap.loadPresetByName("Init") => int success;
```

### `loadPresetFile(string filepath) -> int`

Load a preset from a file path.

**Parameters:**
- `filepath` - Absolute or relative path to preset file

**Returns:**
- `1` on success
- `0` on failure

**Notes:**
- Requires plugin to support the preset loading extension
- File format depends on plugin vendor

**Example:**
```chuck
clap.loadPresetFile("/path/to/preset.clappreset") => int success;
```

---

## System Discovery

Static method to enumerate all CLAP plugins installed on the system.

### `list() -> void` (static)

List all available CLAP plugins found in standard system paths to console.

**Standard Search Paths:**
- **macOS:** `/Library/Audio/Plug-Ins/CLAP`, `~/Library/Audio/Plug-Ins/CLAP`
- **Linux:** `/usr/lib/clap`, `/usr/local/lib/clap`, `~/.clap`
- **Windows:** `%CommonProgramFiles%\CLAP`, `%LocalAppData%\Programs\Common\CLAP`

**Environment Variable:**
- Also checks `CLAP_PATH` if set

**Example:**
```chuck
Clap clap;
clap.list();

// Prints:
// [CLAP]: Available CLAP Plugins:
// ----------------------------------------
//   1. PluginName.clap
//      Path: /Library/Audio/Plug-Ins/CLAP/PluginName.clap/Contents/MacOS/PluginName
// ----------------------------------------
// Total: N CLAP plugins
```

---

## Audio Chain Setup

CLAP chugins integrate into ChucK's audio graph as UGens (unit generators).

### Effect Processing

```chuck
// Load CLAP effect
Clap effect;
effect.load("/Library/Audio/Plug-Ins/CLAP/Reverb.clap");

// Connect audio chain
adc => effect => dac;

// Process audio
while (true) {
    1::second => now;
}
```

### Instrument Processing

```chuck
// Load CLAP instrument
Clap synth;
synth.load("/Library/Audio/Plug-Ins/CLAP/Synthesizer.clap");

// Connect to output
synth => dac;

// Play notes
synth.noteOn(60, 100);
1::second => now;
synth.noteOff(60);
```

### Effect Chain with Gain

For proper signal flow, CLAP effects should be followed by a Gain object:

```chuck
Clap clap;
Gain g;
1 => g.gain;

clap => g => dac;

// Connect audio source
adc => clap;
```

---

## Return Values

### Success/Failure Methods

Methods that return `int` typically use:
- `1` = success
- `0` = failure or not found

These include:
- `load()`
- `setParamByName()`
- `noteOn()`, `noteOff()`, `controlChange()`, `sendMIDI()`
- `loadPreset()`, `loadPresetByName()`, `loadPresetFile()`
- `activatePlugin()`
- `isInstrument()`

### Value Retrieval Methods

Methods that return values:
- `paramCount()`, `pluginCount()`, `presetCount()` - Return count (0 if none)
- `getParam()`, `getParamByName()` - Return normalized value (0.0 if invalid)
- `paramName()`, `pluginName()`, `presetName()` - Return string (empty if invalid)

### Void Methods

These methods don't return values:
- `close()`
- `setParam()`
- `bypass()`
- `list()`

---

## Platform-Specific Notes

### macOS Bundle Paths

On macOS, CLAP plugins are bundles (directories). You can pass:
- Bundle path: `/Library/Audio/Plug-Ins/CLAP/Plugin.clap`
- Binary path: `/Library/Audio/Plug-Ins/CLAP/Plugin.clap/Contents/MacOS/Plugin`

The chugin automatically resolves bundle paths to find the correct binary.

### Parameter Value Ranges

CLAP parameters are typically normalized to 0.0-1.0 range, but some plugins may use different ranges. Consult the plugin documentation for specific parameter behaviors.

### MIDI Channel

All MIDI methods operate on channel 0. Multi-channel support is not currently implemented.

### Sample-by-Sample Processing

The CLAP chugin processes audio sample-by-sample to integrate with ChucK's timing model. This may impact performance with some plugins but ensures precise timing synchronization.

---

## Error Handling

The CLAP chugin logs errors to stderr. Common error messages:

- `"Could not load library"` - Invalid path or missing plugin file
- `"Could not find clap_entry symbol"` - Not a valid CLAP plugin
- `"Failed to initialize plugin entry"` - Plugin initialization failed
- `"No plugins found in library"` - Empty CLAP bundle
- `"Plugin does not support preset loading"` - Preset methods unavailable
- `"Invalid preset index"` - Index out of range

Check console output for detailed error information when operations fail.
