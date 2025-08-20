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

#include <stddef.h>

#include "core/macros.h"
#include "sg_component.h"

// #define SG_COMMAND_STRUCT(type) struct type##_Data
// #define SG_COMMAND_DATA_PTR(type) type##_Data*

/*
What needs to happen?

All writes to writeQueue must be lock protected.

What needs to be known:
- how much memory to allocate:
    - command type (enum)  1:1 with command struct
    - table from command type to size
- how to initialize hte memory
    - table from command type to init function
{
    execute command on audio thread
    THEN
    locked {
        allocate memory for new command
        initialize memory
    } // unlock
}

Do I (1) initialize in separate memory and then copy over,
or do I (2) initialize in the writequeue memory?
- probably (2), (1) requires a separate audio-thread frame arena and won't
necessarily be faster
- tradeoff: all memory is contiguous, no chasing down pointers (prev
implementation was pointer array to commands). But means holding the commandQ
lock for longer. Contention increases, renderer may be blocked on swapQueue. but
my guess is that as # of commands inceases, time saved from avoiding cache
misses will outweigh the lock contention. Also it's okay for the renderer to be
blocked in favor of saving time on the audio thread. (audio thread doesn't need
to ever allocate memory itself or do twice the work of initializing in some
frame arena and then copy over)
*/

enum SG_CommandType : u32 {
    SG_COMMAND_NONE = 0,

    // chugl config
    SG_COMMAND_SET_FIXED_TIMESTEP,
    SG_COMMAND_SET_CHUCK_VM_INFO,

    // window
    SG_COMMAND_WINDOW_CLOSE,
    SG_COMMAND_WINDOW_MODE,
    SG_COMMAND_WINDOW_SIZE_LIMITS,
    SG_COMMAND_WINDOW_POSITION,
    SG_COMMAND_WINDOW_CENTER,
    SG_COMMAND_WINDOW_TITLE,
    SG_COMMAND_WINDOW_ICONIFY,
    SG_COMMAND_WINDOW_ATTRIBUTE,
    SG_COMMAND_WINDOW_OPACITY,

    // mouse
    SG_COMMAND_MOUSE_MODE,
    SG_COMMAND_MOUSE_CURSOR,
    SG_COMMAND_MOUSE_CURSOR_NORMAL,

    // UI
    SG_COMMAND_UI_DISABLED,

    // b2 physics
    SG_COMMAND_b2_WORLD_SET,

    // components
    SG_COMMAND_COMPONENT_UPDATE_NAME,
    SG_COMMAND_COMPONENT_FREE,
    SG_COMMAND_CREATE_XFORM,
    SG_COMMAND_ADD_CHILD,
    SG_COMMAND_REMOVE_CHILD,
    SG_COMMAND_REMOVE_ALL_CHILDREN,
    SG_COMMAND_SET_POSITION,
    SG_COMMAND_SET_ROTATATION,
    SG_COMMAND_SET_SCALE,

    // scene
    SG_COMMAND_SCENE_UPDATE,

    // shader
    SG_COMMAND_SHADER_CREATE,

    // material
    SG_COMMAND_MATERIAL_CREATE,
    SG_COMMAND_MATERIAL_UPDATE_PSO,
    SG_COMMAND_MATERIAL_SET_UNIFORM,
    SG_COMMAND_MATERIAL_SET_STORAGE_BUFFER,

    // mesh
    SG_COMMAND_MESH_UPDATE,

    // camera
    SG_COMMAND_CAMERA_CREATE,
    SG_COMMAND_CAMERA_SET_PARAMS,

    // text
    SG_COMMAND_TEXT_REBUILD,
    SG_COMMAND_TEXT_DEFAULT_FONT,

    // gpass
    // TODO gpass remove everything except _update
    SG_COMMAND_PASS_CREATE,
    SG_COMMAND_PASS_UPDATE,
    SG_COMMAND_PASS_CONNECT,
    SG_COMMAND_PASS_DISCONNECT,

    // geometry
    SG_COMMAND_GEO_CREATE,
    SG_COMMAND_GEO_SET_VERTEX_ATTRIBUTE,
    SG_COMMAND_GEO_SET_PULLED_VERTEX_ATTRIBUTE,
    SG_COMMAND_GEO_SET_VERTEX_COUNT,
    SG_COMMAND_GEO_SET_INDICES_COUNT,
    SG_COMMAND_GEO_SET_INDICES,

    // texture
    SG_COMMAND_TEXTURE_CREATE,
    SG_COMMAND_TEXTURE_WRITE,
    SG_COMMAND_TEXTURE_FROM_FILE,
    SG_COMMAND_TEXTURE_FROM_RAW_DATA,
    SG_COMMAND_CUBEMAP_TEXTURE_FROM_FILE,
    SG_COMMAND_COPY_TEXTURE_TO_TEXTURE,
    SG_COMMAND_COPY_TEXTURE_TO_CPU,

    // buffer
    SG_COMMAND_BUFFER_UPDATE,
    SG_COMMAND_BUFFER_WRITE,

    // light
    SG_COMMAND_LIGHT_UPDATE,

    // shadows
    SG_COMMAND_SHADOW_ADD_MESH,
    SG_COMMAND_MESH_SET_SHADOWED,

    // video
    SG_COMMAND_VIDEO_UPDATE,
    SG_COMMAND_VIDEO_SEEK,
    SG_COMMAND_VIDEO_RATE,

    // webcam
    SG_COMMAND_WEBCAM_CREATE,
    SG_COMMAND_WEBCAM_UPDATE,

    // ================================
    // graphics2audio commands
    // ================================

    // reading back gpu data
    SG_COMMAND_G2A_TEXTURE_READ,

    SG_COMMAND_COUNT
};

struct SG_Command {
    SG_CommandType type;
    u64 nextCommandOffset;
};

// chugl config --------------------------------------------------------

struct SG_Command_SetFixedTimestep : public SG_Command {
    int fps;
};

struct SG_Command_SetChuckVMInfo : public SG_Command {
    int srate;
};

// Window Commands --------------------------------------------------------

struct SG_Command_WindowClose : public SG_Command {
};

enum SG_WindowMode : u8 {
    SG_WINDOW_MODE_WINDOWED = 0,
    SG_WINDOW_MODE_FULLSCREEN,
    SG_WINDOW_MODE_WINDOWED_FULLSCREEN
};

enum CHUGL_WindowAttrib : u8 {
    CHUGL_WINDOW_ATTRIB_RESIZABLE = 0,
    CHUGL_WINDOW_ATTRIB_DECORATED,
    CHUGL_WINDOW_ATTRIB_FLOATING,
    CHUGL_WINDOW_ATTRIB_TRANSPARENT_FRAMEBUFFER,
};

struct SG_Command_WindowMode : public SG_Command {
    SG_WindowMode mode;
    int width;
    int height;
};

struct SG_Command_WindowSizeLimits : public SG_Command {
    int min_width;
    int min_height;
    int max_width;
    int max_height;
    int aspect_ratio_x;
    int aspect_ratio_y;
};

struct SG_Command_WindowPosition : public SG_Command {
    int x;
    int y;
};

struct SG_Command_WindowCenter : public SG_Command {
};

struct SG_Command_WindowTitle : public SG_Command {
    u64 title_offset; // get char* via CG_ReadCommandGetOffset(title_offset)
};

struct SG_Command_WindowIconify : public SG_Command {
    bool iconify;
};

struct SG_Command_WindowAttribute : public SG_Command {
    CHUGL_WindowAttrib attrib;
    bool value;
};

struct SG_Command_WindowOpacity : public SG_Command {
    float opacity;
};

// Mouse commands ---------------------------------------------------------

struct SG_Command_MouseMode : public SG_Command {
    int mode;
};

struct SG_Command_MouseCursorNormal : public SG_Command {
};

struct SG_Command_MouseCursor : public SG_Command {
    u64 mouse_cursor_image_offset;
    u32 width;
    u32 height;
    u32 xhot;
    u32 yhot;
};

// UI commands ---------------------------------------------------------

struct SG_Command_UI_Disabled : public SG_Command {
    bool disabled;
};

// Component Commands -----------------------------------------------------
struct SG_Command_ComponentUpdateName : public SG_Command {
    SG_ID sg_id;
    ptrdiff_t name_offset;
};

struct SG_Command_ComponentFree : public SG_Command {
    SG_ID id;
};

struct SG_Command_CreateXform : public SG_Command {
    SG_ID sg_id;
    glm::vec3 pos;
    glm::quat rot;
    glm::vec3 sca;

    // relationships
    // SG_ID parentID;
};

struct SG_Command_AddChild : public SG_Command {
    SG_ID parent_id;
    SG_ID child_id;
};

struct SG_Command_RemoveChild : public SG_Command {
    SG_ID parent;
    SG_ID child;
};

struct SG_Command_RemoveAllChildren : public SG_Command {
    SG_ID parent;
};

struct SG_Command_SetPosition : public SG_Command {
    SG_ID sg_id;
    glm::vec3 pos;
};

struct SG_Command_SetRotation : public SG_Command {
    SG_ID sg_id;
    glm::quat rot;
};

struct SG_Command_SetScale : public SG_Command {
    SG_ID sg_id;
    glm::vec3 sca;
};

struct SG_Command_SceneUpdate : public SG_Command {
    SG_ID sg_id;
    SG_SceneDesc desc;
};

struct SG_Command_GeoCreate : public SG_Command {
    SG_ID sg_id;
};

struct SG_Command_GeoSetVertexAttribute : public SG_Command {
    SG_ID sg_id;
    int location;
    int num_components;
    int data_size_bytes;
    ptrdiff_t data_offset; // byte offset into command queue arena for attribute data
};

struct SG_Command_GeometrySetPulledVertexAttribute : public SG_Command {
    SG_ID sg_id;
    int location;
    size_t data_bytes;
    ptrdiff_t data_offset;
};

struct SG_Command_GeometrySetVertexCount : public SG_Command {
    SG_ID sg_id;
    int count;
};

struct SG_Command_GeometrySetIndicesCount : public SG_Command {
    SG_ID sg_id;
    int count;
};

struct SG_Command_GeoSetIndices : public SG_Command {
    SG_ID sg_id;
    int index_count;
    ptrdiff_t indices_offset;
};

struct SG_Command_TextureCreate : public SG_Command {
    SG_ID sg_id;
    SG_TextureDesc desc;
    char name[64];
};

struct SG_Command_TextureWrite : public SG_Command {
    SG_ID sg_id;
    SG_TextureWriteDesc write_desc;
    int data_size_bytes;
    ptrdiff_t data_offset;
};

struct SG_Command_TextureFromFile : public SG_Command {
    SG_ID sg_id;
    ptrdiff_t filepath_offset;
    bool flip_vertically;
    bool gen_mips;
};

struct SG_Command_TextureFromRawData : public SG_Command {
    SG_ID sg_id;
    int buffer_len;
    ptrdiff_t buffer_offset;
    b32 flip_vertically;
    b32 gen_mips;
};

struct SG_Command_CubemapTextureFromFile : public SG_Command {
    SG_ID sg_id;
    ptrdiff_t right_face_offset;
    ptrdiff_t left_face_offset;
    ptrdiff_t top_face_offset;
    ptrdiff_t bottom_face_offset;
    ptrdiff_t back_face_offset;
    ptrdiff_t front_face_offset;
    bool flip_vertically;
};

struct SG_Command_CopyTextureToTexture : public SG_Command {
    SG_ID dst_texture_id;
    SG_ID src_texture_id;
    SG_TextureLocation dst_location;
    SG_TextureLocation src_location;
    int width;
    int height;
    int depth;
};

struct SG_Command_CopyTextureToCPU : public SG_Command {
    SG_ID id;
};

// shader commands -----------------------------------------------------

struct SG_Command_ShaderCreate : public SG_Command {
    SG_ID sg_id;
    // strings to be freed by render thread
    ptrdiff_t vertex_string_offset;
    ptrdiff_t vertex_filepath_offset;
    ptrdiff_t fragment_string_offset;
    ptrdiff_t fragment_filepath_offset;
    ptrdiff_t compute_string_offset;
    ptrdiff_t compute_filepath_offset;
    WGPUVertexFormat vertex_layout[SG_GEOMETRY_MAX_VERTEX_ATTRIBUTES];

    SG_ShaderIncludes includes;
};

struct SG_Command_MaterialCreate : public SG_Command {
    SG_ID sg_id;
    // SG_MaterialParams params;
    SG_MaterialType material_type;
    SG_MaterialPipelineState pso;
};

struct SG_Command_MaterialUpdatePSO : public SG_Command {
    SG_ID sg_id;
    SG_MaterialPipelineState pso;
};

struct SG_Command_MaterialSetUniform : public SG_Command {
    SG_ID sg_id;
    SG_MaterialUniform uniform;
    int location;
};

struct SG_Command_MaterialSetStorageBuffer : public SG_Command {
    SG_ID sg_id;
    int location;
    ptrdiff_t data_offset;
    int data_size_bytes;
};

struct SG_Command_MeshUpdate : public SG_Command {
    SG_ID mesh_id;
    SG_ID geo_id;
    SG_ID mat_id;
};

// text commands -----------------------------------------------------

struct SG_Command_TextDefaultFont : public SG_Command {
    ptrdiff_t font_path_str_offset;
};

struct SG_Command_TextRebuild : public SG_Command {
    SG_ID text_id; // lazily create text if not found
    SG_ID material_id;
    glm::vec2 control_point; // TODO do as material uniform
    float vertical_spacing;
    ptrdiff_t font_path_str_offset;
    ptrdiff_t text_str_offset;
};

// camera commands -----------------------------------------------------

struct SG_Command_CameraCreate : public SG_Command {
    SG_Camera camera;
};

struct SG_Command_CameraSetParams : public SG_Command {
    SG_ID camera_id;
    SG_CameraParams params;
};

// pass commands -----------------------------------------------------

struct SG_Command_PassCreate : public SG_Command {
    SG_ID pass_id;
    SG_PassType pass_type;
};

// TODO consolidate into single struct, copy all of SG_Pass?
struct SG_Command_PassUpdate : public SG_Command {
    SG_Pass pass;
};

struct SG_Command_PassConnect : public SG_Command {
    SG_ID pass_id;
    SG_ID next_pass_id;
};

// b2 physics commands -----------------------------------------------------

struct b2_SimulateDesc {
    u32 world_id;
    int substeps = 4;
    float rate   = 1.0f;
};

struct SG_Command_b2World_Set : public SG_Command {
    b2_SimulateDesc desc;
};

// buffer commands -----------------------------------------------------

struct SG_Command_BufferUpdate : public SG_Command {
    SG_ID buffer_id;
    SG_BufferDesc desc;
};

struct SG_Command_BufferWrite : public SG_Command {
    SG_ID buffer_id;
    u64 offset_bytes;
    u64 data_size_bytes;
    ptrdiff_t data_offset;
};

// light commands -----------------------------------------------------

struct SG_Command_LightUpdate : public SG_Command {
    SG_ID light_id;
    SG_LightDesc desc;
};

// shadow commands -----------------------------------------------------

struct SG_Command_ShadowAddMesh : public SG_Command {
    SG_ID light_id;
    b32 add; // if false, remove
    int mesh_id_list_len;
    ptrdiff_t mesh_id_list_offset;
};

struct SG_Command_MeshSetShadowed : public SG_Command {
    SG_ID mesh_id;
    b32 shadowed;
};

// video commands -----------------------------------------------------
struct SG_Command_VideoUpdate : public SG_Command {
    SG_ID video_id;
    SG_ID rgba_video_texture_id;
    ptrdiff_t path_offset;
};

struct SG_Command_VideoSeek : public SG_Command {
    SG_ID video_id;
    double time_secs;
};

struct SG_Command_VideoRate : public SG_Command {
    SG_ID video_id;
    double rate;
    bool loop;
};

// webcam commands -----------------------------------------------------

struct _sr_webcam_device;
typedef struct _sr_webcam_device sr_webcam_device;
struct SG_Command_WebcamCreate : public SG_Command {
    SG_ID webcam_id;
    SG_ID webcam_texture_id;
    sr_webcam_device* device;
    int device_id;
};

struct SG_Command_WebcamUpdate : public SG_Command {
    SG_ID webcam_id;
    bool freeze;
    bool capture;
};

// ============================================================================
// Graphics to Audio Commands
// ============================================================================

// reading back gpu data

struct SG_Command_G2A_TextureRead : public SG_Command {
    SG_ID texture_id;
    void* data_OWNED; // malloc on graphics thread, free on audio thread
    int size_bytes;   // size of data
    WGPUBufferMapAsyncStatus status;
};

// ============================================================================
// Command Queue API
// ============================================================================

void CQ_Init();

// swap the command queue double buffer
void CQ_SwapQueues(bool which);

bool CQ_ReadCommandQueueIter(SG_Command** command, bool which);

void CQ_ReadCommandQueueClear(bool which);

// some command structs have variable data (e.g. strings), which are stored in
// the same command queue arena as cmd->xxx_offset. This function returns the
// pointer to the data at the offset.
// (necessary to avoid segfaults from direct pointers to the arena memory
// caused by Arena resizing)
void* CQ_ReadCommandGetOffset(u64 byte_offset, bool which);

// ============================================================================
// Commands
// ============================================================================

// config ---------------------------------------------------------------
void CQ_PushCommand_SetFixedTimestep(int fps);

// window ---------------------------------------------------------------

void CQ_PushCommand_WindowClose();
void CQ_PushCommand_WindowMode(SG_WindowMode mode, int width, int height);
void CQ_PushCommand_WindowSizeLimits(int min_width, int min_height, int max_width,
                                     int max_height, int aspect_ratio_x,
                                     int aspect_ratio_y);
void CQ_PushCommand_WindowPosition(int x, int y);
void CQ_PushCommand_WindowCenter();
void CQ_PushCommand_WindowTitle(const char* title);
void CQ_PushCommand_WindowIconify(bool iconify);
void CQ_PushCommand_WindowAttribute(CHUGL_WindowAttrib attrib, bool value);
void CQ_PushCommand_WindowOpacity(float opacity);

void CQ_PushCommand_MouseMode(int mode);
void CQ_PushCommand_MouseCursor(CK_DL_API API, Chuck_ArrayInt* image_data, u32 width,
                                u32 height, u32 xhot, u32 yhot);

void CQ_PushCommand_MouseCursorNormal();

// UI -------------------------------------------------------------------
void CQ_PushCommand_UI_Disabled(bool disabled);

// components
void CQ_PushCommand_ComponentUpdateName(SG_Component* component);
void CQ_PushCommand_ComponentFree(SG_Component* component);

void CQ_PushCommand_CreateTransform(SG_Transform* xform);
void CQ_PushCommand_AddChild(SG_Transform* parent, SG_Transform* child);
void CQ_PushCommand_RemoveChild(SG_Transform* parent, SG_Transform* child);
void CQ_PushCommand_RemoveAllChildren(SG_Transform* parent);
void CQ_PushCommand_SetPosition(SG_Transform* xform);
void CQ_PushCommand_SetRotation(SG_Transform* xform);
void CQ_PushCommand_SetScale(SG_Transform* xform);

// scene
void CQ_PushCommand_SceneUpdate(SG_Scene* scene);

// geometry
void CQ_PushCommand_GeometryCreate(SG_Geometry* geo);
void CQ_PushCommand_GeometrySetVertexAttribute(SG_Geometry* geo, int location,
                                               int num_components, void* data,
                                               int data_size_bytes);
void CQ_PushCommand_GeometrySetIndices(SG_Geometry* geo, u32* indices, int index_count);
void CQ_PushCommand_GeometrySetPulledVertexAttribute(SG_Geometry* geo, int location,
                                                     void* data, size_t bytes);
void CQ_PushCommand_GeometrySetVertexCount(SG_Geometry* geo, int count);
void CQ_PushCommand_GeometrySetIndicesCount(SG_Geometry* geo, int count);

// texture
void CQ_PushCommand_TextureCreate(SG_Texture* texture);
void CQ_PushCommand_TextureWrite(SG_Texture* texture, SG_TextureWriteDesc* desc,
                                 Chuck_ArrayFloat* ck_array, CK_DL_API API);
void CQ_PushCommand_TextureWriteExternalPtr(SG_Texture* texture,
                                            SG_TextureWriteDesc* desc, void* data);

void CQ_PushCommand_TextureFromFile(SG_Texture* texture, const char* filepath,
                                    SG_TextureLoadDesc* desc);
void CQ_PushCommand_TextureFromRawData(SG_Texture* texture, unsigned char* buffer,
                                       int buffer_len, SG_TextureLoadDesc* desc);
void CQ_PushCommand_CubemapTextureFromFile(
  SG_Texture* texture, SG_TextureLoadDesc* desc, const char* right_face,
  const char* left_face, const char* top_face, const char* bottom_face,
  const char* back_face, const char* front_face);
void CQ_PushCommand_CopyTextureToTexture(SG_Texture* dst_texture,
                                         SG_Texture* src_texture,
                                         SG_TextureLocation* dst_location,
                                         SG_TextureLocation* src_location, int width,
                                         int height, int depth);
void CQ_PushCommand_CopyTextureToCPU(SG_Texture* texture);

// shader
void CQ_PushCommand_ShaderCreate(SG_Shader* shader);

// material
void CQ_PushCommand_MaterialCreate(SG_Material* material);
void CQ_PushCommand_MaterialUpdatePSO(SG_Material* material);
void CQ_PushCommand_MaterialSetUniform(SG_Material* material, int location);
void CQ_PushCommand_MaterialSetStorageBuffer(
  SG_Material* material, int location, Chuck_Object* ck_arr,
  SG_MaterialUniformType storage_buffer_type);

// mesh
void CQ_PushCommand_MeshUpdate(SG_Mesh* mesh);

// camera
void CQ_PushCommand_CameraCreate(SG_Camera* camera);
void CQ_PushCommand_CameraSetParams(SG_Camera* camera);

// text
void CQ_PushCommand_TextRebuild(SG_Text* text);
void CQ_PushCommand_TextDefaultFont(const char* font_path);

// pass
// void CQ_PushCommand_PassCreate(SG_Pass* pass);
void CQ_PushCommand_PassUpdate(SG_Pass* pass);
void CQ_PushCommand_PassConnect(SG_Pass* pass, SG_Pass* next_pass);
void CQ_PushCommand_PassDisconnect(SG_Pass* pass, SG_Pass* next_pass);

// b2
void CQ_PushCommand_b2World_Set(b2_SimulateDesc desc);

// buffer
void CQ_PushCommand_BufferUpdate(SG_Buffer* buffer);
void CQ_PushCommand_BufferWrite(SG_Buffer* buffer, Chuck_ArrayFloat* data,
                                u64 offset_bytes);

// light
void CQ_PushCommand_LightUpdate(SG_Light* light);

// shadow
void CQ_PushCommand_ShadowAddMesh(SG_Light* light, SG_Transform* xform,
                                  bool add_children, bool add);
void CQ_PushCommand_MeshSetShadowed(SG_Transform* xform, bool shadowed);

// video
void CQ_PushCommand_VideoUpdate(SG_Video* video);
void CQ_PushCommand_VideoSeek(SG_ID video_id, double time);
void CQ_PushCommand_VideoRate(SG_ID video_id, double rate, bool loop);

// webcam
void CQ_PushCommand_WebcamCreate(SG_Webcam* webcam, sr_webcam_device* device);
void CQ_PushCommand_WebcamUpdate(SG_Webcam* webcam);

// ============================================================================
// Commands from Graphics Thread --> Audio Thread
// ============================================================================

void CQ_PushCommand_G2A_TextureRead(SG_ID id, void* data, int size_bytes,
                                    WGPUBufferMapAsyncStatus status);