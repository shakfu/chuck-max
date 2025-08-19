SphereGeometry sphere_geo;
SuzanneGeometry suzanne_geo;
Material custom_material;

// patch
adc => Gain g => OnePole p => blackhole;
// square the input
adc => g;
// multiply
3 => g.op;

// set filter pole position (between 0 and 1)
// NOTE: this controls how smooth the output is
// closer to 1 == smoother but less responsive
// closer to 0 == more jumpy but also more responsive
0.97 => p.pole;

"struct FrameUniforms {
    projectionMat: mat4x4f,
    viewMat: mat4x4f,
    projViewMat: mat4x4f,
    camPos: vec3f, // camera
    dirLight: vec3f, // lighting
    time: f32,
};

@group(0) @binding(0) var<uniform> u_frame: FrameUniforms;

// our custom material uniforms
@group(1) @binding(0) var<uniform> u_grayscale: f32;
@group(1) @binding(1) var<storage> u_color: array<f32>;

// TODO handle normal Mat
struct DrawUniforms {
    modelMat: mat4x4f,
    // normalMat: mat4x4f,  // needed to account for non-uniform scaling
};

@group(2) @binding(0) var<storage> u_draw_instances: array<DrawUniforms>;


struct VertexOutput {
    @builtin(position) position : vec4f,
    @location(0) v_worldPos : vec3f,
    @location(1) v_normal : vec3f,
    @location(2) v_uv : vec2f,
    @location(3) v_tangent : vec4f,
};

struct VertexInput {
    @location(0) position : vec3f,
    @location(1) normal : vec3f,
    @location(2) uv : vec2f,
    @location(3) tangent : vec4f,
    @builtin(instance_index) instance : u32,
};

@vertex 
fn vs_main(in : VertexInput) -> VertexOutput
{
    var out : VertexOutput;
    var u_Draw : DrawUniforms = u_draw_instances[in.instance];

    let modelMat3 : mat3x3<f32> = mat3x3(
        u_Draw.modelMat[0].xyz,
        u_Draw.modelMat[1].xyz,
        u_Draw.modelMat[2].xyz
    );

    var worldPos : vec4f = u_frame.projViewMat * u_Draw.modelMat * vec4f(in.position, 1.0f);
    out.v_worldPos = worldPos.xyz;
    out.v_normal = (u_Draw.modelMat * vec4f(in.normal, 0.0)).xyz;
    out.v_tangent = vec4f(modelMat3 * in.tangent.xyz, in.tangent.w);
    out.v_uv     = in.uv;
    out.position = worldPos;

    return out;
}

@fragment 
fn fs_main(in : VertexOutput, @builtin(front_facing) is_front: bool) -> @location(0) vec4f
{
    var normal : vec3f;
    if (is_front) {
        normal = in.v_normal;
    } else {
        normal = -in.v_normal;
    }

    // valve half-lambert
    // https://developer.valvesoftware.com/wiki/Half_Lambert
    var diffuse : f32 = 0.5 * dot(normal, -u_frame.dirLight) + 0.5;
    diffuse = diffuse * diffuse;
    let u_color_len : u32 = arrayLength(&u_color);
    return vec4f(diffuse * u_grayscale * vec3f(u_color[u_color_len - 3u], u_color[u_color_len - 2u], u_color[u_color_len - 1u]), 1.0);
}" => string lambert_shader_string;

ShaderDesc shader_desc;
lambert_shader_string => shader_desc.vertexCode;
lambert_shader_string => shader_desc.fragmentCode;
sphere_geo.vertexAttributeNumComponents() @=> shader_desc.vertexLayout;

Shader custom_shader(shader_desc); // create shader from shader_desc
custom_shader => custom_material.shader; // connect shader to material

GGen big_suzanne --> GG.scene();
for (-1 => int i; i < 2; i++) {
    for (-1 => int j; j < 2; j++) {
        GMesh custom_mesh(suzanne_geo, custom_material) --> big_suzanne;
        // GMesh custom_mesh(sphere_geo, custom_material) --> GG.scene();
        custom_mesh.translate(2.0 * @(i, j, 0));
    }
}


custom_material.uniformFloat(0, 1.0);
[0.0, 1.0, 1.0] @=> float storage_buffer[];
custom_material.storageBuffer(1, storage_buffer);

// test changing material pso
fun void psoChanger() {
    while (1::second => now) {
        // 1 - custom_material.cullMode() => custom_material.cullMode;
        (custom_material.topology() + 1) % (Material.TOPOLOGY_TRIANGLESTRIP + 1) => custom_material.topology;

        // update storage buffer
        for (0 => int i; i < 3; i++) {
           Math.random2f(0.0, 1.0) => storage_buffer[i];
        }
        storage_buffer << 1.0; // update size to force bindgroupentry rebuild
        custom_material.storageBuffer(1, storage_buffer);
    }
} 
// spork ~ psoChanger();

// test switching scenes AND changing material pso
// correct behavior: changing the pso of a material that belongs to multiple scenes
// should update all scenes 
fun void sceneSwitcher() {
    GG.scene() @=> GScene@ default_scene;
    GScene scene1;
    GMesh mesh(suzanne_geo, custom_material) --> scene1;
    while (true) {
        (custom_material.topology() + 1) % (Material.TOPOLOGY_TRIANGLESTRIP + 1) => custom_material.topology;
        2::second => now; // render current scene for 1 second
        GG.scene(scene1); // switch to new scene
        2::second => now;
        GG.scene(default_scene); // switch back to default scene
    }
}
spork ~ sceneSwitcher();



while (true) {
    GG.nextFrame() => now;
    custom_material.uniformFloat(0, .5 * Math.sin(now/second) + 0.5);

    // get volume
    .75 + Math.sqrt(p.last()) => big_suzanne.sca;
}