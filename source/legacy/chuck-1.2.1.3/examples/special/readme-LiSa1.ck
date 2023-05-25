//-----------------------------------------------------------------------------
// name: readme-LiSa1.ck
// desc: Live sampling utilities for ChucK
//
// author: Dan Trueman, 2007
//
// the LiSa ugens allow realtime recording of audio to a buffer for various
// kinds of manipulation. 
//
// Below is a simple example demonstrating the basic functionality of
// LiSa.
//
// See readme-LiSa2.ck for a command summary and instructions for doing
// multiple voice playback.
//-----------------------------------------------------------------------------

// signal chain; record a sine wave, play it back
SinOsc s => LiSa saveme => dac;
s => dac; // monitor the input
440. => s.freq;
0.2 => s.gain;

// alloc memory; required
60::second => saveme.duration;

// set playback rate
1.5 => saveme.rate;

// start recording input
saveme.record( 1 );

// 1 sec later, start playing what was just recorded
1000::ms => now;
saveme.rampUp( 100::ms );
// use saveme.play( 1 ) to start playing without ramp

// hang for a bit
1000::ms => now;

// rampdown
saveme.rampDown( 300::ms );
// use saveme.play( 0 ) to stop playing without ramp

500::ms => now;

// bye bye
