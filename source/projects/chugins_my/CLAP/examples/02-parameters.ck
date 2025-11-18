// CLAP Parameter Control
// Demonstrates discovering and controlling CLAP plugin parameters

adc => CLAP clap => dac;

<<< "=== CLAP Parameter Control Example ===" >>>;

// Replace with a valid CLAP plugin path on your system
"/Library/Audio/Plug-Ins/CLAP/TAL-Reverb-4.clap" => string pluginPath;

if (clap.load(pluginPath)) {
    <<< "Successfully loaded CLAP plugin!" >>>;

    // Discover parameters
    <<< "Parameter count:", clap.paramCount() >>>;

    // Print all parameters with their current values
    <<< "Available parameters:" >>>;
    for (0 => int i; i < clap.paramCount(); i++) {
        <<< "  [" + i + "]", clap.paramName(i) + ":", clap.getParam(i) >>>;
    }

    // Set parameters
    // Note: Parameter indices and ranges may vary by plugin
    if (clap.paramCount() > 0) {
        <<< "Setting parameter 0 to 0.5..." >>>;
        clap.setParam(0, 0.5);
        <<< "New value:", clap.getParam(0) >>>;
    }

    if (clap.paramCount() > 1) {
        <<< "Setting parameter 1 to 0.3..." >>>;
        clap.setParam(1, 0.3);
        <<< "New value:", clap.getParam(1) >>>;
    }

    <<< "Processing audio with new settings for 5 seconds..." >>>;
    5::second => now;

    clap.close();
} else {
    <<< "Failed to load CLAP plugin at:", pluginPath >>>;
    <<< "Run examples/00-list-clap.ck to see available plugins" >>>;
}

<<< "=== Example complete ===" >>>;
