//--------------------------------------------------------------------
// name: ray_triangle_intersection.ck
// desc: Ray-triangle intersection testing using mouse cursor
// 
// author: Shenran Wang and Andrew Zhu Aday
//   date: Fall 2024
//--------------------------------------------------------------------

GPlane plane --> GG.scene();

GOrbitCamera orbit_camera --> GG.scene();
GG.scene().camera(orbit_camera);

// a plane is made up of two triangles:
[
    // triangle 0
    @(0.5, 0.5, 0),
    @(-0.5, -0.5, 0),
    @(0.5, -0.5, 0),

    // triangle 1
    @(-0.5, -0.5, 0),
    @(0.5, 0.5, 0),
    @(-0.5, 0.5, 0),
]  @=> vec3 triangles[];

fun float dot(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// triangle intersection test
// translated from https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
fun int isect(
    vec3 ray_origin,
    vec3 ray_dir,
    vec3 v0,
    vec3 v1,
    vec3 v2
) {
    .0001 => float epsilon;

    v1 - v0 => vec3 e1;
    v2 - v0 => vec3 e2;
    ray_dir * e2 => vec3 ray_cross_e2;
    dot(e1, ray_cross_e2) => float det;

    if (det > -epsilon && det < epsilon)
        return false;    // This ray is parallel to this triangle.

    1.0 / det => float inv_det;
    ray_origin - v0 => vec3 s;
    inv_det * dot(s, ray_cross_e2) => float u;

    if (u < 0 || u > 1)
        return false;

    s * e1 => vec3 s_cross_e1;
    inv_det * dot(ray_dir, s_cross_e1) => float v;

    if (v < 0 || u + v > 1)
        return false;

    // At this stage we can compute t to find out where the intersection point is on the line.
    inv_det * dot(e2, s_cross_e1) => float t;

    if (t > epsilon) // ray intersection
        return true;
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}


while (true) {
    GG.nextFrame() => now;

    // rotate the plane
    GG.dt() => plane.rotateY;

    // compute the ray origin and direction given mouse position 
    GG.scene().camera().screenCoordToWorldPos(GWindow.mousePos(), 1.0) => vec3 mouse_world_pos;
    mouse_world_pos - GG.scene().camera().posWorld() => vec3 ray_dir;
    ray_dir.normalize();
    GG.scene().camera().posWorld() => vec3 ray_origin;

    // check if the ray intersects the plane by checking if it intersects any of the two triangles
    if (
        isect(ray_origin, ray_dir, 
            plane.posLocalToWorld(triangles[0]),
            plane.posLocalToWorld(triangles[1]), 
            plane.posLocalToWorld(triangles[2])
        )
        ||
        isect(ray_origin, ray_dir, 
            plane.posLocalToWorld(triangles[3]), 
            plane.posLocalToWorld(triangles[4]), 
            plane.posLocalToWorld(triangles[5])
        )
    ) {
        plane.color(Color.RED);
    } else {
        plane.color(Color.WHITE);
    }
}