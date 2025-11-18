// AudioUnit Parameter Control by Name
// Demonstrates using parameter names instead of indices

@import "AudioUnit";

adc => AudioUnit au => dac;

<<< "=== AudioUnit Parameter Control by Name Example ===" >>>;


if (au.load("AUDelay")) {
    <<< "Loaded AUDelay" >>>;


    // Discover parameters
    <<< "Parameter count:", au.paramCount() >>>;

    // Print all parameters with their current values
    <<< "Available parameters:" >>>;
    for (0 => int i; i < au.paramCount(); i++) {
        <<< "  [" + i + "]", au.paramName(i) + ":", au.getParam(i) >>>;
    }

    // Set parameters by name (more readable and maintainable!)
    // Note: Parameter names are case-sensitive

    <<< "\nSetting 'Delay Time' to 2.0..." >>>;
    if (au.setParamByName("Delay Time", 2.0)) {
        <<< "Successfully set Delay Time" >>>;
        <<< "Current value:", au.getParamByName("Delay Time") >>>;
    } else {
        <<< "Parameter 'Delay Time' not found" >>>;
    }

    <<< "\nSetting 'Feedback' to 60.0..." >>>;
    if (au.setParamByName("Feedback", 60.0)) {
        <<< "Successfully set Feedback" >>>;
        <<< "Current value:", au.getParamByName("Feedback") >>>;
    } else {
        <<< "Parameter 'Feedback' not found" >>>;
    }

    <<< "\nSetting 'Dry/Wet Mix' to 50.0..." >>>;
    if (au.setParamByName("Dry/Wet Mix", 50.0)) {
        <<< "Successfully set Dry/Wet Mix" >>>;
        <<< "Current value:", au.getParamByName("Dry/Wet Mix") >>>;
    } else {
        <<< "Parameter 'Dry/Wet Mix' not found" >>>;
    }

    <<< "\nProcessing audio with new settings for 10 seconds..." >>>;
    10::second => now;

    au.close();
} else {
    <<< "Failed to load AUDelay" >>>;
}


<<< "=== Example complete ===" >>>;
