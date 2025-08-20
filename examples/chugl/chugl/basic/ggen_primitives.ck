//-----------------------------------------------------------------------------
// name: ggen-primitives.ck
// desc: drawing a few primitives with basic GGens
// requires: ChuGL + chuck-1.5.1.5 or higher
//
// author: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//         Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Fall 2024
//-----------------------------------------------------------------------------

// uncomment to run in fullscreen mode
GG.fullscreen();

// empty group to hold all our primitives
GGen group --> GG.scene();

// connect different GGens to the scene root
// 3D primitives
GCube cube --> group;
GSphere sphere --> group;
GTorus torus --> group; torus.sca(.7);
GCylinder cylinder --> group;
GKnot knot --> group; knot.sca(.5);
GSuzanne suzanne --> group; suzanne.sca(.5);

// 2D primitives
GPlane plane --> group;
GCircle circle --> group;

// put into an array of GMesh (super class)
[ cube,
  sphere,
  circle,
  plane,
  torus,
  cylinder,
  knot,
  suzanne,
] @=> GMesh ggens[];

0 => int pos;
// loop over our array
for( GMesh obj : ggens )
{
    // set position
    -ggens.size() + 2 * pos++ => obj.posX;
    
    // randomize color
    Math.random2f(0.0, 1.0) => float r;
    Math.random2f(0.0, 1.0) => float g;
    Math.random2f(0.0, 1.0) => float b;
    // set color on the material for each GGen
    @(r, g, b) => (obj.mat() $ PhongMaterial).color;
}

// position
GG.camera().posZ( 10 );

// our local update
fun void update()
{
    // raise the cube to a constant height
    2 => cube.posY;
    // oscillate as a function of time
    Math.sin(now/second) => cube.posX;
}

// infinite time loop
while( true )
{
    // IMPORTANT: sync with next graphics frame
    GG.nextFrame() => now;
    // manually call local update
    update();
}