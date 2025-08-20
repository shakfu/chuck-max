@import "../lib/g2d/ChuGL_debug.chug"

// inspo: https://www.shadertoy.com/view/4tGfDW

// TODO: release chugl should also crash / error on shader compilation failure
// TODO switch to screen pass shader

/*
Cool Parameters
- linear <--> nearest filtering (on nearest filtering looks like glitchy manhattan)
- the decay rate of the color texture
- ink
    - color
    - spawn position
    - intensity
- viscocity
- background color aka fade color (go from fading to white to fading to black)

Liu Shui
- in the harmonics at end of 72 gun fu, go from white background to black,
    - the ink on paper become stars in night sky (celestial)
- motion track hands and/or fingers during 72 gun fu, use these nodes 
as velocity sources for the fluid sim (so it looks like pools/vortexes moving across screen)
    - maybe also lerp color to blue


*/

GCamera camera --> GG.scene();
camera.orthographic();
camera.viewSize(2);
camera.posZ(1.0);
camera => GG.scene().camera;

// ------- graphics stuff -----------
GG.outputPass().tonemap(OutputPass.ToneMap_None);

Material material;
PlaneGeometry plane_geo;

ShaderDesc shader_desc;
me.dir() + "./inkwash.wgsl" => shader_desc.vertexPath;
me.dir() + "./inkwash.wgsl" => shader_desc.fragmentPath;

Shader custom_shader(shader_desc); // create shader from shader_desc
custom_shader => material.shader; // connect shader to material

GMesh mesh(plane_geo, material) --> GG.scene();

TextureDesc conway_tex_desc;
256 => int WINDOW_SIZE;
WINDOW_SIZE => conway_tex_desc.width;
WINDOW_SIZE => conway_tex_desc.height;

Texture conway_tex_a(conway_tex_desc);
Texture conway_tex_b(conway_tex_desc);

float texture_data[4 * WINDOW_SIZE * WINDOW_SIZE];
// TODO need a better way to specify texture size
conway_tex_b.write(texture_data); // initialize empty texture
conway_tex_a.write(texture_data);

fun void simulate() 
{
    true => int flip;
    material.uniformInt(2, 0);
    material.texture(0, conway_tex_a);
    material.storageTexture(1, conway_tex_b);

    2::second => now;

    while (true) {
        // only step once per second
        now + .1::second => time later; 
        while (now < later) GG.nextFrame() => now;

        // enable for a single frame
        material.uniformInt(2, 1);
        GG.nextFrame() => now;

        // disable again
        material.uniformInt(2, 0);
        // flip textures
        if (flip) {
            material.texture(0, conway_tex_b);
            material.storageTexture(1, conway_tex_a);
        } else {
            material.texture(0, conway_tex_a);
            material.storageTexture(1, conway_tex_b);
        }
        1 - flip => flip;
    }
}
// spork ~ simulate();

while (true) {
    // write new audio data to shader
    // material.storageBuffer(3, [1.0]);

    GG.nextFrame() => now;
}
