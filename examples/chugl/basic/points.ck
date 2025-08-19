//--------------------------------------------------------------------
// name: points.ck
// desc: 1,000,000 points with color; use WASD to move through
// 
// author: Andrew Zhu Aday
//  date: Fall 2023
//--------------------------------------------------------------------

// Scene setup ===================
GG.scene().backgroundColor(@(0,0,0));
GPoints points --> GG.scene();

// choose mouse mode
GWindow.mouseMode( GWindow.MouseMode_Disabled );
// set up camera
GFlyCamera cam --> GG.scene();
// select camera
GG.scene().camera(cam);
// set camera position
cam.posZ(10);

// points stress test
100 => int POINTS_PER_AXIS;

// prepare vertex data for 1,000,000 points!
POINTS_PER_AXIS * POINTS_PER_AXIS * POINTS_PER_AXIS => int numPoints;
vec3 pointPos[numPoints];
vec3 pointColor[numPoints];

// populate within a 10x10x10 cube
for (int i; i < POINTS_PER_AXIS; i++) {
for (int j; j < POINTS_PER_AXIS; j++) {
for (int k; k < POINTS_PER_AXIS; k++) {
    // get the index of this vertex
    i * POINTS_PER_AXIS * POINTS_PER_AXIS + j * POINTS_PER_AXIS + k => int index;

    1.0 * POINTS_PER_AXIS => float colorScale;
    POINTS_PER_AXIS / 10.0 => float posScale;
    // caculate position
    i / colorScale => float x;
    j / colorScale => float y;
    k / colorScale => float z;

    // set position of this vertex
    @(
        posScale * x - posScale / 2.0, 
        posScale * y - posScale / 2.0, 
        posScale * z - posScale / 2.0
    ) => pointPos[index];
    
    // set color of this vertex
    // (same as position, so have 1:1 mapping between xyz physical space and rgb color space) 
    @(
        x, 
        y, 
        z
    ) => pointColor[index];
}}}

points.positions(pointPos);  // set vertex position data
points.colors(pointColor);   // set vertex color data

// Game loop =====================
while (true) { 
    GG.nextFrame() => now; 

    // UI
    if (UI.begin("Points")) {
        UI.text("WASD to move");
    }
    UI.end();
}
