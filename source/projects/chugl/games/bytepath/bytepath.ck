/*
TODO
- remove radius from b2_polygon shader (simplify overall)

- IF rocks stay in game: change rock to dynamicBody, apply initial impulse to left or right.
    - prevents rocks from overlapping accidentally
    - need to use contactEvent instead of sensorEvent (remember to enable contact events!!!)

- Maybe: combine projectile, enemy, and pickup into single EntityPool

- Mode Henri: MAKES BULLETS PIERCING!

=====================
Simplification of Architecture
=====================
Entity superstruct (rfleury style)
- entity flags: Player, Pickup, Projectile, Enemy
    - these correspond to b2Filter categories
- entity_id *is* b2body_id
- hashmap from b2body_id --> Entity@

=====================
Shower Thoughts 1/31
=====================
- combo meter (deactivates after N seconds, every kill increments)
    - increasing combo meter increases BPM, reverb, damage?
    - resetting combo moves you to different area in topograph
- add pickup bubbles for more tracks
    - random sustained vocal
    - harmony pad (lazer that rotates around you? look to SNKRX / Bytepath / VampireSurivors/ FLERP / Brotato for weapon idea inspos)
    - synth lead (blit, hnkeytnk or 303)
    - maybe percussion can be a short-ranged melee attack
- by default the other tracks don't exist
    - picking up a powerup bubble for that track:
        - adds time to a second timer bar (beneath the ammo bar)
    - every instrument has a color, and powerup bubble is that color (minimize text usage)
- synthesize 808 or 909 kick
- rhythm:
    - when no weapons have ammo, default to 4-on-the-floor kick
    - have dodge-teleport with iframes if you dodge on beat
- try to come up with a different type of attack for each track
- Transparency UGH would be cool to mimic SNRKX explode AOE visual
- all spawning (enemy, pickup) happen according to poisson distribution by director
- Implement 16 waves of increasing difficulty. Each wave lasts for N *beats* (not time)
    - increasing combo increases BPM and therefore can clear the wave faster?
    - for now just do time maybe
- cool: LPF or slow down soundtrack upon taking damage
    - start with LPF for now
- other enemy types should somehow contribute to the music? Or act according to musical timing
- maybe music only plays if detecting enemy within range?

=====================
Playtest 
=====================
- crash on enemyPool and textPool out of size
    - add dynamic growth on pools
    - should really impl garbage collection sigh...
- ammo allocation and powerup timer seem to collide, they serve the same function really...
    - what if ammo pickup was EXP instead and we had leveling system?
- music too stagnant, needs to change over time gradually
- movement is super frustrating, much prefer the protato style can freely move / stop
    - rn movement is the least fun part
- imagining some kind of roll + powerup mechanic...
    - important: the music musn't ever stop!
    - somehow reward player for being on beat...maybe they need to pick the upgrade on the beat?
    - is there a way to somehow incorporate beat/rhythm instead of shop rerolls? 
        - like ae327x frontloaded power scaling... all we need is a way to randomize the upgrade path
        - the shop does NOT take player out of music, keeps the flow
        - the mechanic somehow rewards player for being in-sync with music...

Path A: remove pickups (except health), keep ammo pickup that you can allocate to a track
- but then how do you get exp to level?
    - maybe no exp, you just level at the end of the wave...
    - better: level every 16 beats
    - q: how to select which upgrade?
        - defer answer, for now just pick a random one and show text above player
- upgrades to unlock tracks, increase ammo capacity on ship, increase ammo capacity of track
- upgrades appear as pickups
    - 3 pickups that show text above
    - dotted lines between all pickups
    - they pulse with the beat
    - moving/sliding dotted line between your ship and the pickup when you get within range
    - if you pickup one of them by moving, the others disappear
    - press space on beat to instantly teleport to the pickup, NOT making the others disappear
        - this option available when the upgrade is within your autolock range
        - show's moving dashed line to upgrade, same color as upgrade color
    - upgrades spawn in random location for now, statically (no moving across screen, simpler)
        - but then how do they time out?
        - okay try spawning them moving across screen, maybe north/south instead

*/

// import materials for 2D drawing
@import "b2.ck"
@import "topograph.ck"

GG.logLevel(GG.LogLevel_Debug);

GG.camera().orthographic();
GG.camera().viewSize(5);

// set target resolution
// most steam games (over half) default to 1920x1080
// 1920x1080 / 4 = 480x270
GG.renderPass().resolution(480, 270);
GG.hudPass().resolution(480, 270);

// set MSAA
GG.renderPass().msaa(1);

// pixelate the output
TextureSampler output_sampler;
TextureSampler.Filter_Nearest => output_sampler.filterMin;  
TextureSampler.Filter_Nearest => output_sampler.filterMag;  
TextureSampler.Filter_Nearest => output_sampler.filterMip;  
GG.outputPass().sampler(output_sampler);

// TODO: fix the screenpass stuff
// 2D Vector Graphics
b2DebugDraw_Circles circles --> GG.scene();
b2DebugDraw_Lines lines --> GG.scene();
b2DebugDraw_SolidPolygon polygons --> GG.scene();
circles.antialias(false);

// font
GText.defaultFont(me.dir() + "./m5x7.ttf");

// Globals ============================================
class G
{
    static dur dt;
    static float dtf; // equivalent to G.dt, but cast as float
    1.0 => static float dt_rate; // modifier for time stretch
    static time dt_cum;      // cumulative dt (equiv to time-stretched "now")
    static float dtf_cum;    // cumulative dtf

    static GCamera@ camera;

    static HashMap@ pickup_map; // hashmap from pickup b2bodyid --> pickup type

    // Music Timings
    120.0 => static float BPM;
    (60.0 / BPM)::second => static dur qt_note;
    64 => static int BEATS_PER_WAVE;
    16 => static int WAVES_PER_GAME;
    32 => static int BEATS_PER_UPGRADE;
    static int beat_count;  // incremented every qt_note
    static int step_count;  // inc every 32nd note (8 times per beat)
    static int wave_count;  // Q: are waves somehow related to beats/steps/phrases/patterns?
    static Event beat_event;
    static time last_beat_time;
    static time next_beat_time;

    // Enemy stuff
    static Enemy@ closest_enemy;
    static float  closest_enemy_dist;

    // Upgrades
    static int active_upgrades[0]; // body_ids of upgrades currently on screen
    static int closest_upgrade_body_id;
    static float closest_upgrade_dist;

    // Color Palette
    Color.hex(0x7F00FF) => static vec3 Color_Kick;
    Color.hex(0x00ffff) => static vec3 Color_Snare;
    Color.hex(0x00FF00) => static vec3 Color_Hat;
    Color.hex(0x808080) => static vec3 Color_Locked;
}

GG.camera() @=> G.camera;
new HashMap @=> G.pickup_map;

// Utility fns
fun int offscreen(vec2 pos, float threshold) {
    G.camera.worldPosToNDC(@(pos.x, pos.y, 0)) => vec3 pos_ndc;
    return (Math.fabs(pos_ndc.x) > threshold || Math.fabs(pos_ndc.y) > threshold);
}

fun int offscreen(vec2 pos) {
    return offscreen(pos, 1.0);
}


fun vec2 NDCToWorldPos(float x, float y) {
    G.camera.NDCToWorldPos(@(x, y, 0)) => vec3 world_pos;
    return world_pos $ vec2;
}

fun vec2 NDCToWorldPos(vec2 ndc) {
    G.camera.NDCToWorldPos(@(ndc.x, ndc.y, 0)) => vec3 world_pos;
    return world_pos $ vec2;
}

fun vec3 NDCToWorldPos(float x, float y, float depth) {
    G.camera.NDCToWorldPos(@(x, y, 0)) => vec3 world_pos;
    depth => world_pos.z;
    return world_pos;
}

// time-scaled wait (respects G.d_rate time stretch)
fun int wait(dur d) {
    G.dt_cum + d => time target;
    while (G.dt_cum < target) {
        8::ms => now; // approx 120fps resolution
    }
    return true;
}

// ==============================
// Upgrades
// ==============================
// - Upgrades (simplest is non-unique upgrades that always scale like max HP...)
//     - PickupType_XXX Spawn frequency
//     - (if level 0) Unlock PickupType_XXX (equivalent to going from pickup spawn frequency level 0 --> level 1)
//     - max health
//     - ammo max capacity
//     - track/weapon max capacity
//     - player movespeed / tempo ??? 
//     - track/weapon damage
//     - track density
//     - increase ship auto-lock range
//     - increase ship steering rate

// Side enum
// -1 == spawn side right, go left
// 1 == spawn side left, go right
@(1,0) => vec2 Side_Right;
@(-1,0) => vec2 Side_Left;
@(0, 1) => vec2 Side_Top;
@(0, -1) => vec2 Side_Bottom;

Pickup pickup_types[0];
class Pickup
{
    string desc;
    vec3 color;
    int unique;
    int is_upgrade;

    fun static Pickup@ add(string desc, vec3 color, int unique, int is_upgrade) {
        Pickup p;
        desc => p.desc;
        color => p.color;
        unique => p.unique;
        is_upgrade => p.is_upgrade;

        pickup_types << p;
        return p;
    }

    // @param from_teleport: true if player zapped there from being on beat.
    fun static void activate(int pickup_id, int from_teleport) {
        b2Body.position(pickup_id) => vec2 pickup_pos;
        // switch on pickup type
        (G.pickup_map.getObj(pickup_id) $ Pickup) @=> Pickup pickup_type;
        if (pickup_type == Pickup_Ammo) {
            // pickup effect
            spork ~ rippleEffect(pickup_pos);
            if (!player.addAmmo(1)) {
                spork ~ textEffect(
                    "AMMO FULL",
                    M.randomPointInCircle(pickup_pos, .25, .35),
                    pickup_type.color
                );
            }
        } else { // all other pickup types
            spork ~ pickupEffect(pickup_pos, pickup_type);
            spork ~ textEffect(
                pickup_type.desc,
                M.randomPointInCircle(pickup_pos, .25, .35),
                pickup_type.color
            );

            // pickup logic
            if (pickup_type == Pickup_Upgrade_AmmoCap) {
                10 +=> player.ammo_max;
            } else if (pickup_type == Pickup_Upgrade_LockOnSpeed) {
                M.clamp01(player.dev_auto_lock_rotate_rate.val() + .01) => player.dev_auto_lock_rotate_rate.val;
            } else if (pickup_type == Pickup_Upgrade_ShipRange) {
                player.dev_auto_lock_range.val() + .2 => player.dev_auto_lock_range.val;
            } else {
                T.err("Unsupported Pickup Type " + pickup_type.desc);
            }
        }

        { // cleanup

            // if player got the upgrade by walking and *not* being on beat, destroy all others
            if (pickup_type.is_upgrade && !from_teleport) {
                for (auto upgrade_id : G.active_upgrades) {
                    b2.destroyBody(upgrade_id);
                }
                G.active_upgrades.clear();
            } else {
                // just destroy this one upgrade
                b2.destroyBody(pickup_id);
            }
        }

    }

    fun static void spawnUpgrades() {
        // TODO randomize
        // should upgrades always come in from different directions?
        [Side_Left, Side_Right, Side_Bottom, Side_Top] @=> vec2 sides[];
        sides.shuffle();
        spork ~ spawn(sides[0], Pickup_Upgrade_AmmoCap);
        spork ~ spawn(sides[1], Pickup_Upgrade_LockOnSpeed);
        spork ~ spawn(sides[2], Pickup_Upgrade_ShipRange);
    }

    // TODO make into pool...
    fun static void spawn(vec2 which_side, Pickup pickup_type) 
    {
        GG.nextFrame() => now; // to register as graphics shred

        null @=> GText text;
        if (pickup_type.is_upgrade) {
            text_pool.get() @=> text;
            .4 => text.sca;
        }

        // params
        Math.random2f(.16, .24) => float speed;
        Math.random2f(2.5, 3.5) => float pulse_freq; // for pulsing the health orb
        pickup_type.color => vec3 color;

        int body_id;

        { // create b2 body
            b2BodyDef pickup_body_def;
            b2BodyType.kinematicBody => pickup_body_def.type;

            // pickups that rotate while moving across screen
            if (pickup_type != Pickup_Health) {
                Math.random2f(1, 2) => float angular_velocity;
                angular_velocity => pickup_body_def.angularVelocity;
            }

            Math.random2f(-.66, .66) => float rand_pos;
            which_side => vec2 spawn_pos_ndc;
            if (which_side == Side_Left || which_side == Side_Right) {
                rand_pos +=> spawn_pos_ndc.y;
            } else if (which_side == Side_Top || which_side == Side_Bottom) {
                rand_pos +=> spawn_pos_ndc.x;
            }
            NDCToWorldPos(spawn_pos_ndc) => pickup_body_def.position;
            -speed * which_side => pickup_body_def.linearVelocity;

            false => pickup_body_def.enableSleep; // disable otherwise slowly rotating objects will be put to sleep

            b2Filter pickup_filter;
            Category_Pickup => pickup_filter.categoryBits;
            Category_Player => pickup_filter.maskBits;

            b2ShapeDef pickup_shape_def;
            true => pickup_shape_def.isSensor;
            pickup_filter @=> pickup_shape_def.filter;

            b2.createBody(world_id, pickup_body_def) => body_id;
            T.assert(b2Body.isValid(body_id), "boost pickup b2body invalid");

            // physics shape
            if (pickup_type == Pickup_Health) {
                b2.createCircleShape(body_id, pickup_shape_def, new b2Circle(pickup_scale * .5));
            }  else {
                b2.makeBox(pickup_scale, pickup_scale) @=> b2Polygon polygon;
                b2.createPolygonShape(body_id, pickup_shape_def, polygon);
            }
        }

        // register body id in LUT
        G.pickup_map.set(body_id, pickup_type);

        // if upgrade, register in globals
        if (pickup_type.is_upgrade) {
            G.active_upgrades << body_id;
        }

        // draw until destroyed
        while (true) {
            GG.nextFrame() => now;

            // if destroyed, break out
            if (!b2Body.isValid(body_id)) break;

            // calculate transform
            b2Body.position(body_id) => vec2 pos;
            b2Body.angle(body_id) => float rot_radians;

            // if its off the screen, destroy and break
            G.camera.worldPosToNDC(@(pos.x, pos.y, 0)) => vec3 pos_ndc;
            if (Math.fabs(pos_ndc.x) > 1 || Math.fabs(pos_ndc.y) > 1) {
                b2.destroyBody(body_id);
                break;
            }

            { // draw
                if (pickup_type == Pickup_Health) {
                    pickup_scale * .6 => float w; // center size
                    1 + .12 * Math.sin(G.dtf_cum * pulse_freq) *=> w; // oscillate!

                    polygons.box(pos, 0, w, w * .33, color);
                    polygons.box(pos, .5 * Math.pi, w, w * .33, color);
                    lines.circle(pos, w, Color.hex(0xFFFFFF));
                } else {
                    polygons.drawSquare(pos, rot_radians, pickup_scale * .4, color);
                    lines.square(pos, rot_radians, pickup_scale, color);
                }

                if (pickup_type.is_upgrade) {
                    T.assert(text != null, "upgrade doesn't have GText");
                }

                // upgrade text
                if (text != null) {
                    pickup_type.desc => text.text;
                    text.pos(pos + @(0, pickup_scale));
                    T.assert(text.text() == pickup_type.desc, "upgrade text doesn't match desc");
                }
            }
        }

        { // cleanup
            // remove from pick up map
            G.pickup_map.del(body_id);
            <<< "pickup map size: ", G.pickup_map.size() >>>;

            if (text != null) {
                text_pool.ret(text);
            }
        }
    }
}

// pickups
Pickup.add("+Ammo", Color.hex(0xFFFFFF), false, false) @=> Pickup Pickup_Ammo; 
Pickup.add("+Health", Color.hex(0xFF0000), false, false) @=> Pickup Pickup_Health; 
// upgrades
Pickup.add("+Max Health", Color.hex(0xFF0000), false, true) @=> Pickup Pickup_Upgrade_MaxHealth;  // max health
Pickup.add("+Ammo Cap", Color.hex(0xFFFFFF), false, true) @=> Pickup Pickup_Upgrade_AmmoCap;      // max ammo storage
Pickup.add("+Range", Color.hex(0xFFFFFF), false, true) @=> Pickup Pickup_Upgrade_ShipRange;       // range for auto-lock and upgrades
Pickup.add("+Lock-On Speed", Color.hex(0xFFFFFF), false, true) @=> Pickup Pickup_Upgrade_LockOnSpeed; // speed of rotation steering
Pickup.add("+Attract", Color.hex(0xFFFFFF), false, true) @=> Pickup Pickup_Upgrade_AmmoAttractSpeed; // speed of ammo coming to you

.25 => float pickup_scale;


// ========================================================================
// Attack Types
// ========================================================================

class Attack
{
    vec3 color;
    dur rate;
    string name;

    fun @construct(vec3 c, dur r, string name) {
        c => this.color;
        r => this.rate;
        name => this.name;
    }
}

[
    new Attack(Color.WHITE, .5::second, "Normal"), // Normal
    new Attack(Color.hex(0xff0000), .75::second, "Triple"), // Triple
    new Attack(Color.hex(0x00ffff), .2::second, "Rapid"), // Spread
    new Attack(Color.hex(0x32CD32), .75::second, "Side"),

] @=> Attack attacks[];

attacks[0] @=> Attack Attack_Normal;
attacks[1] @=> Attack Attack_Triple;
attacks[2] @=> Attack Attack_Spread;
attacks[3] @=> Attack Attack_Side;


// Math ============================================
class M
{
    fun static vec2 rot2vec(float radians) {
        return @( Math.cos(radians), Math.sin(radians) );
    }

    fun static vec2 randomDir() {
        return rot2vec(Math.random2f(0, Math.two_pi));
    }

    fun static float easeOutQuad(float x) {
        return 1 - (1 - x) * (1 - x);
    }

    fun static float easeInOutCubic(float x) {
        if (x < 0.5) 
            return 4 * x * x * x;
        else 
            return 1 - Math.pow(-2 * x + 2, 3) / 2;
    }

    fun static float lerp(float t, float a, float b) {
        return a + t * (b - a);
    }

    fun static float clamp01(float f) {
        return Math.clampf(f, 0, 1);
    }

    // returns ratio of x in range [a, b]
    fun static float invLerp(float x, float a, float b) {
        return (x - a) / (b - a);
    }

    // rotate dir v by `a` radians
    fun static vec2 rot(vec2 v, float a) {
        Math.cos(a) => float cos_a;
        Math.sin(a) => float sin_a;
        return @(
            cos_a * v.x - sin_a * v.y,
            sin_a * v.x + cos_a * v.y
        );
    }

    fun static vec2 normalize(vec2 n)
    {
        return n / Math.hypot(n.x, n.y); // hypot is the magnitude
    }

    fun static float magnitude(vec2 n) {
        return  Math.hypot(n.x, n.y); 
    }

    fun static float angle(vec2 n) {
        return Math.atan2(n.y, n.x);
    }

    // ---------------
    // random
    // ---------------
    fun static vec2 randomPointInCircle(vec2 center, float min_r, float max_r) {
        Math.random2f(0, Math.two_pi) => float theta;
        Math.random2f(min_r, max_r) => float radius;
        return center + radius * @(Math.cos(theta), Math.sin(theta));
    }


    fun static vec2 randomPointInArea(vec2 center, float w, float h) {
        return center + @(-w * 0.5, -h * 0.5) + @(
            Math.random2f(0, w),
            Math.random2f(0, h)
        );
    }

    // given an event happens once every `period_secs`, returns
    // the amount of time you need to wait until the next occurance.
    // Assumes the wait interval is 1 frame at 60fps
    // returns the amount of time in seconds until the next event occurs
    fun static float poisson(float period_sec) {
        // chance / chunk = period_secs
        // e.g. if you're checking every 10::ms and you want the 
        // event to happen every 1::second, then the check needs 
        // to only pass with probability 10::ms / 1::sec = 1%
        (1 / 1000.0) => float dt_sec;
        dt_sec / period_sec => float success_rate;

        if (success_rate <= 0) {
            T.assert(false, "poisson success_rate <= 0");
            return 0;
        }

        0 => int count;
        while (Math.randomf() > success_rate) count++;

        return count * dt_sec;
    }
}

// physics state setup ============================================

// collision category flags (for b2 physics)
(1 << 1) => int Category_Player;      // 2
(1 << 2) => int Category_Projectile;  // 4
(1 << 3) => int Category_Pickup;      // 8
(1 << 4) => int Category_Enemy;       // 16

b2WorldDef world_def;
@(0,0) => world_def.gravity;
b2.createWorld(world_def) => int world_id;
int begin_touch_events[0]; // for holding collision data
int begin_sensor_events[0]; // for holding sensor data

{ // simulation config (eventually group these into single function/command/struct)
    b2.world(world_id);
    b2.substeps(4);
}


// player ============================================

class Player
{
    // dev params
    UI_Bool dev_movement_type(false);
    UI_Float dev_auto_lock_rotate_rate(.05);
    UI_Float dev_auto_lock_range(1.0);
    UI_Float dev_steering_rate(.18);

    // TODO cache and store player rotation / position once per frame rather than recalculating constantly
    int body_id;
    int shape_id;
    float rotation;
    .1 => float radius;
    vec2 pos; // updated every frame
    vec2 movement_vec;
    vec2 movement_steering_vec;
    vec2 movement_desired_vec;

    UI_Bool invincible(false);
    UI_Bool invisible(false);

    Attack_Normal @=> Attack attack;

    1.0 => float velocity_scale;
    Color.WHITE => vec3 boost_trail_color;

    // HUD-related
    25 => int exp; // spendable
    50 => int ammo_max;
    // GText exp_text --> GG.scene();
    // NDCToWorldPos(0, 1, 0) =>  exp_text.pos;
    100 => float hp_max;
    hp_max => float hp; // spendable

    // Music params
    int kick_ammo;
    int snare_ammo;
    int hat_ammo;
    ammo_max => int kick_ammo_max;
    ammo_max => int snare_ammo_max;
    ammo_max => int hat_ammo_max;

    fun @construct() {
        // create body def
        // @(Math.random2f(-4.0, 4.0), Math.random2f(6.0, 12.0)) => kinematic_body_def.position;
        // Math.random2f(0.0,Math.two_pi) => float angle;
        // @(Math.cos(angle), Math.sin(angle)) => kinematic_body_def.rotation;
        b2BodyDef player_body_def;
        b2BodyType.dynamicBody => player_body_def.type; // must be dynamic to trigger sensor

        b2.createBody(world_id, player_body_def) => this.body_id;

        // then shape
        // TODO make player collider match the visual
        b2Filter player_filter;
        Category_Player => player_filter.categoryBits;
        Category_Pickup | Category_Enemy => player_filter.maskBits; // nothing

        b2ShapeDef shape_def;
        player_filter @=> shape_def.filter;

        b2Circle circle(this.radius);
        b2.createCircleShape(this.body_id, shape_def, circle) => this.shape_id;

        // listeners
        spork ~ beatListener();
    }

    fun void beatListener() {
        while (true) {
            G.beat_event => now;
            <<< "beat" >>>;
            // TODO: somehow visualize the beat, maybe with range indicator?
            spork ~ rippleEffect(pos);
        }
    }

    // returns the delta
    fun int addAmmo(int amt) {
        exp => int prev_exp;
        Math.clampi(exp + amt, 0, ammo_max) => exp;
        return exp - prev_exp;
    }

    // pos in [-1, 1] is NDC
    // width and height are [0, 1], relative to screen dimensions
    fun void progressBar(float curr, float max, vec2 pos, float width_ndc, float height_ndc, vec3 color) {
        NDCToWorldPos(pos.x, pos.y, 1) $ vec2  => pos;
        NDCToWorldPos(width_ndc, height_ndc) => vec2 hw_hh;
        hw_hh.x => float hw;
        hw_hh.y => float hh;
        // shift pos.x to center
        // hw +=> pos.x;

        lines.box(pos, 2 * hw, 2 * hh, color);

        curr / max => float progress;
        -hw + (progress * 2 * hw) => float end_x;
        polygons.box(
            pos - @(hw, hh), // top left
            pos + @(end_x, hh), // bot right
            color
        );
    }
    
    fun void updateHUD() {
        // exp meter
        .45 => float exp_bar_w;
        .025 => float exp_bar_h;
        @(.5, .9) => vec2 exp_bar_pos;
        progressBar(exp $ float, ammo_max, exp_bar_pos, exp_bar_w, exp_bar_h, Color.WHITE);

        // hp meter
        .45 => float hp_bar_hw;
        .025 => float hp_bar_hh;
        @(-.5, .9) => vec2 hp_bar_pos;
        progressBar(hp, hp_max, hp_bar_pos, hp_bar_hw, hp_bar_hh, Color.RED);

        // ammo meters
        .15 => float ammo_bar_w;
        .025 => float ammo_bar_h;
        @(-.75, -.9) => vec2 kick_ammo_pos;
        progressBar(kick_ammo $ float, kick_ammo_max, kick_ammo_pos, ammo_bar_w, ammo_bar_h, G.Color_Kick);

        kick_ammo_pos + @(2.1 * ammo_bar_w, 0) => vec2 snare_ammo_pos;
        progressBar(snare_ammo $ float, snare_ammo_max, snare_ammo_pos, ammo_bar_w, ammo_bar_h, G.Color_Snare);

        snare_ammo_pos + @(2.1 * ammo_bar_w, 0) => vec2 hat_ammo_pos;
        progressBar(hat_ammo $ float, hat_ammo_max, hat_ammo_pos, ammo_bar_w, ammo_bar_h, G.Color_Hat);
    }
}

Player player;

// Effects ==============================================================

/* Effects Architecture
To avoid the rabbit hole / performance sink of an EffectsManager +
Effect base class + overriding/constructors/etc we instead
have all effects be functions that can be sporked and combined
to create larger meta effects.

An effect is: a temporary shred that interpolates value(s) over time
Currently it both
- updates the interpolated value
- applies those values to update state / make draw commands

All the interpolated state in contained within the function body 
of the effect itself, although this may change (add pooling?) 
for performance reasons

So far have observed these different "types" of effects
- only single global instance allowed (e.g. cameraShake
and timeWarp)
    - for these, a single generation counter is sufficient
- any number if instances allowed, no external state for de-duping required
(e.g. bullet boom effect)
- multiple allowed, but each of these is named and must be deduped
within that name. Use a a hashmap<string name, int generation> for this

Rules:
- an effect ought to be sporked from a permanent graphics shred
- an effect *cannot* spork other effects, instead just spork the two
effects separately from the permanent graphics shred (this avoids bug
where the parent effect shred exits before child is finished)

Meta: follow philosophy of change/discipline/constrain your coding
style to fit within the constraints of simplicity and performance,
rather than complexifying the system (e.g. going the OOP/Effects 
command queue route) to accomodate a more flexible but less performant
programming style (e.g. one where effects can spawn other effects)
- to be seen if this is the right move
*/

int camera_shake_generation;
fun void cameraShakeEffect(float amplitude, dur shake_dur, float hz) {
    ++camera_shake_generation => int gen;
    dur elapsed_time;

    // generate shake params
    shake_dur / 1::second => float shake_dur_secs;
    vec2 camera_deltas[(hz * shake_dur_secs + 1) $ int];
    for (int i; i < camera_deltas.size(); i++) {
        @(Math.random2f(-amplitude, amplitude),
        Math.random2f(-amplitude, amplitude)) => camera_deltas[i];
    }
    @(0,0) => camera_deltas[0]; // start from original pos
    @(0,0) => camera_deltas[-2]; // return to original pos
    @(0,0) => camera_deltas[-1]; // return to original pos (yes need this one too)

    (1.0 / hz)::second => dur camera_delta_period; // time for 1 cycle of shake

    while (true) {
        GG.nextFrame() => now;
        // another shake triggred, stop this one
        if (elapsed_time > shake_dur || gen != camera_shake_generation) break;
        // update elapsed time
        G.dt +=> elapsed_time;

        // compute fraction shake progress
        elapsed_time / shake_dur => float progress;
        elapsed_time / camera_delta_period => float elapsed_periods;
        elapsed_periods $ int => int floor;
        elapsed_periods - floor => float fract;

        // clamp to end of camera_deltas
        if (floor + 1 >= camera_deltas.size()) {
            camera_deltas.size() - 2 => floor;
            1.0 => fract;
        }

        // interpolate the progress
        camera_deltas[floor] * (1.0 - fract) + camera_deltas[floor + 1] * fract => vec2 delta;
        // update camera pos with linear decay based on progress
        (1.0 - progress) * delta => G.camera.pos;
    }
}

// shrinking white square on blaster end
fun void shootEffect(dur tween_dur) {
    dur elapsed_time;

    while (elapsed_time < tween_dur) {
        GG.nextFrame() => now;
        G.dt +=> elapsed_time;

        elapsed_time / tween_dur => float t;
        .1 * (1 - t * t) => float shoot_effect_scale;
        polygons.drawSquare(
            player.pos + player.radius * M.rot2vec(player.rotation),
            player.rotation, // rotation
            shoot_effect_scale,
            Color.WHITE
        );
    }
}

fun void boomEffect(vec2 pos, float scale) {
    dur elapsed_time;
    while (elapsed_time < .28::second) {
        GG.nextFrame() => now;
        G.dt +=> elapsed_time;

        if (elapsed_time < .12::second) 
            polygons.drawSquare(
                pos,
                0, // rotation
                scale,
                Color.WHITE
            );
        else 
            polygons.drawSquare(
                pos,
                0, // rotation
                scale,
                Color.RED
            );
    }
}

// slows time to rate, and over d ramps back to normal speed
int slow_effect_generation;
fun void slowEffect(float rate, dur d) {
    ++slow_effect_generation => int gen;
    dur elapsed_time;

    while (gen == slow_effect_generation) {
        GG.nextFrame() => now;
        // must use GG.dt() so it's not affected by it's own time warp!
        GG.dt()::second +=> elapsed_time; 

        if (elapsed_time > d) break;

        M.lerp(M.easeInOutCubic(elapsed_time / d), rate, 1.0) => float t;

        // adjust animation rate
        t => G.dt_rate;
        // adjust physics rate
        t => b2.rate;

        T.assert(G.dt_rate <= 1.0, "dt_rate exceeds bounds");
        if (G.dt_rate > 1.0) {
            <<< "rate",  G.dt_rate, "t", t, "d", d, "rate", rate >>>;
        }
    }

    // restore to normal rate
    if (gen == slow_effect_generation) {
        1.0 => G.dt_rate;
        1.0 => b2.rate;
    }
}

// spawns an explosion of lines going in random directinos that gradually shorten
fun void explodeEffect(vec2 pos, dur max_dur) {
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
        G.dt +=> elapsed_time;

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
                lines.drawSegment(positions[i], positions[i] + len * dir[i]);
            }
        }
    }
}

// NOTE: not affected by time warp
int screen_flash_effect_generation;
fun void screenFlashEffect(float hz, dur duration) {
    // <<< "screen flash EFfect" >>>;
    ++screen_flash_effect_generation => int gen;

    (1.0/hz)::second => dur period;

    dur elapsed_time;
    int toggle;
    while (elapsed_time < duration && gen == screen_flash_effect_generation) {
        // <<< "screen flash", toggle >>>;
        if (toggle) {
            GG.outputPass().exposure(1.0);
            // <<< "1 exposure" >>>;
        } else {
            GG.outputPass().exposure(0.0);
            // <<< "0 exposure" >>>;
        }
        1 - toggle => toggle;

        // pass time
        period => now;
        period +=> elapsed_time;
    }

    // restore
    if (gen == screen_flash_effect_generation) {
        GG.outputPass().exposure(1.0);
    }
}


int refresh_effect_generation;
// white box that slides up across player (for power up)
fun void refreshEffect() {
    ++refresh_effect_generation => int gen;
    .1::second => dur effect_dur;

    dur elapsed_time;

    player.radius * 4 => float width;

    while (elapsed_time < effect_dur && gen == refresh_effect_generation) {
        GG.nextFrame() => now;
        G.dt +=> elapsed_time;

        (elapsed_time / effect_dur) => float t;

        (width * (1 - t)) => float height;
        (width * 0.5) - height / 2 => float y_offset;

        polygons.box(
            player.pos + @(0, y_offset),
            0, // rotation
            width,
            height,
            Color.WHITE 
        );
    }

}

// flame trail from ship
// multiple allowed
// gradually shrinking/fading circle
// ==optimize== group all trails into circular buffer and pool, 1 shred
fun void boostTrailEffect(vec2 pos, vec3 color, float radius_scale) {
    radius_scale * Math.random2f(.04, .046) => float init_radius;
    init_radius * 8 * 1::second => dur effect_dur; // time to dissolve

    Math.random2f(0.8, 1.0) * color => vec3 init_color;

    dur elapsed_time;
    while (elapsed_time < effect_dur) {
        GG.nextFrame() => now;
        G.dt +=> elapsed_time;
        1.0 - elapsed_time / effect_dur => float t;

        circles.drawCircle(pos, init_radius * t, 1.0, t * t * init_color);
    }
}

// slowly expanding ring
fun void rippleEffect(vec2 pos) {
    .5 => float end_radius;
    .5::second => dur effect_dur;

    dur elapsed_time;
    while (elapsed_time < effect_dur) {
        GG.nextFrame() => now;
        G.dt +=> elapsed_time;
        M.easeOutQuad(elapsed_time / effect_dur) => float t;

        circles.drawCircle(pos, end_radius * t, .1 * (1 - t), Color.WHITE * (1 - t));
    }
}

// toggles a UI_Bool at hz freq n times after initial wait
fun void blinkEffect(UI_Bool b, float hz, dur d, dur init_wait) {
    wait(init_wait);
    (1 / hz)::second => dur T;
    dur elapsed_time;
    while (elapsed_time < d) {
        wait(T);
        T +=> elapsed_time;
        (1 - b.val()) => b.val;
    }
}

// known bug: doesn't dedup by hitFlash generation number
// would need a hashmap keyed on some unique identifier, so that say multiple rapid hits on a rock don't override each other
fun void hitFlashEffect(UI_Bool b) {
    true => b.val;
    wait(.2::second);
    false => b.val;
}

int invincible_effect_gen;
fun void invincibleEffect() {
    ++invincible_effect_gen => int gen;
    true => player.invincible.val;
    wait(2::second);
    if (invincible_effect_gen == gen) {
        false => player.invincible.val;
    }
}

int invisible_effect_gen;
fun void invisibleEffect() {
    ++invisible_effect_gen => int gen;
    dur elapsed_time;
    while (elapsed_time < 2::second && gen == invisible_effect_gen) {
        wait(.08::second);
        .08::second +=> elapsed_time;
        1 - player.invisible.val() => player.invisible.val;
    }

    if (gen == invisible_effect_gen) {
        false => player.invisible.val;
    }
}

// multiple parts
// 1. color changes from white --> blue after .2 sec
// 2. after x secs, starts flashing (toggling draw) every .05 seconds 6 times
// 3. outer ring scale from 1 --> 2 over .35 sec
fun void pickupEffect(vec2 pos, Pickup pickup_type) {
    // params
    UI_Bool draw(true);
    Color.WHITE => vec3 color;
    1 => float ring_scale;

    (.2 + .6)::second => dur effect_dur;
    dur elapsed_time;
    spork ~ blinkEffect(draw, 20, 1::second, .2::second);

    while (true) {
        GG.nextFrame() => now;
        G.dt +=> elapsed_time;
        if (elapsed_time > effect_dur) break;

        // after .2 secs
        if (elapsed_time > .2::second) {
            // change color
            if (elapsed_time > .2::second)
                pickup_type.color => color;

            // scale outer ring over .35 seconds
            ((elapsed_time - .2::second) / .6::second) => float t;
            1 + t * t => ring_scale;
        }

        if (draw.val()) { // draw
            if (pickup_type == Pickup_Health) {
                pickup_scale * .6 => float w; // center size
                polygons.box(pos, 0, w, w * .33, color);
                polygons.box(pos, .5 * Math.pi, w, w * .33, color);
                lines.circle(pos, ring_scale * w, Color.hex(0xFFFFFF));
            } else {
                polygons.drawSquare(pos, Math.pi * .25, pickup_scale * .4, color);
                lines.square(pos, Math.pi * .25, ring_scale * pickup_scale, color);
            }
            
        }
    }
}

// TODO: add text garbage collection to ChuGL....
// using pool for now to prevenet leak
class TextPool 
{
    GText text_pool[256]; // shouldn't need more than this?
    0 => int num_active;  // number of text from text_pool actively used

    // init GText
    for (int i; i < text_pool.size(); i++) {
        text_pool[i].text("");
        text_pool[i] --> GG.scene();
        text_pool[i].antialias(false);
    }

    fun GText get() {
        if (num_active >= text_pool.size()) {
            T.assert(false, "insufficient GText in TextPool");
            return null;
        } 

        num_active++;
        return text_pool[num_active - 1];
    }

    fun void ret(GText text) {
        // ==optimize== linear search to hashmap

        for (int i; i < num_active; i++) {
            // return and swap with last active
            if (text_pool[i] == text) {
                text_pool[num_active - 1] @=> text_pool[i];
                text @=> text_pool[num_active - 1];
                text.text("");
                num_active--;
                return;
            }
        }
        T.assert(false, "GText not found in pool");
    }
}
TextPool text_pool;

fun void textEffect(string text_str, vec2 pos, vec3 color) {
    textEffect(text_str, pos, color, .8::second);
}

fun void textEffect(string text_str, vec2 pos, vec3 color, dur d) {
    text_pool.get() @=> GText text;

    text.color(color);
    text.pos(pos);
    text.sca(.6);
    text.text(text_str);

    UI_Bool draw(true);
    spork ~ blinkEffect(draw, 20, d, .2::second);

    dur elapsed_time;
    while (true) {
        GG.nextFrame() => now;
        G.dt +=> elapsed_time;
        if (elapsed_time > d) break;

        text.alpha(draw.val());
    }

    text_pool.ret(text);
}


// ==optimize== group all under projectile pool if there are ever enough to matter
fun void spawnPickupAmmo(vec2 pos) {
    GG.nextFrame() => now; // to register as graphics shred

    // params
    .1 => float w;
    .1 => float h;
    .12 => float speed;
    .08 => float steering_rate;
    1 => float angular_velocity;

    b2BodyDef pickup_body_def;
    b2BodyType.kinematicBody => pickup_body_def.type;
    angular_velocity => pickup_body_def.angularVelocity;
    pos => pickup_body_def.position;
    false => pickup_body_def.enableSleep; // disable otherwise slowly rotating objects will be put to sleep

    b2Filter pickup_filter;
    Category_Pickup => pickup_filter.categoryBits;
    Category_Player => pickup_filter.maskBits;

    b2ShapeDef pickup_shape_def;
	/// A sensor shape generates overlap events but never generates a collision response.
    true => pickup_shape_def.isSensor; // note: sensors only create events with dynamic bodies?
    pickup_filter @=> pickup_shape_def.filter;

    b2.createBody(world_id, pickup_body_def) => int body_id;
    T.assert(b2Body.isValid(body_id), "spawnPickup body invalid");
    b2.makeBox(w, h) @=> b2Polygon polygon;
    b2.createPolygonShape(body_id, pickup_shape_def, polygon);

    // register body id in LUT
    G.pickup_map.set(body_id, Pickup_Ammo);

    polygon.vertices() @=> vec2 vertices[];

    // draw until destroyed
    while (true) {
        GG.nextFrame() => now;

        // if destroyed, break out
        if (!b2Body.isValid(body_id)) break;

        // calculate transform
        b2Body.position(body_id) => vec2 pos;
        b2Body.angle(body_id) => float rot_radians;

        // steering towards player (if player is alive)
        // https://code.tutsplus.com/understanding-steering-behaviors-seek--gamedev-849t
        if (b2Body.isValid(player.body_id)) {
            b2Body.linearVelocity(body_id) => vec2 current_heading;
            // lines.drawSegment(pos, pos + current_heading); // debug draw
            M.normalize(player.pos - pos) => vec2 desired_heading;
            desired_heading - current_heading => vec2 steering;
            b2Body.linearVelocity(
                body_id, 
                speed * M.normalize(current_heading + steering_rate * steering)
                // desired_heading // uncomment for no steering, instant course correction
            );
        }

        // <<< rot_radians , "velocity: ", b2Body.angularVelocity(body_id), b2Body.isAwake(body_id) >>>;
        // <<< "sleep enabled", b2Body.isSleepEnabled(body_id), "sleep threshold", b2Body.sleepThreshold(body_id) >>>;

        // TODO add outline width option to polygon drawer
        lines.drawPolygon(
            pos,
            rot_radians,
            vertices
        );
    }

    { // cleanup
        // remove from pick up map
        G.pickup_map.del(body_id);
        // <<< "pickup map size: ", G.pickup_map.size() >>>;
    }
} 

// ========================================================================
// Shoot System
// ========================================================================

// projectile b2 definitions
b2Filter projectile_filter;
Category_Projectile => projectile_filter.categoryBits;
Category_Enemy => projectile_filter.maskBits; // collides with enemy

b2ShapeDef projectile_shape_def;
projectile_filter @=> projectile_shape_def.filter;

b2BodyDef projectile_body_def;
// b2BodyType.kinematicBody => projectile_body_def.type;
b2BodyType.dynamicBody => projectile_body_def.type;
// true => projectile_body_def.isBullet;

class Projectile 
{
    int body_id;
    int shape_id; // for disabling sensor events
    int pool_index;
    Attack attack;
    vec2 dir;
    float dir_radians;
}

class ProjectilePool
{
    Projectile projectiles[0];
    HashMap projectile_map;

    int num_active;

    .025 => float radius;
    4.2 => float speed;
    // 1 => float speed;

    // debug
    int prev_active;

    fun void ret(int projectile_body_id) {
        ret(projectile_map.getObj(projectile_body_id) $ Projectile);
    }

    fun int active(Projectile p) {
        return p.pool_index < num_active;
    }

    fun void ret(Projectile p) {
        // error handle
        T.assert(active(p), "returning a projectile that's already inactive!");
        if (!active(p)) return;

        p.pool_index => int i;

        // swap with last
        projectiles[num_active - 1] @=> projectiles[i];
        p @=> projectiles[num_active - 1];

        // update indices
        num_active - 1 => projectiles[num_active - 1].pool_index;
        i => projectiles[i].pool_index;

        // disable sensor events
        b2Shape.enableSensorEvents(p.shape_id, false);

        num_active--;

        T.assert(!active(p), "projectile index incorrect, still active");
    }

    // fires a single projectile from given position along the direction
    fun void fire(vec2 position, float dir_radians, Attack attack_type) {
        // add new body
        if (num_active >= projectiles.size()) {
            Projectile p;
            num_active => p.pool_index;
            b2.createBody(world_id, projectile_body_def) => p.body_id;
            b2Circle circle(radius);  // projectile collider will be smaller circle at end
            b2.createCircleShape(p.body_id, projectile_shape_def, circle) => p.shape_id;

            projectiles << p;
            projectile_map.set(p.body_id, p);
        } 

        // save projectile info
        projectiles[num_active] @=> Projectile p;
        p.body_id => int body_id;
        attack_type @=> p.attack;
        @(Math.cos(dir_radians), Math.sin(dir_radians)) => vec2 dir;
        dir => p.dir;

        // true sensor events
        b2Shape.enableSensorEvents(p.shape_id, true);

        b2Body.position(body_id, position);
        b2Body.linearVelocity(
            body_id,
            speed * dir
        );
        num_active++;
    }

    Attack@ attack_queue[0];

    fun void update() {
        player.pos => vec2 player_pos;

        { // handle any queued attacks (assumes ammo bookkeeping already handled)
            for (int i; i < attack_queue.size(); i++) {
                attack_queue[i] @=> Attack attack;
                if (attack == Attack_Normal) {
                    this.fire(player_pos, player.rotation, attack);
                } else if (attack == Attack_Triple) {
                    this.fire(player_pos, player.rotation + .2, attack);
                    this.fire(player_pos, player.rotation     , attack);
                    this.fire(player_pos, player.rotation - .2, attack);
                } else if (attack == Attack_Spread) {
                    Math.pi / 8.0 => float spread;
                    this.fire(player_pos, player.rotation + Math.random2f(-spread, spread), attack);
                } else if (attack == Attack_Side) {
                    this.fire(player_pos, player.rotation, attack);                 // forward
                    this.fire(player_pos, player.rotation + 0.5 * Math.pi, attack); // up
                    this.fire(player_pos, player.rotation - 0.5 * Math.pi , attack); // down
                }
            }
            attack_queue.clear();
        }
        T.assert(attack_queue.size() == 0, "didn't clear attack queue in proj pool update");

        num_active => prev_active;

        for (int i; i < num_active; i++) {
            projectiles[i] @=> Projectile p;

            b2Body.position(p.body_id) => vec2 pos;
            G.camera.worldPosToNDC(@(pos.x, pos.y, 0)) => vec3 pos_ndc;

            // if its off the screen, set inactive
            if (Math.fabs(pos_ndc.x) > 1 || Math.fabs(pos_ndc.y) > 1) {
                ret(p);
                // decrement counter to re-check the newly swapped id
                i--;
                // projectile boom!
                spork ~ boomEffect(pos, .1);
                continue;
            } 

            // else draw the active!
            lines.segment(pos, pos + .08 * p.dir, Color.WHITE);
            lines.segment(pos, pos - .08 * p.dir, p.attack.color);

            // debug draw collider
            circles.drawCircle(pos, radius, .15, Color.WHITE);
        }

        // if (num_active != prev_active)
            // <<< "num bullets active ", prev_active, " to ", num_active >>>;
    }
}

ProjectilePool projectile_pool;

// ========================================================================
// Enemies
// ========================================================================

0 => int EnemyType_None;
1 => int EnemyType_Rock;

class EnemyType {}

class Enemy
{
    int pool_index; // index in EnemyPool.enemies array
    int body_id;
    int type;

    float hp;
    UI_Bool hit_flash;
    Color.hex(0xFFFFFF) => vec3 hit_color;

    // rock stuff
    Color.hex(0xFF0000) => vec3 rock_color;
    vec2 vertices[8];
    float rock_size;
}

class EnemyPool
{ // repeating all this pool logic for now, maybe at some point consolidate all into 1
    Enemy enemies[32];
    int num_active;
    HashMap enemy_map; // map : body_id --> Enemy

    // initialize enemies
    for (int i; i < enemies.size(); i++) {
        i => enemies[i].pool_index;
    }

    fun int active(Enemy e) {
        return e.pool_index < num_active;
    }

    fun Enemy get() {
        if (num_active >= enemies.size()) {
            T.assert(false, "insufficient Enemy in EnemyPool");
            enemies << new Enemy;
            enemies.size() => enemies[-1].pool_index;
        } 

        num_active++;
        enemies[num_active - 1] @=> Enemy e;
        T.assert(e.pool_index == num_active - 1, "pool_index error on Enemy");
        return e;
    }

    fun void ret(Enemy e) {
        T.assert(active(e), "trying to return inactive enemy");
        if (!active(e)) return;

        e.pool_index => int i;

        // return and swap with last active
        enemies[num_active - 1] @=> enemies[i];
        e @=> enemies[num_active - 1];

        // update indices
        num_active - 1 => e.pool_index;
        i => enemies[i].pool_index;

        // destroy b2body
        if (b2Body.isValid(e.body_id)) {
            b2.destroyBody(e.body_id);
        }

        // remove from map
        // TODO: try reusing b2body, only destroy/create shapes.
        // hashmap body_id --> enemy entries become permanent
        enemy_map.del(e.body_id);

        // zero out
        0 => e.body_id;
        0 => e.type;

        num_active--;
    }

    // note: b2 polygons can have at most 8 vertices
    fun Enemy createRock(float size) {
        // TODO: jank, somehow allow creation outside of a graphics shred? 
        // this nextFrame is to register the shred as graphics so we can use b2
        GG.nextFrame() => now; 

        get() @=> Enemy e;
        EnemyType_Rock => e.type;
        size => e.rock_size;
        100.0 => e.hp;

        // movement params
        Math.random2f(.2, .3) => float speed;

        // generate the rock
        Math.two_pi / 8.0 => float theta;
        theta / 4.0 => float angle_variance;
        size / 4.0 => float size_variance;
        for (int i; i < 8; i++) {
            (size + Math.random2f(-size_variance, size_variance)) * 
            M.rot2vec(i * theta + Math.random2f(-angle_variance, angle_variance)) => e.vertices[i];
        }

        { // b2 setup (TODO: this is exactly same as pickups except for category, consolidate later)
            // pick a side
            (Math.randomf() < .5) ? Side_Left : Side_Right => vec2 which_side;

            b2BodyDef enemy_body_def;
            b2BodyType.kinematicBody => enemy_body_def.type;
            // b2BodyType.dynamicBody => enemy_body_def.type;

            Math.random2f(1.1, 2.1) => float angular_velocity;
            angular_velocity => enemy_body_def.angularVelocity;
            // movement (side to side on screen)
            -speed * which_side => enemy_body_def.linearVelocity;
            NDCToWorldPos(which_side + @(0, Math.random2f(-0.8, 0.8))) => enemy_body_def.position;
            false => enemy_body_def.enableSleep; // disable otherwise slowly rotating objects will be put to sleep

            b2Filter filter;
            Category_Enemy => filter.categoryBits;
            Category_Player | Category_Projectile => filter.maskBits;

            b2ShapeDef shape_def;
            true => shape_def.isSensor;
            filter @=> shape_def.filter;

            b2.createBody(world_id, enemy_body_def) => e.body_id;
            T.assert(b2Body.isValid(e.body_id), "enemy b2body invalid");

            // physics shape
            b2.makePolygon(e.vertices, 0.0) @=> b2Polygon polygon;
            b2.createPolygonShape(e.body_id, shape_def, polygon);

            // register in map
            enemy_map.set(e.body_id, e);
        }

        return e;
    }

    // destroy an enemy
    fun void destroy(Enemy e) {
        b2.destroyBody(e.body_id);

        0 => e.body_id;

    }

    fun void update() {
        for (int i; i < num_active; i++) {
            enemies[i] @=> Enemy e;
            // enemies are destroyed if
            // - offscreen
            // - b2body no longer valid
            if (
                !b2Body.isValid(e.body_id)
            ) {
                ret(e);
                i--;  // decrement counter to process newly swapped
                continue;
            }

            b2Body.position(e.body_id) => vec2 pos;
            b2Body.angle(e.body_id) => float rot_radians;

            if (offscreen(pos, 1.2)) {
                ret(e);
                i--;  // decrement counter to process newly swapped
                continue;
            }

            // draw
            if (e.type == EnemyType_Rock) {
                e.rock_color => vec3 color;
                if (e.hit_flash.val()) e.hit_color => color;
                lines.drawPolygon(pos, rot_radians, e.vertices, color);
            }

            // track closest enemy to player
            Math.euclidean(player.pos, pos) => float dist;
            if (G.closest_enemy == null || dist < G.closest_enemy_dist) {
                dist => G.closest_enemy_dist;
                e @=> G.closest_enemy;
            }
        }
    }
}
EnemyPool enemy_pool;

GText beat_progress_text --> GG.scene();
GText wave_progress_text --> GG.scene();
.5 => beat_progress_text.sca; 
.5 => wave_progress_text.sca;
beat_progress_text.antialias(false);
wave_progress_text.antialias(false);
beat_progress_text.color(Color.GRAY);
wave_progress_text.color(Color.GRAY);


// given an event happens once every `period_secs`, returns
// the amount of time you need to wait until the next occurance.
// Assumes the wait interval is 1 frame at 60fps
// returns the amount of time in seconds until the next event occurs
// fun static float poisson(float period_sec) {
fun void pickupSpawner(Pickup pickup_type) {
    while (true) {
        M.poisson(8)::second => now;
        if (player.hp < player.hp_max) {
            spork ~ Pickup.spawn(
                Math.randomf() < .5 ? Side_Left : Side_Right, 
                pickup_type
            );
        }
    }
}

fun void enemySpawner() {
    <<< "enenySpawner" >>>;
    while (true) {
        // start at 1 enemy per measure = 4 beats = 0.25 enemies per beat
        // end at 4 enemies per beat = 16 per measure
        // so we scale linearly from wave 1 --> wave 16, 1 enemy per measure --> 16 enemies per measure
        // trying poisson for now
        G.wave_count + 1.0 => float enemies_per_measure;
        1.0 / 4.0 => float measures_per_beat;
        second / G.qt_note => float beats_per_second;
        enemies_per_measure * measures_per_beat * beats_per_second => float enemies_per_second;
        1.0 / enemies_per_second => float seconds_per_enemy;
        M.poisson(seconds_per_enemy)::second => now;
        spork ~ enemy_pool.createRock(Math.random2f(.2, .35));
    }
}

fun void spawner() {
    spork ~ pickupSpawner(Pickup_Health);
    spork ~ enemySpawner();
    while (true) {
        GG.nextFrame() => now;
        { // reposition text (TODO do this in drum update)
            "BEAT " + (((G.beat_count - 1) % G.BEATS_PER_WAVE) + 1) + "/" + G.BEATS_PER_WAVE => beat_progress_text.text; // 64 beats == 32 seconds per wave T
            NDCToWorldPos(0, .8, -2) => beat_progress_text.posWorld;

            "WAVE " + G.wave_count + "/" + G.WAVES_PER_GAME => wave_progress_text.text;
            NDCToWorldPos(0, .7, -2) => wave_progress_text.posWorld;
        }
    }
} spork ~ spawner();


// ============================================
// Sound
// ============================================

class Sound
{
    Topograph topograph;

    JCRev rev => dac;
    0.05 => rev.mix;
    SndBuf kick => rev;
    SndBuf snare => rev;
    SndBuf hat => rev;
    me.dir() + "../../../assets/samples/punchy-kick.wav" => kick.read;
    me.dir() + "../../../assets/samples/snare.wav" => snare.read;
    me.dir() + "../../../assets/samples/trap-hihat.wav" => hat.read;
    0 => kick.rate => snare.rate => hat.rate;
    [kick, snare, hat] @=> SndBuf drum_rack[];

    G.qt_note / 8.0 => dur step; // 1 drum machine step = 32nd note

    float topograph_xy[2];
    float topograph_density[3];

    fun void drumUpdate() {
        while (true) {
            false => int is_beat;
            { // update parameters 
                // global beat count
                G.step_count++;
                if (G.step_count % 8 == 1) {
                    G.beat_count++;
                    true => is_beat;
                    G.beat_event.broadcast();
                    now => G.last_beat_time;
                    if (G.beat_count > 1) {
                        T.assert(G.last_beat_time == G.next_beat_time, "beat at unexpected time");
                    }
                    G.last_beat_time + G.qt_note => G.next_beat_time;

                    if (G.beat_count % G.BEATS_PER_UPGRADE == 0) {
                        Pickup.spawnUpgrades();
                    }

                    if (G.beat_count % G.BEATS_PER_WAVE == 1) {
                        G.wave_count++;


                    }
                }

                // map density to amount
                // interestingly, since we map density to %capacity, increasing ammo
                // capacity will always decrease density at the same amount of ammo....
                // maybe consolidate all ammos into a single ammo_max?
                topograph.density(topograph.Inst_Kick, (player.kick_ammo $ float) / player.kick_ammo_max);
                topograph.density(topograph.Inst_Snare, (player.snare_ammo $ float) / player.snare_ammo_max);
                topograph.density(topograph.Inst_Hat, (player.hat_ammo $ float) / player.hat_ammo_max);
            }

            // render music
            topograph.step() @=> int velocities[];
            if (is_beat) 255 => velocities[0];
            // play instruments
            for (int i; i < velocities.size(); i++) {
                if (velocities[i] > 0) {
                    // handle kick
                    if (i == 0) {
                        if (player.kick_ammo) projectile_pool.attack_queue << Attack_Triple;
                        Math.max(0, player.kick_ammo - 1) => player.kick_ammo;
                    } else if (i == 1) { // snare
                        if (player.snare_ammo) projectile_pool.attack_queue << Attack_Side;
                        Math.max(0, player.snare_ammo - 1) => player.snare_ammo;
                    } else if (i == 2) { // hat 
                        if (player.hat_ammo) projectile_pool.attack_queue << Attack_Spread;
                        Math.max(0, player.hat_ammo - 1) => player.hat_ammo;
                    }

                    0 => drum_rack[i].pos;
                    1.0 => drum_rack[i].rate;
                    velocities[i] / 255.0 => drum_rack[i].gain;
                }
            }
            step => now;
        }
    }
}
Sound S;
spork ~ S.drumUpdate();


// ============================================
// Developer Console
// ============================================
null => UI_Style@ ui_style;
UI_Int player_exp(player.exp);
fun void devConsole() {
    // init
    if (ui_style == null) {
        UI.getStyle() @=> ui_style;
        ui_style.alpha(.2);
    }

    // update vars
    player.exp => player_exp.val;
    if (UI.begin("Developer Console", null, UI_WindowFlags.NoBackground | UI_WindowFlags.NoTitleBar | UI_WindowFlags.NoNavInputs)) {
        UI.separatorText("Player");

        if (UI.slider("XP", player_exp, 0, player.ammo_max)) {
            player_exp.val() => player.exp;
        }

        UI.checkbox("Movement Mode", player.dev_movement_type);

        UI.slider("Player Lock-On Steering", player.dev_auto_lock_rotate_rate, 0.0, 1.0);
        UI.slider("Player Lock-On Range", player.dev_auto_lock_range, 0.0, 5.0);
        UI.slider("Player Movement Steering", player.dev_steering_rate, 0.0, 1.0);

        if (UI.button("Spawn Upgrades")) {
            Pickup.spawnUpgrades();
        }
    }
    UI.end();

}

// gameloop ============================================
DebugDraw debug_draw;
true => debug_draw.drawShapes;

int keys[0];
// keys.help();

0 => int player_attack_idx;
while (true) {
    GG.nextFrame() => now;
    
    // update globals ======================================================
    GG.dt() * G.dt_rate => G.dtf;
    G.dtf::second => G.dt;
    G.dt +=> G.dt_cum;
    G.dtf +=> G.dtf_cum;
    null @=> G.closest_enemy;
    0 => G.closest_upgrade_body_id;

    // dev ======================================================
    devConsole();

    
    // enemy update (must happen before player rotation for auto aim) =============================================
    enemy_pool.update();

    // upgrade update
    vec2 upgrade_positions[0];
    for (int i; i < G.active_upgrades.size(); i++) {
        G.active_upgrades[i] => int upgrade_body_id;
        // filter invalid upgrades
        if (!b2Body.isValid(upgrade_body_id)) {
            G.active_upgrades[-1] => G.active_upgrades[i];
            G.active_upgrades.popBack();
            i--;
            continue;
        } else {
            upgrade_positions << b2Body.position(upgrade_body_id);
            // update closest
            Math.euclidean(player.pos, upgrade_positions[-1]) => float dist_to_player;
            if (G.closest_upgrade_body_id == 0 || dist_to_player < G.closest_upgrade_dist) {
                if (dist_to_player <= player.dev_auto_lock_range.val()) {
                    upgrade_body_id => G.closest_upgrade_body_id;
                    dist_to_player => G.closest_upgrade_dist;
                }
            }
        }
    }
    lines.drawPolygon(@(0, 0), 0, upgrade_positions, Color.WHITE);
    



    // input ======================================================
    if (GWindow.keyDown(GWindow.Key_Space)) {
        // wow the latency is crazy on this for macbook builtin keyboard, ~220ms
        // adjust latency
        // TODO: why is this so far off?
        // WTH on lab bluetooth keyboard (with mac) there isn't any latency?? 
        // now - 220::ms => time hit_time;
        now => time hit_time;

        Math.min((hit_time - G.last_beat_time)/ms, (G.next_beat_time - hit_time)/ms) => float offbeat_ms;
        <<< "adjusted accuracy (ms)", offbeat_ms >>>; 

        if (offbeat_ms > 80) {
            spork ~ textEffect(
                "OFF BEAT",
                player.pos,
                Color.RED
            );
        } else {
            // teleport and acquire nearest upgrade, if available
            if (b2Body.isValid(G.closest_upgrade_body_id)) {
                // teleport player
                b2Body.position(player.body_id, b2Body.position(G.closest_upgrade_body_id));
                Pickup.activate(G.closest_upgrade_body_id, true);
            }
        }
    }

    false => int auto_locked;
    if (player.dev_movement_type.val()) {
        // boost
        if ( GWindow.key(GWindow.Key_Up) || UI.isKeyDown(UI_Key.GamepadR2) ) {
            1.5 => player.velocity_scale;
            Color.hex(0x00ffff) => player.boost_trail_color;
        } else if ( GWindow.key(GWindow.Key_Down) || UI.isKeyDown(UI_Key.GamepadL2) ) {
            (0.5) => player.velocity_scale;
            Color.hex(0xff0000) => player.boost_trail_color;
        } else {
            1.0 => player.velocity_scale;
            Color.hex(0xffa500) => player.boost_trail_color;
        }

        if (
            GWindow.key(GWindow.Key_Left)
            ||
            UI.isKeyDown(UI_Key.GamepadLStickLeft)
        ) {
            .1 +=> player.rotation;
        } 
        if (
            GWindow.key(GWindow.Key_Right)
            ||
            UI.isKeyDown(UI_Key.GamepadLStickRight)
        ) {
            -.1 +=> player.rotation;
        } 

    } else {
        // auto-aim movement

        // @(0,0) => player.movement_vec;
        vec2 desired_heading;
        if (GWindow.key(GWindow.Key_Left)) -1 +=> desired_heading.x;
        if (GWindow.key(GWindow.Key_Right)) 1 +=> desired_heading.x;
        if (GWindow.key(GWindow.Key_Up)) 1 +=> desired_heading.y;
        if (GWindow.key(GWindow.Key_Down)) -1 +=> desired_heading.y;
        @(0,0) => player.movement_desired_vec => player.movement_steering_vec;
        if (M.magnitude(desired_heading) > 0) {
            M.normalize(desired_heading) => desired_heading;
            desired_heading => player.movement_desired_vec;
            (desired_heading - player.movement_vec) => vec2 steering; // don't normalize this bc it's 0 when desired_heading == movement_vec
            steering => player.movement_steering_vec;
            player.movement_vec + player.dev_steering_rate.val() * steering => vec2 new_heading;
            // new_heading => player.movement_vec;

            lines.segment(player.pos, player.pos + player.movement_vec, Color.WHITE); // debug draw
            lines.segment(player.pos, player.pos + desired_heading, Color.RED); // debug draw
            lines.segment(player.pos, player.pos + steering, Color.GREEN); // debug draw

            // M.normalize(new_heading) => player.movement_vec;
            new_heading => player.movement_vec;
        }


        // always face the closest enemy
        // TODO add range to projectiles?
        if (G.closest_enemy != null && G.closest_enemy_dist <= player.dev_auto_lock_range.val()) {
            M.rot2vec(player.rotation) => vec2 current_heading;
            M.normalize(b2Body.position(G.closest_enemy.body_id) - player.pos) => vec2 desired_heading;
            // intentionally not normalizing the steering vector here
            // this makes the ship rotate faster when the different in angles is greater,
            // and slower as it approaches alignment. Like builtin smoothing!
            (desired_heading - current_heading) => vec2 steering;

            // lines.segment(player.pos, player.pos + current_heading, Color.WHITE); // debug draw
            // lines.segment(player.pos, player.pos + desired_heading, Color.RED); // debug draw
            // lines.segment(player.pos, player.pos + steering, Color.GREEN); // debug draw

            (current_heading + player.dev_auto_lock_rotate_rate.val() * steering) => vec2 new_heading;
            M.angle(new_heading) => player.rotation;
            true => auto_locked;
        }

    }

    // apply movement to b2 ======================================================
    @(Math.cos(player.rotation), Math.sin(player.rotation)) => vec2 player_dir;
    if (player.dev_movement_type.val()) {
        // apply velocity
        b2Body.linearVelocity(
            player.body_id, 
            player.velocity_scale * player_dir
        );
    } else {
        // apply basde on movement vector, not rotation
        if (M.magnitude(player.movement_vec) > 0) {
            b2Body.linearVelocity(
                player.body_id, 
                M.normalize(player.movement_vec)
            );
        }
    }
    // apply rotation
    b2Body.angle(player.body_id, player.rotation);


    { // XP/ammo allocation
        if (GWindow.key(GWindow.Key_1)) {
            player.kick_ammo == player.kick_ammo_max => int full;
            if (!full) {
                if (player.addAmmo(-1))
                    Math.min(1 + player.kick_ammo, player.kick_ammo_max) => player.kick_ammo;
            }
        }
        if (GWindow.key(GWindow.Key_2)) {
            player.snare_ammo == player.snare_ammo_max => int full;
            if (!full) {
                if (player.addAmmo(-1))
                    Math.min(1 + player.snare_ammo, player.snare_ammo_max) => player.snare_ammo;
            }
        }
        if (GWindow.key(GWindow.Key_3)) {
            player.hat_ammo == player.hat_ammo_max => int full;
            if (!full) {
                if (player.addAmmo(-1))
                    Math.min(1 + player.hat_ammo, player.hat_ammo_max) => player.hat_ammo;
            }
        }
    }

    // switch weapons
    if (
        UI.isKeyPressed(UI_Key.GamepadFaceDown)
        ||
        GWindow.keyDown(GWindow.Key_Tab)
    ) {
        attacks[(1 + player_attack_idx++) % attacks.size()] @=> player.attack;
    }

    // explode test
    if (
        UI.isKeyPressed(UI_Key.GamepadFaceRight)
        ||
        GWindow.keyDown(GWindow.Key_Enter)
    ) {
        .5::second => dur explode_dur;
        // shake it up
        spork ~ cameraShakeEffect(.08, explode_dur, 30);
        // time warp!
        spork ~ slowEffect(.15, 2::second);
        spork ~ explodeEffect(@(0,0), explode_dur);
        spork ~ screenFlashEffect(20, .1::second);
    }

    // effects test
    if (GWindow.keyDown(GWindow.Key_1)) {
        spork ~ refreshEffect();
    }

    { // player logic
        // render ======================================================
        b2Body.position(player.body_id) => player.pos; // update player pos
        player.pos => vec2 player_pos;

        // wrap player around screen
        G.camera.worldPosToNDC(player_pos $ vec3) => vec3 player_pos_ndc;
        if (player_pos_ndc.x > 1) -1 => player_pos_ndc.x;
        if (player_pos_ndc.x < -1) 1 => player_pos_ndc.x;
        if (player_pos_ndc.y > 1) -1 => player_pos_ndc.y;
        if (player_pos_ndc.y < -1) 1 => player_pos_ndc.y;
        G.camera.NDCToWorldPos(player_pos_ndc) $ vec2 => player_pos;
        b2Body.position(player.body_id, player_pos);

        // draw ship
        if (!player.invisible.val()) {
            circles.drawCircle( player_pos, player.radius, 1.0, Color.WHITE);
        }

        // ship auto-lock range

        lines.dottedCircle(player_pos, player.dev_auto_lock_range.val() * .8, now/second, auto_locked ? .5 * Color.WHITE : .1 * G.Color_Locked);
        // ship auto-lock orientation
        lines.dotted(player_pos, player_pos + player.dev_auto_lock_range.val() * .8 * M.rot2vec(player.rotation), 
            auto_locked ? .5 * Color.WHITE : .1 * G.Color_Locked, .1);
        
        // teleport to upgrade
        if (b2Body.isValid(G.closest_upgrade_body_id) && G.closest_upgrade_dist <= player.dev_auto_lock_range.val()) {
            // draw line to it
            lines.segment(player_pos, b2Body.position(G.closest_upgrade_body_id), Color.hex(0x00FFFF));
        }

        // ship exhaust
        M.rot2vec(player.rotation - (Math.pi / 2.0)) => vec2 player_rot_perp;
        player_pos - .9 * player.radius * player_dir => vec2 exhaust_pos;
        player_pos - .9 * player.radius * player.movement_steering_vec => vec2 steering_pos;

        // ~20 shreds
        spork ~ boostTrailEffect(
            player_pos,
            player.boost_trail_color,
            player.velocity_scale // scale trail size by speed
        );

        // spork ~ boostTrailEffect(
        //     steering_pos,
        //     // player.boost_trail_color,
        //     Color.GREEN,
        //     player.velocity_scale // scale trail size by speed
        // );

        // ~20 shreds
        // spork ~ boostTrailEffect(
        //     exhaust_pos - .07 * player_rot_perp,
        //     player.boost_trail_color,
        //     player.velocity_scale // scale trail size by speed
        // );
    }
    player.updateHUD();

    projectile_pool.update();


    // collisions (TODO does it matter what order in game loop this happens?)
    {
        b2World.contactEvents(world_id, begin_touch_events, null, null);
        for (int i; i < begin_touch_events.size(); i++) {
            <<< "begin_touch:", begin_touch_events[i] >>>;
        }

        b2World.sensorEvents(world_id, begin_sensor_events, null);
        for (int i; i < begin_sensor_events.size(); 2 +=> i) {
            // assume for now that only pickups trigger sensor events
            // and that the order is deterministic (sensor, player, sensor, player....) 
            begin_sensor_events[i] => int sensor_id;
            begin_sensor_events[i + 1] => int not_sensor_id;

            // if either are invalid, (e.g. destroyed in previous sensor event) skip
            if (!b2Body.isValid(sensor_id) || !b2Body.isValid(not_sensor_id)) continue;

            b2Body.position(sensor_id) => vec2 sensor_pos;
            b2Body.position(not_sensor_id) => vec2 not_sensor_pos;

            // sensor was a pickup
            if (G.pickup_map.has(sensor_id)) {
                sensor_id => int pickup_id;
                T.assert(not_sensor_id == player.body_id, "non-player triggered pickup sensor");

                Pickup.activate(pickup_id, false);
            } else {
                // something collided with enemy

                sensor_id => int enemy_body_id;
                T.assert(enemy_pool.enemy_map.has(enemy_body_id), "sensor is not a pickup or enemy?");

                if (not_sensor_id == player.body_id) {
                    // invincible, nothing happens
                    if (player.invincible.val()) continue;

                    // player hit enemy
                    // TODO player damage
                    .5::second => dur explode_dur;
                    // shake it up
                    spork ~ cameraShakeEffect(.08, explode_dur, 30);
                    // time warp!
                    spork ~ slowEffect(.5, explode_dur * 2);
                    spork ~ explodeEffect(not_sensor_pos, explode_dur);
                    spork ~ screenFlashEffect(20, .1::second);

                    // invincible
                    spork ~ invincibleEffect();
                    // invisible
                    spork ~ invisibleEffect();

                } else if (projectile_pool.projectile_map.has(not_sensor_id)) {
                    // BUG: sensor events are disabled when we return projectile to pool, yet
                    // still triggering sensor events in b2. causes a bullet to lazer through multiple
                    // Temporary soln: only handle bullet collision if bullet is active
                    // maybe refactoring to use contactEvents will solve this
                    projectile_pool.projectile_map.getObj(not_sensor_id) $ Projectile @=> Projectile p;
                    if (projectile_pool.active(p)) {
                        { // enemy takes damage
                            enemy_pool.enemy_map.getObj(enemy_body_id) $ Enemy @=> Enemy e;

                            50 -=> e.hp; // 50 dmg fixed for now
                            
                            // ENEMY ONDEATH
                            if (e.hp <= 0.01) {
                                // destroy and return to pool
                                enemy_pool.ret(e); 
                                spork ~ boomEffect(sensor_pos, 2*e.rock_size);
                                // spawn exp (TODO pool the pickup logic)
                                repeat (5) {
                                    spork ~ spawnPickupAmmo(M.randomPointInCircle(sensor_pos, 0, .35)); 
                                }
                            } else {
                                // dmg on hit flash
                                spork ~ hitFlashEffect(e.hit_flash);
                            }
                        }

                        { // projectile bye bye
                            projectile_pool.ret(p);
                            spork ~ boomEffect(not_sensor_pos, .1);
                        }
                    }
                } else {
                    T.assert(false, "thing that hit enemy is neither player nor projectile?");
                    
                }

                // TODO: actually called *player* hit

                // enemy_pool.hit(enemy_body_id);
            }
        }
    }

    // flush 
    if (true) {
        circles.update();
        lines.update();
        polygons.update();
    } else {
        // physics debug draw
        b2World.draw(world_id, debug_draw);
        debug_draw.update();
    }
}
