// VST3 Preset Control
// Demonstrates discovering and selecting VST3 plugin presets

TriOsc tri => VST3 vst => dac;
220 => tri.freq;

<<< "=== VST3 Preset Control Example ===" >>>;

// Replace with a valid VST3 plugin path on your system
"/Library/Audio/Plug-Ins/VST3/JR-Granular.vst3" => string pluginPath;

if (vst.load(pluginPath)) {
    <<< "Successfully loaded VST3 plugin!" >>>;

    // Discover presets
    <<< "Preset count:", vst.presetCount() >>>;

    if (vst.presetCount() > 0) {
        // List all available presets
        <<< "Available presets:" >>>;
        for (0 => int i; i < vst.presetCount(); i++) {
            <<< "  [" + i + "]", vst.presetName(i) >>>;
        }

        // Get current preset
        <<< "Current preset index:", vst.getPreset() >>>;

        // Set presets by index
        <<< "Setting preset 0..." >>>;
        if (vst.setPreset(0)) {
            <<< "Successfully set preset 0:", vst.presetName(0) >>>;
            <<< "Current preset:", vst.getPreset() >>>;
        }

        // Process audio with preset 0
        <<< "Processing audio with preset 0 for 3 seconds..." >>>;
        3::second => now;

        // Try another preset if available
        if (vst.presetCount() > 1) {
            <<< "Setting preset 1..." >>>;
            if (vst.setPreset(1)) {
                <<< "Successfully set preset 1:", vst.presetName(1) >>>;
                <<< "Current preset:", vst.getPreset() >>>;
            }

            <<< "Processing audio with preset 1 for 3 seconds..." >>>;
            3::second => now;
        }

        // List all presets to console (for reference)
        <<< "Calling listPresets():" >>>;
        vst.listPresets();

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
