//-----------------------------------------------------------------------------
// name: boids_compute.ck
// desc: boids with compute shaders
// 
// author: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//   date: Fall 2024
//-----------------------------------------------------------------------------
// uncomment for full screen
// GWindow.fullscreen();

// audio graph
adc => Gain g => OnePole p => blackhole;
// square the input
adc => g;
// multiply
3 => g.op;

// set filter pole position (between 0 and 1)
// NOTE: this controls how smooth the output is
// closer to 1 == smoother but less responsive
// closer to 0 == more jumpy but also more responsive
0.999 => p.pole;

// Render Graph
GG.rootPass() --> ComputePass compute_pass --> GG.renderPass();
5000 => int NUM_BOIDS;
(NUM_BOIDS / 64) + 1 => int work_group_count;
compute_pass.workgroup(work_group_count, 1, 1);

// camera
GCamera camera --> GG.scene();
camera.orthographic();
camera.viewSize(5.0);
camera.posZ(5.0);
GG.scene().camera(camera);

// compute/simluation shader code string
"
struct Boid {
    pos : vec2f,
    vel : vec2f,
}

// boid data
@group(0) @binding(0) var<storage, read> boids_curr : array<Boid>;
@group(0) @binding(1) var<storage, read_write> boids_next : array<Boid>;
@group(0) @binding(2) var<uniform> num_boids : u32;

// simulation parameters
@group(0) @binding(3) var<uniform> dt : f32;
@group(0) @binding(4) var<uniform> cohesion_radius : f32;   // move towards the center of mass
@group(0) @binding(5) var<uniform> separation_radius : f32; // avoid crowding
@group(0) @binding(6) var<uniform> alignment_radius : f32;  // match velocity with neighbors
@group(0) @binding(7) var<uniform> cohesion_factor : f32;
@group(0) @binding(8) var<uniform> separation_factor : f32;
@group(0) @binding(9) var<uniform> alignment_factor : f32;
@group(0) @binding(10) var<uniform> avoidance_factor : f32; // avoid obstacles
@group(0) @binding(11) var<uniform> mouse_cursor: vec2f;
@group(0) @binding(12) var<uniform> volume: f32;
@group(0) @binding(13) var<uniform> volume_factor: f32;

@compute @workgroup_size(64, 1, 1)
fn main(@builtin(global_invocation_id) GlobalInvocationID : vec3<u32>) {
    var index = GlobalInvocationID.x;
    if (index >= num_boids) {
        return;
    }

    var my_pos = boids_curr[index].pos;
    var my_vel = boids_curr[index].vel;
    
    var center_of_mass = vec2f(0.0f);
    var separation = vec2f(0.0f);
    var alignment = vec2f(0.0f);
    var center_of_mass_count = 0;
    var alignment_count = 0;

    for (var i = 0u; i < num_boids; i++) {
        if (i == index) {
            continue;
        }

        var pos = boids_curr[i].pos;
        var vel = boids_curr[i].vel;
        var dist = distance(my_pos, pos);

        if (dist < cohesion_radius) {
            center_of_mass += pos;
            center_of_mass_count++;
        }
        if (dist < separation_radius) {
            separation += ((my_pos - pos) * (1.0f / dist));
            // separation += (my_pos - pos);
        }
        if (dist < alignment_radius) {
            alignment += vel;
            alignment_count++;
        }
    } // end foreach boid

    if (center_of_mass_count > 0) {
        center_of_mass /= f32(center_of_mass_count);
    }

    if (alignment_count > 0) {
        alignment /= f32(alignment_count);
    }

    var modified_alignment_factor = alignment_factor - (volume * volume_factor);
    modified_alignment_factor = max(modified_alignment_factor, 0.0f);

    my_vel += (center_of_mass - my_pos) * cohesion_factor + 
               separation * separation_factor + 
               alignment * modified_alignment_factor;

    // avoid edges
    let x_bound = 2.0f;
    let y_bound = 2.0f;
    if (my_pos.x < -x_bound) {
        my_vel.x += 1.0f * avoidance_factor;
    }
    if (my_pos.x > x_bound) {
        my_vel.x -= 1.0f * avoidance_factor;
    }
    if (my_pos.y < -y_bound) {
        my_vel.y += 1.0f * avoidance_factor;
    }
    if (my_pos.y > y_bound) {
        my_vel.y -= 1.0f * avoidance_factor;
    }

    // bias towards mouse cursor
    if (distance(my_pos, mouse_cursor) < 0.5) {
        my_vel += (mouse_cursor - my_pos) * 0.1;
    }

    // clamp velocity
    if (volume > .1) {
        my_vel = normalize(my_vel) * clamp(length(my_vel), 0.01, 0.1 + volume * volume_factor * 4.0);
    } else {
        my_vel = normalize(my_vel) * clamp(length(my_vel), 0.01, 0.1);
    }

    // update position
    my_pos += my_vel * dt;

    // write next
    boids_next[index].pos = my_pos;
    boids_next[index].vel = my_vel;
}
" @=> string compute_shader_code;

// drawing shader code string
"
struct Boid {
    pos : vec2f,
    vel : vec2f,
}

#include FRAME_UNIFORMS
#include DRAW_UNIFORMS;

@group(1) @binding(0) var<storage> boids : array<Boid>;
@group(1) @binding(1) var<uniform> volume: f32;

@group(3) @binding(0) var<storage> boids_geometry : array<f32>; // 3 vec2f per boid

struct VertexOutput {
    @builtin(position) position : vec4f,
};

@vertex 
fn vs_main(
    @builtin(vertex_index) vertexIndex : u32, 
    @builtin(instance_index) instanceIndex : u32
) -> VertexOutput {
    var out : VertexOutput;
    var model_mat : mat4x4f = u_draw_instances[instanceIndex].model;

    // 3 vertices per boid
    var boid : Boid = boids[vertexIndex / 3u];
    var vertex : vec2f = vec2f(boids_geometry[(vertexIndex % 3u) * 2u + 0u], boids_geometry[(vertexIndex % 3u) * 2u + 1u]);

    let angle = -atan2(boid.vel.x, boid.vel.y);
    let rot_matrix = mat2x2f(cos(angle), sin(angle), -sin(angle), cos(angle));
    let pos = rot_matrix * vertex;

    out.position = u_frame.projection * u_frame.view * model_mat * vec4f(boid.pos + pos, 0.0f, 1.0f);
    return out;
}

@fragment 
fn fs_main(in : VertexOutput) -> @location(0) vec4f
{
    if (volume > .1) {
        let delta = max(0., 1.0 - 3.0 * volume);
        return vec4f(1.0f, delta, delta, 1.0f);
    } else {
        return vec4f(1.0f);
    }
}
" @=> string shader_code;

// set simulation shader
ShaderDesc sim;
compute_shader_code => sim.computeCode;
Shader compute_shader(sim);
compute_pass.shader(compute_shader);

// set drawing shader
ShaderDesc draw;
shader_code => draw.vertexCode;
shader_code => draw.fragmentCode;
null => draw.vertexLayout; // no vertex layout
// material shader
Shader mat(draw);
Material boids_material;
boids_material.shader(mat);

Geometry boids_geo;
boids_geo.vertexCount(3 * NUM_BOIDS); // 1 triangle per boid
// 3 vertices per boid                x0   y0    x1   y1   x2   y2
boids_geo.pulledVertexAttribute(0, [-.05, -0.05, .05, -.05, 0, 0.05]);
GMesh boids_mesh(boids_geo, boids_material) --> GG.scene();

StorageBuffer boids_buffer_a;
StorageBuffer boids_buffer_b;
// populate initial boids buffer
boids_buffer_a.size(NUM_BOIDS * 4); // 4 fields (pos_x, pos_y, vel_x, vel_y)
boids_buffer_b.size(NUM_BOIDS * 4);

float initial_boid_pos_and_velocity[NUM_BOIDS * 4];
for (int i; i < initial_boid_pos_and_velocity.size(); i++) {
    Math.random2f(-1.0, 1.0) => initial_boid_pos_and_velocity[i]; // pos_x
}
boids_buffer_a.write(initial_boid_pos_and_velocity);

UI_Float cohesion_radius(.35);
UI_Float separation_radius(.1);
UI_Float alignment_radius(.5);
UI_Float cohesion_factor(.002);
UI_Float separation_factor(.005);
UI_Float alignment_factor(.1);
UI_Float volume_factor(.0);
UI_Float volume(p.last());

fun void initBindGroups() {
    compute_pass.storageBuffer(0, boids_buffer_a); // read
    compute_pass.storageBuffer(1, boids_buffer_b); // write
    compute_pass.uniformInt(2, NUM_BOIDS);
    compute_pass.uniformFloat(3, 0.16); // dt
    compute_pass.uniformFloat(4, cohesion_radius.val()); // cohesion_radius
    compute_pass.uniformFloat(5, separation_radius.val()); // separation_radius
    compute_pass.uniformFloat(6, alignment_radius.val()); // alignment_radius
    compute_pass.uniformFloat(7, cohesion_factor.val()); // cohesion_factor
    compute_pass.uniformFloat(8, separation_factor.val()); // separation_factor
    compute_pass.uniformFloat(9, alignment_factor.val()); // alignment_factor
    compute_pass.uniformFloat(10, .01); // avoidance_factor
    compute_pass.uniformFloat2(11, @(0.0, 0.0)); // mouse cursor
    compute_pass.uniformFloat(12, p.last());
    compute_pass.uniformFloat(13, volume_factor.val());

    boids_material.storageBuffer(0, boids_buffer_a);
    boids_material.uniformFloat(1, volume.val());
}
initBindGroups();

fun void ui() {
    while (true) {
        GG.nextFrame() => now;
        UI.setNextWindowSize(@(400, 600), UI_Cond.Once);
        if (UI.begin("Compute Shader Example", null, 0)) {

            if (UI.slider("Cohesion Radius", cohesion_radius, 0.0, 0.5)) {
                compute_pass.uniformFloat(4, cohesion_radius.val());
            }

            if (UI.slider("Cohesion Factor", cohesion_factor, 0.0, 0.5)) {
                compute_pass.uniformFloat(7, cohesion_factor.val());
            }

            if (UI.slider("Separation Radius", separation_radius, 0.0, 0.5)) {
                compute_pass.uniformFloat(5, separation_radius.val());
            }

            if (UI.slider("Separation Factor", separation_factor, 0.0, 0.5)) {
                compute_pass.uniformFloat(8, separation_factor.val());
            }

            if (UI.slider("Alignment Radius", alignment_radius, 0.0, 0.5)) {
                compute_pass.uniformFloat(6, alignment_radius.val());
            }

            if (UI.slider("Alignment Factor", alignment_factor, 0.0, 0.5)) {
                compute_pass.uniformFloat(9, alignment_factor.val());
            }

            UI.slider("Volume", volume, 0.0, 1.0);

            if (UI.slider("Volume Factor", volume_factor, 0.0, 1.0)) {
                compute_pass.uniformFloat(13, volume_factor.val());
            }
        }
        UI.end();
    }
} spork ~ ui();


PlaneGeometry plane_geo;
FlatMaterial flat_material;
flat_material.color(@(1, 0, 0));
GMesh cursor(plane_geo, flat_material) --> GG.scene();
cursor.sca(.05);
fun void drawCursorAndUpdateVolume()
{
    while (true) {
        GG.nextFrame() => now;
        camera.screenCoordToWorldPos(GWindow.mousePos(), 1.0) => vec3 mouse_cursor;
        mouse_cursor => cursor.pos;

        compute_pass.uniformFloat2(11, @(mouse_cursor.x, mouse_cursor.y));
        Math.pow(p.last(), .2) => volume.val;
        compute_pass.uniformFloat(12, volume.val());
        boids_material.uniformFloat(1, volume.val() * volume_factor.val());
    }
} spork ~ drawCursorAndUpdateVolume();

// main loop
while (true)
{
    GG.nextFrame() => now;

    compute_pass.storageBuffer(0, boids_buffer_a); // read
    compute_pass.storageBuffer(1, boids_buffer_b); // write
    compute_pass.uniformFloat(3, 10 * GG.dt());

    boids_material.storageBuffer(0, boids_buffer_b); 

    GG.nextFrame() => now;

    compute_pass.storageBuffer(0, boids_buffer_b); // read
    compute_pass.storageBuffer(1, boids_buffer_a); // write
    compute_pass.uniformFloat(3, 10 * GG.dt());

    boids_material.storageBuffer(0, boids_buffer_a); 
}
