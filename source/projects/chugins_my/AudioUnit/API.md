# AudioUnit Chugin API Reference

The AudioUnit chugin enables loading and using macOS AudioUnit plugins in ChucK. AudioUnits can be effects, instruments (MusicDevice), or generators. This chugin is **macOS-only**.

## Table of Contents

- [Plugin Loading](#plugin-loading)
- [Parameter Control](#parameter-control)
- [Preset Management](#preset-management)
- [MIDI Support](#midi-support)
- [Virtual MIDI Destination](#virtual-midi-destination)
- [System Discovery](#system-discovery)
- [Audio Chain Setup](#audio-chain-setup)
- [Return Values](#return-values)

---

## Plugin Loading

### `load(string name) -> int`

Load an AudioUnit by name or four-char code.

**Parameters:**
- `name` - AudioUnit name (case-insensitive substring match) or four-char code format `"type:subtype:manufacturer"`

**Returns:**
- `1` on success
- `0` on failure

**Four-Char Code Format:**
- Type codes: `aufx` (effect), `aumu` (MusicDevice/instrument), `aumf` (music effect), `augn` (generator)
- Example: `"aufx:dely:appl"` = Apple AUDelay effect

**Name Search:**
- Searches all installed AudioUnits for case-insensitive substring match
- Example: `"DLS Synth"` finds "DLSMusicDevice"

**Examples:**
```chuck
AudioUnit au;

// Load by name (substring match)
au.load("DLS Synth") => int success;

// Load by four-char code
au.load("aufx:dely:appl") => int success;  // Apple Delay effect

if (!success) {
    <<< "Failed to load AudioUnit" >>>;
}
```

### `open(string type, string subtype, string manufacturer) -> int`

Open an AudioUnit by explicit four-char codes.

**Parameters:**
- `type` - 4-character type code (e.g., `"aufx"`, `"aumu"`)
- `subtype` - 4-character subtype code
- `manufacturer` - 4-character manufacturer code (e.g., `"appl"`)

**Returns:**
- `1` on success
- `0` on failure

**Common Type Codes:**
- `"aufx"` - Audio Effect
- `"aumu"` - Music Device (Instrument)
- `"aumf"` - Music Effect
- `"augn"` - Generator
- `"aumx"` - Mixer

**Example:**
```chuck
AudioUnit au;
// Open Apple AUDelay
au.open("aufx", "dely", "appl") => int success;
```

### `close() -> void`

Close the currently loaded AudioUnit and free all resources.

**Notes:**
- Destroys virtual MIDI destination (if created for MusicDevice)
- Clears all cached parameter and preset information

**Example:**
```chuck
au.close();
```

---

## Parameter Control

AudioUnit supports three methods for parameter access: by index (fastest), or by name (most readable).

### `paramCount() -> int`

Get the number of parameters available.

**Returns:** Number of parameters

**Example:**
```chuck
au.paramCount() => int count;
<<< "AudioUnit has", count, "parameters" >>>;
```

### `paramName(int index) -> string`

Get a parameter name by index.

**Parameters:**
- `index` - Parameter index (0-based)

**Returns:** Parameter name, or empty string if invalid

**Example:**
```chuck
for (0 => int i; i < au.paramCount(); i++) {
    <<< i, ":", au.paramName(i), "=", au.getParam(i) >>>;
}
```

### `setParam(int index, float value) -> void`

Set a parameter value by index.

**Parameters:**
- `index` - Parameter index (0-based)
- `value` - Parameter value (range depends on parameter definition)

**Notes:**
- AudioUnit parameters may have different ranges (not always 0.0-1.0)
- Use AudioUnit documentation to determine valid ranges

**Example:**
```chuck
au.setParam(0, 0.5);  // Set first parameter to 0.5
```

### `getParam(int index) -> float`

Get a parameter value by index.

**Parameters:**
- `index` - Parameter index (0-based)

**Returns:** Parameter value, or 0.0 if invalid

**Example:**
```chuck
au.getParam(0) => float value;
<<< "Parameter 0 value:", value >>>;
```

### `setParamByName(string name, float value) -> int`

Set a parameter value by name.

**Parameters:**
- `name` - Parameter name (exact match required)
- `value` - Parameter value

**Returns:**
- `1` on success
- `0` if parameter not found

**Example:**
```chuck
au.setParamByName("Delay Time", 500.0) => int success;
au.setParamByName("Feedback", 0.3);
```

### `getParamByName(string name) -> float`

Get a parameter value by name.

**Parameters:**
- `name` - Parameter name (exact match required)

**Returns:** Parameter value, or 0.0 if not found

**Example:**
```chuck
au.getParamByName("Wet/Dry Mix") => float mix;
<<< "Mix level:", mix >>>;
```

### `bypass(int bypass) -> void`

Bypass the AudioUnit (pass audio through unprocessed).

**Parameters:**
- `bypass` - `1` to bypass, `0` to activate

**Notes:**
- Sets the AudioUnit's internal bypass property
- Also provides software bypass at chugin level

**Example:**
```chuck
au.bypass(1);  // Bypass AudioUnit
1::second => now;
au.bypass(0);  // Re-activate AudioUnit
```

---

## Preset Management

AudioUnits can provide factory presets. Not all AudioUnits have factory presets available.

### `presetCount() -> int`

Get the number of factory presets available.

**Returns:** Number of presets (may be 0)

**Example:**
```chuck
au.presetCount() => int count;
if (count > 0) {
    <<< "Found", count, "factory presets" >>>;
}
```

### `presetName(int index) -> string`

Get the name of a preset by index.

**Parameters:**
- `index` - Preset index (0-based)

**Returns:** Preset name, or empty string if invalid

**Example:**
```chuck
for (0 => int i; i < au.presetCount(); i++) {
    <<< i, ":", au.presetName(i) >>>;
}
```

### `setPreset(int index) -> int`

Set the current preset by index.

**Parameters:**
- `index` - Preset index (0-based)

**Returns:**
- `1` on success
- `0` on failure

**Notes:**
- Loading a preset changes multiple parameter values
- Some AudioUnits may not support preset loading

**Example:**
```chuck
au.setPreset(0) => int success;  // Load first preset
if (success) {
    <<< "Preset loaded successfully" >>>;
}
```

### `setPresetByName(string name) -> int`

Set the current preset by name.

**Parameters:**
- `name` - Preset name (exact match required)

**Returns:**
- `1` on success
- `0` on failure (not found or load failed)

**Example:**
```chuck
au.setPresetByName("Cathedral") => int success;
```

### `getPreset() -> int`

Get the current preset index.

**Returns:**
- Preset index (0-based)
- `-1` if no preset is active or cannot determine

**Example:**
```chuck
au.getPreset() => int currentPreset;
if (currentPreset >= 0) {
    <<< "Current preset:", au.presetName(currentPreset) >>>;
}
```

### `listPresets() -> void`

List all available factory presets to console.

**Example:**
```chuck
au.listPresets();

// Prints:
// [AudioUnit]: Available Presets:
// ----------------------------------------
//   [0] Preset Name 1
//   [1] Preset Name 2
// ----------------------------------------
// Total: N presets
```

---

## MIDI Support

AudioUnits with type `kAudioUnitType_MusicDevice` (instruments) can receive MIDI messages. Check `isMusicDevice()` before using MIDI methods.

### `isMusicDevice() -> int`

Check if the loaded AudioUnit is a MusicDevice (instrument).

**Returns:**
- `1` if AudioUnit is a MusicDevice/instrument
- `0` if AudioUnit is an effect or not loaded

**Example:**
```chuck
if (au.isMusicDevice()) {
    <<< "This is an instrument AudioUnit" >>>;
}
```

### `noteOn(int pitch, int velocity) -> int`

Send MIDI note-on message to the AudioUnit (channel 0).

**Parameters:**
- `pitch` - MIDI note number (0-127)
- `velocity` - Note velocity (0-127)

**Returns:**
- `1` on success
- `0` on failure (not a MusicDevice or plugin not loaded)

**Example:**
```chuck
au.noteOn(60, 100);  // Middle C, forte
500::ms => now;
au.noteOff(60);
```

### `noteOff(int pitch) -> int`

Send MIDI note-off message to the AudioUnit (channel 0).

**Parameters:**
- `pitch` - MIDI note number (0-127)

**Returns:**
- `1` on success
- `0` on failure

**Example:**
```chuck
au.noteOff(60);  // Release middle C
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
au.controlChange(1, 64);   // Modulation wheel to center
au.controlChange(64, 127); // Sustain pedal on
```

### `programChange(int program) -> int`

Send MIDI program change message (channel 0).

**Parameters:**
- `program` - Program number (0-127)

**Returns:**
- `1` on success
- `0` on failure

**Example:**
```chuck
au.programChange(0);  // Switch to program 0
```

### `sendMIDI(int status, int data1, int data2) -> int`

Send raw MIDI message to the AudioUnit.

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
- Program Change: `0xC0` | channel

**Example:**
```chuck
au.sendMIDI(0x90, 60, 100);  // Note On, channel 0, C4, velocity 100
```

---

## Virtual MIDI Destination

When a MusicDevice (instrument) AudioUnit is loaded, the chugin automatically creates a virtual MIDI destination that appears in macOS MIDI routing applications (e.g., Audio MIDI Setup, DAWs).

### `getMIDIDeviceName() -> string`

Get the name of the virtual MIDI destination.

**Returns:** MIDI destination name, or empty string if not a MusicDevice

**Notes:**
- Only created for MusicDevice type AudioUnits
- Name format: `"ChucK AudioUnit: <AudioUnit Name>"`
- Can be routed to from other MIDI applications
- Automatically destroyed when AudioUnit is closed

**Example:**
```chuck
if (au.isMusicDevice()) {
    <<< "Virtual MIDI destination:", au.getMIDIDeviceName() >>>;
    // Output: "ChucK AudioUnit: DLSMusicDevice"
}
```

**MIDI Routing:**
External MIDI controllers or applications can send MIDI to this virtual destination, and the AudioUnit will receive it automatically.

---

## System Discovery

Static method to enumerate all AudioUnits installed on the system.

### `list() -> void` (static)

List all available AudioUnits found on the system to console.

**Notes:**
- Searches all installed AudioUnits
- Displays type (Effect, Instrument, etc.), name, and four-char codes
- Useful for discovering available AudioUnits and their identifiers

**Example:**
```chuck
AudioUnit au;
au.list();

// Prints:
// [AudioUnit]: Available AudioUnits:
// ----------------------------------------
//   1. [Effect] AUDelay
//      Type: 'aufx' SubType: 'dely' Mfr: 'appl'
//   2. [Instrument] DLSMusicDevice
//      Type: 'aumu' SubType: 'dls ' Mfr: 'appl'
// ----------------------------------------
// Total: N AudioUnits
```

**Output Format:**
- **Type**: Human-readable type (Effect, Instrument, MusicEffect, Generator, Mixer)
- **Type Code**: Four-char component type
- **SubType Code**: Four-char component subtype
- **Mfr Code**: Four-char manufacturer code

---

## Audio Chain Setup

AudioUnit chugins integrate into ChucK's audio graph as UGens (unit generators).

### Effect Processing

```chuck
// Load AudioUnit effect
AudioUnit delay;
delay.load("AUDelay");

// Configure parameters
delay.setParamByName("Delay Time", 500.0);
delay.setParamByName("Feedback", 0.3);

// Connect audio chain
adc => delay => dac;

// Process audio
while (true) {
    1::second => now;
}
```

### Instrument Processing

```chuck
// Load AudioUnit instrument
AudioUnit synth;
synth.load("DLS Synth");

// Connect to output
synth => dac;

// Play notes
synth.noteOn(60, 100);
1::second => now;
synth.noteOff(60);
```

### Multiple Effects Chain

```chuck
AudioUnit delay, reverb;
delay.load("AUDelay");
reverb.load("AUMatrixReverb");

// Chain effects
SinOsc osc => delay => reverb => dac;

440 => osc.freq;
0.5 => osc.gain;

while (true) {
    1::second => now;
}
```

---

## Return Values

### Success/Failure Methods

Methods that return `int` typically use:
- `1` = success
- `0` = failure or not found

These include:
- `load()`
- `open()`
- `setParamByName()`
- `noteOn()`, `noteOff()`, `controlChange()`, `programChange()`, `sendMIDI()`
- `setPreset()`, `setPresetByName()`
- `isMusicDevice()`

### Value Retrieval Methods

Methods that return values:
- `paramCount()`, `presetCount()` - Return count (0 if none)
- `getParam()`, `getParamByName()` - Return parameter value (0.0 if invalid)
- `paramName()`, `presetName()`, `getMIDIDeviceName()` - Return string (empty if invalid)
- `getPreset()` - Return preset index (-1 if none active)

### Void Methods

These methods don't return values:
- `close()`
- `setParam()`
- `bypass()`
- `list()`
- `listPresets()`

---

## Platform-Specific Notes

### macOS Only

This chugin **only works on macOS** due to AudioUnit being an Apple framework. On other platforms, all methods return failure/empty values and log an error message.

### AudioUnit Locations

AudioUnits are typically installed in:
- `/Library/Audio/Plug-Ins/Components/`
- `~/Library/Audio/Plug-Ins/Components/`

### Four-Char Code Endianness

Four-character codes are stored in big-endian format. When using `open()`, ensure strings are exactly 4 characters.

### Parameter Ranges

AudioUnit parameters do **not** use a standard range. Common ranges:
- Boolean: 0.0 or 1.0
- Percentage: 0.0 to 100.0
- Frequency: Hz values (e.g., 20.0 to 20000.0)
- Time: milliseconds or seconds
- Decibels: dB values (e.g., -96.0 to 12.0)

Consult AudioUnit documentation for specific parameter ranges.

### MIDI Channel

All MIDI methods operate on channel 0. Multi-channel support is not currently implemented.

### Stereo to Mono Conversion

AudioUnits process in stereo internally, but the chugin converts to mono:
- Input: Mono sample is duplicated to both stereo channels
- Output: Left channel only is returned to ChucK

### Sample-by-Sample Processing

The AudioUnit chugin processes audio sample-by-sample to integrate with ChucK's timing model. This may impact performance with some plugins but ensures precise timing synchronization.

### Virtual MIDI Routing

When a MusicDevice is loaded:
1. A virtual MIDI destination is created automatically
2. The destination appears in macOS MIDI routing (Audio MIDI Setup, DAWs)
3. External MIDI can be routed to the destination
4. The AudioUnit receives MIDI from both ChucK and external sources
5. The destination is destroyed when the AudioUnit is closed

---

## Common AudioUnit Types

### Apple Effects (aufx)
- **AUDelay** (`aufx:dely:appl`) - Delay/echo effect
- **AUMatrixReverb** (`aufx:mrev:appl`) - Reverb
- **AUBandpass** (`aufx:bpas:appl`) - Bandpass filter
- **AULowpass** (`aufx:lpas:appl`) - Lowpass filter
- **AUHighpass** (`aufx:hpas:appl`) - Highpass filter
- **AUDynamicsProcessor** (`aufx:dcmp:appl`) - Compressor/limiter

### Apple Instruments (aumu)
- **DLSMusicDevice** (`aumu:dls :appl`) - General MIDI synth

### Third-Party AudioUnits
Many third-party audio plugins are available as AudioUnits. Use `list()` to discover installed plugins.

---

## Error Handling

The AudioUnit chugin logs errors to stderr. Common error messages:

- `"Could not find AudioUnit component"` - Invalid four-char codes or name not found
- `"Could not instantiate AudioUnit"` - Plugin failed to load
- `"Could not initialize AudioUnit"` - Plugin initialization failed
- `"Could not find AudioUnit named 'X'"` - Name search found no matches
- `"Render error"` - Audio processing error (typically from plugin)
- `"AudioUnits are only available on macOS"` - Attempted use on non-macOS platform

Check console output for detailed error information when operations fail.

---

## Performance Considerations

### Sample-by-Sample vs. Block Processing

AudioUnits are designed for block-based processing, but ChucK processes sample-by-sample. The chugin handles this by:
- Setting `MaximumFramesPerSlice` to 1
- Calling `AudioUnitRender()` for each sample

This has performance implications:
- Higher CPU usage than block processing
- May cause issues with some complex plugins
- Ensures sample-accurate timing in ChucK

### Optimization Tips

1. **Minimize parameter changes**: Parameter updates involve AudioUnit callbacks
2. **Use bypass**: When not needed, bypass rather than constantly loading/closing
3. **Limit concurrent AudioUnits**: Each instance has overhead
4. **Test plugins**: Some AudioUnits may not handle sample-by-sample processing well

---

## Example: Complete Instrument Setup

```chuck
// Load DLS synth
AudioUnit synth;
synth.load("DLS Synth");

// Verify it's an instrument
if (!synth.isMusicDevice()) {
    <<< "Error: Not an instrument!" >>>;
    me.exit();
}

// Print virtual MIDI destination
<<< "MIDI destination:", synth.getMIDIDeviceName() >>>;

// Load a preset if available
if (synth.presetCount() > 0) {
    synth.setPreset(0);
    <<< "Loaded preset:", synth.presetName(0) >>>;
}

// Connect to output
synth => dac;

// Play a melody
[60, 64, 67, 72] @=> int melody[];
for (0 => int i; i < melody.cap(); i++) {
    synth.noteOn(melody[i], 100);
    500::ms => now;
    synth.noteOff(melody[i]);
    100::ms => now;
}

// Clean up
synth.close();
```

---

## Example: Effect Chain with Parameter Control

```chuck
// Load delay effect
AudioUnit delay;
delay.load("AUDelay");

// Configure delay parameters
delay.setParamByName("Delay Time", 250.0);     // 250ms delay
delay.setParamByName("Feedback", 0.5);          // 50% feedback
delay.setParamByName("Dry/Wet Mix", 50.0);      // 50% wet

// Connect audio chain
SinOsc osc => delay => dac;

// Generate test tone
440 => osc.freq;
0.3 => osc.gain;

// Sweep delay time
for (100.0 => float time; time < 1000.0; time + 50.0 => time) {
    delay.setParamByName("Delay Time", time);
    <<< "Delay time:", time, "ms" >>>;
    200::ms => now;
}

// Clean up
delay.close();
```