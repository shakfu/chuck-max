public class M {
    0.017453292519943 => static float DEG2RAD;
    57.295779513082320 => static float RAD2DEG;

    // TODO: add to ulib_color
    fun static vec3 srgbToLinear(vec3 c) {
        2.2 => float g;
        return @(
            Math.pow(c.r, g),
            Math.pow(c.g, g),
            Math.pow(c.b, g)
        );
    }

    fun static vec2 rot2vec(float radians) {
        return @( Math.cos(radians), Math.sin(radians) );
    }

    fun static float angle(vec2 a, vec2 b) {
        b - a => vec2 n;
        return Math.atan2(n.y, n.x);
    }

    fun static vec2 normalize(vec2 n) {
        return n / Math.hypot(n.x, n.y); // hypot is the magnitude
    }

    fun static float mag(vec2 n) {
        return  Math.hypot(n.x, n.y); 
    }

    fun static float dist(vec2 a, vec2 b) {
        return Math.euclidean(a, b);
    }

    fun static float cross(vec2 a, vec2 b) {
        return a.x * b.y - a.y * b.x;
    }

    fun static vec2 perp(vec2 a) {
        return @(-a.y, a.x);
    }

    // unwinds rad to be in range [0, 2pi]
    fun static float unwind(float rad) {
        while (rad > Math.two_pi) Math.two_pi -=> rad;
        while (rad < 0)           Math.two_pi +=> rad;
        return rad;
    }

    // compute the angle between two angles in radians
    fun static float deltaAngle(float rad1, float rad2) {
        unwind(rad1) => rad1;
        unwind(rad2) => rad2;
        Math.fabs(rad1 - rad2) => float delta;
        Math.min(delta, Math.fabs(Math.two_pi - delta)) => delta;
        return delta;
    }

    // returns a clamped between [b, c] inclusive
    fun static int clamp(int a, int b, int c) {
        return Math.min(Math.max(a, b), c);
    }

    // ===================================================
    // Tweens
    // ===================================================
    fun static float easeOutQuad(float x) {
        return 1 - (1 - x) * (1 - x);
    }

    fun static float easeInOutCubic(float x) {
        if (x < 0.5) 
            return 4 * x * x * x;
        else 
            return 1 - Math.pow(-2 * x + 2, 3) / 2;
    }

    // ===================================================
    // Random
    // ===================================================

    fun static vec2 randomDir() {
        return rot2vec(Math.random2f(0, Math.two_pi));
    }

    fun static vec2 randomPointInCircle(vec2 center, float min_r, float max_r) {
        Math.random2f(0, Math.two_pi) => float theta;
        Math.random2f(min_r, max_r) => float radius;
        return center + radius * @(Math.cos(theta), Math.sin(theta));
    }


    fun static vec2 randomPointInArea(vec2 center, float hw, float hh) {
        return center + @(
            Math.random2f(-hw, hw),
            Math.random2f(-hh, hh)
        );
    }

    // returns a weighted-choice from a PDF
    // e.g. [5, 3, 2]
    // return 0: 50% chance
    // return 1: 30% chance
    // return 2: 20% chance
    fun static int choose(float pdf[]) {
        0.0 => float total;
        0.0 => float accum;

        for (auto p : pdf) p +=> total;
        Math.random2f(0.0, total) => float pick;

        for (int i; i < pdf.size(); i++) {
            pdf[i] +=> accum;
            if (accum >= pick) return i;
        }
        return pdf.size() - 1;
    }

    // return a random element from the array with uniform distribution
    fun static int randomElement(int arr[]) {
        return arr[Math.random2(0, arr.size() - 1)];
    }

    // given an event happens once every `period_secs`, returns
    // the amount of time you need to wait until the next occurance.
    // returns the amount of time in seconds until the next event occurs
    fun static float poisson(float period_sec) {
        (1 / 1000.0) => float dt_sec;
        dt_sec / period_sec => float success_rate;

        0 => int count;
        while (Math.randomf() > success_rate) count++;

        return count * dt_sec;
    }


    // offscreen test for orthographic camera
    // does NOT to frustrum test for perpsective camera currently
    // assumes ortho camera pointed down -Z axis centered at @(0,0)
    // TODO: add offscreen test to ulib_camera
    // TODO put in g2d

    // fun static int offscreen(vec2 pos, float bounding_radius_ndc) {
    //     GG.camera().worldPosToNDC(@(pos.x, pos.y, 0)) => vec3 pos_ndc;
    //     return (Math.fabs(pos_ndc.x) > bounding_radius_ndc || Math.fabs(pos_ndc.y) > bounding_radius_ndc);
    // }

    // fun static int offscreen(vec2 pos) {
    //     return offscreen(pos, 1.0);
    // }

    // ===================================================
    // Intersection Collision Testing
    // ===================================================

    // Returns 1 if the lines intersect, otherwise 0. In addition, if the lines 
    // intersect the intersection point may be stored in the floats i_x and i_y.
    // modified and improved from: https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
    fun static int intersect(
        vec2 p0, vec2 p1,
        vec2 p2, vec2 p3
    ) {
        p1 - p0 => vec2 s1;
        p3 - p2 => vec2 s2;

        cross(s1, s2) => float det;
        if (Math.fabs(det) < .000001) return false; // lines are colinear

        p0 - p2 => vec2 p0p2;
        perp(s1).dot(p0p2) / det => float s;
        if (s < 0 || s > 1) return false;
        perp(s2).dot(p0p2) / det => float t;
        if (t < 0 || t > 1) return false;
        return true;
    }
}