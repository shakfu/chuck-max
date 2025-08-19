/*
Test changing material PSO properties.
Added after updating to WebGPU Version 22.1.0.5, which made WGPUBindGroups now
depend on the WGPUBindGroupLayout of a specific pipeline.
*/

GCube cube --> GG.scene();

GG.nextFrame() => now;
cube.mat().cullMode(Material.Cull_Front);
GG.nextFrame() => now;
cube.mat().topology(Material.Topology_LineStrip);
GG.nextFrame() => now;













