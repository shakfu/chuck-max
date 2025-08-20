/*
To avoid an edge-case bug where the graphics-thread XForm cache isn't updated properly, 
disallow setting a foreign camera on a ScenePass, i.e. a camera that is *not* grucked to the scene.

More specifically: the transform hierarchy is cached and updated just-in-time, i.e.
as we are walking the rendergraph from GG.rootPass(). If the foreign camera is not attached,
to the scene being rendered, its world matrix may not be updated, meaning the scene will 
be rendered from an incorrect camera matrix
*/

ScenePass sp;
GScene scene;
GCamera camera --> scene;
GCamera foreign_camera --> GScene other_scene;

sp.scene(scene, camera); // should work
sp.scene(scene, foreign_camera); // fails