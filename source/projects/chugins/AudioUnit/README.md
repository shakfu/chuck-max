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

The chugin requires the AudioToolbox and CoreFoundation frameworks (automatically linked on macOS).

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
- `07-midi.ck` - MIDI control concepts
- `08-multiple-effects.ck` - Chain multiple AudioUnits
- `09-switch-effects.ck` - Switch effects dynamically
- `10-live-input.ck` - Live audio processing

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

## Implementation Notes

- The chugin processes audio sample-by-sample (mono input/output)
- Stereo AudioUnits are converted to mono (left channel used for output)
- Parameter values are typically normalized to 0.0-1.0 range
- The chugin uses the AudioUnit v2 API (compatible with modern macOS)
- Thread-safe parameter changes

## Troubleshooting

**AudioUnit fails to load:**
- Verify the AudioUnit is installed (use `AudioUnit.list()`)
- Check the four-character codes are correct
- Some AudioUnits may require specific initialization

**No sound output:**
- Check the AudioUnit is not bypassed
- Verify input is connected (for effects)
- Some parameters may need initialization

**Parameters not working as expected:**
- Different AudioUnits use different parameter ranges
- Use `paramName()` to identify which parameter controls what
- Some parameters may be read-only

## License

This chugin is licensed under the GNU General Public License v2.0 or later.

## Credits

Part of the official ChucK chugins collection maintained by CCRMA, Stanford University.