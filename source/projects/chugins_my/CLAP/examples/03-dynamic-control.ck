// CLAP Dynamic Parameter Control
// Demonstrates real-time parameter modulation with sine wave

PulseOsc osc => CLAP clap => Gain g => dac;
220 => osc.freq;
0.2 => osc.gain;
0.1 => g.gain;

//adc => CLAP clap => dac;

<<< "=== CLAP Dynamic Parameter Control Example ===" >>>;

// Replace with a valid CLAP plugin path on your system
"/Library/Audio/Plug-Ins/CLAP/TAL-Chorus-LX.clap" => string pluginPath;

if (clap.load(pluginPath)) {
    <<< "Successfully loaded CLAP plugin!" >>>;

    if (clap.paramCount() > 0) {
        <<< "Modulating parameter 0 with a sine wave for 10 seconds..." >>>;

        // Modulate the first parameter with a sine wave
        now => time start;
        while (now - start < 10::second) {
            // Calculate sine wave value (0.0 to 1.0)
            ((now - start) / second) => float t;
            Math.sin(t * 2 * pi / 2.0) * 0.5 + 0.5 => float value;

            // Set the parameter
            clap.setParam(0, value);

            // Update every 50ms
            50::ms => now;
        }

        <<< "Modulation complete!" >>>;
    } else {
        <<< "Plugin has no parameters to modulate" >>>;
        3::second => now;
    }

    clap.close();
} else {
    <<< "Failed to load CLAP plugin at:", pluginPath >>>;
    <<< "Run examples/00-list-clap.ck to see available plugins" >>>;
}

<<< "=== Example complete ===" >>>;
