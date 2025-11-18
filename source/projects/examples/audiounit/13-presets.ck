// AudioUnit Preset Control
// Demonstrates discovering and selecting AudioUnit presets
// Uses AUReverb which has 13 factory presets

@import "AudioUnit";

adc => AudioUnit au => dac;

<<< "=== AudioUnit Preset Control Example ===" >>>;


if (au.load("AUReverb")) {
    <<< "Loaded AUReverb" >>>;

    // Discover presets
    <<< "Preset count:", au.presetCount() >>>;

    // List all available presets
    <<< "Available presets:" >>>;
    for (0 => int i; i < au.presetCount(); i++) {
        <<< "  [" + i + "]", au.presetName(i) >>>;
    }

    // Get current preset
    <<< "Current preset index:", au.getPreset() >>>;

    // Set presets by index
    if (au.presetCount() > 0) {
        <<< "Setting preset 0..." >>>;
        if (au.setPreset(0)) {
            <<< "Successfully set preset 0:", au.presetName(0) >>>;
            <<< "Current preset:", au.getPreset() >>>;
        }
    }

    // Wait a bit to hear the effect
    <<< "Processing audio with preset 0 for 3 seconds..." >>>;
    3::second => now;

    // Try another preset if available
    if (au.presetCount() > 1) {
        <<< "Setting preset 1..." >>>;
        if (au.setPreset(1)) {
            <<< "Successfully set preset 1:", au.presetName(1) >>>;
            <<< "Current preset:", au.getPreset() >>>;
        }

        <<< "Processing audio with preset 1 for 3 seconds..." >>>;
        3::second => now;
    }

    // List all presets to console (for reference)
    <<< "Calling listPresets():" >>>;
    au.listPresets();

    au.close();
} else {
    <<< "Failed to load AUReverb" >>>;
}


<<< "=== Example complete ===" >>>;
