// Basic CLAP Effect Example
// Demonstrates loading and using a CLAP plugin as an audio effect

SinOsc osc => CLAP clap => Gain g => dac;
440 => osc.freq;
0.3 => osc.gain;
0.5 => g.gain;

// Example: Using a filter effect (adjust path to match your system)
"/Library/Audio/Plug-Ins/CLAP/TAL-Filter-2.clap/Contents/MacOS/TAL-Filter-2" => string pluginPath;

// Load the CLAP plugin by file path
if (clap.load(pluginPath)) {
    <<< "Successfully loaded CLAP plugin!" >>>;
    <<< "Parameter count:", clap.paramCount() >>>;

    // Print available parameters
    if (clap.paramCount() > 0) {
        <<< "Available parameters:" >>>;
        for (0 => int i; i < Math.min(clap.paramCount(), 10); i++) {
            <<< "  [" + i + "]", clap.paramName(i) + ":", clap.getParam(i) >>>;
        }
    }

    // Play with default settings for 2 seconds
    <<< "Playing with default settings..." >>>;
    2::second => now;

    // Try to load preset (may not be supported by all plugins)
    if (clap.presetCount() > 0) {
        <<< "Preset count:", clap.presetCount() >>>;
        <<< "Loading preset 0..." >>>;
        clap.loadPreset(0);
        2::second => now;
    } else {
        <<< "Plugin does not support presets, modifying parameters instead..." >>>;
        // Set some parameters to make an audible difference
        if (clap.paramCount() > 0) {
            clap.setParam(5, 0.9);  // Change first parameter
        }
        if (clap.paramCount() > 1) {
            clap.setParam(3, 0.8);  // Change second parameter
        }
        2::second => now;
    }

    <<< "Closing CLAP plugin..." >>>;
    clap.close();
} else {
    <<< "Failed to load CLAP plugin at:", pluginPath >>>;
}
