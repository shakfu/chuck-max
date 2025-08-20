//-----------------------------------------------------------------------------
// name: bloom.ck
// desc: built-in post-processing BLOOM effect!
// 
// author: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//   date: Fall 2024
//-----------------------------------------------------------------------------
PlaneGeometry plane_geo;
SphereGeometry sphere_geo;
FlatMaterial mat1, mat2, mat3;

2 => float intensity;
mat1.color( @(intensity, 0, 0));
mat2.color( @(0, 0, intensity));
mat3.color( @(0, intensity, 0));

GMesh plane_l(plane_geo, mat1) --> GG.scene();
GMesh plane_r(plane_geo, mat2) --> GG.scene();
GMesh sphere(sphere_geo, mat3) --> GG.scene();

@(-1.5, 0, 0) => plane_l.translate;
@(0, 0, 0) => sphere.translate;
@(1.5, 0, 0) => plane_r.translate;

// render graph
GG.outputPass() @=> OutputPass output_pass;
GG.renderPass() --> BloomPass bloom_pass --> output_pass;
bloom_pass.input(GG.renderPass().colorOutput());
output_pass.input(bloom_pass.colorOutput());

// uncomment to bypass bloom
// GG.renderPass() --> output_pass;

UI_Float bloom_intensity(bloom_pass.intensity());
UI_Float radius(bloom_pass.radius());

[ 
    "NONE",
    "LINEAR",
    "REINHARD",
    "CINEON",
    "ACES",
    "UNCHARTED",
] @=> string builtin_tonemap_algorithms[];

UI_Int tonemap(output_pass.tonemap());
UI_Int levels(bloom_pass.levels());
UI_Float exposure(output_pass.exposure());
UI_Float color_intensity(intensity);
UI_Float threshold(bloom_pass.threshold());

while (true) {
    GG.nextFrame() => now;

    UI.setNextWindowSize(@(400, 600), UI_Cond.Once);
    if (UI.begin("Bloom Example", null, 0)) {

        if (UI.slider("Color Intensity", color_intensity, 0.0, 5.0)) {
            mat1.color( @(color_intensity.val(), 0, 0));
            mat2.color( @(0, 0, color_intensity.val()));
            mat3.color( @(0, color_intensity.val(), 0));
        }

        if (UI.slider("Threshold", threshold, 0.0, 4.0)) {
            bloom_pass.threshold(threshold.val());
        }

        if (UI.slider("intensity", bloom_intensity, 0.0, 1.0)) {
            bloom_pass.intensity(bloom_intensity.val());
        }

        if (UI.slider("Radius", radius, 0.0, 1.0)) {
            bloom_pass.radius(radius.val());
        }

        if (UI.slider("Levels", levels, 0, 10)) {
            bloom_pass.levels(levels.val());
        }

        UI.separator();

        if (UI.listBox("Tonemap Function", tonemap, builtin_tonemap_algorithms, -1)) {
            output_pass.tonemap(tonemap.val());
        }

        if (UI.slider("exposure", exposure , 0, 4)) {
            output_pass.exposure(exposure.val());
        }
    }
    UI.end();
}
