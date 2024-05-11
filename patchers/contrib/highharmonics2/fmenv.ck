/* 
This presents a basic FM synthesis where the modulator is given an envelope function. The modulator values of frequency (mod.freq) and gain (mod.gain) are set using multipliers:
  carrier freq * harmonicity = modulator freq
  modulator freq * modIndex = modulator gain

Note that all variable values to be dynamically set in Max are assigned in the while(true) loop. This is needed so that changes sent from Max get evaluated. The reverb mix value is set outside the while loop to demonstrate the different treatment. This value will only change when the program is started or a new shred is created.

HighHarmonics 4/24
*/

// modulator with Envelope
SinOsc mod => Envelope env => SinOsc car => Dyno dy => JCRev r => dac;
2 => car.sync; // needed for fm synth 

//Dyno settings
dy.attackTime(1::ms); // sets time for limiter to kick in
dy.thresh(0.8); // sets limiter threshold

//global variables (needed for Max/MSP)
global float carFreq;
global float carGain;
global float harmonicity;
global float modIndex;
global int   envDurMs;
global int   gapTimeMs;
global float revMix;
dur T;

// reverb mix is not dynamic - set from Max only on "run"
r.mix(revMix);

// time-loop
while( true ) 
{ 
    //values set from Max, reset after loop finishes
    car.freq(carFreq);
    car.gain(carGain);
    car.freq() * harmonicity => mod.freq;
    mod.freq() * modIndex => mod.gain;
    envDurMs::ms => T;
    T => env.duration;

	env.keyOn();
	T => now;
	env.keyOff();
	T => now;
    gapTimeMs::ms => now; //sets a break before the next envelope
}