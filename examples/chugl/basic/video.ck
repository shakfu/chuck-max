//-----------------------------------------------------------------------------
// name: video.ck
// desc: Video playback example. Currently only supports the MPEG1 video and 
// MP2 audio.
// To run, download and place this music video in the same directory: 
// https://ccrma.stanford.edu/~azaday/music/bjork-all-is-full-of-love.mpg
//
// authors: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//
// Find more mpeg samples here: https://filesamples.com/formats/mpeg
//
// date: Fall 2024
//-----------------------------------------------------------------------------

Video video(me.dir() + "./bjork-all-is-full-of-love.mpg") => dac; 

<<< "VM Samplerate: ", 1::second / 1::samp >>>;
<<< "Framerate: ", video.framerate() >>>;
<<< "Samplerate: ", video.samplerate() >>>;
<<< "Duration: ", video.duration() >>>;
<<< "Loop: ", video.loop() >>>;
<<< "Rate: ", video.rate() >>>;

video.texture() @=> Texture video_texture;
(video.width() $ float) / video.height() => float video_aspect;

[
    new PlaneGeometry,
    new SuzanneGeometry,
    new SphereGeometry,
    new CubeGeometry,
    new CircleGeometry,
    new TorusGeometry,
    new CylinderGeometry,
    new KnotGeometry,
] @=> Geometry geometries[];

UI_Int geometry_index;
[
    "PlaneGeometry",
    "SuzanneGeometry",
    "SphereGeometry",
    "CubeGeometry",
    "CircleGeometry",
    "TorusGeometry",
    "CylinderGeometry",
    "KnotGeometry",
] @=> string builtin_geometries[];

FlatMaterial video_mat;
video_mat.scale(@(1, -1)); // flip the y-axis

GMesh video_mesh(geometries[0], video_mat) --> GG.scene();

GOrbitCamera camera --> GG.scene();
camera.clip(.01, 1000);
GG.scene().camera(camera);

video_mesh.scaX(3 * video_aspect);
video_mesh.scaY(3);
video_mesh.scaZ(3);

video_mat.colorMap(video_texture);

UI_Float rate(1.0);
UI_Bool loop(video.loop());
UI_Float2 copies(video_mat.scale());
UI_Float3 scale(video_mesh.sca());

while (true) {
    GG.nextFrame() => now;

    if (GWindow.keyDown(GWindow.Key_Left)) {
        video.seek(video.timestamp() - 10::second);
    } else if (GWindow.keyDown(GWindow.Key_Right)) {
        video.seek(video.timestamp() + 10::second);
    }

    if (UI.begin("")) {

        UI.textWrapped("Use the arrow keys to seek 10 seconds back or forward.");

        UI.separator();

        if (UI.slider("Rate", rate, -2.0, 2.0)) {
            rate.val() => video.rate;
        }

        if (UI.listBox("builtin geometries", geometry_index, builtin_geometries)) {
            video_mesh.geometry(geometries[geometry_index.val()]);
        }

        if (UI.drag("Copies", copies)) {
            copies.val() => video_mat.scale;
        }

        if (UI.drag("Scale", scale)) {
            scale.val() => video_mesh.sca;
        }

        if (UI.checkbox("Loop", loop)) {
            loop.val() => video.loop;
        }
    }
    UI.end();
}