/*
Andrew Zhu Aday
Summer 2025

Fixed bug where bindgroups were not being set on renderpipelines built from shaders that
did not define any bindgroups under @group(1).
This test is for the fix where we still set an empty wgpu bindgroups up until the max non-empty
@group number. E.g. if the shader defines @group(0) and @group(2), we still set an empty
wgpuBindGroup at @group(1). 
*/

UVMaterial mat; // does not have any uniforms, @group(1) is not used
PlaneGeometry geo;

GMesh mesh(geo, mat) --> GG.scene();

GG.nextFrame() => now;