// AudioUnit Parameter Control
// Demonstrates discovering and controlling AudioUnit parameters

@import "AudioUnit";

adc => AudioUnit au => dac;

<<< "=== AudioUnit Parameter Control Example ===" >>>;


if (au.load("AUDelay")) {
    <<< "Loaded AUDelay" >>>;


    // Discover parameters
    <<< "Parameter count:", au.paramCount() >>>;

    // Print all parameters with their current values
    <<< "Available parameters:" >>>;
    for (0 => int i; i < au.paramCount(); i++) {
        <<< "  [" + i + "]", au.paramName(i) + ":", au.getParam(i) >>>;
    }

    // Set parameters
    // Note: Parameter indices and ranges may vary by AudioUnit
    // These are typical values for AUDelay
    if (au.paramCount() > 0) {
        <<< "Setting parameter 0 (typically delay time) to 0.5..." >>>;
        au.setParam(0, 0.5);
        <<< "New value:", au.getParam(0) >>>;
    }

    if (au.paramCount() > 1) {
        <<< "Setting parameter 1 (typically feedback) to 0.3..." >>>;
        au.setParam(1, 0.3);
        <<< "New value:", au.getParam(1) >>>;
    }

    <<< "Processing audio with new settings for 5 seconds..." >>>;
    5::second => now;

    au.close();
} else {
    <<< "Failed to load AUDelay" >>>;
}


<<< "=== Example complete ===" >>>;