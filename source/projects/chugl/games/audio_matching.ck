/*

Parameters to vary
- number of sounds required for match (3 or more?)
- harmonic relationship (octave, unison, triad, 7th, etc)
- auditory gestalt principle
    - similarity in pitch
    - harmonic similarity
    - similarity in timbre
- audio occlusion (via walls/terrain etc)

Ideas:
- water ripple as you move around to slightly shift the positions of the sound sources
- or some puzzles where the sound sources are moving (on rafts/currents/etc) so you have to time it 
perfectly to clear the level (e.g. certain clusters must be done in a specific order at specific time)
- multiple levels that experiment with different matching parameters
- position sound sources according to worley/voronoise on points evenly distributed in a circle,
rather than a rectangular grid
    - something about fibonacci spirals

Sound Ideas
- different levels/worlds can have different musical themes/genres
- for ambient world:
    - maybe limit to Geodesics Energy + Plateau?
- binaural spatialization
    - maybe add doppler too?
*/

@import "lib/g2d/ChuGL.chug"
@import "lib/g2d/g2d.ck"
@import "lib/M.ck"
@import "lib/T.ck"
@import "lib/Binaural-mac.chug"
@import "HashMap.chug"

G2D g;

vec3 midi_colors[12]; // for debug
for (int i; i < midi_colors.size(); i++) {
    Color.hsv2rgb(@(360 * i / 12.0, 1.0, 1.0)) => midi_colors[i];
    <<< midi_colors[i] >>>;
}

class GameState {
    vec2 player_pos;

    72 => int base_midi;
    [0, 2, 4, 7, 9, 11] @=> int scale[];
    HashMap prev_sound_source_set;
    // [null, null, null] @=> Entity nearest_sound_sources[];
    Entity nearest_sound_sources[3];
    float cached_distances[3];
    int max_nearest_idx; // of the closest entities, what index is the farthest one?

    // Sound params =========================
    NRev rev => dac;
    .02 => rev.mix;

    fun void updateNearest(Entity e, vec2 player_pos) {
        M.dist(e.pos, player_pos) => float dist;
        if (dist >= this.cached_distances[this.max_nearest_idx]) return; // early out

        // invariant: replace the farthest sound source
        e @=> this.nearest_sound_sources[this.max_nearest_idx];
        dist => this.cached_distances[this.max_nearest_idx];

        // update max idx
        -1 => float new_max_dist;
        for (int i; i < this.cached_distances.size(); i++) {
            this.cached_distances[i] => float near_dist;
            if (near_dist > new_max_dist) {
                near_dist => new_max_dist;
                i => this.max_nearest_idx;
            }
        }
    }
}

GameState gs;

class Entity {
    int disabled;
    // Sound Source ---------------------
    vec2 pos;
    int midi_pitch;
    TriOsc osc => ADSR env(10::ms, 2000::ms, .1, 1000::ms);
    // CNoise osc => ADSR env(10::ms, 2000::ms, .8, 1000::ms) => Binaural binaural;
    int connect_generation;
    int connected;

    // fx state
    .5::second => dur ripple_cd;
    time last_ripple;

    fun void update(float dt) {
        if (disabled) return;

        if (connected) {
            if (now - last_ripple > ripple_cd) {
                spork ~ FX.ripple(pos, .25);
                now => last_ripple;
            }

            // spatialization
            // <<< (M.RAD2DEG * M.angle(gs.player_pos, pos)) - 90 >>>;
            // (M.RAD2DEG * M.angle(gs.player_pos, pos)) - 90 => float azimuth_deg;
            // azimuth_deg => binaural.azimuth;
        }
    }

    fun void disable() {
        true => disabled;
        spork ~ onDisconnect();
    }

    fun void onConnect() {
        now => last_ripple;
        ++connect_generation;
        true => connected;
        // binaural => gs.rev;
        env => gs.rev;
        osc.gain(.2);
        osc.freq(Std.mtof(midi_pitch));
        env.keyOn(1);
    }
    
    fun void onDisconnect() {
        if (!connected) return;
        false => connected;
        connect_generation => int gen;
        env.keyOff();
        env.releaseTime() => now;

        // <<< "disconnect" >>>;

        // only disconnect if there wasn't another connect
        if (connect_generation == gen) {
            env =< gs.rev;
            // binaural =< gs.rev;
        }
    }
}

class FX {
    // slowly expanding ring
    fun static void ripple(vec2 pos, float end_radius) {
        1.5::second => dur effect_dur;

        dur elapsed_time;
        while (elapsed_time < effect_dur) {
            GG.nextFrame() => now;
            GG.dt()::second +=> elapsed_time;
            M.easeOutQuad(elapsed_time / effect_dur) => float t;

            g.circle(pos, end_radius * t, .1 * (1 - t), Color.WHITE * (1 - t));
        }
    }
}

Entity sound_sources[0];
8 => int grid_dim; // partitions screen into 8x8 grid
5.0 / grid_dim => float box_hw;

for (int x; x < grid_dim; x++) {
    for (int y; y < grid_dim; y++) {
        // compute grid boundaries
        -5 + 2 * box_hw * x + box_hw => float box_x;
        -5 + 2 * box_hw * y  + box_hw => float box_y;

        Entity e;
        sound_sources << e;
        M.randomPointInArea(@(box_x, box_y), box_hw, box_hw) => e.pos;
        Math.random2(60,72) => e.midi_pitch;
        gs.base_midi + M.randomElement(gs.scale) => e.midi_pitch;
    }
}

while (1) {
    GG.nextFrame() => now;
    GG.dt() => float dt;

    { // input
        g.mousePos() => gs.player_pos;
        if (GWindow.mouseLeftDown()) {
            // check and remove matching pitches
            HashMap pitch_map;
            for (int i; i < gs.nearest_sound_sources.size(); i++) {
                gs.nearest_sound_sources[i] @=> Entity e;
                if (e == null) continue;
                if (pitch_map.has(e.midi_pitch)) { // match! remove both
                    (pitch_map.getObj(e.midi_pitch) $ Entity) @=> Entity other;
                    if (other != null) other.disable();
                    e.disable();
                } else {
                    pitch_map.set(e.midi_pitch, e); 
                }
            }
        }
    }

    // reset gamestate
    gs.prev_sound_source_set.clear();
    for (int i; i < gs.nearest_sound_sources.size(); i++) {
        gs.nearest_sound_sources[i] @=> Entity near;
        // save previous set
        if (near != null) gs.prev_sound_source_set.set(gs.nearest_sound_sources[i], true);
        // clear current set
        null @=> gs.nearest_sound_sources[i];
        Math.FLOAT_MAX => gs.cached_distances[i];
    }

    g.circle(gs.player_pos, .1, Color.GREEN);

    // draw gridlines
    g.pushColor(.01 * Color.WHITE);
    for (int x; x <= grid_dim; x++) {
        -5 + 2 * box_hw * x => float x;
        g.line(@(x, 10), @(x,-10));
        g.line(@(10, x), @(-10,x));

        // for (int y; y < grid_dim; y++) {
        //     // compute grid boundaries
        //     -5 + 2 * box_hw * x + box_hw => float box_x;
        //     -5 + 2 * box_hw * y  + box_hw => float box_y;
        // }
    }
    g.popColor();

    // draw sound sources and track 3 nearest
    for (int i; i < sound_sources.size(); i++) {
        sound_sources[i] @=> Entity e;
        if (e == null || e.disabled) continue;

        g.circleFilled(e.pos, .08);

        // debug colors
        // g.circleFilled(e.pos, .08, midi_colors[(e.midi_pitch % 12)]);

        // M.dist(player_pos, e.pos) => float dist;
        // g.pushColor(Color.RED);
        // g.text(dist + "", e.pos, .2);
        // g.popColor();

        gs.updateNearest(e, gs.player_pos);

        e.update(dt);
    }

    // draw dashed lines to 3 nearest
    for (int i; i < gs.nearest_sound_sources.size(); i++) {
        gs.nearest_sound_sources[i] @=> Entity e;
        if (e == null) continue;
        g.dashed(gs.player_pos, e.pos, Color.WHITE, .1);

        // if it's a new connection, trigger note
        !gs.prev_sound_source_set.has(e) => int connected;
        if (connected) {
            e.onConnect();
            spork ~ FX.ripple(e.pos, .5);
        }
    }

    // get new disconnects
    gs.prev_sound_source_set.objKeys() @=> Object prev_connects[];
    for (auto e : prev_connects) {
        !T.arrayHas(gs.nearest_sound_sources, e) => int disconnected;
        if (e == null) continue;
        if (disconnected) spork ~ (e $ Entity).onDisconnect();
    }

    // <<< "------" >>>;

}
