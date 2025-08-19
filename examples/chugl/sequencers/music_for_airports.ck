//-----------------------------------------------------------------------------
// name: music_for_airports.ck
// desc: Generative ambient music, based on track 2/1 from Brian Eno's 
// "Music for Airports"
// 
// author: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//   date: Fall 2024
//-----------------------------------------------------------------------------

// Orthographic camera for a 2D scene
GG.scene().camera().orthographic();

NRev rev => dac;
.50 => rev.mix;

fun void addGraphics(Envelope@ env, float midi) {
    GPlane plane --> GG.scene();
    plane.color(Color.BLACK);

    plane.sca(.2);
    Math.remap(midi, 53, 68, -3.0, 3.0) => plane.posX;

    while (true) {
        GG.nextFrame() => now;
        2 * Color.WHITE * (.1 + -(Math.cos(Math.PI * env.value()) - 1) / 2) => plane.color;
        .1 + (2 * -(Math.cos(Math.PI * env.value()) - 1) / 2) => plane.scaY;
    }
}

fun void addVoice(dur offset, float midi, dur note_dur, dur loop_dur)
{
    FrencHrn tri => Envelope env => rev;
    Std.mtof(midi) => tri.freq;
    .01 => tri.gain;
    note_dur/2 => env.duration;

    spork ~ addGraphics(env, midi);

    offset => now;
    while (true) {
        tri.noteOn(1);
        env.keyOn();
        note_dur/2 => now;
        env.keyOff();
        note_dur/2 => now;
        tri.noteOff(1);
        (loop_dur - note_dur) => now;
    }
}

spork ~ addVoice(1::second + 0.0::second, 60, 7.7::second, 20.1::second); // C
spork ~ addVoice(1::second + 1.9::second, 63, 7.1::second, 16.2::second); // Eb
spork ~ addVoice(1::second + 6.5::second, 65, 8.5::second, 19.6::second); // F
spork ~ addVoice(1::second + 6.7::second, 53, 9.1::second, 24.7::second); // low F
spork ~ addVoice(1::second + 8.2::second, 68, 9.4::second, 17.8::second); // Ab
spork ~ addVoice(1::second + 9.6::second, 56, 7.9::second, 21.3::second); // low Ab
spork ~ addVoice(1::second + 15.0::second, 61, 9.2::second, 31.8::second); // Db

while (true) {
    GG.nextFrame() => now;
}