// Basic AudioUnit Effect Example
// Demonstrates loading and using an AudioUnit as an audio effect

@import "AudioUnit";

// Create an AudioUnit instance and connect it in the audio chain
adc => AudioUnit au => dac;

<<< "=== Basic AudioUnit Effect Example ===" >>>;


// Load Apple's AUDelay by name
if (au.load("AUDelay")) {
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