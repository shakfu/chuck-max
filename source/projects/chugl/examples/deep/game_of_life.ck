//-----------------------------------------------------------------------------
// name: game_of_life.ck
// desc: microphone input waveform as seeds for Conway's Game of Life
// 
// author: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//   date: Fall 2024
//-----------------------------------------------------------------------------
/* Interesting observation:
The fragment shader invocation is not 1:1 with pixels on the quad.
This means at different resolutions (zoom levels / size of quad) different
cells will be updated. If the quad is close enough / resolution is high enough,
every pixel will cover a single cell and simulation will run "accurately".
However if the quad is very far away, and each screen pixel covers multiple
cells, not all cells will be updated. But kinda looks cool?

IDEA: wrap repeat, sample OFF the grid with scaling factor n*m for INFINITE conway!!
- requires doing as screen shader or pass clip space full-screen quad through custom geo */
//-----------------------------------------------------------------------------

GCamera camera --> GG.scene();
camera.orthographic();
camera.viewSize(10.0 / 16);
camera.posZ(1.0);
camera => GG.scene().camera;

// audio stuff -----------------------------------------
// GWindow.fullscreen();
256 => int WINDOW_SIZE;
// accumulate samples from mic
adc => Flip accum => blackhole;
WINDOW_SIZE => accum.size;

float samples[WINDOW_SIZE];

fun void readMicInput()
{
    while( true )
    {
        // upchuck to process accum
        accum.upchuck();
        // get the last window size samples (waveform)
        accum.output( samples );
        // jump by samples
        WINDOW_SIZE::samp => now;
    }
}
spork ~ readMicInput();


// ------- graphics stuff -----------
// shader code string (does the GoL simulation)
"
#include FRAME_UNIFORMS
#include DRAW_UNIFORMS
#include STANDARD_VERTEX_INPUT
#include STANDARD_VERTEX_OUTPUT
#include STANDARD_VERTEX_SHADER

// our custom material uniforms
@group(1) @binding(0) var src: texture_2d<f32>;
@group(1) @binding(1) var dst: texture_storage_2d<rgba8unorm, write>;
@group(1) @binding(2) var<uniform> simulate_step: i32;
@group(1) @binding(3) var<storage> mic_samples: array<f32>;

fn onWaveform(coords: vec2i, dim: vec2u) -> bool {
    return i32((0.5 + mic_samples[coords.x] * 0.5) * f32(dim.y)) == coords.y;
}

fn alive(coords: vec2i, dim: vec2u) -> i32 {
    let on_waveform : bool = i32((0.5 + mic_samples[coords.x] * 0.5) * f32(dim.y)) == coords.y;
    if (on_waveform) {
        return 1;
    }
    let v = textureLoad(src, coords, 0);
    if (v.r < 0.5) { return 0; }
    return 1;
}

@fragment 
fn fs_main(in : VertexOutput, @builtin(front_facing) is_front: bool) -> @location(0) vec4f
{
    let dim : vec2u = textureDimensions(src);

    let coords = vec2i(in.v_uv * vec2f(dim));
    var cell = vec4f(f32(alive(coords, dim)));

    if (bool(simulate_step)) {
        // any cell part of the audio waveform is alive
        let is_alive = bool(alive(coords, dim));
        let neighbors =
            alive(coords + vec2(-1, -1), dim)
            +  alive(coords + vec2(-1,  0), dim)
            +  alive(coords + vec2(-1,  1), dim)
            +  alive(coords + vec2( 0, -1), dim)
            +  alive(coords + vec2( 0,  1), dim)
            +  alive(coords + vec2( 1, -1), dim)
            +  alive(coords + vec2( 1,  0), dim)
            +  alive(coords + vec2( 1,  1), dim);

        var s = 0.0;

        // live cell with 2 or 3 neighbors lives
        if (is_alive && (neighbors == 2 || neighbors == 3)) {
            s = 1.0;
        }
        // dead cell with exactly 3 neighbors becomes alive
        else if (!is_alive && (neighbors == 3)) {
            s = 1.0;
        }
        // all other cases cell dies (don't need to program)

        textureStore(dst, coords, vec4f(s)); // store for next generation
    }

    return vec4f(cell); // render current generation
}" => string game_of_life_shader;

Material material;
PlaneGeometry plane_geo;

ShaderDesc shader_desc;
game_of_life_shader => shader_desc.vertexCode;
game_of_life_shader => shader_desc.fragmentCode;

Shader custom_shader(shader_desc); // create shader from shader_desc
custom_shader => material.shader; // connect shader to material

GMesh mesh(plane_geo, material) --> GG.scene();

TextureDesc conway_tex_desc;
WINDOW_SIZE => conway_tex_desc.width;
WINDOW_SIZE => conway_tex_desc.height;

Texture conway_tex_a(conway_tex_desc);
Texture conway_tex_b(conway_tex_desc);

float texture_data[4 * WINDOW_SIZE * WINDOW_SIZE];
// TODO need a better way to specify texture size
conway_tex_b.write(texture_data); // initialize empty texture
conway_tex_a.write(texture_data);

fun void simulate() 
{
    true => int flip;
    material.uniformInt(2, 0);
    material.texture(0, conway_tex_a);
    material.storageTexture(1, conway_tex_b);

    2::second => now;

    while (true) {
        // only step once per second
        now + .1::second => time later; 
        while (now < later) GG.nextFrame() => now;

        // enable for a single frame
        material.uniformInt(2, 1);
        GG.nextFrame() => now;

        // disable again
        material.uniformInt(2, 0);
        // flip textures
        if (flip) {
            material.texture(0, conway_tex_b);
            material.storageTexture(1, conway_tex_a);
        } else {
            material.texture(0, conway_tex_a);
            material.storageTexture(1, conway_tex_b);
        }
        1 - flip => flip;
    }
}
spork ~ simulate();

while (true) {
    // write new audio data to shader
    material.storageBuffer(3, samples);
    GG.nextFrame() => now;
}
