//-----------------------------------------------------------------------------
// name: webcam.ck
// desc: Webcam example
//
// authors: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//
// date: Fall 2024
//-----------------------------------------------------------------------------

Webcam webcam;
<<< "webcam width: ", webcam.width() >>>;
<<< "webcam height: ", webcam.height() >>>;
<<< "webcam fps: ", webcam.fps() >>>;
<<< "webcam name: ", webcam.deviceName() >>>;

FlatMaterial plane_mat;
plane_mat.colorMap(webcam.texture());
PlaneGeometry plane_geo;
GMesh plane(plane_geo, plane_mat) --> GG.scene();

plane.scaX(3 * webcam.aspect());
plane.scaY(3);

UI_Bool capture(webcam.capture());

while (true) {
    GG.nextFrame() => now;

    if (UI.begin("Webcam Example")) {
        if (UI.checkbox("Capture", capture)) {
            webcam.capture(capture.val());
        }
    }
    UI.end();
}
