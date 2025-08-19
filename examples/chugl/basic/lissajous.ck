//-----------------------------------------------------------------------------
// name: lissajous.ck
// desc: Lissajous visualizer. Oscilloscope music player.
//
// To play oscilloscope music, download the audio file at:
// https://ccrma.stanford.edu/~azaday/music/khrang.wav
// and place in the same directory as this file.
// Then press <space> or the "Play oscilloscope music" button.
//
// author: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//   date: Fall 2024
//-----------------------------------------------------------------------------

// set up bloom
GG.renderPass() --> BloomPass bloom_pass --> GG.outputPass();
bloom_pass.input(GG.renderPass().colorOutput());
GG.outputPass().input(bloom_pass.colorOutput());
bloom_pass.intensity(1.0);

// set up camera
GG.camera().orthographic();
GG.camera().posZ(30.0);

// unit analyzer setup
1024 => int WINDOW_SIZE;

dac.chan(0) => Flip accum_r => blackhole;
dac.chan(1) => Flip accum_l => blackhole;

WINDOW_SIZE => accum_l.size;
WINDOW_SIZE => accum_r.size;

float left_waveform[WINDOW_SIZE];
float right_waveform[WINDOW_SIZE];

fun void audio()
{
    while (true) {
        WINDOW_SIZE::samp => now;

        accum_l.upchuck();
        accum_l.output(left_waveform);

        accum_r.upchuck();
        accum_r.output(right_waveform);
    }
} spork ~ audio();

// ugen setup
SinOsc left_osc => dac.chan(0);
SinOsc right_osc => dac.chan(1);
.1 => left_osc.gain => right_osc.gain;

SndBuf2 buf => blackhole;
me.dir() + "./khrang.wav" => buf.read;
0 => buf.rate;
.5 => buf.gain;

// scenegraph setup
GLines lissajous --> GG.scene();
lissajous.width(.05);
lissajous.color(@(1./32., 1.0, 1./32.));
vec2 positions[WINDOW_SIZE];

UI_Float freq_l(left_osc.freq());
UI_Float freq_r(right_osc.freq());
while (true) {
    GG.nextFrame() => now;

    // update lines with audio data
    for (int i; i < WINDOW_SIZE; i++) {
        10 * @(left_waveform[i], right_waveform[i]) => positions[i];
        lissajous.positions(positions);
    }

    // UI
    if (UI.begin("Lissajous")) {
        if (UI.drag("Left Frequency", freq_l)) {
            left_osc.freq(freq_l.val());
        }

        if (UI.drag("Right Frequency", freq_r)) {
            right_osc.freq(freq_r.val());
        }

        if (UI.button("Sync phase")) {
            left_osc.phase(0);
            right_osc.phase(0);
        }

        if (UI.button("Play oscilloscope music") || GWindow.keyDown(GWindow.Key_Space)) {
            // disconnect the oscillators
            left_osc =< dac.chan(0);
            right_osc =< dac.chan(1);

            // connect and start sndbuf
            if (buf.pos() == 0) {
                GG.camera().viewSize(10.0);
                1.0 => buf.rate;
                buf => dac;
            }
        }
    }
    UI.end();
}