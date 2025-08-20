@import "lib/g2d/ChuGL.chug"
@import "lib/g2d/g2d.ck"
@import "lib/M.ck"
@import "lib/T.ck"

G2D g;

// vec3 midi_colors[12]; // for debug
// for (int i; i < midi_colors.size(); i++) {
//     Color.hsv2rgb(@(360 * i / 12.0, 1.0, 1.0)) => midi_colors[i];
//     <<< midi_colors[i] >>>;
// }

@(.5, .5, .5) => vec3 col;

while (1) {
    GG.nextFrame() => now;
    <<< col, M.srgbToLinear(col) >>>;
    g.boxFilled(@(0,0), 0, 1, 1, M.srgbToLinear(col));
    g.boxFilled(@(2,2), 0, 1, 1, (col));
}
