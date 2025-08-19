/*
Andrew Zhu Aday
Summer 2025

Test to catch crash caused by rapidly switching materials and/or geometries of a mesh.

Fix: Reference count all wgpu resources that are associated with bindgroups (textureviews, bindgrouplayouts, and buffers)
so that those pointer addresses are guaranteed to be unique identifiers. Otherwise, say a buffer is destroyed and another one 
immediately created--the newly created buffer may take the same pointer address as the old one, incorrectly causing a cache
hit in our bindgroup cache in the chugl rendergraph, causing us to bind what is actually a stale bindgroup.

By reference counting those wgpu resources, we guarantee that any newly created wgpu resources are at addresses
which are NOT in the set of those wgpu resource pointers held onto by any bindgroups in the bindgroup cache.
Furthermore, if a wgpu resource address *is* re-used, it can only be because all those bindgroups which referenced
it have since expired, meaning any subsequent bindgroups will be created with the fresh address+generation.
*/

GG.rootPass() --> ScenePass sp(GG.scene());

FlatMaterial mat1, mat2;
PlaneGeometry geo;

GMesh mesh(geo, mat1) --> GG.scene();

repeat (60) {
    GG.nextFrame() => now;
    if (mesh.mat() == mat1) mat2 => mesh.mat;
    else mat1 => mesh.mat;
}

