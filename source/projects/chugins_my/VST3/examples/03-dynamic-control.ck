// VST3 Dynamic Parameter Control
// Demonstrates real-time parameter modulation with sine wave

TriOsc tri => VST3 vst => dac;
220 => tri.freq;

<<< "=== VST3 Dynamic Parameter Control Example ===" >>>;

// Replace with a valid VST3 plugin path on your system
"/Library/Audio/Plug-Ins/VST3/TAL-Filter-2.vst3" => string pluginPath;

if (vst.load(pluginPath)) {
    <<< "Successfully loaded VST3 plugin!" >>>;

    if (vst.paramCount() > 0) {
        <<< "Modulating parameter 0 with a sine wave for 10 seconds..." >>>;

        // Modulate the first parameter with a sine wave
        now => time start;
        while (now - start < 10::second) {
            // Calculate sine wave value (0.0 to 1.0)
            ((now - start) / second) => float t;
            Math.sin(t * 2 * pi / 2.0) * 0.5 + 0.5 => float value;

            // Set the parameter
            vst.setParam(0, value);

            // Update every 50ms
            50::ms => now;
        }

        <<< "Modulation complete!" >>>;
    } else {
        <<< "Plugin has no parameters to modulate" >>>;
        3::second => now;
    }

    vst.close();
} else {
    <<< "Failed to load VST3 plugin at:", pluginPath >>>;
    <<< "Run examples/00-list-vst3.ck to see available plugins" >>>;
}

<<< "=== Example complete ===" >>>;
