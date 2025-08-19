//-----------------------------------------------------------------------------
// name: webcam_echo.ck
// desc: Create an echo/waterfall video effect using webcam, multiple textures,
// and texture copying. 
//
// authors: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//
// date: Fall 2024
//-----------------------------------------------------------------------------

Webcam webcam;

GG.camera().orthographic();

PlaneGeometry geo;
FlatMaterial plane_mats[9];

// function to place a row of delayed webcam textures
fun void positionPlanes(float y_off, int flip_x) {
    for (int i; i < plane_mats.size(); i++) {
        GMesh plane(geo, plane_mats[i]) --> GG.scene();
        plane.pos(@(flip_x * (i - 4), y_off, 0));
        plane.mat(plane_mats[i]);
        plane.geo(geo);
    }
}

// positionPlanes(-3, -1);
positionPlanes(-2, 1);
positionPlanes(-1, -1);
positionPlanes(0, 1);
positionPlanes(1, -1);
positionPlanes(2, 1);
// positionPlanes(3, -1);

// initialize our texture "delay line" to buffer webcam frames
TextureDesc desc;
false => desc.mips; // do NOT generate mips, otherwise will be invisible because we are only copying to mip level 0
webcam.width() => desc.width;
webcam.height() => desc.height;
Texture echo_textures(desc)[30 * plane_mats.size()];

0 => int curr_texture;
while (true) {
    // copy every 2 frames (webcam fps is typically 30 not 60)
    GG.nextFrame() => now; GG.nextFrame() => now;

    // update delayed webcam textures
    if (curr_texture >= echo_textures.size()) 0 => curr_texture;
    for (int i; i < plane_mats.size(); i++) {
        echo_textures[(curr_texture + 30 * (i + 1)) % echo_textures.size()] => plane_mats[i].colorMap;
    } 

    // copy newest frame from webcam
    Texture.copy(echo_textures[curr_texture], webcam.texture());
    curr_texture++;
}