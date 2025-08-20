@import "HashMap"

HashMap hm;

/*

Suika tracks (11)
- 1 VCV:
- 11 chuck: comb filtered Ge video

drone bass
303 bass 

Polyrhythm
- 16 beat pattern x 4 = 64 16th notes
- but 64 is a power of two lol so no non-power-of-2 factors
- just use an odd number like 3, 5, 7, 9, 11, ...

Topology Henri
- bass
- snare
- kick

extra:
- tom
- clap
- vocal sample

dub techno chords

*/

fun int knob(string label, UI_Float v, float min, float max) {
    return  UI.knob(
            label, v, min, max,
            0, // speed
            "%.15f",
            UI_KnobVariant.Wiper, // UI_KnobVariant.Stepped,
            0, // size
            UI_KnobFlags.DragVertical, // UI_KnobFlags
            1000, // steps
            -1, -1); // angles
}

class Solar42Drone extends Chugraph {
    SawOsc saws[5];
    for (auto saw : saws) saw => outlet;

    fun void midi(float m) {
        Std.mtof(m) => float hz;

        Std.mtof(m - 12) => saws[0].freq;  // sub
        Std.mtof(m) => saws[1].freq;       // 
        Std.mtof(m + 7) => saws[2].freq;   // P5
        Std.mtof(m + 12) => saws[3].freq;  // oct
        Std.mtof(m + 16) => saws[3].freq;  // M3
    }

}

class Sequencer {

    // fun void play

}

class Lead303 extends Chugraph {
    PulseOsc osc => LPF lpf => ADSR e => outlet;
    e.set( 5::ms, 1::ms, 1.0, 1::ms );
}

/*
ControlPatch p(Object, "method");
p.apply(float) // internally applies. lets one create a generate patch via string
// remember to reference count Object
*/

Lead303 lead => dac;
UI_Float pulse_width(lead.osc.width());
fun void play() {
    127 => float BPM;
    (60.0 / BPM)::second => dur qt_note;
    qt_note / 4 => dur sixteenth_note;

    lead.gain(.1);
    [0, 0, 3, 3, 0, 0, 0, 5] @=> int seq[];

    while (true) {
        for (auto m : seq) {
            Std.mtof(36 + m) => lead.osc.freq;
            lead.e.keyOn();
            sixteenth_note / 2 => now; 
            lead.e.keyOff();
            sixteenth_note / 2 => now; 
        }
    }
} spork ~ play();



// Plateau
Solar42Drone drone => LPF lpf => Plateau rev => dac;
// Solar42Drone drone => LPF lpf => Plateau rev => blackhole;
drone.gain(.1);
drone.midi(48);
// <<< rev.wet(), rev.decay(), rev.size(), rev.delay(), rev.diffuse() >>>;
Std.mtof(60) => lpf.freq;

class EnergyWidget
{
    Energy energy;
    UI_Float gain(1.0);

    UI_Float scope_scale(.1);
    UI_Bool scope_freeze(false);

    UI_Float feedback_m;
    UI_Float feedback_c;
    // TODO knob turning is jumpy/discrete. for smooth need to add interpolation (VCV implicitly does this)
    // possible locations (needs to be called every 64 samps, much faster than frame rate)
    // 1. in energy chugin, slew (BAD)
    // 2. in ulib imgui knob, interpolate -- too slow, frame rate
    // 3. in UI_Float, option to interpolate? -- lets not introduce complexity here
    // 4. slew limit ugen that's part of the audio graph, so updated at audio rate
    //      - ui knob updates target of slew limiter (can this just be a 1 pole filter? maybe see chuck env_follower example)
    //      - or add Geodesics asymetric slew limiter as UGen
    UI_Float midi((energy.midi()));
    UI_Float m_midi((energy.midiM()));
    UI_Float c_midi((energy.midiC()));

    // visualizer window size
    1024=> int WINDOW_SIZE;
    // audio analysis
    energy => Flip accum => blackhole;
    WINDOW_SIZE => accum.size;
    float samples[0];
    // fft
    energy => PoleZero dcbloke => FFT fft => blackhole;
    .95 => dcbloke.blockZero;
    Windowing.hann(WINDOW_SIZE) => fft.window;
    WINDOW_SIZE*2 => fft.size;
    complex response[0];
    float spectrum_hist_positions[WINDOW_SIZE];

    fun @construct() {
        spork ~ audioAnalyzerShred();
    }

    fun void audioAnalyzerShred() {
        while( true ) {
            if (!scope_freeze.val()) {
                accum.upchuck();
                fft.upchuck();
            }
            // update at rate proportional to waveform freq to show standing wave
            (4.0 / Std.mtof(energy.midi()))::second => now;
        }
    }


    fun void ui() {
        if(UI.begin("Energy")) {
            // draw waveform
            accum.output( samples ); // ==optimize== track in UI chugin directly, don't need to pass array memory from cpp --> chuck --> cpp
            UI.plotLines("##Waveform", samples, 0, "", -scope_scale.val(), scope_scale.val(), @(-1,100));
            UI.slider("Scope Scale##energy", scope_scale, 0, 1);
            UI.sameLine();
            UI.checkbox("Freeze", scope_freeze);

            // draw spectrum ==optimize== do this in ulib_imgui chugin
            fft.spectrum( response );
            // map FFT output to 3D positions
            for(int i; i < response.size(); i++)
                5 * Math.sqrt( (response[i]$polar).mag * 25 ) => spectrum_hist_positions[i];
            UI.plotHistogram("##Spectrum", spectrum_hist_positions, 0, "", 0, 2, @(-1, 100));
            
            if (knob("Feedback M", feedback_m, 0, 1)) feedback_m.val() => energy.feedbackM;
            UI.sameLine();
            if (knob("Feedback C", feedback_c, 0, 1)) feedback_c.val() => energy.feedbackC;

            if (knob("M", m_midi, -36, 36)) (m_midi.val()) => energy.midiM;
            UI.sameLine();
            if (knob("Freq", midi, 0, 120)) (midi.val()) => energy.midi;
            UI.sameLine();
            if (knob("C", c_midi, -36, 36)) (c_midi.val()) => energy.midiC;

        }
        UI.end();
    }
}

EnergyWidget energy_widget;
energy_widget.energy => Gain energy_gain => dac;
.1 => energy_widget.energy.gain;
0 => energy_gain.gain;

// visualizer window size
512 => int WINDOW_SIZE;
// audio analysis
rev => Flip accum => blackhole;
rev => PoleZero dcbloke => FFT fft => blackhole;
.95 => dcbloke.blockZero;
WINDOW_SIZE => accum.size;
Windowing.hann(WINDOW_SIZE) => fft.window;
WINDOW_SIZE*2 => fft.size;
Windowing.hann(WINDOW_SIZE) @=> float window[];
float samples[0];
complex response[0];
float spectrum_hist_positions[WINDOW_SIZE];

// map audio buffer to 3D positions
fun void map2waveform(float in[])
{
    for( int i; i < in.size(); i++ )
        window[i] *=> in[i];
}

// map FFT output to 3D positions
fun void map2spectrum( complex in[], float out[] )
{
    for(int i; i < in.size(); i++)
        5 * Math.sqrt( (in[i]$polar).mag * 25 ) => out[i];
}


fun void audioAnalyzerShred()
{
    while( true ) {
        accum.upchuck();
        accum.output( samples );
        fft.upchuck();
        fft.spectrum( response );
        WINDOW_SIZE::samp/2 => now;
    }
} spork ~ audioAnalyzerShred();



UI_Float lpf_cutoff(lpf.freq());
UI_Float lpf_gain(lpf.gain());

// Plateau params
UI_Float plateau_decay(rev.decay());
UI_Float plateau_size_smoothing(rev.sizeSmoothing());
UI_Float plateau_size(rev.size());
UI_Float plateau_diffusion(rev.diffusion());
UI_Float plateau_input_lowcut(rev.inputLow());
UI_Float plateau_input_highcut(rev.inputHigh());
UI_Float plateau_reverb_lowcut(rev.reverbLow());
UI_Float plateau_reverb_highcut(rev.reverbHigh());
UI_Bool plateau_tuned(rev.tuned());
UI_Float plateau_mod_depth(rev.modDepth());
UI_Float plateau_mod_speed(rev.modSpeed());
UI_Float plateau_pre_delay(rev.delay());

// UI_Float plinky_fade(rev.fade());
// UI_Float plinky_shim(rev.shim());
// UI_Float plinky_wobble(rev.wobble());
// UI_Float plinky_send(rev.send());
// UI_Float plinky_mix(rev.mix());


while (true) {
    GG.nextFrame() => now;

    UI.begin("");

    if (knob( "LPF Cutoff", lpf_cutoff, 40, 20000))
        lpf_cutoff.val() => lpf.freq;

    UI.sameLine();
    if (knob( "LPF Gain", lpf_gain, 0, 1))
        lpf_gain.val() => lpf.gain;

    UI.sameLine();
    if (knob( "size", plateau_size, .0, 1)) plateau_size.val() => rev.size;
    UI.sameLine();
    if (knob( "size_smth", plateau_size_smoothing, .0, 1)) plateau_size_smoothing.val() => rev.sizeSmoothing;
    UI.sameLine();
    if (knob( "diffusion", plateau_diffusion, 0, 1)) plateau_diffusion.val() => rev.diffusion;
    UI.sameLine();
    if (knob( "decay", plateau_decay, 0, 1))plateau_decay.val() => rev.decay;
    UI.sameLine();
    if (knob( "input low", plateau_input_lowcut, 0, 1)) plateau_input_lowcut.val() => rev.inputLow;
    UI.sameLine();
    if (knob( "input high", plateau_input_highcut, 0, 1)) plateau_input_highcut.val() => rev.inputHigh;
    UI.sameLine();
    if (knob( "rev low", plateau_reverb_lowcut, 0, 1)) plateau_reverb_lowcut.val() => rev.reverbLow;
    UI.sameLine();
    if (knob( "rev high", plateau_reverb_highcut, 0, 1)) plateau_reverb_lowcut.val() => rev.reverbHigh;
    UI.sameLine();
    if (UI.button("clear")) rev.clear();
    UI.sameLine();
    if (UI.checkbox("tune", plateau_tuned)) plateau_tuned.val() => rev.tuned;
    UI.sameLine();
    if (knob( "mod depth", plateau_mod_depth, 0, 1)) plateau_mod_depth.val() => rev.modDepth;
    UI.sameLine();
    if (knob( "mod speed", plateau_mod_speed, 0, 1)) plateau_mod_speed.val() => rev.modSpeed;
    UI.sameLine();
    rev.delay() => plateau_pre_delay.val;
    if (knob( "pre delay", plateau_pre_delay, 0, 1)) plateau_pre_delay.val() => rev.delay;

    // UI.sameLine();
    // if (knob("fade", plinky_fade, 0.0, 1.0)) plinky_fade.val() => rev.fade;
    // UI.sameLine();
    // if (knob("shim", plinky_shim, 0.0, 1.0)) plinky_shim.val() => rev.shim;
    // UI.sameLine();
    // if (knob("wobble", plinky_wobble, 0.0, 1.0)) plinky_wobble.val() => rev.wobble;
    // UI.sameLine();
    // if (knob("send", plinky_send, 0.0, 1.0)) plinky_send.val() => rev.send;
    // UI.sameLine();
    // if (knob("mix", plinky_mix, 0.0, 1.0)) plinky_mix.val() => rev.mix;

    // plot waveform as line
    UI.getContentRegionAvail() => vec2 right_size;
    map2waveform( samples );
    UI.plotLines("##Waveform", samples, 0, "512 samples", -1, 1, @(right_size.x * .3, 200));

    UI.dummy(@(0.0f, 20.0f)); // vertical spacing

    // plot spectrum as histogram
    map2spectrum( response, spectrum_hist_positions );
    UI.plotHistogram("##Spectrum", spectrum_hist_positions, 0, "512 bins", 0, 8, @(right_size.x * .3, 200));

    if (knob("Pulse width", pulse_width, .1, .9)) pulse_width.val() => lead.osc.width;

    energy_widget.ui();

    UI.end();
}
