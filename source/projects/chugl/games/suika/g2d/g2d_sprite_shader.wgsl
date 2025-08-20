#include FRAME_UNIFORMS
// struct FrameUniforms {
//     projection: mat4x4f,
//     view: mat4x4f,
//     projection_view_inverse_no_translation: mat4x4f,
//     camera_pos: vec3f,
//     time: f32,
//     ambient_light: vec3f,
//     num_lights: i32,
//     background_color: vec4f,
// };
// @group(0) @binding(0) var<uniform> u_frame: FrameUniforms;


#include DRAW_UNIFORMS
// struct DrawUniforms {
//     model: mat4x4f,
//     id: u32
// };
// @group(2) @binding(0) var<storage> u_draw_instances: array<DrawUniforms>;

/*
vec2 center
vec2 scale (width height)
float rotation radians ccw
vec4 color
texture
sampler
*/

@group(1) @binding(0) var<storage> u_positions : array<vec2f>;
@group(1) @binding(1) var<storage> u_scales :    array<vec2f>; // width, height
@group(1) @binding(2) var<storage> u_rotations : array<f32>;
@group(1) @binding(3) var<storage> u_colors :    array<vec4f>;
@group(1) @binding(4) var          texture_map : texture_2d<f32>;
@group(1) @binding(5) var          texture_sampler: sampler;

const a = 0.5f;
var<private> QUAD_VERTICES : array<vec2f, 6> = array(
    vec2f(-a, -a), // bottom left
    vec2f(a, -a),  // bottom right
    vec2f(-a, a),  // top left
    vec2f(a, -a),  // bottom right
    vec2f(a, a),   // top right
    vec2f(-a, a)   // top left
);

var<private> QUAD_UVS : array<vec2f, 6> = array(
    vec2f(0, 0), // bottom left
    vec2f(1, 0),  // bottom right
    vec2f(0, 1),  // top left
    vec2f(1, 0),  // bottom right
    vec2f(1, 1),   // top right
    vec2f(0, 1)   // top left
);

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) v_color: vec4f, 
    @location(1) v_uv: vec2f,
};

@vertex
fn vs_main(
    @builtin(instance_index) instance_idx : u32,    // carry over from everything being indexed...
    @builtin(vertex_index) vertex_idx : u32,        // used to determine which polygon we are drawing
) -> VertexOutput {
    var out : VertexOutput;

    // compute which sprite we are drawing in this batch
    let sprite_instance_idx =  vertex_idx / 6u;
    let quad_vertex : vec2f = QUAD_VERTICES[vertex_idx % 6u]; 
    out.v_uv = QUAD_UVS[vertex_idx % 6u];
    out.v_color = u_colors[sprite_instance_idx];

    // apply 2d rotation and scale transformation
    let pos = u_positions[sprite_instance_idx];
    let sca = u_scales[sprite_instance_idx];
    let rot = u_rotations[sprite_instance_idx];
    let c : f32 = cos(rot);
    let s : f32 = sin(rot);
    var p = quad_vertex * sca;
    p = vec2f((c * p.x - s * p.y), (s * p.x + c * p.y)) + pos;

    var u_Draw : DrawUniforms = u_draw_instances[instance_idx];
    out.position = (u_frame.projection * u_frame.view) * u_Draw.model * vec4f(p, 0.0, 1.0f);

    return out;
}

// begin fragment shader ----------------------------

@fragment
fn fs_main(in : VertexOutput) -> @location(0) vec4f {
    let color: vec4<f32> = in.v_color * textureSample(texture_map, texture_sampler, in.v_uv);
    if (color.a < .01) { discard; }
    return color;
}