// Chaining Multiple AudioUnits
// Demonstrates using multiple AudioUnit effects in series

@import "AudioUnit";

SinOsc osc => AudioUnit delay => AudioUnit reverb => dac;

// Generate a test signal
440 => osc.freq;
0.3 => osc.gain;

<<< "=== Multiple AudioUnits Example ===" >>>;


// Load delay
if (delay.load("AUDelay")) {
    <<< "Loaded AUDelay into first slot" >>>;

    // Set delay parameters
    if (delay.paramCount() > 0) {
        delay.setParam(0, 0.3);  // Delay time
    }
    if (delay.paramCount() > 1) {
        delay.setParam(1, 0.4);  // Feedback
    }
} else {
    <<< "Failed to load delay" >>>;
}

// Load reverb
if (reverb.load("AUReverb")) {
    <<< "Loaded AUReverb into second slot" >>>;

    // Set reverb parameters if available
    if (reverb.paramCount() > 0) {
        reverb.setParam(0, 0.5);  // Room size or mix
    }
} else {
    <<< "Failed to load reverb" >>>;
}


<<< "Signal chain: SinOsc => Delay => Reverb => dac" >>>;


// Play a melody through the effect chain
<<< "Playing melody through effect chain..." >>>;
[60, 64, 67, 72, 67, 64] @=> int melody[];

for (0 => int i; i < melody.length; i++) {
    Std.mtof(melody[i]) => osc.freq;
    <<< "Note:", melody[i] >>>;
    500::ms => now;
}

// Silence and let effects tail decay
0 => osc.gain;

<<< "Letting effects decay..." >>>;
2::second => now;

// Clean up
delay.close();
reverb.close();


<<< "=== Example complete ===" >>>;