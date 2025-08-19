/*
Ideas

======
Perc
======

Inspos
- Aphex Twin
    - https://www.youtube.com/watch?v=vHXi__0HQrk&ab_channel=CaptainPikant
- Drum and Bass
- Techno
- synthwave
    - airglow, memory bank (bytepath soundtrack)
        - https://airglow-strat.bandcamp.com/album/airglow-memory-bank
        

Techniques
- Ratchet
- Short burts (really short gate/envelope) + higher gain
- variable velocity on drum attacks
- variable swing
- probabilistic spot effects
    - really high reverb send on single drum hit
- ghost kicks
- shifting kick position left/right by 1/8th or 1/16th
- Other things I notice I enjoy doing manually 
    - shifting the position of certain beats around (avoids repetition)

Revive the drum machine from Stormfury

Synthesis Sources
- wav files ok for now
- STK shaker ugen and modal bar
- use vtable offset on UGen ".last()" to create a visualizer UI component for waveform, envelopes, etc.
    - just do in chuck itself if possible, can add to ulib_imgui after

Gameplay
- flying through space, you find random junk flying around (comes out of asteroid)
and these can be used for your drum rack?
    - e.g. you find a metal fork and it adds a sample? 

Plot Premise:
- why are you building up a soundtrack?
    - something about no sound in space, vibrations being amenable to mining...
        - oh duh, all you hear is your own soundtrack! no sfx from asteroids or anything else
        - possible motive: silence of space is oppressive, hence upgrading ship sound system
    - are you an outlaw trying to outfit their ship?
    - or are you some corporate miner?
- Possible name: Major Minor (pun on homophone between Minor and Miner)

Major Qs:
- How much control should the player be afforded?
    - One extreme: present 16-step sequencer to player, let them place beats manually
    - another extreme: musical system determines everything for you, you just increase/decrease density or make other high level decisions
- To explore/implement
    - make each step a "superstruct" of musical options, e.g. bools for: FX/Reverb send...
        - maybe you can treat each step like Balatro treats jokers: they can be upgraded with velocity, gates, FX sends...
    - a pool of beat evolution cards
    - integrate stormfury drum sequencer + euclidean rhythm algo
    - test ImGUI or chugl 2D drum sequencer interface (to at least visualize the sequence)
        - can we go against convention and not just be a square grid?


Topograph params
- tempo
- kick/snare/hat density
- map XY
- mode: original, henri, euclidean
- per instrument:
    - rates
    - volume
    - playhead pattern (linear forward, reverse, pingpong)
    - chaos
- per step
    - ratchet
    - FX send
    - velocity
- *interpolation/filtering pattern* i.e. how do we blend between patterns? (currently does a bilinear filter)

Topograph integration ideas
- 

*/

@import "topograph.ck"
Topograph topograph;

T.print(topograph.drum_map[0][2]);

JCRev rev => Echo a => Echo b => Echo c => dac;
// set max delay for echo
1000::ms => a.max => b.max => c.max;
// set delay for echo
750::ms => a.delay => b.delay => c.delay;
// set the initial effect mix
0.0 => a.mix => b.mix => c.mix;
0 => rev.mix;

SndBuf kick => rev;
SndBuf snare => rev;
SndBuf hat => rev;
me.dir() + "../../../assets/samples/punchy-kick.wav" => kick.read;
me.dir() + "../../../assets/samples/snare.wav" => snare.read;
me.dir() + "../../../assets/samples/trap-hihat.wav" => hat.read;
0 => kick.rate => snare.rate => hat.rate;
[kick, snare, hat] @=> SndBuf drum_rack[];

120.0 => float BPM;
(60.0 / BPM)::second => dur qt_note;
qt_note / 8.0 => dur step; // 1 drum machine step = 32nd note

float topograph_xy[2];
float density[3];
UI_Bool mode_henri(topograph.mode_henri);
UI_Float rev_mix(rev.mix());
UI_Float echo_mix(a.mix());

fun void ui() {
    while (true) {
        GG.nextFrame() => now;
        if (UI.begin("topograph", null, 0)) {
            if (UI.slider("Map XY", topograph_xy, 0, 1)) {
                topograph.pos(topograph_xy[0], topograph_xy[1]);
            }

            if (UI.slider("Density", density, 0, 1)) {
                topograph.density(0, density[0]);
                topograph.density(1, density[1]);
                topograph.density(2, density[2]);
            }

            if (UI.slider("Reverb", rev_mix, 0, 1)) {
                rev_mix.val() => rev.mix;
            }

            if (UI.slider("Echo", echo_mix, 0, 1)) {
                echo_mix.val() => a.mix;
                echo_mix.val() => b.mix;
                echo_mix.val() => c.mix;
            }

            if (UI.checkbox("Mode Henri", mode_henri)) {
                mode_henri.val() => topograph.mode_henri;
            }
        }
        UI.end();
    }
} 
spork ~ ui();

// 1::eon => now;
while (true) {
    topograph.step() @=> int velocities[];
    // play instruments
    for (int i; i < velocities.size(); i++) {
        if (velocities[i] > 0) {
            0 => drum_rack[i].pos;
            1.0 => drum_rack[i].rate;
            velocities[i] / 255.0 => drum_rack[i].gain;
            <<< drum_rack[i].gain() >>>;
        }
    }
    step => now;
}
