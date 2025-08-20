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

// TODO pixel scale material uniform (or just simplify to have no line outline)

#include DRAW_UNIFORMS
// struct DrawUniforms {
//     model: mat4x4f,
//     id: u32
// };
// @group(2) @binding(0) var<storage> u_draw_instances: array<DrawUniforms>;

// struct Polygon {
    // transform : vec4f, // x, y, sin rotation, cos rotation
    // points12 : vec4f, // x1, y1, x2, y2
    // points34 : vec4f, // x3, y3, x4, y4
    // points56 : vec4f, // x5, y5, x6, y6
    // points78 : vec4f, // x7, y7, x8, y8
    // color : vec4f,
    // aabb : vec4f,     // lower left, upper right
    // count : i32,      // number of vertices
//     radius : f32,
// };
// @group(3) @binding(0) var<storage> u_polygon_instances : array<Polygon>;

// using material @group(1) because currently SG_GEOMETRY_MAX_VERTEX_PULL_BUFFERS = 4
// vertex counts in the format [ (offset0, count0), (offset1, count1) ...]
// offset is into u_polygon_vertices
@group(1) @binding(0) var<storage> u_polygon_vertex_counts : array<i32>;
@group(1) @binding(1) var<storage> u_polygon_vertices : array<f32>; // x1, y1, x2, y2 ... vec2f
@group(1) @binding(2) var<storage> u_polygon_transforms : array<f32>; // every 3 floats is a transform, x, y, cos rotation, sin rot radians vecf4f
@group(1) @binding(3) var<storage> u_polygon_colors: array<f32>; // every 4 floats is a color vec4f
@group(1) @binding(4) var<storage> u_polygon_aabb: array<f32>; // lower left, upper right vec4f
@group(1) @binding(5) var<storage> u_polygon_radius: array<f32>; // f32




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
    @location(0) f_position: vec2f, // local vertex position, ranging from -a to a
    @location(1) f_color: vec4f,
    @location(2) @interpolate(flat) f_count: i32, // number of vertices in polygon f_points
    @location(3) f_radius: f32,
    @location(4) f_thickness: f32,
    @location(5) @interpolate(flat) f_points12: vec4f,
    @location(6) @interpolate(flat) f_points34: vec4f,
    @location(7) @interpolate(flat) f_points56: vec4f,
    @location(8) @interpolate(flat) f_points78: vec4f,

    // @location(5) @interpolate(flat) f_points: array<vec2f, 8>,  // why would we want to interpolate this??
    // out vec2 f_position;
    // out vec4 f_color;
    // out vec2 f_points[8];
    // flat out int f_count;
    // out float f_radius;
    // out float f_thickness;
};

@vertex
fn vs_main(
    @builtin(instance_index) instance_idx : u32,    // carry over from everything being indexed...
    @builtin(vertex_index) vertex_idx : u32,        // used to determine which polygon we are drawing
) -> VertexOutput {
    var out : VertexOutput;

    // compute which polygon we are drawing in this batch
    let polygon_instance_idx =  vertex_idx / 6u;
    let quad_vertex : vec2f = QUAD_VERTICES[vertex_idx % 6u]; 

    // let polygon = u_polygon_instances[polygon_instance_idx];
    // access polygon params
    let polygon_color = vec4f(
        u_polygon_colors[4 * polygon_instance_idx + 0],
        u_polygon_colors[4 * polygon_instance_idx + 1],
        u_polygon_colors[4 * polygon_instance_idx + 2],
        u_polygon_colors[4 * polygon_instance_idx + 3],
    );
    let polygon_radius = u_polygon_radius[polygon_instance_idx];
    let polygon_vertex_offset = u_polygon_vertex_counts[2 * polygon_instance_idx + 0];
    let polygon_vertex_count = u_polygon_vertex_counts[2 * polygon_instance_idx + 1];
    let polygon_aabb = vec4f(
        u_polygon_aabb[4 * polygon_instance_idx + 0],
        u_polygon_aabb[4 * polygon_instance_idx + 1],
        u_polygon_aabb[4 * polygon_instance_idx + 2],
        u_polygon_aabb[4 * polygon_instance_idx + 3],
    );
    let polygon_transform = vec4f(
        u_polygon_transforms[4 * polygon_instance_idx + 0],
        u_polygon_transforms[4 * polygon_instance_idx + 1],
        u_polygon_transforms[4 * polygon_instance_idx + 2],
        u_polygon_transforms[4 * polygon_instance_idx + 3],
    );

    out.f_position = quad_vertex;
    out.f_color = polygon_color;
    out.f_radius = polygon_radius;
    out.f_count = polygon_vertex_count;


    // Compute polygon AABB
    let lower = polygon_aabb.xy;
    let upper = polygon_aabb.zw;

    let center : vec2f = 0.5 * (lower + upper);
    let width : vec2f = upper - lower;
    let maxWidth : f32 = max(width.x, width.y);

    let scale : f32 = out.f_radius + 0.5 * maxWidth;
    let invScale = 1.0 / scale;

    var f_points = array<vec2f, 8>(); // 8 is max vertices per polygon
    // Shift and scale polygon points so they fit in 2x2 quad
    for (var i = 0; i < polygon_vertex_count; i++)
    {
        f_points[i] = invScale * (
            vec2f(
                u_polygon_vertices[2 * (polygon_vertex_offset + i) + 0],
                u_polygon_vertices[2 * (polygon_vertex_offset + i) + 1]
            ) - center
        );
        // f_points[i] = invScale * (f_points[i] - center);
        // out.f_points[i] = invScale * (out.f_points[i] - center);
    }

    // pass scaled points to frag shader
    out.f_points12 = vec4f(f_points[0], f_points[1]);
    out.f_points34 = vec4f(f_points[2], f_points[3]);
    out.f_points56 = vec4f(f_points[4], f_points[5]);
    out.f_points78 = vec4f(f_points[6], f_points[7]);

    // Scale radius as well
    out.f_radius = invScale * out.f_radius;

    // resolution.y = pixelScale * scale
    // TODO: set pixelScale?
    // out.f_thickness = 3.0f / (pixelScale * scale);
    out.f_thickness = 3.0f / (80 * scale);

    // scale up and transform quad to fit polygon
    let x : f32 = polygon_transform.x;
    let y : f32 = polygon_transform.y;
    let c : f32 = polygon_transform.z;
    let s : f32 = polygon_transform.w;
    var p = scale * quad_vertex + center;
    p = vec2f((c * p.x - s * p.y) + x, (s * p.x + c * p.y) + y);

    var u_Draw : DrawUniforms = u_draw_instances[instance_idx];
    out.position = (u_frame.projection * u_frame.view) * u_Draw.model * vec4f(p, 0.0, 1.0f);

    return out;
}

// begin fragment shader ----------------------------

// https://en.wikipedia.org/wiki/Alpha_compositing
fn blend_colors(front : vec4f, back : vec4f) -> vec4f
{
    let cSrc = front.rgb;
    let alphaSrc = front.a;
    let cDst = back.rgb;
    let alphaDst = back.a;

    var cOut = cSrc * alphaSrc + cDst * alphaDst * (1.0 - alphaSrc);
    let alphaOut = alphaSrc + alphaDst * (1.0 - alphaSrc);

    // remove alpha from rgb
    cOut = cOut / alphaOut;

    return vec4f(cOut, alphaOut);
}

fn cross2d(v1 : vec2f, v2 : vec2f) -> f32
{
    return v1.x * v2.y - v1.y * v2.x;
}

// Signed distance function for convex polygon
fn sdConvexPolygon(p : vec2f, v : ptr<function, array<vec2f, 8>>, count : i32) -> f32
{
    // Initial squared distance
    // var d = dot(p - v[0], p - v[0]);
    var d = dot(p - (*v)[0], p - (*v)[0]);

    // Consider query point inside to start
    var side = -1.0;
    var j = count - 1;
    for (var i = 0; i < count; i++)
    {
        // Distance to a polygon edge
        let e : vec2f = (*v)[i] - (*v)[j];
        let w : vec2f = p - (*v)[j];
        let we : f32 = dot(w, e);
        let b : vec2f = w - e * clamp(we / dot(e, e), 0.0, 1.0);
        let bb : f32 = dot(b, b);

        // Track smallest distance
        if (bb < d)
        {
            d = bb;
        }

        // If the query point is outside any edge then it is outside the entire polygon.
        // This depends on the CCW winding order of points.
        let s : f32 = cross2d(w, e);
        if (s >= 0.0)
        {
            side = 1.0;
        }

        j = i;
    }

    return side * sqrt(d);
}

@fragment
fn fs_main(in : VertexOutput) -> @location(0) vec4f {
    var f_points = array(
        in.f_points12.xy, in.f_points12.zw,
        in.f_points34.xy, in.f_points34.zw,
        in.f_points56.xy, in.f_points56.zw,
        in.f_points78.xy, in.f_points78.zw
    );
    let sdf = sdConvexPolygon(in.f_position, &f_points, in.f_count);

    // TODO: anti-aliasing broken, fix later
    // reference: http://www.numb3r23.net/2015/08/17/using-fwidth-for-distance-based-anti-aliasing/
    var alpha = 1.0;
    // if (bool(u_antialias)) {
    //     let aaf = fwidth(sdf); // anti alias field
    //     alpha = smoothstep(inner_radius - aaf, inner_radius, d) - 
    //                 smoothstep(1.0 - aaf, 1.0, d);
    // } else {
        alpha = step(sdf, 0.0);
    // }


    if (alpha < .01) { discard; }

    return vec4(in.f_color.rgb, alpha);
}