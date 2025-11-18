// VST3 Preset Control by Name
// Demonstrates selecting presets by name instead of index

adc => VST3 vst => dac;

<<< "=== VST3 Preset Control by Name Example ===" >>>;

// Replace with a valid VST3 plugin path on your system
"/Library/Audio/Plug-Ins/VST3/Serum2.vst3" => string pluginPath;

if (vst.load(pluginPath)) {
    <<< "Successfully loaded VST3 plugin!" >>>;

    if (vst.presetCount() > 0) {
        // List all available preset names
        <<< "Available presets:" >>>;
        vst.listPresets();

        // Set preset by name (adjust name to match your plugin)
        "Default" => string presetName;

        <<< "Attempting to load preset:", presetName >>>;
        if (vst.setPresetByName(presetName)) {
            <<< "Successfully loaded preset:", presetName >>>;
            3::second => now;
        } else {
            <<< "Preset '" + presetName + "' not found" >>>;
            <<< "Trying first available preset instead..." >>>;
            if (vst.setPreset(0)) {
                <<< "Loaded preset 0:", vst.presetName(0) >>>;
                3::second => now;
            }
        }

    } else {
        <<< "This plugin has no presets available" >>>;
        3::second => now;
    }

    vst.close();
} else {
    <<< "Failed to load VST3 plugin at:", pluginPath >>>;
    <<< "Run examples/00-list-vst3.ck to see available plugins" >>>;
}

<<< "=== Example complete ===" >>>;
