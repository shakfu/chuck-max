/*----------------------------------------------------------------------------
 ChuGL: Unified Audiovisual Programming in ChucK

 Copyright (c) 2023 Andrew Zhu Aday and Ge Wang. All rights reserved.
   http://chuck.stanford.edu/chugl/
   http://chuck.cs.princeton.edu/chugl/

 MIT License

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
-----------------------------------------------------------------------------*/
#pragma once

#include <glm/glm.hpp>

#include "core/macros.h"

#include <algorithm>
#include <unordered_map>

struct ShaderEntry {
    const char* name;
    const char* code;
};

#define PER_FRAME_GROUP 0
#define PER_MATERIAL_GROUP 1
#define PER_DRAW_GROUP 2
#define VERTEX_PULL_GROUP 3

#define VS_ENTRY_POINT "vs_main"
#define FS_ENTRY_POINT "fs_main"
#define COMPUTE_ENTRY_POINT "main"

// #define STRINGIFY(s) #s
// #define INTERPOLATE(var) STRINGIFY(${##var##})

// TODO try removing num_lights and use arrayLength(&u_lights) instead
struct FrameUniforms {
    // camera
    glm::mat4x4 projection;                             // at byte offset 0
    glm::mat4x4 view;                                   // at byte offset 64
    glm::mat4x4 projection_view_inverse_no_translation; // at byte offset 128
    glm::vec3 camera_pos;                               // at byte offset 192
    float _pad0;

    // lighting
    glm::vec3 ambient_light;    // at byte offset 208
    int32_t num_lights;         // at byte offset 220
    glm::vec4 background_color; // at byte offset 224

    // general frame info
    glm::ivec3 resolution;  // at byte offset 240
    float time;             // at byte offset 252
    float delta_time;       // at byte offset 256
    int32_t frame_count;    // at byte offset 260
    glm::vec2 mouse;        // at byte offset 264
    glm::ivec2 mouse_click; // at byte offset 272
    float sample_rate;      // at byte offset 280
    float _pad1;
};

void FrameUniforms_ZeroCameraFields(FrameUniforms* f)
{
    f->projection                             = {};
    f->view                                   = {};
    f->projection_view_inverse_no_translation = {};
    f->camera_pos                             = {};
}

void FrameUniforms_ZeroLightingFields(FrameUniforms* f)
{
    f->ambient_light    = {};
    f->num_lights       = 0;
    f->background_color = {};
}

struct LightUniforms {
    glm::vec3 color;    // at byte offset 0
    int32_t light_type; // at byte offset 12
    glm::vec3 position; // at byte offset 16
    float _pad0;
    glm::vec3 direction;          // at byte offset 32
    float point_and_spot_radius;  // at byte offset 44
    float point_and_spot_falloff; // at byte offset 48
    float spot_cos_angle_min;     // at byte offset 52
    float spot_cos_angle_max;     // at byte offset 56
    float spot_angular_falloff;

    glm::mat4x4 proj_view;     // at byte offset 64
    int32_t generates_shadows; // at byte offset 128
    int32_t shadow_map_idx;    // at byte offset 132
    float bias;
    float _pad1[1];
};

struct DrawUniforms {
    glm::mat4x4 model;       // at byte offset 0
    glm::mat4x4 normal;      // at byte offset 64
    int32_t id;              // at byte offset 128
    int32_t receives_shadow; // at byte offset 132
    float _pad0[2];
};

struct b2_DebugDraw_SolidPolygon {
    glm::vec4 transform; // at byte offset 0
    glm::vec4 points12;  // at byte offset 16
    glm::vec4 points34;  // at byte offset 32
    glm::vec4 points56;  // at byte offset 48
    glm::vec4 points78;  // at byte offset 64
    glm::vec4 color;     // at byte offset 80
    glm::vec4 aabb;      // at byte offset 96
    int32_t count;       // at byte offset 96
    float radius;        // at byte offset 100
    float _pad0[2];
};

// clang-format off

static std::unordered_map<std::string, std::string> shader_table = {
    {
        "FRAME_UNIFORMS", 
        R"glsl(

        struct FrameUniforms {
            // scene params (only set in ScenePass, otherwise 0)
            projection: mat4x4f,
            view: mat4x4f,
            projection_view_inverse_no_translation: mat4x4f,
            camera_pos: vec3f,
            ambient_light: vec3f,
            num_lights: i32,
            background_color: vec4f,

            // general params (include in all passes except ComputePass)
            resolution: vec3i,      // window viewport resolution 
            time: f32,              // time in seconds since the graphics window was opened
            delta_time: f32,        // time since last frame (in seconds)
            frame_count: i32,       // frames since window was opened
            mouse: vec2f,           // normalized mouse coords (range 0-1, (0,0) is bottom left)
            mouse_click: vec2i,     // mouse click state
            sample_rate: f32        // chuck VM sound sample rate (e.g. 44100)
        };
                

        @group(0) @binding(0) var<uniform> u_frame: FrameUniforms;

        )glsl"
    },
    { // for dynamic lights only
        "LIGHTING_UNIFORMS",
        R"glsl(

        // light types
        const LightType_None = 0;
        const LightType_Directional = 1;
        const LightType_Point = 2;
        const LightType_Spot = 3;

        struct LightUniforms {
            color : vec3f,
            light_type: i32,
            position: vec3f,
            direction: vec3f, 

            // point AND spot light
            point_and_spot_radius: f32,
            point_and_spot_falloff: f32,

            // spot light
            spot_cos_angle_min: f32,
            spot_cos_angle_max: f32,
            spot_angular_falloff: f32,

            // shadow info
            proj_view: mat4x4f,
            generates_shadows: i32,
            shadow_map_array_layer: i32,
            bias: f32,
        };

        @group(0) @binding(1) var<storage, read> u_lights: array<LightUniforms>;

        )glsl"
    },
    { // for environment mapping
        "ENVIRONMENT_MAP_UNIFORMS",
        R"glsl(
        @group(0) @binding(2) var u_envmap: texture_cube<f32>;
        )glsl"
    },
    {
        "DRAW_UNIFORMS", 
        R"glsl(

        struct DrawUniforms {
            model: mat4x4f,
            normal: mat4x4f,
            id: i32,
            receives_shadow: i32,
        };

        @group(2) @binding(0) var<storage> u_draw_instances: array<DrawUniforms>;

        )glsl"
    },

    {
        "STANDARD_VERTEX_INPUT", // vertex input for standard 3D objects (pos, normal, uv)
        R"glsl(

        struct VertexInput {
            @location(0) position : vec3f,
            @location(1) normal : vec3f,
            @location(2) uv : vec2f,
            @builtin(instance_index) instance : u32,
        };

        )glsl"
    },

    {
        "STANDARD_VERTEX_OUTPUT", // vertex output for standard 3D objects (pos, normal, uv)
        R"glsl(

        struct VertexOutput {
            @builtin(position) position : vec4f,
            @location(0) v_worldpos : vec3f,
            @location(1) v_normal : vec3f,
            @location(2) v_uv : vec2f,
            @location(3) @interpolate(flat) receives_shadow: i32,
        };

        )glsl"
    },

    {
        "STANDARD_VERTEX_SHADER",
        R"glsl(
        @vertex 
        fn vs_main(in : VertexInput) -> VertexOutput
        {
            var out : VertexOutput;
            var u_Draw : DrawUniforms = u_draw_instances[in.instance];

            let modelMat3 : mat3x3<f32> = mat3x3(
                u_Draw.model[0].xyz,
                u_Draw.model[1].xyz,
                u_Draw.model[2].xyz
            );

            let worldpos = u_Draw.model * vec4f(in.position, 1.0f);
            out.position = (u_frame.projection * u_frame.view) * worldpos;
            out.v_worldpos = worldpos.xyz;

            out.v_normal = (u_Draw.normal * vec4f(in.normal, 0.0)).xyz;

            out.v_uv     = in.uv;
            out.receives_shadow = u_Draw.receives_shadow;

            return out;
        }
        )glsl"
    },

    {
        "SCREEN_PASS_VERTEX_SHADER",
        R"glsl(
        struct VertexOutput {
            @builtin(position) position : vec4<f32>,
            @location(0) v_uv : vec2<f32>,
        };

        @vertex 
        fn vs_main(@builtin(vertex_index) vertexIndex : u32) -> VertexOutput {
            var output : VertexOutput;

            // a triangle which covers the screen
            output.v_uv = vec2f(f32((vertexIndex << 1u) & 2u), f32(vertexIndex & 2u));
            output.position = vec4f(output.v_uv * 2.0 - 1.0, 0.0, 1.0);
            
            // if (vertexIndex == 0u) {
            //     output.position = vec4f(-1.0, -1.0, 0.0, 1.0);
            //     output.v_uv = vec2f(0.0, 0.0);
            // } else if (vertexIndex == 1u) {
            //     output.position = vec4f(3.0, -1.0, 0.0, 1.0);
            //     output.v_uv = vec2f(2.0, 0.0);
            // } else {
            //     output.position = vec4f(-1.0, 3.0, 0.0, 1.0);
            //     output.v_uv = vec2f(0.0, 2.0);
            // }

            // flip y (webgpu render textures are flipped)
            output.v_uv.y = 1.0 - output.v_uv.y;
            return output;
        }
        )glsl"
    }, 

    {
        "NORMAL_MAPPING_FUNCTIONS",
        R"glsl(
        // http://www.thetenthplanet.de/archives/1180
        fn cotangentFrame( N : vec3f, p : vec3f, uv : vec2f ) -> mat3x3f { 
            // get edge vectors of the pixel triangle 
            let dp1 = dpdx( p ); 
            let dp2 = dpdy( p ); 
            let duv1 = dpdx( uv ); 
            let duv2 = dpdy( uv );   
            // solve the linear system 
            let dp2perp = cross( dp2, N ); 
            let dp1perp = cross( N, dp1 ); 
            let T = dp2perp * duv1.x + dp1perp * duv2.x;
            let B = dp2perp * duv1.y + dp1perp * duv2.y;
            // construct a scale-invariant frame 
            let invmax = inverseSqrt( max( dot(T,T), dot(B,B) ) ); 
            return mat3x3f( T * invmax, B * invmax, N ); 
        }

        fn perturbNormal( v_normal : vec3f, V : vec3f, texcoord : vec2f, scale : f32, is_front : bool) -> vec3f { 
            let N = normalize(v_normal);

            // assume N, the interpolated vertex normal and 
            // V, the view vector (vertex to eye) 
            var map = textureSample(u_normal_map, texture_sampler, texcoord).xyz * 2.0 - 1.0;
            map.x *= scale;
            map.y *= -scale; // flip y too to be consistent with previuos normal map version

            let TBN = cotangentFrame( N, -V, texcoord ); 
            var normal = normalize( TBN * map ); 
            if (!is_front) {
                normal = -normal;
            }
            return normal;
        }
        )glsl"
    }

    // TODO lighting
    // TODO normal matrix
    // TODO helper fns (srgb to linear, linear to srgb, etc)
};


static const char* uv_shader_string  = R"glsl(
#include FRAME_UNIFORMS
#include DRAW_UNIFORMS
#include STANDARD_VERTEX_INPUT

struct VertexOutput {
    @builtin(position) position : vec4f,
    @location(0) v_uv : vec2f,
};

@vertex 
fn vs_main(in : VertexInput) -> VertexOutput
{
    var out : VertexOutput;
    var u_Draw : DrawUniforms = u_draw_instances[in.instance];

    out.position = (u_frame.projection * u_frame.view) * u_Draw.model * vec4f(in.position, 1.0f);
    out.v_uv = in.uv;

    return out;
}

@fragment 
fn fs_main(in : VertexOutput) -> @location(0) vec4f
{
    return vec4f(in.v_uv, 0.0, 1.0);
}
)glsl";

// https://web.archive.org/web/20130424093557/http://codeflow.org/entries/2012/aug/02/easy-wireframe-display-with-barycentric-coordinates/
static const char* wireframe_shader_string = R"glsl(
    #include FRAME_UNIFORMS
    #include DRAW_UNIFORMS
    #include STANDARD_VERTEX_INPUT

    @group(1) @binding(0) var<uniform> u_thickness: f32;
    @group(1) @binding(1) var<uniform> u_alpha_threshold: f32;
    @group(1) @binding(2) var<uniform> u_color : vec3f;

    struct VertexOutput {
        @builtin(position) position: vec4f,
        @location(0) barycentric: vec3f,
    };

    @vertex 
    fn vs_main(
        @builtin(vertex_index) vNdx: u32,
        in: VertexInput
    ) -> VertexOutput {
        var out : VertexOutput;
        var u_Draw : DrawUniforms = u_draw_instances[in.instance];

        out.position = (u_frame.projection * u_frame.view) * u_Draw.model * vec4f(in.position, 1.0f);
        // emit a barycentric coordinate
        out.barycentric = vec3f(0);
        out.barycentric[vNdx % 3] = 1.0;

        return out;
    }

    fn edgeFactor(bary: vec3f) -> f32 {
        let d = fwidth(bary);
        let a3 = smoothstep(vec3f(0.0), d * u_thickness, bary);
        return min(min(a3.x, a3.y), a3.z);
    }

    @fragment 
    fn fs_main( v: VertexOutput) -> @location(0) vec4f {
        let a = 1.0 - edgeFactor(v.barycentric);
        if (a < u_alpha_threshold) {
            discard;
        }

        // return vec4((u_color), a);
        return vec4((u_color) * a, a);
    }
)glsl";

static const char* normal_shader_string  = R"glsl(
#include FRAME_UNIFORMS
#include DRAW_UNIFORMS
#include STANDARD_VERTEX_INPUT

struct VertexOutput {
    @builtin(position) position : vec4f,
    @location(0) v_world_normal : vec3f,
    @location(1) v_local_normal : vec3f,
};

@vertex 
fn vs_main(in : VertexInput) -> VertexOutput
{
    var out : VertexOutput;
    var u_Draw : DrawUniforms = u_draw_instances[in.instance];

    out.position = (u_frame.projection * u_frame.view) * u_Draw.model * vec4f(in.position, 1.0f);
    out.v_world_normal = (u_Draw.normal * vec4f(in.normal, 0.0)).xyz;
    out.v_local_normal = in.normal;

    return out;
}


// our custom material uniforms
@group(1) @binding(0) var<uniform> world_space_normals: i32;

@fragment 
fn fs_main(in : VertexOutput, @builtin(front_facing) is_front: bool) -> @location(0) vec4f
{   
    var world_normal = in.v_world_normal;
    var local_normal = in.v_local_normal;
    if (!is_front) {
        world_normal = -world_normal;
        local_normal = -local_normal;
    }

    if (world_space_normals == 1) {
        return vec4f(max(world_normal, vec3f(0.0)), 1.0);
    } else {
        return vec4f(max(local_normal, vec3f(0.0)), 1.0);
    }
}
)glsl";

static const char* flat_shader_string  = R"glsl(
#include FRAME_UNIFORMS
#include DRAW_UNIFORMS
#include STANDARD_VERTEX_INPUT
#include STANDARD_VERTEX_OUTPUT
#include STANDARD_VERTEX_SHADER

// our custom material uniforms
@group(1) @binding(0) var<uniform> u_color : vec4f;
@group(1) @binding(1) var u_sampler : sampler;
@group(1) @binding(2) var u_color_map : texture_2d<f32>;
@group(1) @binding(3) var<uniform> u_texture_offset : vec2f;
@group(1) @binding(4) var<uniform> u_texture_scale : vec2f;

fn srgbToLinear(c : vec4f) -> vec4f {
    return vec4f(
        pow(c.r, 2.2),
        pow(c.g, 2.2),
        pow(c.b, 2.2),
        c.a
    );
}

@fragment 
fn fs_main(in : VertexOutput) -> @location(0) vec4f
{
    let uv = in.v_uv * u_texture_scale + u_texture_offset;
    let tex = srgbToLinear(textureSample(u_color_map, u_sampler, uv));
    var ret = u_color * tex;

    // alpha test
    ret.a = clamp(ret.a, 0.0, 1.0);
    if (ret.a < .01) {
        discard;
    }

    return ret;
}
)glsl";

static const char* phong_shader_string = R"glsl(
    // phong impl based off obj material model
    // actually, blinn* phong
    // see https://www.fileformat.info/format/material/

    #include FRAME_UNIFORMS
    #include LIGHTING_UNIFORMS
    #include ENVIRONMENT_MAP_UNIFORMS
    #include DRAW_UNIFORMS
    #include STANDARD_VERTEX_INPUT
    #include STANDARD_VERTEX_OUTPUT
    #include STANDARD_VERTEX_SHADER

    @group(0) @binding(3) var shadow_sampler: sampler_comparison;
    @group(0) @binding(4) var spot_shadow_map_array: texture_depth_2d_array;
    @group(0) @binding(5) var dir_shadow_map_array: texture_depth_2d_array;

    @group(1) @binding(0) var<uniform> u_specular_color : vec3f;
    @group(1) @binding(1) var<uniform> u_diffuse_color : vec4f;
    @group(1) @binding(2) var<uniform> u_shininess : f32; // range from (0, 2^n). must be > 0. logarithmic scale.
    @group(1) @binding(3) var<uniform> u_emission_color : vec3f;
    @group(1) @binding(4) var<uniform> u_normal_factor : f32;
    @group(1) @binding(5) var<uniform> u_ao_factor : f32; // 0 disables ao

    @group(1) @binding(6) var texture_sampler: sampler;
    @group(1) @binding(7) var u_diffuse_map: texture_2d<f32>;   
    @group(1) @binding(8) var u_specular_map: texture_2d<f32>;
    @group(1) @binding(9) var u_ao_map: texture_2d<f32>;
    @group(1) @binding(10) var u_emissive_map: texture_2d<f32>;
    @group(1) @binding(11) var u_normal_map: texture_2d<f32>;

    // envmap params
    const ENVMAP_METHOD_NONE = 0;
    const ENVMAP_METHOD_REFLECTION = 1;
    const ENVMAP_METHOD_REFRACTION = 2;

    const ENVMAP_BLEND_NONE = 0;
    const ENVMAP_BLEND_ADDITIVE = 1;
    const ENVMAP_BLEND_MULTIPLICATIVE = 2;
    const ENVMAP_BLEND_MIX = 3;

    @group(1) @binding(12) var<uniform> u_envmap_method : i32;
    @group(1) @binding(13) var<uniform> u_envmap_ratio : f32; // refraction ratio
    // @group(1) @binding(14) var u_envmap_sampler : sampler;
    @group(1) @binding(16) var<uniform> u_envmap_blend : i32;
    @group(1) @binding(17) var<uniform> u_envmap_intensity : f32;

    fn srgbToLinear(srgb_in : vec3f) -> vec3f {
        return pow(srgb_in.rgb,vec3f(2.2));
    }

    // calculate envmap contribution
    fn envMapContribution(view_dir : vec3f, norm : vec3f) -> vec3f
    {
        if (u_envmap_method == ENVMAP_METHOD_NONE) {
            return vec3f(0.0);
        }

        var normal = vec3f(0.0);
        if (u_envmap_method == ENVMAP_METHOD_REFLECTION) {
            normal = reflect(-view_dir, norm);
        }
        else if (u_envmap_method == ENVMAP_METHOD_REFRACTION) {
            normal = refract(-view_dir, norm, u_envmap_ratio);
        }
        return srgbToLinear(textureSample(u_envmap, texture_sampler, normal * vec3f(1, 1, -1)).rgb);
    }
 
    #include NORMAL_MAPPING_FUNCTIONS

    fn calculateSpotShadow(worldpos: vec3f, layer: i32, proj_view: mat4x4f, bias: f32) -> f32 {
        let shadow_coord = proj_view * vec4f(worldpos, 1.0);
        var p = shadow_coord.xyz / shadow_coord.w;
        // Y is flipped because texture coords are Y-down.
        p = vec3(
            p.xy * vec2(0.5, -0.5) + vec2(0.5),
            p.z
        );

        // Percentage-closer filtering. Sample texels in the region
        // to smooth the result.
        var visibility = 0.0;
        let oneOverShadowDepthTextureSize = 1.0 / f32(textureDimensions(spot_shadow_map_array).x);
        for (var y = -1; y <= 1; y++) {
            for (var x = -1; x <= 1; x++) {
                let offset = vec2<f32>(vec2(x, y)) * oneOverShadowDepthTextureSize;

                visibility += textureSampleCompare(
                    spot_shadow_map_array, shadow_sampler,
                    p.xy + offset, // coords
                    layer, // array layer
                    p.z - bias // shadowBias
                );
            }
        }
        visibility /= 9.0;

        return visibility;
    }

    fn calculateDirShadow(worldpos: vec3f, layer: i32, proj_view: mat4x4f, bias: f32) -> f32 {
        let shadow_coord = proj_view * vec4f(worldpos, 1.0);
        var p = shadow_coord.xyz / shadow_coord.w;
        // Y is flipped because texture coords are Y-down.
        p = vec3(
            p.xy * vec2(0.5, -0.5) + vec2(0.5),
            p.z
        );

        // Percentage-closer filtering. Sample texels in the region
        // to smooth the result.
        var visibility = 0.0;
        let oneOverShadowDepthTextureSize = 1.0 / f32(textureDimensions(dir_shadow_map_array).x);
        for (var y = -1; y <= 1; y++) {
            for (var x = -1; x <= 1; x++) {
                let offset = vec2<f32>(vec2(x, y)) * oneOverShadowDepthTextureSize;

                visibility += textureSampleCompare(
                    dir_shadow_map_array, shadow_sampler,
                    p.xy + offset, // coords
                    layer, // array layer
                    p.z - bias // shadowBias
                );
            }
        }
        visibility /= 9.0;

        return visibility;
    }

// main =====================================================================================
    @fragment 
    fn fs_main(
        in : VertexOutput,
        @builtin(front_facing) is_front: bool,
    ) -> @location(0) vec4f
    {
        let viewVector = u_frame.camera_pos - in.v_worldpos;
        let viewDir = normalize(viewVector);  // direction from camera to this frag

        var normal = perturbNormal(in.v_normal, viewVector, in.v_uv, u_normal_factor, is_front);


        // material color properties (ignore alpha channel for now)
        let diffuseTex = textureSample(u_diffuse_map, texture_sampler, in.v_uv);
        let specularTex = textureSample(u_specular_map, texture_sampler, in.v_uv);
        let aoTex = textureSample(u_ao_map, texture_sampler, in.v_uv);
        let emissiveTex = textureSample(u_emissive_map, texture_sampler, in.v_uv);
        // factor ao into diffuse
        var diffuse_color = u_diffuse_color.rgb * srgbToLinear(diffuseTex.rgb);
        diffuse_color = mix(diffuse_color, diffuse_color * aoTex.r, u_ao_factor);
        let specular_color : vec3f = (specularTex.rgb * u_specular_color);

        var lighting = vec3f(0.0); // accumulate lighting
        for (var i = 0; i < u_frame.num_lights; i++) {
            let light = u_lights[i];

            // these need to be computed based on light type
            var attenuation = 1.0;
            var lightdir = vec3(0.0);

            switch (light.light_type) {
                case 1: { // directional
                    lightdir = normalize(-light.direction);

                    if (bool(in.receives_shadow) && bool(light.generates_shadows)) { 
                        attenuation *= calculateDirShadow(in.v_worldpos, light.shadow_map_array_layer, light.proj_view, light.bias);
                    }
                } 
                case 2: { // point
                    let l = light.position - in.v_worldpos;
                    lightdir = normalize(l);

                    let dist2 = dot(l, l); 
                    let r2 = light.point_and_spot_radius * light.point_and_spot_radius;
                    attenuation = pow(
                        saturate(1.0 - dist2 / r2),
                        light.point_and_spot_falloff
                    );
                }
                case 3: { // spot
                    let l = light.position - in.v_worldpos;
                    let l_len = length(l);
                    lightdir =  l / l_len;

                    // distance falloff
                    let dist2 = dot(l, l); 
                    let r2 = light.point_and_spot_radius * light.point_and_spot_radius;
                    attenuation = pow(
                        saturate(1.0 - dist2 / r2),
                        light.point_and_spot_falloff
                    );

                    // angular falloff
                    let cos_theta = dot(-l, light.direction) / l_len;
                    let t = saturate(
                        (cos_theta - light.spot_cos_angle_min) / 
                        (light.spot_cos_angle_max - light.spot_cos_angle_min)
                    );
                    attenuation *= pow((1.0 - t), light.spot_angular_falloff);

                    if (bool(in.receives_shadow) && bool(light.generates_shadows)) { 
                        attenuation *= calculateSpotShadow(in.v_worldpos, light.shadow_map_array_layer, light.proj_view, light.bias);
                    }
                }
                default: {} // no light
            } // end switch light type
            let halfwaydir = normalize(lightdir + viewDir);
            let diffuse_factor = saturate(dot(normal, lightdir));
            let specular_factor = saturate(pow(saturate(dot(normal, halfwaydir)), u_shininess));
            lighting += (light.color * attenuation) * (diffuse_color * diffuse_factor + specular_color.rgb * specular_factor);
        }  // end light loop

        // ambient light
        lighting += u_frame.ambient_light * diffuse_color;

        // calculate envmap contribution
        if (u_envmap_blend != ENVMAP_BLEND_NONE) {
            let envMapContrib = envMapContribution(viewDir, normal);

            // blending
            if (u_envmap_blend == ENVMAP_BLEND_ADDITIVE) {
                lighting += (u_envmap_intensity * envMapContrib);
            }
            else if (u_envmap_blend == ENVMAP_BLEND_MULTIPLICATIVE) {
                lighting *= (u_envmap_intensity * envMapContrib);
            }
            else if (u_envmap_blend == ENVMAP_BLEND_MIX) {
                lighting = mix(lighting, envMapContrib, u_envmap_intensity);
            }
        }

        // emissive
        lighting += srgbToLinear(emissiveTex.rgb) * u_emission_color;

        // alpha test
        if (diffuseTex.a < .01) {
            discard;
        }

        return vec4f(
            lighting, 
            diffuseTex.a * u_diffuse_color.a
        );
    }
)glsl";

static const char* lines2d_shader_string  = R"glsl(

#include FRAME_UNIFORMS

// line material uniforms
@group(1) @binding(0) var<uniform> u_line_width: f32;
@group(1) @binding(1) var<uniform> u_color: vec3f;

#include DRAW_UNIFORMS

// stored as [x0, y0, x1, y1, ...]
@group(3) @binding(0) var<storage, read> positions : array<f32>; // vertex pulling group 
@group(3) @binding(1) var<storage, read> u_color_array: array<f32>; // per-vertex color rgb

struct VertexInput {
    @builtin(instance_index) instance : u32,
};

struct VertexOutput {
    @builtin(position) position : vec4f,
    @location(0) v_color : vec3f,
};

// for bevel joins
fn getPos(vertex_idx : u32) -> vec2f 
{
    let pos_idx = vertex_idx / 4u; // 4 vertices per line segment in bevel join
    return vec2f(
        positions[2u * pos_idx + 0u],  // x
        positions[2u * pos_idx + 1u]   // y
    );
}

fn calculate_line_pos(vertex_id : u32) -> vec2f
{
    let vertex_idx = vertex_id + 4u; // adding 4 to account for sentinel start point

    let this_pos = getPos(vertex_idx); // input segment pos
    let next_pos = getPos(vertex_idx + 4u);
    let prev_pos = getPos(vertex_idx - 4u);
    var pos = vec2f(0.0); // final extruded pos
    let bevel_idx = vertex_id % 4u;

    let prev_dir = normalize(this_pos - prev_pos);
    let next_dir = normalize(next_pos - this_pos);
    let prev_dir_perp = vec2f(-prev_dir.y, prev_dir.x);
    let next_dir_perp = vec2f(-next_dir.y, next_dir.x);

    // determine orientation from whether vectors are cw or ccw
    var orientation = 1.0;
    var ccw : bool = dot(-prev_dir_perp, next_dir) > 0.0; // next dir is ccw to prev dir
    let cw : bool = !ccw;
    if (cw) {
        orientation = -1.0;
    }

    // every 4 vertices 
    /*
    if ccw, we go bevel --> miter --> bevel --> miter
    if cw, we go miter --> bevel --> miter --> bevel
    */

    if (
        (ccw && (bevel_idx == 1u || bevel_idx == 3u))
        ||
        (cw && (bevel_idx == 0u || bevel_idx == 2u))
    ) {
        let miter_dir = 1.0 * normalize(prev_dir_perp + next_dir_perp);
        let max_miter_len = min(length(next_pos - this_pos), length(prev_pos - this_pos));
        let miter_length = clamp((u_line_width * 0.5) / dot(miter_dir, prev_dir_perp), 0.0, max_miter_len);
        
        pos = this_pos - orientation * miter_length * miter_dir;
    }
    else if (
        (ccw && bevel_idx == 0u)
        ||
        (cw && bevel_idx == 1u)
    ) {
        pos = this_pos + orientation * (u_line_width * 0.5) * prev_dir_perp;
    }
    else if (
        (ccw && bevel_idx == 2u)
        || 
        (cw && bevel_idx == 3u)
    ) {
        pos = this_pos + orientation * (u_line_width * 0.5) * next_dir_perp;
    }

    return pos;
}

@vertex 
fn vs_main(
    in : VertexInput,
    @builtin(vertex_index) vertex_id : u32
) -> VertexOutput
{
    var out : VertexOutput;
    var u_Draw : DrawUniforms = u_draw_instances[in.instance];

    let worldpos = u_Draw.model * vec4f(calculate_line_pos(vertex_id), 0.0, 1.0);
    out.position = (u_frame.projection * u_frame.view) * worldpos;

    // color
    out.v_color = vec3f(1.0);
    var pos_idx = max(i32(vertex_id / 4u), 0);
    let num_colors = i32(arrayLength(&u_color_array));
    if (num_colors > 0) {
        out.v_color = vec3f(
            u_color_array[(3 * pos_idx + 0) % num_colors],
            u_color_array[(3 * pos_idx + 1) % num_colors],
            u_color_array[(3 * pos_idx + 2) % num_colors]
        );
    }

    return out;
}

@fragment 
fn fs_main(in : VertexOutput) -> @location(0) vec4f
{
    return vec4f(u_color * in.v_color, 1.0);
}
)glsl";

static const char* points_shader_string  = R"glsl(

#include FRAME_UNIFORMS

// line material uniforms
@group(1) @binding(0) var<uniform> u_point_global_color : vec4f;
@group(1) @binding(1) var<uniform> u_point_global_size : f32;
@group(1) @binding(2) var u_point_sampler : sampler;
@group(1) @binding(3) var u_point_texture : texture_2d<f32>;
@group(1) @binding(4) var<uniform> u_billboard : i32;

#include DRAW_UNIFORMS

// every 5 f32s is a vertex:  (x, y, z, uv.x, uv,y)
@group(3) @binding(0) var<storage, read> u_point_vertices: array<f32>; // vertex attributes (currently just a plane)
// (r,g,b)
@group(3) @binding(1) var<storage, read> u_point_colors: array<f32>; // per-point color (for now rgb)
@group(3) @binding(2) var<storage, read> u_point_sizes: array<f32>; // per-point size (single f32)
@group(3) @binding(3) var<storage, read> u_point_positions: array<f32>; // per-point positions (x, y, z)

struct VertexOutput {
    @builtin(position) position : vec4f,
    @location(0) v_color : vec3f,
    @location(1) v_uv : vec2f,
};

@vertex 
fn vs_main(
    @builtin(instance_index) instance_id : u32,
    @builtin(vertex_index) vertex_id : u32
) -> VertexOutput
{
    var out : VertexOutput;
    var u_Draw : DrawUniforms = u_draw_instances[instance_id];

    let point_idx = i32(vertex_id / 6u); // 6 vertices per point plane
    let vertex_idx = i32(vertex_id % 6u); // 6 vertices per point plane

    let point_uv = vec2f(
        u_point_vertices[5 * vertex_idx + 3],
        u_point_vertices[5 * vertex_idx + 4]
    );

    let num_colors = i32(arrayLength(&u_point_colors));
    var point_color = u_point_global_color.rgb;
    if (num_colors > 0) {
        point_color *= vec3f(
            u_point_colors[(3 * point_idx + 0) % num_colors],
            u_point_colors[(3 * point_idx + 1) % num_colors],
            u_point_colors[(3 * point_idx + 2) % num_colors]
        );
    }

    var point_size = u_point_global_size;
    let num_sizes = i32(arrayLength(&u_point_sizes));
    if (num_sizes > 0) {
        point_size *= u_point_sizes[point_idx % num_sizes];
    }

    let center_point_pos = vec3f(
        u_point_positions[3 * point_idx + 0],
        u_point_positions[3 * point_idx + 1],
        u_point_positions[3 * point_idx + 2]
    );

    let vertex_pos = point_size * vec3f(
        u_point_vertices[5 * vertex_idx + 0],
        u_point_vertices[5 * vertex_idx + 1],
        u_point_vertices[5 * vertex_idx + 2],
    );

    if (bool(u_billboard)) {
        // in billboard mode the point plane always faces the camera
        // so add the vertex position *after* computing the point position in view space
        var point_pos_view_space = u_frame.view * u_Draw.model * vec4f(center_point_pos, 1.0);
        point_pos_view_space += vec4f(vertex_pos, 0.0);
        out.position = u_frame.projection * point_pos_view_space;
    } else {
        // transform normally
        out.position = (u_frame.projection * u_frame.view * u_Draw.model) * vec4f(center_point_pos + vertex_pos, 1.0);
    }

    out.v_color = point_color;
    out.v_uv = point_uv;

    return out;
}

@fragment 
fn fs_main(in : VertexOutput) -> @location(0) vec4f
{
    let tex = textureSample(u_point_texture, u_point_sampler, in.v_uv);

    // alpha test
    if (tex.a < .01) {
        discard;
    }

    return vec4f(in.v_color * tex.rgb, tex.a);
}

)glsl";


// ----------------------------------------------------------------------------

static const char* pbr_shader_string = R"glsl(
    // includes
    #include FRAME_UNIFORMS
    #include DRAW_UNIFORMS
    #include LIGHTING_UNIFORMS
    #include STANDARD_VERTEX_INPUT
    #include STANDARD_VERTEX_OUTPUT
    #include STANDARD_VERTEX_SHADER

    // textures
    @group(1) @binding(0) var texture_sampler: sampler;
    @group(1) @binding(1) var albedoMap: texture_2d<f32>;
    @group(1) @binding(2) var u_normal_map: texture_2d<f32>;
    @group(1) @binding(3) var aoMap: texture_2d<f32>;
    @group(1) @binding(4) var mrMap: texture_2d<f32>;
    @group(1) @binding(5) var emissiveMap: texture_2d<f32>;

    // uniforms
    @group(1) @binding(6) var<uniform> u_baseColor: vec4f;
    @group(1) @binding(7) var<uniform> u_emissiveFactor: vec3f;
    @group(1) @binding(8) var<uniform> u_metallic: f32;
    @group(1) @binding(9) var<uniform> u_roughness: f32;
    @group(1) @binding(10) var<uniform> u_normal_factor: f32;
    @group(1) @binding(11) var<uniform> u_aoFactor: f32;

    fn srgbToLinear(srgb_in : vec3f) -> vec3f {
        return pow(srgb_in.rgb,vec3f(2.2));
    }

    #include NORMAL_MAPPING_FUNCTIONS

    const PI = 3.1415926535897932384626433832795;
    const reflectivity = 0.04;  // heuristic, assume F0 of 0.04 for all dielectrics

    // Normal Distribution function ----------------------------------------------
    fn D_GGX(dotNH : f32, roughness : f32) -> f32 {
        let alpha : f32 = roughness * roughness;
        let alpha2 : f32 = alpha * alpha;
        let denom : f32 = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
        return (alpha2)/(PI * denom*denom);
    }

    // Geometric Shadowing function ----------------------------------------------
    fn G_SchlickSmithGGX(dotNL : f32, dotNV : f32, roughness : f32) -> f32 {
        let r : f32 = (roughness + 1.0);
        let k : f32 = (r*r) / 8.0;

        let GL : f32 = dotNL / (dotNL * (1.0 - k) + k);
        let GV : f32 = dotNV / (dotNV * (1.0 - k) + k);
        return GL * GV;

    }

    // Fresnel function ----------------------------------------------------------
    // cosTheta assumed to be in range [0, 1]
    fn F_Schlick(cosTheta : f32, F0 : vec3<f32>) -> vec3f {
        return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
    }

    @fragment 
    fn fs_main(
        in : VertexOutput,
        @builtin(front_facing) is_front: bool
    ) -> @location(0) vec4f
    {   
        let viewVector = u_frame.camera_pos - in.v_worldpos;
        let V : vec3f = normalize(viewVector);  // direction from camera to this frag

        let N = perturbNormal(in.v_normal, viewVector, in.v_uv, u_normal_factor, is_front);

        // linear-space albedo (normally authored in sRGB space so we have to convert to linear space)
        // transparency not supported
        let albedo: vec3f = u_baseColor.rgb * srgbToLinear(textureSample(albedoMap, texture_sampler, in.v_uv).rgb);
        
        // The metallicRoughnessTexture contains the metalness value in the "blue" color channel, 
        // and the roughness value in the "green" color channel.
        let metallic_roughness = textureSample(mrMap, texture_sampler, in.v_uv);
        let metallic : f32 = metallic_roughness.b * u_metallic;
        let roughness : f32 = metallic_roughness.g * u_roughness;

        var F0 : vec3f = vec3f(reflectivity);
        F0 = mix(F0, albedo.rgb, metallic); // reflectivity for metals

        var Lo : vec3f = vec3(0.0);
        // loop over all lights
        for (var i = 0; i < u_frame.num_lights; i++) {
            let light = u_lights[i];
            var L : vec3f = vec3(0.0);
            var radiance : vec3f = vec3(0.0);
            switch (light.light_type) {
                case 1: { // directional
                    L = normalize(-light.direction);
                    radiance = light.color;
                }
                case 2: { // point
                    L = normalize(light.position - in.v_worldpos);
                    let dist = distance(in.v_worldpos, light.position);
                    let attenuation = pow(
                        clamp(1.0 - dist / light.point_and_spot_radius, 0.0, 1.0), 
                        light.point_and_spot_falloff
                    );
                    radiance = light.color * attenuation;
                }
                default: { } // no light
            } // end switch light type

            // half vector
            let H : vec3f = normalize(V + L);
            let dotNH : f32 = clamp(dot(N, H), 0.0, 1.0);
            let dotNV : f32 = clamp(dot(N, V), 0.0, 1.0);
            let dotNL : f32 = clamp(dot(N, L), 0.0, 1.0);

            if (dotNL > 0.0) {
                // D = Normal distribution (Distribution of the microfacets)
                let D : f32 = D_GGX(dotNH, roughness);
                // G = Geometric shadowing term (Microfacets shadowing)
                let G : f32 = G_SchlickSmithGGX(dotNL, dotNV, roughness);
                // F = Fresnel factor (Reflectance depending on angle of incidence)
                let F : vec3<f32> = F_Schlick(max(dot(H, V), 0.0), F0);

                // specular contribution
                let spec : vec3f = D * F * G / (4.0 * dotNL * dotNV + 0.0001);
                // diffuse contribution
                let kD : vec3f = (vec3f(1.0) - F) * (1.0 - metallic);
                // final color contribution
                Lo += (kD * albedo / PI + spec) * dotNL * radiance;
            }
        }  // end light loop

        // // ambient occlusion (hardcoded for now) (ambient should only be applied to direct lighting, not indirect lighting)
        let ambient : vec3f = u_frame.ambient_light * albedo * textureSample(aoMap, texture_sampler, in.v_uv).r * u_aoFactor;
        var finalColor : vec3f = Lo + ambient;  // TODO: update ao calculation after adding IBL

        // add emission
        let emissiveColor : vec3f = srgbToLinear(textureSample(emissiveMap, texture_sampler, in.v_uv).rgb);
        finalColor += emissiveColor * u_emissiveFactor;

        return vec4f(finalColor, u_baseColor.a);
    }
)glsl";

static const char* mipMapShader = CODE(
    var<private> pos : array<vec2<f32>, 3> = array<vec2<f32>, 3>(
        vec2<f32>(-1.0, -1.0), 
        vec2<f32>(-1.0, 3.0), 
        vec2<f32>(3.0, -1.0)
    );

    struct VertexOutput {
        @builtin(position) position : vec4<f32>,
        @location(0) texCoord : vec2<f32>,
    }

    @vertex
    fn vs_main(@builtin(vertex_index) vertexIndex : u32) -> VertexOutput {
        var output : VertexOutput;
        // remap uvs to [0, 2] and flip y
        // rasterizer will clip uvs to [0, 1]
        output.texCoord = pos[vertexIndex] * vec2<f32>(0.5, -0.5) + vec2<f32>(0.5);
        // positions in ndc space
        output.position = vec4<f32>(pos[vertexIndex], 0.0, 1.0);
        return output;
    }

    @group(0) @binding(0) var imgSampler : sampler;
    @group(0) @binding(1) var img : texture_2d<f32>;  // image to mip

    @fragment
    fn fs_main(@location(0) texCoord : vec2<f32>) -> @location(0) vec4<f32> {
        return textureSample(img, imgSampler, texCoord);
    }
);

const char* gtext_shader_string = R"glsl(

    // Based on: http://wdobbie.com/post/gpu-text-rendering-with-vector-textures/

    #include FRAME_UNIFORMS
    #include DRAW_UNIFORMS

    // custom material uniforms
    @group(1) @binding(0) var<storage, read> u_Glyphs: array<i32>;
    @group(1) @binding(1) var<storage, read> u_Curves: array<f32>;
    @group(1) @binding(2) var<uniform> u_Color: vec4f;

    // Controls for debugging and exploring:

    // Size of the window (in pixels) used for 1-dimensional anti-aliasing along each rays.
    //   0 - no anti-aliasing
    //   1 - normal anti-aliasing
    // >=2 - exaggerated effect 
    @group(1) @binding(3) var<uniform> antiAliasingWindowSize: f32;

    // Enable a second ray along the y-axis to achieve 2-dimensional anti-aliasing.
    // set to 1 to enable, 0 to disable
    @group(1) @binding(4) var<uniform> enableSuperSamplingAntiAliasing: i32;

    @group(1) @binding(5) var<uniform> bb : vec4f; // x = minx, y = miny, z = maxx, w = maxy
    @group(1) @binding(6) var texture_map: texture_2d<f32>;
    @group(1) @binding(7) var texture_sampler: sampler;


    struct VertexInput {
        @location(0) position : vec2f,
        @location(1) uv : vec2f,
        @location(2) glyph_index : i32, // index into glyphs array (which itself is slice into curves array)
        @builtin(instance_index) instance : u32,
    };

    struct VertexOutput {
        @builtin(position) position : vec4f,
        @location(0) v_uv : vec2f, // per-glyph uv
        @location(1) @interpolate(flat) v_buffer_index: i32,
        @location(2) v_uv_textbox : vec2f, // entire GText uv (e.g. if you want to texture your text)
    };

    @vertex 
    fn vs_main(in : VertexInput) -> VertexOutput
    {
        var out : VertexOutput;
        var u_Draw : DrawUniforms = u_draw_instances[in.instance];
        out.position = (u_frame.projection * u_frame.view) * u_Draw.model * vec4f(in.position, 0.0f, 1.0f);
        out.v_uv     = in.uv;
        out.v_buffer_index = in.glyph_index;

        let bb_w = bb.z - bb.x;
        let bb_h = bb.w - bb.y;
        out.v_uv_textbox = (in.position - vec2f(bb.x, bb.y)) / vec2f(bb_w, bb_h);

        return out;
    }


    struct Glyph {
        start : i32,
        count : i32,
    };

    struct Curve {
        p0 : vec2f,
        p1 : vec2f,
        p2 : vec2f,
    };

    fn loadGlyph(index : i32) -> Glyph {
        var result : Glyph;
        // let data = u_Glyphs[index].xy;
        // result.start = u32(data.x);
        // result.count = u32(data.y);
        result.start = u_Glyphs[2 * index + 0];
        result.count = u_Glyphs[2 * index + 1];
        return result;
    }

    fn loadCurve(index : i32) -> Curve {
        var result : Curve;
        // result.p0 = u_Curves[3u * index + 0u].xy;
        // result.p1 = u_Curves[3u * index + 1u].xy;
        // result.p2 = u_Curves[3u * index + 2u].xy;
        result.p0 = vec2f(u_Curves[6 * index + 0], u_Curves[6 * index + 1]);
        result.p1 = vec2f(u_Curves[6 * index + 2], u_Curves[6 * index + 3]);
        result.p2 = vec2f(u_Curves[6 * index + 4], u_Curves[6 * index + 5]);
        return result;
    }

    fn computeCoverage(inverseDiameter : f32, p0 : vec2f, p1 : vec2f, p2 : vec2f) -> f32 {
        if (p0.y > 0.0 && p1.y > 0.0 && p2.y > 0.0) { return 0.0; }
        if (p0.y < 0.0 && p1.y < 0.0 && p2.y < 0.0) { return 0.0; }

        // Note: Simplified from abc formula by extracting a factor of (-2) from b.
        let a = p0 - 2.0*p1 + p2;
        let b = p0 - p1;
        let c = p0;

        var t0 : f32;
        var t1 : f32;
        if (abs(a.y) >= 1e-5) {
            // Quadratic segment, solve abc formula to find roots.
            let radicand : f32 = b.y*b.y - a.y*c.y;
            if (radicand <= 0.0) { return 0.0; }
        
            let s : f32 = sqrt(radicand);
            t0 = (b.y - s) / a.y;
            t1 = (b.y + s) / a.y;
        } else {
            // Linear segment, avoid division by a.y, which is near zero.
            // There is only one root, so we have to decide which variable to
            // assign it to based on the direction of the segment, to ensure that
            // the ray always exits the shape at t0 and enters at t1. For a
            // quadratic segment this works 'automatically', see readme.
            let t : f32 = p0.y / (p0.y - p2.y);
            if (p0.y < p2.y) {
                t0 = -1.0;
                t1 = t;
            } else {
                t0 = t;
                t1 = -1.0;
            }
        }

        var alpha : f32 = 0.0;
        
        if (t0 >= 0.0 && t0 < 1.0) {
            let x : f32 = (a.x*t0 - 2.0*b.x)*t0 + c.x;
            alpha += clamp(x * inverseDiameter + 0.5, 0.0, 1.0);
        }

        if (t1 >= 0.0 && t1 < 1.0) {
            let x = (a.x*t1 - 2.0*b.x)*t1 + c.x;
            alpha -= clamp(x * inverseDiameter + 0.5, 0.0, 1.0);
        }

        return alpha;
    }

    fn rotate(v : vec2f) -> vec2f {
        return vec2f(v.y, -v.x);
    }

    @fragment
    fn fs_main(in : VertexOutput) -> @location(0) vec4f {
        var alpha : f32 = 0.0;

        // Inverse of the diameter of a pixel in uv units for anti-aliasing.
        let inverseDiameter = 1.0 / (antiAliasingWindowSize * fwidth(in.v_uv));

        let glyph = loadGlyph(in.v_buffer_index);
        for (var i : i32 = 0; i < glyph.count; i++) {
            let curve = loadCurve(glyph.start + i);

            let p0 = curve.p0 - in.v_uv;
            let p1 = curve.p1 - in.v_uv;
            let p2 = curve.p2 - in.v_uv;

            alpha += computeCoverage(inverseDiameter.x, p0, p1, p2);
            if (bool(enableSuperSamplingAntiAliasing)) {
                alpha += computeCoverage(inverseDiameter.y, rotate(p0), rotate(p1), rotate(p2));
            }
        }

        if (bool(enableSuperSamplingAntiAliasing)) {
            alpha *= 0.5;
        }

        alpha = clamp(alpha, 0.0, 1.0);
        let result = u_Color * alpha;
        let sample = textureSample(texture_map, texture_sampler, in.v_uv_textbox);

        // alpha test
        if (result.a < 0.001) {
            discard;
        }

        return result * sample;
        // return vec4f(in.v_uv_textbox, 0.0, 1.0);
    }
)glsl";


const char* default_postprocess_shader_string = R"glsl(
    #include SCREEN_PASS_VERTEX_SHADER

    @fragment 
    fn fs_main(in : VertexOutput) -> @location(0) vec4f {
        return vec4f(in.v_uv, 0.0, 1.0);
    }
)glsl";


const char* output_pass_shader_string = R"glsl(
    #include FRAME_UNIFORMS
    #include SCREEN_PASS_VERTEX_SHADER

    const TONEMAP_NONE = 0;
    const TONEMAP_LINEAR = 1;
    const TONEMAP_REINHARD = 2;
    const TONEMAP_CINEON = 3;
    const TONEMAP_ACES = 4;
    const TONEMAP_UNCHARTED = 5;

    @group(1) @binding(0) var texture: texture_2d<f32>;
    @group(1) @binding(1) var texture_sampler: sampler;
    @group(1) @binding(2) var<uniform> u_Gamma: i32;
    @group(1) @binding(3) var<uniform> u_Exposure: f32;
    @group(1) @binding(4) var<uniform> u_Tonemap: i32;

    // Helpers ==================================================================
    fn Uncharted2Tonemap(x: vec3<f32>) -> vec3<f32> {
        let A: f32 = 0.15;
        let B: f32 = 0.5;
        let C: f32 = 0.1;
        let D: f32 = 0.2;
        let E: f32 = 0.02;
        let F: f32 = 0.3;
        return (x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F) - E / F;
    } 

    // source: https://github.com/selfshadow/ltc_code/blob/master/webgl/shaders/ltc/ltc_blit.fs
    fn rrt_odt_fit(v: vec3<f32>) -> vec3<f32> {
        let a: vec3<f32> = v * (v + 0.0245786) - 0.000090537;
        let b: vec3<f32> = v * (0.983729 * v + 0.432951) + 0.238081;
        return a / b;
    } 

    fn mat3_from_rows(c0: vec3<f32>, c1: vec3<f32>, c2: vec3<f32>) -> mat3x3<f32> {
        var m: mat3x3<f32> = mat3x3<f32>(c0, c1, c2);
        m = transpose(m);
        return m;
    } 

    // from https://medium.com/@tomforsyth/the-srgb-learning-curve-773b7f68cf7a
    fn D3DX_FLOAT_to_SRGB(val: f32) -> f32
    { 
        var v = val;
        if(v < 0.0031308) {
            v *= 12.92;
        } else {
            v = 1.055 * pow(val, 1.0/2.4) - 0.055;
        }
        return v;
    }

    // main =====================================================================
    @fragment 
    fn fs_main(in : VertexOutput) -> @location(0) vec4f {
        let UNUSED = u_frame;
        let hdrColor: vec4<f32> = textureSample(texture, texture_sampler, in.v_uv);
        var color: vec3<f32> = hdrColor.rgb;
        if (u_Tonemap != TONEMAP_NONE) {
            color = color * (u_Exposure);
        }
        switch (u_Tonemap) {
        case 1: { // linear
            color = clamp(color, vec3f(0.), vec3f(1.));
        }
        case 2: { // reinhard
            color = hdrColor.rgb / (hdrColor.rgb + vec3<f32>(1.));
        }
        case 3: { // cineon
            let x: vec3<f32> = max(vec3<f32>(0.), color - 0.004);
            color = x * (6.2 * x + 0.5) / (x * (6.2 * x + 1.7) + 0.06);
            color = pow(color, vec3<f32>(2.2)); // invert gamma correction (assumes final output to srgb texture)
        } 
        case 4: { // aces
            var ACES_INPUT_MAT: mat3x3<f32> = mat3_from_rows(vec3<f32>(0.59719, 0.35458, 0.04823), vec3<f32>(0.076, 0.90834, 0.01566), vec3<f32>(0.0284, 0.13383, 0.83777));
            var ACES_OUTPUT_MAT: mat3x3<f32> = mat3_from_rows(vec3<f32>(1.60475, -0.53108, -0.07367), vec3<f32>(-0.10208, 1.10813, -0.00605), vec3<f32>(-0.00327, -0.07276, 1.07602));
            color = color / 0.6;
            color = ACES_INPUT_MAT * color;
            color = rrt_odt_fit(color);
            color = ACES_OUTPUT_MAT * color;
            color = clamp(color, vec3f(0.), vec3f(1.));
        }
        case 5: { // uncharted
            let ExposureBias: f32 = 2.;
            let curr: vec3<f32> = Uncharted2Tonemap(ExposureBias * color);
            let W: f32 = 11.2;
            let whiteScale: vec3<f32> = vec3<f32>(1. / Uncharted2Tonemap(vec3<f32>(W)));
            color = curr * whiteScale;
        }
        default: {}
        }

        // gamma correction
        // color = pow(color, vec3<f32>(1. / u_Gamma));
        if (bool(u_Gamma)) {
            color.r = D3DX_FLOAT_to_SRGB(color.r);
            color.g = D3DX_FLOAT_to_SRGB(color.g);
            color.b = D3DX_FLOAT_to_SRGB(color.b);
        }

        return vec4<f32>(color, 1.0); // how does alpha work?
        // return vec4<f32>(color, clamp(hdrColor.a, 0.0, 1.0));
    } 



)glsl";


const char* bloom_downsample_screen_shader = R"glsl(

#include SCREEN_PASS_VERTEX_SHADER
@group(0) @binding(0) var u_texture: texture_2d<f32>; // texture at previous mip level
@group(0) @binding(1) var u_sampler: sampler;
@group(0) @binding(2) var<uniform> u_threshold: f32;
@group(0) @binding(3) var<uniform> u_full_res: vec2u; // full resolution of input texture

@fragment 
fn fs_main(in : VertexOutput) -> @location(0) vec4f
{
    let input_dim = textureDimensions(u_texture).xy;
    let dx = 1.0 / f32(input_dim.x); // change in uv.x that corresponds to 1 pixel in input texture x direction
    let dy = 1.0 / f32(input_dim.y); // change in uv.y that corresponds to 1 pixel in input texture y direction
    let uv = in.v_uv;

    // Take 13 samples around current texel:
    // a - b - c
    // - j - k -
    // d - e - f
    // - l - m -
    // g - h - i
    // === ('e' is the current texel) ===
    let a = textureSample(u_texture, u_sampler, vec2f(uv.x - 2.0 * dx, uv.y + 2.0 * dy)).rgb;
    let b = textureSample(u_texture, u_sampler, vec2f(uv.x, uv.y + 2.0 * dy)).rgb;
    let c = textureSample(u_texture, u_sampler, vec2f(uv.x + 2.0 * dx, uv.y + 2.0 * dy)).rgb;

    let d = textureSample(u_texture, u_sampler, vec2f(uv.x - 2.0 * dx, uv.y)).rgb;
    let e = textureSample(u_texture, u_sampler, vec2f(uv.x, uv.y)).rgb;
    let f = textureSample(u_texture, u_sampler, vec2f(uv.x + 2.0 * dx, uv.y)).rgb;

    let g = textureSample(u_texture, u_sampler, vec2f(uv.x - 2.0 * dx, uv.y - 2.0 * dy)).rgb;
    let h = textureSample(u_texture, u_sampler, vec2f(uv.x, uv.y - 2.0 * dy)).rgb;
    let i = textureSample(u_texture, u_sampler, vec2f(uv.x + 2.0 * dx, uv.y - 2.0 * dy)).rgb;

    let j = textureSample(u_texture, u_sampler, vec2f(uv.x - dx, uv.y + dy)).rgb;
    let k = textureSample(u_texture, u_sampler, vec2f(uv.x + dx, uv.y + dy)).rgb;
    let l = textureSample(u_texture, u_sampler, vec2f(uv.x - dx, uv.y - dy)).rgb;
    let m = textureSample(u_texture, u_sampler, vec2f(uv.x + dx, uv.y - dy)).rgb;

    // Apply weighted distribution:
    // 0.5 + 0.125 + 0.125 + 0.125 + 0.125 = 1
    // a,b,d,e * 0.125
    // b,c,e,f * 0.125
    // d,e,g,h * 0.125
    // e,f,h,i * 0.125
    // j,k,l,m * 0.5
    // This shows 5 square areas that are being sampled. But some of them overlap,
    // so to have an energy preserving downsample we need to make some adjustments.
    // The weights are the distributed, so that the sum of j,k,l,m (e.g.)
    // contribute 0.5 to the final color output. The code below is written
    // to effectively yield this sum. We get:
    // 0.125*5 + 0.03125*4 + 0.0625*4 = 1
    var downsample = vec3f(0.0);
    downsample = e*0.125;
    downsample += (a+c+g+i)*0.03125;
    downsample += (b+d+f+h)*0.0625;
    downsample += (j+k+l+m)*0.125;

    if (all(input_dim == u_full_res)) {
        let brightness = max(max(downsample.r, downsample.g), downsample.b);
        let contribution = max(0.0, brightness - u_threshold) / max(brightness, 0.00001);
        downsample *= contribution;
    }

    return vec4f(downsample, 1.0);
}
)glsl";


const char* bloom_downsample_shader_string = R"glsl(

@group(0) @binding(0) var u_input_texture: texture_2d<f32>;
@group(0) @binding(1) var u_input_tex_sampler: sampler;
@group(0) @binding(2) var u_out_texture: texture_storage_2d<rgba16float, write>; // hdr
@group(0) @binding(3) var<uniform> u_mip_level: i32; 

@compute @workgroup_size(8, 8, 1)
fn main(
    @builtin(global_invocation_id) GlobalInvocationID : vec3<u32>,
    @builtin(workgroup_id) WorkGroupID : vec3<u32>,
    @builtin(local_invocation_index) LocalInvocationIndex : u32, // ranges from 0 - 63
    @builtin(local_invocation_id) LocalInvocationID : vec3<u32>
)
{
    // TODO use same texel size logic in upsample shader
    let output_size = textureDimensions(u_out_texture);
    let input_size = textureDimensions(u_input_texture);
    let pixel_coords = vec2i(GlobalInvocationID.xy);
    let x = 1.0 / f32(input_size.x);
    let y = 1.0 / f32(input_size.y);

    let uv        = (vec2f(GlobalInvocationID.xy) + vec2f(0.5)) / vec2f(output_size.xy);

    // Take 13 samples around current texel:
    // a - b - c
    // - j - k -
    // d - e - f
    // - l - m -
    // g - h - i
    // === ('e' is the current texel) ===
    let a = textureSampleLevel(u_input_texture, u_input_tex_sampler, vec2f(uv.x - 2.0*x, uv.y + 2.0*y), 0.0).rgb;
    let b = textureSampleLevel(u_input_texture, u_input_tex_sampler, vec2f(uv.x,       uv.y + 2.0*y), 0.0).rgb;
    let c = textureSampleLevel(u_input_texture, u_input_tex_sampler, vec2f(uv.x + 2.0*x, uv.y + 2.0*y), 0.0).rgb;
    let d = textureSampleLevel(u_input_texture, u_input_tex_sampler, vec2f(uv.x - 2.0*x, uv.y), 0.0).rgb;
    let e = textureSampleLevel(u_input_texture, u_input_tex_sampler, vec2f(uv.x,       uv.y), 0.0).rgb;
    let f = textureSampleLevel(u_input_texture, u_input_tex_sampler, vec2f(uv.x + 2.0*x, uv.y), 0.0).rgb;
    let g = textureSampleLevel(u_input_texture, u_input_tex_sampler, vec2f(uv.x - 2.0*x, uv.y - 2.0*y), 0.0).rgb;
    let h = textureSampleLevel(u_input_texture, u_input_tex_sampler, vec2f(uv.x,       uv.y - 2.0*y), 0.0).rgb;
    let i = textureSampleLevel(u_input_texture, u_input_tex_sampler, vec2f(uv.x + 2.0*x, uv.y - 2.0*y), 0.0).rgb;
    let j = textureSampleLevel(u_input_texture, u_input_tex_sampler, vec2f(uv.x - x,   uv.y + y), 0.0).rgb;
    let k = textureSampleLevel(u_input_texture, u_input_tex_sampler, vec2f(uv.x + x,   uv.y + y), 0.0).rgb;
    let l = textureSampleLevel(u_input_texture, u_input_tex_sampler, vec2f(uv.x - x,   uv.y - y), 0.0).rgb;
    let m = textureSampleLevel(u_input_texture, u_input_tex_sampler, vec2f(uv.x + x,   uv.y - y), 0.0).rgb;

    // Apply weighted distribution:
    // 0.5 + 0.125 + 0.125 + 0.125 + 0.125 = 1
    // a,b,d,e * 0.125
    // b,c,e,f * 0.125
    // d,e,g,h * 0.125
    // e,f,h,i * 0.125
    // j,k,l,m * 0.5
    // This shows 5 square areas that are being sampled. But some of them overlap,
    // so to have an energy preserving downsample we need to make some adjustments.
    // The weights are the distributed, so that the sum of j,k,l,m (e.g.)
    // contribute 0.5 to the final color output. The code below is written
    // to effectively yield this sum. We get:
    // 0.125*5 + 0.03125*4 + 0.0625*4 = 1
    var downsample = vec3f(0.0);
    downsample = e*0.125;
    downsample += (a+c+g+i)*0.03125;
    downsample += (b+d+f+h)*0.0625;
    downsample += (j+k+l+m)*0.125;

    // apply thresholding
    if (u_mip_level == 0) {
    //     // thresholding
    //     float brightness = max3(e);
    //     float contribution = max(0, brightness - u_Threshold) / max (brightness, 0.00001);
    //     downsample *= contribution;
    //     break;
    }

    let alpha = textureSampleLevel(u_input_texture, u_input_tex_sampler, uv, 0.0).a;
	textureStore(u_out_texture, pixel_coords, vec4f(downsample, alpha));
}
)glsl";

const char* bloom_upsample_screen_shader = R"glsl(

#include SCREEN_PASS_VERTEX_SHADER

@group(0) @binding(0) var u_prev_upsample_texture: texture_2d<f32>; // upsample texture at mip i+1 (half-resolution of target)
@group(0) @binding(1) var u_sampler: sampler;
@group(0) @binding(2) var u_curr_downsample_texture: texture_2d<f32>; // downsample texture at same mip/resolution
@group(0) @binding(3) var<uniform> u_full_resolution_size: vec2<u32>;  // used to check if this is last mip level
@group(0) @binding(4) var<uniform> u_internal_blend: f32; // linear blend between mip levels
@group(0) @binding(5) var<uniform> u_final_blend: f32; // linear blend with original image

@fragment
fn fs_main(in : VertexOutput) -> @location(0) vec4f
{
    let input_dim = textureDimensions(u_curr_downsample_texture);
    let dx = 1.0 / f32(input_dim.x); // change in uv.x that corresponds to 1 pixel in input texture x direction
    let dy = 1.0 / f32(input_dim.y); // change in uv.y that corresponds to 1 pixel in input texture y direction
    let uv = in.v_uv;

    let weights = array(
        0.0625, 0.125, 0.0625,
        0.125,  0.25,  0.125,
        0.0625, 0.125, 0.0625
    );

    var upsampled_color = vec3f(0.0);
    upsampled_color += weights[0] * textureSample(u_prev_upsample_texture, u_sampler, uv + vec2f(-dx, dy)).rgb;
    upsampled_color += weights[1] * textureSample(u_prev_upsample_texture, u_sampler, uv + vec2f(0.0, dy)).rgb;
    upsampled_color += weights[2] * textureSample(u_prev_upsample_texture, u_sampler, uv + vec2f(dx, dy)).rgb;
    upsampled_color += weights[3] * textureSample(u_prev_upsample_texture, u_sampler, uv + vec2f(-dx, 0.0)).rgb;
    upsampled_color += weights[4] * textureSample(u_prev_upsample_texture, u_sampler, uv).rgb;
    upsampled_color += weights[5] * textureSample(u_prev_upsample_texture, u_sampler, uv + vec2f(dx, 0.0)).rgb;
    upsampled_color += weights[6] * textureSample(u_prev_upsample_texture, u_sampler, uv + vec2f(-dx, -dy)).rgb;
    upsampled_color += weights[7] * textureSample(u_prev_upsample_texture, u_sampler, uv + vec2f(0.0, -dy)).rgb;
    upsampled_color += weights[8] * textureSample(u_prev_upsample_texture, u_sampler, uv + vec2f(dx, -dy)).rgb;

    let curr_color = textureSample(u_curr_downsample_texture, u_sampler, uv).rgb;

    if (all(input_dim == u_full_resolution_size)) {
        // upsampled_color = mix(curr_color, upsampled_color, u_final_blend);
        // additive blend on final stage
        upsampled_color = curr_color + upsampled_color * u_final_blend;
    } else {
        upsampled_color = mix(curr_color, upsampled_color, u_internal_blend);
    }

    return vec4f(upsampled_color, 1.0);
}   
)glsl";


const char* bloom_upsample_shader_string = R"glsl(

@group(0) @binding(0) var u_input_texture: texture_2d<f32>; // output_render_texture at mip i
@group(0) @binding(1) var u_sampler: sampler;
@group(0) @binding(2) var u_output_texture: texture_storage_2d<rgba16float, write>; // output_render_texture at mip i - 1
// @group(0) @binding(3) var<uniform> u_mip_level: i32; // doesn't seem mip level affects 1-mip views
@group(0) @binding(3) var<uniform> u_full_resolution_size: vec2<u32>;  // same across all calls
@group(0) @binding(4) var<uniform> u_internal_blend: f32; // linear blend between mip levels
@group(0) @binding(5) var<uniform> u_final_blend: f32; // linear blend with original image
@group(0) @binding(6) var u_downsample_texture: texture_2d<f32>; // mip level i-1 of downsample chain

@compute @workgroup_size(8, 8, 1)
fn main(
    @builtin(global_invocation_id) GlobalInvocationID : vec3<u32>,
    @builtin(workgroup_id) WorkGroupID : vec3<u32>,
    @builtin(local_invocation_index) LocalInvocationIndex : u32, // ranges from 0 - 63
    @builtin(local_invocation_id) LocalInvocationID : vec3<u32>
)
{
    let output_size = textureDimensions(u_output_texture);
	let pixel_coords = vec2<i32>(GlobalInvocationID.xy);

    let u_FilterRadius = 0.001;
    let x = u_FilterRadius;
    let y = u_FilterRadius;

    let uv        = (vec2f(pixel_coords.xy) + vec2f(0.5)) / vec2f(f32(output_size.x), f32(output_size.y));

    let a = textureSampleLevel(u_input_texture, u_sampler, vec2f(uv.x - x, uv.y + y), 0.0).rgb;
    let b = textureSampleLevel(u_input_texture, u_sampler, vec2f(uv.x,     uv.y + y), 0.0).rgb;
    let c = textureSampleLevel(u_input_texture, u_sampler, vec2f(uv.x + x, uv.y + y), 0.0).rgb;
    let d = textureSampleLevel(u_input_texture, u_sampler, vec2f(uv.x - x, uv.y), 0.0).rgb;
    let e = textureSampleLevel(u_input_texture, u_sampler, vec2f(uv.x,     uv.y), 0.0).rgb;
    let f = textureSampleLevel(u_input_texture, u_sampler, vec2f(uv.x + x, uv.y), 0.0).rgb;
    let g = textureSampleLevel(u_input_texture, u_sampler, vec2f(uv.x - x, uv.y - y), 0.0).rgb;
    let h = textureSampleLevel(u_input_texture, u_sampler, vec2f(uv.x,     uv.y - y), 0.0).rgb;
    let i = textureSampleLevel(u_input_texture, u_sampler, vec2f(uv.x + x, uv.y - y), 0.0).rgb;

    // Apply weighted distribution, by using a 3x3 tent filter:
    //  1   | 1 2 1 |
    // -- * | 2 4 2 |
    // 16   | 1 2 1 |
    var bloom = e*4.0;
    bloom += (b+d+f+h)*2.0;
    bloom += (a+c+g+i);
    bloom *= (1.0 / 16.0);

	let curr_pixel = textureLoad(u_downsample_texture, pixel_coords, 0);
    var out_pixel = curr_pixel.rgb;

    // if this is last mip level
    if (all(output_size == u_full_resolution_size)) {
        out_pixel = mix(curr_pixel.rgb, bloom, u_final_blend);
    } else {
        out_pixel = mix(curr_pixel.rgb, bloom, u_internal_blend);
    }

	textureStore(u_output_texture, pixel_coords, vec4f(out_pixel, 1.0));
}

)glsl";

// Skybox shader -------------------------

const char* skybox_shader_string = R"glsl(
    #include FRAME_UNIFORMS
    #include ENVIRONMENT_MAP_UNIFORMS

    // need at least 1 binding so getBindGroupLayout doesn't crash...
    @group(1) @binding(0) var u_envmap_sampler: sampler;

    struct VSOutput {
        @builtin(position) position: vec4f,
        @location(0) pos: vec4f,
        // @location(0) v_skybox_normal: vec3f,
    };

    var<private> pos : array<vec2f, 3> = array(
        vec2f(-1, 3),
        vec2f(-1,-1),
        vec2f( 3,-1),
    );

    fn srgbToLinear(c : vec4f) -> vec4f {
        return vec4f(
            pow(c.r, 2.2),
            pow(c.g, 2.2),
            pow(c.b, 2.2),
            c.a
        );
    }
    
    @vertex 
    fn vs_main(@builtin(vertex_index) vertexIndex: u32) -> VSOutput {
        var output : VSOutput;
        output.position = vec4f(pos[vertexIndex], 1, 1);
        output.pos = output.position;

        // output.v_skybox_normal = skybox_positions[vertexIndex];
        // var position = u_frame.projection * u_frame.view * vec4f(output.v_skybox_normal, 1.0);
        // position.z = position.w;  // force z to be 1.0 after perspective division
        // output.position = position;

        return output;
    }

    @fragment
    fn fs_main(vsOut: VSOutput) -> @location(0) vec4f {
        let t = u_frame.projection_view_inverse_no_translation * vsOut.pos;
        var normal = normalize(t.xyz / t.w) * vec3f(1, 1, -1);
        return u_frame.background_color * srgbToLinear(textureSample(u_envmap, u_envmap_sampler, normal));

        // let tmp = textureSample(u_envmap, u_envmap_sampler, normalize(t.xyz / t.w) * vec3f(1, 1, -1));
        // return vec4f(1.0);

        // let color = u_frame.background_color * srgbToLinear(textureSample(u_envmap, u_envmap_sampler, vsOut.v_skybox_normal));
        // return color;
    }
)glsl";

// ======================================
// box2d debug shaders
// ======================================

const char* b2_solid_polygon_shader_string = R"glsl(
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


// layout(location = 0) in vec2 v_localPosition; // get this from QUAD_VERTICES and vertex_idx
// layout(location = 1) in vec4 v_instanceTransform; 
// layout(location = 2) in vec4 v_instancePoints12;
// layout(location = 3) in vec4 v_instancePoints34;
// layout(location = 4) in vec4 v_instancePoints56;
// layout(location = 5) in vec4 v_instancePoints78;
// layout(location = 6) in int v_instanceCount;
// layout(location = 7) in float v_instanceRadius;
// layout(location = 8) in vec4 v_instanceColor;

    struct Polygon {
        transform : vec4f, // x, y, sin rotation, cos rotation
        points12 : vec4f, // x1, y1, x2, y2
        points34 : vec4f, // x3, y3, x4, y4
        points56 : vec4f, // x5, y5, x6, y6
        points78 : vec4f, // x7, y7, x8, y8
        color : vec4f,
        aabb : vec4f,     // lower left, upper right
        count : i32,      // number of vertices
        radius : f32,
    };

    @group(3) @binding(0) var<storage> u_polygon_instances : array<Polygon>;

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


        // @location(2) f_points: array<vec2f, 8>,  // why would we want to interpolate this??
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
        let polygon = u_polygon_instances[polygon_instance_idx];

        out.f_position = quad_vertex;
        out.f_color = polygon.color;
        out.f_radius = polygon.radius;
        out.f_count = polygon.count;

        var f_points = array(
            vec2f(polygon.points12.xy),
            vec2f(polygon.points12.zw),
            vec2f(polygon.points34.xy),
            vec2f(polygon.points34.zw),
            vec2f(polygon.points56.xy),
            vec2f(polygon.points56.zw),
            vec2f(polygon.points78.xy),
            vec2f(polygon.points78.zw)
        );
        // TODO idk how to pass this to the frag shader... can frag shader sample?
        // out.f_points[0] = polygon.points12.xy; // TODO change polygon.points to a vec2f array
        // out.f_points[1] = polygon.points12.zw;
        // out.f_points[2] = polygon.points34.xy;
        // out.f_points[3] = polygon.points34.zw;
        // out.f_points[4] = polygon.points56.xy;
        // out.f_points[5] = polygon.points56.zw;
        // out.f_points[6] = polygon.points78.xy;
        // out.f_points[7] = polygon.points78.zw;

        // Compute polygon AABB
        let lower = polygon.aabb.xy;
        let upper = polygon.aabb.zw;

        let center : vec2f = 0.5 * (lower + upper);
        let width : vec2f = upper - lower;
        let maxWidth : f32 = max(width.x, width.y);

        let scale : f32 = out.f_radius + 0.5 * maxWidth;
        let invScale = 1.0 / scale;

        // Shift and scale polygon points so they fit in 2x2 quad
        for (var i = 0; i < polygon.count; i++)
        {
            f_points[i] = invScale * (f_points[i] - center);
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
        out.f_thickness = 3.0f / (1.0 * scale);

        // scale up and transform quad to fit polygon
        let x : f32 = polygon.transform.x;
        let y : f32 = polygon.transform.y;
        let c : f32 = polygon.transform.z;
        let s : f32 = polygon.transform.w;
        var p = scale * quad_vertex + center;
        p = vec2f((c * p.x - s * p.y) + x, (s * p.x + c * p.y) + y);

        var u_Draw : DrawUniforms = u_draw_instances[instance_idx];
        out.position = (u_frame.projection * u_frame.view) * u_Draw.model * vec4f(p, 0.0, 1.0f);

        return out;
    }

    // begin fragment shader ----------------------------

    // https://en.wikipedia.org/wiki/Alpha_compositing
    // fn blend_colors(front : vec4f, back : vec4f) -> vec4f
    // {
    //     vec3 cSrc = front.rgb;
    //     float alphaSrc = front.a;
    //     vec3 cDst = back.rgb;
    //     float alphaDst = back.a;

    //     vec3 cOut = cSrc * alphaSrc + cDst * alphaDst * (1.0 - alphaSrc);
    //     float alphaOut = alphaSrc + alphaDst * (1.0 - alphaSrc);

    //     // remove alpha from rgb
    //     cOut = cOut / alphaOut;

    //     return vec4(cOut, alphaOut);
    // }

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
        // vec4 borderColor = f_color;
        // vec4 fillColor = 0.6f * borderColor;
        let fillColor = in.f_color;

        var f_points = array(
            in.f_points12.xy, in.f_points12.zw,
            in.f_points34.xy, in.f_points34.zw,
            in.f_points56.xy, in.f_points56.zw,
            in.f_points78.xy, in.f_points78.zw
        );

        let dw = sdConvexPolygon(in.f_position, &f_points, in.f_count);
        let d = abs(dw - in.f_radius);

        // roll the fill alpha down at the border
        let back = vec4f(fillColor.rgb, fillColor.a * smoothstep(in.f_radius + in.f_thickness, in.f_radius, dw));

        // roll the border alpha down from 1 to 0 across the border thickness
        // vec4 front = vec4(borderColor.rgb, smoothstep(f_thickness, 0.0f, d));

        // fragColor = blend_colors(front, back);
        // return in.v_color;
        return back;
    }
)glsl";

static const char* shadow_vertex_string = R"glsl(

@group(0) @binding(0) var<uniform> lightViewProjMatrix : mat4x4f;
@group(1) @binding(0) var<uniform> modelMatrix : mat4x4f;

@vertex
fn main(
@location(0) position: vec3f
) -> @builtin(position) vec4f {
    return lightViewProjMatrix * modelMatrix * vec4(position, 1.0);
}

)glsl";

// clang-format on

std::string Shaders_genSource(const char* src)
{
    size_t include_len = strlen("#include ");
    std::string source(src);

    size_t pos = 0;
    while (pos <= source.size() - include_len) {
        switch (source[pos]) {
            case '/': {
                ++pos;
                if (source[pos] == '/') {
                    // consume line comment
                    pos = source.find('\n', pos);
                } else if (source[pos] == '*') {
                    // consume block comment
                    pos = source.find("*/", pos);
                }
            } break;
            case '#': {
                if (strncmp(source.c_str() + pos, "#include ", include_len) == 0) {
                    size_t start = source.find_first_not_of(WHITESPACE_CHARS ";",
                                                            pos + include_len);
                    size_t end   = source.find_first_of(WHITESPACE_CHARS ";", start);

                    std::string include_name = source.substr(start, end - start);
                    if (shader_table.count(include_name)) {
                        source.replace(pos, end - pos, shader_table[include_name]);
                    } else {
                        pos = end;
                    }
                } else {
                    ++pos;
                }
            } break;
            default: ++pos;
        }
    }

    return source;
}
