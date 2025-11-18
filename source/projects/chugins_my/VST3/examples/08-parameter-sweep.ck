// VST3 Parameter Sweep Example
// Demonstrates various parameter modulation techniques

TriOsc tri => VST3 vst => dac;
220 => tri.freq;

<<< "=== VST3 Parameter Sweep Example ===" >>>;

// Replace with a valid VST3 plugin path on your system
"/Library/Audio/Plug-Ins/VST3/TAL-Filter-2.vst3" => string pluginPath;

if (vst.load(pluginPath)) {
    <<< "Successfully loaded VST3 plugin!" >>>;

    if (vst.paramCount() > 0) {
        <<< "Parameter count:", vst.paramCount() >>>;

        // Linear sweep
        <<< "\n1. Linear sweep (0.0 to 1.0)..." >>>;
        for (0.0 => float v; v <= 1.0; 0.1 +=> v) {
            vst.setParam(0, v);
            200::ms => now;
        }

        // Sine wave modulation
        <<< "2. Sine wave modulation..." >>>;
        now => time start;
        while (now - start < 3::second) {
            ((now - start) / second) => float t;
            Math.sin(t * 2 * pi) * 0.5 + 0.5 => float value;
            vst.setParam(0, value);
            50::ms => now;
        }

        // Triangle wave
        <<< "3. Triangle wave modulation..." >>>;
        now => time start;
        while (now - start < 3::second) {
            ((now - start) / second) % 1.0 => float t;
            (t < 0.5 ? t * 2.0 : 2.0 - t * 2.0) => float value;
            vst.setParam(0, value);
            50::ms => now;
        }

        // Random values
        <<< "4. Random parameter values..." >>>;
        for (0 => int i; i < 10; i++) {
            Math.random2f(0.0, 1.0) => float value;
            <<< "  Setting to:", value >>>;
            vst.setParam(0, value);
            500::ms => now;
        }

    } else {
        <<< "Plugin has no parameters to sweep" >>>;
        3::second => now;
    }

    vst.close();
} else {
    <<< "Failed to load VST3 plugin at:", pluginPath >>>;
    <<< "Run examples/00-list-vst3.ck to see available plugins" >>>;
}

<<< "=== Example complete ===" >>>;
