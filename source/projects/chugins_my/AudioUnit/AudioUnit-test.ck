// AudioUnit Chugin Test
// Demonstrates loading and using AudioUnit plugins in ChucK

// First, list all available AudioUnits on the system
<<< "Listing all available AudioUnits..." >>>;
AudioUnit.list();

// Create an AudioUnit instance
adc => AudioUnit au => dac;

// Example 1: Load Apple's AUDelay by name
<<< "\n=== Example 1: Loading AUDelay by name ===" >>>;
if (au.load("AUDelay")) {
    <<< "Successfully loaded AUDelay!" >>>;

    // Get parameter count
    <<< "Number of parameters:", au.paramCount() >>>;

    // Print all parameters
    for (0 => int i; i < au.paramCount(); i++) {
        <<< "Parameter", i, ":", au.paramName(i), "=", au.getParam(i) >>>;
    }

    // Set some parameters (adjust indices based on actual AU parameters)
    // Typically parameter 0 might be delay time, 1 might be feedback, etc.
    if (au.paramCount() > 0) {
        au.setParam(0, 0.5);  // Example: set first parameter to 0.5
        <<< "Set parameter 0 to:", au.getParam(0) >>>;
    }

    // Let audio process for a few seconds
    3::second => now;

    <<< "Closing AUDelay..." >>>;
    au.close();
} else {
    <<< "Failed to load AUDelay" >>>;
}

// Example 2: Load by four-char code
<<< "\n=== Example 2: Loading AULowpass by four-char code ===" >>>;
if (au.open("aufx", "lpas", "appl")) {
    <<< "Successfully loaded AULowpass!" >>>;

    <<< "Number of parameters:", au.paramCount() >>>;

    for (0 => int i; i < au.paramCount(); i++) {
        <<< "Parameter", i, ":", au.paramName(i), "=", au.getParam(i) >>>;
    }

    // Process audio
    3::second => now;

    au.close();
} else {
    <<< "Failed to load AULowpass" >>>;
}

// Example 3: Bypass test
<<< "\n=== Example 3: Bypass test ===" >>>;
if (au.load("AUDelay")) {
    <<< "Loaded AUDelay - processing audio..." >>>;
    2::second => now;

    <<< "Bypassing AudioUnit..." >>>;
    au.bypass(1);
    2::second => now;

    <<< "Re-enabling AudioUnit..." >>>;
    au.bypass(0);
    2::second => now;

    au.close();
}

// Example 4: Load and control parameters dynamically
<<< "\n=== Example 4: Dynamic parameter control ===" >>>;

// Try loading AUBandpass
if (au.load("Bandpass")) {
    <<< "Successfully loaded Bandpass filter!" >>>;

    // Sweep a parameter over time
    <<< "Sweeping parameters..." >>>;

    0 => float t;
    while (t < 5.0) {
        // Modulate first parameter (usually cutoff frequency)
        if (au.paramCount() > 0) {
            Math.sin(t * 2 * pi / 5.0) * 0.5 + 0.5 => float value;
            au.setParam(0, value);
        }

        100::ms => now;
        t + 0.1 => t;
    }

    au.close();
} else {
    <<< "Could not load Bandpass filter" >>>;
}

<<< "\n=== AudioUnit test complete ===" >>>;

/*
 * Notes:
 *
 * - Use AudioUnit.list() to see all available AudioUnits on your system
 *
 * - Load AudioUnits by name using load():
 *   au.load("AUDelay");
 *
 * - Or load by four-char codes using open():
 *   au.open("aufx", "dely", "appl");
 *
 *   Where:
 *   - First argument is type (aufx=effect, aumu=music effect, aumu=instrument)
 *   - Second argument is subtype (effect-specific identifier)
 *   - Third argument is manufacturer (appl=Apple, etc.)
 *
 * - Control parameters:
 *   au.paramCount() - get number of parameters
 *   au.paramName(index) - get parameter name
 *   au.getParam(index) - get parameter value
 *   au.setParam(index, value) - set parameter value
 *
 * - Bypass:
 *   au.bypass(1) - bypass the effect
 *   au.bypass(0) - enable the effect
 *
 * - Close:
 *   au.close() - close the current AudioUnit
 *
 * Common Apple AudioUnits:
 * - AUDelay - Delay effect
 * - AULowpass - Low-pass filter
 * - AUHighpass - High-pass filter
 * - AUBandpass - Band-pass filter
 * - AUReverb - Reverb effect
 * - AUDistortion - Distortion effect
 * - AUPeakLimiter - Peak limiter
 * - AUDynamicsProcessor - Compressor/limiter
 */