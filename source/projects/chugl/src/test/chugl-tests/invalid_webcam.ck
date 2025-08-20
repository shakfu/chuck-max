Webcam webcam_negative(-1);
Webcam webcam_out_of_bounds(9);

GG.nextFrame() => now;

// both should default to same magenta texture
if (webcam_negative.texture() == webcam_out_of_bounds.texture()) {
    <<< "webcam_negative and webcam_out_of_bounds have the same texture" >>>;
    <<< webcam_negative.texture().width(), webcam_negative.texture().height() >>>;
} else {
    <<< "webcam_negative and webcam_out_of_bounds have different textures" >>>;
}

// test setting fields does not crash
webcam_negative.freeze(true);
webcam_negative.capture(false);
webcam_out_of_bounds.freeze(true);
webcam_out_of_bounds.capture(false);

GG.nextFrame() => now;
GG.nextFrame() => now;
