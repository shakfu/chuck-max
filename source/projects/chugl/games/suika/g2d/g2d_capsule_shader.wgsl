#include FRAME_UNIFORMS
#include DRAW_UNIFORMS

/*
each capsule has
- vec2 p1
- vec2 p2
- f32 radius
- vec4 color
*/

const a = 1.1f;
var<private> QUAD_VERTICES : array<vec2f, 6> = array(
    vec2f(-a, -a), // bottom left
    vec2f(a, -a),  // bottom right
    vec2f(-a, a),  // top left
    vec2f(a, -a),  // bottom right
    vec2f(a, a),   // top right
    vec2f(-a, a)   // top left
);

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) v_pos: vec2f, // scaled+transformed position in quad
    @location(1) @interpolate(flat) v_p1 : vec2f,
    @location(2) @interpolate(flat) v_p2 : vec2f,
    @location(3) @interpolate(flat) v_radius : f32,
    @location(4) v_color: vec4f,
};

@group(1) @binding(0) var<storage> u_p1p2 : array<vec4f>; // .xy is p1, .zw is p2
@group(1) @binding(1) var<storage> u_radius : array<f32>;
@group(1) @binding(2) var<storage> u_color : array<vec4f>;
@group(1) @binding(3) var<uniform> u_antialias : i32;

@vertex
fn vs_main(
    @builtin(instance_index) instance_idx : u32,    // carry over from everything being indexed...
    @builtin(vertex_index) vertex_idx : u32,        // used to determine which polygon we are drawing
) -> VertexOutput {
    var out : VertexOutput;

    // compute which polygon we are drawing in this batch
    let capsule_idx =  vertex_idx / 6u;
    let quad_vertex : vec2f = QUAD_VERTICES[vertex_idx % 6u]; 

    let p1p2 = u_p1p2[capsule_idx];
    let p1 = p1p2.xy;
    let p2 = p1p2.zw;
    let radius = u_radius[capsule_idx];

    // compute aabb
    let radius2 = vec2f(radius);
    let aabb = vec4f(
        min(p1 - radius2, p2 - radius2),
        max(p1 + radius2, p2 + radius2)
    );
    // how much to scale our quad vertices by
    let lower = aabb.xy;
    let upper = aabb.zw;

    let width : vec2f = upper - lower;
    let center : vec2f = 0.5 * (lower + upper);
    let maxWidth : f32 = max(width.x, width.y);
    let scale : f32 = 0.5 * maxWidth;

    let p = scale * quad_vertex + center; // transform quad to world space
    var u_Draw : DrawUniforms = u_draw_instances[instance_idx];
    out.position = (u_frame.projection * u_frame.view) * u_Draw.model * vec4f(p, 0.0, 1.0f);

    out.v_pos = p;
    out.v_p1 = p1;
    out.v_p2 = p2;
    out.v_radius = radius;
    out.v_color = u_color[capsule_idx];

    return out;
}

// begin fragment shader ----------------------------

// https://iquilezles.org/articles/distfunctions2d/
fn sdSegment( p : vec2f , a : vec2f, b : vec2f, radius : f32) -> f32
{
    let pa = p-a;
    let ba = b-a;
    let h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length( pa - ba*h ) - radius;
}

// derivative anti-aliasing from http://www.numb3r23.net/2015/08/17/using-fwidth-for-distance-based-anti-aliasing/
@fragment
fn fs_main(in : VertexOutput) -> @location(0) vec4f {
    let sdf = sdSegment(in.v_pos, in.v_p1, in.v_p2, in.v_radius);
    var alpha = 1.0f;

    if (bool(u_antialias)) {
        let aaf = fwidth(sdf); // anti alias field
        alpha = smoothstep(aaf, 0.0, sdf);
    } else {
        alpha = step(sdf, 0.0);
    }
    
    if (alpha < .01) { discard; }

    return vec4f(in.v_color.rgb, alpha);
}