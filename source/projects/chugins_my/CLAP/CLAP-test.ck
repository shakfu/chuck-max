// CLAP Chugin Test
// Demonstrates loading and using CLAP (CLever Audio Plugin) plugins in ChucK

// First, list all available CLAP plugins on the system
<<< "Listing all available CLAP plugins..." >>>;
CLAP.list();

// Create a CLAP instance
adc => CLAP clap => dac;

// Example 1: Load a CLAP plugin by path
<<< "=== Example 1: Loading CLAP by path ===" >>>;

// Replace this path with an actual CLAP plugin on your system
// Common paths:
// macOS: "/Library/Audio/Plug-Ins/CLAP/YourPlugin.clap"
// Windows: "C:\\Program Files\\Common Files\\CLAP\\YourPlugin.clap"
// Linux: "/usr/lib/clap/YourPlugin.clap"

"/Library/Audio/Plug-Ins/CLAP/ExampleDelay.clap" => string pluginPath;

if (clap.load(pluginPath)) {
    <<< "Successfully loaded CLAP plugin!" >>>;

    // Get parameter count
    <<< "Number of parameters:", clap.paramCount() >>>;

    // Print all parameters
    for (0 => int i; i < clap.paramCount(); i++) {
        <<< "Parameter", i, ":", clap.paramName(i), "=", clap.getParam(i) >>>;
    }

    // Set some parameters
    if (clap.paramCount() > 0) {
        clap.setParam(0, 0.5);  // Example: set first parameter to 0.5
        <<< "Set parameter 0 to:", clap.getParam(0) >>>;
    }

    // Let audio process for a few seconds
    3::second => now;

    <<< "Closing CLAP..." >>>;
    clap.close();
} else {
    <<< "Failed to load CLAP plugin at:", pluginPath >>>;
    <<< "Please update the pluginPath variable with a valid CLAP plugin on your system" >>>;
}

// Example 2: Parameter control by name
<<< "=== Example 2: Parameter control by name ===" >>>;

if (clap.load(pluginPath)) {
    <<< "Successfully loaded CLAP plugin!" >>>;

    // List all parameter names
    <<< "Available parameters:" >>>;
    for (0 => int i; i < clap.paramCount(); i++) {
        <<< " -", clap.paramName(i) >>>;
    }

    // Set parameter by name (adjust name to match your plugin)
    if (clap.setParamByName("Mix", 0.75)) {
        <<< "Set 'Mix' parameter to 0.75" >>>;
        <<< "Current value:", clap.getParamByName("Mix") >>>;
    }

    3::second => now;
    clap.close();
} else {
    <<< "Failed to load CLAP plugin" >>>;
}

// Example 3: Bypass test
<<< "=== Example 3: Bypass test ===" >>>;

if (clap.load(pluginPath)) {
    <<< "Loaded CLAP - processing audio..." >>>;
    2::second => now;

    <<< "Bypassing CLAP..." >>>;
    clap.bypass(1);
    2::second => now;

    <<< "Re-enabling CLAP..." >>>;
    clap.bypass(0);
    2::second => now;

    clap.close();
}

// Example 4: Dynamic parameter modulation
<<< "=== Example 4: Dynamic parameter modulation ===" >>>;

if (clap.load(pluginPath)) {
    <<< "Successfully loaded CLAP plugin!" >>>;

    // Sweep a parameter over time
    <<< "Sweeping parameters..." >>>;

    0 => float t;
    while (t < 5.0) {
        // Modulate first parameter
        if (clap.paramCount() > 0) {
            Math.sin(t * 2 * pi / 5.0) * 0.5 + 0.5 => float value;
            clap.setParam(0, value);
        }

        100::ms => now;
        t + 0.1 => t;
    }

    clap.close();
}

// Example 5: CLAP Instrument (if available)
<<< "=== Example 5: CLAP Instrument test ===" >>>;

// Replace with a CLAP instrument path
"/Library/Audio/Plug-Ins/CLAP/ExampleSynth.clap" => string synthPath;

if (clap.load(synthPath)) {
    <<< "Successfully loaded CLAP plugin!" >>>;

    if (clap.isInstrument()) {
        <<< "This is a CLAP instrument - testing MIDI..." >>>;

        // Play a melody
        [60, 64, 67, 72] @=> int notes[];

        for (0 => int i; i < notes.size(); i++) {
            clap.noteOn(notes[i], 100);  // Note on with velocity 100
            500::ms => now;
            clap.noteOff(notes[i]);      // Note off
            100::ms => now;
        }

    } else {
        <<< "This is not a CLAP instrument - it's an effect" >>>;
    }

    clap.close();
} else {
    <<< "Failed to load CLAP instrument at:", synthPath >>>;
    <<< "Please update synthPath with a valid CLAP instrument plugin" >>>;
}

// Example 6: Multiple CLAP plugins in series
<<< "=== Example 6: CLAP plugin chain ===" >>>;

// Create a chain of CLAP plugins
adc => CLAP clap1 => CLAP clap2 => dac;

"/Library/Audio/Plug-Ins/CLAP/Filter.clap" => string filterPath;
"/Library/Audio/Plug-Ins/CLAP/Reverb.clap" => string reverbPath;

if (clap1.load(filterPath) && clap2.load(reverbPath)) {
    <<< "Loaded filter and reverb in series!" >>>;

    // Process audio through the chain
    5::second => now;

    clap1.close();
    clap2.close();
} else {
    <<< "Could not load plugin chain" >>>;
}

<<< "=== CLAP test complete ===" >>>;

/*
 * Notes:
 *
 * - Use CLAP.list() to see all available CLAP plugins on your system
 *
 * - Load CLAP plugins by file path using load():
 *   clap.load("/path/to/plugin.clap");
 *
 * - Control parameters:
 *   clap.paramCount() - get number of parameters
 *   clap.paramName(index) - get parameter name
 *   clap.getParam(index) - get parameter value
 *   clap.setParam(index, value) - set parameter value
 *   clap.setParamByName("name", value) - set parameter by name
 *   clap.getParamByName("name") - get parameter value by name
 *
 * - Bypass:
 *   clap.bypass(1) - bypass the effect
 *   clap.bypass(0) - enable the effect
 *
 * - MIDI (for instruments):
 *   clap.isInstrument() - check if plugin is an instrument
 *   clap.noteOn(pitch, velocity) - send note on
 *   clap.noteOff(pitch) - send note off
 *   clap.sendMIDI(status, data1, data2) - send raw MIDI message
 *
 * - Close:
 *   clap.close() - close the current CLAP plugin
 *
 * Common CLAP plugin locations:
 *
 * macOS:
 *   /Library/Audio/Plug-Ins/CLAP/
 *   ~/Library/Audio/Plug-Ins/CLAP/
 *
 * Windows:
 *   C:\Program Files\Common Files\CLAP\
 *   %LOCALAPPDATA%\Programs\Common\CLAP\
 *
 * Linux:
 *   /usr/lib/clap/
 *   /usr/local/lib/clap/
 *   ~/.clap/
 *
 * You can also set the CLAP_PATH environment variable to add additional
 * search directories.
 *
 * Free CLAP plugins to try:
 * - Surge XT (synth/effects) - https://surge-synthesizer.github.io/
 * - Vital (synth) - https://vital.audio/
 * - Dexed (FM synth) - https://asb2m10.github.io/dexed/
 * - Airwindows (effects) - https://www.airwindows.com/
 */
