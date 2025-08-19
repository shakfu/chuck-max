//-----------------------------------------------------------------------------
// name: fish.ck
// desc: Procedural animation of a fish using Catmull-Rom splines, dynamic
// polygon triangulation, and trail renderers.
//
// Inspired by this awesome video:
// https://www.youtube.com/watch?v=qlfh_rv6khY&ab_channel=argonaut
//
// authors: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//
// date: Fall 2024
//-----------------------------------------------------------------------------

// Camera and Mouse settings
GG.camera().orthographic();
GWindow.mouseMode(GWindow.MouseMode_Disabled);

// Using GLine as a trail renderer
class TrailRenderer extends GGen
{
    GLines trail --> this;
    trail.width(.01);
    vec2 trail_positions[0]; // position history in *reverse* older. position 0 is oldest. position size() - 1 is newest
    vec3 trail_colors[0];

    false => int initialized;

    fun void init(int history, vec3 gradient_start_color, vec3 gradient_end_color) {
        trail_positions.size(history);
        trail_colors.size(history);

        // set gradient to black
        for (int i; i < trail_colors.size(); i++) {
            (i $ float) / trail_colors.size() => float t;
            t * gradient_start_color + (1 - t) * gradient_end_color => trail_colors[i];
        }
        trail.colors(trail_colors);
    }

    fun void follow(vec2 world_pos) {
        if (!initialized) {
            true => initialized;
            for (int i; i < trail_positions.size(); i++)
                world_pos => trail_positions[i];
        }

        world_pos - trail_positions[trail_positions.size() - 1] => vec3 delta;
        // only update trail renderer if moved
        if (delta.magnitude() > .001) {
            trail_positions.popFront();
            trail_positions << world_pos;
            trail.positions(trail_positions);
            // zeno interpolator: fade in width
            trail.width() + (.005 * (.01 - trail.width())) => trail.width;
        } 
    }
}

// Catmull-Rom Spline (assumes `control_points` forms a loop)
fun void catmullRomSpline(vec2 control_points[], int subdivisions_per_segment, vec2 line_positions[])
{
    line_positions.clear();

    for (int segment; segment < control_points.size(); segment++) {
        for (int i; i < subdivisions_per_segment; i++) {
            segment => int p1;
            (p1 + 1) % control_points.size() => int p2;
            (p2 + 1) % control_points.size() => int p3;
            p1 - 1 => int p0; if (p0 < 0) control_points.size() +=> p0;

            i $ float / subdivisions_per_segment => float t;
            t * t => float tt;
            tt * t => float ttt;

            -ttt + 2*tt - t => float q0;
            3*ttt - 5*tt + 2 => float q1;
            -3*ttt + 4*tt + t => float q2;
            ttt - tt => float q3;

            line_positions << .5 * (
                control_points[p0] * q0 +
                control_points[p1] * q1 +
                control_points[p2] * q2 +
                control_points[p3] * q3
            );
        }
    }

    // add start positions to complete the loop
    line_positions << line_positions[0];
    line_positions << line_positions[1];
}

// helper math functions ========================================

fun vec2 normalize(vec2 n)
{
    return n / Math.hypot(n.x, n.y); // hypot is the magnitude
}

fun void constrainDistance(GGen point, GGen anchor, float dist)
{
    point.pos() $ vec2 => vec2 p;
    anchor.pos() $ vec2 => vec2 a;

    Math.euclidean(a, p) => float curr_dist;

    a + dist * normalize(p - a) => p;
    p => point.pos;
}

fun float dot(vec2 a, vec2 b) {
    return a.x*b.x + a.y*b.y;
}

// returns vector given by rotating a by theta radians ccw
fun vec2 rot(vec2 a, float theta) {
    Math.cos(theta) => float cos_theta;
    Math.sin(theta) => float sin_theta;
    return a.x * @(cos_theta, sin_theta) + a.y * @(-sin_theta, cos_theta);
}

fun float deg2rad(float deg) {
    return deg * Math.pi / 180;
}

fun float rad2deg(float rad) {
    return rad * 180 / Math.pi;
}

fun float angle(vec2 a, vec2 b) {
    @(-a.y, a.x) => vec2 a_perp;
    (2 * (dot(a_perp, b) > 0)) - 1 => int ccw_side; // +1 if ccw, else -1
    return ccw_side * Math.acos(dot( normalize(a), normalize(b)));
}

// end helper math functions ========================================

// fish body outline
GLines fish_body_outline --> GG.scene();
fish_body_outline.width(.02);

// fish body segment diameters
[
    (.15),
    (.24),
    (.225),
    (.21),
    (.19),
    (.164),
    (.129),
    (.093),
    (.039),
] @=> float diameters[];

// the vertebrae of the fish
vec2 spine[10];
// initialize spine vertebrae positions
// the last spine element is a "sentinal" value to make loops simpler
for (int i; i < spine.size() - 1; ++i) {
    @(i, 0) => spine[i];
}
spine.size() - 1 => int num_vertebrae;

// positions at the sides of each vertebrae , used form body outline
vec2 sides[2 * (spine.size())];

// final outline, the result of turning `sides` array into a catmull-rom spline
vec2 outline[2 * num_vertebrae + 2]; // +2 for extra control point at tip of nose and end of tail

// fish body GGens
PolygonGeometry poly_geo;
FlatMaterial poly_mat;
GMesh poly(poly_geo, poly_mat)  --> GG.scene();
poly_mat.color(Color.RED);

// Fish fins
FlatMaterial fin_material;
CircleGeometry fin_geo;

GMesh fin_right(fin_geo, fin_material) --> GG.scene();
fin_right.scaX(.5); fin_right.scaY(.2);
fin_right.posZ(poly.posZ() - 1);

GMesh fin_left(fin_geo, fin_material) --> GG.scene();
fin_left.scaX(.5); fin_left.scaY(.2);
fin_left.posZ(poly.posZ() - 1);

GMesh fin_left_small(fin_geo, fin_material) --> GG.scene();
fin_left_small.scaX(.25); 
fin_left_small.scaY(.1);
fin_left_small.posZ(poly.posZ() - 1);

GMesh fin_right_small(fin_geo, fin_material) --> GG.scene();
fin_right_small.scaX(.25); 
fin_right_small.scaY(.1);
fin_right_small.posZ(poly.posZ() - 1);

GMesh tail(fin_geo, fin_material) --> GG.scene();
tail.posZ(poly.posZ() - 1);
tail.sca(.8);

.24 => float DIST_LIMIT; // the maximum allowed distance between vertebrae
2.6 => float ANGLE_LIMIT; // the maximum allowed angle between vertebrae
true => int follow_mouse;
vec2 line_positions[0];

// arrays to help position trail renderers along dorsal fin
vec2 dorsal_outline[4];
vec2 dorsal_spline_positions[0];

// whispy strands of dorsal fin
TrailRenderer dorsal_trails[3 * 8];
for (int i; i < dorsal_trails.size(); i++) {
    dorsal_trails[i] --> GG.scene();
    dorsal_trails[i].posZ(fish_body_outline.posZ() + 1);

    // randomize color between white and black for some strands
    if(Math.randomf() < .4) {
        dorsal_trails[i].init(Math.random2(12, 20), Color.WHITE, Color.BLACK);
    } else {
        dorsal_trails[i].init(Math.random2(12, 20), Color.WHITE, Color.WHITE);
    }
}


while (true) {
    GG.nextFrame() => now;

    GG.camera().screenCoordToWorldPos(GWindow.mousePos(), 1.0) $ vec2 => vec2 mouse_pos;

    mouse_pos => spine[0];

    0 => float curvature;
    for (1 => int i; i < spine.size(); i++) {
        // contrain distance (https://zalo.github.io/blog/constraints/)
        spine[i-1] => vec2 p1;
        spine[i] => vec2 p2;
        normalize(p2 - p1) => vec2 q_dir; // direction vector from anchor to point
        p1 + DIST_LIMIT * q_dir => spine[i];

        // constrain angle
        if (i > 1) { 
            spine[i-2] => vec2 p0;

            p0 - p1 => vec2 p;
            normalize(p) => vec2 p_dir;

            // figure out which side q is pointing
            @(-p_dir.y, p_dir.x) => vec2 p_dir_perp;
            (2 * (dot(p_dir_perp, q_dir) > 0)) - 1 => int ccw_side; // +1 if ccw, else -1
            Math.acos(dot(p_dir, q_dir)) => float angle_rad;

            Math.pi - Math.max(angle_rad, ANGLE_LIMIT) +=> curvature;

            // clamp angle
            if (angle_rad < ANGLE_LIMIT) {
                p1 + rot(p, ANGLE_LIMIT * ccw_side) => spine[i];
            }
        }

        { // set outline and debug marker positions
            // get sides of circle from direction of spine segment
            Math.atan2(q_dir.y, q_dir.x) => float theta;
            diameters[i-1] => float diameter;
            p1 + diameter * @(Math.cos(theta + Math.pi / 2), Math.sin(theta + Math.pi / 2)) => vec2 cw_side;
            p1 + diameter * @(Math.cos(theta - Math.pi / 2), Math.sin(theta - Math.pi / 2)) => vec2 ccw_side;
            cw_side => sides[(i-1)*2];
            cw_side => outline[i];
            ccw_side => sides[(i-1)*2 + 1];
            ccw_side => outline[outline.size() - i];

            // tip of nose control point
            if (i == 1) {
                p1 - diameter * @(Math.cos(theta), Math.sin(theta)) => outline[0];
            }

            // tip of tail control point
            if (i == spine.size() - 1) {
                p1 + diameter * @(Math.cos(theta), Math.sin(theta)) => outline[outline.size() / 2];
            }
        }
    }

    catmullRomSpline(outline, 16, line_positions);
    fish_body_outline.positions(line_positions);

    poly_geo.build(line_positions);

    // normalize curvature by # of segments
    curvature / (num_vertebrae - 1) => curvature;
    // take curvature as a percentage of angle constraint
    curvature / (Math.pi - ANGLE_LIMIT)  => curvature;

    // position fins
    {   
        // TODO wtf is this rotation bug... have to set rotZ on right fin and rotateZ on left fins

        // get angle of segment 2-3
        (-angle(outline[2] - outline[3], @(0, 1)) - .8) => fin_right.rotZ;
        // offset by angle between 2-3 and 3-4
        fin_right.pos(outline[3]);
        
        // position left fin
        0 => fin_left.rotZ;
        (angle(outline[outline.size()-2] - outline[outline.size()-3], @(0, 1)) - .8) => fin_left.rotateZ;
        fin_left.pos(outline[outline.size() - 3]);

        // position right small fin
        (-angle(outline[5] - outline[6], @(0, 1)) - .8) => fin_right_small.rotZ;
        fin_right_small.pos(outline[6]);

        // position left small fin
        0 => fin_left_small.rotZ;
        (angle(outline[outline.size()-5] - outline[outline.size()-6], @(0, 1)) - .8) => fin_left_small.rotateZ;
        fin_left_small.pos(outline[outline.size() - 6]);

        // position tail at end of outline
        tail.pos(outline[outline.size() / 2]);
        tail.scaX(.05 + .15 * curvature);
        // match tail rotation to rotation of vertebrae
        -angle(spine[spine.size() - 2] - spine[spine.size() - 3], @(0, 1)) => tail.rotZ;
    }

    { // build dorsal fin from spline on vertebrae
        spine[2] => dorsal_outline[0];
        spine[3] => dorsal_outline[1];
        spine[4] => dorsal_outline[2];
        spine[5] => dorsal_outline[3];
        catmullRomSpline(dorsal_outline, 8, dorsal_spline_positions);

        // wispy strands
        for (int i; i < dorsal_trails.size(); i++) {
            dorsal_spline_positions[i] => dorsal_trails[i].follow;
        }
    }
}