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
#include "sg_command.h"

#include "core/macros.h"
#include "core/spinlock.h"

// include for static assert
#include <type_traits>

// command queue
struct CQ {
    // command queue lock
    // only held when 1: adding new command and 2:
    // swapping the read/write queues
    spinlock write_q_lock;
    Arena* read_q;
    Arena* write_q;

    // TODO: test if toggle is faster
    // brings the command queue struct to below 64 bytes
    // b8 toggle;

    Arena cq_a = {};
    Arena cq_b = {};

    static void init(CQ* cq)
    {
        ASSERT(cq->cq_a.base == NULL);
        ASSERT(cq->cq_b.base == NULL);

        Arena::init(&cq->cq_a, MEGABYTE);
        Arena::init(&cq->cq_b, MEGABYTE);

        cq->read_q  = &cq->cq_a;
        cq->write_q = &cq->cq_b;
    }
};

// enforce it fits within a cache line
// static_assert(sizeof(CQ) <= 64);
// TODO: look into aligned_alloc for cache line alignment

static CQ audio_to_graphics_cq = {};
static CQ graphics_to_audio_cq = {};

// static Arena* _CQ_GetReadQ()
// {
//     return cq.toggle ? &cq.cq_a : &cq.cq_b;
// }

// static Arena* _CQ_GetWriteQ()
// {
//     return cq.toggle ? &cq.cq_b : &cq.cq_a;
// }

void CQ_Init()
{
    CQ::init(&audio_to_graphics_cq);
    CQ::init(&graphics_to_audio_cq);
}

// swap the command queue double buffer
static void _CQ_SwapQueuesImpl(CQ& cq)
{
    // assert read queue has been flushed before swapping
    ASSERT(cq.read_q->curr == 0);

    Arena* temp = cq.read_q;
    cq.read_q   = cq.write_q;

    spinlock::lock(&cq.write_q_lock);
    cq.write_q = temp;
    spinlock::unlock(&cq.write_q_lock);
}

void CQ_SwapQueues(bool which = false)
{
    if (which) {
        _CQ_SwapQueuesImpl(graphics_to_audio_cq);
    } else {
        _CQ_SwapQueuesImpl(audio_to_graphics_cq);
    }
}

static bool _CQ_ReadCommandQueueIterImpl(CQ& cq, SG_Command** command)
{
    // command queue empty
    if (cq.read_q->curr == 0) {
        *command = NULL;
        return false;
    }

    // return the first command in queue
    if (*command == NULL) {
        *command = (SG_Command*)cq.read_q->base;
        return true;
    }

    // sanity bounds check
    ASSERT(*command >= (SG_Command*)cq.read_q->base
           && *command < (SG_Command*)Arena::top(cq.read_q));

    // at last element
    if ((*command)->nextCommandOffset == cq.read_q->curr) {
        *command = NULL;
        return false;
    }

    // else return the nextOffset
    *command = (SG_Command*)Arena::get(cq.read_q, (*command)->nextCommandOffset);
    return true;
}

bool CQ_ReadCommandQueueIter(SG_Command** command, bool which = false)
{
    if (which)
        return _CQ_ReadCommandQueueIterImpl(graphics_to_audio_cq, command);
    else
        return _CQ_ReadCommandQueueIterImpl(audio_to_graphics_cq, command);
}

static void _CQ_ReadCommandQueueClearImpl(CQ& cq)
{
    Arena::clear(cq.read_q);
}

void CQ_ReadCommandQueueClear(bool which = false)
{
    if (which)
        _CQ_ReadCommandQueueClearImpl(graphics_to_audio_cq);
    else
        _CQ_ReadCommandQueueClearImpl(audio_to_graphics_cq);
}

static void* _CQ_ReadCommandGetOffsetImpl(CQ& cq, u64 byte_offset)
{
    return Arena::get(cq.read_q, byte_offset);
}

void* CQ_ReadCommandGetOffset(u64 byte_offset, bool which = false)
{
    if (which)
        return _CQ_ReadCommandGetOffsetImpl(graphics_to_audio_cq, byte_offset);
    else
        return _CQ_ReadCommandGetOffsetImpl(audio_to_graphics_cq, byte_offset);
}

// ============================================================================
// Command API
// ============================================================================

// hack to avoid having to pass the command queue around
#define cq audio_to_graphics_cq

#define BEGIN_COMMAND(cmd_type, cmd_enum)                                              \
    spinlock::lock(&cq.write_q_lock);                                                  \
    cmd_type* command = ARENA_PUSH_TYPE(cq.write_q, cmd_type);                         \
    command->type     = cmd_enum;

#define BEGIN_COMMAND_ADDITIONAL_MEMORY(cmd_type, cmd_enum, additional_bytes)          \
    spinlock::lock(&cq.write_q_lock);                                                  \
    cmd_type* command                                                                  \
      = (cmd_type*)Arena::push(cq.write_q, sizeof(cmd_type) + (additional_bytes));     \
    void* memory  = (void*)(command + 1);                                              \
    command->type = cmd_enum;

#define BEGIN_COMMAND_ADDITIONAL_MEMORY_ZERO(cmd_type, cmd_enum, additional_bytes)     \
    spinlock::lock(&cq.write_q_lock);                                                  \
    cmd_type* command                                                                  \
      = (cmd_type*)Arena::pushZero(cq.write_q, sizeof(cmd_type) + (additional_bytes)); \
    void* memory  = (void*)(command + 1);                                              \
    command->type = cmd_enum;

#define END_COMMAND()                                                                  \
    command->nextCommandOffset = cq.write_q->curr;                                     \
    spinlock::unlock(&cq.write_q_lock);

void CQ_PushCommand_SetFixedTimestep(int fps)
{
    BEGIN_COMMAND(SG_Command_SetFixedTimestep, SG_COMMAND_SET_FIXED_TIMESTEP);
    command->fps = fps;
    END_COMMAND();
}

void CQ_PushCommand_SetChuckVMInfo(int srate)
{
    BEGIN_COMMAND(SG_Command_SetChuckVMInfo, SG_COMMAND_SET_CHUCK_VM_INFO);
    command->srate = srate;
    END_COMMAND();
}

void CQ_PushCommand_WindowClose()
{
    BEGIN_COMMAND(SG_Command_WindowClose, SG_COMMAND_WINDOW_CLOSE);
    END_COMMAND();
}

void CQ_PushCommand_WindowMode(SG_WindowMode mode, int width, int height)
{
    BEGIN_COMMAND(SG_Command_WindowMode, SG_COMMAND_WINDOW_MODE);
    command->mode   = mode;
    command->width  = width;
    command->height = height;
    END_COMMAND();
}

void CQ_PushCommand_WindowSizeLimits(int min_width, int min_height, int max_width,
                                     int max_height, int aspect_ratio_x,
                                     int aspect_ratio_y)
{
    BEGIN_COMMAND(SG_Command_WindowSizeLimits, SG_COMMAND_WINDOW_SIZE_LIMITS);
    command->min_width      = min_width;
    command->min_height     = min_height;
    command->max_width      = max_width;
    command->max_height     = max_height;
    command->aspect_ratio_x = aspect_ratio_x;
    command->aspect_ratio_y = aspect_ratio_y;
    END_COMMAND();
}

void CQ_PushCommand_WindowPosition(int x, int y)
{
    BEGIN_COMMAND(SG_Command_WindowPosition, SG_COMMAND_WINDOW_POSITION);
    command->x = x;
    command->y = y;
    END_COMMAND();
}

void CQ_PushCommand_WindowCenter()
{
    BEGIN_COMMAND(SG_Command_WindowPosition, SG_COMMAND_WINDOW_CENTER);
    END_COMMAND();
}

// copies title into command arena
void CQ_PushCommand_WindowTitle(const char* title)
{
    size_t title_len = strlen(title);
    BEGIN_COMMAND_ADDITIONAL_MEMORY_ZERO(SG_Command_WindowTitle,
                                         SG_COMMAND_WINDOW_TITLE, title_len + 1);
    // copy title
    strncpy((char*)memory, title, title_len);

    // store offset not pointer in case arena resizes
    command->title_offset = Arena::offsetOf(cq.write_q, memory);

    END_COMMAND();
}

void CQ_PushCommand_WindowIconify(bool iconify)
{
    BEGIN_COMMAND(SG_Command_WindowIconify, SG_COMMAND_WINDOW_ICONIFY);
    command->iconify = iconify;
    END_COMMAND();
}

void CQ_PushCommand_WindowAttribute(CHUGL_WindowAttrib attrib, bool value)
{
    BEGIN_COMMAND(SG_Command_WindowAttribute, SG_COMMAND_WINDOW_ATTRIBUTE);
    command->attrib = attrib;
    command->value  = value;
    END_COMMAND();
}

void CQ_PushCommand_WindowOpacity(float opacity)
{
    BEGIN_COMMAND(SG_Command_WindowOpacity, SG_COMMAND_WINDOW_OPACITY);
    command->opacity = opacity;
    END_COMMAND();
}

void CQ_PushCommand_MouseMode(int mode)
{
    BEGIN_COMMAND(SG_Command_MouseMode, SG_COMMAND_MOUSE_MODE);
    command->mode = mode;
    END_COMMAND();
}

void CQ_PushCommand_MouseCursorNormal()
{
    BEGIN_COMMAND(SG_Command_MouseCursorNormal, SG_COMMAND_MOUSE_CURSOR_NORMAL);
    END_COMMAND();
}

void CQ_PushCommand_MouseCursor(CK_DL_API API, Chuck_ArrayInt* image_data, u32 width,
                                u32 height, u32 xhot, u32 yhot)
{
    u32 data_size = API->object->array_int_size(image_data);
    ASSERT(data_size == width * height * 4);

    BEGIN_COMMAND_ADDITIONAL_MEMORY(SG_Command_MouseCursor, SG_COMMAND_MOUSE_CURSOR,
                                    data_size);
    // push bytes for pixel data
    char* image_data_bytes = (char*)memory;
    // copy
    for (u32 i = 0; i < data_size; i++) {
        image_data_bytes[i]
          = (unsigned char)CLAMP(API->object->array_int_get_idx(image_data, i), 0, 255);
    }
    // store offset not pointer in case arena resizes
    command->mouse_cursor_image_offset = Arena::offsetOf(cq.write_q, image_data_bytes);
    command->width                     = width;
    command->height                    = height;
    command->xhot                      = xhot;
    command->yhot                      = yhot;
    command->type                      = SG_COMMAND_MOUSE_CURSOR;
    END_COMMAND();
}

void CQ_PushCommand_UI_Disabled(bool disabled)
{
    BEGIN_COMMAND(SG_Command_UI_Disabled, SG_COMMAND_UI_DISABLED);
    command->disabled = disabled;
    END_COMMAND();
}

void CQ_PushCommand_ComponentUpdateName(SG_Component* component)
{
    int max_name_len = sizeof(component->name);
    BEGIN_COMMAND_ADDITIONAL_MEMORY_ZERO(
      SG_Command_ComponentUpdateName, SG_COMMAND_COMPONENT_UPDATE_NAME, max_name_len);

    // copy string
    strncpy((char*)memory, component->name, max_name_len);
    command->sg_id       = component->id;
    command->name_offset = Arena::offsetOf(cq.write_q, memory);
    END_COMMAND();
}

void CQ_PushCommand_ComponentFree(SG_Component* component)
{
    BEGIN_COMMAND(SG_Command_ComponentFree, SG_COMMAND_COMPONENT_FREE);
    command->id = component->id;
    END_COMMAND();
}

void CQ_PushCommand_CreateTransform(SG_Transform* xform)
{
    BEGIN_COMMAND(SG_Command_CreateXform, SG_COMMAND_CREATE_XFORM);
    command->sg_id = xform->id;
    command->pos   = xform->pos;
    command->rot   = xform->rot;
    command->sca   = xform->sca;
    END_COMMAND();
}

void CQ_PushCommand_AddChild(SG_Transform* parent, SG_Transform* child)
{
    // execute change on audio thread side
    SG_Transform::addChild(parent, child);

    spinlock::lock(&cq.write_q_lock);
    {
        // allocate memory
        SG_Command_AddChild* command = ARENA_PUSH_TYPE(cq.write_q, SG_Command_AddChild);

        // initialize memory
        command->type              = SG_COMMAND_ADD_CHILD;
        command->nextCommandOffset = cq.write_q->curr;
        command->parent_id         = parent->id;
        command->child_id          = child->id;
    }
    spinlock::unlock(&cq.write_q_lock);
}

void CQ_PushCommand_RemoveChild(SG_Transform* parent, SG_Transform* child)
{
    if (parent == NULL || child == NULL) return;

    // execute change on audio thread side
    SG_Transform::removeChild(parent, child);

    spinlock::lock(&cq.write_q_lock);
    {
        // allocate memory
        SG_Command_RemoveChild* command
          = ARENA_PUSH_TYPE(cq.write_q, SG_Command_RemoveChild);

        // initialize memory
        command->type              = SG_COMMAND_REMOVE_CHILD;
        command->nextCommandOffset = cq.write_q->curr;
        command->parent            = parent->id;
        command->child             = child->id;
    }
    spinlock::unlock(&cq.write_q_lock);
}

void CQ_PushCommand_RemoveAllChildren(SG_Transform* parent)
{
    BEGIN_COMMAND(SG_Command_RemoveAllChildren, SG_COMMAND_REMOVE_ALL_CHILDREN);
    command->parent = parent->id;
    END_COMMAND();
}

void CQ_PushCommand_SetPosition(SG_Transform* xform)
{
    spinlock::lock(&cq.write_q_lock);
    {
        // allocate memory
        SG_Command_SetPosition* command
          = ARENA_PUSH_TYPE(cq.write_q, SG_Command_SetPosition);

        // initialize memory
        command->type              = SG_COMMAND_SET_POSITION;
        command->nextCommandOffset = cq.write_q->curr;
        command->sg_id             = xform->id;
        command->pos               = xform->pos;
    }
    spinlock::unlock(&cq.write_q_lock);
}

void CQ_PushCommand_SetRotation(SG_Transform* xform)
{
    spinlock::lock(&cq.write_q_lock);
    {
        // allocate memory
        SG_Command_SetRotation* command
          = ARENA_PUSH_TYPE(cq.write_q, SG_Command_SetRotation);

        // initialize memory
        command->type              = SG_COMMAND_SET_ROTATATION;
        command->nextCommandOffset = cq.write_q->curr;
        command->sg_id             = xform->id;
        command->rot               = xform->rot;
    }
    spinlock::unlock(&cq.write_q_lock);
}

void CQ_PushCommand_SetScale(SG_Transform* xform)
{
    spinlock::lock(&cq.write_q_lock);
    {
        // allocate memory
        SG_Command_SetScale* command = ARENA_PUSH_TYPE(cq.write_q, SG_Command_SetScale);

        // initialize memory
        command->type              = SG_COMMAND_SET_SCALE;
        command->nextCommandOffset = cq.write_q->curr;
        command->sg_id             = xform->id;
        command->sca               = xform->sca;
    }
    spinlock::unlock(&cq.write_q_lock);
}

void CQ_PushCommand_SceneUpdate(SG_Scene* scene)
{
    BEGIN_COMMAND(SG_Command_SceneUpdate, SG_COMMAND_SCENE_UPDATE);
    command->sg_id = scene->id;
    command->desc  = scene->desc;
    END_COMMAND();
}

void CQ_PushCommand_GeometryCreate(SG_Geometry* geo)
{
    BEGIN_COMMAND(SG_Command_GeoCreate, SG_COMMAND_GEO_CREATE);
    command->sg_id = geo->id;
    END_COMMAND();
}

// copies data pointer into command arena. does NOT take ownership
// assumes data is 4bytes (i32 or f32)
// data_len is the number of elements in data array, NOT size in bytes
void CQ_PushCommand_GeometrySetVertexAttribute(SG_Geometry* geo, int location,
                                               int num_components, void* data,
                                               int data_size_bytes)
{
    if (data == NULL || data_size_bytes == 0 || num_components == 0) return;

    BEGIN_COMMAND_ADDITIONAL_MEMORY(SG_Command_GeoSetVertexAttribute,
                                    SG_COMMAND_GEO_SET_VERTEX_ATTRIBUTE,
                                    data_size_bytes);

    // get cq memory for vertex data
    f32* attribute_data = (f32*)memory;
    memcpy(attribute_data, data, data_size_bytes);

    command->sg_id           = geo->id;
    command->num_components  = num_components;
    command->location        = location;
    command->data_size_bytes = data_size_bytes;
    command->data_offset     = Arena::offsetOf(cq.write_q, attribute_data);

    ASSERT((data_size_bytes % 4) == 0);
    ASSERT((data_size_bytes / 4) % num_components == 0);

    END_COMMAND();
}

void CQ_PushCommand_GeometrySetIndices(SG_Geometry* geo, u32* indices, int index_count)
{
    BEGIN_COMMAND_ADDITIONAL_MEMORY(SG_Command_GeoSetIndices,
                                    SG_COMMAND_GEO_SET_INDICES,
                                    index_count * sizeof(*indices));

    u32* index_data = (u32*)memory;
    memcpy(index_data, indices, index_count * sizeof(*indices));

    command->sg_id          = geo->id;
    command->index_count    = index_count;
    command->indices_offset = Arena::offsetOf(cq.write_q, index_data);

    END_COMMAND();
}

void CQ_PushCommand_GeometrySetPulledVertexAttribute(SG_Geometry* geo, int location,
                                                     void* data, size_t bytes)
{
    BEGIN_COMMAND_ADDITIONAL_MEMORY(SG_Command_GeometrySetPulledVertexAttribute,
                                    SG_COMMAND_GEO_SET_PULLED_VERTEX_ATTRIBUTE, bytes);

    u8* attribute_data = (u8*)memory;
    if (bytes && data) {
        memcpy(attribute_data, data, bytes);
    }

    command->sg_id       = geo->id;
    command->location    = location;
    command->data_bytes  = bytes;
    command->data_offset = Arena::offsetOf(cq.write_q, attribute_data);
    END_COMMAND();
}

void CQ_PushCommand_GeometrySetVertexCount(SG_Geometry* geo, int count)
{
    BEGIN_COMMAND(SG_Command_GeometrySetVertexCount, SG_COMMAND_GEO_SET_VERTEX_COUNT);
    command->sg_id = geo->id;
    command->count = count;
    END_COMMAND();
}

void CQ_PushCommand_GeometrySetIndicesCount(SG_Geometry* geo, int count)
{
    BEGIN_COMMAND(SG_Command_GeometrySetIndicesCount, SG_COMMAND_GEO_SET_INDICES_COUNT);
    command->sg_id = geo->id;
    command->count = count;
    END_COMMAND();
}

// Textures ====================================================================

// maybe change to TextureUpdate + lazy creation to support mutable texture
// formats/usage/dimension
void CQ_PushCommand_TextureCreate(SG_Texture* texture)
{
    BEGIN_COMMAND(SG_Command_TextureCreate, SG_COMMAND_TEXTURE_CREATE);
    command->sg_id = texture->id;
    command->desc  = texture->desc;
    COPY_STRING(command->name, texture->name);
    END_COMMAND();
}

void CQ_PushCommand_TextureWrite(SG_Texture* texture, SG_TextureWriteDesc* desc,
                                 Chuck_ArrayFloat* ck_array, CK_DL_API API)
{
    // calculate the necessary size in bytes from data (assume data length has already
    // been validated)
    int write_region_num_texels = desc->width * desc->height * desc->depth;
    int bytes_per_texel         = SG_Texture_byteSizePerTexel(texture->desc.format);
    int write_region_num_components
      = write_region_num_texels
        * SG_Texture_numComponentsPerTexel(texture->desc.format);
    int write_size_bytes = write_region_num_texels * bytes_per_texel;

    BEGIN_COMMAND_ADDITIONAL_MEMORY(SG_Command_TextureWrite, SG_COMMAND_TEXTURE_WRITE,
                                    write_size_bytes);

    command->sg_id           = texture->id;
    command->write_desc      = *desc;
    command->data_size_bytes = write_size_bytes;
    command->data_offset     = Arena::offsetOf(cq.write_q, memory);

    // copy texture data to write_q
#define CQ_TEXTURE_WRITE(type, type_max)                                               \
    ASSERT(write_region_num_components <= API->object->array_float_size(ck_array));    \
    type* pixel_data = (type*)memory;                                                  \
    for (int i = 0; i < write_region_num_components; i++) {                            \
        pixel_data[i]                                                                  \
          = (type)(type_max * API->object->array_float_get_idx(ck_array, i));          \
    }

    switch (texture->desc.format) {
        case WGPUTextureFormat_RGBA8Unorm: {
            CQ_TEXTURE_WRITE(u8, UINT8_MAX);
        } break;
        case WGPUTextureFormat_RGBA16Float: {
            ASSERT(false); // not impl
        } break;
        case WGPUTextureFormat_R32Float:
        case WGPUTextureFormat_RGBA32Float: {
            CQ_TEXTURE_WRITE(f32, 1.0f);
        } break;
        default: ASSERT(false);
    }

    END_COMMAND();
#undef CQ_TEXTURE_WRITE
}

void CQ_PushCommand_TextureWriteExternalPtr(SG_Texture* texture,
                                            SG_TextureWriteDesc* desc, void* data)
{
    // calculate the necessary size in bytes from data (assume data length has already
    // been validated)
    int write_region_num_texels = desc->width * desc->height * desc->depth;
    int bytes_per_texel         = SG_Texture_byteSizePerTexel(texture->desc.format);
    int write_size_bytes        = write_region_num_texels * bytes_per_texel;

    BEGIN_COMMAND_ADDITIONAL_MEMORY(SG_Command_TextureWrite, SG_COMMAND_TEXTURE_WRITE,
                                    write_size_bytes);

    command->sg_id           = texture->id;
    command->write_desc      = *desc;
    command->data_size_bytes = write_size_bytes;
    command->data_offset     = Arena::offsetOf(cq.write_q, memory);

    memcpy(memory, data, write_size_bytes);
    END_COMMAND();
}

void CQ_PushCommand_TextureFromFile(SG_Texture* texture, const char* filepath,
                                    SG_TextureLoadDesc* desc)
{
    size_t filepath_len = filepath ? strlen(filepath) : 0;
    BEGIN_COMMAND_ADDITIONAL_MEMORY_ZERO(
      SG_Command_TextureFromFile, SG_COMMAND_TEXTURE_FROM_FILE, filepath_len + 1);
    command->sg_id      = texture->id;
    char* filepath_copy = (char*)memory;
    strncpy(filepath_copy, filepath, strlen(filepath));
    command->filepath_offset = Arena::offsetOf(cq.write_q, filepath_copy);
    command->flip_vertically = desc->flip_y;
    command->gen_mips        = desc->gen_mips;
    END_COMMAND();
}

void CQ_PushCommand_TextureFromRawData(SG_Texture* texture, u8* buffer, int buffer_len,
                                       SG_TextureLoadDesc* desc)
{
    BEGIN_COMMAND_ADDITIONAL_MEMORY(SG_Command_TextureFromRawData,
                                    SG_COMMAND_TEXTURE_FROM_RAW_DATA, buffer_len);
    u8* buffer_copy = (u8*)memory;
    memcpy(buffer_copy, buffer, buffer_len);
    command->sg_id           = texture->id;
    command->buffer_len      = buffer_len;
    command->buffer_offset   = Arena::offsetOf(cq.write_q, buffer_copy);
    command->flip_vertically = desc->flip_y ? 1 : 0;
    command->gen_mips        = desc->gen_mips ? 1 : 0;
    END_COMMAND();
}

void CQ_PushCommand_CubemapTextureFromFile(
  SG_Texture* texture, SG_TextureLoadDesc* desc, const char* right_face,
  const char* left_face, const char* top_face, const char* bottom_face,
  const char* back_face, const char* front_face)
{
    size_t right_face_len  = right_face ? strlen(right_face) : 0;
    size_t left_face_len   = left_face ? strlen(left_face) : 0;
    size_t top_face_len    = top_face ? strlen(top_face) : 0;
    size_t bottom_face_len = bottom_face ? strlen(bottom_face) : 0;
    size_t back_face_len   = back_face ? strlen(back_face) : 0;
    size_t front_face_len  = front_face ? strlen(front_face) : 0;
    BEGIN_COMMAND_ADDITIONAL_MEMORY_ZERO(
      SG_Command_CubemapTextureFromFile, SG_COMMAND_CUBEMAP_TEXTURE_FROM_FILE,
      right_face_len + left_face_len + top_face_len + bottom_face_len + back_face_len
        + front_face_len + 6);

    command->sg_id = texture->id;

    char* right_face_copy  = (char*)memory;
    char* left_face_copy   = right_face_copy + right_face_len + 1;
    char* top_face_copy    = left_face_copy + left_face_len + 1;
    char* bottom_face_copy = top_face_copy + top_face_len + 1;
    char* back_face_copy   = bottom_face_copy + bottom_face_len + 1;
    char* front_face_copy  = back_face_copy + back_face_len + 1;
    strncpy(right_face_copy, right_face, right_face_len);
    strncpy(left_face_copy, left_face, left_face_len);
    strncpy(top_face_copy, top_face, top_face_len);
    strncpy(bottom_face_copy, bottom_face, bottom_face_len);
    strncpy(back_face_copy, back_face, back_face_len);
    strncpy(front_face_copy, front_face, front_face_len);
    command->right_face_offset  = Arena::offsetOf(cq.write_q, right_face_copy);
    command->left_face_offset   = Arena::offsetOf(cq.write_q, left_face_copy);
    command->top_face_offset    = Arena::offsetOf(cq.write_q, top_face_copy);
    command->bottom_face_offset = Arena::offsetOf(cq.write_q, bottom_face_copy);
    command->back_face_offset   = Arena::offsetOf(cq.write_q, back_face_copy);
    command->front_face_offset  = Arena::offsetOf(cq.write_q, front_face_copy);

    command->flip_vertically = desc->flip_y;
    END_COMMAND();
}

void CQ_PushCommand_CopyTextureToTexture(SG_Texture* dst_texture,
                                         SG_Texture* src_texture,
                                         SG_TextureLocation* dst_location,
                                         SG_TextureLocation* src_location, int width,
                                         int height, int depth)
{
    BEGIN_COMMAND(SG_Command_CopyTextureToTexture, SG_COMMAND_COPY_TEXTURE_TO_TEXTURE);
    command->dst_texture_id = dst_texture->id;
    command->src_texture_id = src_texture->id;
    command->dst_location   = *dst_location;
    command->src_location   = *src_location;
    command->width          = width;
    command->height         = height;
    command->depth          = depth;
    END_COMMAND();
}

void CQ_PushCommand_CopyTextureToCPU(SG_Texture* texture)
{
    // TODO malloc memory for data
    BEGIN_COMMAND(SG_Command_CopyTextureToCPU, SG_COMMAND_COPY_TEXTURE_TO_CPU);
    command->id = texture->id;
    END_COMMAND();
}

// Shader ======================================================================

void CQ_PushCommand_ShaderCreate(SG_Shader* shader)
{
    const char* safe_vertex_filepath
      = shader->vertex_filepath_owned ? shader->vertex_filepath_owned : "";
    const char* safe_fragment_filepath
      = shader->fragment_filepath_owned ? shader->fragment_filepath_owned : "";
    const char* safe_vertex_string
      = shader->vertex_string_owned ? shader->vertex_string_owned : "";
    const char* safe_fragment_string
      = shader->fragment_string_owned ? shader->fragment_string_owned : "";
    const char* safe_compute_string
      = shader->compute_string_owned ? shader->compute_string_owned : "";
    const char* safe_compute_filepath
      = shader->compute_filepath_owned ? shader->compute_filepath_owned : "";

    size_t vertex_filepath_len   = strlen(safe_vertex_filepath) + 1;
    size_t fragment_filepath_len = strlen(safe_fragment_filepath) + 1;
    size_t vertex_string_len     = strlen(safe_vertex_string) + 1;
    size_t fragment_string_len   = strlen(safe_fragment_string) + 1;
    size_t compute_string_len    = strlen(safe_compute_string) + 1;
    size_t compute_filepath_len  = strlen(safe_compute_filepath) + 1;

    size_t additional_memory = vertex_filepath_len + fragment_filepath_len
                               + vertex_string_len + fragment_string_len
                               + compute_string_len + compute_filepath_len;

    BEGIN_COMMAND_ADDITIONAL_MEMORY_ZERO(SG_Command_ShaderCreate,
                                         SG_COMMAND_SHADER_CREATE, additional_memory);

    command->sg_id = shader->id;

    char* vertex_filepath   = (char*)memory;
    char* fragment_filepath = vertex_filepath + vertex_filepath_len;
    char* vertex_string     = fragment_filepath + fragment_filepath_len;
    char* fragment_string   = vertex_string + vertex_string_len;
    char* compute_string    = fragment_string + fragment_string_len;
    char* compute_filepath  = compute_string + compute_string_len;

    // copy strings (leaving space for null terminators)
    strncpy(vertex_filepath, safe_vertex_filepath, vertex_filepath_len - 1);
    strncpy(fragment_filepath, safe_fragment_filepath, fragment_filepath_len - 1);
    strncpy(vertex_string, safe_vertex_string, vertex_string_len - 1);
    strncpy(fragment_string, safe_fragment_string, fragment_string_len - 1);
    strncpy(compute_string, safe_compute_string, compute_string_len - 1);
    strncpy(compute_filepath, safe_compute_filepath, compute_filepath_len - 1);

    // set offsets
    command->vertex_filepath_offset   = Arena::offsetOf(cq.write_q, vertex_filepath);
    command->fragment_filepath_offset = Arena::offsetOf(cq.write_q, fragment_filepath);
    command->vertex_string_offset     = Arena::offsetOf(cq.write_q, vertex_string);
    command->fragment_string_offset   = Arena::offsetOf(cq.write_q, fragment_string);
    command->compute_string_offset    = Arena::offsetOf(cq.write_q, compute_string);
    command->compute_filepath_offset  = Arena::offsetOf(cq.write_q, compute_filepath);

    ASSERT(sizeof(shader->vertex_layout) == sizeof(command->vertex_layout));
    memcpy(command->vertex_layout, shader->vertex_layout,
           sizeof(shader->vertex_layout));

    command->includes = shader->includes;

    END_COMMAND();
}

void CQ_PushCommand_MaterialCreate(SG_Material* material)
{
    BEGIN_COMMAND(SG_Command_MaterialCreate, SG_COMMAND_MATERIAL_CREATE);
    command->material_type = material->material_type; // TODO remove?
    command->sg_id         = material->id;
    command->pso           = material->pso;
    // command->params        = material->params;
    END_COMMAND();
}

void CQ_PushCommand_MaterialUpdatePSO(SG_Material* material)
{
    BEGIN_COMMAND(SG_Command_MaterialUpdatePSO, SG_COMMAND_MATERIAL_UPDATE_PSO);
    command->sg_id = material->id;
    command->pso   = material->pso;
    END_COMMAND();
}

void CQ_PushCommand_MaterialSetUniform(SG_Material* material, int location)
{
    BEGIN_COMMAND(SG_Command_MaterialSetUniform, SG_COMMAND_MATERIAL_SET_UNIFORM);
    command->sg_id    = material->id;
    command->uniform  = material->uniforms[location];
    command->location = location;
    END_COMMAND();
}

void CQ_PushCommand_MaterialSetStorageBuffer(SG_Material* material, int location,
                                             Chuck_Object* ck_arr,
                                             SG_MaterialUniformType storage_buffer_type)
{
    int data_count = 0;
    switch (storage_buffer_type) {
        case SG_MATERIAL_UNIFORM_FLOAT: {
            data_count
              = g_chuglAPI->object->array_float_size((Chuck_ArrayFloat*)ck_arr);
        } break;
        case SG_MATERIAL_UNIFORM_VEC2F: {
            data_count
              = 2 * g_chuglAPI->object->array_vec2_size((Chuck_ArrayVec2*)ck_arr);
        } break;
        case SG_MATERIAL_UNIFORM_VEC3F: {
            data_count
              = 3 * g_chuglAPI->object->array_vec3_size((Chuck_ArrayVec3*)ck_arr);
        } break;
        case SG_MATERIAL_UNIFORM_VEC4F: {
            data_count
              = 4 * g_chuglAPI->object->array_vec4_size((Chuck_ArrayVec4*)ck_arr);
        } break;
        case SG_MATERIAL_UNIFORM_INT: {
            data_count = g_chuglAPI->object->array_int_size((Chuck_ArrayInt*)ck_arr);
        } break;
        default: ASSERT(false); // unsupported type
    }
    BEGIN_COMMAND_ADDITIONAL_MEMORY(SG_Command_MaterialSetStorageBuffer,
                                    SG_COMMAND_MATERIAL_SET_STORAGE_BUFFER,
                                    data_count * sizeof(f32));
    command->sg_id           = material->id;
    command->location        = location;
    command->data_size_bytes = data_count * sizeof(f32);
    switch (storage_buffer_type) {
        case SG_MATERIAL_UNIFORM_FLOAT: {
            chugin_copyCkFloatArray((Chuck_ArrayFloat*)ck_arr, (f32*)memory,
                                    data_count);
        } break;
        case SG_MATERIAL_UNIFORM_VEC2F: {
            chugin_copyCkVec2Array((Chuck_ArrayVec2*)ck_arr, (f32*)memory);
        } break;
        case SG_MATERIAL_UNIFORM_VEC3F: {
            chugin_copyCkVec3Array((Chuck_ArrayVec3*)ck_arr, (f32*)memory);
        } break;
        case SG_MATERIAL_UNIFORM_VEC4F: {
            chugin_copyCkVec4Array((Chuck_ArrayVec4*)ck_arr, (f32*)memory);
        } break;
        case SG_MATERIAL_UNIFORM_INT: {
            chugin_copyCkIntArray((Chuck_ArrayInt*)ck_arr, (i32*)memory, data_count);
        } break;
        default: ASSERT(false); // unsupported type
    }
    command->data_offset = Arena::offsetOf(cq.write_q, memory);
    END_COMMAND();
}

void CQ_PushCommand_MeshUpdate(SG_Mesh* mesh)
{
    BEGIN_COMMAND(SG_Command_MeshUpdate, SG_COMMAND_MESH_UPDATE);
    command->mesh_id = mesh->id;
    command->geo_id  = mesh->_geo_id;
    command->mat_id  = mesh->_mat_id;
    END_COMMAND();
}

void CQ_PushCommand_CameraCreate(SG_Camera* camera)
{
    BEGIN_COMMAND(SG_Command_CameraCreate, SG_COMMAND_CAMERA_CREATE);
    command->camera = *camera;
    END_COMMAND();
}

void CQ_PushCommand_CameraSetParams(SG_Camera* cam)
{
    BEGIN_COMMAND(SG_Command_CameraSetParams, SG_COMMAND_CAMERA_SET_PARAMS);
    command->camera_id = cam->id;
    command->params    = cam->params;
    END_COMMAND();
}

void CQ_PushCommand_TextRebuild(SG_Text* text)
{
    size_t additional_bytes = text->text.length() + 1 + text->font_path.length() + 1;
    ASSERT(text->_mat_id != 0);

    BEGIN_COMMAND_ADDITIONAL_MEMORY_ZERO(SG_Command_TextRebuild,
                                         SG_COMMAND_TEXT_REBUILD, additional_bytes);
    command->text_id          = text->id;
    command->material_id      = text->_mat_id;
    command->control_point    = { text->control_points.x, text->control_points.y };
    command->vertical_spacing = text->vertical_spacing;

    char* text_copy = (char*)memory;
    char* font_path = text_copy + text->text.length() + 1;

    // copy strings
    strncpy(text_copy, text->text.c_str(), text->text.length());
    strncpy(font_path, text->font_path.c_str(), text->font_path.length());

    command->text_str_offset      = Arena::offsetOf(cq.write_q, text_copy);
    command->font_path_str_offset = Arena::offsetOf(cq.write_q, font_path);
    END_COMMAND();
}

void CQ_PushCommand_TextDefaultFont(const char* font_path)
{
    size_t len = font_path ? strlen(font_path) : 0;
    BEGIN_COMMAND_ADDITIONAL_MEMORY_ZERO(SG_Command_TextDefaultFont,
                                         SG_COMMAND_TEXT_DEFAULT_FONT, len + 1);
    // if (font_path) strncpy((char*)memory, font_path, additional_bytes - 1);
    if (font_path) strncpy((char*)memory, font_path, len);
    command->font_path_str_offset = Arena::offsetOf(cq.write_q, memory);
    END_COMMAND();
}

void CQ_PushCommand_PassCreate(SG_Pass* pass)
{
    BEGIN_COMMAND(SG_Command_PassCreate, SG_COMMAND_PASS_CREATE);
    command->pass_id   = pass->id;
    command->pass_type = pass->pass_type;
    END_COMMAND();
}

void CQ_PushCommand_PassUpdate(SG_Pass* pass)
{
    BEGIN_COMMAND(SG_Command_PassUpdate, SG_COMMAND_PASS_UPDATE);
    memcpy(&command->pass, pass, sizeof(*pass));
    END_COMMAND();
}

void CQ_PushCommand_PassConnect(SG_Pass* pass, SG_Pass* next_pass)
{
    BEGIN_COMMAND(SG_Command_PassConnect, SG_COMMAND_PASS_CONNECT);
    command->pass_id      = pass ? pass->id : 0;
    command->next_pass_id = next_pass ? next_pass->id : 0;
    END_COMMAND();
}

void CQ_PushCommand_PassDisconnect(SG_Pass* pass, SG_Pass* next_pass)
{
    BEGIN_COMMAND(SG_Command_PassConnect, SG_COMMAND_PASS_DISCONNECT);
    command->pass_id      = pass ? pass->id : 0;
    command->next_pass_id = next_pass ? next_pass->id : 0;
    END_COMMAND();
}

void CQ_PushCommand_b2World_Set(b2_SimulateDesc desc)
{
    spinlock::lock(&cq.write_q_lock);
    {
        // allocate memory
        SG_Command_b2World_Set* command
          = ARENA_PUSH_TYPE(cq.write_q, SG_Command_b2World_Set);

        // initialize memory
        command->type              = SG_COMMAND_b2_WORLD_SET;
        command->nextCommandOffset = cq.write_q->curr;
        command->desc              = desc;
    }
    spinlock::unlock(&cq.write_q_lock);
}

void CQ_PushCommand_BufferUpdate(SG_Buffer* buffer)
{
    BEGIN_COMMAND(SG_Command_BufferUpdate, SG_COMMAND_BUFFER_UPDATE);
    command->buffer_id = buffer->id;
    command->desc      = buffer->desc;
    END_COMMAND();
}

void CQ_PushCommand_BufferWrite(SG_Buffer* buffer, Chuck_ArrayFloat* data,
                                u64 offset_bytes)
{
    int data_count       = g_chuglAPI->object->array_float_size(data);
    int additional_bytes = data_count * sizeof(f32);
    BEGIN_COMMAND_ADDITIONAL_MEMORY(SG_Command_BufferWrite, SG_COMMAND_BUFFER_WRITE,
                                    additional_bytes);
    command->buffer_id       = buffer->id;
    command->offset_bytes    = offset_bytes;
    command->data_size_bytes = additional_bytes;
    f32* data_ptr            = (f32*)memory;
    chugin_copyCkFloatArray(data, data_ptr, data_count);
    command->data_offset = Arena::offsetOf(cq.write_q, data_ptr);
    END_COMMAND();
}

void CQ_PushCommand_LightUpdate(SG_Light* light)
{
    BEGIN_COMMAND(SG_Command_LightUpdate, SG_COMMAND_LIGHT_UPDATE);
    command->light_id = light->id;
    command->desc     = light->desc;
    END_COMMAND();
}

void CQ_PushCommand_ShadowAddMesh(SG_Light* light, SG_Transform* xform,
                                  bool add_children, bool add)
{
    if (light == NULL || xform == NULL) return;

    BEGIN_COMMAND(SG_Command_ShadowAddMesh, SG_COMMAND_SHADOW_ADD_MESH);
    command->add                        = add;
    command->light_id                   = light->id;
    command->mesh_id_list_offset        = cq.write_q->curr;
    *ARENA_PUSH_TYPE(cq.write_q, SG_ID) = xform->id;

    // ==optimize== when refactoring scenegraph to use linked list to connect children,
    // only add the XForms which are actually GMeshs
    if (add_children) { // BFS add all children
        u64 curr = cq.write_q->curr;
        memcpy(Arena::push(cq.write_q, xform->childrenIDs.curr),
               xform->childrenIDs.base, xform->childrenIDs.curr);

        while (curr != cq.write_q->curr) {
            xform = SG_GetTransform(*(SG_ID*)Arena::get(cq.write_q, curr));
            ASSERT(xform);
            curr += sizeof(SG_ID);

            memcpy(Arena::push(cq.write_q, xform->childrenIDs.curr),
                   xform->childrenIDs.base, xform->childrenIDs.curr);
        }
    }
    command->mesh_id_list_len
      = (cq.write_q->curr - command->mesh_id_list_offset) / sizeof(SG_ID);

    END_COMMAND();
}

void CQ_PushCommand_MeshSetShadowed(SG_Transform* xform, bool shadowed)
{
    if (xform->type != SG_COMPONENT_MESH) return;
    SG_Mesh* mesh          = (SG_Mesh*)xform;
    mesh->receives_shadows = shadowed;

    BEGIN_COMMAND(SG_Command_MeshSetShadowed, SG_COMMAND_MESH_SET_SHADOWED);
    command->mesh_id  = mesh->id;
    command->shadowed = mesh->receives_shadows;
    END_COMMAND();
}

void CQ_PushCommand_VideoUpdate(SG_Video* video)
{
    const char* path = video->path_OWNED ? video->path_OWNED : "";
    int path_len     = strlen(path) + 1;
    BEGIN_COMMAND_ADDITIONAL_MEMORY_ZERO(SG_Command_VideoUpdate,
                                         SG_COMMAND_VIDEO_UPDATE, path_len);
    command->video_id              = video->id;
    command->rgba_video_texture_id = video->video_texture_rgba_id;
    command->path_offset           = Arena::offsetOf(cq.write_q, memory);
    strncpy((char*)memory, path, path_len);
    END_COMMAND();
}

void CQ_PushCommand_VideoSeek(SG_ID video_id, double time)
{
    BEGIN_COMMAND(SG_Command_VideoSeek, SG_COMMAND_VIDEO_SEEK);
    command->video_id  = video_id;
    command->time_secs = time;
    END_COMMAND();
}

void CQ_PushCommand_VideoRate(SG_ID video_id, double rate, bool loop)
{
    BEGIN_COMMAND(SG_Command_VideoRate, SG_COMMAND_VIDEO_RATE);
    command->video_id = video_id;
    command->rate     = rate;
    command->loop     = loop;
    END_COMMAND();
}

void CQ_PushCommand_WebcamCreate(SG_Webcam* webcam, sr_webcam_device* device)
{
    BEGIN_COMMAND(SG_Command_WebcamCreate, SG_COMMAND_WEBCAM_CREATE);
    command->webcam_id         = webcam->id;
    command->webcam_texture_id = webcam->texture_id;
    command->device            = device;
    command->device_id         = webcam->device_id;
    END_COMMAND();
}

void CQ_PushCommand_WebcamUpdate(SG_Webcam* webcam)
{
    BEGIN_COMMAND(SG_Command_WebcamUpdate, SG_COMMAND_WEBCAM_UPDATE);
    command->webcam_id = webcam->id;
    command->freeze    = webcam->freeze;
    command->capture   = webcam->capture;
    END_COMMAND();
}

#undef cq

// ============================================================================
// Graphics to Audio Commands
// ============================================================================

#define cq graphics_to_audio_cq

void CQ_PushCommand_G2A_TextureRead(SG_ID id, void* data, int size_bytes,
                                    WGPUBufferMapAsyncStatus status)
{
    BEGIN_COMMAND(SG_Command_G2A_TextureRead, SG_COMMAND_G2A_TEXTURE_READ);
    command->texture_id = id;
    command->data_OWNED = data;
    command->size_bytes = size_bytes;
    command->status     = status;
    END_COMMAND();
}

#undef cq
