// CLAP Parameter Sweep Example
// Demonstrates various parameter modulation techniques

adc => CLAP clap => dac;

<<< "=== CLAP Parameter Sweep Example ===" >>>;

// Replace with a valid CLAP plugin path on your system
"/Library/Audio/Plug-Ins/CLAP/TAL-Filter-2.clap" => string pluginPath;

if (clap.load(pluginPath)) {
    <<< "Successfully loaded CLAP plugin!" >>>;

    if (clap.paramCount() > 0) {
        <<< "Parameter count:", clap.paramCount() >>>;

        // Linear sweep
        <<< "\n1. Linear sweep (0.0 to 1.0)..." >>>;
        for (0.0 => float v; v <= 1.0; 0.1 +=> v) {
            clap.setParam(0, v);
            200::ms => now;
        }

        // Sine wave modulation
        <<< "2. Sine wave modulation..." >>>;
        now => time start;
        while (now - start < 3::second) {
            ((now - start) / second) => float t;
            Math.sin(t * 2 * pi) * 0.5 + 0.5 => float value;
            clap.setParam(0, value);
            50::ms => now;
        }

        // Triangle wave
        <<< "3. Triangle wave modulation..." >>>;
        now => start;  // Reuse start variable
        while (now - start < 3::second) {
            ((now - start) / second) % 1.0 => float t;
            (t < 0.5 ? t * 2.0 : 2.0 - t * 2.0) => float value;
            clap.setParam(0, value);
            50::ms => now;
        }

        // Random values
        <<< "4. Random parameter values..." >>>;
        for (0 => int i; i < 10; i++) {
            Math.random2f(0.0, 1.0) => float value;
            <<< "  Setting to:", value >>>;
            clap.setParam(0, value);
            500::ms => now;
        }

    } else {
        <<< "Plugin has no parameters to sweep" >>>;
        3::second => now;
    }

    clap.close();
} else {
    <<< "Failed to load CLAP plugin at:", pluginPath >>>;
    <<< "Run examples/00-list-clap.ck to see available plugins" >>>;
}

<<< "=== Example complete ===" >>>;
