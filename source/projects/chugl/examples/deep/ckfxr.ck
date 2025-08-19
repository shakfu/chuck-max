//-----------------------------------------------------------------------------
// name: ckfxr.ck
// desc: ChucK Sound FX synthesizer!
// 
// author: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//   date: Fall 2024
//-----------------------------------------------------------------------------

// CKFXR custom Chugraph UGen
CKFXR ckfxr => dac;
// for recording
ckfxr => WvOut wvout => blackhole;
// synth parameters
CKFXR_Params p;

// visualizer window size
512 => int WINDOW_SIZE;
// audio analysis
ckfxr => Flip accum => blackhole;
ckfxr => PoleZero dcbloke => FFT fft => blackhole;
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


// Delay-based effects including flange, chorus, doubling
// https://github.com/ccrma/music220a/blob/main/07-time-and-space/delay-based-efx/script.js
// https://ccrma.stanford.edu/~dattorro/EffectDesignPart2.pdf
class DelayFX extends Chugraph
{
    // audio signal
    inlet => DelayL delay => outlet;
    delay => Gain feedback => delay;

    delay.max(1::second); // max delay time

    // control signal
    SinOsc lfo_delay_mod => blackhole;
    0 => float delay_samps;

    // modulator
    spork ~ modulate();
    fun void modulate()
    {
        // remap from [-1, 1] to [0, 2*delay_samps]
        while (1::ms => now) {
            lfo_delay_mod.last() + 1 => float mod;
            (1 + mod * delay_samps)::samp => delay.delay;
        }
    }

    // default to flanger
    flange();  

    // params
    // float feedback_gain 
    // delay_base (center delay duration)
    // delay_mod_freq (lfo to modulate delay length)
    // delay_mod_depth (delay_base is modulated between [delay_base*(1 - depth), delay_base * (1 + depth)])

    // public API ---------------------------------------------
    fun void flange() {
        // typical range: 1~10ms
        delayBase(5::ms);
        feedbackGain(.85);
        delayModFreq(.1);
        delayModDepth(.80);  // 80% depth. oscillate between (1ms, 9ms)
    }

    fun void chorus() {
        // typical range: 5~30ms
        delayBase(12::ms);
        feedbackGain(.55);
        delayModFreq(.2);
        delayModDepth(.50);  // 50% depth. oscillate between (6ms, 18ms)
    }

    fun void double() {
        // typical range: 20~100ms
        delayBase(60::ms);
        feedbackGain(.45);
        delayModFreq(.1);
        delayModDepth(.2);  // 20% depth. oscillate between (48ms, 72ms)
    }

    // Setters
    fun void feedbackGain(float f) { f => this.feedback.gain; }
    fun void delayBase(dur d) {
        delay.delay(d);
        d / samp => delay_samps;
    }
    fun void delayModFreq(float f) { lfo_delay_mod.freq(f); }
    fun void delayModDepth(float f) { lfo_delay_mod.gain(f); }

    // Getters 
    fun float feedbackGain() { return feedback.gain(); }
    fun dur delayBase() { return delay.delay(); }
    fun float delayModFreq() { return lfo_delay_mod.freq(); }
    fun float delayModDepth() { return lfo_delay_mod.gain(); }
}

// custom CKFXR UGen
class CKFXR extends Chugraph
{
    LPF lpf => HPF hpf => DelayFX delayfx => ADSR adsr => outlet;
    PulseOsc square; SawOsc saw; SinOsc sin; Noise noise;
    [square, saw, sin, noise] @=> UGen waveforms[];

    // modulators
    SinOsc lfo_vibrato => blackhole;
    TriOsc lfo_pwm(0.0) => blackhole;

    // WaveType enum
    0 => static int WaveType_SQUARE;
    1 => static int WaveType_SAW;
    2 => static int WaveType_SIN;
    3 => static int WaveType_NOISE;

    // Synth params parameters are on [0,1] unless noted SIGNED & thus
    // on [-1,1]
    0 => int p_wave_type;

    // env
    dur p_attack_dur;
    dur p_release_dur; 
    dur p_sustain_dur;  // noteOn - noteOff
    float p_sustain_level;

    // freq (in midi for linear properties) 
    float p_freq_base_midi;    // Start frequency. doing midi from [12, 127] ~ [16.345hz, 12543.85hz]
    float p_freq_limit_midi;   // Min frequency cutoff, anything below is stopped 
    float p_freq_ramp;         // Slide in semitones / s (SIGNED)
    float p_freq_dramp;        // Delta slide semitones / s^2 (SIGNED)

    // Vibrato
    float p_vib_depth; // Vibrato depth [0, 1]
    float p_vib_freq;    // Vibrato speed

    // Tonal change
    float p_arp_mod_midi;      // Freq Change amount (SIGNED)
    dur p_arp_time;          // how long after attack to apply arp_mod

    // PWM
    float p_pwm_depth;         
    float p_pwm_freq;          

    // Flanger
    float p_feedback_gain; 
    dur p_delay_base_dur;
    float p_delay_mod_freq;
    float p_delay_mod_depth;

    // Low-pass filter
    float p_lpf_freq;      // Low-pass filter cutoff
    float p_lpf_ramp;      // Low-pass filter cutoff sweep (semitones / s)
    float p_lpf_resonance; // Low-pass filter resonance

    // High-pass filter
    float p_hpf_freq;     // High-pass filter cutoff
    float p_hpf_ramp;     // High-pass filter cutoff sweep (semitones / s)
    
    // initialize values
    resetParams();

    // pwm modulator
    spork ~ pwmModulate();
    fun void pwmModulate()
    {
        while (1::ms => now) {
            Math.remap(lfo_pwm.last(), -1, 1, .001, .999) => square.width;
        }
    }
    // public API
    fun void resetParams()
    {
        WaveType_SQUARE => p_wave_type;

        // env
        1::ms => p_attack_dur;
        1::ms => p_release_dur;
        1::second => p_sustain_dur;
        1.0 => p_sustain_level;

        // freq 
        60.0 => p_freq_base_midi;    
        0 => p_freq_limit_midi;
        0 => p_freq_ramp;    
        0 => p_freq_dramp;   

        // vibrato
        0 => p_vib_depth;
        0 => p_vib_freq; 

        // arp
        0 => p_arp_mod_midi;      
        0::ms => p_arp_time;      

        // pwm 
        0 => p_pwm_depth;         
        0 => p_pwm_freq;          

        // delay 
        0 => p_feedback_gain; 
        0::ms => p_delay_base_dur;
        0 => p_delay_mod_freq;
        0 => p_delay_mod_depth;

        // lpf
        20000 => p_lpf_freq;    
        0 => p_lpf_ramp;      
        1 => p_lpf_resonance; 

        // hpf
        20 => p_hpf_freq;    
        0 => p_hpf_ramp;    
    }

    fun void play()
    {   
        _play(++_play_count);
    }

    // Internal -----------------------------------------------------------------
    // used to prevent overlapping play calls so that the synth can be 
    // retriggered before a previous shred ends. 
    // DO NOT MODIFY _play_count directly. Use play() instead.
    0 => int _play_count; 

    // map user params to synthesis 
    fun void _assign()
    {
        // reset lfo phase
        0 => lfo_vibrato.phase;
        0 => lfo_pwm.phase;

        // waveform
        for (auto ugen : waveforms) ugen =< lpf;
        waveforms[p_wave_type] => lpf;

        // envelope
        adsr.set(p_attack_dur, 1::ms, p_sustain_level, p_release_dur);

        // frequency
        _setFreq(p_freq_base_midi);

        // bibrato
        lfo_vibrato.freq(p_vib_freq);
        lfo_vibrato.gain(p_vib_depth);

        // duty
        lfo_pwm.freq(p_pwm_freq);
        lfo_pwm.gain(p_pwm_depth);

        // delay effect
        delayfx.feedbackGain(p_feedback_gain);
        delayfx.delayBase(p_delay_base_dur);
        delayfx.delayModFreq(p_delay_mod_freq);
        delayfx.delayModDepth(p_delay_mod_depth);

        // Low-pass filter
        lpf.freq(p_lpf_freq);
        lpf.Q(p_lpf_resonance);

        // High-pass filter
        hpf.freq(p_hpf_freq);
    }

    fun void _play(int pc)
    {
        if (pc != _play_count) return;

        _assign();

        spork ~ _playArp(pc);
        spork ~_playMod(pc);

        adsr.keyOn();
        adsr.attackTime() + adsr.decayTime() + p_sustain_dur => now;

        if (pc != this._play_count) return;

        this.adsr.keyOff();
        this.adsr.releaseTime() => now;

        // wait for env to finish. if, after release, pc is the same, bump pc again. 
        // this means no other incoming play requests were made
        // and bumping pc again will stop any inifinite loop sporks
        if (pc == _play_count) _play_count++;
    }

    fun int _setFreq(float freq_midi)
    {
        // only set if it's above the cutoff
        if (freq_midi < p_freq_limit_midi) return false;

        Std.mtof(freq_midi) => float freq_hz;

        freq_hz => sin.freq;
        freq_hz => saw.freq;
        freq_hz => square.freq;
        return true;
    }

    fun void _playArp(int pc)
    {
        p_arp_time => now;
        if (pc != _play_count) return;

        // adjust frequency
        p_arp_mod_midi +=> p_freq_base_midi;
    }

    // frequency + vibrato mod + filter mod
    fun void _playMod(int pc)
    {
        now => time t;

        while (1::ms => now) {
            if (pc != _play_count) return;

            // dt
            (now - t) / second => float dt; // elapsed time in seconds
            now => t;

            // frequency modulation
            (dt * p_freq_ramp) +=> p_freq_base_midi;
            (dt * p_freq_dramp) +=> p_freq_ramp;

            // vibrato (remap from [-1, 1] to [1-depth, 1+depth] )
            (lfo_vibrato.last() + 1) * p_freq_base_midi => float freq_midi;

            // set frequency
            _setFreq(freq_midi);
            
            // lpf ramp
            Std.mtof((dt * p_lpf_ramp) + Std.ftom(lpf.freq())) => lpf.freq;

            // hpf ramp
            Std.mtof((dt * p_hpf_ramp) + Std.ftom(hpf.freq())) => hpf.freq;
        }
    }
}

class CKFXR_Params
{
    0 => int waveform;

    UI_Float attack_dur_ms;
    UI_Float release_dur_ms;
    UI_Float sustain_dur_ms;
    UI_Float sustain_level;

    UI_Float freq_base_midi;    
    UI_Float freq_limit_midi;   
    UI_Float freq_ramp;  // semitones / sec
    UI_Float freq_dramp; // semitones / sec

    UI_Float vib_depth;
    UI_Float vib_freq; // hz

    UI_Float arp_mod_midi; 
    UI_Float arp_time_ms;       

    UI_Float pwm_depth;         
    UI_Float pwm_freq;          

    UI_Float feedback_gain; 
    UI_Float delay_base_dur_ms;
    UI_Float delay_mod_freq;
    UI_Float delay_mod_depth;

    UI_Float lpf_freq;     
    UI_Float lpf_ramp;     
    UI_Float lpf_resonance;

    UI_Float hpf_freq;     
    UI_Float hpf_ramp;

    UI_Float main_gain(.5);
    UI_String export_wav_path("output.wav");

    reset();

    fun void copyToSynth(CKFXR@ ckfxr)
    {
        waveform => ckfxr.p_wave_type;

        attack_dur_ms.val()::ms => ckfxr.p_attack_dur;
        release_dur_ms.val()::ms => ckfxr.p_release_dur;
        sustain_dur_ms.val()::ms => ckfxr.p_sustain_dur;
        sustain_level.val() => ckfxr.p_sustain_level;

        freq_base_midi.val() => ckfxr.p_freq_base_midi;
        freq_limit_midi.val() => ckfxr.p_freq_limit_midi;
        freq_ramp.val() => ckfxr.p_freq_ramp;
        freq_dramp.val() => ckfxr.p_freq_dramp;

        vib_depth.val() => ckfxr.p_vib_depth;
        vib_freq.val() => ckfxr.p_vib_freq;

        arp_mod_midi.val() => ckfxr.p_arp_mod_midi;
        arp_time_ms.val()::ms => ckfxr.p_arp_time;

        pwm_depth.val() => ckfxr.p_pwm_depth;
        pwm_freq.val() => ckfxr.p_pwm_freq;

        feedback_gain.val() => ckfxr.p_feedback_gain;
        delay_base_dur_ms.val()::ms => ckfxr.p_delay_base_dur;
        delay_mod_freq.val() => ckfxr.p_delay_mod_freq;
        delay_mod_depth.val() => ckfxr.p_delay_mod_depth;

        lpf_freq.val() => ckfxr.p_lpf_freq;
        lpf_ramp.val() => ckfxr.p_lpf_ramp;
        lpf_resonance.val() => ckfxr.p_lpf_resonance;

        hpf_freq.val() => ckfxr.p_hpf_freq;
        hpf_ramp.val() => ckfxr.p_hpf_ramp;
    }

    fun void reset()
    {
        0 => waveform;

        1 => attack_dur_ms.val;
        1 => release_dur_ms.val;
        1000 => sustain_dur_ms.val;
        1 => sustain_level.val;

        60 => freq_base_midi.val;
        0 => freq_limit_midi.val;
        0 => freq_ramp.val;
        0 => freq_dramp.val;

        0 => vib_depth.val;
        0 => vib_freq.val;

        0 => arp_mod_midi.val;
        0 => arp_time_ms.val;

        0 => pwm_depth.val;
        0 => pwm_freq.val;

        0 => feedback_gain.val;
        0 => delay_base_dur_ms.val;
        0 => delay_mod_freq.val;
        0 => delay_mod_depth.val;

        20000 => lpf_freq.val;
        0 => lpf_ramp.val;
        1 => lpf_resonance.val;

        20 => hpf_freq.val;
        0 => hpf_ramp.val;
    }

    fun void pickupCoin(CKFXR@ ckfxr)
    {
        reset();

        if (rnd(1)) CKFXR.WaveType_SAW => waveform;
        else CKFXR.WaveType_SQUARE => waveform;

        Math.random2f(60, 84) => freq_base_midi.val;

        frnd(0.05, 0.15) * 1000 => sustain_dur_ms.val;
        frnd(0.15, 0.3) * 1000 => release_dur_ms.val;
        frnd(.6, .9) => sustain_level.val;

        frnd(sustain_dur_ms.val()*.6, sustain_dur_ms.val() * 1.2) => arp_time_ms.val;
        Math.random2f(2, 24) $ int => arp_mod_midi.val;
    }

    fun void shootLaser(CKFXR@ ckfxr)
    {
        reset();

        rnd(2) => waveform;
        
        frnd(-120, -12) => freq_ramp.val;
        frnd(-80, -12) => freq_dramp.val;
        frnd(60, 127) => freq_base_midi.val;

        frnd(0.05, 0.2) * 1000 => sustain_dur_ms.val;
        frnd(0.05, 0.3) * 1000 => release_dur_ms.val;
        frnd(.6, .9) => sustain_level.val;
    }

    fun void explosion(CKFXR@ ckfxr)
    {
        reset();

        CKFXR.WaveType_NOISE => waveform;

        frnd(0.01, 0.1) * 1000 => sustain_dur_ms.val;
        frnd(0.1, 0.7) * 1000 => release_dur_ms.val;

        if (rnd(1)) {
            frnd(5,65) => delay_base_dur_ms.val;
            frnd(0.1, 15) => delay_mod_freq.val;
            frnd(0.1, 0.5) => delay_mod_depth.val;
            frnd(0.1, 0.9) => feedback_gain.val;
        }

        frnd(2000, 10000) => lpf_freq.val;
        frnd(-48, 0) => lpf_ramp.val;
        frnd(.1, 4) => lpf_resonance.val;
    }

    fun void powerup(CKFXR@ ckfxr)
    {
        reset();
        if (rnd(1)) {
            CKFXR.WaveType_SAW => waveform;
        } else {
            CKFXR.WaveType_SQUARE => waveform;
        }

        frnd(50, 70) => freq_base_midi.val;

        if (rnd(1)) {
            frnd(10, 60) => freq_ramp.val;
        } else {
            frnd(2, 15) => freq_ramp.val;
            frnd(2, 15) => freq_dramp.val;
        }

        if (rnd(1)) {
            frnd(0.2) => vib_depth.val;
            frnd(7,20) => vib_freq.val;
        }

        frnd(100, 400) => sustain_dur_ms.val;
        frnd(100, 500) => release_dur_ms.val;
    }

    fun void hitHurt(CKFXR@ ckfxr)
    {
        reset();

        rnd(2) => waveform;
        if (waveform == CKFXR.WaveType_SIN) CKFXR.WaveType_NOISE => waveform;

        if (waveform == CKFXR.WaveType_SQUARE) {
            frnd(0.2) => pwm_depth.val;
            frnd(30) => pwm_freq.val;
        }

        frnd(20, 80) => freq_base_midi.val;
        frnd(-55, -5) => freq_ramp.val;

        frnd(10, 100) => sustain_dur_ms.val;
        frnd(100, 300) => release_dur_ms.val;

        if (rnd(1))
           frnd(30, 3000) => hpf_freq.val;
    }

    fun void jump(CKFXR@ ckfxr)
    {
        reset();

        CKFXR.WaveType_SQUARE => waveform;

        if (rnd(1)) {
            frnd(0.1, 0.6) => pwm_depth.val;
            frnd(2, 20) => pwm_freq.val;
        }

        frnd(40, 80) => freq_base_midi.val;
        frnd(12, 48) => freq_ramp.val;
        frnd(12, 48) => freq_dramp.val;

        frnd(100, 300) => sustain_dur_ms.val;
        frnd(100, 300) => release_dur_ms.val;

        if (rnd(1))
            frnd(30, 3000) => hpf_freq.val;
        if (rnd(1))
            frnd(hpf_freq.val(), 18000) => lpf_freq.val;
    }

    fun void blipSelect(CKFXR@ ckfxr)
    {
        reset();

        rnd(1) => waveform;
        if (waveform == CKFXR.WaveType_SQUARE) {
            frnd(0.2) => pwm_depth.val;
            frnd(20) => pwm_freq.val;
        }

        frnd(36, 96) => freq_base_midi.val;

        frnd(50, 100) => sustain_dur_ms.val;
        frnd(10, 100) => release_dur_ms.val;

        frnd(100, 200) => hpf_freq.val;
    }

    fun void play(CKFXR@ ckfxr)
    {
        copyToSynth(ckfxr);
        spork ~ ckfxr.play();
    }

    fun void record(CKFXR@ ckfxr)
    {
        copyToSynth(ckfxr);
        wvout.wavFilename(me.dir() + export_wav_path.val());
        ckfxr.play();
        wvout.closeFile();
    }

    // return random float in range [0, range]
    fun float frnd(float range)
    {
        return range * Math.randomf();
    }

    fun float frnd(float l, float h)
    {
        return Math.random2f(l, h);
    }

    // return random int in range [0, n]
    fun int rnd(int n)
    {
        return Math.random() % (n + 1);
    }
}

fun void centerNext(float item_width)
{
    UI.getContentRegionAvail().x => float avail_width;
    UI.setCursorPosX(UI.getCursorPosX() + (avail_width - item_width) * .5);
}

fun void centerText(string t)
{
    centerNext(UI.calcTextSize(t).x);
    UI.text(t);
}

fun int centerButton(string t, vec2 size)
{
    centerNext(size.x);
    return UI.button(t, size);
}

fun void ui()
{

["square", "saw", "sin", "noise"] @=> string waveforms[];
["Pickup Coin", "Laser", "Explosion", "Powerup", "Hit/Hurt", "Jump", "Blip/Select"] @=> string presets[];

while (1) {
GG.nextFrame() => now;

UI.getMainViewport() @=> UI_Viewport @ viewport;
UI.setNextWindowPos(viewport.workPos(), UI_Cond.Always);
UI.setNextWindowSize(viewport.workSize(), UI_Cond.Always);
UI.pushStyleVar(UI_StyleVar.WindowRounding, 0.0);
if (UI.begin("CKFXR", null, UI_WindowFlags.NoDecoration | UI_WindowFlags.NoResize | UI_WindowFlags.NoMove)) {
    UI.getWindowSize() => vec2 size;

    // Left Pane ---------------------------------------
    UI.pushStyleVar(UI_StyleVar.WindowPadding, @(0, 10));
    UI.beginChild(
        "Left Generator",
        @(size.x * .2, 0), 
        UI_ChildFlags.Border,
        0
    );

    UI.getWindowSize() => vec2 left_size;

    centerText("Presets");

    UI.dummy(@(0.0, 10.0)); // vertical spacing

    presets.size() $ float => float num_presets;
    for (int i; i < presets.size(); i++) {
        UI.pushStyleColor(UI_Color.Button, UI.convertHSVtoRGB(@(i / num_presets, 0.6f, 0.6f)));
        UI.pushStyleColor(UI_Color.ButtonHovered, UI.convertHSVtoRGB(@(i / num_presets, 0.7f, 0.7f)));
        UI.pushStyleColor(UI_Color.ButtonActive, UI.convertHSVtoRGB(@(i / num_presets, 0.8f, 0.8f)));

        if (centerButton(presets[i], @(left_size.x * .8, 40))) {
            if (i == 0) p.pickupCoin(ckfxr);
            else if (i == 1) p.shootLaser(ckfxr);
            else if (i == 2) p.explosion(ckfxr);
            else if (i == 3) p.powerup(ckfxr);
            else if (i == 4) p.hitHurt(ckfxr);
            else if (i == 5) p.jump(ckfxr);
            else if (i == 6) p.blipSelect(ckfxr);
            p.play(ckfxr);
        }
        UI.popStyleColor(3);

        UI.dummy(@(0.0, 10.0)); // vertical spacing
    }

    UI.endChild(); // "Left Generator"
    UI.popStyleVar(); // WindowPadding = 0, 10

    UI.sameLine();

    // Middle Pane ---------------------------------------
    UI.beginChild(
        "item view", 
        @(size.x * .6, 0),
        0, 0
    ); 

    centerText("Manual Settings");

    UI.getWindowWidth() => float middle_width;

    // waveform selection
    UI.separatorText("Waveform");
    for (int n; n < waveforms.size(); n++) {
        if (UI.selectable(waveforms[n], p.waveform == n, 0, 
            @(middle_width / waveforms.size(), 0))
        )
            n => p.waveform;
        if (n < waveforms.size() - 1) UI.sameLine();
    }

    UI.separatorText("Envelope");
    UI.itemTooltip("ctrl+click a slider to input a specific value");

    UI.slider("Attack (ms)", p.attack_dur_ms, 1, 1000);
    UI.slider("Release (ms)", p.release_dur_ms, 1, 1000);
    UI.slider("Sustain (ms)", p.sustain_dur_ms, 1, 1000);
    UI.slider("Sustain Level", p.sustain_level, 0, 1);

    UI.separatorText("Frequency");
    UI.slider("Base (MIDI)", p.freq_base_midi, 0, 127);
    UI.slider("Limit (MIDI)", p.freq_limit_midi, 0, 127);
    UI.slider("Ramp (semitones/s)", p.freq_ramp, -127, 127);
    UI.slider("Ramp Delta (semitones/s^2)", p.freq_dramp, -127, 127);

    UI.separatorText("Vibrato");
    UI.slider("Vibrato Depth", p.vib_depth, 0, 1);
    UI.slider("Vibrato Frequency (Hz)", p.vib_freq, 0, 20);

    UI.separatorText("Interval (changes base freq)");
    UI.slider("Size (MIDI)", p.arp_mod_midi, -127, 127);
    UI.slider("Onset (ms)", p.arp_time_ms, 0, 1000);

    UI.separatorText("Pulse-Width Modulation");
    UI.slider("PWM Depth", p.pwm_depth, 0, 1);
    UI.slider("PWM Frequency (Hz)", p.pwm_freq, 0, 20);

    UI.separatorText("Delay");
    UI.slider("Feedback Gain", p.feedback_gain, 0, .99);
    UI.slider("Base Delay (ms)", p.delay_base_dur_ms, 0, 100);
    UI.slider("Delay Mod Frequency (Hz)", p.delay_mod_freq, 0, 20);
    UI.slider("Delay Mod Depth", p.delay_mod_depth, 0, 1);

    UI.separatorText("Lowpass Filter");
    UI.slider("LPF Frequency (Hz)", p.lpf_freq, 0, 20000);
    UI.slider("LPF Ramp (semitones/s)", p.lpf_ramp, -127, 127);
    UI.slider("LPF Resonance", p.lpf_resonance, 0, 10);

    UI.separatorText("Highpass Filter");
    UI.slider("HPF Frequency (Hz)", p.hpf_freq, 0, 20000);
    UI.slider("HPF Ramp (semitones/s)", p.hpf_ramp, -127, 127);

    UI.separator();
    UI.endChild();

    UI.sameLine();

    // Right Pane ---------------------------------------

    UI.pushStyleVar(UI_StyleVar.WindowPadding, @(0, 10));
    UI.beginChild(
        "right pane", @(0, 0), UI_ChildFlags.Border, 0
    );
    // UI.getWindowSize() => vec2 right_size;
    UI.getContentRegionAvail() => vec2 right_size;

    centerNext(UI.calcTextSize("Volume").x);
    UI.text("Volume");
    centerNext(right_size.x * .4);
    // ## hidden label + id. See https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-how-can-i-have-multiple-windows-with-the-same-label
    if (UI.vslider("##volume", @(right_size.x * .4, right_size.y * .2), p.main_gain, 0, 1)) {
        p.main_gain.val() => ckfxr.gain;
    }

    UI.dummy(@(0.0f, 20.0f)); // vertical spacing

    UI.pushStyleColor(UI_Color.Button, UI.convertHSVtoRGB(@(2.0/7, 0.6, 0.6)));
    UI.pushStyleColor(UI_Color.ButtonHovered, UI.convertHSVtoRGB(@(2.0/7, 0.7, 0.7)));
    UI.pushStyleColor(UI_Color.ButtonActive, UI.convertHSVtoRGB(@(2.0/7, 0.8, 0.8)));
    if (centerButton("Play Sound", @(right_size.x * .8, 40)))
        p.play(ckfxr);
    UI.itemTooltip("Press space or return to play sound");
    UI.popStyleColor(3);

    UI.dummy(@(0.0f, 20.0f)); // vertical spacing

    centerNext(right_size.x * .8);
    UI.setNextItemWidth(right_size.x * .8);
    UI.inputText("##export_wav_path", p.export_wav_path);
    centerNext(right_size.x * .8);
    if (UI.button("Export WAV", @(right_size.x * .8, 40))) {
        <<< "Exporting to", me.dir() + p.export_wav_path.val() >>>;
        spork ~ p.record(ckfxr);
    }

    UI.dummy(@(0.0f, 20.0f)); // vertical spacing

    // plot waveform as line
    map2waveform( samples );
    centerNext(right_size.x * .8);
    UI.plotLines("##Waveform", samples, 0, "512 samples", -1, 1, @(right_size.x * .8, 100));

    UI.dummy(@(0.0f, 20.0f)); // vertical spacing

    // plot spectrum as histogram
    centerNext(right_size.x * .8);
    map2spectrum( response, spectrum_hist_positions );
    UI.plotHistogram("##Spectrum", spectrum_hist_positions, 0, "512 bins", 0, 8, @(right_size.x * .8, 100));

    UI.endChild();
    UI.popStyleVar();

}
UI.end(); // CKFXR
UI.popStyleVar(1); // WindowRounding = 0.0
}
} spork ~ ui();

// keyboard controls
fun void kb()
{
    while (1) {
        GG.nextFrame() => now;
        if (UI.isKeyPressed(UI_Key.Space, false) || UI.isKeyPressed(UI_Key.Enter, false))
            p.play(ckfxr);
        // generator presets
        if (UI.isKeyPressed(UI_Key.Num1)) {
            p.pickupCoin(ckfxr);
            p.play(ckfxr);
        }
        if (UI.isKeyPressed(UI_Key.Num2)) {
            p.shootLaser(ckfxr);
            p.play(ckfxr);
        }
        if (UI.isKeyPressed(UI_Key.Num3)) {
            p.explosion(ckfxr);
            p.play(ckfxr);
        }
        if (UI.isKeyPressed(UI_Key.Num4)) {
            p.powerup(ckfxr);
            p.play(ckfxr);
        }
        if (UI.isKeyPressed(UI_Key.Num5)) {
            p.hitHurt(ckfxr);
            p.play(ckfxr);
        }
        if (UI.isKeyPressed(UI_Key.Num6)) {
            p.jump(ckfxr);
            p.play(ckfxr);
        }
        if (UI.isKeyPressed(UI_Key.Num7)) {
            p.blipSelect(ckfxr);
            p.play(ckfxr);
        }
        if (UI.isKeyPressed(UI_Key.Z, false)) {
            p.reset();
        }
    }
} spork ~ kb();

while( true ) 1::eon => now;
