/*
Problems:
- setting scale to -1 inverts the lighting
    - adding inv transpose normal matrix might fix this

*/


// scene setup
GG.scene() @=> GScene scene;
GOrbitCamera cam --> scene;
// select as main camera
scene.camera( cam );
// camera angle
@(0, 10, 10) => cam.pos;

// disable default directional light
scene.light().intensity(0);

// disable default ambient light
scene.ambient(.02 * @(1,1,1));

// ground
GPlane ground --> GG.scene();
@(10, 10, 1) => ground.sca;
@(-Math.PI/2, 0, 0) => ground.rot;  // make the plane lay flat

// GSuzanne suz --> GG.scene();
// suz.posY(1);

// position some boxes
// GCube cube1 --> GG.scene();
// Color.GOLD => cube1.color;
// cube1.scaY(3);
// cube1.posX(-2).posY(1.5).posZ(2);

// GCube cube2 --> GG.scene();
// Color.GOLD => cube2.color;
// cube2.scaY(3);
// cube2.posX(2).posY(1.5).posZ(2);

// slights
GSpotLight spot --> GG.scene();
spot.posY(3);
spot.lookAt(@(0,0,0), @(0, 0, -1));
spot.color(3 * Color.RED);
spot.name("Spotlight 1");

GSpotLight spot2 --> GG.scene();
spot2.posY(3).posX(3);
spot2.lookAt(@(0,0,0), @(0, 0, -1));
spot2.color(3 * Color.GREEN);

GSuzanne dir_light_marker --> GMesh container(new CubeGeometry, new WireframeMaterial) --> GG.scene();

GKnot knot --> GG.scene();
knot.sca(.5);
knot.posY(1).posZ(1);

dir_light_marker.sca(.2);
container.posY(1);

// shadows
// true => spot.shadow;
spot.shadowAdd(dir_light_marker, true);
spot.shadowAdd(knot, false);
// true => spot2.shadow;
spot2.shadowAdd(knot, true);
spot2.shadowAdd(dir_light_marker, false);

scene.light().shadowAdd(dir_light_marker, true);

UI_Float3 spot_color(spot.color());
UI_Float dirlight_rot(scene.light().rotX());

UI_Float spot_rot(spot.rotZ());
UI_Float spot_angle_min(spot.angleMin());
UI_Float spot_angle_max(spot.angleMax());
UI_Float spot_falloff(spot.falloff());
UI_Float spot_angular_falloff(spot.angularFalloff());
UI_Float spot_range(spot.range());

UI_Bool spot1_casts_shadow(spot.shadow());
UI_Bool spot2_casts_shadow(spot2.shadow());
UI_Bool dirlight_casts_shadow(scene.light().shadow());

UI_Bool receives_shadow(false);

UI_Float spot1_bias(spot.shadowBias());
UI_Float dirlight_bias(scene.light().shadowBias());
[
    (scene.light() $ GDirLight).shadowBounds().x,
    (scene.light() $ GDirLight).shadowBounds().z
] @=> float dirlight_shadow_size[];

while (1) {
    GG.nextFrame() => now;

    UI.setNextWindowBgAlpha(0.00);
    if (UI.begin("Light and Shadow Example")) {

        if (UI.sliderAngle("direction light angle", dirlight_rot)) dirlight_rot.val() => scene.light().rotX;
        if (UI.sliderAngle("spotlight angle", spot_rot, -90, 90)) spot_rot.val() => spot.rotZ;
        if (UI.sliderAngle("spotlight angle min", spot_angle_min, 0, 90)) spot_angle_min.val() => spot.angleMin;
        if (UI.sliderAngle("spotlight angle max", spot_angle_max, 0, 90)) spot_angle_max.val() => spot.angleMax;
        if (UI.slider("spotlight falloff", spot_falloff, 0, 3)) spot_falloff.val() => spot.falloff;
        if (UI.slider("spotlight angular falloff", spot_angular_falloff, 0, 3)) spot_angular_falloff.val() => spot.angularFalloff;
        if (UI.slider("spotlight range", spot_range, 0, 10)) spot_range.val() => spot.range;
        if (UI.colorEdit("spotlight color", spot_color)) spot_color.val() => spot.color;

        if (UI.checkbox("SpotLight1 Generates Shadow", spot1_casts_shadow)) spot1_casts_shadow.val() => spot.shadow;
        if (UI.checkbox("SpotLight2 Generates Shadow", spot2_casts_shadow)) spot2_casts_shadow.val() => spot2.shadow;
        if (UI.checkbox("DirLight Generates Shadow", dirlight_casts_shadow)) dirlight_casts_shadow.val() => scene.light().shadow;
        if (UI.checkbox("Receive Shadows", receives_shadow)) GG.scene().shadowed(receives_shadow.val(), true);

        // if (UI.checkbox("Receive Shadows", receives_shadow)) ground.shadowed(receives_shadow.val());
        if (UI.slider("spotlight bias", spot1_bias, 0, .001, "%.5f", 0)) spot1_bias.val() => spot.shadowBias;
        if (UI.slider("dirlight bias", dirlight_bias, 0, .001, "%.5f", 0)) dirlight_bias.val() => scene.light().shadowBias;
        if (UI.slider("dirlight size", dirlight_shadow_size, 1, 100)) {
                (scene.light() $ GDirLight).shadowBounds(dirlight_shadow_size[0], dirlight_shadow_size[1]);
        }

        if (UI.button("shadowRemove(scene)")) spot.shadowRemove(GG.scene(), true);

        UI.scenegraph(scene);


    } UI.end();

    // dir_light_marker.pos(scene.light().pos());
    // dir_light_marker.rot(scene.light().rot());

    // dir_light_marker.pos(spot.posWorld());
    // container.posY(2 + Math.cos(now/second));
    // container.rotateX(GG.dt());
    dir_light_marker.lookAt(@(0,0,0), @(0, 0, -1));
    // dir_light_marker.rot(spot.rot());
}