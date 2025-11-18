// Basic AudioUnit Effect Example
// Demonstrates loading and using an AudioUnit as an audio effect

@import "AudioUnit";

// Create an AudioUnit instance and connect it in the audio chain
adc => AudioUnit au => dac;

<<< "=== Basic AudioUnit Effect Example ===" >>>;

// [0] Dry/Wet Mix: 50.000000 
// [1] Delay Time: 1.000000 
// [2] Feedback: 50.000000 
// [3] Lowpass Cutoff Frequency: 15000.000000 

// Load Apple's AUDelay by name
if (au.load("AUDelay")) {
    // au.setParam(1, 2.0); // delay time
    // au.setParam(2, 50.0); // feedback
    <<< "Successfully loaded AUDelay!" >>>;
    <<< "Processing audio from ADC through AUDelay for 5 seconds..." >>>;

    // Process audio for 5 seconds
    5::second => now;

    <<< "Closing AudioUnit..." >>>;
    au.close();
} else {
    <<< "Failed to load AUDelay" >>>;
    <<< "Make sure you're running on macOS with AudioUnits available" >>>;
}


<<< "=== Example complete ===" >>>;