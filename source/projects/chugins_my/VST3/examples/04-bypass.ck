// VST3 Bypass Example
// Demonstrates toggling VST3 bypass to compare processed vs unprocessed audio

TriOsc tri => VST3 vst => dac;
220 => tri.freq;

<<< "=== VST3 Bypass Example ===" >>>;

// Replace with a valid VST3 plugin path on your system
"/Library/Audio/Plug-Ins/VST3/JR-Granular.vst3" => string pluginPath;

if (vst.load(pluginPath)) {
    <<< "Successfully loaded VST3 plugin!" >>>;

    // Process audio normally
    <<< "Processing audio (effect enabled)..." >>>;
    3::second => now;

    // Bypass the effect - audio passes through unchanged
    <<< "Bypassing effect..." >>>;
    vst.bypass(1);
    3::second => now;

    // Re-enable the effect
    <<< "Re-enabling effect..." >>>;
    vst.bypass(0);
    3::second => now;

    // Toggle a few more times for comparison
    <<< "Toggling bypass on/off..." >>>;
    for (0 => int i; i < 3; i++) {
        <<< "Bypass ON" >>>;
        vst.bypass(1);
        1::second => now;

        <<< "Bypass OFF" >>>;
        vst.bypass(0);
        1::second => now;
    }

    vst.close();
} else {
    <<< "Failed to load VST3 plugin at:", pluginPath >>>;
    <<< "Run examples/00-list-vst3.ck to see available plugins" >>>;
}

<<< "=== Example complete ===" >>>;
