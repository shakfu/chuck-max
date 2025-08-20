//-----------------------------------------------------------------------------
// name: audio_donut.ck
// desc: example of writing to textures:
//       1) writes audio spectrum data to a height map
//       2) uses height map to display surface of a donut
// 
// author: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//   date: Fall 2024
//-----------------------------------------------------------------------------

// window size
1024 => int WINDOW_SIZE;
1024 => int WATERFALL_DEPTH;
// 512 => int WATERFALL_DEPTH;
// accumulate samples from mic
adc => Flip accum => blackhole;
// take the FFT
adc => PoleZero dcbloke => FFT fft => blackhole;
// set DC blocker
.95 => dcbloke.blockZero;
// set size of flip
WINDOW_SIZE => accum.size;
// set window type and size
Windowing.hann(WINDOW_SIZE) => fft.window;
// set FFT size (will automatically zero pad)
WINDOW_SIZE*2 => fft.size;
// get a reference for our window for visual tapering of the waveform
Windowing.hann(WINDOW_SIZE) @=> float window[];
// sample array
float samples[0];
// FFT response
complex response[0];
// mapped FFT response
float spectrum[WINDOW_SIZE];

// audio loop (runs every 1/2 window size)
fun void doAudio()
{
    while( true )
    {
        // upchuck to process accum
        accum.upchuck();
        // get the last window size samples (waveform)
        accum.output( samples );
        // upchuck to take FFT, get magnitude response
        fft.upchuck();
        // get spectrum (as complex values)
        fft.spectrum( response );
        // jump by samples
        WINDOW_SIZE::samp/2 => now;
    }
}
spork ~ doAudio();

// map FFT output to scalar values
fun void map2spectrum( complex in[], float out[] )
{
    if( in.size() != out.size() )
    {
        <<< "size mismatch in map2spectrum()", "" >>>;
        return;
    }
    
    // mapping to scalar value
    for (int i; i < in.size(); i++)
    {
        // map frequency bin magnitude
        25 * Math.pow( (in[i]$polar).mag, .5 ) => out[i];
    }
}

// our custom audio terrain shader
"
#include FRAME_UNIFORMS
#include DRAW_UNIFORMS
#include STANDARD_VERTEX_INPUT

struct VertexOutput {
    @builtin(position) position : vec4<f32>,
    @location(0) v_height : f32,
};

// our custom material uniforms
// @group(1) @binding(0) var u_sampler : sampler;
@group(1) @binding(0) var u_height_map : texture_2d<f32>;
@group(1) @binding(1) var<uniform> u_playhead : i32;
@group(1) @binding(2) var<uniform> u_color : vec3f;
@group(1) @binding(3) var<uniform> u_repetitions : f32;
@group(1) @binding(4) var<uniform> u_mirror: i32;

@vertex 
fn vs_main(in : VertexInput) -> VertexOutput
{
    var out : VertexOutput;
    let u_draw : DrawUniforms = u_draw_instances[in.instance];

    let heightmap_dim = textureDimensions(u_height_map);

    var mirrored_uv = in.uv;
    mirrored_uv.x = fract(in.uv.x * u_repetitions);
    if (bool(u_mirror)) {
        mirrored_uv.x = abs(mirrored_uv.x * 2.0 - 1.0);
    }
    var sample_coords = vec2i(mirrored_uv * vec2f(heightmap_dim));
    sample_coords.y = u_playhead - sample_coords.y; // scroll the heightmap
    if (sample_coords.y < 0) {
        sample_coords.y += i32(heightmap_dim.y);
    }
    if (sample_coords.y >= i32(heightmap_dim.y)) {
        sample_coords.y = sample_coords.y % i32(heightmap_dim.y);
    }

    // mirror the uv.x

    let heightmap = textureLoad(u_height_map, sample_coords, 0).r;
    let heightmap_scaled_pos = in.position + (heightmap * in.normal);
    let worldpos = u_draw.model * vec4f(heightmap_scaled_pos, 1.0f);

    out.v_height = heightmap;

    // let worldpos = u_draw.model * vec4f(in.position, 1.0f);

    out.position = (u_frame.projection * u_frame.view) * worldpos;
    // out.v_worldpos = worldpos.xyz;

    return out;
}

// don't actually need normals/tangents
@fragment 
fn fs_main(in : VertexOutput) -> @location(0) vec4f
{
    let color_scale = pow((in.v_height / 8.0), .5) + .05;

    let alpha = clamp(color_scale, 0.0, 1.0);
    return vec4f(vec3f(u_color * color_scale), alpha);
}
" @=> string audio_terrain_shader_string;

// Initialize our spectrum height map texture
TextureDesc spectrum_texture_desc;
WINDOW_SIZE => spectrum_texture_desc.width;
WATERFALL_DEPTH => spectrum_texture_desc.height;
Texture.Format_R32Float => spectrum_texture_desc.format; // single channel float (to hold spectrum data)
1 => spectrum_texture_desc.mips; // no mipmaps

Texture spectrum_texture(spectrum_texture_desc);

 // initialize TextureWrite params for writing spectrum data to texture
TextureWriteDesc write_desc;
WINDOW_SIZE => write_desc.width;

// Create our custom audio shader
ShaderDesc shader_desc;
audio_terrain_shader_string => shader_desc.vertexCode;
audio_terrain_shader_string => shader_desc.fragmentCode;
Shader terrain_shader(shader_desc); // create shader from shader_desc

// Apply the shader to a material
Material terrain_material;
terrain_shader => terrain_material.shader;
// assign the spectrum texture to the material
terrain_material.texture(0, spectrum_texture);
terrain_material.uniformInt(1, 0); // initialize playhead to 0
terrain_material.uniformFloat3(2, Color.WHITE);
terrain_material.uniformFloat2(3, @(1,1));
terrain_material.uniformInt(4, true);
terrain_material.topology(Material.Topology_LineList);

// create our terrain mesh
PlaneGeometry plane_geo(
    10,  // width
    10,  // height
    spectrum_texture.width(), // width segments
    spectrum_texture.height()// height segments
);
TorusGeometry torus_geo(
    5,  // radius
    2,  // tube radius
    spectrum_texture.height(), // radial segments
    spectrum_texture.width(), // tubular segments
    Math.PI * 2 // arc length
);

[torus_geo, plane_geo] @=> Geometry geometries[];
["Torus", "Plane"] @=> string geo_names[];


// GMesh terrain_mesh(plane_geo, terrain_material) --> GG.scene();
GMesh terrain_mesh(torus_geo, terrain_material) --> GG.scene();
terrain_mesh.rotateX(-Math.PI/2);
terrain_mesh.rotateZ(-Math.PI/2);

// camera
GOrbitCamera cam --> GG.scene();
GG.scene().camera(cam);
cam.posZ(15);
cam.posY(10);

// UI variables
UI_Int geo_index;
UI_Float3 terrain_color(Color.WHITE);
UI_Float repetitions(1);
UI_Bool mirrored(true);

// game loop (runs at frame rate) ============================================

while (true) {
    GG.nextFrame() => now;
    // map FFT response to scalar values
    map2spectrum( response, spectrum );
    // write to texture
    {
        spectrum_texture.write( spectrum, write_desc );

        // update the playhead
        terrain_material.uniformInt(1, write_desc.y);

        // bump the row we write to next frame
        (write_desc.y + 1) % WATERFALL_DEPTH => write_desc.y;
    }

    // UI
    if (UI.begin("Audio Terrain")) {
        UI.scenegraph(GG.scene());

        if (UI.listBox("Geometry", geo_index, geo_names)) {
            terrain_mesh.geometry(geometries[geo_index.val()]);
        }

        if (UI.drag("Repetitions", repetitions, 0.01, 0.1, 0, "%.2f", 0)) {
            terrain_material.uniformFloat(3, repetitions.val());
        }

        if (UI.checkbox("Mirrored", mirrored)) {
            terrain_material.uniformInt(4, mirrored.val());
        }

        if (UI.colorEdit("Terrain Color", terrain_color, 0)) {
            terrain_material.uniformFloat3(2, terrain_color.val());
        }
    }
    UI.end();
}
