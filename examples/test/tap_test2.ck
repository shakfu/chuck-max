// tap_test2.ck
// Demonstrates tapping multiple global UGens
// Two global UGens can be tapped from Max using the tap message

// Declare two global UGens that can be tapped
global SinOsc osc1;      // smooth sine wave
global SqrOsc osc2;      // square wave (visually distinct)

// Set up oscillator 1 (sine wave - low frequency for clear scope display)
2 => osc1.freq;          // 2 Hz - slow, smooth sine
0.5 => osc1.gain;
1 => osc1.buffered;      // Enable buffering for external tapping

// Set up oscillator 2 (square wave - different frequency)
5 => osc2.freq;          // 5 Hz - faster square wave
0.5 => osc2.gain;
1 => osc2.buffered;      // Enable buffering for external tapping

// Mix both to dac for normal audio output
osc1 => dac;
osc2 => dac;

// Keep running
while (true) {
    1::second => now;
}
