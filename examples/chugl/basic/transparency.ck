//-----------------------------------------------------------------------------
// name: bloom.ck
// desc: Transparent materials are sorted from back-to-front and drawn after
// all opaque, i.e. non-transparent materials in the scene. Distance is 
// determined by distance to the plane of the camera, and as a result the 
// render order may change as depending on the angle of the camera. For things
// like UI or 2D scenes this should be sufficient.
//
// ChuGL does *not* store a per-pixel transparency list or do any techniques 
// for OIT.
// 
// author: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//   date: Summer 2025
//-----------------------------------------------------------------------------

// Init camera
GOrbitCamera orbit_cam --> GG.scene();
GG.scene().camera(orbit_cam);
orbit_cam.posZ(2);
orbit_cam.viewSize(2);

// init materials
FlatMaterial opaque_mat;
FlatMaterial red_mat, green_mat, blue_mat;
red_mat.color(@(1, 0, 0, 0.5)); 
green_mat.color(@(0, 1, 0, 0.5)); 
blue_mat.color(@(0, 0, 1, 0.5));
true => red_mat.transparent; // IMPORTANT!! explicitly mark materials as transparent
true => green_mat.transparent;
true => blue_mat.transparent;

PlaneGeometry plane_geo;

// Init meshes
GMesh opaque(plane_geo, opaque_mat) --> GG.scene();
GMesh transparent_green(plane_geo, green_mat) --> GG.scene();
GMesh transparent_blue(plane_geo, blue_mat) --> GG.scene();
GMesh transparent_red_list(plane_geo, red_mat)[4];
GGen transparent_red --> GG.scene();
for (int i; i < transparent_red_list.size(); ++i) {
    transparent_red_list[i] --> transparent_red;
    transparent_red_list[i].posZ(-i);
}

transparent_red.pos(-.1, -.1, -.2);
transparent_green.pos(0, .1, -.4);
transparent_blue.pos(.1, -.1, -.6);
opaque.posZ(-.8);

UI_Float opaque_depth(opaque.posZ());
UI_Float transparent_green_depth(transparent_green.posZ());
UI_Float transparent_blue_depth(transparent_blue.posZ());
UI_Float transparent_red_depth(transparent_red.posZ());
UI_Bool orthographic;
UI_Bool red_is_transparent(red_mat.transparent());

while (1) {
    GG.nextFrame() => now;
    // UI for positioning objects
    if (UI.begin("transparency example")) {
        if (UI.slider("transparent red depth", transparent_red_depth, -1, 0))
            transparent_red_depth.val() => transparent_red.posZ;

        if (UI.slider("transparent green depth", transparent_green_depth, -1, 0))
            transparent_green_depth.val() => transparent_green.posZ;

        if (UI.slider("transparent blue depth", transparent_blue_depth, -1, 0))
            transparent_blue_depth.val() => transparent_blue.posZ;

        if (UI.slider("opaque depth", opaque_depth, -1, 0))
            opaque_depth.val() => opaque.posZ;
        
        if (UI.checkbox("orthographic", orthographic)) {
            if (orthographic.val()) orbit_cam.orthographic();
            else orbit_cam.perspective();
        }

        if (UI.checkbox("red transparency", red_is_transparent))
            red_is_transparent.val() => red_mat.transparent;
    }
    UI.end();
}