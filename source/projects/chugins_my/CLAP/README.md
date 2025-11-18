# CLAP Chugin for ChucK

A ChucK chugin for loading and using CLAP (CLever Audio Plugin) plugins in ChucK.

## Features

- Load CLAP plugins by file path
- **Multi-plugin bundle support** - enumerate and switch between plugins
- Support for both effect and instrument plugins
- **Dual-precision audio** - float32 and float64 support for high-quality processing
- **Stereo mixdown** - proper handling of stereo plugins in mono context
- Parameter control by index and by name
- MIDI support for CLAP instruments (note on/off)
- **Factory preset support** - discover and load presets
- **Instrument detection** - automatic identification of synthesizers
- Bypass functionality
- Cross-platform support (macOS, Windows, Linux)
- **No external SDK required** - CLAP headers included in repository

## What is CLAP?

CLAP (CLever Audio Plugin) is a modern, open-source audio plugin standard developed by the audio plugin community. Key features:

- **Open Source** - Free and open specification
- **Modern Design** - Built for current and future audio production needs
- **Extensible** - Modular extension system
- **Cross-Platform** - Works on macOS, Windows, and Linux
- **No Licensing Fees** - Completely free to use

Learn more at [cleveraudio.org](https://cleveraudio.org/)

## Requirements

### Building

The CLAP headers are included in the repository, so no external dependencies are required!

The headers are automatically cloned from the official CLAP repository during the build process.

## Building

### macOS

```bash
cd CLAP2
make mac
sudo make install
```

This will build `CLAP.chug` and install it to `/usr/local/lib/chuck/`.

### Linux

```bash
cd CLAP2
make linux
sudo make install
```

### Windows

#### Using Visual Studio

1. Open `CLAP.vcxproj` in Visual Studio
2. Build the solution

#### Using nmake

```cmd
nmake -f makefile.win
```

### Build Output

- **CLAP.chug** - The compiled chugin (dynamic library)
- Installed to: `/usr/local/lib/chuck/` (Unix) or ChucK's chugin directory (Windows)

## Usage

### Loading a CLAP Plugin

```chuck
// Create CLAP instance
adc => CLAP clap => dac;

// Load a plugin by file path
// The first plugin (index 0) is automatically activated
clap.load("/Library/Audio/Plug-Ins/CLAP/YourPlugin.clap");
```

### Multi-Plugin Bundles

Some CLAP files contain multiple plugins. This chugin supports enumerating and switching between them:

```chuck
CLAP clap => dac;

// Load the bundle
clap.load("/path/to/multi-plugin.clap");

// Check how many plugins are in the bundle
clap.pluginCount() => int count;
<<< "Found", count, "plugin(s)" >>>;

// List all plugins (prints to console)
if(count > 1) {
    clap.listPlugins();  // Prints [0] Plugin Name, [1] Another Plugin, etc.
}

// Get plugin names programmatically
for(0 => int i; i < count; i++) {
    <<< "Plugin", i, ":", clap.pluginName(i) >>>;
}

// Switch to a different plugin
if(count > 1) {
    clap.activatePlugin(1);  // Activate plugin at index 1
    <<< "Now using:", clap.pluginName(1) >>>;
}

// The first plugin (index 0) is auto-activated by load()
// No need to call activatePlugin(0) initially
```

**Note:** Most CLAP files contain only one plugin, so multi-plugin support is only needed for special cases.

### Parameter Control

```chuck
// Get parameter count
clap.paramCount() => int count;

// Get parameter info
for (0 => int i; i < count; i++) {
    <<< "Parameter", i, ":", clap.paramName(i), "=", clap.getParam(i) >>>;
}

// Set parameter by index
clap.setParam(0, 0.5);

// Set parameter by name
clap.setParamByName("Cutoff", 1000.0);

// Get parameter by name
clap.getParamByName("Resonance") => float resonance;
```

### MIDI (for instruments)

```chuck
// Check if plugin is an instrument
if (clap.isInstrument()) {
    // Play a note
    clap.noteOn(60, 100);  // Middle C, velocity 100
    1::second => now;
    clap.noteOff(60);
}
```

### Presets

```chuck
// Check if plugin has presets
clap.presetCount() => int count;
<<< "Available presets:", count >>>;

// List preset names
for (0 => int i; i < count; i++) {
    <<< "Preset", i, ":", clap.presetName(i) >>>;
}

// Load preset by index
clap.loadPreset(0);

// Load preset by name
clap.loadPresetByName("My Favorite Sound");
```

### Bypass

```chuck
// Bypass the plugin (audio passes through unchanged)
clap.bypass(1);

// Re-enable the plugin
clap.bypass(0);
```

### Cleanup

```chuck
// Close the plugin when done
clap.close();
```

## Examples

Comprehensive examples are provided in the `examples/` directory with an easy-to-use Makefile.

### Quick Start

```bash
cd CLAP2/examples

# See all available examples
make help

# Run specific examples
make list              # List all installed CLAP plugins
make basic-effect      # Load and use a basic effect
make multi-plugin      # Multi-plugin bundle support

# Run all CLAP2 examples in order
make all

# Run legacy CLAP examples
make legacy
```

### Example Categories

**Basic Usage (00-02):**
- `00-list-clap.ck` - List all CLAP plugins on the system (`make list`)
- `01-basic-effect.ck` - Load and use a basic effect (`make basic-effect`)
- `02-parameters.ck` - Parameter control (`make parameters`)

**Parameter Control (03, 08-09):**
- `03-dynamic-control.ck` - Real-time parameter modulation (`make dynamic-control`)
- `08-parameter-sweep.ck` - Parameter automation (`make parameter-sweep`)
- `09-parameter-by-name.ck` - Parameter control by name (`make parameter-by-name`)

**Advanced Features (04-07, 10):**
- `04-bypass.ck` - Bypass functionality (`make bypass`)
- `05-instrument.ck` - MIDI instrument usage (`make instrument`)
- `06-midi-simple.ck` - Simple MIDI example (`make midi-simple`)
- `07-multiple-effects.ck` - Chaining effects (`make multiple-effects`)
- `10-multi-plugin.ck` - **Multi-plugin bundle support** (`make multi-plugin`)

**Legacy Examples:**
- `Clap-midi-demo.ck`, `Clap-test.ck`, `TAL-test.ck`, and more

See `examples/README.md` for detailed information on each example.

## CLAP Plugin Locations

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

You can also set the `CLAP_PATH` environment variable to add additional search directories:

```bash
export CLAP_PATH=/path/to/my/clap/plugins
```

## API Reference

### Methods

#### Loading
- `int load(string path)` - Load a CLAP plugin by file path (auto-activates first plugin)
- `void close()` - Close the currently loaded plugin
- `void list()` - List all available CLAP plugins on the system (static method)

#### Multi-Plugin Support
- `int pluginCount()` - Get the number of plugins in the loaded bundle
- `int listPlugins()` - List all plugins in the loaded bundle (returns count)
- `string pluginName(int index)` - Get the name of a plugin by index
- `int activatePlugin(int index)` - Activate a specific plugin by index (returns 1 on success)

#### Parameters
- `int paramCount()` - Get number of parameters
- `string paramName(int index)` - Get parameter name by index
- `float getParam(int index)` - Get parameter value by index
- `void setParam(int index, float value)` - Set parameter value by index
- `float getParamByName(string name)` - Get parameter value by name
- `int setParamByName(string name, float value)` - Set parameter value by name (returns 1 on success)

#### MIDI (for instruments)
- `int isInstrument()` - Check if plugin is an instrument
- `int noteOn(int pitch, int velocity)` - Send MIDI note on
- `int noteOff(int pitch)` - Send MIDI note off
- `int sendMIDI(int status, int data1, int data2)` - Send raw MIDI message

#### Presets
- `int presetCount()` - Get the number of available presets
- `string presetName(int index)` - Get preset name by index
- `int loadPreset(int index)` - Load a preset by index (returns 1 on success)
- `int loadPresetByName(string name)` - Load a preset by name (returns 1 on success)

#### Other
- `void bypass(int bypass)` - Bypass the plugin (1 = bypass, 0 = active)

## Technical Details

### Audio Processing

This chugin provides high-quality audio processing with the following characteristics:

#### Float32 and Float64 Support

The chugin supports both single-precision (float32) and double-precision (float64) audio processing:

- Both float32 and float64 buffer pointers are provided to plugins
- Plugins can choose which precision to use
- Automatic detection of which format the plugin used
- No performance penalty if plugin uses float32

**Why this matters:** Some precision-critical plugins (like certain filters and effects) benefit from float64 processing to minimize accumulation errors.

#### Stereo Mixdown

ChucK processes audio in mono, but many CLAP plugins are stereo:

- Input: ChucK's mono signal is duplicated to both stereo channels
- Processing: Plugin processes in stereo
- Output: Left and right channels are averaged (mixdown) to mono

**Formula:** `output = (left + right) * 0.5`

This approach:
- Preserves stereo plugin behavior (panning, width, etc.)
- Provides better sound quality than using only one channel
- Maintains phase coherency
- Works correctly with mono plugins (both channels identical)

#### Sample-by-Sample Processing

Unlike typical DAW hosts that use buffer-based processing, this chugin processes one sample at a time:

- Integrates seamlessly with ChucK's sample-rate control flow
- Allows precise timing control in ChucK
- Each `tick()` call processes exactly one sample
- Time advances by 1 sample per call

## Free CLAP Plugins to Try

Here are some excellent free CLAP plugins you can use with this chugin:

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

Most of these plugins are available in CLAP format or are adding CLAP support.

## Troubleshooting

### Build Issues

**Problem**: CLAP headers not found
- **Solution**: The headers should be automatically cloned during build. If not, manually run:
  ```bash
  cd CLAP
  git clone https://github.com/free-audio/clap.git clap-headers
  ```

### Runtime Issues

**Problem**: Plugin fails to load
- **Solution**: Verify the plugin path is correct and the .clap file exists
- **Solution**: Make sure the plugin is compatible with your system architecture

**Problem**: No audio output from instrument
- **Solution**: Send MIDI note on messages: `clap.noteOn(60, 100)`
- **Solution**: Check if `clap.isInstrument()` returns true

**Problem**: No plugins found with `CLAP.list()`
- **Solution**: Install CLAP plugins in standard locations
- **Solution**: Set `CLAP_PATH` environment variable to your plugin directory

## Advantages of CLAP

Compared to other plugin formats:

### vs VST3
- **No SDK required** - CLAP headers are open source
- **No licensing restrictions** - Completely free
- **Modern design** - Built from scratch for modern workflows
- **Better thread safety** - Improved multi-threading support

### vs AudioUnit
- **Cross-platform** - Works on Windows and Linux too
- **Open standard** - Not tied to a single vendor
- **Extensible** - Easy to add new features

### vs LV2
- **Simpler** - Easier to implement and use
- **Better documentation** - More modern documentation

- **Growing ecosystem** - Rapidly increasing plugin support

## Contributing

CLAP is an open-source standard. To contribute:
- [CLAP Specification](https://github.com/free-audio/clap)
- [CLAP Helpers](https://github.com/free-audio/clap-helpers)
- [Community Forum](https://github.com/free-audio/clap/discussions)

## License

This chugin follows the same license as ChucK (GPL v2).

The CLAP headers are licensed under the MIT License.

## See Also

- [VST3 chugin](../VST3/) - Similar functionality for VST3 plugins
- [AudioUnit chugin](../AudioUnit/) - Similar functionality for macOS AudioUnit plugins
- [ChucK Documentation](https://chuck.cs.princeton.edu/doc/)
- [CLAP Website](https://cleveraudio.org/)
- [CLAP GitHub](https://github.com/free-audio/clap)
