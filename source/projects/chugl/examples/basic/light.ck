//--------------------------------------------------------------------
// name: light.ck
// desc: lights (no limit on max # of lights)
// requires: ChuGL + chuck-1.5.3.0 or higher
// 
// author: Andrew Zhu Aday
//   date: Fall 2024
//--------------------------------------------------------------------

// window title
GG.windowTitle( "ChuGL light demo" );

// lightbulb class that renders pointlight with a colored sphere to visualize its position
class LightBulb extends GGen
{
    // GGen network. a light + sphere at the same position
    FlatMaterial mat;
    GPointLight light --> GSphere bulb --> this;

    // set up sphere to be a flat color
    bulb.mat(mat);
    mat.color(@(1, 1, 1));
    @(0.1, 0.1, 0.1) => bulb.sca;

    // set light radius (how far the light reaches)
    light.radius(2);

    vec3 lightCol;
    Math.random2f(0.5, 1.5) => float pulseRate;  // randomize pulse rate for fading in/out

    fun void color(float r, float g, float b) {
        @(r, g, b) => lightCol;  // save the set color
        mat.color(@(r, g, b));   // set material color
        light.color(@(r, g, b));  // set light diffuse color
    }

    // this is called automatically every frame but ChuGL
    // IF the GGen or one of its parents is connected to GG.scene()
    fun void update(float dt) {
        // fluctuate intensity
        0.5 + 0.5 * Math.sin((now/second) * pulseRate) => light.intensity;  // range [0, 1]
        // fluctuate material color
        light.intensity() * lightCol => mat.color;
    }
}

// scene setup
GG.scene() @=> GScene @ scene;
// connect camera to scenegraph
GOrbitCamera cam --> scene;
// select as main camera
scene.camera( cam );

// disable default directional light
scene.light().intensity(0);
// disable default ambient light
scene.ambient(@(0,0,0));

// camera angle
@(0, 10, 10) => cam.pos;

// ground for lights to cast on
GPlane ground --> GG.scene();
@(10, 10, 1) => ground.sca;
@(-Math.PI/2, 0, 0) => ground.rot;  // make the plane lay flat

// instantiate lightbulbs
GGen lightGroup --> scene;
LightBulb redLight--> lightGroup;
LightBulb greenLight--> lightGroup;
LightBulb blueLight--> lightGroup;
LightBulb whiteLight--> lightGroup;
1 => lightGroup.posY; // lift all lights 1 unit off the ground

// set light colors
2 => redLight.posX;
redLight.color(1, 0, 0);
2 => greenLight.posZ;
greenLight.color(0, 1, 0);
-2 => blueLight.posX;
blueLight.color(0, 0, 1);
-2 => whiteLight.posZ;
whiteLight.color(1, 1, 1);

// Gameloop ==================================
UI_Float light_radius(2);
UI_Float light_falloff(2);

while (true)
{
    GG.nextFrame() => now;
    // rotate lights
    GG.dt() => lightGroup.rotateY;

    if (UI.begin("Point light demo")) {
        if (UI.slider("Light radius", light_radius, 0, 10)) {
            redLight.light.radius(light_radius.val());
            greenLight.light.radius(light_radius.val());
            blueLight.light.radius(light_radius.val());
            whiteLight.light.radius(light_radius.val());
        }

        if (UI.slider("Light falloff", light_falloff, 0, 10)) {
            redLight.light.falloff(light_falloff.val());
            greenLight.light.falloff(light_falloff.val());
            blueLight.light.falloff(light_falloff.val());
            whiteLight.light.falloff(light_falloff.val());
        }
    }
    UI.end();
}
