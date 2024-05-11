/*
plu3-max.ck is adapted from the ChucK example file: /deep/plu3.ck 
The core systhesis generation and functions are unchanged. 
Added:
    - global variables for use in Max
    - reverb added to signal chain
    - code to support running locally vs in Max
    - while loop that supports input from the Max sequencer object

The changes enable plu3 to be a synthesis instrument responding the Midi note number output from the Max live.step sequencer. 

5/2024 HighHarmonics

original attribution for plu3.ck
// tuned plucked string filter
// Ge Wang (gewang@cs.princeton.edu)
*/

// feedforward
Noise imp => OneZero lowpass => PoleZero allpass => NRev rev => dac;
// feedback
allpass => Delay delay => lowpass;

//globals for parameters in Max
global int noteNum; //sets MIDI note number output from sequencer object (live.step)
global float beatMs; //sets the sequencer beat tempo
global int seqOn; //used in Max to turn on sequencer
global float revMix;

0 => int localOn; //for running outside of Max

//initialize
Math.random2(20,100) => noteNum;
1000 => beatMs;
if (localOn == 1) 1 => seqOn; //turn on sequence if running locally
0 => imp.gain;
0 => rev.mix;

// our radius
.99999 => float R;
-1 => lowpass.zero; // place zero

while(true)
{
    Std.mtof( noteNum ) => setFreq => float L; // get (integer) delay order
    L::samp => delay.delay; // set delay
    Math.pow( R, L ) => delay.gain;// set dissipation factor
    if (seqOn == 1)
    {
        revMix => rev.mix;
        1 => imp.gain; // fire excitation
        L::samp => now; // for one delay round trip
        0 => imp.gain; // done
    }
    beatMs::ms => now;
}

// set (fundamental) freq
fun int setFreq( float freq )
{
    // sample rate
    second / samp => float SR;
    // omega
    2 * pi * freq / SR => float omega;
    // figure total delay needed
    SR / freq - .5 => float D;
    // the integer part
    D $ int => int Di;
    // the fraction
    D - Di => float Df;
    // set allpass using fractional and fundamental
    polePos( Df, omega ) => allpass.allpass;

    // return integer portion
    return Di;
}

// find pole location from delay and omega
fun float polePos( float D, float omega )
{
    // here it is (a la Jaffe & Smith)
    return Math.sin( (1-D) * omega / 2 ) / 
           Math.sin( (1+D) * omega / 2 );
}
