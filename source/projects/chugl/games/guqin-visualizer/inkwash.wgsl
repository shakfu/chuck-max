#include FRAME_UNIFORMS
// will include the following:
// struct FrameUniforms {
//     projection: mat4x4f,                     // 4x4 projection matrix, used to transform from camera space to clip space.
//     view: mat4x4f,                           // 4x4 view matrix, used to transform from world space to camera space.
//     projection_view_inverse_no_translation: mat4x4f, // Inverse of the combined projection and view matrices, but without translation. Useful for certain non-world-space calculations (e.g., ray casting).
//     camera_pos: vec3f,                       // Position of the camera in world space.
//     time: f32,                               // Time elapsed (likely in seconds), useful for animations or time-dependent effects.
//     ambient_light: vec3f,                    // RGB color for the ambient light in the scene.
//     num_lights: i32,                         // Number of active lights in the scene.
//     background_color: vec4f,                 // RGBA color of the background.
// };
// @group(0) @binding(0) var<uniform> u_frame: FrameUniforms;

#include DRAW_UNIFORMS
#include STANDARD_VERTEX_INPUT
#include STANDARD_VERTEX_OUTPUT
// struct VertexOutput {
//     @builtin(position) position : vec4f,     // Built-in variable for the clip-space position of the vertex, used for rasterization.
//     @location(0) v_worldpos : vec3f,         // World-space position of the vertex, passed to the fragment shader for effects like lighting.
//     @location(1) v_normal : vec3f,           // World-space normal vector, passed to the fragment shader for lighting calculations.
//     @location(2) v_uv : vec2f,               // Interpolated texture coordinates, passed to the fragment shader for sampling textures.
// };

#include STANDARD_VERTEX_SHADER

/*
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

fn conway_sim(in : VertexOutput, @builtin(front_facing) is_front: bool) -> @location(0) vec4f
{
    let dim : vec2u = textureDimensions(src);

    let coords = vec2i(in.v_uv * vec2f(dim));
    var cell = vec4f(f32(alive(coords, dim)));

    if (bool(simulate_step)) {
        // any cell part of the audio waveform is alive
        let is_alive = bool(alive(coords, dim));
        let neighbors =
            alive(coords + vec2f(-1, -1), dim)
            +  alive(coords + vec2f(-1,  0), dim)
            +  alive(coords + vec2f(-1,  1), dim)
            +  alive(coords + vec2f( 0, -1), dim)
            +  alive(coords + vec2f( 0,  1), dim)
            +  alive(coords + vec2f( 1, -1), dim)
            +  alive(coords + vec2f( 1,  0), dim)
            +  alive(coords + vec2f( 1,  1), dim);

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
}
*/
//Chimera's Breath
//by nimitz 2018 (twitter: @stormoid)

/*
	The main interest here is the addition of vorticity confinement with the curl stored in
	the alpha channel of the simulation texture (which was not used in the paper)
	this in turns allows for believable simulation of much lower viscosity fluids.
	Without vorticity confinement, the fluids that can be simulated are much more akin to
	thick oil.
	
	Base Simulation based on the 2011 paper: "Simple and fast fluids"
	(Martin Guay, Fabrice Colin, Richard Egli)
	(https://hal.inria.fr/inria-00596050/document)

	The actual simulation only requires one pass, Buffer A, B and C	are just copies 
	of each other to increase the simulation speed (3 simulation passes per frame)
	and Buffer D is drawing colors on the simulated fluid 
	(could be using particles instead in a real scenario)
*/

const dt = 0.15;
//Recommended values between 0.03 and 0.2
//higher values simulate lower viscosity fluids (think billowing smoke)
const VORTICITY_AMOUNT = 0.2;

fn mag2(p: vec2f) -> f32 {
    return dot(p,p);
}

fn point1(t: f32) -> vec2f {
    t *= 0.62;
    return vec2f(0.12, 0.5 + sin(t) * 0.2);
}

fn point2(t: f32) -> vec2f {
    t *= 0.62;
    return vec2f(0.88,0.5 + cos(t + 1.5708)*0.2);
}

fn solveFluid(
    // sampler2D smp, 
    uv: vec2f, w: vec2f, time: f32, mouse: vec3f, lastMouse: vec3f
    // TODO refactor assuming w = 1 texel
) -> vec4f {
	const K = 0.2;
	const v = 0.55;

    // TODO uv needs to be convverted to integer texture coordinates
    var data: vec4f = textureLoad(smp, uv, 0);
    var tr: vec4f = textureLoad(smp, uv + vec2f(w.x , 0), 0);
    var tl: vec4f = textureLoad(smp, uv - vec2f(w.x , 0), 0);
    var tu: vec4f = textureLoad(smp, uv + vec2f(0 , w.y), 0);
    var td: vec4f = textureLoad(smp, uv - vec2f(0 , w.y), 0);
    
    var dx: vec3f = (tr.xyz - tl.xyz)*0.5;
    var dy: vec3f = (tu.xyz - td.xyz)*0.5;
    var densDif: vec2f = vec2f(dx.z ,dy.z);
    
    data.z -= dt*dot(vec3f(densDif, dx.x + dy.y) ,data.xyz); //density
    var laplacian: vec2f = tu.xy + td.xy + tr.xy + tl.xy - 4.0*data.xy;
    var viscForce: vec2f = vec2f(v)*laplacian;
    data.xyw = textureLoad(smp, uv - dt*data.xy*w, 0).xyw; //advection
    
    var newForce: vec2f = vec2f(0);

    // auto force vectors
    newForce.xy += 0.75*vec2f(.0003, 0.00015)/(mag2(uv-point1(time))+0.0001);
    newForce.xy -= 0.75*vec2f(.0003, 0.00015)/(mag2(uv-point2(time))+0.0001);

    // add force at mouse point
    if (mouse.z > 1. && lastMouse.z > 1.)
    {
        var vv: vec2f = clamp(vec2f(mouse.xy*w - lastMouse.xy*w)*400., -6., 6.);
        newForce.xy += .001/(mag2(uv - mouse.xy*w)+0.001)*vv;
    }
    
    // TODO: can velocity decay be played with?
    data.xy += dt*(viscForce.xy - K/dt*densDif + newForce);  // update velocity
    data.xy = max(vec2f(0), abs(data.xy) - 1e-4) * sign(data.xy); // linear velocity decay
    
   	data.w = (tr.y - tl.y - tu.x + td.x);
    var vort: vec2f = vec2f(abs(tu.w) - abs(td.w), abs(tl.w) - abs(tr.w));
    vort *= VORTICITY_AMOUNT/length(vort + 1e-9)*data.w;
    data.xy += vort;
    
    data.y *= smoothstep(.5,.48,abs(uv.y-0.5)); //Boundaries
    
    data = clamp(data, vec4f(vec2f(-10), 0.5, -10.), vec4f(vec2f(10), 3.0, 10.));
    
    return data;
}

@fragment 
fn fs_main(
    in : VertexOutput, @builtin(front_facing) is_front: bool
) -> @location(0) vec4f {
    let dim : vec2u = textureDimensions(src);
    let uv = in.v_uv;
    let w: vec2f = 1.0/dim; // w = delta x, delta y in uv space

    // update velocity field
    // TODO run this 3 times per frame
    vec4 data = solveFluid(
        iChannel0, 
        in.v_uv, w, 
        u_frame.time, 
        vec3f(0.0), vec3f(0.0) // mouse and last_mouse
    );

    { // compute color
        vec2 velo = textureLod(iChannel0, uv, 0.).xy; // channel0 is velocity
        
        // channel1 holds colors from past frame
        vec4 col = textureLod(
            iChannel1, 
            uv - dt * velo * w * 3. // x3 because the shader runs 3 steps per frame?
            , 0.
        ); //advection
    
        vec2 brush_pos = vec2(.12, .5);
        col += (
            .0025 / (.0005 + pow(length(uv - brush_pos),1.75) ) // pick position
            
            * dt   // scale amount of ink by dt
            * 0.12 // scale down color 
            * vec4(1, 1, 1, 1) // pick color
        );
        
        col = clamp(col, 0.,5.);
        col = max(.998 * col - .0001, 0.); //decay
        
        fragColor = col;
    }
    

    return vec4f(in.v_uv, 0.0, 1.0);
}
