// VST3 Multiple Effects Example
// Demonstrates chaining multiple VST3 plugins in series

// Create a chain of VST3 plugins
adc => VST3 vst1 => VST3 vst2 => dac;

<<< "=== VST3 Multiple Effects Chain Example ===" >>>;

// Replace these paths with valid VST3 plugins on your system
"/Library/Audio/Plug-Ins/VST3/Filter.vst3" => string filterPath;
"/Library/Audio/Plug-Ins/VST3/Reverb.vst3" => string reverbPath;

// Load first effect
if (vst1.load(filterPath)) {
    <<< "Successfully loaded first VST3 plugin (filter)!" >>>;

    // Load second effect
    if (vst2.load(reverbPath)) {
        <<< "Successfully loaded second VST3 plugin (reverb)!" >>>;
        <<< "Processing audio through plugin chain for 8 seconds..." >>>;

        // Process audio through both effects
        8::second => now;

        <<< "Closing plugins..." >>>;
        vst1.close();
        vst2.close();

    } else {
        <<< "Failed to load second VST3 plugin at:", reverbPath >>>;
        <<< "Processing with just the first plugin..." >>>;
        5::second => now;
        vst1.close();
    }

} else {
    <<< "Failed to load first VST3 plugin at:", filterPath >>>;
    <<< "Run examples/00-list-vst3.ck to see available plugins" >>>;
}

<<< "=== Example complete ===" >>>;
