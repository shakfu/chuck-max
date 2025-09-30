// Parameter Sweep and Modulation
// Demonstrates various parameter modulation techniques

@import "AudioUnit";

SinOsc osc => AudioUnit au => dac;

220 => osc.freq;
0.4 => osc.gain;

<<< "=== Parameter Sweep and Modulation Example ===" >>>;


if (au.load("AULowpass")) {
    <<< "Loaded AULowpass filter" >>>;

    // Display parameters
    <<< "Parameters:" >>>;
    for (0 => int i; i < au.paramCount(); i++) {
        <<< "  [" + i + "]", au.paramName(i), "=", au.getParam(i) >>>;
    }

    if (au.paramCount() > 0) {
        // Example 1: Linear sweep
        <<< "1. Linear sweep (0.0 to 1.0)" >>>;
        for (0.0 => float t; t <= 1.0; t + 0.01 => t) {
            au.setParam(0, t);
            20::ms => now;
        }

        // Example 2: Sine wave modulation
        <<< "2. Sine wave modulation" >>>;
        for (0.0 => float t; t < 4.0; t + 0.05 => t) {
            Math.sin(t * pi) * 0.5 + 0.5 => float value;
            au.setParam(0, value);
            50::ms => now;
        }

        // Example 3: Triangle wave
        <<< "3. Triangle wave modulation" >>>;
        for (0 => int cycle; cycle < 4; cycle++) {
            // Ramp up
            for (0.0 => float t; t <= 1.0; t + 0.02 => t) {
                au.setParam(0, t);
                25::ms => now;
            }
            // Ramp down
            for (1.0 => float t; t >= 0.0; t - 0.02 => t) {
                au.setParam(0, t);
                25::ms => now;
            }
        }

        // Example 4: Random jumps
        <<< "4. Random parameter jumps" >>>;
        for (0 => int i; i < 20; i++) {
            Math.random2f(0.0, 1.0) => float value;
            au.setParam(0, value);
            <<< "  Random value:", value >>>;
            200::ms => now;
        }

    } else {
        <<< "No parameters available for modulation" >>>;
    }

    au.close();
} else {
    <<< "Failed to load AULowpass" >>>;
}


<<< "=== Example complete ===" >>>;