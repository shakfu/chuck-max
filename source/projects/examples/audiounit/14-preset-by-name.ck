// AudioUnit Preset Control by Name
// Demonstrates using preset names instead of indices
// Uses AUDistortion which has 22 factory presets with descriptive names

@import "AudioUnit";

adc => AudioUnit au => dac;

<<< "=== AudioUnit Preset Control by Name Example ===" >>>;


if (au.load("AUDistortion")) {
    <<< "Loaded AUDistortion" >>>;

    // Discover presets
    <<< "Preset count:", au.presetCount() >>>;

    // Print all presets with their indices
    <<< "Available presets:" >>>;
    for (0 => int i; i < au.presetCount(); i++) {
        <<< "  [" + i + "]", au.presetName(i) >>>;
    }

    // Set presets by name (more readable and maintainable!)
    // Note: Preset names are case-sensitive

    // Try setting a specific preset by name
    <<< "Setting 'Speech- Alien Chatter' preset by name..." >>>;
    if (au.setPresetByName("Speech- Alien Chatter")) {
        <<< "Successfully set preset" >>>;
        <<< "Current preset index:", au.getPreset() >>>;

        <<< "Processing audio with preset for 3 seconds..." >>>;
        3::second => now;
    } else {
        <<< "Preset not found" >>>;
    }

    // Try another preset
    <<< "Setting 'Multi- Broken Speaker' preset by name..." >>>;
    if (au.setPresetByName("Multi- Broken Speaker")) {
        <<< "Successfully set preset" >>>;
        <<< "Current preset index:", au.getPreset() >>>;

        <<< "Processing audio with preset for 3 seconds..." >>>;
        3::second => now;
    } else {
        <<< "Preset not found" >>>;
    }

    // Try setting a non-existent preset
    <<< "Trying to set non-existent preset 'NonExistent'..." >>>;
    if (au.setPresetByName("NonExistent")) {
        <<< "Successfully set preset" >>>;
    } else {
        <<< "Preset 'NonExistent' not found (as expected)" >>>;
    }

    au.close();
} else {
    <<< "Failed to load AUDistortion" >>>;
}


<<< "=== Example complete ===" >>>;
