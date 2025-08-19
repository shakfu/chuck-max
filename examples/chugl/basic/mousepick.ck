//--------------------------------------------------------------------
// name: mousepick.ck
// desc: mouse picking with different projections
// 
// author: Andrew Zhu Aday
//   date: Fall 2024
//--------------------------------------------------------------------
FlatMaterial material;
SphereGeometry sphere_geo;

GCamera camera;
camera --> GGen dolly --> GG.scene();
camera.pos(@(1, 2, 3)); 
GG.scene().camera(camera);

[
    "perspective",
    "orthographic",
] @=> string camera_modes[];
UI_Int camera_mode(0);

fun void ui()
{
    while (true) {
        GG.nextFrame() => now;
        if (UI.listBox("Camera mode", camera_mode, camera_modes, -1)) {
            if (camera_mode.val() == 0) {
                camera.perspective();
            } else {
                camera.orthographic();
            }
        }
    }
} spork ~ ui();

while (true) {
    GG.nextFrame() => now;
    if (GWindow.mouseLeftDown()) {
        GMesh sphere(sphere_geo, material) --> GG.scene();
        sphere.sca(.1);
        sphere.pos(camera.screenCoordToWorldPos(GWindow.mousePos(), 10));
    }
}