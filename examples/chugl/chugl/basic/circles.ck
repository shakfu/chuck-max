//--------------------------------------------------------------------
// name: circles.ck
// desc: "circles, in numbers, are cool" -- Ge
//
// this example demonstrates:
//   1) creating a custom GGen class
//   2) plotting our circle with #math
//   3) many instances: local independence + global organization
//      -- design principle from artful design chapter 3
//--------------------------------------------------------------------

@import "ChuGL";

// window title
GG.windowTitle( "circles in ChuGL" );
// fullscreen
GG.fullscreen();
// position
GG.camera().posZ( 3 );

// how many circles?
512 => int NUM_CIRCLES;
// how many vertices per circle? (more == smoother)
128 => int N;
// normalized radius
1 => float RADIUS;

// creating a custom GGen class for a 2D circle
class Circle extends GGen
{
    // for drawing our circle 
    GLines circle --> this;
    // randomize rate
    Math.random2f(2,3) => float rate;
    // default color
    color( @(.5, 1, .5) );
    // default line width
    circle.width(.01);

    // initialize a circle
    fun void init( int resolution, float radius )
    {
        // incremental angle from 0 to 2pi in N-2 step, plus to steps to close the line loop
        2*pi / (resolution-2) => float theta;    
        // positions of our circle
        vec2 pos[resolution];
        // previous, init to 1 zero
        @(radius) => vec2 prev;
        // loop over vertices
        for( int i; i < pos.size(); i++ )
        {
            // rotate our vector to plot a circle
            // https://en.wikipedia.org/wiki/Rotation_matrix
            Math.cos(theta)*prev.x - Math.sin(theta)*prev.y => pos[i].x;
            Math.sin(theta)*prev.x + Math.cos(theta)*prev.y => pos[i].y;
            // remember v as the new previous
            pos[i] => prev;
        }
        
        // set positions
        circle.positions( pos );
    }
    
    fun void color( vec3 c )
    {
        circle.color( c );
    }
    
    fun void update( float dt )
    {
        .35 + .25*Math.sin(now/second*rate) => float s;
        circle.sca( s );
        // uncomment for xtra weirdness
        // circle.rotateY(dt*rate/3);
    }
}

// an array of our custom circles
Circle circles[NUM_CIRCLES];
// iterate over circles array
for( auto circ : circles )
{
    // initialize each 
    circ.init( N, RADIUS );
    // connect it
    circ --> GG.scene();
    // randomize location in XY
    @( Math.random2f(-1.5,1.5),
       Math.random2f(-1,1),
       Math.random2f(-1,1) ) => circ.pos;
}

// function to cycle colors
fun void cycleColors( float dt )
{
    // iterate over circles array
    for( auto circ : circles )
    {
        // cycle RGB
        @( (1+Math.sin(now/second*.7))/2,
           (1+Math.sin(now/second*.8))/2,
           (1+Math.sin(now/second*.9))/2 ) => circ.color;
    }
}

// time loop
while( true )
{
    GG.nextFrame() => now;
    // cycle colors
    cycleColors( GG.dt() );
}
