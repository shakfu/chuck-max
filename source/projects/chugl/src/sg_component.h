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
#pragma once

#include "graphics.h"

#include "core/macros.h"
#include "core/memory.h"

#include <chuck/chugin.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream> // std::string

#include <pl/pl_mpeg.h>

// forward decls
struct SG_Light;
struct SG_Camera;
struct SG_Material;

#define QUAT_IDENTITY (glm::quat(1.0, 0.0, 0.0, 0.0))
#define MAT_IDENTITY (glm::mat4(1.0))

#define VEC_ORIGIN (glm::vec3(0.0f, 0.0f, 0.0f))
#define VEC_UP (glm::vec3(0.0f, 1.0f, 0.0f))
#define VEC_DOWN (glm::vec3(0.0f, -1.0f, 0.0f))
#define VEC_LEFT (glm::vec3(-1.0f, 0.0f, 0.0f))
#define VEC_RIGHT (glm::vec3(1.0f, 0.0f, 0.0f))
#define VEC_FORWARD (glm::vec3(0.0f, 0.0f, -1.0f))
#define VEC_BACKWARD (glm::vec3(0.0f, 0.0f, 1.0f))
#define VEC_ONES (glm::vec3(1.0f, 1.0f, 1.0f))

// chugl will only ever use positive SG_IDs
// but making signed to allow renderer to use negative IDs for internal impl
// also needs to be a signed int for the compareSGIDs comparison function to work
typedef i32 SG_ID;

// (enum, ckname)
// TODO resolve GBuffer with StorageBuffer
#define SG_ComponentTable                                                              \
    X(SG_COMPONENT_INVALID = 0, "Invalid")                                             \
    X(SG_COMPONENT_BASE, "SG_Component")                                               \
    X(SG_COMPONENT_TRANSFORM, "GGen")                                                  \
    X(SG_COMPONENT_SCENE, "GScene")                                                    \
    X(SG_COMPONENT_GEOMETRY, "Geometry")                                               \
    X(SG_COMPONENT_SHADER, "Shader")                                                   \
    X(SG_COMPONENT_MATERIAL, "Material")                                               \
    X(SG_COMPONENT_TEXTURE, "Texture")                                                 \
    X(SG_COMPONENT_MESH, "GMesh")                                                      \
    X(SG_COMPONENT_CAMERA, "GCamera")                                                  \
    X(SG_COMPONENT_PASS, "GPass")                                                      \
    X(SG_COMPONENT_BUFFER, "GBuffer")                                                  \
    X(SG_COMPONENT_LIGHT, "GLight")                                                    \
    X(SG_COMPONENT_VIDEO, "Video")                                                     \
    X(SG_COMPONENT_WEBCAM, "Webcam")

enum SG_ComponentType {
#define X(name, str) name,
    SG_ComponentTable
#undef X
      SG_COMPONENT_COUNT
};

static const char* SG_CKNames[SG_COMPONENT_COUNT] = {
#define X(name, str) str,
    SG_ComponentTable
#undef X
};

struct SG_Component {
    SG_ID id;
    SG_ComponentType type;
    char name[64] = {};
    Chuck_Object* ckobj;
    // Chuck_Type* cktype;
    // TODO cache hash
    // u64 hash;
};

// ============================================================================
// SG_Buffer
// ============================================================================

struct SG_BufferDesc {
    WGPUBufferUsageFlags usage;
    u64 size; // size in bytes
};

struct SG_Buffer : public SG_Component {
    // ==optimize== wrap in SG_BufferDesc and pass that to CQ Command, rather than
    // entire SG_Buffer also unsafe to have ckobj pointer on graphics thread
    SG_BufferDesc desc;
};

// ============================================================================
// SG_Sampler
// ============================================================================

enum SG_Sampler_WrapMode : u8 {
    SG_SAMPLER_WRAP_REPEAT        = WGPUAddressMode_Repeat,
    SG_SAMPLER_WRAP_MIRROR_REPEAT = WGPUAddressMode_MirrorRepeat,
    SG_SAMPLER_WRAP_CLAMP_TO_EDGE = WGPUAddressMode_ClampToEdge,
};

enum SG_Sampler_FilterMode : u8 {
    SG_SAMPLER_FILTER_NEAREST = WGPUFilterMode_Nearest,
    SG_SAMPLER_FILTER_LINEAR  = WGPUFilterMode_Linear,
};

struct SG_Sampler {
    SG_Sampler_WrapMode wrapU, wrapV, wrapW;
    SG_Sampler_FilterMode filterMin, filterMag, filterMip;

    static SG_Sampler fromCkObj(Chuck_Object* ckobj);
};
static_assert(sizeof(SG_Sampler) == 6, "SG_Sampler size mismatch");

static SG_Sampler SG_SAMPLER_DEFAULT // make this a #define instead?
  = { SG_SAMPLER_WRAP_REPEAT,   SG_SAMPLER_WRAP_REPEAT,   SG_SAMPLER_WRAP_REPEAT,
      SG_SAMPLER_FILTER_LINEAR, SG_SAMPLER_FILTER_LINEAR, SG_SAMPLER_FILTER_LINEAR };

// ============================================================================
// SG_Texture
// ============================================================================

WGPUTextureUsageFlags WGPUTextureUsage_All
  = WGPUTextureUsage_CopyDst | WGPUTextureUsage_CopySrc
    | WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_TextureBinding
    | WGPUTextureUsage_StorageBinding;

enum SG_TextureResizeMode : u8 {
    SG_TextureResizeMode_Fixed = 0, // fixed width/height in pixels

    SG_TextureResizeMode_Ratio, // rescale texture proportional to final framebuffer
                                // dimensions
};

struct SG_TextureDesc {
    // for now default to ALL usage flags to simplify
    WGPUTextureUsageFlags usage      = WGPUTextureUsage_All;
    WGPUTextureDimension dimension   = WGPUTextureDimension_2D;
    WGPUTextureFormat format         = WGPUTextureFormat_RGBA8Unorm;
    SG_TextureResizeMode resize_mode = SG_TextureResizeMode_Fixed;
    f32 width_ratio                  = 1.0f;
    f32 height_ratio                 = 1.0f;
    int width                        = 1;
    int height                       = 1;
    int depth                        = 1;
    b32 gen_mips                     = 1L;
};

struct SG_TextureWriteDesc {
    // Image Location
    int mip;
    int offset_x;
    int offset_y;
    int offset_z;

    // write region size (in texels)
    int width  = 1;
    int height = 1;
    int depth  = 1;
};

struct SG_TextureLoadDesc {
    bool flip_y   = false;
    bool gen_mips = true;
};

struct SG_TextureLocation {
    int mip;
    int origin_x;
    int origin_y;
    int origin_z;

    static WGPUImageCopyTexture wgpuImageCopyTexture(SG_TextureLocation loc,
                                                     WGPUTexture texture)
    {
        WGPUImageCopyTexture img = {};
        img.texture              = texture;
        img.mipLevel             = loc.mip;
        img.origin = { (u32)loc.origin_x, (u32)loc.origin_y, (u32)loc.origin_z };
        return img;
    }
};

int SG_Texture_numComponentsPerTexel(WGPUTextureFormat format);
int SG_Texture_byteSizePerTexel(
  WGPUTextureFormat format); // size of a single texel in bytes

struct SG_Texture : SG_Component {
    SG_TextureDesc desc;

    // set by SG_COMMAND_G2A_TEXTURE_READ handler
    Chuck_ArrayFloat* texture_data;
    Chuck_Event* texture_read_event;

    static void updateTextureData(SG_Texture* texture, void* data, int data_size_bytes);
};

// ============================================================================
// SG_Transform
// ============================================================================

struct SG_Transform : public SG_Component {
    glm::vec3 pos;
    glm::quat rot;
    glm::vec3 sca;

    // relationships
    SG_ID parentID;
    Arena childrenIDs;
    SG_ID scene_id; // the scene this transform belongs to

    // TODO: come up with staleness scheme that makes sense for scenegraph

    // don't init directly. Use SG Component Manager instead
    static void _init(SG_Transform* t, Chuck_Object* ckobj);

    static void translate(SG_Transform* t, glm::vec3 delta);
    static void rotate(SG_Transform* t, glm::quat q);
    static void rotate(SG_Transform* t, glm::vec3 eulers);
    static void scale(SG_Transform* t, glm::vec3 s);

    static void rotateOnWorldAxis(SG_Transform* t, glm::vec3 axis, float rad);
    static void rotateOnLocalAxis(SG_Transform* t, glm::vec3 axis, float rad);
    static void rotateX(SG_Transform* t, float deg);
    static void rotateY(SG_Transform* t, float deg);
    static void rotateZ(SG_Transform* t, float deg);
    static void lookAt(SG_Transform* t, glm::vec3 pos, glm::vec3 up);

    static glm::vec3 eulerRotationRadians(SG_Transform* t);
    static glm::mat4 modelMatrix(SG_Transform* t);
    static glm::mat4 worldMatrix(SG_Transform* t);
    static glm::quat worldRotation(SG_Transform* t);
    static glm::vec3 worldPosition(SG_Transform* t);
    static glm::vec3 worldScale(SG_Transform* t);
    static void worldPosition(SG_Transform* t, glm::vec3 pos);
    static glm::vec3 localToWorldPosition(SG_Transform* t, glm::vec3 pos);
    static void worldScale(SG_Transform* t, glm::vec3 scale);
    static glm::vec3 right(SG_Transform* t);
    static glm::vec3 forward(SG_Transform* t);
    static glm::vec3 up(SG_Transform* t);

    // SceneGraph relationships ========================================
    static void addChild(SG_Transform* parent, SG_Transform* child);
    static void removeChild(SG_Transform* parent, SG_Transform* child);
    static void removeAllChildren(SG_Transform* parent);
    static bool isAncestor(SG_Transform* ancestor, SG_Transform* descendent);
    static size_t numChildren(SG_Transform* t);
    static SG_Transform* child(SG_Transform* t, size_t index);

    // disconnect from both parent and children
    // void Disconnect( bool sendChildrenToGrandparent = false );
};

// ============================================================================
// SG_Scene
// ============================================================================

struct SG_SceneDesc {
    glm::vec4 bg_color      = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    SG_ID main_camera_id    = 0;
    glm::vec3 ambient_light = glm::vec3(0.04f);

    // environment map
    SG_ID env_map_id;
    SG_Sampler env_map_sampler;
    SG_ID skybox_material_id;
};

struct SG_Scene : public SG_Transform {
    SG_SceneDesc desc;
    Arena light_ids;

    // bookkeeping for automatic update (to prevent multiple updates per frame)
    i64 last_auto_update_frame = 0;

    static void setMainCamera(SG_Scene* scene, SG_Camera* camera);

    static void addLight(SG_Scene* scene, SG_ID light_id);
    static void removeLight(SG_Scene* scene, SG_ID light_id);
    static SG_Light* getLight(SG_Scene* scene, u32 idx);

    // envmap methods
    static void setEnvMap(SG_Scene* scene, SG_Texture* env_map);
    static void setEnvMapSampler(SG_Scene* scene, SG_Sampler sampler);
    static void setSkyboxMaterial(SG_Scene* scene, SG_Material* skybox_material);
};

// ============================================================================
// SG_Geometry
// ============================================================================

#define SG_GeometryTable                                                               \
    X(SG_GEOMETRY = 0, "Geometry")                                                     \
    X(SG_GEOMETRY_PLANE, "PlaneGeometry")                                              \
    X(SG_GEOMETRY_CUBE, "CubeGeometry")                                                \
    X(SG_GEOMETRY_CIRCLE, "CircleGeometry")                                            \
    X(SG_GEOMETRY_SPHERE, "SphereGeometry")                                            \
    X(SG_GEOMETRY_CYLINDER, "CylinderGeometry")                                        \
    X(SG_GEOMETRY_TORUS, "TorusGeometry")                                              \
    X(SG_GEOMETRY_SUZANNE, "SuzanneGeometry")                                          \
    X(SG_GEOMETRY_KNOT, "KnotGeometry")                                                \
    X(SG_GEOMETRY_LINES2D, "LinesGeometry")                                            \
    X(SG_GEOMETRY_POLYGON, "PolygonGeometry")

enum SG_GeometryType {
#define X(name, str) name,
    SG_GeometryTable
#undef X
      SG_GEOMETRY_COUNT
};

static const char* SG_GeometryTypeNames[SG_GEOMETRY_COUNT] = {
#define X(name, str) str,
    SG_GeometryTable
#undef X
};

union SG_GeometryParams {
    PlaneParams plane;
    SphereParams sphere;
    BoxParams box;
    CircleParams circle;
    TorusParams torus;
    CylinderParams cylinder;
    KnotParams knot;
    PolygonParams polygon;
};

#define SG_GEOMETRY_MAX_VERTEX_ATTRIBUTES 8

#define SG_GEOMETRY_POSITION_ATTRIBUTE_LOCATION 0
#define SG_GEOMETRY_NORMAL_ATTRIBUTE_LOCATION 1
#define SG_GEOMETRY_UV_ATTRIBUTE_LOCATION 2

#define CHUGL_GEOMETRY_MAX_PULLED_VERTEX_BUFFERS 4

struct SG_Geometry : SG_Component {
    SG_GeometryType geo_type;
    SG_GeometryParams params;

    // buffers to hold vertex data
    Arena vertex_attribute_data[SG_GEOMETRY_MAX_VERTEX_ATTRIBUTES];
    int vertex_attribute_num_components[SG_GEOMETRY_MAX_VERTEX_ATTRIBUTES];
    Arena indices;

    // buffers to hold pull data
    Arena vertex_pull_buffers[CHUGL_GEOMETRY_MAX_PULLED_VERTEX_BUFFERS];

    static u32 vertexCount(SG_Geometry* geo);
    static u32 indexCount(SG_Geometry* geo);

    // data_len is length of data in floats, not bytes not components
    static Arena* setAttribute(SG_Geometry* geo, int location, int num_components,
                               CK_DL_API api, Chuck_Object* ck_array,
                               int ck_array_num_components, bool is_int);
    static u32* setIndices(SG_Geometry* geo, CK_DL_API API, Chuck_ArrayInt* indices,
                           int index_count);
    static u32* getIndices(SG_Geometry* geo);

    static f32* getAttributeData(SG_Geometry* geo, int location);

    // builder functions
    static void initGABandNumComponents(GeometryArenaBuilder* b, SG_Geometry* g,
                                        bool clear);
    static void buildPlane(SG_Geometry* g, PlaneParams* p);
    static void buildSphere(SG_Geometry* g, SphereParams* p);
    static void buildSuzanne(SG_Geometry* g);
    static void buildBox(SG_Geometry* g, BoxParams* p);
    static void buildCircle(SG_Geometry* g, CircleParams* p);
    static void buildTorus(SG_Geometry* g, TorusParams* p);
    static void buildCylinder(SG_Geometry* g, CylinderParams* p);
    static void buildKnot(SG_Geometry* g, KnotParams* p);
    static void buildPolygon(SG_Geometry* g, PolygonParams* p);
};

// ============================================================================
// SG_Shader
// ============================================================================

struct SG_ShaderIncludes {
    bool lit;          // if true, renderer will pass lighting storage buffer
    bool uses_env_map; // if true, renderer will pass env map texture
    bool shadows;      // if true, renderer will pass shadow params
};

struct SG_Shader : SG_Component {
    const char* vertex_string_owned;
    const char* fragment_string_owned;
    const char* vertex_filepath_owned;
    const char* fragment_filepath_owned;
    WGPUVertexFormat vertex_layout[SG_GEOMETRY_MAX_VERTEX_ATTRIBUTES];

    // compute shader specific
    const char* compute_string_owned;
    const char* compute_filepath_owned;

    // material properties
    SG_ShaderIncludes includes;

    static void free(SG_Shader* shader)
    {
        FREE(shader->vertex_string_owned);
        FREE(shader->fragment_string_owned);
        FREE(shader->vertex_filepath_owned);
        FREE(shader->fragment_filepath_owned);
        FREE(shader->compute_string_owned);
        FREE(shader->compute_filepath_owned);
    }
};

// ============================================================================
// SG_Material
// ============================================================================

#define SG_MaterialTypeTable                                                           \
    X(SG_MATERIAL_INVALID = 0, "Invalid")                                              \
    X(SG_MATERIAL_CUSTOM, "Material")                                                  \
    X(SG_MATERIAL_LINES2D, "LinesMaterial")                                            \
    X(SG_MATERIAL_FLAT, "FlatMaterial")                                                \
    X(SG_MATERIAL_UV, "UVMaterial")                                                    \
    X(SG_MATERIAL_NORMAL, "NormalMaterial")                                            \
    X(SG_MATERIAL_WIREFRAME, "WireframeMaterial")                                      \
    X(SG_MATERIAL_DIFFUSE, "DiffuseMaterial")                                          \
    X(SG_MATERIAL_PHONG, "PhongMaterial")                                              \
    X(SG_MATERIAL_PBR, "PBRMaterial")                                                  \
    X(SG_MATERIAL_TEXT3D, "TextMaterial")                                              \
    X(SG_MATERIAL_COMPUTE, "ComputeMaterial")                                          \
    X(SG_MATERIAL_SKYBOX, "SkyboxMaterial")

enum SG_MaterialType {
#define X(name, str) name,
    SG_MaterialTypeTable
#undef X
      SG_MATERIAL_COUNT
};

static const char* SG_MaterialTypeNames[SG_MATERIAL_COUNT] = {
#define X(name, str) str,
    SG_MaterialTypeTable
#undef X
};

struct SG_Material_PBR_Params {
    // uniforms
    glm::vec4 baseColor      = glm::vec4(1.0f);
    glm::vec3 emissiveFactor = glm::vec3(0.0f);
    f32 metallic             = 0.0f;
    f32 roughness            = 1.0f;
    f32 normalFactor         = 1.0f;
    f32 aoFactor             = 1.0f;

    // textures and samplers
    // TODO
    // SG_Sampler baseColorSampler;
};

enum SG_EnvmapSampleMode : u8 {
    SG_ENVMAP_SAMPLE_NONE = 0,
    SG_ENVMAP_SAMPLE_REFLECT,
    SG_ENVMAP_SAMPLE_REFRACT,
};

enum SG_EnvmapBlendMode : u8 {
    SG_ENVMAP_BLEND_NONE = 0,
    SG_ENVMAP_BLEND_ADD,
    SG_ENVMAP_BLEND_MULTIPLY,
    SG_ENVMAP_BLEND_MIX,
};

enum SG_MaterialUniformType : u8 {
    SG_MATERIAL_UNIFORM_NONE = 0,
    SG_MATERIAL_UNIFORM_FLOAT,
    SG_MATERIAL_UNIFORM_VEC2F,
    SG_MATERIAL_UNIFORM_VEC3F,
    SG_MATERIAL_UNIFORM_VEC4F,
    SG_MATERIAL_UNIFORM_INT,
    SG_MATERIAL_UNIFORM_IVEC2,
    SG_MATERIAL_UNIFORM_IVEC3,
    SG_MATERIAL_UNIFORM_IVEC4,
    SG_MATERIAL_UNIFORM_TEXTURE,
    SG_MATERIAL_UNIFORM_SAMPLER,
    SG_MATERIAL_UNIFORM_STORAGE_BUFFER,
    SG_MATERIAL_UNIFORM_STORAGE_BUFFER_EXTERNAL,
    SG_MATERIAL_STORAGE_TEXTURE,
};

union SG_MaterialUniformData {
    f32 f;
    glm::vec2 vec2f;
    glm::vec3 vec3f;
    glm::vec4 vec4f;
    i32 i;
    glm::ivec2 ivec2;
    glm::ivec3 ivec3;
    glm::ivec4 ivec4;
    SG_Sampler sampler;
    SG_ID texture_id;
    SG_ID storage_buffer_id;
    // TODO arena for storage buffer
};

struct SG_MaterialUniformPtrAndSize {
    void* ptr;
    size_t size;
};

struct SG_MaterialUniform {
    SG_MaterialUniformType type;
    SG_MaterialUniformData as;
    // TODO: texture, sampler, array, storage buffer (array<int> and array<float>)
    // for array storage, can we just store a ref to the ck array itself? avoids
    // duplication

    static SG_MaterialUniformPtrAndSize data(SG_MaterialUniform* u)
    {
        // clang-format off
        switch (u->type) {
            case SG_MATERIAL_UNIFORM_FLOAT: return {&u->as.f, sizeof(u->as.f)};
            case SG_MATERIAL_UNIFORM_VEC2F: return {&u->as.vec2f, sizeof(u->as.vec2f)};
            case SG_MATERIAL_UNIFORM_VEC3F: return {&u->as.vec3f, sizeof(u->as.vec3f)};
            case SG_MATERIAL_UNIFORM_VEC4F: return {&u->as.vec4f, sizeof(u->as.vec4f)};
            case SG_MATERIAL_UNIFORM_INT: return {&u->as.i, sizeof(u->as.i)};
            case SG_MATERIAL_UNIFORM_IVEC2: return {&u->as.ivec2, sizeof(u->as.ivec2)};
            case SG_MATERIAL_UNIFORM_IVEC3: return {&u->as.ivec3, sizeof(u->as.ivec3)};
            case SG_MATERIAL_UNIFORM_IVEC4: return {&u->as.ivec4, sizeof(u->as.ivec4)};
            default: ASSERT(false);
        }
        // clang-format on
        return {};
    }
};

// TODO if discrepency between material params too large,
// switch to allocated void* rather than union
// Then SG_Command_MaterialCreate will need a pointer too...
union SG_MaterialParams {
    SG_Material_PBR_Params pbr;
};

struct SG_MaterialPipelineState {
    SG_ID sg_shader_id;
    WGPUCullMode cull_mode                   = WGPUCullMode_None;
    WGPUPrimitiveTopology primitive_topology = WGPUPrimitiveTopology_TriangleList;
    b32 transparent;
};

struct SG_Material : SG_Component {
    SG_MaterialType material_type;
    SG_MaterialParams params;

    // PSO
    SG_MaterialPipelineState pso; // keep it copy by value

    // uniforms
    SG_MaterialUniform uniforms[CHUGL_MATERIAL_MAX_BINDINGS];

    // fns
    static void removeUniform(SG_Material* mat, int location);
    static void setUniform(SG_Material* mat, int location, void* uniform,
                           SG_MaterialUniformType type);

    static void uniformInt(SG_Material* mat, int location, int value)
    {
        mat->uniforms[location].type = SG_MATERIAL_UNIFORM_INT;
        mat->uniforms[location].as.i = value;
    }

    static void uniformFloat(SG_Material* mat, int location, f32 value)
    {
        mat->uniforms[location].type = SG_MATERIAL_UNIFORM_FLOAT;
        mat->uniforms[location].as.f = value;
    }

    static void uniformVec2f(SG_Material* mat, int location, glm::vec2 value)
    {
        mat->uniforms[location].type     = SG_MATERIAL_UNIFORM_VEC2F;
        mat->uniforms[location].as.vec2f = value;
    }

    static void uniformVec3f(SG_Material* mat, int location, glm::vec3 value)
    {
        mat->uniforms[location].type     = SG_MATERIAL_UNIFORM_VEC3F;
        mat->uniforms[location].as.vec3f = value;
    }

    static void uniformVec4f(SG_Material* mat, int location, glm::vec4 value)
    {
        mat->uniforms[location].type     = SG_MATERIAL_UNIFORM_VEC4F;
        mat->uniforms[location].as.vec4f = value;
    }

    static void setStorageBuffer(SG_Material* mat, int location)
    {
        mat->uniforms[location].type = SG_MATERIAL_UNIFORM_STORAGE_BUFFER;
    }

    static void storageBuffer(SG_Material* mat, int location, SG_Buffer* buffer)
    {
        mat->uniforms[location].type = SG_MATERIAL_UNIFORM_STORAGE_BUFFER_EXTERNAL;
        mat->uniforms[location].as.storage_buffer_id = buffer->id;
    }

    static void setSampler(SG_Material* mat, int location, SG_Sampler sampler)
    {
        mat->uniforms[location].type       = SG_MATERIAL_UNIFORM_SAMPLER;
        mat->uniforms[location].as.sampler = sampler;
    }

    static void setTexture(SG_Material* mat, int location, SG_Texture* tex);

    static void setStorageTexture(SG_Material* mat, int location, SG_Texture* tex);

    static void shader(SG_Material* mat, SG_Shader* shader);
};

// ============================================================================
// SG_Mesh
// ============================================================================

// in SG we'll try deep: SG_Mesh inherits SG_Transform
// in R we'll try flat: R_Transform contains union { mesh, light, camera }
struct SG_Mesh : SG_Transform {
    // don't set directly. use setGeometry and setMaterial for proper
    // refcounting
    SG_ID _geo_id;
    SG_ID _mat_id;
    b32 receives_shadows;

    static void setGeometry(SG_Mesh* mesh, SG_Geometry* geo);
    static void setMaterial(SG_Mesh* mesh, SG_Material* mat);
};

// ============================================================================
// SG_Camera
// ============================================================================

enum SG_CameraType : u8 {
    SG_CameraType_PERPSECTIVE  = 0,
    SG_CameraType_ORTHOGRAPHIC = 1,
};

enum SG_CameraControllerType : u8 {
    SG_CameraControllerType_None  = 0,
    SG_CameraControllerType_Orbit = 1,
    SG_CameraControllerType_Fly   = 2,
};

struct SG_CameraParams {
    SG_CameraType camera_type;
    float fov_radians = PI / 4.0f; // radians (45deg)
    float size = 6.6f; // orthographic size (scales view volume while preserving ratio
                       // of width to height)
    float far_plane  = 100.0f;
    float near_plane = .1f;
};

// spherical coordinates for OrbitCamera
struct SphericalCoords {
    f32 radius;
    f32 theta; // polar (radians)
    f32 phi;   // azimuth (radians)

    // Left handed system
    // (1, 0, 0) maps to cartesion coordinate (0, 0, 1)
    static glm::vec3 toCartesian(SphericalCoords s, glm::vec3 o)
    {
        f32 v = s.radius * cos(s.phi);
        return o
               + glm::vec3(v * sin(s.theta),      // x
                           s.radius * sin(s.phi), // y
                           v * cos(s.theta)       // z
               );
    }

    static SphericalCoords fromCartesian(glm::vec3 c)
    {
        SphericalCoords s;
        s.radius = glm::length(c);
        s.theta  = atan2(c.x, c.z);
        s.phi    = asin(c.y / s.radius);
        return s;
    }
};

struct SG_OrbitCameraParams {
    SphericalCoords spherical = { 6.0f, 0.0f, 0.0f }; // *relative to target*
    f32 zoom_speed            = 0.5f;
    glm::vec3 target          = glm::vec3(0.0f); // point to look at
    t_CKVEC2 speed            = { 0.01f, 0.01f };
};

struct SG_FlyCameraParams {
    f32 speed             = 2.5f;
    f32 mouse_sensitivity = 0.003f;
};

struct SG_Camera : SG_Transform {
    SG_CameraParams params;
    SG_CameraControllerType controller_type;

    SG_OrbitCameraParams orbit;
    SG_FlyCameraParams fly;

    static glm::mat4 projectionMatrix(SG_Camera* camera, float aspect)
    {
        switch (camera->params.camera_type) {
            case SG_CameraType_PERPSECTIVE:
                return glm::perspective(camera->params.fov_radians, aspect,
                                        camera->params.near_plane,
                                        camera->params.far_plane);
            case SG_CameraType_ORTHOGRAPHIC: {
                float width  = camera->params.size * aspect;
                float height = camera->params.size;
                return glm::ortho( // extents in WORLD SPACE units
                  -width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f,
                  camera->params.near_plane, camera->params.far_plane);
            }
            default: ASSERT(false); return glm::mat4(1.0f);
        }
    }

    static glm::mat4 viewMatrix(SG_Camera* cam)
    {
        return glm::inverse(SG_Transform::worldMatrix(cam));
    }
};

// ============================================================================
// SG_Text
// TODO is it possible to fit this into the geo, mat, mesh system?
// this, like GLines is a renderable that *doesn't* have normal geometry (pos, uv, etc)
// rather, the geometry and visuals are procedurally calculated on graphics thread and
// gpu
// ============================================================================

struct SG_Text : public SG_Mesh {
    std::string font_path   = "";
    std::string text        = "";
    t_CKVEC2 control_points = { 0.5f, 0.5f };
    float vertical_spacing  = 1.0f;
};

// ============================================================================
// SG_Pass
// ============================================================================

enum SG_PassType : u8 {
    SG_PassType_Root = 0, // special start of pass chain. reserved for GG.rootPass();
    SG_PassType_Render,
    SG_PassType_Scene,
    SG_PassType_Screen,
    SG_PassType_Compute,
    SG_PassType_Bloom, // hacking in bloom for now
    SG_PassType_Count,
};

struct SG_Pass : public SG_Component {
    SG_ID next_pass_id;
    SG_PassType pass_type;

    // RenderPass params
    b32 color_target_clear_on_load = 1;
    SG_ID color_target_id;

    b32 viewport_normalized = 1;
    f32 viewport_x          = 0.0f;
    f32 viewport_y          = 0.0f;
    f32 viewport_w          = 1.0f;
    f32 viewport_h          = 1.0f;

    b32 scissor_normalized = 1;
    f32 scissor_x          = 0.0f;
    f32 scissor_y          = 0.0f;
    f32 scissor_w          = 1.0f;
    f32 scissor_h          = 1.0f;

    // ScenePass params
    SG_ID scene_id;
    SG_ID camera_id;
    b32 scene_pass_msaa;

    // ScreenPass params
    SG_ID screen_material_id; // created implicitly, material.pos.shader_id =
                              // screen_shader_id.
                              // backed by ckobj. DELETE in SG_Pass dtor.

    // ComputePass params
    SG_ID compute_material_id; // created implicitly, material.pos.shader_id =
                               // compute_shader_id DELETE in SG_Pass dtor.
    struct {
        u32 x, y, z;
    } compute_workgroup;

    // bloom pass params
    SG_ID bloom_downsample_material_id;
    SG_ID bloom_upsample_material_id;
    SG_ID bloom_input_render_texture_id;
    SG_ID bloom_output_render_texture_id;
    int bloom_num_blur_levels = 7;

    // true if pass_a and pass_b are connected by any number of steps
    static bool isConnected(SG_Pass* pass_a, SG_Pass* pass_b);

    // connects two passes iff does not form a cycle
    static bool connect(SG_Pass* this_pass, SG_Pass* next_pass);

    static void disconnect(SG_Pass* this_pass, SG_Pass* next_pass);

    // renderpass methods
    static void colorTarget(SG_Pass* pass, SG_Texture* tex);

    // scenepass methods
    static void scene(SG_Pass* pass, SG_Scene* scene);
    static void camera(SG_Pass* pass, SG_Camera* cam);

    // screenpass methods
    static void screenMaterial(SG_Pass* pass, SG_Material* mat);

    // computepass methods

    // bloom pass methods
    static void bloomInputRenderTexture(SG_Pass* pass, SG_Texture* tex);
    static void bloomOutputRenderTexture(SG_Pass* pass, SG_Texture* tex);
};

// ============================================================================
// SG_Light
// ============================================================================

enum SG_LightType : u8 {
    SG_LightType_None = 0,
    SG_LightType_Directional,
    SG_LightType_Point,
    SG_LightType_Spot,
    SG_LightType_Count,
};

struct SG_LightDesc {
    SG_LightType type;

    glm::vec3 color = glm::vec3(1.0f);
    float intensity = .7f;

    // point AND spot
    // formula: intensity = (1 - distance / radius)^falloff
    float radius  = 10.0f;
    float falloff = 2.0f; // 1.0 = linear, 2.0 = quadratic

    // spot
    float angle_min;
    float angle_max     = 0.523599f; // 30 degrees
    float angle_falloff = 2.0f;

    // shadow
    b32 generates_shadows;
    float bias = .0001f;

    // dirlight shadow
    struct {
        float size  = 20.0f;
        float depth = 500.0f; // far - near
    } dirlight_shadow_bounds;
};

struct SG_Light : public SG_Transform {
    SG_LightDesc desc;

    static void lightType(SG_Light* light, SG_LightType type)
    {
        if (type >= SG_LightType_Count) {
            log_warn(
              "Cannot set invalid light type %d. Use one of GLight.Directional, "
              "GLight.Point, or GLight.spot",
              type);
            return;
        }

        light->desc.type = type;
    }
};

// ============================================================================
// SG Video
// ============================================================================

struct SG_Video : public SG_Component {
    plm_t* plm;
    const char* path_OWNED; // malloced, must free
    SG_ID video_texture_rgba_id;
    float framerate;
    int samplerate;
    float duration_secs;

    // audio playback
    float rate = 1.0;
    float audio_playhead; // position in audio frames
    plm_samples_t* samples;
    float last_audio_samples[2]; // last audio samples (left/right channel) from
                                 // previous audio frame. used for interpolation

    // get length of video in samples
    static int audioFrames(SG_Video* video)
    {
        return (int)(video->duration_secs * video->samplerate);
    }
};

// ============================================================================
// webcam
// ============================================================================

struct SG_Webcam : public SG_Component {
    SG_ID texture_id;
    int device_id;
    int fps;
    bool freeze;
    bool capture = true;
    char device_name[64];
};

// ============================================================================
// SG Component Manager
// ============================================================================

void SG_Init(const Chuck_DL_Api* api);
void SG_Free();

SG_Transform* SG_CreateTransform(Chuck_Object* ckobj);
SG_Scene* SG_CreateScene(Chuck_Object* ckobj);
SG_Geometry* SG_CreateGeometry(Chuck_Object* ckobj);
SG_Texture* SG_CreateTexture(SG_TextureDesc* desc, Chuck_Object* ckobj,
                             Chuck_VM_Shred* shred, bool add_ref,
                             const char* name = NULL);
SG_Camera* SG_CreateCamera(Chuck_Object* ckobj, SG_CameraParams camera_params);
SG_Text* SG_CreateText(Chuck_Object* ckobj);
SG_Pass* SG_CreatePass(Chuck_Object* ckobj, SG_PassType pass_type);
SG_Buffer* SG_CreateBuffer(Chuck_Object* ckobj);

SG_Shader* SG_CreateShader(Chuck_Object* ckobj, const char* vertex_string,
                           const char* fragment_string, const char* vertex_filepath,
                           const char* fragment_filepath,
                           WGPUVertexFormat* vertex_layout, int vertex_layout_len,
                           const char* compute_string, const char* compute_filepath,
                           SG_ShaderIncludes includes);

SG_Material* SG_CreateMaterial(Chuck_Object* ckobj, SG_MaterialType material_type);
SG_Mesh* SG_CreateMesh(Chuck_Object* ckobj, SG_Geometry* sg_geo, SG_Material* sg_mat);
SG_Light* SG_CreateLight(Chuck_Object* ckobj);
SG_Video* SG_CreateVideo(Chuck_Object* ckobj, t_CKUINT id_offset);
SG_Webcam* SG_CreateWebcam(Chuck_Object* ckobj, Chuck_VM_Shred* shred, int device_id,
                           int width, int height, int fps);

SG_Component* SG_GetComponent(SG_ID id);
SG_Transform* SG_GetTransform(SG_ID id);
SG_Scene* SG_GetScene(SG_ID id);
SG_Geometry* SG_GetGeometry(SG_ID id);
SG_Shader* SG_GetShader(SG_ID id);
SG_Material* SG_GetMaterial(SG_ID id);
SG_Mesh* SG_GetMesh(SG_ID id);
SG_Texture* SG_GetTexture(SG_ID id);
SG_Camera* SG_GetCamera(SG_ID id);
SG_Text* SG_GetText(SG_ID id);
SG_Pass* SG_GetPass(SG_ID id);
SG_Buffer* SG_GetBuffer(SG_ID id);
SG_Light* SG_GetLight(SG_ID id);
SG_Video* SG_GetVideo(SG_ID id);
SG_Webcam* SG_GetWebcam(SG_ID id);

// ============================================================================
// SG Garbage Collection
// ============================================================================

void SG_DecrementRef(SG_ID id);
void SG_AddRef(SG_Component* comp);
void SG_GC();
void SG_ComponentFree(SG_Component* comp);
