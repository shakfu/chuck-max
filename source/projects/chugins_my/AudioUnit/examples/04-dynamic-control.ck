// Dynamic Parameter Control
// Demonstrates real-time parameter modulation

@import "AudioUnit";

adc => AudioUnit au => dac;

<<< "=== Dynamic Parameter Control Example ===" >>>;


// Load a filter effect
if (au.load("Lowpass")) {
    <<< "Loaded Lowpass filter" >>>;


    // Print parameters
    <<< "Parameters:" >>>;
    for (0 => int i; i < au.paramCount(); i++) {
        <<< "  [" + i + "]", au.paramName(i) >>>;
    }


    <<< "Sweeping first parameter (typically cutoff frequency) with a sine wave..." >>>;
    <<< "Duration: 10 seconds" >>>;


    // Sweep parameter 0 over time
    0.0 => float t;
    10.0 => float duration;

    while (t < duration) {
        // Generate sine wave modulation (0.0 to 1.0 range)
        Math.sin(t * 2.0 * pi / 5.0) * 0.5 + 0.5 => float value;

        // Apply to first parameter
        if (au.paramCount() > 0) {
            au.setParam(0, value);
        }

        // Update every 50ms
        50::ms => now;
        t + 0.05 => t;
    }

    au.close();
    <<< "Closed AudioUnit" >>>;
} else {
    <<< "Failed to load Lowpass filter" >>>;
    <<< "Try another filter effect available on your system" >>>;
}


<<< "=== Example complete ===" >>>;