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

#include <chuck/chugin.h>
#include <unordered_map>
#include <webgpu/webgpu.h>

#include "core/macros.h"
#include "core/memory.h"

#include "sg_command.h"

// TODO: group all this shared state together into a "chugl_audio_context"
// struct, similar to App or Renderer struct

#define BEGIN_CLASS(type, base) QUERY->begin_class(QUERY, type, base)
#define END_CLASS() QUERY->end_class(QUERY)
#define CTOR(func) QUERY->add_ctor(QUERY, func)
#define DTOR(func) QUERY->add_dtor(QUERY, func)
#define MVAR(type, name, is_const) QUERY->add_mvar(QUERY, type, name, is_const)
#define MFUN(func, ret, name) QUERY->add_mfun(QUERY, func, ret, name)
#define SFUN(func, ret, name) QUERY->add_sfun(QUERY, func, ret, name)
#define SVAR(type, name, val) QUERY->add_svar(QUERY, type, name, true, val);
#define ARG(type, name) QUERY->add_arg(QUERY, type, name)
#define DOC_FUNC(doc) QUERY->doc_func(QUERY, doc)
#define DOC_CLASS(doc) QUERY->doc_class(QUERY, doc)
#define DOC_VAR(doc) QUERY->doc_var(QUERY, doc)
#define ADD_EX(path) QUERY->add_ex(QUERY, path)

#define GET_NEXT_INT_ARRAY(ptr) (*((Chuck_ArrayInt**&)ptr)++)
#define GET_NEXT_FLOAT_ARRAY(ptr) (*((Chuck_ArrayFloat**&)ptr)++)
#define GET_NEXT_VEC2_ARRAY(ptr) (*((Chuck_ArrayVec2**&)ptr)++)
#define GET_NEXT_VEC3_ARRAY(ptr) (*((Chuck_ArrayVec3**&)ptr)++)
#define GET_NEXT_VEC4_ARRAY(ptr) (*((Chuck_ArrayVec4**&)ptr)++)
#define GET_NEXT_OBJECT_ARRAY(ptr) (*((Chuck_ArrayInt**&)ptr)++)

#define GET_XFORM(ckobj) SG_GetTransform(OBJ_MEMBER_UINT(ckobj, component_offset_id));
#define GET_MESH(ckobj) SG_GetMesh(OBJ_MEMBER_UINT(ckobj, component_offset_id))
#define GET_MESH_MATERIAL(ckobj) SG_GetMaterial(GET_MESH(ckobj)->_mat_id)
#define GET_MESH_GEOMETRY(ckobj) SG_GetGeometry(GET_MESH(ckobj)->_geo_id)
#define GET_CAMERA(ckobj) SG_GetCamera(OBJ_MEMBER_UINT(ckobj, component_offset_id))
#define GET_TEXTURE(ckobj) SG_GetTexture(OBJ_MEMBER_UINT(ckobj, component_offset_id))

#define ADVANCE_BY_INT(ptr) (((t_CKINT*&)ptr)++)

#define OBJ_MEMBER_INT_ARRAY(obj, offset)                                              \
    (*(Chuck_ArrayInt**)OBJ_MEMBER_DATA(obj, offset))
#define OBJ_MEMBER_FLOAT_ARRAY(obj, offset)                                            \
    (*(Chuck_ArrayFloat**)OBJ_MEMBER_DATA(obj, offset))
#define OBJ_MEMBER_VEC2_ARRAY(obj, offset)                                             \
    (*(Chuck_ArrayVec2**)OBJ_MEMBER_DATA(obj, offset))

// log levels (copied from
// https://github.com/ccrma/chuck/blob/90f966cb8649840bc05f5d77219867593eb7fe94/src/core/chuck_errmsg.h#L78)
#define CK_LOG_ALL 10 // set this to log everything
#define CK_LOG_FINEST 9
#define CK_LOG_FINER 8
#define CK_LOG_FINE 7
#define CK_LOG_DEBUG 6 // 1.5.0.5 was: CK_LOG_CONFIG
#define CK_LOG_INFO 5
#define CK_LOG_WARNING 4
#define CK_LOG_HERALD 3
#define CK_LOG_SYSTEM 2
#define CK_LOG_CORE 1
#define CK_LOG_NONE 0 // set this to log nothing

#define CK_LOG(level, text) g_chuglAPI->vm->em_log(level, text)
#define CK_THROW(exception, desc, shred)                                               \
    g_chuglAPI->vm->throw_exception(exception, desc, shred)

#define CHUGIN_SAFE_DELETE(type, data_offset)                                          \
    type* obj = (type*)OBJ_MEMBER_UINT(SELF, data_offset);                             \
    if (obj) delete obj;                                                               \
    OBJ_MEMBER_UINT(SELF, data_offset) = 0;

// global main hook
Chuck_DL_MainThreadHook* hook = NULL;
bool hookActivated            = false;

// references to VM and API
Chuck_VM* g_chuglVM             = NULL;
CK_DL_API g_chuglAPI            = NULL;
Arena audio_frame_arena         = {};
f64 g_last_dt                   = 0.0;
i64 g_frame_count               = 0;
bool g_chugl_window_initialized = false; // set to true the on first GG.nextFrame()

// cached chuck types
static struct {
    Chuck_DL_Api::Type int_array;
    Chuck_DL_Api::Type float_array;
    Chuck_DL_Api::Type vec2_array;
    Chuck_DL_Api::Type vec3_array;
    Chuck_DL_Api::Type vec4_array;
} g_chuck_types;

// metadata required for scene rendering
struct GG_Config {
    SG_ID mainScene;
    SG_ID mainCamera;
    SG_ID root_pass_id;
    SG_ID default_scene_pass_id;
    SG_ID default_output_pass_id;

    // options
    bool auto_update_scenegraph = true;
    int fixed_timestep_fps      = 60;
};
GG_Config gg_config = {};

// offset which stores the component's SG_ID.
static t_CKUINT component_offset_id = 0;

// vtable offsets
static t_CKINT ggen_update_vt_offset = -1;

// SG_Sampler offsets
// idea: store these as static ints on SG_Sampler struct?
static t_CKUINT sampler_offset_wrapU     = 0;
static t_CKUINT sampler_offset_wrapV     = 0;
static t_CKUINT sampler_offset_wrapW     = 0;
static t_CKUINT sampler_offset_filterMin = 0;
static t_CKUINT sampler_offset_filterMag = 0;
static t_CKUINT sampler_offset_filterMip = 0;

// other shared data offset
// static t_CKUINT b2_world_data_offset = 0;

struct chugl_MaterialBuiltinShaders {
    SG_ID lines2d_shader_id;
    SG_ID flat_shader_id;
    SG_ID gtext_shader_id;
    SG_ID pbr_shader_id;
    SG_ID uv_shader_id;
    SG_ID normal_shader_id;
    SG_ID phong_shader_id;
    SG_ID points_shader_id;
    SG_ID wireframe_shader_id;

    // screen shaders
    SG_ID output_pass_shader_id;
    SG_ID bloom_downsample_screen_shader_id;
    SG_ID bloom_upsample_screen_shader_id;

    // compute shaders
    SG_ID bloom_downsample_shader_id;
    SG_ID bloom_upsample_shader_id;

    // skybox shaders
    SG_ID skybox_shader_id;

    // box2d debug shaders
    SG_ID b2_debug_solid_polygon_shader_id;
};
static chugl_MaterialBuiltinShaders g_material_builtin_shaders;

struct chugl_builtin_textures {
    SG_ID white_pixel_id;
    SG_ID black_pixel_id;
    SG_ID normal_pixel_id;
    SG_ID magenta_pixel_id;
    SG_ID default_render_texture_id;
    SG_ID default_cubemap_id;
};
static chugl_builtin_textures g_builtin_textures;

struct {
    Chuck_ArrayFloat* empty_float_array;
    Chuck_ArrayFloat* init_2d_pos;           // {0.0, 0.0}
    Chuck_ArrayVec3* init_white_color;       // [@(1,1,1)]
    Chuck_ArrayFloat* white_pixel_data;      // {1.0, 1.0, 1.0, 1.0}
    Chuck_ArrayFloat* black_pixel_data;      // {0.0, 0.0, 0.0, 0.0}
    Chuck_ArrayFloat* normal_pixel_data;     // {0.5, 0.5, 1.0, 1.0}
    Chuck_ArrayFloat* magenta_pixel_data;    // {1.0, 0.0, 1.0, 1.0}
    Chuck_ArrayFloat* default_cube_map_data; // 6 x {1.0, 0.0, 1.0, 1.0}

    Chuck_String* FRAME_UNIFORMS;
    Chuck_String* LIGHTING_UNIFORMS;
    Chuck_String* DRAW_UNIFORMS;
    Chuck_String* STANDARD_VERTEX_INPUT;
    Chuck_String* STANDARD_VERTEX_OUTPUT;
    Chuck_String* STANDARD_VERTEX_SHADER;
} g_builtin_ckobjs;

// map from ckobj to shred
std::unordered_map<Chuck_Object*, Chuck_VM_Shred*> ckobj_2_OriginShred;

t_CKINT chugin_setVTableOffset(const char* type_name, const char* method_name)
{
    // update() vt offset
    Chuck_Type* cktype = g_chuglAPI->type->lookup(g_chuglVM, type_name);
    // find the offset for update
    return g_chuglAPI->type->get_vtable_offset(g_chuglVM, cktype, method_name);
}

// store ckobj --> ancestor shred mapping
void chugin_setOriginShred(Chuck_Object* ckobj, Chuck_VM_Shred* shred)
{
    Chuck_VM_Shred* parent_shred = shred;
    // walk up parent chain until top-level
    while (g_chuglAPI->shred->parent(parent_shred)) {
        parent_shred = g_chuglAPI->shred->parent(parent_shred);
    }

    ckobj_2_OriginShred[ckobj] = parent_shred;
}

Chuck_VM_Shred* chugin_getOriginShred(Chuck_Object* ckobj)
{
    return ckobj_2_OriginShred.find(ckobj) == ckobj_2_OriginShred.end() ?
             NULL :
             ckobj_2_OriginShred[ckobj];
}

Chuck_VM_Shred* chugin_removeFromOriginShredMap(Chuck_Object* ckobj)
{
    Chuck_VM_Shred* shred = chugin_getOriginShred(ckobj);
    ckobj_2_OriginShred.erase(ckobj);
    return shred;
}

Chuck_Object* chugin_createCkObj(const char* type_name, bool add_ref,
                                 Chuck_VM_Shred* shred = NULL)
{
    Chuck_DL_Api::Type cktype = g_chuglAPI->type->lookup(g_chuglVM, type_name);
    if (shred) {
        return g_chuglAPI->object->create(shred, cktype, add_ref);
    } else {
        return g_chuglAPI->object->create_without_shred(g_chuglVM, cktype, add_ref);
    }
}

Chuck_Object* chugin_createCkObj(Chuck_DL_Api::Type cktype, bool add_ref,
                                 Chuck_VM_Shred* shred = NULL)
{
    if (shred) {
        return g_chuglAPI->object->create(shred, cktype, add_ref);
    } else {
        return g_chuglAPI->object->create_without_shred(g_chuglVM, cktype, add_ref);
    }
}

const char* chugin_copyCkString(Chuck_String* ck_str)
{
    return strdup(g_chuglAPI->object->str(ck_str));
}

Chuck_String* chugin_createCkString(const char* str, bool add_ref)
{
    return g_chuglAPI->object->create_string(g_chuglVM, str, add_ref);
}

// copies up to count elements from ck_arr to arr
int chugin_copyCkIntArray(Chuck_ArrayInt* ck_arr, int* arr, int count)
{
    ASSERT(arr);
    int size = ck_arr ? MIN(g_chuglAPI->object->array_int_size(ck_arr), count) : 0;
    for (int i = 0; i < size; i++) {
        arr[i] = (i32)g_chuglAPI->object->array_int_get_idx(ck_arr, i);
    }
    return size;
}

// copies up to count elements from ck_arr to arr
void chugin_copyCkFloatArray(Chuck_ArrayFloat* ck_arr, float* arr, int count)
{
    if (!ck_arr) return;
    int size = MIN(g_chuglAPI->object->array_float_size(ck_arr), count);
    for (int i = 0; i < size; i++) {
        arr[i] = (f32)g_chuglAPI->object->array_float_get_idx(ck_arr, i);
    }
}

// copies a chuck vec2 array into `arr`
void chugin_copyCkVec2Array(Chuck_ArrayVec2* ck_arr, f32* arr)
{
    if (!ck_arr) return;
    int size = g_chuglAPI->object->array_vec2_size(ck_arr);
    for (int i = 0; i < size; i++) {
        t_CKVEC2 vec2  = g_chuglAPI->object->array_vec2_get_idx(ck_arr, i);
        arr[i * 2]     = vec2.x;
        arr[i * 2 + 1] = vec2.y;
    }
}

int chugin_copyCkVec2Array(Chuck_ArrayVec2* ck_arr, f32* arr, int count)
{
    if (!ck_arr) return 0;
    int size = MIN(g_chuglAPI->object->array_vec2_size(ck_arr), count);
    for (int i = 0; i < size; i++) {
        t_CKVEC2 vec2  = g_chuglAPI->object->array_vec2_get_idx(ck_arr, i);
        arr[i * 2]     = vec2.x;
        arr[i * 2 + 1] = vec2.y;
    }
    return size;
}

void chugin_copyCkVec3Array(Chuck_ArrayVec3* ck_arr, f32* arr)
{
    if (!ck_arr) return;
    int size = g_chuglAPI->object->array_vec3_size(ck_arr);
    for (int i = 0; i < size; i++) {
        t_CKVEC3 vec3  = g_chuglAPI->object->array_vec3_get_idx(ck_arr, i);
        arr[i * 3]     = vec3.x;
        arr[i * 3 + 1] = vec3.y;
        arr[i * 3 + 2] = vec3.z;
    }
}

void chugin_copyCkVec4Array(Chuck_ArrayVec4* ck_arr, f32* arr)
{
    if (!ck_arr) return;
    int size = g_chuglAPI->object->array_vec4_size(ck_arr);
    for (int i = 0; i < size; i++) {
        t_CKVEC4 vec4  = g_chuglAPI->object->array_vec4_get_idx(ck_arr, i);
        arr[i * 4]     = vec4.x;
        arr[i * 4 + 1] = vec4.y;
        arr[i * 4 + 2] = vec4.z;
        arr[i * 4 + 3] = vec4.w;
    }
}

Chuck_ArrayInt* chugin_createCkIntArray(int* arr, int count, bool add_ref = false)
{
    Chuck_ArrayInt* ck_arr = (Chuck_ArrayInt*)chugin_createCkObj("int[]", add_ref);
    ASSERT(g_chuglAPI->object->array_int_size(ck_arr) == 0);
    for (int i = 0; i < count; i++) {
        g_chuglAPI->object->array_int_push_back(ck_arr, arr[i]);
    }
    return ck_arr;
}

Chuck_ArrayFloat* chugin_createCkFloatArray(float* arr, int count, bool add_ref = false)
{
    Chuck_ArrayFloat* ck_arr
      = (Chuck_ArrayFloat*)chugin_createCkObj("float[]", add_ref);
    ASSERT(g_chuglAPI->object->array_float_size(ck_arr) == 0);
    for (int i = 0; i < count; i++) {
        g_chuglAPI->object->array_float_push_back(ck_arr, arr[i]);
    }
    return ck_arr;
}

Chuck_ArrayVec2* chugin_createCkFloat2Array(glm::vec2* arr, int count,
                                            bool add_ref          = false,
                                            Chuck_VM_Shred* shred = NULL)
{
    Chuck_ArrayVec2* ck_arr
      = (Chuck_ArrayVec2*)chugin_createCkObj(g_chuck_types.vec2_array, add_ref, shred);
    for (int i = 0; i < count; i++) {
        g_chuglAPI->object->array_vec2_push_back(ck_arr, { arr[i].x, arr[i].y });
    }
    return ck_arr;
}

Chuck_ArrayVec3* chugin_createCkFloat3Array(glm::vec3* arr, int count,
                                            bool add_ref = false)
{
    Chuck_ArrayVec3* ck_arr = (Chuck_ArrayVec3*)chugin_createCkObj("vec3[]", add_ref);
    for (int i = 0; i < count; i++) {
        g_chuglAPI->object->array_vec3_push_back(ck_arr,
                                                 { arr[i].x, arr[i].y, arr[i].z });
    }
    return ck_arr;
}

Chuck_ArrayVec4* chugin_createCkFloat4Array(glm::vec4* arr, int count,
                                            bool add_ref = false)
{
    Chuck_ArrayVec4* ck_arr = (Chuck_ArrayVec4*)chugin_createCkObj("vec4[]", add_ref);
    for (int i = 0; i < count; i++) {
        g_chuglAPI->object->array_vec4_push_back(
          ck_arr, { arr[i].x, arr[i].y, arr[i].z, arr[i].w });
    }
    return ck_arr;
}

bool chugin_typeEquals(Chuck_Object* ckobj, const char* type_name)
{
    Chuck_DL_Api::Type ggenType = g_chuglAPI->type->lookup(g_chuglVM, type_name);
    Chuck_DL_Api::Type thisType = g_chuglAPI->object->get_type(ckobj);
    // check for exact match (subclasses are handled on their own)
    return g_chuglAPI->type->is_equal(thisType, ggenType);
}

// impl in ulib_light.cpp
SG_Light* ulib_light_create(Chuck_Object* ckobj, SG_LightType type);

// impl in ulib_texture.cpp
SG_Texture* ulib_texture_load(const char* filepath, SG_TextureLoadDesc* load_desc,
                              Chuck_VM_Shred* shred);
Chuck_Object* ulib_texture_ckobj_from_sampler(SG_Sampler sampler, bool add_ref,
                                              Chuck_VM_Shred* shred);

// impl in ulib_material.cpp
void ulib_material_set_shader(SG_Material* material, SG_Shader* shader);
SG_Material* ulib_material_create(SG_MaterialType type, Chuck_VM_Shred* shred);
void ulib_material_cq_update_all_uniforms(SG_Material* material);

// impl in ulib_geometry.cpp
SG_Geometry* ulib_geometry_create(SG_GeometryType type, Chuck_VM_Shred* shred);
void ulib_geo_lines2d_set_lines_points(SG_Geometry* geo, Chuck_Object* ck_arr);
void ulib_geo_lines2d_set_lines_points(SG_Geometry* geo, Chuck_Object* ck_arr, int len);
void ulib_geo_lines2d_set_line_colors(SG_Geometry* geo, Chuck_Object* ck_arr);
void ulib_geo_lines2d_set_line_colors(SG_Geometry* geo, f32* data, int data_len);
void CQ_UpdateAllVertexAttributes(SG_Geometry* geo);
void ulib_geo_set_pulled_vertex_attribute_data(SG_Geometry* geo, t_CKINT location,
                                               f32* data, int data_len);
void geoSetPulledVertexAttribute(SG_Geometry* geo, t_CKINT location,
                                 Chuck_Object* ck_arr, int num_components, bool is_int);

// impl in ulib_component.cpp
struct SG_Mesh;
struct SG_Geometry;
struct SG_Material;

void ulib_component_set_name(SG_Component* component, const char* name);
SG_Transform* ulib_ggen_create(Chuck_Object* ckobj, Chuck_VM_Shred* shred);
SG_Mesh* ulib_mesh_create(Chuck_Object* mesh_ckobj, SG_Geometry* geo, SG_Material* mat,
                          Chuck_VM_Shred* shred);

// builder for Phong Material
struct PhongParams {
    static void specular(SG_Material* mat, glm::vec3 color)
    {
        SG_Material::uniformVec3f(mat, 0, color);
        CQ_PushCommand_MaterialSetUniform(mat, 0);
    }

    static glm::vec3* specular(SG_Material* mat)
    {
        return &mat->uniforms[0].as.vec3f;
    }

    static void diffuse(SG_Material* mat, glm::vec3 color)
    {
        SG_Material::uniformVec4f(mat, 1, glm::vec4(color, 1.0));
        CQ_PushCommand_MaterialSetUniform(mat, 1);
    }

    static glm::vec4* diffuse(SG_Material* mat)
    {
        return &mat->uniforms[1].as.vec4f;
    }

    static void alpha(SG_Material* mat, float alpha)
    {
        mat->uniforms[1].as.vec4f.a = alpha;
        CQ_PushCommand_MaterialSetUniform(mat, 1);
    }

    static void shininess(SG_Material* mat, float shininess)
    {
        SG_Material::uniformFloat(mat, 2, shininess);
        CQ_PushCommand_MaterialSetUniform(mat, 2);
    }

    static float* shininess(SG_Material* mat)
    {
        return &mat->uniforms[2].as.f;
    }
    static void emission(SG_Material* mat, glm::vec3 color)
    {
        SG_Material::uniformVec3f(mat, 3, color);
        CQ_PushCommand_MaterialSetUniform(mat, 3);
    }

    static glm::vec3* emission(SG_Material* mat)
    {
        return &mat->uniforms[3].as.vec3f;
    }

    static void normalFactor(SG_Material* mat, float factor)
    {
        SG_Material::uniformFloat(mat, 4, factor);
        CQ_PushCommand_MaterialSetUniform(mat, 4);
    }

    static float* normalFactor(SG_Material* mat)
    {
        return &mat->uniforms[4].as.f;
    }

    static void aoFactor(SG_Material* mat, float factor)
    {
        SG_Material::uniformFloat(mat, 5, factor);
        CQ_PushCommand_MaterialSetUniform(mat, 5);
    }

    static float* aoFactor(SG_Material* mat)
    {
        return &mat->uniforms[5].as.f;
    }

    static void sampler(SG_Material* mat, SG_Sampler sampler)
    {
        SG_Material::setSampler(mat, 6, sampler);
        CQ_PushCommand_MaterialSetUniform(mat, 6);
    }

    static SG_Sampler sampler(SG_Material* mat)
    {
        return mat->uniforms[6].as.sampler;
    }

    static void albedoTex(SG_Material* mat, SG_Texture* tex)
    {
        if (!tex) {
            tex = SG_GetTexture(g_builtin_textures.white_pixel_id);
        }
        SG_Material::setTexture(mat, 7, tex);
        CQ_PushCommand_MaterialSetUniform(mat, 7);
    }

    static SG_Texture* albedoTex(SG_Material* mat)
    {
        return SG_GetTexture(mat->uniforms[7].as.texture_id);
    }

    static void specularTex(SG_Material* mat, SG_Texture* tex)
    {
        if (!tex) {
            tex = SG_GetTexture(g_builtin_textures.white_pixel_id);
        }
        SG_Material::setTexture(mat, 8, tex);
        CQ_PushCommand_MaterialSetUniform(mat, 8);
    }

    static SG_Texture* specularTex(SG_Material* mat)
    {
        return SG_GetTexture(mat->uniforms[8].as.texture_id);
    }

    static void aoTex(SG_Material* mat, SG_Texture* tex)
    {
        if (!tex) {
            tex = SG_GetTexture(g_builtin_textures.white_pixel_id);
        }
        SG_Material::setTexture(mat, 9, tex);
        CQ_PushCommand_MaterialSetUniform(mat, 9);
    }

    static SG_Texture* aoTex(SG_Material* mat)
    {
        return SG_GetTexture(mat->uniforms[9].as.texture_id);
    }

    static void emissiveTex(SG_Material* mat, SG_Texture* tex)
    {
        if (!tex) {
            tex = SG_GetTexture(g_builtin_textures.black_pixel_id);
        }
        SG_Material::setTexture(mat, 10, tex);
        CQ_PushCommand_MaterialSetUniform(mat, 10);
    }

    static SG_Texture* emissiveTex(SG_Material* mat)
    {
        return SG_GetTexture(mat->uniforms[10].as.texture_id);
    }

    static void normalTex(SG_Material* mat, SG_Texture* tex)
    {
        if (!tex) {
            tex = SG_GetTexture(g_builtin_textures.normal_pixel_id);
        }
        SG_Material::setTexture(mat, 11, tex);
        CQ_PushCommand_MaterialSetUniform(mat, 11);
    }

    static SG_Texture* normalTex(SG_Material* mat)
    {
        return SG_GetTexture(mat->uniforms[11].as.texture_id);
    }

    static void envmapMethod(SG_Material* mat, SG_EnvmapSampleMode mode)
    {
        SG_Material::uniformInt(mat, 12, mode);
        CQ_PushCommand_MaterialSetUniform(mat, 12);
    }

    static SG_EnvmapSampleMode* envmapMethod(SG_Material* mat)
    {
        return (SG_EnvmapSampleMode*)&mat->uniforms[12].as.i;
    }

    static void envmapRefractionRatio(SG_Material* mat, float ratio)
    {
        SG_Material::uniformFloat(mat, 13, ratio);
        CQ_PushCommand_MaterialSetUniform(mat, 13);
    }

    static float* envmapRefractionRatio(SG_Material* mat)
    {
        return &mat->uniforms[13].as.f;
    }

    static void envmapBlendMode(SG_Material* mat, SG_EnvmapBlendMode mode)
    {
        SG_Material::uniformInt(mat, 16, mode);
        CQ_PushCommand_MaterialSetUniform(mat, 16);
    }

    static SG_EnvmapBlendMode* envmapBlendMode(SG_Material* mat)
    {
        return (SG_EnvmapBlendMode*)&mat->uniforms[16].as.i;
    }

    static void envmapIntensity(SG_Material* mat, float intensity)
    {
        SG_Material::uniformFloat(mat, 17, intensity);
        CQ_PushCommand_MaterialSetUniform(mat, 17);
    }

    static float* envmapIntensity(SG_Material* mat)
    {
        return &mat->uniforms[17].as.f;
    }
};

#define PHONG_MATERIAL_METHODS(prefix)                                                 \
    {                                                                                  \
        MFUN(prefix##_material_get_specular_color, "vec3", "specular");                \
        DOC_FUNC("Get the specular color of the material.");                           \
                                                                                       \
        MFUN(prefix##_material_set_specular_color, "void", "specular");                \
        ARG("vec3", "specular");                                                       \
        DOC_FUNC("Set the specular color of the material.");                           \
                                                                                       \
        MFUN(prefix##_material_get_diffuse_color, "vec3", "color");                    \
        DOC_FUNC("Get the diffuse color of the material.");                            \
                                                                                       \
        MFUN(prefix##_material_set_diffuse_color, "void", "color");                    \
        ARG("vec3", "diffuse");                                                        \
        DOC_FUNC("Set the diffuse color of the material.");                            \
                                                                                       \
        MFUN(prefix##_material_get_alpha, "float", "alpha");                           \
        DOC_FUNC("Get the alpha channel of the diffuse color of the material.");       \
                                                                                       \
        MFUN(prefix##_material_set_alpha, "void", "alpha");                            \
        ARG("float", "alpha");                                                         \
        DOC_FUNC("Set the alpha channel of the diffuse color of the material.");       \
                                                                                       \
        MFUN(prefix##_material_get_log_shininess, "float", "shine");                   \
        DOC_FUNC("Get the log shininess exponent");                                    \
                                                                                       \
        MFUN(prefix##_material_set_log_shininess, "void", "shine");                    \
        ARG("float", "shine");                                                         \
        DOC_FUNC("Set the log shininess exponent. default 5.");                        \
                                                                                       \
        MFUN(prefix##_material_get_emission_color, "vec3", "emission");                \
        DOC_FUNC("Get the emission color of the material.");                           \
                                                                                       \
        MFUN(prefix##_material_set_emission_color, "void", "emission");                \
        ARG("vec3", "emission");                                                       \
        DOC_FUNC("Set the emission color of the material. Default black.");            \
                                                                                       \
        MFUN(prefix##_material_get_normal_factor, "float", "normalFactor");            \
        DOC_FUNC(                                                                      \
          "Get the normal factor of the material. Scales effect of normal map. "       \
          "Default "                                                                   \
          "1.0");                                                                      \
                                                                                       \
        MFUN(prefix##_material_set_normal_factor, "void", "normalFactor");             \
        ARG("float", "normalFactor");                                                  \
        DOC_FUNC(                                                                      \
          "Set the normal factor of the material. Scales effect of normal map");       \
                                                                                       \
        MFUN(prefix##_material_get_ao_factor, "float", "aoFactor");                    \
        DOC_FUNC("Get the ambient occlusion factor of the material. Default 1.0");     \
                                                                                       \
        MFUN(prefix##_material_set_ao_factor, "void", "aoFactor");                     \
        ARG("float", "aoFactor");                                                      \
        DOC_FUNC(                                                                      \
          "Set the ambient occlusion factor of the material. Default 1.0. 0 disables " \
          "AO. Set between 0 and 1.");                                                 \
                                                                                       \
        MFUN(prefix##_material_get_albedo_tex, SG_CKNames[SG_COMPONENT_TEXTURE],       \
             "colorMap");                                                              \
        DOC_FUNC("Get the diffuse texture of the material.");                          \
                                                                                       \
        MFUN(prefix##_material_set_albedo_tex, "void", "colorMap");                    \
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "albedoTexture");                        \
        DOC_FUNC("Set the diffuse texture of the material.");                          \
                                                                                       \
        MFUN(prefix##_material_get_specular_tex, SG_CKNames[SG_COMPONENT_TEXTURE],     \
             "specularMap");                                                           \
        DOC_FUNC("Get the specular texture of the material.");                         \
                                                                                       \
        MFUN(prefix##_material_set_specular_tex, "void", "specularMap");               \
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "specularTexture");                      \
        DOC_FUNC("Set the specular texture of the material.");                         \
                                                                                       \
        MFUN(prefix##_material_get_ao_tex, SG_CKNames[SG_COMPONENT_TEXTURE], "aoMap"); \
        DOC_FUNC("Get the ambient occlusion texture of the material.");                \
                                                                                       \
        MFUN(prefix##_material_set_ao_tex, "void", "aoMap");                           \
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "aoTexture");                            \
        DOC_FUNC("Set the ambient occlusion texture of the material.");                \
                                                                                       \
        MFUN(prefix##_material_get_emissive_tex, SG_CKNames[SG_COMPONENT_TEXTURE],     \
             "emissiveMap");                                                           \
        DOC_FUNC("Get the emissive texture of the material.");                         \
                                                                                       \
        MFUN(prefix##_material_set_emissive_tex, "void", "emissiveMap");               \
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "emissiveTexture");                      \
        DOC_FUNC("Set the emissive texture of the material.");                         \
                                                                                       \
        MFUN(prefix##_material_get_normal_tex, SG_CKNames[SG_COMPONENT_TEXTURE],       \
             "normalMap");                                                             \
        DOC_FUNC("Get the normal texture of the material.");                           \
                                                                                       \
        MFUN(prefix##_material_set_normal_tex, "void", "normalMap");                   \
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "normalTexture");                        \
        DOC_FUNC("Set the normal texture of the material.");                           \
                                                                                       \
        MFUN(prefix##_material_get_envmap_method, "int", "envmapMethod");              \
        DOC_FUNC(                                                                      \
          "Get the environment map sampling method. One of "                           \
          "PhongMaterial.EnvmapMethod_None, PhongMaterial.EnvmapMethod_Reflect, "      \
          "PhongMaterial.EnvmapMethod_Refract. Set the environment map via "           \
          "GScene.envMap(Texture). Default EnvmapMethod_Reflect");                     \
                                                                                       \
        MFUN(prefix##_material_set_envmap_method, "void", "envmapMethod");             \
        ARG("int", "envmapMethod");                                                    \
        DOC_FUNC(                                                                      \
          "Set the environment map sampling method. One of "                           \
          "PhongMaterial.EnvmapMethod_None, PhongMaterial.EnvmapMethod_Reflect, "      \
          "PhongMaterial.EnvmapMethod_Refract. Default EnvmapMethod_Reflect");         \
                                                                                       \
        MFUN(prefix##_material_get_envmap_refraction_ratio, "float",                   \
             "refractionRatio");                                                       \
        DOC_FUNC(                                                                      \
          "Get the refraction ratio of the material. Only used when "                  \
          "envmapMethod is PhongMaterial.EnvmapMethod_Refract. Default 0.5");          \
                                                                                       \
        MFUN(prefix##_material_set_envmap_refraction_ratio, "void",                    \
             "refractionRatio");                                                       \
        ARG("float", "ratio");                                                         \
        DOC_FUNC(                                                                      \
          "Set the refraction ratio of the material. Only used when "                  \
          "envmapMethod is PhongMaterial.EnvmapMethod_Refract. Default 1.0");          \
                                                                                       \
        MFUN(prefix##_material_get_envmap_blend_mode, "int", "envmapBlend");           \
        DOC_FUNC(                                                                      \
          "Get the blend mode of the environment map. One of "                         \
          "PhongMaterial.EnvmapBlend_None, PhongMaterial.EnvmapBlend_Mix, "            \
          "PhongMaterial.EnvmapBlend_Add, "                                            \
          "PhongMaterial.EnvmapBlend_Multiply. Default EnvmapBlend_None");             \
                                                                                       \
        MFUN(prefix##_material_set_envmap_blend_mode, "void", "envmapBlend");          \
        ARG("int", "blendMode");                                                       \
        DOC_FUNC(                                                                      \
          "Set the blend mode of the environment map. One of "                         \
          "PhongMaterial.EnvmapBlend_None, PhongMaterial.EnvmapBlend_Mix, "            \
          "PhongMaterial.EnvmapBlend_Add, "                                            \
          "PhongMaterial.EnvmapBlend_Multiply. Set to EnvmapBlend_None to disable "    \
          "environment lighting");                                                     \
                                                                                       \
        MFUN(prefix##_material_get_envmap_intensity, "float", "envmapIntensity");      \
        DOC_FUNC("Get the intensity of the environment map lighting. Default 1.0");    \
                                                                                       \
        MFUN(prefix##_material_set_envmap_intensity, "void", "envmapIntensity");       \
        ARG("float", "intensity");                                                     \
        DOC_FUNC("Set the intensity of the environment map lighting. Default 1.0");    \
    }
