// Basic VST3 Effect Example
// Demonstrates loading and using a VST3 plugin as an audio effect

// Create a VST3 instance and connect it in the audio chain
TriOsc tri => VST3 vst => dac;
220 => tri.freq;

<<< "=== Basic VST3 Effect Example ===" >>>;

// Replace this path with an actual VST3 plugin on your system
// Common VST3 plugin locations:
// macOS: /Library/Audio/Plug-Ins/VST3/ or ~/Library/Audio/Plug-Ins/VST3/
// Windows: C:\Program Files\Common Files\VST3\
// Linux: /usr/lib/vst3/ or ~/.vst3/

// Example: Using a filter effect (adjust path to match your system)
"/Library/Audio/Plug-Ins/VST3/TAL-Filter-2.vst3" => string pluginPath;

// Load the VST3 plugin by file path
if (vst.load(pluginPath)) {
    <<< "Successfully loaded VST3 plugin!" >>>;
    <<< "Processing audio from ADC through VST3 for 5 seconds..." >>>;

    // Process audio for 5 seconds
    5::second => now;

    <<< "Closing VST3 plugin..." >>>;
    vst.close();
} else {
    <<< "Failed to load VST3 plugin at:", pluginPath >>>;
    <<< "Please update pluginPath with a valid VST3 plugin on your system" >>>;
    <<< "Run examples/00-list-vst3.ck to see available plugins" >>>;
}

<<< "=== Example complete ===" >>>;
