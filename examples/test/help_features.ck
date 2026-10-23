// help_features.ck
// companion to help/chuck~.maxhelp and patchers/tests/test_chuck_tilde.maxpat
//
// declares the globals that the help patcher reads, taps and listens to.
// the globals are wired to audible parameters so that sending 'set' messages
// from Max produces an obvious change rather than only updating a number.

// --- globals readable via the 'get' message -------------------------------

global int   counter;       // get int counter
global float freq;          // get float freq       -> mono oscillator pitch
global string label;        // get string label
global int    steps[8];     // get int[] steps
global float  levels[4];    // get float[] levels   -> levels[0] is mono gain

220.0 => freq;
0 => counter;
"chuck-max" => label;

for (0 => int i; i < steps.size(); i++) {
    i * 2 => steps[i];
}
// levels[0] drives the mono gain, so it must start audible. assigned per
// element rather than with @=> so the array object the globals manager
// registered is the one that keeps being used
for (0 => int i; i < levels.size(); i++) {
    0.2 + (i * 0.2) => levels[i];
}

// --- event signalled from chuck, heard via the 'listen' message ------------

global Event tick;

// --- mono global UGen: 'tap 1 mono' ---------------------------------------
// a saw rather than a sine: the harmonics make pitch and level changes far
// easier to hear, and give the tap scope a shape you can actually read

global SawOsc mono;
freq => mono.freq;
levels[0] => mono.gain;
1 => mono.buffered;         // required for sample tapping

// --- stereo global UGen: 'tap 2 stereo 2' ---------------------------------
// Pan2 has two channels, so it can fill a pair of adjacent tap outlets.
// each sub-channel needs its own buffered flag; the parent flag does not
// propagate to the channels that getGlobalUGenSamplesMulti() reads

global Pan2 stereo;
TriOsc src => stereo;       // a different timbre, to tell the two apart
330.0 => src.freq;
0.25 => src.gain;
1 => stereo.chan(0).buffered;
1 => stereo.chan(1).buffered;

// --- audible output -------------------------------------------------------

mono => dac;
stereo => dac;

// --- apply Max-side changes to the audio ----------------------------------
// polls the globals so that 'set float freq 660.', the 'freq 550.' shorthand
// and 'set float[i] levels 0 0.75' are all immediately audible

spork ~ apply();
fun void apply() {
    while (true) {
        freq => mono.freq;
        levels[0] => mono.gain;
        10::ms => now;
    }
}

// --- sweep the stereo pan so the two tap channels differ audibly -----------

spork ~ sweep();
fun void sweep() {
    while (true) {
        for (0 => int i; i < 100; i++) {
            (i / 50.0) - 1.0 => stereo.pan;
            10::ms => now;
        }
        for (100 => int i; i > 0; i--) {
            (i / 50.0) - 1.0 => stereo.pan;
            10::ms => now;
        }
    }
}

// --- drive the counter and fire the event ---------------------------------

while (true) {
    counter + 1 => counter;
    tick.broadcast();
    500::ms => now;
}
