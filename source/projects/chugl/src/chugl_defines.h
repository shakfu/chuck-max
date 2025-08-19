#pragma once

// ChuGL version string
#define CHUGL_VERSION_STRING "0.2.6 (alpha)"

#define CHUGL_RENDERGRAPH_MAX_PASSES 32
#define CHUGL_MATERIAL_MAX_BINDINGS 32 // @group(1) @binding(0 - 31)
#define CHUGL_MAX_BINDGROUPS 4

// how many frames does a bindgroup need to be unused before we evict it from our
// rendergraph cache
#define CHUGL_CACHE_BINDGROUP_FRAMES_TILL_EXPIRED 30
#define CHUGL_CACHE_TEXTURE_VIEW_FRAMES_TILL_EXPIRED 30

#define CHUGL_GEOMETRY_MAX_PULLED_VERTEX_BUFFERS 4 // @group(4) storage buffers

#define CHUGL_COMPUTE_ENTRY_POINT "main"

// shadow stuff
#define CHUGL_SPOT_SHADOWMAP_DEFAULT_DIM 512
#define CHUGL_DIR_SHADOWMAP_DEFAULT_DIM 1024

// from E. Lengyel Foundations of Game Engine Development Volume II pg. 191
#define CHUGL_SHADOW_MAP_DEPTH_OFFSET                                                  \
    0.00000190734f // depth offset for light projection matrix to remove self-shadowing
                   // artifact. delta = 2^-19