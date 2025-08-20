Color.RED => vec3 red_rgb;
Color.rgb2hsv(red_rgb) => vec3 red_hsv;
Color.hsv2rgb(red_hsv) => vec3 red_hsv2rgb;

T.assert(T.veq(red_rgb, @(1, 0, 0)), "red_rgb");
T.assert(T.veq(red_hsv, @(0, 1, 1)), "red_hsv");
T.assert(T.veq(red_hsv2rgb, @(1, 0, 0)), "red_hsv2rgb");

@(
    Math.random2f(0, 1),
    Math.random2f(0, 1),
    Math.random2f(0, 1)
 ) => vec3 rand_rgb;
Color.rgb2hsv(rand_rgb) => vec3 rand_hsv;
Color.hsv2rgb(rand_hsv) => vec3 rand_hsv2rgb;

T.assert(T.veq(rand_rgb, rand_hsv2rgb), "rand_rgb == rand_hsv2rgb");

