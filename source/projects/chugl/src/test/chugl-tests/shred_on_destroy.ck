/*
Test shred_on_destroy_listener.
A graphics shred can exit, automatically unregister itself, and NOT hang the window
*/

GCube cube --> GG.scene();

fun void color(vec3 color) {
    GG.nextFrame() => now;
    cube.color(color);
} 
spork ~ color(Color.WHITE);

repeat (5) {
    GG.nextFrame() => now;
    spork ~ color(Color.random());
    GG.dt() => cube.rotateY;
}
