/*----------------------------------------------------------------------------
 ChuGL: Unified Audiovisual Programming in ChucK

 Copyright (c) 2023 Andrew Zhu Aday and Ge Wang. All rights reserved.
   http://chuck.stanford.edu/chugl/
   http://chuck.cs.princeton.edu/chugl/

 MIT License

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
-----------------------------------------------------------------------------*/
#include "r_component.h"
#include "core/hashmap.h"
#include "geometry.h"
#include "graphics.h"
#include "shaders.h"

#include "compressed_fonts.h"

#include "core/file.h"
#include "core/log.h"
#include "core/spinlock.h"

#include <stb/stb_image.h>

#include <glm/gtx/matrix_decompose.hpp>

#include <sr_webcam/include/sr_webcam.h>

#include <sokol/sokol_time.h>

static int compareSGIDs(const void* a, const void* b, void* udata)
{
    return *(SG_ID*)a - *(SG_ID*)b;
}

static uint64_t hashSGID(const void* item, uint64_t seed0, uint64_t seed1)
{
    return hashmap_xxhash3(item, sizeof(SG_ID), seed0, seed1);
}

/*
XForm system:
- each XForm component is marked with a stale flag NONE/DESCENDENTS/WORLD/LOCAL
- when an XForm is reparented or moved, it marks itself as stale WORLD/LOCAL and
all its parents as DESCENDENTS
    - DESCENDENTS means my own transform is fine, but at least 1 descendent
needs to be recomputed
    - WORLD means my local matrix is fine, but my world matrix needs to be
recomputed
    - LOCAL means both local and world matrices need to be recomputed
- at the start of each render, after all updates, the graphics thread needs to
call R_Transform::rebuildMatrices(root) to update all world matrices
    - the staleness flags optimizes this process by ignoring all branches that
don't require updating
    - during this rebuild, transforms that have been marked as WORLD or LOCAL
will also mark their geometry as stale

Geometry system:
- all draws are instanced. All xforms bound to a certain geometry and material
will be rendered in a single draw call
- each geometry component has a storage buffer that holds all the world matrices
of its associated xform instances
- if a xform is moved or reparented, the geometry component will be marked
stale, and the storage buffer will be rebuilt via R_Geometry::rebuildBindGroup

Component Manager:
- handles all creation and deletion of components
- components (Xforms, Geos ...) are stored in contiguous Arena memory
- deletions are handled by swapping the deleted component with the last
component
- each component has a unique ID
- a hashmap stores the ID to offset mapping
  - store offset and not pointer because pointers can be invalidated if the
arena grows, or elements within are deleted and swapped

Note: the stale flag scenegraph system allows for a nice programming pattern
in ChucK:

// put all objects that can move under here
GGen dynamic --> GG.scene();
// put all immobile objects here
GGen static --> GG.scene();

all static GGens are grouped under a single XForm, and this entire branch of the
scenegraph will be skipped every frame!

*/

// ============================================================================
// Forward Declarations
// ============================================================================

static SG_ID _componentIDCounter = 1; // reserve 0 for NULL

// All R_IDs are negative to avoid conflict with positive SG_IDs
static R_ID _R_IDCounter = -1; // reserve 0 for NULL.

static SG_ID getNewComponentID()
{
    return _componentIDCounter++;
}

static R_ID getNewRID()
{
    return _R_IDCounter--;
}

// ============================================================================
// Transform Component
// ============================================================================

static void R_Transform_init(R_Transform* xform, SG_ID id, SG_ComponentType comp_type)
{
    *xform = {};

    xform->id   = id;
    xform->type = comp_type;

    xform->_pos = glm::vec3(0.0f);
    xform->_rot = QUAT_IDENTITY;
    xform->_sca = glm::vec3(1.0f);

    xform->world  = MAT_IDENTITY;
    xform->local  = MAT_IDENTITY;
    xform->_stale = R_Transform_STALE_LOCAL;

    xform->parentID = 0;

    // initialize children array for 8 children
    Arena::init(&xform->children, sizeof(SG_ID) * 8);
}

void R_Transform::init(R_Transform* transform)
{
    ASSERT(transform->id == 0); // ensure not initialized twice
    *transform = {};

    transform->id   = getNewComponentID();
    transform->type = SG_COMPONENT_TRANSFORM;

    transform->_pos = glm::vec3(0.0f);
    transform->_rot = QUAT_IDENTITY;
    transform->_sca = glm::vec3(1.0f);

    transform->world  = MAT_IDENTITY;
    transform->local  = MAT_IDENTITY;
    transform->_stale = R_Transform_STALE_NONE;

    transform->parentID = 0;
    // initialize children array for 8 children
    Arena::init(&transform->children, sizeof(SG_ID) * 8);
}

void R_Transform::initFromSG(R_Transform* r_xform, SG_Command_CreateXform* cmd)
{
    ASSERT(r_xform->id == 0); // ensure not initialized twice
    *r_xform = {};

    // copy base component data
    // TODO have a separate R_ComponentType enum?
    r_xform->id   = cmd->sg_id;
    r_xform->type = SG_COMPONENT_TRANSFORM;

    // copy xform
    r_xform->_pos   = cmd->pos;
    r_xform->_rot   = cmd->rot;
    r_xform->_sca   = cmd->sca;
    r_xform->_stale = R_Transform_STALE_LOCAL;

    // initialize children array for 8 children
    Arena::init(&r_xform->children, sizeof(SG_ID) * 8);
}

void R_Transform::setStale(R_Transform* xform, R_Transform_Staleness stale)
{
    // only set if new staleness is higher priority
    if (stale > xform->_stale) xform->_stale = stale;

    // propagate staleness to parent
    // it is assumed that if a parent has staleness, all its parents will
    // also have been marked with prio at least R_Transform_STALE_DESCENDENTS
    while (xform->parentID != 0) {
        R_Transform* parent = Component_GetXform(xform->parentID);
        // upwards stale chain already established. skip
        if (parent->_stale > R_Transform_STALE_NONE) break;
        // otherwise set parent staleness and continue propagating
        parent->_stale = R_Transform_STALE_DESCENDENTS;
        xform          = parent;
    }
}

R_Scene* R_Transform::getScene(R_Transform* xform)
{
    // walk up parent chain until scene is found
    while (xform) {
        if (xform->type == SG_COMPONENT_SCENE) return (R_Scene*)xform;
        xform = Component_GetXform(xform->parentID);
    }

    return NULL;
}

bool R_Transform::isAncestor(R_Transform* ancestor, R_Transform* descendent)
{
    while (descendent != NULL) {
        if (descendent == ancestor) return true;
        descendent = Component_GetXform(descendent->parentID);
    }
    return false;
}

void R_Transform::removeChild(R_Transform* parent, R_Transform* child)
{
    if (child->parentID != parent->id) {
        log_error("cannot remove a child who does not belong to parent");
        return;
    }

    size_t numChildren = ARENA_LENGTH(&parent->children, SG_ID);
    SG_ID* children    = (SG_ID*)parent->children.base;

    // remove child's parent reference
    child->parentID = 0;

    // remove from parent
    for (size_t i = 0; i < numChildren; ++i) {
        if (children[i] == child->id) {
            // swap with last element
            children[i] = children[numChildren - 1];
            // pop last element
            Arena::pop(&parent->children, sizeof(SG_ID));
            break;
        }
    }

    // remove child subgraph from scene render state
    R_Scene* scene = R_Transform::getScene(parent);
    R_Scene::removeSubgraphFromRenderState(scene, child);
}

void R_Transform::removeAllChildren(R_Transform* parent)
{
    if (!parent) return;

    R_Scene* scene     = R_Transform::getScene(parent);
    size_t numChildren = ARENA_LENGTH(&parent->children, SG_ID);
    SG_ID* children    = (SG_ID*)parent->children.base;

    for (size_t i = 0; i < numChildren; ++i)
        R_Scene::removeSubgraphFromRenderState(scene, Component_GetXform(children[i]));

    Arena::clear(&parent->children);
}

void R_Transform::addChild(R_Transform* parent, R_Transform* child)
{
    if (R_Transform::isAncestor(child, parent)) {
        log_error("No cycles in scenegraph; cannot add parent as child of descendent");
        return;
    }

    if (parent == NULL || child == NULL) {
        log_error("Cannot add NULL parent or child to scenegraph");
        return;
    }

    // relationship already in place, do noting
    if (child->parentID == parent->id) return;

    // remove child from previous parent
    if (child->parentID != 0) {
        R_Transform* prevParent = Component_GetXform(child->parentID);
        R_Transform::removeChild(prevParent, child);
    }

    // set parent of child
    child->parentID = parent->id;

    // add child to parent
    SG_ID* xformID = ARENA_PUSH_ZERO_TYPE(&parent->children, SG_ID);
    *xformID       = child->id;

    R_Transform::setStale(child, R_Transform_STALE_WORLD);

    // add child subgraph to scene render state
    R_Scene* scene = R_Transform::getScene(parent);
    R_Scene::addSubgraphToRenderState(scene, child);
}

glm::mat4 R_Transform::localMatrix(R_Transform* xform)
{
    glm::mat4 M = glm::mat4(1.0);
    M           = glm::translate(M, xform->_pos);
    M           = M * glm::toMat4(xform->_rot);
    M           = glm::scale(M, xform->_sca);
    return M;
}

/// @brief decompose matrix into transform data
void R_Transform::setXformFromMatrix(R_Transform* xform, const glm::mat4& M)
{
    log_trace("decomposing matrix");
    xform->_pos  = glm::vec3(M[3]);
    xform->_rot  = glm::quat_cast(M);
    xform->_sca  = glm::vec3(glm::length(M[0]), glm::length(M[1]), glm::length(M[2]));
    xform->local = M;

    // log_trace("pos: %s", glm::to_string(xform->_pos).c_str());
    // log_trace("rot: %s", glm::to_string(xform->_rot).c_str());
    // log_trace("sca: %s", glm::to_string(xform->_sca).c_str());

    R_Transform::setStale(xform, R_Transform_STALE_LOCAL);
}

void R_Transform::setXform(R_Transform* xform, const glm::vec3& pos,
                           const glm::quat& rot, const glm::vec3& sca)
{
    xform->_pos = pos;
    xform->_rot = rot;
    xform->_sca = sca;
    R_Transform::setStale(xform, R_Transform_STALE_LOCAL);
}

void R_Transform::pos(R_Transform* xform, const glm::vec3& pos)
{
    xform->_pos = pos;
    R_Transform::setStale(xform, R_Transform_STALE_LOCAL);
}

void R_Transform::rot(R_Transform* xform, const glm::quat& rot)
{
    xform->_rot = rot;
    R_Transform::setStale(xform, R_Transform_STALE_LOCAL);
}

void R_Transform::sca(R_Transform* xform, const glm::vec3& sca)
{
    xform->_sca = sca;
    R_Transform::setStale(xform, R_Transform_STALE_LOCAL);
}

void R_Transform::decomposeWorldMatrix(const glm::mat4& m, glm::vec3& pos,
                                       glm::quat& rot, glm::vec3& scale)
{
    // pos = m[3];
    // for (int i = 0; i < 3; i++) scale[i] = glm::length(glm::vec3(m[i]));
    // const glm::mat3 rotMtx(glm::vec3(m[0]) / scale[0], glm::vec3(m[1]) / scale[1],
    //                        glm::vec3(m[2]) / scale[2]);
    // rot = glm::quat_cast(rotMtx);
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(m, scale, rot, pos, skew, perspective);
}

/// @brief recursive helper to regen matrices for xform and all descendents
static void _Transform_RebuildDescendants(R_Scene* scene, R_Transform* xform,
                                          const glm::mat4* parentWorld)
{
    // mark primitive as stale since world matrix will change
    if (xform->_geoID && xform->_matID) {
        ASSERT(xform->type == SG_COMPONENT_MESH);
        R_Scene::markPrimitiveStale(scene, xform);
    }

    // TODO ==optimize==: this is where we would mark lights as stale
    // For now we rebuild the light storage buffer every frame, no memoization

    // rebuild local mat
    if (xform->_stale == R_Transform_STALE_LOCAL)
        xform->local = R_Transform::localMatrix(xform);

    // always rebuild world mat
    xform->world  = (*parentWorld) * xform->local;
    xform->normal = glm::transpose(glm::inverse(xform->world));

    // set fresh
    xform->_stale = R_Transform_STALE_NONE;

    // rebuild all children
    for (u32 i = 0; i < ARENA_LENGTH(&xform->children, SG_ID); ++i) {
        R_Transform* child
          = Component_GetXform(*ARENA_GET_TYPE(&xform->children, SG_ID, i));
        ASSERT(child != NULL);
        _Transform_RebuildDescendants(scene, child, &xform->world);
    }
}

void R_Transform::rebuildMatrices(R_Scene* root, Arena* arena)
{
    glm::mat4 identityMat = MAT_IDENTITY;
    u64 arena_orig_size   = arena->curr;

    // push root onto stack
    *ARENA_PUSH_TYPE(arena, SG_ID) = root->id;

    // while stack is not empty
    while (arena->curr != arena_orig_size) {
        // pop id from stack
        Arena::pop(arena, sizeof(SG_ID));
        // update stack top
        SG_ID xformID = *(SG_ID*)Arena::top(arena);

        R_Transform* xform = Component_GetXform(xformID);
        ASSERT(xform != NULL);

        switch (xform->_stale) {
            case R_Transform_STALE_NONE: break;
            case R_Transform_STALE_DESCENDENTS: {
                // add to stack
                SG_ID* children = (SG_ID*)xform->children.base;
                for (u32 i = 0; i < ARENA_LENGTH(&xform->children, SG_ID); ++i) {
                    SG_ID* childID = ARENA_PUSH_ZERO_TYPE(arena, SG_ID);
                    *childID       = children[i];
                }
                break;
            }
            case R_Transform_STALE_WORLD:
            case R_Transform_STALE_LOCAL: {
                // get parent world matrix
                R_Transform* parent = Component_GetXform(xform->parentID);
                _Transform_RebuildDescendants(root, xform,
                                              parent ? &parent->world : &identityMat);
                break;
            }
            default: log_error("unhandled staleness %d", xform->_stale); break;
        }

        // always set fresh
        xform->_stale = R_Transform_STALE_NONE;
    }
}

u32 R_Transform::numChildren(R_Transform* xform)
{
    return ARENA_LENGTH(&xform->children, SG_ID);
}

R_Transform* R_Transform::getChild(R_Transform* xform, u32 index)
{
    return Component_GetXform(*ARENA_GET_TYPE(&xform->children, SG_ID, index));
}

void R_Transform::rotateOnLocalAxis(R_Transform* xform, glm::vec3 axis, f32 deg)
{
    R_Transform::rot(xform, xform->_rot * glm::angleAxis(deg, glm::normalize(axis)));
}

void R_Transform::rotateOnWorldAxis(R_Transform* xform, glm::vec3 axis, f32 deg)
{
    R_Transform::rot(xform, glm::angleAxis(deg, glm::normalize(axis)) * xform->_rot);
}

void R_Transform::updateMesh(R_Transform* xform, SG_ID geo_id, SG_ID mat_id)
{
    R_Scene* scene = Component_GetScene(xform->scene_id);
    if (scene) {
        // remove from previous primitive
        R_Scene::unregisterMesh(scene, xform);
    }

    xform->_geoID = geo_id;
    xform->_matID = mat_id;

    // add new primitive
    if (scene) R_Scene::registerMesh(scene, xform);
}

void R_Transform::print(R_Transform* xform)
{
    R_Transform::print(xform, 0);
}

void R_Transform::print(R_Transform* xform, u32 depth)
{
    for (u32 i = 0; i < depth; ++i) {
        printf("  ");
    }
    // printf("%s\n", xform->name.c_str());
    printf("%s\n", xform->name);
    // print position
    for (u32 i = 0; i < depth; ++i) {
        printf("  ");
    }
    printf("  pos: %f %f %f\n", xform->_pos.x, xform->_pos.y, xform->_pos.z);

    u32 childrenCount = R_Transform::numChildren(xform);
    for (u32 i = 0; i < childrenCount; ++i) {
        print(R_Transform::getChild(xform, i), depth + 1);
    }
}

// ============================================================================
// Geometry Component
// ============================================================================
void R_Geometry::init(R_Geometry* geo)
{
    ASSERT(geo->id == 0);
    *geo = {};

    geo->id   = getNewComponentID();
    geo->type = SG_COMPONENT_GEOMETRY;
}

u32 R_Geometry::indexCount(R_Geometry* geo)
{
    return geo->gpu_index_buffer.size / sizeof(u32);
}

u32 R_Geometry::vertexCount(R_Geometry* geo)
{
    if (geo->vertex_attribute_num_components[0] == 0) return 0;

    return geo->gpu_vertex_buffers[0].size
           / (sizeof(f32) * geo->vertex_attribute_num_components[0]);
}

// returns # of contiguous non-zero vertex attributes
u32 R_Geometry::vertexAttributeCount(R_Geometry* geo)
{
    for (int i = 0; i < ARRAY_LENGTH(geo->vertex_attribute_num_components); ++i) {
        if (geo->vertex_attribute_num_components[i] == 0) return i;
    }
    return ARRAY_LENGTH(geo->vertex_attribute_num_components);
}

void R_Geometry::setVertexAttribute(GraphicsContext* gctx, R_Geometry* geo,
                                    u32 location, u32 num_components_per_attrib,
                                    void* data, size_t size)
{
    ASSERT(location >= 0
           && location < ARRAY_LENGTH(geo->vertex_attribute_num_components));

    geo->vertex_attribute_num_components[location] = num_components_per_attrib;
    GPU_Buffer::write(gctx, &geo->gpu_vertex_buffers[location],
                      (WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst), data, size);
}

void R_Geometry::setIndices(GraphicsContext* gctx, R_Geometry* geo, u32* indices,
                            u32 indices_count)
{
    GPU_Buffer::write(gctx, &geo->gpu_index_buffer,
                      (WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst), indices,
                      indices_count * sizeof(*indices));
}

bool R_Geometry::usesVertexPulling(R_Geometry* geo)
{
    for (int i = 0; i < ARRAY_LENGTH(geo->pull_buffers); ++i) {
        if (geo->pull_buffers[i].buf) return true;
    }
    return false;
}

void R_Geometry::addPullBindGroupEntries(R_Geometry* geo, G_Graph* graph, G_DrawCall* d)
{
    for (u32 i = 0; i < ARRAY_LENGTH(geo->pull_buffers); i++) {
        if (geo->pull_buffers[i].buf == NULL) {
            continue;
        }

        graph->bindBuffer(d, VERTEX_PULL_GROUP, i, geo->pull_buffers[i].buf, 0,
                          geo->pull_buffers[i].size);
    }
}

void R_Geometry::setPulledVertexAttribute(GraphicsContext* gctx, R_Geometry* geo,
                                          u32 location, void* data, size_t size_bytes)
{
    GPU_Buffer::write(gctx, &geo->pull_buffers[location], WGPUBufferUsage_Storage, data,
                      size_bytes);
}

// ============================================================================
// R_Texture
// ============================================================================

struct LoadImageResult {
    void* pixel_data_OWNED; // free with stbi_image_free
    size_t pixel_data_size; // byte size of pixel data
    i32 width;
    i32 height;
    i32 components; // 4 for rgba
};

enum LoadImageType {
    LoadImageType_File = 0,
    LoadImageType_Raw,
};

struct LoadImageParams {
    LoadImageType type;

    // shared params
    WGPUTextureFormat format;
    bool flip_y;

    // raw params
    u8* buffer;
    int buffer_len;

    // file params
    const char* filepath;
};

static LoadImageResult R_Texture_LoadImage(LoadImageParams p)
{
    LoadImageResult result = {};
    // Force loading 3 channel images to 4 channel by stb becasue Dawn
    // doesn't support 3 channel formats currently. The group is discussing
    // on whether webgpu shoud support 3 channel format.
    // https://github.com/gpuweb/gpuweb/issues/66#issuecomment-410021505
    i32 desired_comps = STBI_rgb_alpha; // force 4 channels

    stbi_set_flip_vertically_on_load(p.flip_y);

    // currently only support ldr (TODO add hdr f16 and f32)
    ASSERT(p.format == WGPUTextureFormat_RGBA8Unorm);

    if (p.type == LoadImageType_File) {
        result.pixel_data_OWNED = stbi_load(p.filepath,         //
                                            &result.width,      //
                                            &result.height,     //
                                            &result.components, //
                                            desired_comps       //
        );
    } else if (p.type == LoadImageType_Raw) {
        // STBIDEF stbi_uc *stbi_load_from_memory   (stbi_uc           const *buffer,
        // int len   , int *x, int *y, int *channels_in_file, int desired_channels);
        result.pixel_data_OWNED = stbi_load_from_memory(p.buffer, p.buffer_len,
                                                        &result.width,      //
                                                        &result.height,     //
                                                        &result.components, //
                                                        desired_comps       //
        );
    } else {
        UNREACHABLE;
    }
    bool is_hdr = false;

    // update byte size
    if (result.pixel_data_OWNED) {
        result.pixel_data_size = result.width * result.height * desired_comps;
    }

    // if (texture->desc.format == WGPUTextureFormat_RGBA32Float) {
    //     pixelData = stbi_loadf(filepath,     //
    //                            &width,       //
    //                            &height,      //
    //                            &read_comps,  //
    //                            desired_comps //
    //     );

    if (result.pixel_data_OWNED == NULL) {
        log_error("Couldn't load image from raw data\n. Reason: %s",
                  stbi_failure_reason());
    } else {
        log_info("Loaded %s image from raw data (%d, %d, %d / %d)\n",
                 is_hdr ? "HDR" : "LDR", result.width, result.height, result.components,
                 desired_comps);
    }

    return result;
}

int R_Texture::sizeBytes(R_Texture* texture)
{
    return wgpuTextureGetWidth(texture->gpu_texture)
           * wgpuTextureGetHeight(texture->gpu_texture)
           * wgpuTextureGetDepthOrArrayLayers(texture->gpu_texture)
           * G_bytesPerTexel(wgpuTextureGetFormat(texture->gpu_texture));
}

void R_Texture::load(GraphicsContext* gctx, R_Texture* texture, const char* filepath,
                     bool flip_vertically, bool gen_mips)
{
    LoadImageParams params = {};
    params.type            = LoadImageType_File;
    params.format          = texture->desc.format;
    params.flip_y          = flip_vertically;
    params.filepath        = filepath;
    LoadImageResult result = R_Texture_LoadImage(params);
    // free pixel data
    defer(if (result.pixel_data_OWNED) stbi_image_free(result.pixel_data_OWNED););

    SG_TextureWriteDesc write_desc = {};
    write_desc.width               = result.width;
    write_desc.height              = result.height;
    R_Texture::write(gctx, texture, &write_desc, result.pixel_data_OWNED,
                     result.width * result.height
                       * G_bytesPerTexel(texture->desc.format));

    if (gen_mips) {
        // MipMapGenerator_generate(gctx, texture->gpu_texture, texture->name.c_str());
        MipMapGenerator_generate(gctx, texture->gpu_texture, texture->name);
    }
}

void R_Texture::load(GraphicsContext* gctx, R_Texture* texture, u8* buffer,
                     int buffer_len, bool flip_vertically, bool gen_mips)
{
    LoadImageParams params = {};
    params.type            = LoadImageType_Raw;
    params.format          = texture->desc.format;
    params.flip_y          = flip_vertically;
    params.buffer          = buffer;
    params.buffer_len      = buffer_len;
    LoadImageResult result = R_Texture_LoadImage(params);

    // free pixel data
    defer(if (result.pixel_data_OWNED) stbi_image_free(result.pixel_data_OWNED););

    SG_TextureWriteDesc write_desc = {};
    write_desc.width               = result.width;
    write_desc.height              = result.height;
    R_Texture::write(gctx, texture, &write_desc, result.pixel_data_OWNED,
                     result.width * result.height
                       * G_bytesPerTexel(texture->desc.format));

    if (gen_mips) {
        // MipMapGenerator_generate(gctx, texture->gpu_texture, texture->name.c_str());
        MipMapGenerator_generate(gctx, texture->gpu_texture, texture->name);
    }
}

void R_Texture::loadCubemap(GraphicsContext* gctx, R_Texture* texture,
                            const char* right_face_path, const char* left_face_path,
                            const char* top_face_path, const char* bottom_face_path,
                            const char* back_face_path, const char* front_face_path,
                            bool flip_y)
{
    // cubemap validation
    ASSERT(texture->desc.depth == 6);
    ASSERT(texture->desc.format == WGPUTextureFormat_RGBA8Unorm);
    ASSERT(!texture->desc.gen_mips);

    const char* faces[6] = { right_face_path,  left_face_path, top_face_path,
                             bottom_face_path, back_face_path, front_face_path };

    LoadImageParams params = {};
    params.type            = LoadImageType_File;
    params.format          = texture->desc.format;
    params.flip_y          = flip_y;
    for (int i = 0; i < 6; i++) {
        params.filepath        = faces[i];
        LoadImageResult result = R_Texture_LoadImage(params);
        // free pixel data
        defer(if (result.pixel_data_OWNED) stbi_image_free(result.pixel_data_OWNED););

        // validate size
        ASSERT(texture->desc.width == result.width);
        ASSERT(texture->desc.height == result.height);

        SG_TextureWriteDesc write_desc = {};
        write_desc.offset_z            = i; // write to ith cubemap face
        write_desc.width               = result.width;
        write_desc.height              = result.height;
        R_Texture::write(gctx, texture, &write_desc, result.pixel_data_OWNED,
                         result.pixel_data_size);
    }
}

// ============================================================================
// R_Material
// ============================================================================

void MaterialTextureView::init(MaterialTextureView* view)
{
    *view          = {};
    view->strength = 1.0f;
    view->scale[0] = 1.0f;
    view->scale[1] = 1.0f;
}

void R_Material::createBindGroupEntries(R_Material* mat, int group, G_Graph* graph,
                                        G_DrawCall* drawcall, GraphicsContext* gctx)
{
    // create bindgroups for all bindings

    // super jank rn, if drawcall is NULL we assume we are adding bindings to a compute
    // pass
    ASSERT(CHUGL_MATERIAL_MAX_BINDINGS == ARRAY_LENGTH(mat->bindings));
    for (int i = 0; i < CHUGL_MATERIAL_MAX_BINDINGS; ++i) {
        R_Binding* binding = &mat->bindings[i];
        if (binding->type == R_BIND_EMPTY) continue;

        switch (binding->type) {
            case R_BIND_UNIFORM: {
                const int UNIFORM_OFFSET
                  = MAX(gctx->limits.minUniformBufferOffsetAlignment,
                        sizeof(SG_MaterialUniformData));
                drawcall ? graph->bindBuffer(drawcall, group, i, mat->uniform_buffer,
                                             UNIFORM_OFFSET * i,
                                             sizeof(SG_MaterialUniformData)) :
                           graph->computePassBindBuffer(i, mat->uniform_buffer,
                                                        UNIFORM_OFFSET * i,
                                                        sizeof(SG_MaterialUniformData));
            } break;
            case R_BIND_STORAGE: {
                drawcall ?
                  graph->bindBuffer(drawcall, group, i, binding->as.storage_buffer.buf,
                                    0, binding->size) :
                  graph->computePassBindBuffer(i, binding->as.storage_buffer.buf, 0,
                                               binding->size);
            } break;
            case R_BIND_SAMPLER: {
                drawcall ? graph->bindSampler(
                             drawcall, group, i,
                             Graphics_GetSampler(gctx, binding->as.samplerConfig)) :
                           graph->computePassBindSampler(
                             i, Graphics_GetSampler(gctx, binding->as.samplerConfig));
            } break;
            case R_BIND_TEXTURE: {
                R_Texture* r_texture
                  = Component_GetTexture(binding->as.texture.texture_id);

                // hacky: clamp INT32_MAX to num_mips to signify we're creating a
                // texture view of the entire chain
                int num_mips = (int)wgpuTextureGetMipLevelCount(r_texture->gpu_texture);
                bool is_cubemap
                  = (wgpuTextureGetDepthOrArrayLayers(r_texture->gpu_texture) == 6);
                G_CacheTextureViewDesc view_desc
                  = { r_texture->gpu_texture,
                      is_cubemap ? WGPUTextureViewDimension_Cube :
                                   WGPUTextureViewDimension_2D,
                      binding->as.texture.base_mip_level,
                      MIN(num_mips, binding->as.texture.mip_level_count),
                      0,
                      1 };

                drawcall ? graph->bindTexture(drawcall, group, i, view_desc) :
                           graph->computePassBindTexture(i, view_desc);

                ASSERT(r_texture->gpu_texture);
                ASSERT(binding->size == sizeof(R_TextureBinding));
            } break;
            case R_BIND_STORAGE_EXTERNAL: {
                drawcall ? graph->bindBuffer(drawcall, group, i,
                                             binding->as.storage_external->buf, 0,
                                             binding->size) :
                           graph->computePassBindBuffer(
                             i, binding->as.storage_external->buf, 0, binding->size);
            } break;
            default: ASSERT(false);
        }
    }
}

static SamplerConfig samplerConfigFromSGSampler(SG_Sampler sg_sampler)
{
    // TODO make SamplerConfig and SG_Sampler just use WGPU types
    SamplerConfig sampler = {};
    sampler.wrapU         = (SamplerWrapMode)sg_sampler.wrapU;
    sampler.wrapV         = (SamplerWrapMode)sg_sampler.wrapV;
    sampler.wrapW         = (SamplerWrapMode)sg_sampler.wrapW;
    sampler.filterMin     = (SamplerFilterMode)sg_sampler.filterMin;
    sampler.filterMag     = (SamplerFilterMode)sg_sampler.filterMag;
    sampler.filterMip     = (SamplerFilterMode)sg_sampler.filterMip;
    return sampler;
}

void R_Material::setSamplerBinding(GraphicsContext* gctx, R_Material* mat, u32 location,
                                   SG_Sampler sampler)
{
    SamplerConfig sampler_config = samplerConfigFromSGSampler(sampler);
    R_Material::setBinding(gctx, mat, location, R_BIND_SAMPLER, &sampler_config,
                           sizeof(sampler_config));
}

void R_Material::setExternalStorageBinding(GraphicsContext* gctx, R_Material* mat,
                                           u32 location, GPU_Buffer* buffer)
{
    ASSERT(buffer->usage & WGPUBufferUsage_Storage);
    R_Material::setBinding(gctx, mat, location, R_BIND_STORAGE_EXTERNAL, buffer,
                           buffer->size);
}

void R_Material::bindTexture(GraphicsContext* gctx, R_Material* mat, u32 location,
                             R_TextureBinding bind_desc)
{
    R_Material::setBinding(gctx, mat, location, R_BIND_TEXTURE, &bind_desc,
                           sizeof(bind_desc));
}

void R_Material::setBinding(GraphicsContext* gctx, R_Material* mat, u32 location,
                            R_BindType type, void* data, size_t bytes)
{
    R_Binding* binding = &mat->bindings[location];
    binding->type      = type;
    binding->size      = bytes;

    // create new binding
    switch (type) {
        case R_BIND_UNIFORM: {
            size_t offset = MAX(gctx->limits.minUniformBufferOffsetAlignment,
                                sizeof(SG_MaterialUniformData))
                            * location;
            // ==optimize== set uniform buffer stale flag, flush the whole buffer at
            // once before traversing the rendergraph
            wgpuQueueWriteBuffer(gctx->queue, mat->uniform_buffer, offset, data, bytes);
        } break;
        case R_BIND_TEXTURE: {
            ASSERT(bytes == sizeof(R_TextureBinding));
            binding->as.texture = *(R_TextureBinding*)data;
        } break;
        case R_BIND_SAMPLER: {
            ASSERT(bytes == sizeof(SamplerConfig));
            binding->as.samplerConfig = *(SamplerConfig*)data;
        } break;
        case R_BIND_STORAGE: {
            GPU_Buffer::write(gctx, &binding->as.storage_buffer,
                              WGPUBufferUsage_Storage, data, bytes);
        } break;
        case R_BIND_STORAGE_EXTERNAL: {
            // external storage buffer
            binding->as.storage_external = (GPU_Buffer*)data;
        } break;
        default:
            // if the new binding is also STORAGE reuse the memory, don't
            // free
            log_error("unsupported binding type %d", type);
            ASSERT(false);
            break;
    }
}

// ============================================================================
// GeometryToXforms (scene helper)
// ============================================================================

struct GeometryToXformKey {
    SG_ID geo_id;
    SG_ID mat_id;
};

struct GeometryToXforms {
    GeometryToXformKey key;
    hashmap* xform_id_set; // xforms that are using this geometry
    GPU_Buffer xform_storage_buffer;
    Arena draw_uniform_list; // array of DrawUniforms
    size_t push_size;        // size in bytes per element of draw_uniform_list
    b8 buffer_stale;         // if true, need to update storage buffer

    static int count(GeometryToXforms* g2x)
    {
        return hashmap_count(g2x->xform_id_set);
    }

    static bool hasXform(GeometryToXforms* g2x, SG_ID xform_id)
    {
        return hashmap_get(g2x->xform_id_set, &xform_id) != NULL;
    }

    static void addXform(GeometryToXforms* g2x, SG_ID xform_id)
    {
        hashmap_set(g2x->xform_id_set, &xform_id);
        g2x->buffer_stale = true;
    }

    static void removeXform(GeometryToXforms* g2x, SG_ID xform_id)
    {
        const void* removed = hashmap_delete(g2x->xform_id_set, &xform_id);
        g2x->buffer_stale   = true;
        ASSERT(removed);
        UNUSED_VAR(removed);
    }

    static int compare(const void* a, const void* b, void* udata)
    {
        GeometryToXforms* ga = (GeometryToXforms*)a;
        GeometryToXforms* gb = (GeometryToXforms*)b;
        return memcmp(&ga->key, &gb->key, sizeof(ga->key));
    }

    static u64 hash(const void* item, uint64_t seed0, uint64_t seed1)
    {
        GeometryToXforms* g2x = (GeometryToXforms*)item;
        return hashmap_xxhash3(&g2x->key, sizeof(g2x->key), seed0, seed1);
    }

    static void free(void* item)
    {
        GeometryToXforms* g2x = (GeometryToXforms*)item;
        GPU_Buffer::destroy(&g2x->xform_storage_buffer);
        hashmap_free(g2x->xform_id_set);
        Arena::free(&g2x->draw_uniform_list);
    }

    static void updateStorageBuffer(GraphicsContext* gctx, R_Scene* scene,
                                    GeometryToXforms* g2x, WGPULimits* limits)
    {
        // should be nonempty (if empty, should have already been deleted)
        ASSERT(hashmap_count(g2x->xform_id_set) > 0);
        R_Material* mat  = Component_GetMaterial(g2x->key.mat_id);
        size_t push_size = mat->pso.transparent ?
                             ALIGN_NON_POW2(sizeof(DrawUniforms),
                                            limits->minStorageBufferOffsetAlignment) :
                             sizeof(DrawUniforms);

        bool draw_uniform_padding_unchanged = (push_size == g2x->push_size);

        if (draw_uniform_padding_unchanged && !g2x->buffer_stale) return;
        defer(g2x->buffer_stale = false);
        defer(g2x->push_size = push_size);

        // if material is flipped from transparent --> not transparent,
        // we actually need to mark this as stale and rebuild the buffer because
        // now the padding between frameuniforms must be at
        // limits.minStorageBufferOffsetAlignment (cannot instanced draw transparent
        // materials, must draw 1 at a time)
        /*
        option 1: no more stale/not stale, just always populate a (dynamic) storage
        buffer every frame with the per-draw uniform data.
            - but then binding would be tricky, need to have another G_BindGroup type
        for this StorageBuffer class, because StorageBuffer might have to resize,
        invalidating the older bindgroups that pointed to a pre-resize WGPUBuffer

        <<< WENT WITH THIS ONE >>>
        option 2: store push size, check the intended push size based on mat
        transparency. simplest for now.
        */

        // build new array of matrices on CPU
        Arena::clear(&g2x->draw_uniform_list);

        size_t hashmap_idx_DONT_USE = 0;
        SG_ID* xform_id             = NULL;
        while (
          hashmap_iter(g2x->xform_id_set, &hashmap_idx_DONT_USE, (void**)&xform_id)) {
            R_Transform* xform = Component_GetXform(*xform_id);

            // all xforms should be valid here (can't delete xforms while
            // iterating)
            bool xform_same_mesh
              = (xform->_geoID == g2x->key.geo_id && xform->_matID == g2x->key.mat_id);
            bool xform_same_scene = (xform->scene_id == scene->id);
            ASSERT(xform && xform_same_mesh && xform_same_scene);
            UNUSED_VAR(xform_same_mesh);
            UNUSED_VAR(xform_same_scene);

            // world matrix should already have been computed by now
            ASSERT(xform->_stale == R_Transform_STALE_NONE);

            // add xform matrix to arena
            DrawUniforms* draw_uniforms
              = (DrawUniforms*)Arena::push(&g2x->draw_uniform_list, push_size);
            *draw_uniforms
              = { xform->world, xform->normal, xform->id, xform->receives_shadows, {} };
        }

        snprintf(g2x->xform_storage_buffer.label,
                 sizeof(g2x->xform_storage_buffer.label),
                 "Per-Draw Storage Buffer for Mat: %d, Geo: %d", g2x->key.mat_id,
                 g2x->key.geo_id);

        u64 write_size = g2x->draw_uniform_list.curr;
        GPU_Buffer::write(gctx, &g2x->xform_storage_buffer, WGPUBufferUsage_Storage,
                          g2x->draw_uniform_list.base, write_size);
    }

    static DrawUniforms* drawUniform(GeometryToXforms* g2x, int i)
    {
        ASSERT(g2x->push_size);
        ASSERT(i < GeometryToXforms::count(g2x));
        return (DrawUniforms*)Arena::get(&g2x->draw_uniform_list, i * g2x->push_size);
    }
};

static GeometryToXforms* R_Scene_getPrimitive(R_Scene* scene, SG_ID mat_id,
                                              SG_ID geo_id)
{
    GeometryToXformKey key = {};
    key.geo_id             = geo_id;
    key.mat_id             = mat_id;
    GeometryToXforms* g2x  = (GeometryToXforms*)hashmap_get(scene->geo_to_xform, &key);

    if (!g2x) {
        // create new one
        GeometryToXforms new_g2x = {};
        new_g2x.key.geo_id       = geo_id;
        new_g2x.key.mat_id       = mat_id;
        u64 seed                 = time(NULL);
        new_g2x.xform_id_set     = hashmap_new(sizeof(SG_ID), 0, seed, seed, hashSGID,
                                               compareSGIDs, NULL, NULL);
        hashmap_set(scene->geo_to_xform, &new_g2x);
        g2x = (GeometryToXforms*)hashmap_get(scene->geo_to_xform, &key);
    }
    ASSERT(g2x);
    return g2x;
}

// ============================================================================
// R_Scene
// ============================================================================

void R_Scene::removeSubgraphFromRenderState(R_Scene* scene, R_Transform* root)
{
    if (!scene || !root) return;

    ASSERT(scene->id == root->scene_id);

    // find all meshes in child subgraph
    static Arena arena{};
    defer(Arena::clear(&arena));

    SG_ID* sg_id = ARENA_PUSH_TYPE(&arena, SG_ID);
    *sg_id       = root->id;
    while (ARENA_LENGTH(&arena, SG_ID)) {
        SG_ID xformID = *ARENA_GET_LAST_TYPE(&arena, SG_ID);
        ARENA_POP_TYPE(&arena, SG_ID);
        R_Transform* xform = Component_GetXform(xformID);

        // add children to queue
        for (u32 i = 0; i < ARENA_LENGTH(&xform->children, SG_ID); ++i) {
            *ARENA_PUSH_ZERO_TYPE(&arena, SG_ID)
              = *ARENA_GET_TYPE(&xform->children, SG_ID, i);
        }

        // remove scene from xform
        ASSERT(xform->scene_id == scene->id);
        xform->scene_id = 0;

        if (xform->type == SG_COMPONENT_LIGHT) {
            // remove from light set
            const void* prev_item = hashmap_delete(scene->light_id_set, &xform->id);
            UNUSED_VAR(prev_item);
            ASSERT(prev_item);
            // TODO: somehow consolidate GText into also being a mesh...
        } else if (xform->type == SG_COMPONENT_MESH) {
            R_Scene::unregisterMesh(scene, xform);
        }
    }
}

void R_Scene::addSubgraphToRenderState(R_Scene* scene, R_Transform* root)
{
    ASSERT(scene == R_Transform::getScene(root));

    if (!scene) return;

    // find all meshes in child subgraph
    static Arena arena{};
    defer(Arena::clear(&arena));

    *ARENA_PUSH_TYPE(&arena, SG_ID) = root->id;
    while (ARENA_LENGTH(&arena, SG_ID)) {
        R_Transform* xform = Component_GetXform(*ARENA_GET_LAST_TYPE(&arena, SG_ID));
        ARENA_POP_TYPE(&arena, SG_ID);

        // add children to queue
        for (u32 i = 0; i < ARENA_LENGTH(&xform->children, SG_ID); ++i) {
            *ARENA_PUSH_ZERO_TYPE(&arena, SG_ID)
              = *ARENA_GET_TYPE(&xform->children, SG_ID, i);
        }

        // add scene to transform state
        ASSERT(xform->scene_id != scene->id);
        xform->scene_id = scene->id;

        // lighting logic
        if (xform->type == SG_COMPONENT_LIGHT) {
            // add light to scene
            const void* replaced = hashmap_set(scene->light_id_set, &xform->id);
            // light should not already be in scene
            UNUSED_VAR(replaced);
            ASSERT(!replaced);
        } else if (xform->_geoID != 0 && xform->_matID != 0) { // for all renderables
            ASSERT(xform->type == SG_COMPONENT_MESH);
            R_Scene::registerMesh(scene, xform);
        }
    }
}

void R_Scene::rebuildLightInfoBuffer(GraphicsContext* gctx, R_Scene* scene,
                                     G_Graph* graph, FrameUniforms* frame_uniforms)
{
    static Arena light_info_arena{};
    ASSERT(light_info_arena.curr == 0);
    defer(Arena::clear(&light_info_arena));

    int shadow_generators_count                 = 0;
    int shadow_generators_total_renderlist_size = 0;

    char string_buf[128] = {};
    u32 shadow_map_write_indices[SG_LightType_Count]
      = {}; // which array layer of the shadowmap Texture2D array to write to
    u32 shadow_map_counts_by_type[SG_LightType_Count] = {};

    // allocate cpu memory for light uniform data
    int num_lights = R_Scene::numLights(scene);
    LightUniforms* light_uniforms
      = ARENA_PUSH_COUNT(&light_info_arena, LightUniforms, num_lights);

    size_t hashmap_idx_DONT_USE = 0;
    SG_ID* light_id             = NULL;
    int light_idx               = 0;
    while (
      hashmap_iter(scene->light_id_set, &hashmap_idx_DONT_USE, (void**)&light_id)) {
        LightUniforms* light_uniform = &light_uniforms[light_idx++];
        *light_uniform               = {};

        R_Light* light = Component_GetLight(*light_id);
        ASSERT(light);
        ASSERT(light->_stale == R_Transform_STALE_NONE);

        { // initialize uniform struct
            light_uniform->color      = light->desc.intensity * light->desc.color;
            light_uniform->light_type = (i32)light->desc.type;
            light_uniform->position   = light->world[3];
            light_uniform->direction  = -glm::normalize(light->world[2]);

            if (light->desc.generates_shadows) {
                light_uniform->proj_view
                  = light->projection(false) * R_Transform::viewMatrix(light);
                light_uniform->generates_shadows = 1;
                light_uniform->shadow_map_idx
                  = shadow_map_counts_by_type[light->desc.type];
                light_uniform->bias = light->desc.bias;
                ++shadow_map_counts_by_type[light->desc.type];
                ++shadow_generators_count;
                shadow_generators_total_renderlist_size
                  += hashmap_count(light->shadow_render_id_set);
            }

            switch (light->desc.type) {
                case SG_LightType_None: break;
                case SG_LightType_Directional: {
                } break;
                case SG_LightType_Point: {
                    light_uniform->point_and_spot_radius  = light->desc.radius;
                    light_uniform->point_and_spot_falloff = light->desc.falloff;

                } break;
                case SG_LightType_Spot: {
                    light_uniform->point_and_spot_radius  = light->desc.radius;
                    light_uniform->point_and_spot_falloff = light->desc.falloff;
                    light_uniform->spot_cos_angle_min     = cosf(light->desc.angle_min);
                    light_uniform->spot_cos_angle_max     = cosf(light->desc.angle_max);
                    light_uniform->spot_angular_falloff   = light->desc.angle_falloff;
                } break;
                default: ASSERT(false);
            }
        }
    }

    // upload light uniforms to gpu
    GPU_Buffer::write(gctx, &scene->light_info_buffer, WGPUBufferUsage_Storage,
                      light_info_arena.base, light_info_arena.curr);

    // rebuild shadow map arrays if resized
    for (int light_type = 0; light_type < SG_LightType_Count; ++light_type) {
        if (light_type != SG_LightType_Spot && light_type != SG_LightType_Directional)
            continue; // other types not impl

        int curr_layers = 0;
        if (light_type == SG_LightType_Spot) {
            curr_layers
              = scene->spot_shadow_map_array ?
                  wgpuTextureGetDepthOrArrayLayers(scene->spot_shadow_map_array) :
                  0;
        } else if (light_type == SG_LightType_Directional) {
            curr_layers
              = scene->dir_shadow_map_array ?
                  wgpuTextureGetDepthOrArrayLayers(scene->dir_shadow_map_array) :
                  0;
        }

        // add +1 to prevent crash from not binding anything
        u32 min_layers = shadow_map_counts_by_type[light_type] + 1;
        if (curr_layers >= min_layers) continue;

        WGPUTextureDescriptor shadowmap_desc = {};
        shadowmap_desc.usage
          = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_TextureBinding;
        shadowmap_desc.dimension     = WGPUTextureDimension_2D;
        shadowmap_desc.format        = WGPUTextureFormat_Depth32Float;
        shadowmap_desc.mipLevelCount = 1;
        shadowmap_desc.sampleCount   = 1;

        if (light_type == SG_LightType_Spot) {
            shadowmap_desc.size // TODO ==api==: setting for shadowmap resolution
              = { CHUGL_SPOT_SHADOWMAP_DEFAULT_DIM, CHUGL_SPOT_SHADOWMAP_DEFAULT_DIM,
                  min_layers };
            snprintf(string_buf, sizeof(string_buf),
                     "Spot shadowmap array for Scene[%d] %s", scene->id, scene->name);
            shadowmap_desc.label = string_buf;

            WGPU_RELEASE_RESOURCE(Texture, scene->spot_shadow_map_array);
            scene->spot_shadow_map_array
              = wgpuDeviceCreateTexture(gctx->device, &shadowmap_desc);

            // create color target
            snprintf(string_buf, sizeof(string_buf),
                     "Spot ShadowPass Color Target array for Scene[%d] %s", scene->id,
                     scene->name);
            shadowmap_desc.format = WGPUTextureFormat_RGBA8Unorm;

            WGPU_RELEASE_RESOURCE(Texture, scene->spot_shadow_color_map_array);
            scene->spot_shadow_color_map_array
              = wgpuDeviceCreateTexture(gctx->device, &shadowmap_desc);
        } else if (light_type == SG_LightType_Directional) {
            shadowmap_desc.size // TODO ==api==: setting for shadowmap resolution
              = { CHUGL_DIR_SHADOWMAP_DEFAULT_DIM, CHUGL_DIR_SHADOWMAP_DEFAULT_DIM,
                  min_layers };
            snprintf(string_buf, sizeof(string_buf),
                     "Directional shadowmap array for Scene[%d] %s", scene->id,
                     scene->name);
            shadowmap_desc.label = string_buf;

            WGPU_RELEASE_RESOURCE(Texture, scene->dir_shadow_map_array);
            scene->dir_shadow_map_array
              = wgpuDeviceCreateTexture(gctx->device, &shadowmap_desc);

            // create color target
            snprintf(string_buf, sizeof(string_buf),
                     "Directional ShadowPass Color Target array for Scene[%d] %s",
                     scene->id, scene->name);
            shadowmap_desc.format = WGPUTextureFormat_RGBA8Unorm;

            WGPU_RELEASE_RESOURCE(Texture, scene->dir_shadow_color_map_array);
            scene->dir_shadow_color_map_array
              = wgpuDeviceCreateTexture(gctx->device, &shadowmap_desc);
        }
    }

    // prepare frame uniforms
    FrameUniforms light_frame_uniforms = {};
    COPY_STRUCT(&light_frame_uniforms, frame_uniforms);

    // shadowmap passes
    hashmap_idx_DONT_USE = 0;
    light_id             = NULL;
    light_idx            = 0;
    while (
      hashmap_iter(scene->light_id_set, &hashmap_idx_DONT_USE, (void**)&light_id)) {
        LightUniforms* light_uniform = &light_uniforms[light_idx++];
        if (!light_uniform->generates_shadows) continue;

        R_Light* light = Component_GetLight(*light_id);
        ASSERT(light->scene_id == scene->id);
        int layer = shadow_map_write_indices[light->desc.type]++;
        ASSERT(layer == light_uniform->shadow_map_idx); // should match
        if (light->desc.type != SG_LightType_Spot
            && light->desc.type != SG_LightType_Directional)
            continue; // others not impl

        // update frame uniform buffer
        light_frame_uniforms.projection = light->projection(false);
        light_frame_uniforms.view       = R_Transform::viewMatrix(light);
        light_frame_uniforms.projection_view_inverse_no_translation
          = glm::inverse(light_frame_uniforms.projection
                         * glm::mat4(glm::mat3(light_frame_uniforms.view)));
        light_frame_uniforms.camera_pos = light->_pos;

        // 0 num lights to save compute and not recalculate shadows in a shadow pass
        // ==api== in future can add mesh.depthMaterial() which bypasses frag shader or
        // just does simple alpha test
        light_frame_uniforms.num_lights = 0;

        wgpuQueueWriteBuffer(gctx->queue, light->frame_uniform_buffer, 0,
                             &light_frame_uniforms, sizeof(light_frame_uniforms));

        // resize the per-draw storage buffer
        size_t shadow_renderlist_count = hashmap_count(light->shadow_render_id_set);
        int draw_uniform_size          = NEXT_MULT(sizeof(DrawUniforms),
                                                   gctx->limits.minStorageBufferOffsetAlignment);
        if (light->draw_storage_buffer == NULL
            || wgpuBufferGetSize(light->draw_storage_buffer)
                 < shadow_renderlist_count * draw_uniform_size) {

            snprintf(string_buf, sizeof(string_buf),
                     "Shadow Pass DrawUniforms for Scene[%d:%s], Light[%d:%s]",
                     scene->id, scene->name, light->id, light->name);
            WGPUBufferDescriptor buff_desc = {};
            buff_desc.label                = string_buf;
            buff_desc.usage = WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst;
            buff_desc.size  = shadow_renderlist_count * draw_uniform_size;

            WGPU_RELEASE_RESOURCE(Buffer, light->draw_storage_buffer);
            light->draw_storage_buffer
              = wgpuDeviceCreateBuffer(gctx->device, &buff_desc);
            ASSERT(light->draw_storage_buffer);
        }

        // write draw uniforms cpu --> gpu
        u8* cpu_draw_uniform_list = (u8*)Arena::push(
          &gctx->frame_arena, shadow_renderlist_count * draw_uniform_size);
        u64 cpu_draw_uniform_offset
          = Arena::offsetOf(&gctx->frame_arena, cpu_draw_uniform_list);
        defer(
          wgpuQueueWriteBuffer(gctx->queue, light->draw_storage_buffer, 0,
                               Arena::get(&gctx->frame_arena, cpu_draw_uniform_offset),
                               shadow_renderlist_count * draw_uniform_size););

        // set up the renderpass
        snprintf(string_buf, sizeof(string_buf),
                 "Shadow Pass for Scene[%d:%s] Light[%d:%s]", scene->id, scene->name,
                 light->id, light->name);
        graph->addRenderPass(string_buf);

        if (light->desc.type == SG_LightType_Spot) {
            graph->renderPassDepthTarget(scene->spot_shadow_map_array, layer, 1);
            graph->renderPassColorTarget(scene->spot_shadow_color_map_array, 0, layer);
        } else if (light->desc.type == SG_LightType_Directional) {
            graph->renderPassDepthTarget(scene->dir_shadow_map_array, layer, 1);
            graph->renderPassColorTarget(scene->dir_shadow_color_map_array, 0, layer);
        } else {
            UNREACHABLE;
        }

        // TODO: test with WGPUStoreOp_Discard see if it still writes to depth tex
        graph->renderPassColorOp(WGPUColor{ 0.0f, 0.0f, 0.0f, 1.0f }, WGPULoadOp_Clear,
                                 WGPUStoreOp_Store);

        G_DrawCallListID dc_list = graph->renderPassAddDrawCallList();

        size_t shadowmap_renderlist_idx_DONT_USE = 0;
        SG_ID* mesh_id                           = NULL;
        int draw_uniform_idx                     = 0;
        // iterate over light's shadowcaster renderlist
        while (hashmap_iter(light->shadow_render_id_set,
                            &shadowmap_renderlist_idx_DONT_USE, (void**)&mesh_id)) {
            R_Transform* mesh = Component_GetMesh(*mesh_id);
            ASSERT(mesh->_stale == R_Transform_STALE_NONE);

            // skip over the meshes that no longer belong to this scene
            // ==optimize== remove these meshes from the ID set
            bool mesh_belongs_to_scene = (mesh->scene_id == scene->id);
            if (!mesh_belongs_to_scene) continue;

            // below is mostly copied from _R_RenderScene(...)
            R_Material* material = Component_GetMaterial(mesh->_matID);
            R_Geometry* geo      = Component_GetGeometry(mesh->_geoID);
            R_Shader* shader     = Component_GetShader(material->pso.sg_shader_id);
            if (!material || !geo || !shader) continue; // incomplete mesh

            // add to draw call list
            G_DrawCall* d = graph->addDraw(dc_list);
            float dist_from_camera
              = 0.0; // ==optimize== sort opaque geometry front-to-back
            d->sort_key = G_SortKey::create(false, G_RenderingLayer_World, material->id,
                                            dist_from_camera, 1.0);

            { // set bindgroup state
                // @group(0)
                R_BindFrameUniforms(light->frame_uniform_buffer, gctx, d, graph, shader,
                                    scene, true);

                // @group(1)
                // ==optimize== only run fragment shader if alpha-test discard on
                // material is true
                R_Material::createBindGroupEntries(material, 1, graph, d, gctx);

                // @group(2)
                DrawUniforms* draw_uniform
                  = (DrawUniforms*)(cpu_draw_uniform_list
                                    + draw_uniform_idx * draw_uniform_size);
                *draw_uniform
                  = { mesh->world, mesh->normal, mesh->id, mesh->receives_shadows, {} };
                graph->bindBuffer(d, PER_DRAW_GROUP, 0, light->draw_storage_buffer,
                                  draw_uniform_idx * draw_uniform_size,
                                  draw_uniform_size);

                ++draw_uniform_idx;

                // @group(3) pulled vertex attribs
                R_Geometry::addPullBindGroupEntries(geo, graph, d);

                // ==optimize== after moving to dynamic bg offsets and sharing a single
                // G_DynamicBuffer across all drawcalls for a frame, we will only need
                // to upload the per-draw uniform data *once* per mesh, rather than once
                // per mesh per pass that it appears in
            }

            { // set vertex info
                d->instance_count
                  = 1; // shadow passes currently don't support instanced draws

                // populate index buffer
                d->index_count    = R_Geometry::indexCount(geo);
                bool indexed_draw = (d->index_count > 0);
                if (indexed_draw) {
                    d->index_buffer        = geo->gpu_index_buffer.buf;
                    d->index_buffer_offset = 0;
                    d->index_buffer_size   = geo->gpu_index_buffer.size;
                } else {
                    // TODO come up with a better way to set a custom number of
                    // vertices to draw having -1 actually mean ALL is confusing 2
                    // different states.
                    u32 vertex_count                = R_Geometry::vertexCount(geo);
                    bool user_provided_vertex_count = geo->vertex_count >= 0;
                    d->vertex_count
                      = user_provided_vertex_count ? geo->vertex_count : vertex_count;
                }

                // set vertex attributes
                for (int vertex_slot = 0;
                     vertex_slot < ARRAY_LENGTH(geo->gpu_vertex_buffers);
                     ++vertex_slot) {
                    GPU_Buffer* gpu_buffer = &geo->gpu_vertex_buffers[vertex_slot];
                    if (gpu_buffer->buf && gpu_buffer->size > 0)
                        graph->vertexBuffer(d, vertex_slot, gpu_buffer->buf, 0,
                                            gpu_buffer->size);
                }
            }

            // set pso
            // ==optimize== create duplicate shader that only has vertex shader, no
            // fragment
            d->pipelineDesc(material->pso.sg_shader_id, material->pso.cull_mode,
                            material->pso.primitive_topology, false, true);
        }
    }

    // validation
    for (int i = 0; i < ARRAY_LENGTH(shadow_map_write_indices); ++i) {
        ASSERT(shadow_map_counts_by_type[i] == shadow_map_write_indices[i]);
    }
}

void R_Scene::registerMesh(R_Scene* scene, R_Transform* mesh)
{
    if (!scene || !mesh) return;
    if (mesh->_geoID == 0 || mesh->_matID == 0) return;

    ASSERT(mesh->type == SG_COMPONENT_MESH);
    GeometryToXforms::addXform(R_Scene_getPrimitive(scene, mesh->_matID, mesh->_geoID),
                               mesh->id);
}

void R_Scene::unregisterMesh(R_Scene* scene, R_Transform* mesh)
{
    if (!scene || !mesh) return;
    if (mesh->_geoID == 0 || mesh->_matID == 0) return;

    // get xforms from geometry
    GeometryToXforms* g2x = R_Scene_getPrimitive(scene, mesh->_matID, mesh->_geoID);

    ASSERT(GeometryToXforms::hasXform(g2x, mesh->id));

    // remove xform from primitive set
    GeometryToXforms::removeXform(g2x, mesh->id);

    // delete primitive if empty
    if (GeometryToXforms::count(g2x) == 0) {
        const void* removed = hashmap_delete(scene->geo_to_xform, &g2x->key);
        ASSERT(removed);
        // TODO: hashmap bug, doesn't call elfree on removed item. do it ourselves
        GeometryToXforms::free((void*)removed);
    }
}

void R_Scene::markPrimitiveStale(R_Scene* scene, R_Transform* mesh)
{
    if (!scene || !mesh) return;
    GeometryToXforms* g2x = R_Scene_getPrimitive(scene, mesh->_matID, mesh->_geoID);
    g2x->buffer_stale     = true;
}

int R_Scene::numPrimitives(R_Scene* scene, SG_ID material_id, SG_ID geo_id)
{
    GeometryToXforms* g2x = R_Scene_getPrimitive(scene, material_id, geo_id);
    return GeometryToXforms::count(g2x);
}

void R_Scene::initFromSG(GraphicsContext* gctx, R_Scene* r_scene, SG_ID scene_id,
                         SG_SceneDesc* sg_scene_desc)
{
    ASSERT(r_scene->id == 0); // ensure not initialized twice
    *r_scene = {};

    // copy base component data
    // TODO have a separate R_ComponentType enum?
    r_scene->id   = scene_id;
    r_scene->type = SG_COMPONENT_SCENE;

    // copy xform
    r_scene->_pos = VEC_ORIGIN;
    r_scene->_rot = QUAT_IDENTITY;
    r_scene->_sca = VEC_ONES;

    // set stale to force rebuild of matrices
    r_scene->_stale = R_Transform_STALE_LOCAL;

    // copy scene desc
    r_scene->sg_scene_desc = *sg_scene_desc;

    // initialize arenas
    int seed = time(NULL);

    r_scene->geo_to_xform
      = hashmap_new(sizeof(GeometryToXforms), 0, seed, seed, GeometryToXforms::hash,
                    GeometryToXforms::compare, GeometryToXforms::free, NULL);

    r_scene->light_id_set
      = hashmap_new(sizeof(SG_ID), 0, seed, seed, hashSGID, compareSGIDs, NULL, NULL);

    GPU_Buffer::init(gctx, &r_scene->light_info_buffer, WGPUBufferUsage_Storage,
                     sizeof(LightUniforms) * 16);

    // initialize children array for 8 children
    Arena::init(&r_scene->children, sizeof(SG_ID) * 8);
}

// ============================================================================
// Component Manager Definitions
// ============================================================================

// storage arenas
static Arena xformArena;
static Arena sceneArena;
static Arena geoArena;
static Arena shaderArena;
static Arena materialArena;
static Arena textureArena;
static Arena passArena;
static Arena bufferArena;
static Arena cameraArena;
static Arena textArena;
static Arena lightArena;
static Arena videoArena;
static Arena webcamArena;

// maps from id --> offset
static hashmap* r_locator = NULL;

// fonts
// each font is 600bytes, 128 fonts is 76.8KB
static R_Font component_fonts[128];
static int component_font_count = 0;

// webcam
/*
set user data to be the device id
have a stack lock-protected array of 8 webcam data slots
in callback, webcam thread locks the device id of its bucket and copies rgb data,
setting has_frame = true

in app, before rendering, check all buckets for has_frame = true and copy data to
texture
*/

struct R_WebcamData {
    sr_webcam_device* webcam;
    u8* data;
    size_t size;
    int count;           // number of ChuGL webcam objects using this device id
    u64 frame_count;     // incremented every time the webcam writes a new frame
    bool capture = true; // whether to capture frames
    spinlock lock;
};

// stores webcam pixel data, device id is the key
static R_WebcamData _r_webcam_data[8] = {}; // supports up to 8 webcams

struct R_Location {
    SG_ID id;     // key
    u64 offset;   // value (byte offset into arena)
    Arena* arena; // where to find
};

static int R_CompareLocation(const void* a, const void* b, void* udata)
{
    R_Location* locA = (R_Location*)a;
    R_Location* locB = (R_Location*)b;
    return locA->id - locB->id;
}

static u64 R_HashLocation(const void* item, uint64_t seed0, uint64_t seed1)
{
    // tested xxhash3 is best
    R_Location* key = (R_Location*)item;
    return hashmap_xxhash3(&key->id, sizeof(SG_ID), seed0, seed1);
    // return hashmap_sip(item, sizeof(int), seed0, seed1);
    // return hashmap_murmur(item, sizeof(int), seed0, seed1);
}

void Component_Init(GraphicsContext* gctx)
{
    // initialize arena memory
    Arena::init(&xformArena, sizeof(R_Transform) * 128);
    Arena::init(&sceneArena, sizeof(R_Scene) * 128);
    Arena::init(&geoArena, sizeof(R_Geometry) * 128);
    Arena::init(&shaderArena, sizeof(R_Shader) * 64);
    Arena::init(&materialArena, sizeof(R_Material) * 64);
    Arena::init(&textureArena, sizeof(R_Texture) * 64);
    Arena::init(&cameraArena, sizeof(R_Camera) * 4);
    Arena::init(&textArena, sizeof(R_Text) * 64);
    Arena::init(&passArena, sizeof(R_Pass) * 16);
    Arena::init(&bufferArena, sizeof(R_Buffer) * 64);
    Arena::init(&lightArena, sizeof(R_Light) * 16);
    Arena::init(&videoArena, sizeof(R_Video) * 16);
    Arena::init(&webcamArena, sizeof(R_Webcam) * 8);

    // init locator
    int seed = time(NULL);
    srand(seed);
    r_locator = hashmap_new(sizeof(R_Location), 0, seed, seed, R_HashLocation,
                            R_CompareLocation, NULL, NULL);
}

void Component_Free()
{
    // TODO: should we also free the individual components?

    // free arena memory
    Arena::free(&xformArena);
    Arena::free(&sceneArena);
    Arena::free(&geoArena);
    Arena::free(&shaderArena);
    Arena::free(&materialArena);
    Arena::free(&textureArena);
    Arena::free(&cameraArena);
    Arena::free(&textArena);
    Arena::free(&passArena);
    // Arena::free(&bufferArena);

    // free locator
    hashmap_free(r_locator);
    r_locator = NULL;

    // free webcam (doesn't crash)
    for (int i = 0; i < ARRAY_LENGTH(_r_webcam_data); i++) {
        if (_r_webcam_data[i].webcam) {
            log_info("Closing webcam device %d", i);
            sr_webcam_delete(_r_webcam_data[i].webcam);
            _r_webcam_data[i].webcam = NULL;
        }
    }
}

// analgous to audio thread's `_SG_ComponentManagerFree`
// frees resources within the locator hashmap and R_Component arenas
// sectioned off as a separate function to prevent any memory errors
// `component_size` is size in bytes
static void _Component_FreeComponent(SG_ID id, int component_size)
{
    // remove from shader arena (via swap-delete with last element)
    R_Location* result = (R_Location*)hashmap_get(r_locator, &id);
    ASSERT(result);
    ASSERT(result->offset % component_size
           == 0); // offset should be a multiple of struct size
    ASSERT(result->arena->curr % component_size
           == 0); // arena size should be multiple of component size
    ASSERT(result->arena->curr >= component_size); // nonempty
    ASSERT(result->id == id);

    // int component_idx = result->offset / component_size;
    memcpy(Arena::get(result->arena, result->offset),
           Arena::get(result->arena, result->arena->curr - component_size),
           component_size);
    Arena::pop(result->arena, component_size);

    // IMPORTANT: change offset of the newly swapped arena item
    SG_ID swapped_component_id
      = ((R_Component*)Arena::get(result->arena, result->offset))->id;
    R_Location* swapped_component_location
      = (R_Location*)hashmap_get(r_locator, &swapped_component_id);
    ASSERT(swapped_component_location);
    ASSERT(swapped_component_location->id == swapped_component_id);
    ASSERT(swapped_component_location->arena == result->arena);
    swapped_component_location->offset = result->offset;

    // delete old item from locator
    const void* delete_result = hashmap_delete(r_locator, &id);
    UNUSED_VAR(delete_result);
    ASSERT(delete_result);
}

// component garbage collection
void Component_FreeComponent(SG_ID id)
{
    R_Component* comp = Component_GetComponent(id);
    if (!comp) return; // already freed

    switch (comp->type) {
        case SG_COMPONENT_SHADER: {
            log_trace("graphics thread freeing shader %d", comp->id);
            R_Shader::free((R_Shader*)comp);
            _Component_FreeComponent(id, sizeof(R_Shader));
        } break;
        default: {
            // other types not yet supported
        }
    }
}

R_Transform* Component_CreateTransform()
{
    R_Transform* xform = ARENA_PUSH_ZERO_TYPE(&xformArena, R_Transform);
    R_Transform::init(xform);

    ASSERT(xform->id != 0);                        // ensure id is set
    ASSERT(xform->type == SG_COMPONENT_TRANSFORM); // ensure type is set

    // store offset
    R_Location loc = { xform->id, Arena::offsetOf(&xformArena, xform), &xformArena };
    const void* result = hashmap_set(r_locator, &loc);
    ASSERT(result == NULL); // ensure id is unique
    UNUSED_VAR(result);     // ensure id is unique

    return xform;
}

R_Transform* Component_CreateTransform(SG_Command_CreateXform* cmd)
{
    R_Transform* xform = ARENA_PUSH_ZERO_TYPE(&xformArena, R_Transform);
    R_Transform::initFromSG(xform, cmd);

    ASSERT(xform->id != 0);                        // ensure id is set
    ASSERT(xform->type == SG_COMPONENT_TRANSFORM); // ensure type is set

    // store offset
    R_Location loc = { xform->id, Arena::offsetOf(&xformArena, xform), &xformArena };
    const void* result = hashmap_set(r_locator, &loc);
    ASSERT(result == NULL); // ensure id is unique
    UNUSED_VAR(result);

    return xform;
}

R_Transform* Component_CreateMesh(SG_ID mesh_id, SG_ID geo_id, SG_ID mat_id)
{
    R_Transform* xform = ARENA_PUSH_ZERO_TYPE(&xformArena, R_Transform);

    R_Transform_init(xform, mesh_id, SG_COMPONENT_MESH);

    // init mesh
    xform->_geoID = geo_id;
    xform->_matID = mat_id;

    // store offset
    R_Location loc = { xform->id, Arena::offsetOf(&xformArena, xform), &xformArena };
    const void* result = hashmap_set(r_locator, &loc);
    ASSERT(result == NULL); // ensure id is unique
    UNUSED_VAR(result);     // ensure id is unique

    return xform;
}

R_Camera* Component_CreateCamera(GraphicsContext* gctx, SG_Command_CameraCreate* cmd)
{
    R_Camera* cam = ARENA_PUSH_ZERO_TYPE(&cameraArena, R_Camera);

    R_Transform_init(cam, cmd->camera.id, SG_COMPONENT_CAMERA);

    // camera init
    cam->params = cmd->camera.params;

    // store offset
    R_Location loc     = { cam->id, Arena::offsetOf(&cameraArena, cam), &cameraArena };
    const void* result = hashmap_set(r_locator, &loc);
    ASSERT(result == NULL); // ensure id is unique
    UNUSED_VAR(result);

    return cam;
}

R_Text* Component_CreateText(GraphicsContext* gctx, FT_Library ft,
                             SG_Command_TextRebuild* cmd, R_Font* default_font)
{
    // see if text is already created
    R_Text* text = Component_GetText(cmd->text_id);
    if (text == NULL) { // else lazily create
        // OOP is creeping in here....
        // can factor these out into separate fns: createMaterialImpl,
        // createGeometryImpl, createMeshImpl

        // initialize material
        R_Material* mat = Component_GetMaterial(cmd->material_id);
        ASSERT(mat);

        // init geometry
        R_Geometry* geo = ARENA_PUSH_TYPE(&geoArena, R_Geometry);
        {
            *geo              = {};
            geo->id           = getNewRID();
            geo->type         = SG_COMPONENT_GEOMETRY;
            geo->vertex_count = -1; // -1 means draw all vertices

            // store offset
            R_Location loc = { geo->id, Arena::offsetOf(&geoArena, geo), &geoArena };
            const void* result = hashmap_set(r_locator, &loc);
            ASSERT(result == NULL); // ensure id is unique
            UNUSED_VAR(result);
        }

        // init text
        text = ARENA_PUSH_ZERO_TYPE(&textArena,
                                    R_Text); // can also add void* udata to R_Transform
                                             // to support these kinds of renderables
        {
            R_Transform_init(text, cmd->text_id,
                             SG_COMPONENT_MESH); // text or mesh type?

            // init mesh
            text->_geoID = geo->id;
            text->_matID = mat->id;

            // store offset
            R_Location loc
              = { text->id, Arena::offsetOf(&textArena, text), &textArena };
            const void* result = hashmap_set(r_locator, &loc);
            ASSERT(result == NULL); // ensure id is unique
            UNUSED_VAR(result);

            // make sure these are internal
            ASSERT(text->_geoID < 0);
        }
    }

    // build text
    text->text
      = std::string((const char*)CQ_ReadCommandGetOffset(cmd->text_str_offset));
    text->font_path
      = std::string((const char*)CQ_ReadCommandGetOffset(cmd->font_path_str_offset));
    text->vertical_spacing = cmd->vertical_spacing;
    text->control_points   = cmd->control_point;

    R_Font* font = Component_GetFont(gctx, ft, text->font_path.c_str());
    if (!font) font = default_font;
    R_Font::updateText(gctx, font, text);

    return text;
}

R_Scene* Component_CreateScene(GraphicsContext* gctx, SG_ID scene_id,
                               SG_SceneDesc* sg_scene_desc)
{
    Arena* arena     = &sceneArena;
    R_Scene* r_scene = ARENA_PUSH_ZERO_TYPE(arena, R_Scene);
    R_Scene::initFromSG(gctx, r_scene, scene_id, sg_scene_desc);

    ASSERT(r_scene->id != 0);                    // ensure id is set
    ASSERT(r_scene->type == SG_COMPONENT_SCENE); // ensure type is set

    // store offset
    R_Location loc     = { r_scene->id, Arena::offsetOf(arena, r_scene), arena };
    const void* result = hashmap_set(r_locator, &loc);
    ASSERT(result == NULL); // ensure id is unique
    UNUSED_VAR(result);

    return r_scene;
}

R_Geometry* Component_CreateGeometry()
{
    R_Geometry* geo = ARENA_PUSH_ZERO_TYPE(&geoArena, R_Geometry);
    R_Geometry::init(geo);

    ASSERT(geo->id != 0);                       // ensure id is set
    ASSERT(geo->type == SG_COMPONENT_GEOMETRY); // ensure type is set

    // store offset
    R_Location loc     = { geo->id, Arena::offsetOf(&geoArena, geo), &geoArena };
    const void* result = hashmap_set(r_locator, &loc);
    ASSERT(result == NULL); // ensure id is unique
    UNUSED_VAR(result);

    return geo;
}

R_Geometry* Component_CreateGeometry(GraphicsContext* gctx, SG_ID geo_id)
{
    R_Geometry* geo = ARENA_PUSH_ZERO_TYPE(&geoArena, R_Geometry);

    geo->id           = geo_id;
    geo->type         = SG_COMPONENT_GEOMETRY;
    geo->vertex_count = -1; // -1 means draw all vertices

    // for now not storing geo_type (cube, sphere, custom etc.)
    // we only store the GPU vertex data, and don't care about semantics

    // store offset
    R_Location loc     = { geo->id, Arena::offsetOf(&geoArena, geo), &geoArena };
    const void* result = hashmap_set(r_locator, &loc);
    ASSERT(result == NULL); // ensure id is unique
    UNUSED_VAR(result);

    return geo;
}

R_Shader* Component_CreateShader(GraphicsContext* gctx, SG_Command_ShaderCreate* cmd)
{
    R_Shader* shader = ARENA_PUSH_ZERO_TYPE(&shaderArena, R_Shader);

    shader->id   = cmd->sg_id;
    shader->type = SG_COMPONENT_SHADER;

    // shader member vars
    const char* vertex_string
      = (const char*)CQ_ReadCommandGetOffset(cmd->vertex_string_offset);
    const char* fragment_string
      = (const char*)CQ_ReadCommandGetOffset(cmd->fragment_string_offset);
    const char* vertex_filepath
      = (const char*)CQ_ReadCommandGetOffset(cmd->vertex_filepath_offset);
    const char* fragment_filepath
      = (const char*)CQ_ReadCommandGetOffset(cmd->fragment_filepath_offset);
    const char* compute_filepath
      = (const char*)CQ_ReadCommandGetOffset(cmd->compute_filepath_offset);
    const char* compute_string
      = (const char*)CQ_ReadCommandGetOffset(cmd->compute_string_offset);

    ASSERT(sizeof(cmd->vertex_layout) == sizeof(shader->vertex_layout));
    R_Shader::init(gctx, shader, vertex_string, vertex_filepath, fragment_string,
                   fragment_filepath, cmd->vertex_layout,
                   ARRAY_LENGTH(cmd->vertex_layout), compute_string, compute_filepath,
                   &cmd->includes);

    // store offset
    R_Location loc
      = { shader->id, Arena::offsetOf(&shaderArena, shader), &shaderArena };
    const void* result = hashmap_set(r_locator, &loc);
    ASSERT(result == NULL); // ensure id is unique
    UNUSED_VAR(result);

    return shader;
}

R_Material* Component_CreateMaterial(GraphicsContext* gctx,
                                     SG_Command_MaterialCreate* cmd)
{
    R_Material* mat = ARENA_PUSH_ZERO_TYPE(&materialArena, R_Material);

    // initialize
    {
        *mat      = {};
        mat->id   = cmd->sg_id;
        mat->type = SG_COMPONENT_MATERIAL;

        const int UNIFORM_OFFSET = MAX(gctx->limits.minUniformBufferOffsetAlignment,
                                       sizeof(SG_MaterialUniformData));

        // init uniform buffer
        char label[128] = {};
        snprintf(label, sizeof(label), "UniformBuffer for Material[%d:%s]", mat->id,
                 mat->name);

        WGPUBufferDescriptor desc = {};
        desc.size                 = UNIFORM_OFFSET * ARRAY_LENGTH(mat->bindings);
        desc.usage                = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
        mat->uniform_buffer       = wgpuDeviceCreateBuffer(gctx->device, &desc);

        mat->pso = cmd->pso;
    }

    // store offset
    R_Location loc = { mat->id, Arena::offsetOf(&materialArena, mat), &materialArena };
    const void* result = hashmap_set(r_locator, &loc);
    ASSERT(result == NULL); // ensure id is unique
    UNUSED_VAR(result);

    return mat;
}

R_Texture* Component_CreateTexture(GraphicsContext* gctx, SG_Command_TextureCreate* cmd,
                                   u32 framebuffer_width, u32 framebuffer_height)
{
    R_Texture* tex = ARENA_PUSH_TYPE(&textureArena, R_Texture);
    *tex           = {};

    // R_Component init
    tex->id   = cmd->sg_id;
    tex->type = SG_COMPONENT_TEXTURE;
    COPY_STRING(tex->name, cmd->name);

    // R_Texture init
    R_Texture::init(gctx, tex, &cmd->desc, framebuffer_width, framebuffer_height);

    // store offset
    R_Location loc = { tex->id, Arena::offsetOf(&textureArena, tex), &textureArena };
    const void* result = hashmap_set(r_locator, &loc);
    ASSERT(result == NULL); // ensure id is unique
    UNUSED_VAR(result);

    return tex;
}

R_Pass* Component_CreatePass(SG_ID pass_id, WGPUDevice device)
{
    Arena* arena = &passArena;
    R_Pass* pass = ARENA_PUSH_TYPE(arena, R_Pass);
    *pass        = {};

    // SG_Component init
    pass->id   = pass_id;
    pass->type = SG_COMPONENT_PASS;

    // init frame uniform buffer
    char label[64];
    snprintf(label, sizeof(label), "Pass[%d] Frame Uniform Buffer", pass_id);
    WGPUBufferDescriptor desc  = {};
    desc.label                 = label;
    desc.usage                 = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
    desc.size                  = NEXT_MULT4(sizeof(FrameUniforms));
    pass->frame_uniform_buffer = wgpuDeviceCreateBuffer(device, &desc);
    ASSERT(pass->frame_uniform_buffer);

    // store offset
    R_Location loc     = { pass->id, Arena::offsetOf(arena, pass), arena };
    const void* result = hashmap_set(r_locator, &loc);
    ASSERT(result == NULL); // ensure id is unique
    UNUSED_VAR(result);

    return pass;
}

R_Buffer* Component_CreateBuffer(SG_ID id)
{
    Arena* arena     = &bufferArena;
    R_Buffer* buffer = ARENA_PUSH_TYPE(arena, R_Buffer);
    *buffer          = {};

    // SG_Component init
    buffer->id   = id;
    buffer->type = SG_COMPONENT_BUFFER;

    // store offset
    R_Location loc     = { buffer->id, Arena::offsetOf(arena, buffer), arena };
    const void* result = hashmap_set(r_locator, &loc);
    ASSERT(result == NULL); // ensure id is unique
    UNUSED_VAR(result);

    return buffer;
}

R_Light* Component_CreateLight(SG_ID id, SG_LightDesc* desc, WGPUDevice device,
                               WGPULimits* limits)
{
    R_Light* light = ARENA_PUSH_ZERO_TYPE(&lightArena, R_Light);

    R_Transform_init(light, id, SG_COMPONENT_LIGHT);

    // light init
    light->desc = *desc;
    light->shadow_render_id_set
      = hashmap_new_simple(sizeof(SG_ID), hashSGID, compareSGIDs);
    light->draw_storage_buffer = NULL;

    // init frame uniform buffer
    char label[64];
    snprintf(label, sizeof(label), "Light[%d] Frame Uniform Buffer", id);
    WGPUBufferDescriptor buffer_desc = {};
    buffer_desc.label                = label;
    buffer_desc.usage           = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
    buffer_desc.size            = NEXT_MULT4(sizeof(FrameUniforms));
    light->frame_uniform_buffer = wgpuDeviceCreateBuffer(device, &buffer_desc);
    ASSERT(light->frame_uniform_buffer);

    // store offset
    R_Location loc = { light->id, Arena::offsetOf(&lightArena, light), &lightArena };
    const void* result = hashmap_set(r_locator, &loc);
    ASSERT(result == NULL); // ensure id is unique
    UNUSED_VAR(result);

    return light;
}

static void R_Video_OnVideo(plm_t* player, plm_frame_t* frame, void* video_id)
{
    R_Video* video = Component_GetVideo((intptr_t)video_id);

    // log_info("Video frame time %f", frame->time);

    // Hand the decoded data over to OpenGL. For the RGB texture mode, the
    // YCrCb->RGB conversion is done on the CPU.

    // TODO support YCrCb mode
    // if (self->texture_mode == APP_TEXTURE_MODE_YCRCB) {
    //     app_update_texture(self, GL_TEXTURE0, self->texture_y, &frame->y);
    //     app_update_texture(self, GL_TEXTURE1, self->texture_cb, &frame->cb);
    //     app_update_texture(self, GL_TEXTURE2, self->texture_cr, &frame->cr);
    // } else {

    R_Texture* video_texture_rgba = Component_GetTexture(video->video_texture_rgba_id);

    { // memory bounds check
        // check texture
        ASSERT(video_texture_rgba);
        ASSERT(video_texture_rgba->desc.width == frame->width);
        ASSERT(video_texture_rgba->desc.height == frame->height);
        ASSERT(video_texture_rgba->desc.depth == 1);
        ASSERT(!video_texture_rgba->desc.gen_mips);
        ASSERT(video_texture_rgba->desc.format
               == WGPUTextureFormat_RGBA8Unorm); // TODO might need to go srgb

        // check cpu pixel buffer size
        ASSERT(video->rgba_data_size == frame->width * frame->height * 4);
        ASSERT(video->rgba_data_OWNED);
    }

    // convert yCrCb to rgba
    plm_frame_to_rgba(frame, video->rgba_data_OWNED, frame->width * 4);

    SG_TextureWriteDesc write_desc = {};
    write_desc.width               = frame->width;
    write_desc.height              = frame->height;
    R_Texture::write(video->gctx, video_texture_rgba, &write_desc,
                     video->rgba_data_OWNED, video->rgba_data_size);
}

R_Video* Component_CreateVideo(GraphicsContext* gctx, SG_ID id, const char* filename,
                               SG_ID rgba_texture_id)
{
    Arena* arena   = &videoArena;
    R_Video* video = ARENA_PUSH_TYPE(arena, R_Video);
    *video         = {};

    // component init
    video->id   = id;
    video->type = SG_COMPONENT_VIDEO;
    strncpy(video->name, filename, sizeof(video->name));

    // store offset
    R_Location loc     = { video->id, Arena::offsetOf(arena, video), arena };
    const void* result = hashmap_set(r_locator, &loc);
    ASSERT(result == NULL); // ensure id is unique
    UNUSED_VAR(result);

    { // video init (TODO move to video Desc struct)
        video->gctx                  = gctx;
        video->video_texture_rgba_id = rgba_texture_id;

        video->plm = plm_create_with_filename(filename);

        // validation
        if (video->plm) {
            if (!plm_probe(video->plm, 5000 * 1024)) {
                // no streams found, destroy
                plm_destroy(video->plm);
                video->plm = NULL;
            }
        }

        if (video->plm) {
            // allocate pixel memory
            int num_pixels = plm_get_width(video->plm) * plm_get_height(video->plm);
            video->rgba_data_OWNED = ALLOCATE_BYTES(u8, num_pixels * 4);
            video->rgba_data_size  = num_pixels * 4;

            // memset to 256 for alpha
            memset(video->rgba_data_OWNED, 255, video->rgba_data_size);

            plm_set_video_decode_callback(video->plm, R_Video_OnVideo,
                                          (void*)(intptr_t)video->id);

            // don't process audio
            plm_set_audio_enabled(video->plm, FALSE);
        }
    }

    return video;
}

void R_Webcam::updateTexture(GraphicsContext* gctx, R_Webcam* webcam)
{
    if (webcam->freeze) return;

    R_WebcamData* webcam_data = &_r_webcam_data[webcam->device_id];
    R_Texture* texture        = Component_GetTexture(webcam->webcam_texture_id);
    ASSERT(texture);

    // lock
    spinlock::lock(&webcam_data->lock);
    defer(spinlock::unlock(&webcam_data->lock));

    // validate texture dimensions
    int webcam_width{}, webcam_height{};
    sr_webcam_get_dimensions(webcam_data->webcam, &webcam_width, &webcam_height);
    ASSERT(texture->desc.width == webcam_width);
    ASSERT(texture->desc.height == webcam_height);

    ASSERT(webcam->last_frame_count <= webcam_data->frame_count);

    // check if has_frame
    if (webcam_data->frame_count == webcam->last_frame_count) {
        return;
    }

    // write to texture
    SG_TextureWriteDesc write_desc = {};
    write_desc.width               = texture->desc.width;
    write_desc.height              = texture->desc.height;
    R_Texture::write(gctx, texture, &write_desc, webcam_data->data, webcam_data->size);

    // update frame count
    webcam->last_frame_count = webcam_data->frame_count;
}

void R_Webcam::update(SG_Command_WebcamUpdate* cmd)
{
    R_Webcam* webcam = Component_GetWebcam(cmd->webcam_id);
    if (!webcam) return;

    webcam->freeze = cmd->freeze;

    // update R_WebcamData
    R_WebcamData* webcam_data = &_r_webcam_data[webcam->device_id];
    spinlock::lock(&webcam_data->lock);
    webcam_data->capture = cmd->capture;
    spinlock::unlock(&webcam_data->lock);
}

// EXECUTED ON SEPARATE THREAD
static void R_Webcam_Callback(sr_webcam_device* device, void* data)
{
    // static u64 last_time{ stm_now() };
    // double time_sec = stm_sec(stm_laptime(&last_time));
    // log_trace("%f sec since last webcam update; framerate: %f", time_sec,
    //           1.0 / time_sec);

    int device_id             = (intptr_t)(sr_webcam_get_user(device));
    R_WebcamData* webcam_data = &_r_webcam_data[device_id];

    // lock
    spinlock::lock(&webcam_data->lock);
    defer(spinlock::unlock(&webcam_data->lock));

    // if capture disabled, early exit
    if (!webcam_data->capture) return;

    // copy data, scaling up size from rgb to rgba
    int data_size     = sr_webcam_get_format_size(device);
    webcam_data->size = data_size;
    webcam_data->data = (u8*)data;

    // bump frame count
    webcam_data->frame_count++;
}

R_Webcam* Component_CreateWebcam(SG_Command_WebcamCreate* cmd)
{
    // bounds check
    ASSERT(cmd->device_id < ARRAY_LENGTH(_r_webcam_data));

    Arena* arena     = &webcamArena;
    R_Webcam* webcam = ARENA_PUSH_TYPE(arena, R_Webcam);
    *webcam          = {};

    // component init
    webcam->id   = cmd->webcam_id;
    webcam->type = SG_COMPONENT_WEBCAM;

    // store offset
    R_Location loc     = { webcam->id, Arena::offsetOf(arena, webcam), arena };
    const void* result = hashmap_set(r_locator, &loc);
    ASSERT(result == NULL); // ensure id is unique
    UNUSED_VAR(result);

    { // webcam init
        webcam->device_id         = cmd->device_id;
        webcam->webcam_texture_id = cmd->webcam_texture_id;

        R_WebcamData* webcam_data = &_r_webcam_data[cmd->device_id];
        spinlock::lock(&webcam_data->lock);
        defer(spinlock::unlock(&webcam_data->lock));

        // support multiple ChuGL Webcams using the same device id
        if (webcam_data->webcam == NULL) {
            ASSERT(webcam_data->count == 0);
            sr_webcam_set_user(cmd->device, (void*)(intptr_t)cmd->device_id);
            sr_webcam_set_callback(cmd->device, R_Webcam_Callback);

            // Start. (already openned on audio thread in SG_CreateWebcam())
            sr_webcam_start(cmd->device);

            webcam_data->webcam = cmd->device;
        }

        // update webcam refcount
        webcam_data->count++;
    }

    return webcam;
}

// linear search by font path, lazily creates if not found
R_Font* Component_GetFont(GraphicsContext* gctx, FT_Library library,
                          const char* font_path)
{
    if (font_path == NULL || strlen(font_path) == 0) return NULL;

    for (int i = 0; i < component_font_count; ++i) {
        // this lookup won't work for loading fonts from memory
        // actually it will if we give builtin fonts special names
        if (component_fonts[i].font_path == font_path) {
            return &component_fonts[i];
        }
    }

    R_Font* font = &component_fonts[component_font_count];
    if (R_Font::init(gctx, library, font, font_path)) {
        component_font_count++;
        return font;
    }
    return NULL;
}

R_Component* Component_GetComponent(SG_ID id)
{
    R_Location loc     = { id, 0, NULL };
    R_Location* result = (R_Location*)hashmap_get(r_locator, &loc);
    R_Component* comp
      = result ? (R_Component*)Arena::get(result->arena, result->offset) : NULL;
    if (comp) {
        ASSERT(comp->id == id);
        ASSERT(result->id == id);
    }
    return comp;
}

WGPUSampler Component_GetSampler(GraphicsContext* gctx, SG_Sampler sampler)
{
    return Graphics_GetSampler(gctx, samplerConfigFromSGSampler(sampler));
}

R_Transform* Component_GetXform(SG_ID id)
{
    R_Component* comp = Component_GetComponent(id);
    if (comp) {
        ASSERT(comp->type == SG_COMPONENT_TRANSFORM || comp->type == SG_COMPONENT_SCENE
               || comp->type == SG_COMPONENT_MESH || comp->type == SG_COMPONENT_CAMERA
               || comp->type == SG_COMPONENT_LIGHT);
    }
    return (R_Transform*)comp;
}

R_Transform* Component_GetMesh(SG_ID id)
{
    R_Component* comp = Component_GetComponent(id);
    ASSERT(comp == NULL || comp->type == SG_COMPONENT_MESH);
    return (R_Transform*)comp;
}

R_Scene* Component_GetScene(SG_ID id)
{
    R_Component* comp = Component_GetComponent(id);
    ASSERT(comp == NULL || comp->type == SG_COMPONENT_SCENE);
    return (R_Scene*)comp;
}

R_Geometry* Component_GetGeometry(SG_ID id)
{
    R_Component* comp = Component_GetComponent(id);
    ASSERT(comp == NULL || comp->type == SG_COMPONENT_GEOMETRY);
    return (R_Geometry*)comp;
}

R_Shader* Component_GetShader(SG_ID id)
{
    R_Component* comp = Component_GetComponent(id);
    ASSERT(comp == NULL || comp->type == SG_COMPONENT_SHADER);
    return (R_Shader*)comp;
}

R_Material* Component_GetMaterial(SG_ID id)
{
    R_Component* comp = Component_GetComponent(id);
    ASSERT(comp == NULL || comp->type == SG_COMPONENT_MATERIAL);
    return (R_Material*)comp;
}

R_Texture* Component_GetTexture(SG_ID id)
{
    R_Component* comp = Component_GetComponent(id);
    ASSERT(comp == NULL || comp->type == SG_COMPONENT_TEXTURE);
    return (R_Texture*)comp;
}

R_Camera* Component_GetCamera(SG_ID id)
{
    R_Component* comp = Component_GetComponent(id);
    ASSERT(comp == NULL || comp->type == SG_COMPONENT_CAMERA);
    return (R_Camera*)comp;
}

R_Text* Component_GetText(SG_ID id)
{
    R_Component* comp = Component_GetComponent(id);
    ASSERT(comp == NULL || comp->type == SG_COMPONENT_MESH);
    return (R_Text*)comp;
}

R_Pass* Component_GetPass(SG_ID id)
{
    R_Component* comp = Component_GetComponent(id);
    ASSERT(comp == NULL || comp->type == SG_COMPONENT_PASS);
    return (R_Pass*)comp;
}

R_Buffer* Component_GetBuffer(SG_ID id)
{
    R_Component* comp = Component_GetComponent(id);
    ASSERT(comp == NULL || comp->type == SG_COMPONENT_BUFFER);
    return (R_Buffer*)comp;
}

R_Light* Component_GetLight(SG_ID id)
{
    R_Component* comp = Component_GetComponent(id);
    ASSERT(comp == NULL || comp->type == SG_COMPONENT_LIGHT);
    return (R_Light*)comp;
}

R_Video* Component_GetVideo(SG_ID id)
{
    R_Component* comp = Component_GetComponent(id);
    ASSERT(comp == NULL || comp->type == SG_COMPONENT_VIDEO);
    return (R_Video*)comp;
}

R_Webcam* Component_GetWebcam(SG_ID id)
{
    R_Component* comp = Component_GetComponent(id);
    ASSERT(comp == NULL || comp->type == SG_COMPONENT_WEBCAM);
    return (R_Webcam*)comp;
}

bool Component_MaterialIter(size_t* i, R_Material** material)
{
    if (*i >= ARENA_LENGTH(&materialArena, R_Material)) {
        *material = NULL;
        return false;
    }

    *material = ARENA_GET_TYPE(&materialArena, R_Material, *i);
    ++(*i);
    return true;
}

bool Component_VideoIter(size_t* i, R_Video** video)
{
    if (*i >= ARENA_LENGTH(&videoArena, R_Video)) {
        *video = NULL;
        return false;
    }

    *video = ARENA_GET_TYPE(&videoArena, R_Video, *i);
    ++(*i);
    return true;
}

bool Component_WebcamIter(size_t* i, R_Webcam** webcam)
{
    if (*i >= ARENA_LENGTH(&webcamArena, R_Webcam)) {
        *webcam = NULL;
        return false;
    }

    *webcam = ARENA_GET_TYPE(&webcamArena, R_Webcam, *i);
    ++(*i);
    return true;
}

// =============================================================================
// R_Shader
// =============================================================================

void R_Shader::init(GraphicsContext* gctx, R_Shader* shader, const char* vertex_string,
                    const char* vertex_filepath, const char* fragment_string,
                    const char* fragment_filepath, WGPUVertexFormat* vertex_layout,
                    int vertex_layout_count, const char* compute_string,
                    const char* compute_filepath, SG_ShaderIncludes* includes)
{
    shader->includes = *includes;

    char vertex_shader_label[32] = {};
    snprintf(vertex_shader_label, sizeof(vertex_shader_label), "vertex shader %d",
             (int)shader->id);
    char fragment_shader_label[32] = {};
    snprintf(fragment_shader_label, sizeof(fragment_shader_label), "fragment shader %d",
             (int)shader->id);

    if (vertex_string && strlen(vertex_string) > 0) {
        shader->vertex_shader_module
          = G_createShaderModule(gctx, vertex_string, vertex_shader_label);
    } else if (vertex_filepath && strlen(vertex_filepath) > 0) {
        // read entire file contents
        FileReadResult vertex_file = File_read(vertex_filepath, true);
        if (vertex_file.data_owned) {
            shader->vertex_shader_module = G_createShaderModule(
              gctx, (const char*)vertex_file.data_owned, vertex_shader_label);
            FREE(vertex_file.data_owned);
        } else {
            log_error("failed to read vertex shader file %s", vertex_filepath);
        }
    }

    if (fragment_string && strlen(fragment_string) > 0) {
        shader->fragment_shader_module
          = G_createShaderModule(gctx, fragment_string, fragment_shader_label);
    } else if (fragment_filepath && strlen(fragment_filepath) > 0) {
        // read entire file contents
        FileReadResult fragment_file = File_read(fragment_filepath, true);
        if (fragment_file.data_owned) {
            shader->fragment_shader_module = G_createShaderModule(
              gctx, (const char*)fragment_file.data_owned, fragment_shader_label);
            FREE(fragment_file.data_owned);
        } else {
            log_error("failed to read fragment shader file %s", fragment_filepath);
        }
    }

    // copy vertex layout
    ASSERT(sizeof(*shader->vertex_layout) == sizeof(*vertex_layout));
    memcpy(shader->vertex_layout, vertex_layout,
           sizeof(*vertex_layout) * vertex_layout_count);

    // compute shaders
    char compute_shader_label[32] = {};
    snprintf(compute_shader_label, sizeof(compute_shader_label), "compute shader %d",
             (int)shader->id);
    if (compute_string && strlen(compute_string) > 0) {
        shader->compute_shader_module
          = G_createShaderModule(gctx, compute_string, compute_shader_label);
    } else if (compute_filepath && strlen(compute_filepath) > 0) {
        // read entire file contents
        FileReadResult compute_file = File_read(compute_filepath, true);
        if (compute_file.data_owned) {
            shader->compute_shader_module = G_createShaderModule(
              gctx, (const char*)compute_file.data_owned, compute_shader_label);
            FREE(compute_file.data_owned);
        } else {
            log_error("failed to read compute shader file %s", compute_filepath);
        }
    }
}

void R_Shader::free(R_Shader* shader)
{
    WGPU_RELEASE_RESOURCE(ShaderModule, shader->vertex_shader_module);
    WGPU_RELEASE_RESOURCE(ShaderModule, shader->fragment_shader_module);
    WGPU_RELEASE_RESOURCE(ShaderModule, shader->compute_shader_module);
}

// =============================================================================
// R_Font
// =============================================================================

// Decodes the first Unicode code point from the null-terminated UTF-8 string *text and
// advances *text to point at the next code point. If the encoding is invalid, advances
// *text by one byte and returns 0. *text should not be empty, because it will be
// advanced past the null terminator.
static u32 R_Font_decodeCharcode(char** text)
{
    uint8_t first = static_cast<uint8_t>((*text)[0]);

    // Fast-path for ASCII.
    if (first < 128) {
        (*text)++;
        return static_cast<uint32_t>(first);
    }

    // This could probably be optimized a bit.
    uint32_t result;
    int size;
    if ((first & 0xE0) == 0xC0) { // 110xxxxx
        result = first & 0x1F;
        size   = 2;
    } else if ((first & 0xF0) == 0xE0) { // 1110xxxx
        result = first & 0x0F;
        size   = 3;
    } else if ((first & 0xF8) == 0xF0) { // 11110xxx
        result = first & 0x07;
        size   = 4;
    } else {
        // Invalid encoding.
        (*text)++;
        return 0;
    }

    for (int i = 1; i < size; i++) {
        uint8_t value = static_cast<uint8_t>((*text)[i]);
        // Invalid encoding (also catches a null terminator in the middle of a code
        // point).
        if ((value & 0xC0) != 0x80) { // 10xxxxxx
            (*text)++;
            return 0;
        }
        result = (result << 6) | (value & 0x3F);
    }

    (*text) += size;
    return result;
}

FT_Face R_Font_loadFace(FT_Library library, const char* filename)
{
    FT_Face face = NULL;

    FT_Error ftError = FT_New_Face(library, filename, 0, &face);
    if (ftError) {
        const char* ftErrorStr = FT_Error_String(ftError);
        log_error("Error loading font face [error number %d] for file %s: %s", ftError,
                  filename, ftErrorStr);
        return NULL;
    }

    if (!(face->face_flags & FT_FACE_FLAG_SCALABLE)) {
        log_error("non-scalable fonts are not supported. Font file: %s", filename);
        FT_Done_Face(face);
        return NULL;
    }

    return face;
}

void R_Font::rebuildVertexBuffers(R_Font* font, const char* mainText, float x, float y,
                                  Arena* positions, Arena* uvs, Arena* glyph_indices,
                                  Arena* indices, float verticalScale)
{
    float originalX = x;

    FT_UInt previous = 0;
    char* textIt     = (char*)mainText;
    while (*textIt != '\0') {
        uint32_t charcode = R_Font_decodeCharcode(&textIt);

        if (charcode == '\r') continue;

        if (charcode == '\n') {
            x = originalX;
            y -= verticalScale
                 * ((float)font->face->height / (float)font->face->units_per_EM
                    * font->worldSize);
            continue;
        }

        auto glyphIt = font->glyphs.find(charcode);
        Glyph& glyph
          = (glyphIt == font->glyphs.end()) ? font->glyphs[0] : glyphIt->second;

        if (previous != 0 && glyph.index != 0) {
            FT_Vector kerning;
            FT_Error error = FT_Get_Kerning(font->face, previous, glyph.index,
                                            font->kerningMode, &kerning);
            if (!error) {
                x += (float)kerning.x / font->emSize * font->worldSize;
            }
        }

        // Do not emit quad for empty glyphs (whitespace).
        if (glyph.curveCount) {
            FT_Pos d = (FT_Pos)(font->emSize * font->dilation);

            float u0 = (float)(glyph.bearingX - d) / font->emSize;
            float v0 = (float)(glyph.bearingY - glyph.height - d) / font->emSize;
            float u1 = (float)(glyph.bearingX + glyph.width + d) / font->emSize;
            float v1 = (float)(glyph.bearingY + d) / font->emSize;

            float x0 = x + u0 * font->worldSize;
            float y0 = y + v0 * font->worldSize;
            float x1 = x + u1 * font->worldSize;
            float y1 = y + v1 * font->worldSize;

            u32 base                               = ARENA_LENGTH(positions, glm::vec2);
            *ARENA_PUSH_TYPE(positions, glm::vec2) = glm::vec2(x0, y0);
            *ARENA_PUSH_TYPE(positions, glm::vec2) = glm::vec2(x1, y0);
            *ARENA_PUSH_TYPE(positions, glm::vec2) = glm::vec2(x1, y1);
            *ARENA_PUSH_TYPE(positions, glm::vec2) = glm::vec2(x0, y1);

            *ARENA_PUSH_TYPE(uvs, glm::vec2) = glm::vec2(u0, v0);
            *ARENA_PUSH_TYPE(uvs, glm::vec2) = glm::vec2(u1, v0);
            *ARENA_PUSH_TYPE(uvs, glm::vec2) = glm::vec2(u1, v1);
            *ARENA_PUSH_TYPE(uvs, glm::vec2) = glm::vec2(u0, v1);

            *ARENA_PUSH_TYPE(glyph_indices, u32) = glyph.bufferIndex;
            *ARENA_PUSH_TYPE(glyph_indices, u32) = glyph.bufferIndex;
            *ARENA_PUSH_TYPE(glyph_indices, u32) = glyph.bufferIndex;
            *ARENA_PUSH_TYPE(glyph_indices, u32) = glyph.bufferIndex;

            *ARENA_PUSH_TYPE(indices, u32) = base;
            *ARENA_PUSH_TYPE(indices, u32) = base + 1;
            *ARENA_PUSH_TYPE(indices, u32) = base + 2;
            *ARENA_PUSH_TYPE(indices, u32) = base + 2;
            *ARENA_PUSH_TYPE(indices, u32) = base + 3;
            *ARENA_PUSH_TYPE(indices, u32) = base;
        }

        x += (float)glyph.advance / font->emSize * font->worldSize;
        previous = glyph.index;
    }
}

// ==optimize== consolidate with R_Font::rebuildBuffers
// calculate vertices assuming control point 0,0, and determine BB at same time
// after calculating bb, apply translation to vertices.
BoundingBox R_Font::measure(float x, float y, const char* text, float verticalScale)
{
    BoundingBox bb = {};
    bb.minX        = +std::numeric_limits<float>::infinity();
    bb.minY        = +std::numeric_limits<float>::infinity();
    bb.maxX        = -std::numeric_limits<float>::infinity();
    bb.maxY        = -std::numeric_limits<float>::infinity();

    float originalX = x;

    FT_UInt previous = 0;
    char* textIt     = (char*)text;
    while (*textIt != '\0') {
        uint32_t charcode = R_Font_decodeCharcode(&textIt);

        if (charcode == '\r') continue;

        if (charcode == '\n') {
            x = originalX;
            y -= verticalScale
                 * ((float)face->height / (float)face->units_per_EM * worldSize);
            continue;
        }

        auto glyphIt = glyphs.find(charcode);
        Glyph& glyph = (glyphIt == glyphs.end()) ? glyphs[0] : glyphIt->second;

        if (previous != 0 && glyph.index != 0) {
            FT_Vector kerning;
            FT_Error error
              = FT_Get_Kerning(face, previous, glyph.index, kerningMode, &kerning);
            if (!error) {
                x += (float)kerning.x / emSize * worldSize;
            }
        }

        // Note: Do not apply dilation here, we want to calculate exact bounds.
        float u0 = (float)(glyph.bearingX) / emSize;
        float v0 = (float)(glyph.bearingY - glyph.height) / emSize;
        float u1 = (float)(glyph.bearingX + glyph.width) / emSize;
        float v1 = (float)(glyph.bearingY) / emSize;

        float x0 = x + u0 * worldSize;
        float y0 = y + v0 * worldSize;
        float x1 = x + u1 * worldSize;
        float y1 = y + v1 * worldSize;

        if (x0 < bb.minX) bb.minX = x0;
        if (y0 < bb.minY) bb.minY = y0;
        if (x1 > bb.maxX) bb.maxX = x1;
        if (y1 > bb.maxY) bb.maxY = y1;

        x += (float)glyph.advance / emSize * worldSize;
        previous = glyph.index;
    }

    return bb;
}

// This function takes a single contour (defined by firstIndex and
// lastIndex, both inclusive) from outline and converts it into individual
// quadratic bezier curves, which are added to the curves vector.
static void convertContour(std::vector<BufferCurve>& curves, const FT_Outline* outline,
                           short firstIndex, short lastIndex, float emSize)
{
    // See https://freetype.org/freetype2/docs/glyphs/glyphs-6.html
    // for a detailed description of the outline format.
    //
    // In short, a contour is a list of points describing line segments
    // and quadratic or cubic bezier curves that form a closed shape.
    //
    // TrueType fonts only contain quadratic bezier curves. OpenType fonts
    // may contain outline data in TrueType format or in Compact Font
    // Format, which also allows cubic beziers. However, in FreeType it is
    // (theoretically) possible to mix the two types of bezier curves, so
    // we handle both at the same time.
    //
    // Each point in the contour has a tag specifying its type
    // (FT_CURVE_TAG_ON, FT_CURVE_TAG_CONIC or FT_CURVE_TAG_CUBIC).
    // FT_CURVE_TAG_ON points sit exactly on the outline, whereas the
    // other types are control points for quadratic/conic bezier curves,
    // which in general do not sit exactly on the outline and are also
    // called off points.
    //
    // Some examples of the basic segments:
    // ON - ON ... line segment
    // ON - CONIC - ON ... quadratic bezier curve
    // ON - CUBIC - CUBIC - ON ... cubic bezier curve
    //
    // Cubic bezier curves must always be described by two CUBIC points
    // inbetween two ON points. For the points used in the TrueType format
    // (ON, CONIC) there is a special rule, that two consecutive points of
    // the same type imply a virtual point of the opposite type at their
    // exact midpoint.
    //
    // For example the sequence ON - CONIC - CONIC - ON describes two
    // quadratic bezier curves where the virtual point forms the joining
    // end point of the two curves: ON - CONIC - [ON] - CONIC - ON.
    //
    // Similarly the sequence ON - ON can be thought of as a line segment
    // or a quadratic bezier curve (ON - [CONIC] - ON). Because the
    // virtual point is at the exact middle of the two endpoints, the
    // bezier curve is identical to the line segment.
    //
    // The font shader only supports quadratic bezier curves, so we use
    // this virtual point rule to represent line segments as quadratic
    // bezier curves.
    //
    // Cubic bezier curves are slightly more difficult, since they have a
    // higher degree than the shader supports. Each cubic curve is
    // approximated by two quadratic curves according to the following
    // paper. This preserves C1-continuity (location of and tangents at
    // the end points of the cubic curve) and the paper even proves that
    // splitting at the parametric center minimizes the error due to the
    // degree reduction. One could also analyze the approximation error
    // and split the cubic curve, if the error is too large. However,
    // almost all fonts use "nice" cubic curves, resulting in very small
    // errors already (see also the section on Font Design in the paper).
    //
    // Quadratic Approximation of Cubic Curves
    // Nghia Truong, Cem Yuksel, Larry Seiler
    // https://ttnghia.github.io/pdf/QuadraticApproximation.pdf
    // https://doi.org/10.1145/3406178

    if (firstIndex == lastIndex) return;

    short dIndex = 1;
    if (outline->flags & FT_OUTLINE_REVERSE_FILL) {
        short tmpIndex = lastIndex;
        lastIndex      = firstIndex;
        firstIndex     = tmpIndex;
        dIndex         = -1;
    }

    auto convert = [emSize](const FT_Vector& v) {
        return glm::vec2((float)v.x / emSize, (float)v.y / emSize);
    };

    auto makeMidpoint
      = [](const glm::vec2& a, const glm::vec2& b) { return 0.5f * (a + b); };

    auto makeCurve = [](const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2) {
        BufferCurve result{};
        result.x0 = p0.x;
        result.y0 = p0.y;
        result.x1 = p1.x;
        result.y1 = p1.y;
        result.x2 = p2.x;
        result.y2 = p2.y;
        return result;
    };

    // Find a point that is on the curve and remove it from the list.
    glm::vec2 first;
    bool firstOnCurve = (outline->tags[firstIndex] & FT_CURVE_TAG_ON);
    if (firstOnCurve) {
        first = convert(outline->points[firstIndex]);
        firstIndex += dIndex;
    } else {
        bool lastOnCurve = (outline->tags[lastIndex] & FT_CURVE_TAG_ON);
        if (lastOnCurve) {
            first = convert(outline->points[lastIndex]);
            lastIndex -= dIndex;
        } else {
            first = makeMidpoint(convert(outline->points[firstIndex]),
                                 convert(outline->points[lastIndex]));
            // This is a virtual point, so we don't have to remove it.
        }
    }

    glm::vec2 start    = first;
    glm::vec2 control  = first;
    glm::vec2 previous = first;
    char previousTag   = FT_CURVE_TAG_ON;
    for (short index = firstIndex; index != lastIndex + dIndex; index += dIndex) {
        glm::vec2 current = convert(outline->points[index]);
        char currentTag   = FT_CURVE_TAG(outline->tags[index]);
        if (currentTag == FT_CURVE_TAG_CUBIC) {
            // No-op, wait for more points.
            control = previous;
        } else if (currentTag == FT_CURVE_TAG_ON) {
            if (previousTag == FT_CURVE_TAG_CUBIC) {
                glm::vec2& b0 = start;
                glm::vec2& b1 = control;
                glm::vec2& b2 = previous;
                glm::vec2& b3 = current;

                glm::vec2 c0 = b0 + 0.75f * (b1 - b0);
                glm::vec2 c1 = b3 + 0.75f * (b2 - b3);

                glm::vec2 d = makeMidpoint(c0, c1);

                curves.push_back(makeCurve(b0, c0, d));
                curves.push_back(makeCurve(d, c1, b3));
            } else if (previousTag == FT_CURVE_TAG_ON) {
                // Linear segment.
                curves.push_back(
                  makeCurve(previous, makeMidpoint(previous, current), current));
            } else {
                // Regular bezier curve.
                curves.push_back(makeCurve(start, previous, current));
            }
            start   = current;
            control = current;
        } else /* currentTag == FT_CURVE_TAG_CONIC */
        {
            if (previousTag == FT_CURVE_TAG_ON) {
                // No-op, wait for third point.
            } else {
                // Create virtual on point.
                glm::vec2 mid = makeMidpoint(previous, current);
                curves.push_back(makeCurve(start, previous, mid));
                start   = mid;
                control = mid;
            }
        }
        previous    = current;
        previousTag = currentTag;
    }

    // Close the contour.
    if (previousTag == FT_CURVE_TAG_CUBIC) {
        glm::vec2& b0 = start;
        glm::vec2& b1 = control;
        glm::vec2& b2 = previous;
        glm::vec2& b3 = first;

        glm::vec2 c0 = b0 + 0.75f * (b1 - b0);
        glm::vec2 c1 = b3 + 0.75f * (b2 - b3);

        glm::vec2 d = makeMidpoint(c0, c1);

        curves.push_back(makeCurve(b0, c0, d));
        curves.push_back(makeCurve(d, c1, b3));
    } else if (previousTag == FT_CURVE_TAG_ON) {
        // Linear segment.
        curves.push_back(makeCurve(previous, makeMidpoint(previous, first), first));
    } else {
        curves.push_back(makeCurve(start, previous, first));
    }
}

static void R_Font_buildGlyph(R_Font* font, u32 charcode, FT_UInt glyphIndex)
{
    BufferGlyph bufferGlyph{};
    bufferGlyph.start = (i32)font->bufferCurves.size();

    short start = 0;
    for (int i = 0; i < font->face->glyph->outline.n_contours; i++) {
        // Note: The end indices in face->glyph->outline.contours are inclusive.
        convertContour(font->bufferCurves, &font->face->glyph->outline, start,
                       font->face->glyph->outline.contours[i], font->emSize);
        start = font->face->glyph->outline.contours[i] + 1;
    }

    bufferGlyph.count = (i32)font->bufferCurves.size() - bufferGlyph.start;

    i32 bufferIndex = (i32)font->bufferGlyphs.size();
    font->bufferGlyphs.push_back(bufferGlyph);

    Glyph glyph{};
    glyph.index            = glyphIndex;
    glyph.bufferIndex      = bufferIndex;
    glyph.curveCount       = bufferGlyph.count;
    glyph.width            = font->face->glyph->metrics.width;
    glyph.height           = font->face->glyph->metrics.height;
    glyph.bearingX         = font->face->glyph->metrics.horiBearingX;
    glyph.bearingY         = font->face->glyph->metrics.horiBearingY;
    glyph.advance          = font->face->glyph->metrics.horiAdvance;
    font->glyphs[charcode] = glyph;
}

// updates gpu buffers with new glyph/curve data
static void R_Font_uploadBuffers(GraphicsContext* gctx, R_Font* font)
{
    // could only write new data (can calculate offset from bufferGlyphs.size - gpu
    // buffer curr size)
    // check limits->minStorageBufferOffsetAlignment first
    // but not needed as these are small buffers relative to PCIe bandwidth

    // make sure we only rewrite on getting new glyph data
    ASSERT(sizeof(BufferGlyph) * font->bufferGlyphs.size() > font->glyph_buffer.size);
    ASSERT(sizeof(BufferCurve) * font->bufferCurves.size() > font->curve_buffer.size);

    GPU_Buffer::write(gctx, &font->glyph_buffer, WGPUBufferUsage_Storage,
                      font->bufferGlyphs.data(),
                      sizeof(BufferGlyph) * font->bufferGlyphs.size());
    GPU_Buffer::write(gctx, &font->curve_buffer, WGPUBufferUsage_Storage,
                      font->bufferCurves.data(),
                      sizeof(BufferCurve) * font->bufferCurves.size());
}

// impl in imgui_draw.cpp
unsigned char* imgui_decompressBase85TTF(const char* compressed_ttf_data_base85,
                                         int* out_size);

bool R_Font::init(GraphicsContext* gctx, FT_Library library, R_Font* font,
                  const char* font_path)
{
    ASSERT(font->face == NULL);
    *font           = {}; // init defaults
    font->font_path = std::string(font_path);
    ASSERT(font->worldSize > 0.0f);

    log_debug("Creating new R_Font with font path: %s", font_path);

    // DEFAULT FONTS
    // if font path starts with chugl:
    // then it is a builtin font we load from memory
    if (strncmp(font_path, "chugl:", 6) == 0) {
        unsigned char* font_data = NULL;
        int font_memory_size     = 0;
        if (strncmp(&font_path[6], "cousine-regular", 15) == 0) {
            font_data = imgui_decompressBase85TTF(
              cousine_regular_compressed_data_base85, &font_memory_size);
            ASSERT(font_memory_size == 43912);
        } else if (strncmp(&font_path[6], "karla-regular", 13) == 0) {
            font_data = imgui_decompressBase85TTF(karla_regular_compressed_data_base85,
                                                  &font_memory_size);
            ASSERT(font_memory_size == 16848);
        } else if (strncmp(&font_path[6], "proggy-tiny", 11) == 0) {
            font_data = imgui_decompressBase85TTF(proggy_tiny_compressed_data_base85,
                                                  &font_memory_size);
            ASSERT(font_memory_size == 35656);
        } else if (strncmp(&font_path[6], "proggy-clean", 12) == 0) {
            font_data = imgui_decompressBase85TTF(proggy_clean_compressed_data_base85,
                                                  &font_memory_size);
            ASSERT(font_memory_size == 41208);
        }

        FT_Error error = FT_New_Memory_Face(library, (const FT_Byte*)font_data,
                                            font_memory_size, 0, &font->face);

        if (error) {
            log_error("error while loading font face from memory: %d", error);
            return false;
        }
    } else {
        font->face = R_Font_loadFace(library, font_path);
        if (!font->face) return false;
    }

    FT_Face face = font->face;

    font->loadFlags   = FT_LOAD_NO_SCALE | FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP;
    font->kerningMode = FT_KERNING_UNSCALED;
    font->emSize      = font->face->units_per_EM;

    { // build undefined glyph
        uint32_t charcode  = 0;
        FT_UInt glyphIndex = 0;
        FT_Error error     = FT_Load_Glyph(font->face, glyphIndex, font->loadFlags);
        if (error) {
            log_error("error while loading undefined glyph: %d", error);
            return false;
            // Continue, because we always want an entry for the undefined glyph in our
            // glyphs map!
        }

        R_Font_buildGlyph(font, charcode, glyphIndex);
    }

    // build glyphs for ASCII characters
    // 32-127 are printable ASCII characters
    for (uint32_t charcode = 32; charcode < 128; charcode++) {
        FT_UInt glyphIndex = FT_Get_Char_Index(face, charcode);
        if (!glyphIndex) continue;

        FT_Error error = FT_Load_Glyph(face, glyphIndex, font->loadFlags);
        if (error) {
            log_error("error while loading glyph for character %d: %d", charcode,
                      error);
            continue;
        }

        R_Font_buildGlyph(font, charcode, glyphIndex);
    }

    R_Font_uploadBuffers(gctx, font);
    font->font_path = std::string(font_path);
    return true;
}

void R_Font::updateText(GraphicsContext* gctx, R_Font* font, R_Text* text)
{
    static Arena positions;
    static Arena uvs;
    static Arena glyph_indices;
    static Arena indices;

    // clear the buffers
    Arena::clear(&positions);
    Arena::clear(&uvs);
    Arena::clear(&glyph_indices);
    Arena::clear(&indices);

    // generate new glyps for this font
    R_Font::prepareGlyphsForText(gctx, font, text->text.c_str());

    // compute new bounding box
    BoundingBox bb = font->measure(0, 0, text->text.c_str(), text->vertical_spacing);

    // update material bindgroup
    R_Material* mat = Component_GetMaterial(text->_matID);
    R_Material::setExternalStorageBinding(gctx, mat, 0, &font->glyph_buffer);
    R_Material::setExternalStorageBinding(gctx, mat, 1, &font->curve_buffer);

    float cx = bb.minX + text->control_points.x * (bb.maxX - bb.minX);
    float cy = bb.minY + text->control_points.y * (bb.maxY - bb.minY);

    R_Font::rebuildVertexBuffers(font, text->text.c_str(), -cx, -cy, &positions, &uvs,
                                 &glyph_indices, &indices, text->vertical_spacing);

    // write buffer data to geometry
    R_Geometry* geo = Component_GetGeometry(text->_geoID);
    R_Geometry::setVertexAttribute(gctx, geo, 0, 2, positions.base, positions.curr);
    R_Geometry::setVertexAttribute(gctx, geo, 1, 2, uvs.base, uvs.curr);
    R_Geometry::setVertexAttribute(gctx, geo, 2, 1, glyph_indices.base,
                                   glyph_indices.curr);
    R_Geometry::setIndices(gctx, geo, (u32*)indices.base, ARENA_LENGTH(&indices, u32));

    // set internal uniforms
    // recompute bb adjusted by control points
    BoundingBox adjust_bb = { bb.minX - cx, bb.minY - cy, bb.maxX - cx, bb.maxY - cy };
    R_Material::setUniformBinding(gctx, mat, 5, &adjust_bb, sizeof(adjust_bb));

    // leq because whitespaces are skipped
    ASSERT(ARENA_LENGTH(&indices, u32) <= text->text.length() * 6);
}

// build new glyphs if text has unseen characters
void R_Font::prepareGlyphsForText(GraphicsContext* gctx, R_Font* font, const char* text)
{
    bool changed = false;

    char* textIt = (char*)text;
    while (*textIt != '\0') {
        uint32_t charcode = R_Font_decodeCharcode(&textIt);

        if (charcode == '\r' || charcode == '\n') continue;
        if (font->glyphs.count(charcode) != 0) continue; // if already exists, move on

        FT_UInt glyphIndex = FT_Get_Char_Index(font->face, charcode);
        if (!glyphIndex) continue;

        FT_Error error = FT_Load_Glyph(font->face, glyphIndex, font->loadFlags);
        if (error) {
            log_error("error while loading glyph for character %d: %d", charcode,
                      error);
            continue;
        }

        R_Font_buildGlyph(font, charcode, glyphIndex);
        changed = true;
    }

    if (changed) {
        // Reupload the full buffer contents. To make this even more
        // dynamic, the buffers could be overallocated and only the added
        // data could be uploaded.
        // not necessary, glyph+curve buffers are only ~200kb for ASCII chars
        // 3080 PCI has 32GB/s bandwidth, these buffers are nothing
        R_Font_uploadBuffers(gctx, font);
    }
}

void R_BindFrameUniforms(WGPUBuffer frame_uniform_buffer, GraphicsContext* gctx,
                         G_DrawCall* d, G_Graph* graph, R_Shader* shader,
                         R_Scene* scene, bool is_shadow_pass)
{
    // group(0) must be bound if group(1) is (no holes in bindgroups allowed)
    // so for now we always bind the per-frame uniforms
    graph->bindBuffer(d, PER_FRAME_GROUP, 0, frame_uniform_buffer, 0,
                      sizeof(FrameUniforms));

    if (scene) {
        if (shader->includes.lit)
            graph->bindBuffer(d, PER_FRAME_GROUP, 1, scene->light_info_buffer.buf, 0,
                              MAX(scene->light_info_buffer.size, 1));

        if (shader->includes.uses_env_map) {
            R_Texture* envmap = Component_GetTexture(scene->sg_scene_desc.env_map_id);
            ASSERT(envmap && envmap->gpu_texture)
            graph->bindTexture(
              d, PER_FRAME_GROUP, 2,
              { envmap->gpu_texture, WGPUTextureViewDimension_Cube, 0, 1, 0, 6 });
        }

        if (shader->includes.shadows) {
            graph->bindSampler(d, PER_FRAME_GROUP, 3, gctx->shadow_comparison_sampler);

            // use sentinel shadow map in shadowpass because we cannot bind the
            // actual shadow map as both depth target and render attachment
            WGPUTexture spot_shadow_map_array
              = is_shadow_pass ? gctx->sentinel_spotlight_depth_2d_array :
                                 scene->spot_shadow_map_array;
            WGPUTexture dir_shadow_map_array
              = is_shadow_pass ? gctx->sentinel_dirlight_depth_2d_array :
                                 scene->dir_shadow_map_array;
            graph->bindTexture(
              d, PER_FRAME_GROUP, 4,
              { spot_shadow_map_array, WGPUTextureViewDimension_2DArray, 0, 1, 0,
                (int)wgpuTextureGetDepthOrArrayLayers(spot_shadow_map_array) });

            graph->bindTexture(
              d, PER_FRAME_GROUP, 5,
              { dir_shadow_map_array, WGPUTextureViewDimension_2DArray, 0, 1, 0,
                (int)wgpuTextureGetDepthOrArrayLayers(dir_shadow_map_array) });
        }
    }
}

void R_Light::shadowAddMesh(SG_ID* mesh_list, int mesh_count, bool add)
{
    if (add) {
        for (int i = 0; i < mesh_count; ++i) {
            R_Transform* x = Component_GetXform(mesh_list[i]);
            if (x->type == SG_COMPONENT_MESH) {
                hashmap_set(shadow_render_id_set, mesh_list + i);
            }
        }
    } else {
        for (int i = 0; i < mesh_count; ++i)
            hashmap_delete(shadow_render_id_set, mesh_list + i);
    }
}
