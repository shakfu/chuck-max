/*-----------------------------------------------------------------------------
name: shadows.ck
desc: how to generate shadows from SpotLights and DirLights. 
Point light shadows to come eventually...

author: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
  date: July 2025
//---------------------------------------------------------------------------*/

// scene setup
GG.scene() @=> GScene scene;
GOrbitCamera cam --> scene;
// select as main camera
scene.camera( cam );
// camera angle
@(0, 10, 10) => cam.pos;

// disable default directional light
scene.light().intensity(0);
true => scene.light().shadow;

// disable default ambient light
scene.ambient(.0 * @(1,1,1));

// place ground
GPlane ground --> GG.scene();
ground.scaX(15).scaY(15);
@(-Math.PI/2, 0, 0) => ground.rot;  // make the plane lay flat

// place other meshes
GKnot knot --> GGen shadow_casters --> GG.scene();
knot.sca(.5).posY(1);
GSuzanne suzanne --> shadow_casters;
suzanne.posY(1).posX(3);
GSphere sphere --> shadow_casters;
sphere.sca(1.5).posY(1).posX(-3);

// spotlights
GSpotLight spot_center --> GG.scene();
spot_center.shadowBias(.001);
spot_center.angleMax(Math.pi/4);
spot_center.posY(3).rotX(-Math.pi/2);
spot_center.color(3 * Color.GREEN);
spot_center.name("Center Spotlight");

GSpotLight spot_right --> GG.scene();
spot_right.angleMax(Math.pi/4);
spot_right.posY(3).posX(2).rotX(-Math.pi/2);
spot_right.color(3 * Color.BLUE);
spot_right.name("Right Spotlight");

GSpotLight spot_left --> GG.scene();
spot_left.angleMax(Math.pi/4);
spot_left.posY(3).posX(-2).rotX(-Math.pi/2);
spot_left.color(3 * Color.RED);
spot_left.name("Left Spotlight");

// mark lights as shadow generators and set bias
spot_center.shadow(true);
spot_center.shadowBias(.001);
spot_right.shadow(true);
spot_right.shadowBias(.001);
spot_left.shadow(true);
spot_left.shadowBias(.001);

// add shadow casters to lights
// this tells ChuGL the mesh will cast shadows from the 
// given light source
spot_center.shadowAdd(shadow_casters, true /* include children? */);
spot_left.shadowAdd(shadow_casters, true /* include children? */);
spot_right.shadowAdd(shadow_casters, true /* include children? */);
scene.light().shadowAdd(shadow_casters, true);

// set shadow receivers
// this tells the shader whether a GGen (and its children) 
// should receive shadows
scene.shadowed(true, true);
// alternately you can set each mesh individually like so:
// knot.shadowed(true);
// floor.shadowed(true);
// etc...

UI_Bool left_spotlight_shadow(spot_left.shadow());
UI_Bool center_spotlight_shadow(spot_center.shadow());
UI_Bool right_spotlight_shadow(spot_right.shadow());

UI_Float dirlight_intensity(scene.light().intensity());
UI_Bool dirlight_shadow(scene.light().shadow());
UI_Float dirlight_shadow_bounds((scene.light() $ GDirLight).shadowBounds().x);

UI_Float spot_bias(spot_center.shadowBias());
UI_Float dir_bias(scene.light().shadowBias());

int do_light_show;
float t;
fun void lightShow() {
    while (1) {
        GG.nextFrame() => now;
        if (do_light_show) {
            GG.dt() +=> t;
            -.5 * Math.sin(t * 1.13) => spot_right.rotZ;
            .5 * Math.sin(t * 1.5) => spot_center.rotZ;
            .5 * Math.sin(t * 1.27) => spot_left.rotZ;
        }
    }
} spork ~ lightShow();

while (1) {
    GG.nextFrame() => now;

    UI.setNextWindowBgAlpha(0.00);
    if (UI.begin("Shadow Example")) {
        UI.separatorText("Spotlights");
        if (UI.checkbox("Left", left_spotlight_shadow))
            left_spotlight_shadow.val() => spot_left.shadow;
        UI.sameLine();
        if (UI.checkbox("Center", center_spotlight_shadow))
            center_spotlight_shadow.val() => spot_center.shadow;
        UI.sameLine();
        if (UI.checkbox("Right", right_spotlight_shadow))
            right_spotlight_shadow.val() => spot_right.shadow;
        if (UI.slider("Shadow Bias", spot_bias, 0, .002, "%.5f", 0)) {
            spot_bias.val() => spot_left.shadowBias;
            spot_bias.val() => spot_right.shadowBias; 
            spot_bias.val() => spot_center.shadowBias;
        }

        do_light_show ? "stop light show :(" : "START LIGHT SHOW" => string button_txt;
        if (UI.button(button_txt)) 1 - do_light_show => do_light_show;
        UI.sameLine();
        if (UI.button("reset")) {
            0 => spot_left.rotZ;
            0 => spot_center.rotZ;
            0 => spot_right.rotZ;
            0 => t;
        }

        UI.separatorText("Directional Light");
        if (UI.slider("Intensity", dirlight_intensity, 0, 1))
            dirlight_intensity.val() => scene.light().intensity;
        if (UI.checkbox("Casts Shadow?", dirlight_shadow))
            dirlight_shadow.val() => scene.light().shadow;
        if (UI.slider("Shadow Bounds", dirlight_shadow_bounds, 10, 30))
            (scene.light() $ GDirLight).shadowBounds(dirlight_shadow_bounds.val(), 500);


    } UI.end();
}