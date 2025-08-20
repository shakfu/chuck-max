#include FRAME_UNIFORMS
#include DRAW_UNIFORMS

/*
each circle has
- vec2 center 
- f32 radius
- vec4 color
- f32 thickness (relative, [0, 1], 0 being just outline, 1 being solid)
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
    @location(0) v_local_pos: vec2f, // local position in bounding box, between [0, 1]
    @location(1) @interpolate(flat) v_thickness: f32,
    @location(2) v_color: vec4f,
};

@group(1) @binding(0) var<storage> u_center_radius_thickness : array<vec4f>; // .xy is center, .z is radius, .w is thickness
@group(1) @binding(1) var<storage> u_color : array<vec4f>;
@group(1) @binding(2) var<uniform> u_antialias : i32;

@vertex
fn vs_main(
    @builtin(instance_index) instance_idx : u32,    // carry over from everything being indexed...
    @builtin(vertex_index) vertex_idx : u32,        // used to determine which polygon we are drawing
) -> VertexOutput {
    var out : VertexOutput;

    // compute which polygon we are drawing in this batch
    let circle_idx =  vertex_idx / 6u;
    let quad_vertex : vec2f = QUAD_VERTICES[vertex_idx % 6u]; 

    let center_radius_thickness = u_center_radius_thickness[circle_idx];
    let center = center_radius_thickness.xy;
    let radius = center_radius_thickness.z;
    let thickness = center_radius_thickness.w;


    let p = radius * quad_vertex + center; // transform quad to world space
    var u_Draw : DrawUniforms = u_draw_instances[instance_idx];
    out.position = (u_frame.projection * u_frame.view) * u_Draw.model * vec4f(p, 0.0, 1.0f);

    out.v_local_pos = quad_vertex;
    out.v_thickness = thickness;
    out.v_color = u_color[circle_idx];

    return out;
}

// begin fragment shader ----------------------------
// derivative anti-aliasing from http://www.numb3r23.net/2015/08/17/using-fwidth-for-distance-based-anti-aliasing/
@fragment
fn fs_main(in : VertexOutput) -> @location(0) vec4f {
    let inner_radius = 1.0 - in.v_thickness;
    let d = length(in.v_local_pos);
    let sdf = max(d - 1.0, inner_radius - d); // distance from circle outline

    var alpha = 1.0;
    if (bool(u_antialias)) {
        let aaf = fwidth(sdf); // anti alias field
        alpha = smoothstep(inner_radius - aaf, inner_radius, d) - 
                    smoothstep(1.0 - aaf, 1.0, d);
    } else {
        alpha = step(sdf, 0.0);
    }
    
    if (alpha < .01) { discard; }

    return vec4f(in.v_color.rgb, alpha);
}