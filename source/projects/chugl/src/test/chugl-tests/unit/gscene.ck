GCamera camera --> GScene scene;

T.assert(scene.camera() != null, "default camera NOT null");

scene.camera(camera);
T.assert(scene.camera().id() == camera.id(), "scene set camera");

T.assert(T.veq(scene.backgroundColor(), @(0, 0, 0)), "default background color");
scene.backgroundColor(@(0.1, 0.2, 0.3));
T.assert(T.veq(scene.backgroundColor(), @(0.1, 0.2, 0.3)), "set background color");

// default scene lights
GG.scene().lights() @=> GLight lights[];
T.assert(lights.size() == 1, "default scene lights size");
T.assert(GG.scene().light() == lights[0], "default scene light");

GPointLight point_light --> GG.scene();
T.assert(GG.scene().light() != point_light, "adding lights does not change default light");
T.assert(GG.scene().lights().size() == 2, "adding lights increases lights size");
point_light.detach();
T.assert(GG.scene().lights().size() == 1, "detaching lights decreases lights size");

// test ambient light
GG.scene().ambient(@(0.1, 0.2, 0.3));
T.assert(T.veq(GG.scene().ambient(), @(0.1, 0.2, 0.3)), "set ambient light");

// test auto update

T.assert(GG.autoUpdate(), "default auto update");
GG.autoUpdate(false);
T.assert(!GG.autoUpdate(), "set auto update");
