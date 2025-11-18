// VST3 Parameter Control
// Demonstrates discovering and controlling VST3 plugin parameters

TriOsc tri => VST3 vst => dac;
220 => tri.freq;

<<< "=== VST3 Parameter Control Example ===" >>>;

// Replace with a valid VST3 plugin path on your system
"/Library/Audio/Plug-Ins/VST3/TAL-Filter-2.vst3" => string pluginPath;

if (vst.load(pluginPath)) {
    <<< "Successfully loaded VST3 plugin!" >>>;

    // Discover parameters
    <<< "Parameter count:", vst.paramCount() >>>;

    // Print all parameters with their current values
    <<< "Available parameters:" >>>;
    for (0 => int i; i < vst.paramCount(); i++) {
        <<< "  [" + i + "]", vst.paramName(i) + ":", vst.getParam(i) >>>;
    }

    // Set parameters
    // Note: Parameter indices and ranges may vary by plugin
    if (vst.paramCount() > 0) {
        <<< "Setting parameter 0 to 0.5..." >>>;
        vst.setParam(0, 0.5);
        <<< "New value:", vst.getParam(0) >>>;
    }

    if (vst.paramCount() > 1) {
        <<< "Setting parameter 1 to 0.3..." >>>;
        vst.setParam(1, 0.3);
        <<< "New value:", vst.getParam(1) >>>;
    }

    <<< "Processing audio with new settings for 5 seconds..." >>>;
    5::second => now;

    vst.close();
} else {
    <<< "Failed to load VST3 plugin at:", pluginPath >>>;
    <<< "Run examples/00-list-vst3.ck to see available plugins" >>>;
}

<<< "=== Example complete ===" >>>;
