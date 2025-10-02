# AudioUnit Chugin

A ChucK chugin that enables loading and using macOS AudioUnit plugins directly in ChucK programs.

## Overview

AudioUnit is Apple's plugin architecture for audio effects and instruments on macOS. This chugin provides a bridge between ChucK and the AudioUnit ecosystem, allowing you to use any AudioUnit plugin (effects, instruments, generators) as a UGen in ChucK.

## Platform Support

**macOS only** - AudioUnits are an Apple technology and only available on macOS.

On Linux and Windows, the chugin will build but AudioUnit functionality will not be available (stub implementation).

## Features

- Load AudioUnit plugins by name or four-character code
- Process audio through AudioUnit effects in real-time
- **Full MIDI support for AudioUnit instruments (MusicDevice)**
  - Direct programmatic MIDI control (noteOn, noteOff, CC, etc.)
  - Automatic virtual MIDI destination creation
  - Integration with ChucK's native MIDI system
- Access and control AudioUnit parameters dynamically
- Query available parameters and their names
- Bypass AudioUnit processing
- List all available AudioUnits on the system

## Building

```bash
cd AudioUnit
make mac
sudo make install
```

The chugin requires the AudioToolbox, CoreFoundation, and CoreMIDI frameworks (automatically linked on macOS).

## Usage

### Basic Setup

```chuck
// Create an AudioUnit instance
adc => AudioUnit au => dac;

// Load an AudioUnit by name
au.load("AUDelay");

// Or load by four-character code
au.open("aufx", "dely", "appl");
```

### Loading AudioUnits

**By name:**
```chuck
au.load("AUDelay");          // Apple's delay
au.load("AULowpass");        // Apple's lowpass filter
au.load("AUReverb");         // Apple's reverb
```

**By four-character codes:**
```chuck
// Format: au.open(type, subtype, manufacturer)
au.open("aufx", "dely", "appl");  // Effect: Delay by Apple
au.open("aufx", "lpas", "appl");  // Effect: Lowpass by Apple
au.open("aumu", "????", "????");  // Music Effect
```

Where:
- **type**: `aufx` = effect, `aumu` = music effect, `aumf` = music device/instrument
- **subtype**: Four-char code identifying the specific plugin
- **manufacturer**: Four-char code for the manufacturer (e.g., `appl` = Apple)

### Discovering Available AudioUnits

```chuck
// List all available AudioUnits on your system
AudioUnit.list();
```

This prints to the console all available AudioUnits with their types, names, and four-character codes.

### Controlling Parameters

```chuck
// Get number of parameters
au.paramCount() => int numParams;

// Get parameter name
au.paramName(0) => string paramName;

// Get parameter value
au.getParam(0) => float value;

// Set parameter value (0.0 to 1.0 typically, but depends on plugin)
au.setParam(0, 0.5);

// Iterate through all parameters
for (0 => int i; i < au.paramCount(); i++) {
    <<< "Param", i, ":", au.paramName(i), "=", au.getParam(i) >>>;
}
```

### Bypass

```chuck
// Bypass the AudioUnit (pass audio through unchanged)
au.bypass(1);

// Re-enable processing
au.bypass(0);
```

### Closing

```chuck
// Close the current AudioUnit
au.close();

// You can then load a different one
au.load("AUReverb");
```

## MIDI Support

The AudioUnit chugin provides comprehensive MIDI support for AudioUnit instruments (MusicDevice type). When you load a MusicDevice AudioUnit, two MIDI integration methods are automatically available:

### Option A: Direct Programmatic MIDI Control

Send MIDI messages directly to the AudioUnit from ChucK code:

```chuck
AudioUnit synth => dac;
0.5 => synth.gain;  // Set gain to hear output

// Load a MusicDevice (instrument)
synth.load("DLSMusicDevice");

// Check if it's a MusicDevice
if (synth.isMusicDevice()) {
    // Send note-on (channel 0)
    synth.noteOn(60, 100);    // Middle C, velocity 100
    1::second => now;

    // Send note-off
    synth.noteOff(60);

    // Send control change
    synth.controlChange(1, 64);  // Modulation wheel to 50%

    // Send program change
    synth.programChange(5);      // Change to program 5

    // Or send raw MIDI for full control
    synth.sendMIDI(0x90, 60, 100);  // Note on, channel 0
}
```

**Direct MIDI Methods:**
- `noteOn(int pitch, int velocity)` - Send MIDI note-on (channel 0)
- `noteOff(int pitch)` - Send MIDI note-off (channel 0)
- `controlChange(int cc, int value)` - Send MIDI CC message
- `programChange(int program)` - Send MIDI program change
- `sendMIDI(int status, int data1, int data2)` - Send raw MIDI message

All methods return 1 on success, 0 on failure (e.g., not a MusicDevice).

### Option C: Virtual MIDI Destination

When a MusicDevice is loaded, the chugin **automatically creates a virtual MIDI destination** using CoreMIDI. This destination:

- Appears in `chuck --probe` MIDI device list
- Can receive MIDI from ChucK's `MidiOut`
- Can receive MIDI from external MIDI controllers
- Can receive MIDI from other music software (DAWs, etc.)

**Example using ChucK's MidiOut:**

```chuck
// First, load the AudioUnit instrument
AudioUnit synth => dac;
0.5 => synth.gain;  // Set gain to hear output

synth.load("DLSMusicDevice");

// The virtual MIDI destination is created automatically
<<< "MIDI destination:", synth.getMIDIDeviceName() >>>;

// In a separate ChucK program or thread, you can send to it:
MidiOut mout;
MidiMsg msg;

// Find the device number with: chuck --probe
// Then open the AudioUnit's virtual MIDI destination
mout.open(DEVICE_NUMBER);  // Replace with actual device number

// Send MIDI messages
0x90 => msg.data1;  // Note on
60 => msg.data2;    // Middle C
100 => msg.data3;   // Velocity
mout.send(msg);
```

**Querying MIDI Capabilities:**
```chuck
// Check if loaded AudioUnit accepts MIDI
if (au.isMusicDevice()) {
    <<< "This is a MusicDevice (instrument)" >>>;

    // Get the virtual MIDI destination name
    <<< "MIDI device name:", au.getMIDIDeviceName() >>>;
}
```

### Manual MIDI Forwarding (Option B Alternative)

If you want to manually route ChucK's `MidiIn` to an AudioUnit, you can do this in ChucK code:

```chuck
MidiIn min;
MidiMsg msg;
AudioUnit synth => dac;
0.5 => synth.gain;  // Set gain to hear output

min.open(0);  // Open MIDI input device
synth.load("DLSMusicDevice");

while (true) {
    min => now;  // Wait for MIDI event

    while (min.recv(msg)) {
        // Forward MIDI message to AudioUnit
        synth.sendMIDI(msg.data1, msg.data2, msg.data3);
    }
}
```

This approach gives you full control over MIDI routing and allows filtering or transforming MIDI data before sending to the AudioUnit.

### Common AudioUnit Instruments

Some MusicDevice AudioUnits available on macOS:
- `DLSMusicDevice` - Apple's DLS (DownLoadable Sounds) synthesizer
- Third-party instrument plugins (check with `AudioUnit.list()`)

## Examples

Comprehensive examples are provided in the `examples/` directory:

### Basic Usage
- `00-list-audiounit.ck` - List all available AudioUnits
- `01-basic-effect.ck` - Simple effect processing
- `02-load-by-fourcc.ck` - Load by four-character codes

### Parameter Control
- `03-parameters.ck` - Discover and control parameters
- `04-dynamic-control.ck` - Real-time parameter modulation
- `11-parameter-sweep.ck` - Various modulation techniques

### Advanced Features
- `05-bypass.ck` - Toggle bypass mode
- `06-generator.ck` - Using instruments/generators
- `08-multiple-effects.ck` - Chain multiple AudioUnits
- `09-switch-effects.ck` - Switch effects dynamically
- `10-live-input.ck` - Live audio processing

### MIDI Control
- `07-midi.ck` - **Option A:** Direct programmatic MIDI control (noteOn, noteOff, CC, etc.)
- `07-midi-simple.ck` - Minimal MIDI example (6-note melody, verified working)
- `07-midi-routing.ck` - **Option C:** Virtual MIDI destination (external MIDI routing)

See `examples/README.md` for detailed information about each example.

## Common Apple AudioUnits

Some AudioUnits included with macOS:

**Effects:**
- `AUDelay` - Delay effect
- `AULowpass` - Low-pass filter
- `AUHighpass` - High-pass filter
- `AUBandpass` - Band-pass filter
- `AUReverb` - Reverb effect
- `AUDistortion` - Distortion effect
- `AUPeakLimiter` - Peak limiter
- `AUDynamicsProcessor` - Compressor/limiter/expander/gate

**Additional AudioUnits:**
- Third-party AudioUnit plugins installed on your system will also be available
- Use `AudioUnit.list()` to see all available plugins

## API Reference

### Methods

#### `int load(string name)`
Load an AudioUnit by name (case-insensitive substring match).
- **Returns:** 1 on success, 0 on failure

#### `int open(string type, string subtype, string manufacturer)`
Load an AudioUnit by four-character codes.
- **Parameters:** All parameters must be exactly 4 characters
- **Returns:** 1 on success, 0 on failure

#### `void close()`
Close the currently loaded AudioUnit.

#### `void list()`
Print all available AudioUnits to the console (static method).

#### `void setParam(int index, float value)`
Set a parameter value by index.

#### `float getParam(int index)`
Get a parameter value by index.

#### `string paramName(int index)`
Get the name of a parameter by index.

#### `int paramCount()`
Get the total number of parameters.

#### `void bypass(int bypass)`
Bypass the AudioUnit (1 = bypass, 0 = active).

### MIDI Methods (MusicDevice only)

#### `int isMusicDevice()`
Check if the loaded AudioUnit is a MusicDevice (instrument) that accepts MIDI.
- **Returns:** 1 if MusicDevice, 0 otherwise

#### `string getMIDIDeviceName()`
Get the name of the automatically-created virtual MIDI destination.
- **Returns:** MIDI destination name, or empty string if not a MusicDevice

#### `int noteOn(int pitch, int velocity)`
Send MIDI note-on message (channel 0).
- **Parameters:**
  - `pitch`: MIDI note number (0-127)
  - `velocity`: Note velocity (0-127)
- **Returns:** 1 on success, 0 on failure

#### `int noteOff(int pitch)`
Send MIDI note-off message (channel 0).
- **Parameters:**
  - `pitch`: MIDI note number (0-127)
- **Returns:** 1 on success, 0 on failure

#### `int controlChange(int cc, int value)`
Send MIDI control change message (channel 0).
- **Parameters:**
  - `cc`: Controller number (0-127)
  - `value`: Controller value (0-127)
- **Returns:** 1 on success, 0 on failure

#### `int programChange(int program)`
Send MIDI program change message (channel 0).
- **Parameters:**
  - `program`: Program number (0-127)
- **Returns:** 1 on success, 0 on failure

#### `int sendMIDI(int status, int data1, int data2)`
Send raw MIDI message for full control.
- **Parameters:**
  - `status`: MIDI status byte (e.g., 0x90 for note-on)
  - `data1`: First data byte
  - `data2`: Second data byte
- **Returns:** 1 on success, 0 on failure

## Implementation Notes

- The chugin processes audio sample-by-sample (mono input/output)
- Stereo AudioUnits are converted to mono (left channel used for output)
- Parameter values are typically normalized to 0.0-1.0 range
- The chugin uses the AudioUnit v2 API (compatible with modern macOS)
- Thread-safe parameter changes
- **MIDI Support:**
  - MusicDevice AudioUnits automatically create virtual MIDI destinations via CoreMIDI
  - MIDI messages use sample-accurate timing
  - Virtual MIDI destinations appear in `chuck --probe` output
  - Non-interleaved stereo audio buffers for optimal AudioUnit compatibility

## Troubleshooting

**AudioUnit fails to load:**
- Verify the AudioUnit is installed (use `AudioUnit.list()`)
- Check the four-character codes are correct
- Some AudioUnits may require specific initialization

**No sound output:**
- Check the AudioUnit is not bypassed
- Verify input is connected (for effects)
- Some parameters may need initialization
- **For MusicDevice instruments:** Make sure to set gain (e.g., `0.5 => synth.gain`)

**MIDI not producing sound:**
- Verify the AudioUnit is a MusicDevice using `isMusicDevice()`
- Ensure gain is set on the AudioUnit UGen
- Check that MIDI messages are being sent (noteOn returns 1 on success)
- Try the verified working example `07-midi-simple.ck` first

**Parameters not working as expected:**
- Different AudioUnits use different parameter ranges
- Use `paramName()` to identify which parameter controls what
- Some parameters may be read-only

## License

This chugin is licensed under the GNU General Public License v2.0 or later.

## Credits

Part of the official ChucK chugins collection maintained by CCRMA, Stanford University.