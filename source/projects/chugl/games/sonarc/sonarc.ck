/*

MVP
- Start Room
    - UI to choose number of players (max 4)
    - press <Space> or click [Start] to start game
- Game Room
    - boundaries are slightly within edge of screen
    - mic input controls movement
    - trails are fixed #steps for now, and always deploying
        - to avoid precision errors, only write to trail history if player
        delta pos is > some Epsilon (test this out)
    - game ends when 1 player is left alive
        - transition back to start room
- bonus: replay the previous match with audio and all

TODO
- b2 collision testing
- add explosion sound effect / fart noise when someone dies
- add center and left-align options to GText
    - also add #characters to allow for rpg-like rolling text
    - also be able to query size of characters in base font, so we know how to rescale
    when screen size changes.
        - what ever the default font size we rasterize at, give worldspace dimensions of a character 
        given .sca == 1

VR Lab Feedback
- when player wins, change font color to that player, add fireworks in that color in the background
- slow-motion hit-stop when someone dies (watch sakurai's video)
- have number of players NOT mapped to arrow keys

Ideas for progression/scaling difficulty:
- trails grow longer over time (add food to eat to grow longer like slither.io?)
- arena gets smaller
- players can choose when to drop trails (by holding down key)
    - trails remain forever
    - no default trail history
    - you have to yell into mic to replenish ammo for drawing trails?
        - but then does mic input do both movemenet AND trail ammo?
- overtime start playing music that forces the mic gain to be triggered
- lines that spawn on border or center and start sweeping in towards center
    - lines become the waveform of mic input


Optimization for line collision testing
- only add a history position if it's *not* collinear with the last segment
    - if it is collinear, extend the last segment
- track AABB for each player's arc, in broad-phase culling compute AABB intersection
- check all players simultaneously against all trail histories to avoid looping per player and to improve cache performance
- actually most optimal is just put the trail in b2, use b2 for collision detection

*/

@import "lib/g2d/ChuGL_debug.chug"
@import "lib/g2d/g2d.ck"
@import "lib/M.ck"
@import "lib/T.ck"

G2D g;

// ========================
// Sound
// ========================
// idea: we want the mic volume to slew up quickly but slew down slowly (fills fast, depletes slow)
// 
// TODO: maybe run through LPF first for gain detection?
adc => Gain adc_square => OnePole env_follower => blackhole;
// square the input
adc => adc_square;
// multiply
3 => adc_square.op;

SndBuf replay_audio => dac;

// filter pole position
UI_Float env_low_cut(.08);
UI_Float env_exp(.22);
UI_Float env_pol_last;
UI_Float env_pole_pos(.9998);

env_pole_pos.val() => env_follower.pole;

class Room {
    fun void enter() {}
    fun void leave() {}
    fun void update(float dt) {}
    fun void draw() {}
}

class GameState {
    Room room;

    2 => int num_players;
    Entity players[4]; // max 4 players

    // sound ==================================
    float mic_volume;

    // config constants =======================
    .5 => float BORDER_PADDING; // padding in worldspace units of sides/top


    .25 => float player_scale;
    UI_Float player_speed_volume_scale(8); // how much to scale speed with mic volume 
    .3 => float player_base_speed;
    3.5 => float player_rot_speed; // idea: scale with mic volume too
    [ 
        player_scale * @(1 / Math.sqrt(3), 0),
        player_scale * @(-1 / (2 * Math.sqrt(3)), .5),
        player_scale * @(-1 / (2 * Math.sqrt(3)), -.5),
    ] @=> vec2 player_vertices[];

    [
        @(3, -3),
        @(-3, 3),
        @(3, 3),
        @(-3, -3),
    ] @=> vec2 player_spawns[];

    [
        90.0 + 45,
        -45.0,
        180 + 45,
        45,
    ] @=> float player_rots_deg[];

    [
        Color.hex(0x00ffff),
        Color.hex(0xffa500),
        Color.hex(0x32CD32),
        Color.hex(0xFF0f00),
    ] @=> vec3 player_colors[];

    [
        GWindow.Key_Right,  // p1
        GWindow.Key_D,      // p2
        GWindow.Key_P,
        GWindow.Key_M,
    ] @=> int player_key_right[];

    [
        GWindow.Key_Left, 
        GWindow.Key_A, 
        GWindow.Key_O, 
        GWindow.Key_N, 
    ] @=> int player_key_left[];

    // replay ==================================
    "sonarc-replay-audio.wav" => string replay_audio_filename;
    "sonarc-replay-keystrokes.txt" => string replay_ks_filename;
    "sonarc-replay-volume.txt" => string replay_gain_filename;



    fun void enterRoom(Room new_room) {
        T.assert(new_room != room, "cannot transition from a room to itself");
        room.leave();
        new_room.enter();
        new_room @=> room;
    }
}
GameState gs;

// key mapping enum
0 => int Key_Left;
1 => int Key_Right;

class Entity {
    // player entity
    int which_player; // 1 2 3 or 4
    vec2 player_pos;
    vec2 player_prev_pos;
    float player_rot;
    vec2 player_pos_history[120];
    int player_pos_history_write_idx;
    int player_pos_history_read_idx;
    int frame_alive_count;
    vec3 color;
    float last_speed;

    vec2 colliding_segments[0];

    int disabled;

    // control mapping for each player
    fun int key(int which) {
        if (gs.room == match_room && match_room.match_state == match_room.Match_Replay) {
            if (which == Key_Left) {
                return T.arrayHas(match_room.replay_current_keys, gs.player_key_left[which_player]);
            }
            if (which == Key_Right) {
                return T.arrayHas(match_room.replay_current_keys, gs.player_key_right[which_player]);
            }
        } else {
            if (which == Key_Left) return GWindow.key(gs.player_key_left[which_player]);
            if (which == Key_Right) return GWindow.key(gs.player_key_right[which_player]);
        }
        return false;
    }

    fun void die() {
        // destroy player
        true => disabled;
        spork ~ FX.explode(player_pos, 1::second, color);
    }

    // checks trail collection of segment against this entity's trail history
    // fun int checkTrailCollision(vec2 prev_pos, vec2 curr_pos) {
    //     // don't collide with a newly spawned player
    //     player_pos_history.size() => int history_size;
    //     if (frame_alive_count < history_size) return false;

    //     player_pos_history_read_idx => int start_idx;
    //     start_idx - 3 => int end_idx; // ignore most recent trail segments to avoid self-collision
    //     if (end_idx < 0) player_pos_history.size() +=> end_idx;

    //     // go through entire buffer (because we only check after it's been filled once)
    //     // ignore the 3 entries before read_idx

    //     if (start_idx > end_idx) {
    //         for (start_idx => int i; i < player_pos_history.size() - 1; i++) {
    //             if (M.intersect(
    //                 prev_pos, curr_pos, 
    //                 player_pos_history[i], player_pos_history[i + 1]
    //             )) {
    //                 <<< "54", start_idx, end_idx, i, 
    //                 "prev pos", prev_pos, "cur pos", curr_pos,
    //                 player_pos_history[i], player_pos_history[i + 1]
    //                 >>>;
    //                 colliding_segments << prev_pos << curr_pos;
    //                 colliding_segments << player_pos_history[i] << player_pos_history[i + 1];
    //                 return true;
    //             }
    //         }
    //         for (0 => int i; i <= end_idx; i++) {
    //             if (M.intersect(
    //                 prev_pos, curr_pos, 
    //                 player_pos_history[i - 1], player_pos_history[i]
    //             )) {
    //                 <<< "60", start_idx, end_idx, i >>>;
    //                 colliding_segments << prev_pos << curr_pos;
    //                 colliding_segments << player_pos_history[i - 1] << player_pos_history[i];
    //                 return true;
    //             }
    //         }
    //     } else {
    //         for (start_idx => int i; i < end_idx; i++) {
    //             if (M.intersect(
    //                 prev_pos, curr_pos, 
    //                 player_pos_history[i], player_pos_history[i + 1]
    //             )) {
    //                 <<< "67" >>>;
    //                 return true;
    //             }
    //         }
    //     }
    //     return false;
    // }

    fun void update(float dt) {
        if (disabled) return;

        frame_alive_count++;

        { // input
            // gs.player_rot_speed + Math.pow(1 + gs.mic_volume, 2) * gs.player_speed_volume_scale.val() => float rot_speed;
            gs.player_rot_speed + gs.player_speed_volume_scale.val() * gs.mic_volume => float rot_speed;
            if (key(Key_Left)) {
                (dt * rot_speed) +=> player_rot;
            }
            if (key(Key_Right)) {
                -(dt * rot_speed) +=> player_rot;
            }
        }

        { // update
            player_pos => player_prev_pos;
            // (Math.pow(1 + gs.mic_volume, 2) * gs.player_speed_volume_scale.val()) => float speed;
            gs.player_base_speed + gs.player_speed_volume_scale.val() * gs.mic_volume => float speed;
            speed => last_speed;
            dt * speed * M.rot2vec(player_rot) +=> player_pos;
            player_pos => player_pos_history[player_pos_history_write_idx++];
            if (player_pos_history_write_idx >= player_pos_history.size()) {
                0 => player_pos_history_write_idx;
            }
            // if we have filled the buffer once, update the read head to 1 in front of write
            if (frame_alive_count >= player_pos_history.size()) {
                player_pos_history_write_idx => player_pos_history_read_idx;
            }    
        }
    }

    fun void draw() {
        g.pushColor(color);

        // draw player
        if (!disabled) {
            g.polygonFilled(
                player_pos, player_rot, gs.player_vertices, 0.0
            );
        }

        // draw trail
        // g.pushColor(Color.RED);
        // g.line(player_pos_history, player_pos_history_write_idx - 20, 20);
        // g.popColor();
        g.line(player_pos_history, player_pos_history_read_idx, frame_alive_count);

        spork ~ FX.booster(
            player_pos,
            color,
            last_speed * gs.player_scale * 2 // scale trail size by speed
        );

        // g.pushColor(Color.GOLD);
        for (int i; i < colliding_segments.size(); 2 +=> i) {
            colliding_segments[i] - colliding_segments[i + 1] => vec2 dir;
            .5 * (colliding_segments[i] + colliding_segments[i + 1]) => vec2 center;
            g.line(center + 10 * dir, center - 10 * dir);
        }
        g.popColor();
    }
}

class FX {

    // spawns an explosion of lines going in random directinos that gradually shorten
    fun static void explode(vec2 pos, dur max_dur, vec3 color) {
        // params
        Math.random2(8, 16) => int num; // number of lines

        vec2 positions[num];
        vec2 dir[num];
        float lengths[num];
        dur durations[num];
        float velocities[num];

        // init
        for (int i; i < num; i++) {
            pos => positions[i];
            M.randomDir() => dir[i];
            Math.random2f(.1, .2) => lengths[i];
            Math.random2f(.3, max_dur / second)::second => durations[i];
            Math.random2f(.01, .02) => velocities[i];
        }

        dur elapsed_time;
        while (elapsed_time < max_dur) {
            GG.nextFrame() => now;
            GG.dt()::second +=> elapsed_time;

            for (int i; i < num; i++) {
                // update line 
                (elapsed_time) / durations[i] => float t;
                // if animation still in progress for this line
                if (t < 1) {
                    // update position
                    velocities[i] * dir[i] +=> positions[i];
                    // shrink lengths linearly down to 0
                    lengths[i] * (1 - t) => float len;
                    // draw
                    g.line(positions[i], positions[i] + len * dir[i], color);
                }
            }
        }
    }

    fun static void booster(vec2 pos, vec3 color, float radius_scale) {
        radius_scale * Math.random2f(.04, .046) => float init_radius;
        init_radius * 8 * .5::second => dur effect_dur; // time to dissolve

        Math.random2f(0.8, 1.0) * color => vec3 init_color;

        dur elapsed_time;
        while (elapsed_time < effect_dur) {
            GG.nextFrame() => now;
            GG.dt()::second +=> elapsed_time;
            1.0 - elapsed_time / effect_dur => float t;

            g.circle(pos, init_radius * t, 1.0, t * t * init_color);
        }
    }
}

class StartRoom extends Room {}
class MatchRoom extends Room {
    // match state enum
    0 => static int Match_Select;
    1 => static int Match_Running;
    2 => static int Match_Replay;

    vec2 border_min, border_max;

    Match_Select => int match_state;
    Entity@ winner;
    int is_draw;

    // replay state
    FileIO replay_ks;
    FileIO replay_mic_gain;
    int replay_current_keys[0];
    StringTokenizer strtok;
    float current_mic_gain;

    fun void enter() {
        _initPlayers();
    }

    fun void update(float dt) {
        { // sound update
            if (match_state == Match_Replay) {
                if (replay_mic_gain.more()) replay_mic_gain.readLine().trim().toFloat() => gs.mic_volume;
            } else {
                Math.max(
                    0,
                    Math.pow(env_follower.last(), env_exp.val()) - env_low_cut.val()
                ) => float new_volume;
                new_volume => gs.mic_volume;

            }
        }

        // update border vertices
        g.screenSize() => vec2 screen_size;
        screen_size * -.5 + @(gs.BORDER_PADDING, gs.BORDER_PADDING) => border_min;
        screen_size *  .5 - @(gs.BORDER_PADDING, gs.BORDER_PADDING) => border_max;
        [
            border_min, @(border_max.x, border_min.y),
            border_max, @(border_min.x, border_max.y)
        ] @=> vec2 border_vertices[];

        if (match_state == Match_Select) {
            // start match
            if (GWindow.keyDown(GWindow.Key_Space)) {
                Match_Running => match_state;
                // start collecting replay data
                spork ~ record();
                return;
            }

            if (GWindow.keyDown(GWindow.Key_R)) {
                // TODO add error handling if replay files don't exist
                Match_Replay => match_state;

                // open text file and replay file
                replay_ks.open( gs.replay_ks_filename, FileIO.READ );
                replay_mic_gain.open( gs.replay_gain_filename, FileIO.READ );

                // load replay audio file
                replay_audio.read(gs.replay_audio_filename);
                return;
            }

            // update player count
            if (GWindow.keyDown(GWindow.Key_Left)) gs.num_players--;
            if (GWindow.keyDown(GWindow.Key_Right)) gs.num_players++;
            if (GWindow.keyDown(GWindow.Key_2)) 2 => gs.num_players;
            if (GWindow.keyDown(GWindow.Key_3)) 3 => gs.num_players;
            if (GWindow.keyDown(GWindow.Key_4)) 4 => gs.num_players;
            
            M.clamp(gs.num_players, 2, 4) => gs.num_players;
            _initPlayers();
        } else {
            if (match_state == Match_Replay) {
                if (replay_ks.more()) {
                    replay_current_keys.clear(); // clear current key cache

                    // update keys pressed
                    strtok.set( replay_ks.readLine().trim() );
                    while (strtok.more()) {
                        replay_current_keys << strtok.next().toInt();
                    }
                }
            }

            // update players
            for (int i; i < gs.num_players; i++) {
                gs.players[i].update(dt);
            }

            processTrailCollisions(); // AFTER all players have moved

            // test if outside border
            for (auto e : gs.players) {
                if (e.disabled) continue;

                for (-1 => int i; i < border_vertices.size() -1; i++) {
                    e.player_pos.x > border_max.x
                    || 
                    e.player_pos.y > border_max.y
                    || 
                    e.player_pos.x < border_min.x
                    ||
                    e.player_pos.y < border_min.y => int outside_border;
                    if (outside_border) {
                        e.die();
                    }
                }
            }

            // check end game condition
            0 => int num_alive;
            for (auto e : gs.players) {
                if (!e.disabled) {
                    num_alive++;
                    e @=> winner;
                }
            }
            if (num_alive <= 1) {
                (num_alive == 0) => is_draw;

                // if exiting a replay, cleanup
                if (match_state == Match_Replay) {
                    replay_ks.close();
                    replay_mic_gain.close();
                }

                // transition state
                Match_Select => match_state;
            }
        }
    }

    fun void draw() {
        // draw border
        g.pushColor(Color.WHITE);
        g.box(border_min, border_max);
        g.popColor();

        // draw sound meter
        progressBar(gs.mic_volume, @(0, .9), .3, .05, Color.WHITE);

        // draw players
        for (int i; i < gs.num_players; i++) {
            gs.players[i].draw();
        }

        if (match_state == Match_Select) {
            1 => float text_y_offset;
            // display text options
            g.pushFontSize(.69);
            if (is_draw) {
                g.text("IT'S A DRAW!!", @(0, text_y_offset));
                1 -=> text_y_offset;
            } else if (winner != null) {
                g.text("Winner: Player " + (winner.which_player), @(0, text_y_offset));
                1 -=> text_y_offset;
            }
            g.text("Number of Players: " + gs.num_players, @(0, text_y_offset));
            1 -=> text_y_offset;
            g.text("Press <Space> to Restart", @(0, text_y_offset));
            1 -=> text_y_offset;
            g.text("Press <R> to Watch Replay", @(0, text_y_offset));
        }

    }

// ========================== Internal ==============================

    // TODO: rather than spork this, do it in the match_room update loop
    // so we guarantee frame-accuracy
    fun void record() {
        adc => WvOut w => blackhole;
        gs.replay_audio_filename => w.wavFilename;

        // open for write (default mode: ASCII)
        replay_ks.open( gs.replay_ks_filename, FileIO.WRITE );
        replay_mic_gain.open( gs.replay_gain_filename, FileIO.WRITE );

        // test
        if( !replay_ks.good() ) {
            cherr <= "can't open file for writing..." <= replay_ks.filename() <= IO.newline();
            return;
        }

        if( !replay_mic_gain.good() ) {
            cherr <= "can't open file for writing..." <= replay_mic_gain.filename() <= IO.newline();
            return;
        }

        // print
        while (match_state == Match_Running) {
            GG.nextFrame() => now;  // THIS GOES FIRST
            // write all player keys being held this frame
            GWindow.keys() @=> int keys_held[];
            for (auto key : keys_held) {
                replay_ks <= key <= " ";
            }
            replay_ks <= IO.newline();
            replay_mic_gain <= gs.mic_volume <= IO.newline();
        }

        // close the thing
        replay_ks.close();
        replay_mic_gain.close();
        w.closeFile();
        adc =< w;
    }

    // pos in [-1, 1] is NDC
    // width and height are [0, 1], relative to screen dimensions
    fun void progressBar(float percentage, vec2 pos_ndc, float width_ndc, float height_ndc, vec3 color) {
        g.NDCToWorldPos(pos_ndc.x, pos_ndc.y) => vec2 pos;
        g.NDCToWorldPos(width_ndc, height_ndc) => vec2 hw_hh;
        hw_hh.x => float hw;
        hw_hh.y => float hh;
        // shift pos.x to center
        // hw +=> pos.x;

        g.box(pos, 2 * hw, 2 * hh, color);

        -hw + (percentage * 2 * hw) => float end_x;
        g.boxFilled(
            pos - @(hw, hh),   // bot left
            pos + @(end_x, hh), // top right
            Color.WHITE
        );
    }

    // initializes player data *excluding* trail history
    fun void _initPlayers() {
        // init player entities
        for (int i; i < gs.players.size(); i++) {
            gs.players[i] @=> Entity@ e;
            i => e.which_player;
            gs.player_colors[i] => e.color;
            gs.player_spawns[i] => e.player_pos;
            M.DEG2RAD * gs.player_rots_deg[i] => e.player_rot;
            gs.player_spawns[i] => e.player_prev_pos;
            0 => e.player_pos_history_write_idx;
            0 => e.player_pos_history_read_idx;
            0 => e.frame_alive_count;
            0 => e.last_speed;

            (i >= gs.num_players) => e.disabled;
        }
    }

    fun void _checkAllPlayersAgainstSegment(vec2 p0, vec2 p1) {
        for (auto e : gs.players) {
            if (e.disabled) continue;

            if (M.intersect(
                e.player_prev_pos, e.player_pos, 
                p0, p1
            )) {
                e.die();
            }
        }
    }

    // call AFTER all player updates have finished
    fun void processTrailCollisions() {
        gs.players[0].player_pos_history.size() => int history_size;
        for (int i; i < gs.num_players; i++) {
            gs.players[i] @=> Entity this_player;

            // don't collide with a newly spawned player
            if (this_player.frame_alive_count < history_size) continue;

            this_player.player_pos_history_read_idx => int start_idx;
            start_idx - 3 => int end_idx; // ignore most recent trail segments to avoid self-collision
            if (end_idx < 0) history_size +=> end_idx;

            if (start_idx > end_idx) {
                for (start_idx => int i; i < history_size - 1; i++) {
                    _checkAllPlayersAgainstSegment(
                        this_player.player_pos_history[i], this_player.player_pos_history[i + 1]
                    );
                }
                for (0 => int i; i <= end_idx; i++) {
                    _checkAllPlayersAgainstSegment(
                        this_player.player_pos_history[i-1], this_player.player_pos_history[i]
                    );
                }
            } else {
                for (start_idx => int i; i < end_idx; i++) {
                    _checkAllPlayersAgainstSegment(
                        this_player.player_pos_history[i], this_player.player_pos_history[i + 1]
                    );
                }
            }
        }
    }
}

// init
StartRoom start_room;
MatchRoom match_room;
gs.enterRoom(match_room);

// gameloop
while (1) {
    GG.nextFrame() => now;
    GG.dt() => float dt;

    gs.room.update(dt);
    gs.room.draw();

    // { // UI
    //     if (UI.begin("test")) {
    //         // TODO: UI Library ideas
    //         // - show waveform history of a UGen
    //         // - show amplitude plot of a UGen last
    //         gs.mic_volume => env_pol_last.val;
    //         UI.slider("Mic Low Cut", env_low_cut, 0.00, 1.);
    //         UI.slider("Mic Exponent", env_exp, 0.00, 1.);
    //         if (UI.slider("Mic Pole", env_pole_pos, 0.95, 1.)) env_pole_pos.val() => env_follower.pole;
    //         UI.slider("Mic Scaled Volume", env_pol_last, 0.00, 1.);

    //         UI.slider("Speed-Volume Scale", gs.player_speed_volume_scale, 1.00, 4.);
    //     }
    //     UI.end();
    // }
}
