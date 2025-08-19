//-----------------------------------------------------------------------------
// name: ik.ck
// desc: inverse kinematics
//
// author: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
// adapted from: http://www.andreasaristidou.com/FABRIK.html
//   date: Fall 2024
//-----------------------------------------------------------------------------

SphereGeometry sphere_geo;
FlatMaterial flat_material;

40 => int NUM_LENGTHS;
float lengths[NUM_LENGTHS];
for (int i; i < NUM_LENGTHS; i++) {
    4.0 / NUM_LENGTHS => lengths[i];
}
vec3 points[lengths.size() + 1]; // N points

GMesh joints[0];
for (int i; i < points.size(); i++) {
    GMesh joint(sphere_geo, flat_material) --> GG.scene();
    joints << joint;
    .1 => joint.sca;
}

// add camera to scene graph
GCamera camera --> GG.scene();
// set main camera
GG.scene().camera(camera);
// camera projection
camera.orthographic();
// position camera
camera.posZ(5);

fun void fabrik(vec3 start_target, vec3 end_target, vec3 points[], float lengths[]) {
    0.01 => float TOLERANCE;
    10 => int MAX_ITERATIONS;
    0 => float total_length;
    for (0 => int i; i < lengths.size(); i++) {
        lengths[i] +=> total_length;
    }
    // case: end target out of reach
    end_target - start_target => vec3 dir;
    if (total_length < dir.magnitude()) {
        dir.normalize();
        // set all points to point towards end_target
        for (1 => int i; i < points.size(); i++) {
            dir * lengths[i - 1] + points[i - 1] => points[i];
        }
        return;
    }

    points.size() => int N;
    repeat (MAX_ITERATIONS) {
        if (Math.euclidean(points[points.size() - 1], end_target) < TOLERANCE) {
            return;
        }

        // forward
        end_target => points[N-1]; // put last point at target
        for (N - 2 => int i; i >= 0; i--) {
            points[i] - points[i+1] => vec3 dir;
            dir.normalize();
            points[i + 1] + dir * lengths[i] => points[i];
        }

        // backwards
        start_target => points[0];
        for (1 => int i; i < points.size() - 1; i++) {
            points[i] - points[i-1] => vec3 dir; 
            dir.normalize();
            lengths[i-1] * dir + points[i-1] => points[i];
        }
    }
}

fun void draw() {
    camera.screenCoordToWorldPos(GWindow.mousePos(), 5) => vec3 end_target;
    0 => end_target.z; // put on plane
    fabrik(@(0, 0, 0), end_target, points, lengths);

    for (int i; i < points.size(); i++) {
        points[i] => joints[i].pos;
    }
}

while (true) {
    GG.nextFrame() => now;
    draw();
}
