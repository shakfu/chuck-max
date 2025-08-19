//--------------------------------------------------------------------
// name: skybox.ck
// desc: skybox and environment mapping
// 
// To run, download the bridge cubemap from here:
// https://ccrma.stanford.edu/~azaday/assets/bridge.zip
// and place the unzipped "bridge" folder in the same directory as
// this program
// 
// author: Andrew Zhu Aday
//   date: Fall 2024
//--------------------------------------------------------------------

// load the cubemap
Texture.load(
    me.dir() + "./bridge/posx.jpg", // right
    me.dir() + "./bridge/negx.jpg", // left
    me.dir() + "./bridge/posy.jpg", // top
    me.dir() + "./bridge/negy.jpg", // bottom
    me.dir() + "./bridge/posz.jpg", // back
    me.dir() + "./bridge/negz.jpg"  // front
) @=> Texture cubemap;

// apply the cubemap to the scene
GG.scene().envMap(cubemap);
GG.scene().backgroundColor(Color.WHITE); // IMPORTANT!!! Final color skybox *times* background color

// use orbit camera
GOrbitCamera camera --> GG.scene();
GG.scene().camera(camera);

// connect suzanne to scene
GSuzanne suzanne --> GG.scene();
// enable environment mapping on suzanne
suzanne.envmapBlend(PhongMaterial.EnvmapBlend_Multiply);

[
    "None",
    "Reflect",
    "Refract",
] @=> string envmap_methods[];

[
    "None",
    "Add",
    "Multiply",
    "Mix",
] @=> string envmap_blend_modes[];

UI_Float3 background_color(GG.scene().backgroundColor());
UI_Float envmap_intensity(suzanne.envmapIntensity());
UI_Float refraction_ratio(suzanne.refractionRatio());
UI_Int envmap_method_index(suzanne.envmapMethod());
UI_Int envmap_blend_mode_index(suzanne.envmapBlend());

while (true) {
    GG.nextFrame() => now;

    if (UI.begin("Skybox")) {
        if (UI.colorEdit("Background Color", background_color, 0)) {
            GG.scene().backgroundColor(background_color.val());
        }

        if (UI.slider("Envmap Intensity", envmap_intensity, 0.0, 1.0)) {
            suzanne.envmapIntensity(envmap_intensity.val());
        }

        if (UI.slider("Refraction Ratio", refraction_ratio, 0.0, 1.0)) {
            suzanne.refractionRatio(refraction_ratio.val());
        }

        if (UI.listBox("Envmap Method", envmap_method_index, envmap_methods)) {
            suzanne.envmapMethod(envmap_method_index.val());
        }

        if (UI.listBox("Envmap Blend Mode", envmap_blend_mode_index, envmap_blend_modes)) {
            suzanne.envmapBlend(envmap_blend_mode_index.val());
        }
    }
    UI.end();
}
