// CLAP Parameter Control by Name
// Demonstrates controlling parameters by name instead of index (more readable)

adc => CLAP clap => dac;

<<< "=== CLAP Parameter Control by Name Example ===" >>>;

// Replace with a valid CLAP plugin path on your system
"/Library/Audio/Plug-Ins/CLAP/TAL-Chorus-LX.clap" => string pluginPath;

if (clap.load(pluginPath)) {
    <<< "Successfully loaded CLAP plugin!" >>>;

    // List all parameter names
    <<< "Available parameters:" >>>;
    for (0 => int i; i < clap.paramCount(); i++) {
        <<< "  -", clap.paramName(i), "=", clap.getParam(i) >>>;
    }

    // Set parameters by name (more readable than using indices)
    // Note: Parameter names are plugin-specific
    // Common examples shown below - adjust for your specific plugin

    // Example 1: Set "Mix" parameter
    if (clap.setParamByName("Mix", 0.75)) {
        <<< "Set 'Mix' parameter to 0.75" >>>;
        <<< "Current value:", clap.getParamByName("Mix") >>>;
    } else {
        <<< "'Mix' parameter not found in this plugin" >>>;
    }

    // Example 2: Set "Feedback" parameter
    if (clap.setParamByName("Feedback", 0.4)) {
        <<< "Set 'Feedback' parameter to 0.4" >>>;
        <<< "Current value:", clap.getParamByName("Feedback") >>>;
    } else {
        <<< "'Feedback' parameter not found in this plugin" >>>;
    }

    // Example 3: Set "Delay Time" parameter
    if (clap.setParamByName("Delay Time", 0.6)) {
        <<< "Set 'Delay Time' parameter to 0.6" >>>;
        <<< "Current value:", clap.getParamByName("Delay Time") >>>;
    } else {
        <<< "'Delay Time' parameter not found in this plugin" >>>;
    }

    <<< "Processing audio with named parameters for 5 seconds..." >>>;
    5::second => now;

    clap.close();
} else {
    <<< "Failed to load CLAP plugin at:", pluginPath >>>;
    <<< "Run examples/00-list-clap.ck to see available plugins" >>>;
}

<<< "=== Example complete ===" >>>;
