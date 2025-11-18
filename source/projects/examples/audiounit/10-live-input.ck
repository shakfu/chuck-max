// Live Input Processing
// Demonstrates processing live audio input through AudioUnits

@import "AudioUnit";

// Connect ADC (audio input) through multiple effects
adc => AudioUnit compressor => AudioUnit eq => AudioUnit reverb => dac;

<<< "=== Live Input Processing Example ===" >>>;

<<< "Make sure your audio input (microphone/line-in) is connected!" >>>;


// Load compressor
if (compressor.load("AUDynamicsProcessor")) {
    <<< "Loaded AUDynamicsProcessor (compressor)" >>>;
} else {
    <<< "Failed to load compressor, bypassing..." >>>;
}

// Load EQ (try different names)
if (eq.load("AUNBandEQ")) {
    <<< "Loaded AUNBandEQ (equalizer)" >>>;
} else if (eq.load("AUPeakLimiter")) {
    <<< "Loaded AUPeakLimiter as alternative" >>>;
} else {
    <<< "Failed to load EQ, bypassing..." >>>;
}

// Load reverb
if (reverb.load("AUReverb")) {
    <<< "Loaded AUReverb" >>>;
} else {
    <<< "Failed to load reverb, bypassing..." >>>;
}


<<< "Processing live input through effect chain:" >>>;
<<< "  ADC => Compressor => EQ => Reverb => DAC" >>>;

<<< "Duration: 15 seconds" >>>;
<<< "Speak, sing, or play into your microphone!" >>>;


// Process live audio
15::second => now;


<<< "Stopping..." >>>;

// Clean up
compressor.close();
eq.close();
reverb.close();


<<< "=== Example complete ===" >>>;