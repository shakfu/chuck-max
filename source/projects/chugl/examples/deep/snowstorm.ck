//-----------------------------------------------------------------------------
// name: snowstorm.ck
// desc: example demoing the use of point sprites, transparency, 
//       and UI controls.
// requires: ChuGL + chuck-1.5.2.5 or higher
//
// author: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
// source: inspired and adapted from the following three.js example
//         https://github.com/mrdoob/three.js/blob/master/examples/webgl_points_sprites.html
//   date: Fall 2023
//-----------------------------------------------------------------------------

// setup scene ====================================================
GG.scene() @=> GScene@ scene;
GOrbitCamera cam --> scene;
scene.camera(cam);
cam.clip(1.5, 1000);
cam.posZ(16);

// snow GGens
GGen snowstorm --> scene;
GPoints snowflakes[6];

// setup textures ======================================
Texture@ sprites[6];
for (int i; i < 6; i++) {
    Texture.load(me.dir() + "../data/textures/snowflake" + (i+1) + ".png") @=> sprites[i];
}

// default simulation params
@(.741, .894, 1.0) => vec3 DEFAULT_COLOR;
10000 => int NUM_SNOWFLAKES;
14 => int SNOWFLAKE_SPREAD;

// initialize snowflake positions
vec3 snowflakePos[NUM_SNOWFLAKES];
for (0 => int i; i < snowflakePos.size(); i++) {
    @(
        Math.random2f(-SNOWFLAKE_SPREAD, SNOWFLAKE_SPREAD),  // x
        Math.random2f(-SNOWFLAKE_SPREAD, SNOWFLAKE_SPREAD),  // y
        Math.random2f(-SNOWFLAKE_SPREAD, SNOWFLAKE_SPREAD)   // z
    ) => snowflakePos[i];
}

// initialize point GGens
for (int i; i < snowflakes.size(); i++) {
    // set point positions and material properties
    snowflakes[i].texture(sprites[i]);
    snowflakes[i].color(DEFAULT_COLOR);
    snowflakes[i].positions(snowflakePos);

    // attach to parent snowstorm ggen
    snowflakes[i] --> snowstorm;

    // randomize rotation
    .15 => float s;
    Math.random2f(0, s*Math.TWO_PI) => snowflakes[i].rotX;
    Math.random2f(0, s*Math.TWO_PI) => snowflakes[i].rotY;
    Math.random2f(0, s*Math.TWO_PI) => snowflakes[i].rotZ;
}

// Game loop ============================================
UI_Float rotationRate(0.05);
UI_Float pointSize(snowflakes[0].size());
UI_Float3 color(DEFAULT_COLOR);

// render loop
while (true) {
    GG.nextFrame() => now;

    // snowflake update
    for (int i; i < snowflakes.size(); i++) {
        Math.map(i, 0, snowflakes.size(), 1, 2.5) * rotationRate.val() => float snowflakeRotRate;
        snowflakeRotRate * GG.dt() => snowflakes[i].rotateY;
    }

    // UI
    if (UI.begin("Snow Simulation Params")) {

        UI.slider("Wind Speed", rotationRate, 0.0, 1.0);

        if (UI.colorEdit("Snowflake Color", color, 0)) {
            color.val() => DEFAULT_COLOR;
            for (int i; i < snowflakes.size(); i++) {
                color.val() => snowflakes[i].color;
            }
        }

        if (UI.slider("Snowflake Size", pointSize, 0.01, 0.15)) {
            for (int i; i < snowflakes.size(); i++) {
                pointSize.val() => snowflakes[i].size;
            }
        }
    }
    UI.end();
}
