// Switching AudioUnits Dynamically
// Demonstrates loading different AudioUnits during runtime

@import "AudioUnit";

SinOsc osc => AudioUnit au => dac;

// Test signal
440 => osc.freq;
0.3 => osc.gain;

<<< "=== Dynamic AudioUnit Switching Example ===" >>>;


// List of effects to try
["AUDelay", "AULowpass", "AUDistortion", "AUReverb"] @=> string effects[];

for (0 => int i; i < effects.length; i++) {
    <<< "Switching to:", effects[i] >>>;

    if (au.load(effects[i])) {
        <<< "  Successfully loaded", effects[i] >>>;
        <<< "  Parameters:", au.paramCount() >>>;

        // List first 3 parameters
        for (0 => int p; p < au.paramCount() && p < 3; p++) {
            <<< "    [" + p + "]", au.paramName(p) >>>;
        }

        // Play through effect
        <<< "  Playing..." >>>;
        2::second => now;

        // Close current effect
        au.close();
    } else {
        <<< "  Failed to load", effects[i] >>>;
        <<< "  Skipping..." >>>;
    }


}

<<< "=== Example complete ===" >>>;