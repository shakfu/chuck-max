// VST3 Chugin Test
// Demonstrates loading and using VST3 plugins in ChucK

// First, list all available VST3 plugins on the system
<<< "Listing all available VST3 plugins..." >>>;
VST3.list();

// Create a VST3 instance
adc => VST3 vst => dac;

// Example 1: Load a VST3 plugin by path
<<< "\n=== Example 1: Loading VST3 by path ===" >>>;

// Replace this path with an actual VST3 plugin on your system
// Common paths:
// macOS: "/Library/Audio/Plug-Ins/VST3/YourPlugin.vst3"
// Windows: "C:\\Program Files\\Common Files\\VST3\\YourPlugin.vst3"
// Linux: "/usr/lib/vst3/YourPlugin.vst3"

"/Library/Audio/Plug-Ins/VST3/ExampleDelay.vst3" => string pluginPath;

if (vst.load(pluginPath)) {
    <<< "Successfully loaded VST3 plugin!" >>>;

    // Get parameter count
    <<< "Number of parameters:", vst.paramCount() >>>;

    // Print all parameters
    for (0 => int i; i < vst.paramCount(); i++) {
        <<< "Parameter", i, ":", vst.paramName(i), "=", vst.getParam(i) >>>;
    }

    // Set some parameters
    if (vst.paramCount() > 0) {
        vst.setParam(0, 0.5);  // Example: set first parameter to 0.5
        <<< "Set parameter 0 to:", vst.getParam(0) >>>;
    }

    // Let audio process for a few seconds
    3::second => now;

    <<< "Closing VST3..." >>>;
    vst.close();
} else {
    <<< "Failed to load VST3 plugin at:", pluginPath >>>;
    <<< "Please update the pluginPath variable with a valid VST3 plugin on your system" >>>;
}

// Example 2: Parameter control by name
<<< "\n=== Example 2: Parameter control by name ===" >>>;

if (vst.load(pluginPath)) {
    <<< "Successfully loaded VST3 plugin!" >>>;

    // List all parameter names
    <<< "Available parameters:" >>>;
    for (0 => int i; i < vst.paramCount(); i++) {
        <<< " -", vst.paramName(i) >>>;
    }

    // Set parameter by name (adjust name to match your plugin)
    if (vst.setParamByName("Mix", 0.75)) {
        <<< "Set 'Mix' parameter to 0.75" >>>;
        <<< "Current value:", vst.getParamByName("Mix") >>>;
    }

    3::second => now;
    vst.close();
} else {
    <<< "Failed to load VST3 plugin" >>>;
}

// Example 3: Bypass test
<<< "\n=== Example 3: Bypass test ===" >>>;

if (vst.load(pluginPath)) {
    <<< "Loaded VST3 - processing audio..." >>>;
    2::second => now;

    <<< "Bypassing VST3..." >>>;
    vst.bypass(1);
    2::second => now;

    <<< "Re-enabling VST3..." >>>;
    vst.bypass(0);
    2::second => now;

    vst.close();
}

// Example 4: Preset management
<<< "\n=== Example 4: Preset management ===" >>>;

if (vst.load(pluginPath)) {
    <<< "Successfully loaded VST3 plugin!" >>>;

    // Get preset count
    <<< "Number of presets:", vst.presetCount() >>>;

    // List all presets
    if (vst.presetCount() > 0) {
        <<< "Available presets:" >>>;
        vst.listPresets();

        // Load different presets
        for (0 => int i; i < vst.presetCount(); i++) {
            <<< "\nLoading preset", i, ":", vst.presetName(i) >>>;
            vst.setPreset(i);
            1::second => now;
        }
    } else {
        <<< "No presets available for this plugin" >>>;
    }

    vst.close();
}

// Example 5: Dynamic parameter modulation
<<< "\n=== Example 5: Dynamic parameter modulation ===" >>>;

if (vst.load(pluginPath)) {
    <<< "Successfully loaded VST3 plugin!" >>>;

    // Sweep a parameter over time
    <<< "Sweeping parameters..." >>>;

    0 => float t;
    while (t < 5.0) {
        // Modulate first parameter
        if (vst.paramCount() > 0) {
            Math.sin(t * 2 * pi / 5.0) * 0.5 + 0.5 => float value;
            vst.setParam(0, value);
        }

        100::ms => now;
        t + 0.1 => t;
    }

    vst.close();
}

// Example 6: VST3 Instrument (if available)
<<< "\n=== Example 6: VST3 Instrument test ===" >>>;

// Replace with a VST3 instrument path
"/Library/Audio/Plug-Ins/VST3/ExampleSynth.vst3" => string synthPath;

if (vst.load(synthPath)) {
    <<< "Successfully loaded VST3 plugin!" >>>;

    if (vst.isInstrument()) {
        <<< "This is a VST3 instrument - testing MIDI..." >>>;

        // Play a melody
        [60, 64, 67, 72] @=> int notes[];

        for (0 => int i; i < notes.size(); i++) {
            vst.noteOn(notes[i], 100);  // Note on with velocity 100
            500::ms => now;
            vst.noteOff(notes[i]);      // Note off
            100::ms => now;
        }

        // Test control change
        <<< "Sending control change (CC 1 = 64)..." >>>;
        vst.controlChange(1, 64);
        1::second => now;

        // Test program change
        if (vst.presetCount() > 1) {
            <<< "Changing program..." >>>;
            vst.programChange(1);
            1::second => now;
        }

    } else {
        <<< "This is not a VST3 instrument - it's an effect" >>>;
    }

    vst.close();
} else {
    <<< "Failed to load VST3 instrument at:", synthPath >>>;
    <<< "Please update synthPath with a valid VST3 instrument plugin" >>>;
}

<<< "\n=== VST3 test complete ===" >>>;

/*
 * Notes:
 *
 * - Use VST3.list() to see all available VST3 plugins on your system
 *
 * - Load VST3 plugins by file path using load():
 *   vst.load("/path/to/plugin.vst3");
 *
 * - Control parameters:
 *   vst.paramCount() - get number of parameters
 *   vst.paramName(index) - get parameter name
 *   vst.getParam(index) - get parameter value
 *   vst.setParam(index, value) - set parameter value
 *   vst.setParamByName("name", value) - set parameter by name
 *   vst.getParamByName("name") - get parameter value by name
 *
 * - Preset management:
 *   vst.presetCount() - get number of presets
 *   vst.presetName(index) - get preset name
 *   vst.setPreset(index) - load preset by index
 *   vst.setPresetByName("name") - load preset by name
 *   vst.getPreset() - get current preset index
 *   vst.listPresets() - print all available presets
 *
 * - Bypass:
 *   vst.bypass(1) - bypass the effect
 *   vst.bypass(0) - enable the effect
 *
 * - MIDI (for instruments):
 *   vst.isInstrument() - check if plugin is an instrument
 *   vst.noteOn(pitch, velocity) - send note on
 *   vst.noteOff(pitch) - send note off
 *   vst.controlChange(cc, value) - send control change
 *   vst.programChange(program) - send program change
 *   vst.sendMIDI(status, data1, data2) - send raw MIDI message
 *
 * - Close:
 *   vst.close() - close the current VST3 plugin
 *
 * Common VST3 plugin locations:
 *
 * macOS:
 *   /Library/Audio/Plug-Ins/VST3/
 *   ~/Library/Audio/Plug-Ins/VST3/
 *
 * Windows:
 *   C:\Program Files\Common Files\VST3\
 *   C:\Program Files (x86)\Common Files\VST3\
 *
 * Linux:
 *   /usr/lib/vst3/
 *   /usr/local/lib/vst3/
 *   ~/.vst3/
 */
