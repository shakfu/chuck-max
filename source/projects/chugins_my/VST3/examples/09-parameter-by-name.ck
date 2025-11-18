// VST3 Parameter Control by Name
// Demonstrates controlling parameters by name instead of index (more readable)

adc => VST3 vst => dac;

<<< "=== VST3 Parameter Control by Name Example ===" >>>;

// Replace with a valid VST3 plugin path on your system
"/Library/Audio/Plug-Ins/VST3/ExampleDelay.vst3" => string pluginPath;

if (vst.load(pluginPath)) {
    <<< "Successfully loaded VST3 plugin!" >>>;

    // List all parameter names
    <<< "Available parameters:" >>>;
    for (0 => int i; i < vst.paramCount(); i++) {
        <<< "  -", vst.paramName(i), "=", vst.getParam(i) >>>;
    }

    // Set parameters by name (more readable than using indices)
    // Note: Parameter names are plugin-specific
    // Common examples shown below - adjust for your specific plugin

    // Example 1: Set "Mix" parameter
    if (vst.setParamByName("Mix", 0.75)) {
        <<< "Set 'Mix' parameter to 0.75" >>>;
        <<< "Current value:", vst.getParamByName("Mix") >>>;
    } else {
        <<< "'Mix' parameter not found in this plugin" >>>;
    }

    // Example 2: Set "Feedback" parameter
    if (vst.setParamByName("Feedback", 0.4)) {
        <<< "Set 'Feedback' parameter to 0.4" >>>;
        <<< "Current value:", vst.getParamByName("Feedback") >>>;
    } else {
        <<< "'Feedback' parameter not found in this plugin" >>>;
    }

    // Example 3: Set "Delay Time" parameter
    if (vst.setParamByName("Delay Time", 0.6)) {
        <<< "Set 'Delay Time' parameter to 0.6" >>>;
        <<< "Current value:", vst.getParamByName("Delay Time") >>>;
    } else {
        <<< "'Delay Time' parameter not found in this plugin" >>>;
    }

    <<< "Processing audio with named parameters for 5 seconds..." >>>;
    5::second => now;

    vst.close();
} else {
    <<< "Failed to load VST3 plugin at:", pluginPath >>>;
    <<< "Run examples/00-list-vst3.ck to see available plugins" >>>;
}

<<< "=== Example complete ===" >>>;
