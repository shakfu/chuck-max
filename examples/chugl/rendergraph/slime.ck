//-----------------------------------------------------------------------------
// name: slime.ck
// desc: slime mold simulation using compute shaders
// 
// author: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//   date: Summer 2025
//
// https://www.youtube.com/watch?v=X-iSQQgOd1A&ab_channel=SebastianLague
// https://uwe-repository.worktribe.com/output/980579
// https://cargocollective.com/sagejenson/physarum
//-----------------------------------------------------------------------------

// simulation parameters
150000 => int NUM_SLIMES;  
1920 => int RESOLUTION_X;
1080 => int RESOLUTION_Y;

// dissolve shader params
UI_Float diffusion_speed(1.0);
UI_Float dissolve_factor(.95);

// agent shader params
UI_Float sensor_offset(8.0);
UI_Int sensor_size(4);
UI_Float sense_angle_deg(45);
UI_Float speed_pixels(60);
UI_Float turn_speed(16);
UI_Int simulation_mode(0);
UI_Bool pause;

// screen shader params
UI_Float3 base_color(0, 0, 0);
UI_Float3 highlight_color(1.0, 1.0, 1.0);

ShaderDesc compute_shader_desc;
"
struct Agent {
    pos : vec2f,
    rot : f32,
    pad : f32,
}

@group(0) @binding(0) var<storage, read_write> agents: array<Agent>;
@group(0) @binding(1) var trail_texture_read: texture_2d<f32>;
@group(0) @binding(2) var trail_texture_write: texture_storage_2d<rgba8unorm, write>;
@group(0) @binding(3) var<uniform> dt : f32;

@group(0) @binding(4) var<uniform> sensor_offset : f32;  // in pixels
@group(0) @binding(5) var<uniform> sensor_size : i32;    // in pixels
@group(0) @binding(6) var<uniform> sense_angle_deg : f32;
@group(0) @binding(7) var<uniform> speed_pixels : f32;
@group(0) @binding(8) var<uniform> turn_speed : f32;

@group(0) @binding(9) var<uniform> simulation_mode : i32;

fn deg2rad(deg: f32) -> f32 { return deg * 3.14159 / 180.0; }
fn rand(co: f32) -> f32 { return fract(sin(co*(91.3458)) * 47453.5453); }
fn rand2(co: vec2f) -> f32 { return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453); }
fn rand3(co: vec3f) -> f32 { return rand2(co.xy+rand(co.z)); }

fn sense(pos_pixel: vec2f, rot: f32, delta_angle: f32, dim: vec2i) -> f32 {
    var sensor_dir = vec2f(cos(rot + delta_angle), sin(rot + delta_angle));
    var sensor_pos = pos_pixel + sensor_dir * sensor_offset;
    var sum = 0.0;

    for (var dx = -sensor_size; dx <= sensor_size; dx++) {
        for (var dy = -sensor_size; dy <= sensor_size; dy++) {
            let coord = vec2i(sensor_pos + vec2f(f32(dx), f32(dy)));
            // can improve sampling with linear filtering
            if (coord.x >= 0 && coord.x < dim.x && coord.y >= 0 && coord.y < dim.y) {
                sum += textureLoad(trail_texture_read, coord, 0).r; // NOTE: assuming every channel holds same value
            }
        }
    }

    return sum;
}

@compute @workgroup_size(64, 1, 1)
fn main(@builtin(global_invocation_id) GlobalInvocationID : vec3<u32>) {
    let index = GlobalInvocationID.x;
    if (index >= arrayLength(&agents)) {
        return;
    }

    var pos = agents[index].pos;
    var rot = agents[index].rot;
    let random = rand3(vec3(pos, f32(index)));

    let dim = vec2i(textureDimensions(trail_texture_read));
    var pos_pixel = pos * vec2f(dim);

    // steer
    let left = sense(pos_pixel, rot, deg2rad(sense_angle_deg), dim);
    let forward = sense(pos_pixel, rot, 0.0, dim);
    let right = sense(pos_pixel, rot, deg2rad(-sense_angle_deg), dim);


    if (simulation_mode == 0) {
        // original implementation
        // relative to the forward weight, left and right can either be gt or less than forward,
        // yielding 2 * 2 = 4 possible combinations:
        if (forward > left && forward > right) {
            // do nothing
        } else if (forward < left && forward < right) {
            // turn randomly
            rot += 2.0 * (random - 0.5) * turn_speed * dt;
        } else if (right > left) { // right > forward > left
            rot -= random * turn_speed * dt;
        } else if (left > right) { // right < foward < left
            rot += random * turn_speed * dt;
        }
    } else {
        // Picking based on a weighted pdf (also looks cool, more chaotic/extreme)
        let total = left + forward + right;
        let weighted_l = left / total;
        let weighted_r = right / total;
        let weighted_f = forward / total;
        if (random < weighted_l) {
            rot += turn_speed * dt;
        } else if (random >= weighted_l && random < weighted_l + weighted_r) {
            rot -= turn_speed * dt;
        } else {
            // go forward
        }
    }

    pos_pixel += vec2f(cos(rot), sin(rot)) * speed_pixels * dt;
    pos = pos_pixel / vec2f(dim);

    // boundaries
    if (pos.x < 0 || pos.x > 1 || pos.y < 0 || pos.y > 1) {
        pos = clamp(pos, vec2f(0.0), vec2f(1.0));
        rot = random * 2.0 * 3.14159;
    }

    // write back
    agents[index].pos = pos;
    agents[index].rot = rot;

    // dump to texture
    var tex_coords = vec2u(pos * vec2f(textureDimensions(trail_texture_write)));
    textureStore(trail_texture_write, tex_coords, vec4f(1));
}
" => compute_shader_desc.computeCode;
Shader agent_shader(compute_shader_desc);

ShaderDesc trail_shader_desc;
"
@group(0) @binding(0) var trail_texture_read: texture_2d<f32>;
@group(0) @binding(1) var trail_texture_write: texture_storage_2d<rgba8unorm, write>;

@group(0) @binding(2) var<uniform> dt: f32;
@group(0) @binding(3) var<uniform> diffusion_speed: f32;
@group(0) @binding(4) var<uniform> dissolve_factor: f32;

@compute @workgroup_size(8, 8, 1)
fn main(@builtin(global_invocation_id) id : vec3u) {
    let dim = textureDimensions(trail_texture_read);
    if (id.x >= dim.x || id.y >= dim.y) {
        return;
    }

    let original_value = textureLoad(trail_texture_read, id.xy, 0);

    // diffuse
    var sum = vec4f(0.0);
    for (var dx = -1; dx <= 1; dx++) {
        for (var dy = -1; dy <= 1; dy++) {
            var sample = vec2i(id.xy) + vec2i(dx, dy);
            if (sample.x >= 0 && sample.x < i32(dim.x) && sample.y >= 0 && sample.y < i32(dim.y)) {
                sum += textureLoad(trail_texture_read, sample, 0);
            }
        }
    }

    var diffuse = mix(original_value, sum / 9.0, diffusion_speed * dt);

    // dissolve 
    if (dt > 0.0) {
        diffuse *= dissolve_factor;
        if (diffuse.r < .0333) {
            diffuse = vec4f(0.0); // remove ghost trails
        }
    }

    // dump to texture
    textureStore(trail_texture_write, id.xy, diffuse);
}
" => trail_shader_desc.computeCode;
Shader trail_shader(trail_shader_desc);


" 
    #include FRAME_UNIFORMS
    @group(1) @binding(0) var src: texture_2d<f32>;
    @group(1) @binding(1) var texture_sampler: sampler;
    @group(1) @binding(2) var<uniform> base_color: vec3f;
    @group(1) @binding(3) var<uniform> highlight_color: vec3f;

    struct VertexOutput {
        @builtin(position) position : vec4<f32>,
        @location(0) v_uv : vec2<f32>,
    };

    fn rand(co: f32) -> f32 { return fract(sin(co*(91.3458)) * 47453.5453); }
    fn rand2(co: vec2f) -> f32 { return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453); }
    fn rand3(co: vec3f) -> f32 { return rand2(co.xy+rand(co.z)); }

    @vertex 
    fn vs_main(@builtin(vertex_index) vertexIndex : u32) -> VertexOutput {
        var output : VertexOutput;

        // a triangle which covers the screen
        output.v_uv = vec2f(f32((vertexIndex << 1u) & 2u), f32(vertexIndex & 2u));
        output.position = vec4f(output.v_uv * 2.0 - 1.0, 0.0, 1.0);
        
        return output;
    }

    @fragment 
    fn fs_main(in : VertexOutput) -> @location(0) vec4f {
        let t0 = u_frame;

        var uv = in.position.xy / vec2f(u_frame.resolution.xy); // interesting. fragCoord doesn't change based on viewport
        uv.y = 1.0 - uv.y;

        let trail_value = textureSampleLevel(src, texture_sampler, in.v_uv, 0.0).r;
        let col = mix(base_color, highlight_color, trail_value);
        return vec4f(
                col,
                1.0
        );
    }
" => string screen_shader;

ShaderDesc desc;
screen_shader => desc.vertexCode => desc.fragmentCode;
null => desc.vertexLayout; // screen shader does not take vertex data

Shader shader(desc);
shader.name("screen shader");

// storage buffer to hold agent data
StorageBuffer slime_buffer;
slime_buffer.size(4 * NUM_SLIMES);
float slime_init[4 * NUM_SLIMES];

fun void initSlimeBuffer() {
    for (int i; i < NUM_SLIMES; i++) {
        // set position (xy)
        0.5 => slime_init[4*i + 0];
        0.5 => slime_init[4*i + 1];

        // set heading (radians)
        Math.random2f(0, Math.two_pi) => slime_init[4*i + 2];

        // padding (Agents struct on GPU takes up 16 bytes)
        0 => slime_init[4*i + 3];
    }
    slime_buffer.write(slime_init);
}

// render graph
GG.rootPass() --> ComputePass agent_pass(agent_shader) --> ComputePass trail_pass(trail_shader) --> ScreenPass screen_pass(shader);

// textures to hold slime trail data
TextureDesc trail_tex_desc;
RESOLUTION_X => trail_tex_desc.width;
RESOLUTION_Y => trail_tex_desc.height;
false => trail_tex_desc.mips;

Texture trail_tex_a(trail_tex_desc); 
Texture trail_tex_b(trail_tex_desc); 

fun void swapBuffers() {
    (GG.fc() % 2 == 0) => int swap;

    swap ? trail_tex_a : trail_tex_b @=> Texture read_tex;
    swap ? trail_tex_b : trail_tex_a @=> Texture write_tex;

    agent_pass.texture(1, read_tex); // read
    agent_pass.storageTexture(2, write_tex); // write

    trail_pass.texture(0, write_tex); // read
    trail_pass.storageTexture(1, read_tex); // write

    screen_pass.material().texture(0, read_tex);
}

fun void setUniforms(float dt) {
    agent_pass.uniformFloat(3, dt);
    agent_pass.uniformFloat(4, sensor_offset.val());
    agent_pass.uniformInt(5, sensor_size.val());
    agent_pass.uniformFloat(6, sense_angle_deg.val());
    agent_pass.uniformFloat(7, speed_pixels.val());
    agent_pass.uniformFloat(8, turn_speed.val());
    agent_pass.uniformInt(9, simulation_mode.val());

    trail_pass.uniformFloat(2, dt);
    trail_pass.uniformFloat(3, diffusion_speed.val());
    trail_pass.uniformFloat(4, dissolve_factor.val());

    screen_pass.material().uniformFloat3(2, base_color.val());
    screen_pass.material().uniformFloat3(3, highlight_color.val());
}

initSlimeBuffer();
swapBuffers();
setUniforms(0);
agent_pass.storageBuffer(0, slime_buffer);
screen_pass.material().sampler(1, TextureSampler.linear());
agent_pass.workgroup((NUM_SLIMES / 64) + 1, 1, 1);
trail_pass.workgroup((RESOLUTION_X / 8) + 1, (RESOLUTION_Y / 8) + 1, 1);

while (1) {
    GG.nextFrame() => now;

    if (UI.begin("")) {
        UI.slider("trail diffusion speed", diffusion_speed, 0.0, 20.0);
        UI.slider("trail dissolve rate", dissolve_factor, 0.0, 1.0);
        
        UI.slider("slime sensor distance (pixels)", sensor_offset, 0.0, 20.0);
        UI.slider("slime sensor size (pixels)", sensor_size, 0, 8);
        UI.slider("slime sensor angle (degrees)", sense_angle_deg, 0, 90);
        UI.slider("slime speed", speed_pixels, 0, 120);
        UI.slider("slime turn speed", turn_speed, 0, 32);
        UI.listBox("simulation mode", simulation_mode, ["Default", "Weighted PDF"]);

        UI.colorEdit("base color", base_color);
        UI.colorEdit("highlight color", highlight_color);

        UI.checkbox("pause", pause);

        if (UI.button("restart simulation")) {
            initSlimeBuffer();
        }
    }
    UI.end();

    swapBuffers();
    setUniforms(pause.val() ? 0.0 : GG.dt());
}
