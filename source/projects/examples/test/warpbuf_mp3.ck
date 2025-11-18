global float xbpm;

fun void heartbeat() {
    while(true) {
        <<<"heartbeat">>>;
        500::ms => now;
    }
}
spork~ heartbeat();

WarpBuf s1 => dac;


// me.dir() + "assets/nylon2.flac" => s1.read; // flac works
me.dir() + "../data/nylon2.mp3" => s1.read;

s1.gain(.4);

float BPM;

fun void setBPM(float newBPM) {
    newBPM => BPM => s1.bpm;
}

setBPM(140.);


// optionally "pause" either buffer:
// 0 => s1.play;
// 0 => s2.play;

// Test that loop off works.
// You should only hear the drum loop once
// and the guitar should loop.
// 0 => s2.loop;

// Test that startMarker works.
// You should two beats of the drum
// and the guitar should loop.
// 2 => s2.startMarker => s2.playhead;
// 0 => s2.loop;

// Test that endMarker works.
// You should two beats of the drum
// and the guitar should loop.
// 0 => s2.loop;
// 2 => s2.endMarker;

// Test that extending the loopEnd of the drum works.
// The drum is 1 bars (4 beats).
// So if we make the loopEnd 8, we should hear 4 beats
// of the drum, 4 of silence, then repeat.
// 8 => s2.loopEnd;

// Test that extending the loopStart of the drum works.
// The drum is 1 bars (4 beats).
// So if we make the loopStart -4, we should hear 4 beats
// of the drum, 4 of silence, then repeat.
// -4 => s2.loopStart;

// Test that there is silence when the loop end is less
// than the loop start.
// The guitar should be silent always.
// -4 => s1.loopEnd;
// 0 => s1.loopStart;

// Test that ridiculous bounds don't break it.
// The guitar should be silent always.
// 0. => s1.loopStart => s1.startMarker;
// 0. => s1.loopEnd;
// 0. => s1.playhead;

// Test that ridiculous bounds don't break it.
// The guitar should be silent always.
// 1000. => s1.loopStart => s1.startMarker;
// 1000. => s1.loopEnd;
// 1000. => s1.playhead;

// Test that ridiculous bounds don't break it.
// The guitar should be silent always.
// 1000. => s1.loopStart;
// 999. => s1.loopEnd;
// 998. => s1.playhead;

// Test that ridiculous bounds don't break it.
// The guitar should be silent always.
// -2. => s1.loopStart;
// -1. => s1.loopEnd;
// 3. => s1.playhead;

// Test that ridiculous bounds don't break it.
// The guitar should be silent always.
// -2. => s1.loopStart;
// -1. => s1.loopEnd;
// -3. => s1.playhead;

while(true) {
    setBPM(xbpm+20.*Math.random2(0,4));
    (240./BPM)::second => now;
    Std.rand2(-6, 0) => s1.transpose;
}