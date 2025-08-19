//-----------------------------------------------------------------------------
// name: sprite_animation.ck
// desc: animate a cat with sprite sheet textures and a simple state machine
// Requires downloading the examples/data/textures/Cat-1 directory.
// 
// author: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//   date: Fall 2024
//-----------------------------------------------------------------------------

// animation helper class ===============================================
class SpriteAnimation {
    Texture sprite_sheet;
    int frames;
    int loop; // true if the animation should loop

    fun @construct(Texture@ tex, int frames, int loop) {
        tex @=> this.sprite_sheet;
        frames => this.frames;
        loop => this.loop;
    }
}

// load sprite sheets ===============================================
TextureLoadDesc tex_load_desc;
true => tex_load_desc.flip_y;
false => tex_load_desc.gen_mips;

SpriteAnimation cat_idle(
    Texture.load(me.dir() + "../data/textures/Cat-1/Cat-1-Idle.png", tex_load_desc),
    10, // 10 frames
    true // should loop
);

SpriteAnimation cat_laying(
    Texture.load(me.dir() + "../data/textures/Cat-1/Cat-1-Laying.png", tex_load_desc),
    8, // 8 frames
    false // should not loop
);

// uncomment for a different laying animation
// SpriteAnimation cat_laying(
//     Texture.load(me.dir() + "../data/textures/Cat-1/Cat-1-Licking 2.png", tex_load_desc),
//     5, // 5 frames
//     true // should not loop
// );

SpriteAnimation cat_walk(
    Texture.load(me.dir() + "../data/textures/Cat-1/Cat-1-Walk.png", tex_load_desc),
    8, // 8 frames
    true // should loop
);

SpriteAnimation cat_meow(
    Texture.load(me.dir() + "../data/textures/Cat-1/Meow-VFX.png", tex_load_desc),
    3, // 8 frames
    false // should not loop
);

// cat sprite setup ===============================================
FlatMaterial sprite_material;

// set sampler filter mode to nearest for pixel-art style
TextureSampler sprite_sampler;
TextureSampler.Filter_Nearest => sprite_sampler.filterMin;
TextureSampler.Filter_Nearest => sprite_sampler.filterMag;
TextureSampler.Filter_Nearest => sprite_sampler.filterMip;

sprite_material.sampler(sprite_sampler); // sampler (nearest)
sprite_material.colorMap(cat_idle.sprite_sheet); // texture


PlaneGeometry plane_geo;
GMesh sprite(plane_geo, sprite_material) --> GG.scene();

// meow sprite setup ===============================================

FlatMaterial cat_meow_material;
cat_meow_material.sampler(sprite_sampler); // sampler (nearest)
cat_meow_material.colorMap(cat_meow.sprite_sheet); // texture
cat_meow_material.scale(@(0.0, 0.0)); // hide the meow sprite

GMesh cat_meow_mesh(plane_geo, cat_meow_material) --> sprite;
.35 => cat_meow_mesh.sca;
@(.32, .05, 0.0) => cat_meow_mesh.pos;

// meow audio setup ===============================================
SndBuf meow_buf => dac;
"special:dope" => meow_buf.read;
0 => meow_buf.rate;
.2 => meow_buf.gain;

cat_idle @=> SpriteAnimation cat_state; // initial state

// animation loop ===============================================
fun void animate(float frame_duration_secs) {
    null @=> SpriteAnimation current_animation;
    0 => int current_frame;
    0 => float time_since_last_frame; // accumulator to know when to flip animation frames

    while (true) {
        GG.nextFrame() => now;

        // animation transition
        if (cat_state != current_animation) {
            cat_state @=> current_animation;
            0 => current_frame;

            // recalculate the texture scale for a single frame
            @(
                1.0 / current_animation.frames,
                1.0
            ) => vec2 frame_uv_scale;
            sprite_material.scale(frame_uv_scale);

            // update the sprite sheet
            sprite_material.colorMap(current_animation.sprite_sheet);

            // switch to first frame of new animation
            sprite_material.offset(@(current_frame $ float / current_animation.frames, 0));
            0 => time_since_last_frame; // reset frame timer
        } 

        // play animation
        else if (time_since_last_frame >= frame_duration_secs) {
            // advance to next frame
            0 => time_since_last_frame;

            if (!current_animation.loop && current_frame + 1 >= current_animation.frames) {
                // reached end of animation
                continue;
            }
            (current_frame + 1) % current_animation.frames => current_frame;
            sprite_material.offset(@(current_frame $ float / current_animation.frames, 0));
        } else {
            GG.dt() +=> time_since_last_frame;
        }
    }
}
spork ~ animate(.1);

// meow animation ===============================================
0 => int meow_generation;
fun void meow()
{
    ++meow_generation => int this_meow_generation;
    // play meow sound
    Math.random2f(2.5, 4.5) => meow_buf.rate;
    0 => meow_buf.pos;
    1 / meow_buf.rate() => cat_meow_mesh.sca;

    // recalculate the texture scale for a single frame
    cat_meow_material.scale( @( 1.0 / cat_meow.frames, 1.0) );
    // true => meowing;
    for (0 => int i; i < cat_meow.frames; i++) {
        // check if another meow has been triggered
        if (this_meow_generation != meow_generation) { return; }

        // switch to first frame of new animation
        cat_meow_material.offset(@(i $ float / cat_meow.frames, 0));

        // play animation
        0.1::second => now;
    }
    // set scale to 0.0 to hide the meow sprite
    cat_meow_material.scale(@(0.0, 0.0));
}

// main loop ===============================================
while (true) {
    GG.nextFrame() => now;

    // meow animation
    if (GWindow.keyDown(GWindow.Key_Space)) {
        spork ~ meow();
    }

    // controls + animation state machine
    if (cat_state == cat_idle) {
        // idle --> walk right
        if (GWindow.keyDown(GWindow.Key_Right)) {
            cat_walk @=> cat_state;
            1 => sprite.scaX;
        // idle --> walk left
        } else if (GWindow.keyDown(GWindow.Key_Left)) {
            cat_walk @=> cat_state;
            -1 => sprite.scaX;
        // idle --> laying
        } else if (GWindow.keyDown(GWindow.Key_Down)) {
            cat_laying @=> cat_state;
        }
    } else if (cat_state == cat_walk) {
        // walk --> laying
        if (GWindow.keyDown(GWindow.Key_Down)) {
            cat_laying @=> cat_state;
        }

        // changing directions
        else if (GWindow.key(GWindow.Key_Right)) {
            1 => sprite.scaX;
            sprite.translateX(GG.dt());
        } else if (GWindow.key(GWindow.Key_Left)) {
            -1 => sprite.scaX;
            sprite.translateX(-GG.dt());
        }

        // walk --> idle
        else if (!GWindow.key(GWindow.Key_Right) && !GWindow.key(GWindow.Key_Left)) {
            cat_idle @=> cat_state;
        }
    } else if (cat_state == cat_laying) {
        // laying --> walk right
        if (GWindow.keyDown(GWindow.Key_Right)) {
            cat_walk @=> cat_state;
            1 => sprite.scaX;
        }
        // laying --> walk left
        else if (GWindow.keyDown(GWindow.Key_Left)) {
            cat_walk @=> cat_state;
            -1 => sprite.scaX;
        }
    }

    // wrap around screen
    GG.camera().worldPosToNDC(sprite.posWorld()) => vec3 pos_ndc;
    if (pos_ndc.x > 1.0 || pos_ndc.x < -1.0) {
        -1.0 *=> pos_ndc.x;
        sprite.posWorld(GG.camera().NDCToWorldPos(pos_ndc));
    } 

    if (UI.begin("Sprite Animation")) {
        UI.textWrapped("Sprite animation example! Use arrow keys to move the cat. Press space to meow. Press down to lay down.");
    }
    UI.end();
}
