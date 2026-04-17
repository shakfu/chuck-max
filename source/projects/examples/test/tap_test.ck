// tap_test.ck
// Demonstrates the tap feature for reading global UGen samples
// The global UGen can be tapped from Max using the tap message

// Declare a global UGen that can be tapped
global SinOsc osc;

// Set up the oscillator
440 => osc.freq;
0.5 => osc.gain;

// Enable buffering so samples can be tapped externally
1 => osc.buffered;

// Connect to dac for normal audio output
osc => dac;

// Keep running
while (true) {
    1::second => now;
}
