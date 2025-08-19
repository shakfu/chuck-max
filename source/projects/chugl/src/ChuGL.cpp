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

// clang-format off
#include "all.cpp"
#include "ulib_helper.h"

// ulibs
#include "ulib_color.cpp"
#include "ulib_box2d.cpp"
#include "ulib_component.cpp"
#include "ulib_camera.cpp"
#include "ulib_scene.cpp"
#include "ulib_geometry.cpp"
#include "ulib_imgui.cpp"
#include "ulib_material.cpp"
#include "ulib_texture.cpp"
#include "ulib_text.cpp"
#include "ulib_window.cpp"
#include "ulib_pass.cpp"
#include "ulib_buffer.cpp"
#include "ulib_light.cpp"

#include "ulib_video.cpp"

#ifndef CHUGL_FAST_COMPILE
#include "ulib_assloader.cpp"
#endif


// vendor
#include <sokol/sokol_time.h>
// clang-format on

static f64 ckdt_sec      = 0;
static f64 system_dt_sec = 0;
static App chugl_app     = {};

t_CKBOOL chugl_main_loop_hook(void* bindle)
{
    UNUSED_VAR(bindle);

    ASSERT(g_chuglAPI && g_chuglVM);

    App::init(&chugl_app, g_chuglVM, g_chuglAPI);
    App::start(&chugl_app); // blocking

    { // cleanup (after exiting main loop)
        // remove all shreds (should trigger shutdown, unless running in --loop
        // mode)
        if (g_chuglVM && g_chuglAPI) g_chuglAPI->vm->remove_all_shreds(g_chuglVM);

        App::end(&chugl_app);
    }

    return true;
}

t_CKBOOL chugl_main_loop_quit(void* bindle)
{
    UNUSED_VAR(bindle);

    // TODO: release per-chugl instances
    // * ck-strings created inside g_builtin_ckobjs
    // * main camera

    // NOT freeing SG components here because causes race with audio/graphics thread
    // we let the OS free for us
    // SG_Free();

    return true;
}

// ChuGL chugin info func
CK_DLL_INFO(ChuGL)
{
    // set module name
    QUERY->setname(QUERY, "ChuGL");

    // set info
    QUERY->setinfo(QUERY, CHUGIN_INFO_AUTHORS, "Andrew Zhu Aday & Ge Wang");
    QUERY->setinfo(QUERY, CHUGIN_INFO_CHUGIN_VERSION, CHUGL_VERSION_STRING);
    QUERY->setinfo(
      QUERY, CHUGIN_INFO_DESCRIPTION,
      "ChuGL (ChucK Graphics Library) is a unified audiovisual programming "
      "framework built into the ChucK programming language.");
    QUERY->setinfo(QUERY, CHUGIN_INFO_URL, "https://chuck.stanford.edu/chugl/");
    QUERY->setinfo(QUERY, CHUGIN_INFO_EMAIL, "azaday@ccrma.stanford.edu");

    { // setup
        // initialize performance counters
        stm_setup();
    }
}

// ============================================================================
// Query
// ============================================================================

static t_CKUINT chugl_next_frame_event_data_offset = 0;

static void autoUpdateScenegraph(Arena* arena, SG_Scene* scene, Chuck_VM* VM,
                                 CK_DL_API API, t_CKINT _ggen_update_vt_offset)
{
    // only update once per frame
    if (scene->last_auto_update_frame == g_frame_count) return;

    // mark updated this frame
    scene->last_auto_update_frame = g_frame_count;

    Chuck_DL_Arg theArg;
    theArg.kind          = kindof_FLOAT;
    theArg.value.v_float = g_last_dt;

    u64 arena_orig_size              = arena->curr;
    *(ARENA_PUSH_TYPE(arena, SG_ID)) = scene->id;

    // BFS through graph
    // TODO: can just walk linearly through entity arenas instead?
    // but then how do we know if something is part of the active scene graph?
    while (arena->curr != arena_orig_size) {
        ARENA_POP_TYPE(arena, SG_ID);
        SG_ID sg_id = *(SG_ID*)Arena::top(arena);

        SG_Transform* xform = SG_GetTransform(sg_id);
        ASSERT(xform != NULL);

        Chuck_Object* ggen = xform->ckobj;
        ASSERT(ggen != NULL);

        Chuck_VM_Shred* origin_shred = chugin_getOriginShred(ggen);
        API->vm->invoke_mfun_immediate_mode(ggen, _ggen_update_vt_offset, VM,
                                            origin_shred, &theArg, 1);

        // add children to stack
        size_t numChildren  = SG_Transform::numChildren(xform);
        SG_ID* children_ptr = ARENA_PUSH_COUNT(arena, SG_ID, numChildren);
        memcpy(children_ptr, xform->childrenIDs.base, sizeof(SG_ID) * numChildren);
    }
}

static const char* BufferMapAsyncStatusToString(WGPUBufferMapAsyncStatus status)
{
    switch (status) {
        case WGPUBufferMapAsyncStatus_Success: {
            return "Success";
        } break;
        case WGPUBufferMapAsyncStatus_ValidationError: {
            return "Validation Error";
        } break;
        case WGPUBufferMapAsyncStatus_Unknown: {
            return "Unknown";
        } break;
        case WGPUBufferMapAsyncStatus_DeviceLost: {
            return "Device Lost";
        } break;
        case WGPUBufferMapAsyncStatus_DestroyedBeforeCallback: {
            return "Destroyed Before Callback";
        } break;
        case WGPUBufferMapAsyncStatus_UnmappedBeforeCallback: {
            return "Unmapped Before Callback";
        } break;
        case WGPUBufferMapAsyncStatus_MappingAlreadyPending: {
            return "Mapping Already Pending";
        } break;
        case WGPUBufferMapAsyncStatus_OffsetOutOfRange: {
            return "Offset Out Of Range";
        } break;
        case WGPUBufferMapAsyncStatus_SizeOutOfRange: {
            return "Size Out Of Range";
        } break;
        default: break;
    }
    ASSERT(false);
    return "Invalid Status";
}

static void FlushGraphicsToAudioCQ()
{
    // swap read/write queues
    CQ_SwapQueues(true);

    // flush command queue
    SG_Command* command = NULL;
    while (CQ_ReadCommandQueueIter(&command, true)) {
        switch (command->type) {
            case SG_COMMAND_G2A_TEXTURE_READ: {
                SG_Command_G2A_TextureRead* cmd = (SG_Command_G2A_TextureRead*)command;
                SG_Texture* texture             = SG_GetTexture(cmd->texture_id);
                defer(free(cmd->data_OWNED));

                // if texture was already GC'd, skip
                if (!texture) break;

                // check status
                if (cmd->status != WGPUBufferMapAsyncStatus_Success) {
                    log_warn("Texture[id=%d, name=%s] read failed with status: %s",
                             texture->id, texture->name,
                             BufferMapAsyncStatusToString(cmd->status));
                    break;
                }

                // else copy data to texture and broadcast event
                SG_Texture::updateTextureData(texture, cmd->data_OWNED,
                                              cmd->size_bytes);
            } break;
            default: ASSERT(false)
        }
    }

    CQ_ReadCommandQueueClear(true);
}

// MUST BE CALLED WITH waitingShredsLock LOCKED
static void chugl_GraphicsShredPerformNextFrameUpdate(Chuck_VM_Shred* SHRED)
{
    CK_DL_API API = g_chuglAPI;
    Chuck_VM* VM  = g_chuglVM;

    // see comment in chugl_next_frame
    bool allShredsWaiting = waitingShreds == registeredShreds.size();

    // when new shreds are sporked, they will have `allShredsWaiting = true`
    // so to prevent multiple updates, we also check against the last update frame
    // and only update if this is the first time this frame
    static i64 last_update_frame_count{ -1 };
    bool first_of_last_shreds_waited
      = last_update_frame_count != waiting_shreds_frame_count;
    if (allShredsWaiting) {
        last_update_frame_count = waiting_shreds_frame_count;
    }

    spinlock::unlock(&waitingShredsLock);

    // THIS IS A VERY IMPORTANT FUNCTION. See
    // https://trello.com/c/Gddnu21j/6-chuglrender-refactor-possible-deadlock-between-cglupdate-and-render
    // and
    // https://github.com/ccrma/chugl/blob/2023-chugl-int/design/multishred-render-1.ck
    // for further context

    // activate hook only on GG.nextFrame();
    if (!hookActivated) {
        hookActivated = true;
        hook->activate(hook);
    }

    if (allShredsWaiting && first_of_last_shreds_waited) {
        // if #waiting == #registered, all chugl shreds have finished work, and
        // we are safe to wakeup the renderer
        // TODO: bug. If a shred does NOT call GG.nextFrame in an infinite loop,
        // i.e. does nextFrame() once and then goes on to say process audio,
        // this code will stay be expecting that shred to call nextFrame() again
        // and waitingShreds will never == registeredShreds.size() thus hanging
        // the renderer

        { // process dt
            // process dt in audio samples
            static t_CKTIME chuglLastUpdateTime{ API->vm->now(VM) };

            t_CKTIME chuckTimeNow  = API->vm->now(VM);
            t_CKTIME chuckTimeDiff = chuckTimeNow - chuglLastUpdateTime;
            ckdt_sec               = chuckTimeDiff / API->vm->srate(VM);
            chuglLastUpdateTime    = chuckTimeNow;

            // process dt with OS-provided timer
            static u64 system_last_time{ stm_now() };
            u64 system_dt_ticks = stm_laptime(&system_last_time);
            system_dt_sec       = stm_sec(system_dt_ticks);

            // update render thread dt
            g_last_dt = CHUGL_Window_dt();
            g_frame_count++;

#ifdef CHUGL_DEBUG
            spinlock::lock(&waitingShredsLock);
            ASSERT(g_frame_count - 1 == waiting_shreds_frame_count);
            spinlock::unlock(&waitingShredsLock);
#endif
        }

        // traverse rendegraph chuck-defined update() on all render passes
        if (gg_config.auto_update_scenegraph) {
            SG_Pass* pass = SG_GetPass(gg_config.root_pass_id);
            while (pass) {
                if (pass->pass_type == SG_PassType_Scene) {
                    SG_Scene* scene = SG_GetScene(pass->scene_id);
                    if (scene) {
                        autoUpdateScenegraph(&audio_frame_arena, scene, g_chuglVM,
                                             g_chuglAPI, ggen_update_vt_offset);
                    }
                }
                pass = SG_GetPass(pass->next_pass_id);
            }
        }

        // signal the graphics-side that audio-side is done processing for
        // this frame
        Sync_SignalUpdateDone();

        // Garbage collect (TODO add API function to control this via GG
        // config)
        SG_GC();

        // Handle commands from graphics thread
        FlushGraphicsToAudioCQ();

        // clear audio frame arena
        Arena::clear(&audio_frame_arena);
    }
}

static void chugl_GraphicsShredUnregister(Chuck_VM_Shred* SHRED)
{
    // do nothing if this is not a graphics shred
    if (!Sync_IsShredRegistered(SHRED)) return;

    // remove from registered list
    Sync_UnregisterShred(SHRED);

    // if this is the last graphics shred, close the window
    if (Sync_NumShredsRegistered() == 0) {
        CQ_PushCommand_WindowClose();
    }

    // if this would have been the one to trigger graphics thread wakeup, do it
    // now
    {
        spinlock::lock(&waitingShredsLock);
        chugl_GraphicsShredPerformNextFrameUpdate(SHRED);
    }
}

// called when shred is taken out of circulation for any reason
// e.g. reached end, removed, VM cleared, etc.
CK_DLL_SHREDS_WATCHER(chugl_shred_on_destroy_listener)
{
    // double check
    ASSERT(CODE == ckvm_shreds_watch_REMOVE);
    chugl_GraphicsShredUnregister(SHRED);
}

CK_DLL_SFUN(chugl_next_frame)
{
    // extract CglEvent from obj
    // TODO: workaround bug where create() object API is not calling
    // preconstructors
    // https://trello.com/c/JwhVQEpv/48-cglnextframe-now-not-calling-preconstructor-of-cglupdate

    g_chugl_window_initialized = true;

    if (!Sync_HasShredWaited(SHRED))
        API->vm->throw_exception(
          "NextFrameNotWaitedOnViolation",
          "You are calling .nextFrame() without chucking to now!\n"
          "Please replace this line with .nextFrame() => now;",
          SHRED);

    RETURN->v_object = (Chuck_Object*)Event_Get(CHUGL_EventType::NEXT_FRAME, API, VM);

    // register shred and set has-waited flag to false
    Sync_RegisterShred(SHRED);

    // bugfix: grabbing this lock prevents race with render thread
    // broadcasting nextFrameEvent and setting waitingShreds to 0.
    // Unlocked in event_next_frame_waiting_on after shred has
    // been added to the nextFrameEvent waiting queue.
    // Render thread holds this lock when broadcasting + setting waitingShreds
    // to 0
    spinlock::lock(&waitingShredsLock);
}

//-----------------------------------------------------------------------------
// this is called by chuck VM at the earliest point when a shred begins to wait
// on an Event used to catch GG.nextFrame() => now; on one or more shreds once
// all expected shreds are waiting on GG.nextFrame(), this function signals the
// graphics-side
//-----------------------------------------------------------------------------
CK_DLL_MFUN(event_next_frame_waiting_on)
{
    ++waitingShreds;
    chugl_GraphicsShredPerformNextFrameUpdate(SHRED);
    Sync_MarkShredWaited(SHRED);
}

CK_DLL_SFUN(chugl_gc)
{
    SG_GC();
}

CK_DLL_SFUN(chugl_get_scene)
{
    RETURN->v_object = SG_GetScene(gg_config.mainScene)->ckobj;
}

// TODO: should this update the main renderpass too?
CK_DLL_SFUN(chugl_set_scene)
{
    SG_ID prev_scene_id = gg_config.mainScene;

    // get new scene
    Chuck_Object* newScene = GET_NEXT_OBJECT(ARGS);
    SG_Scene* sg_scene = SG_GetScene(OBJ_MEMBER_UINT(newScene, component_offset_id));

    // bump refcount on new scene
    SG_AddRef(sg_scene);

    // assign new scene
    gg_config.mainScene = sg_scene ? sg_scene->id : 0;

    // decrement refcount on old scene
    SG_DecrementRef(prev_scene_id);
}

CK_DLL_SFUN(chugl_set_fps)
{
    gg_config.fixed_timestep_fps = GET_NEXT_INT(ARGS);
    CQ_PushCommand_SetFixedTimestep(gg_config.fixed_timestep_fps);
}

CK_DLL_SFUN(chugl_get_fps)
{
    RETURN->v_float = CHUGL_Window_fps();
    return;
}

CK_DLL_SFUN(chugl_get_dt)
{
    RETURN->v_float = g_last_dt;
}

CK_DLL_SFUN(chugl_get_frame_count)
{
    RETURN->v_uint = g_frame_count;
}

CK_DLL_SFUN(chugl_get_root_pass)
{
    RETURN->v_object = SG_GetPass(gg_config.root_pass_id)->ckobj;
}

CK_DLL_SFUN(chugl_get_default_scene_pass)
{
    RETURN->v_object = SG_GetPass(gg_config.default_scene_pass_id)->ckobj;
}

CK_DLL_SFUN(chugl_get_default_output_pass)
{
    RETURN->v_object = SG_GetPass(gg_config.default_output_pass_id)->ckobj;
}

CK_DLL_SFUN(chugl_get_auto_update_scenegraph)
{
    RETURN->v_int = gg_config.auto_update_scenegraph ? 1 : 0;
}

CK_DLL_SFUN(chugl_set_auto_update_scenegraph)
{
    gg_config.auto_update_scenegraph = (GET_NEXT_INT(ARGS) != 0);
}

CK_DLL_SFUN(chugl_get_default_camera)
{
    RETURN->v_object = SG_GetCamera(gg_config.mainCamera)->ckobj;
}

CK_DLL_SFUN(chugl_set_log_level)
{
    log_set_level(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(chugl_get_window_width)
{
    RETURN->v_float = CHUGL_Window_WindowSize().x;
}

CK_DLL_SFUN(chugl_get_window_height)
{
    RETURN->v_float = CHUGL_Window_WindowSize().y;
}

CK_DLL_SFUN(chugl_get_mouse_x)
{
    RETURN->v_float = CHUGL_Mouse_Position().x;
}

CK_DLL_SFUN(chugl_get_mouse_y)
{
    RETURN->v_float = CHUGL_Mouse_Position().y;
}

CK_DLL_SFUN(chugl_get_frame_width)
{
    RETURN->v_float = CHUGL_Window_FramebufferSize().x;
}

CK_DLL_SFUN(chugl_get_frame_height)
{
    RETURN->v_float = CHUGL_Window_FramebufferSize().y;
}

CK_DLL_SFUN(chugl_unregister_shred)
{
    chugl_GraphicsShredUnregister(SHRED);
}

// ============================================================================
// Chugin entry point
// ============================================================================
CK_DLL_QUERY(ChuGL)
{
    // set log level
#ifdef CHUGL_RELEASE
    log_set_level(LOG_WARN); // only log errors and fatal in release mode
#endif

    // remember
    g_chuglVM  = QUERY->ck_vm(QUERY);
    g_chuglAPI = QUERY->ck_api(QUERY);

    { // cache common chuck types
        g_chuck_types.int_array   = g_chuglAPI->type->lookup(g_chuglVM, "int[]");
        g_chuck_types.float_array = g_chuglAPI->type->lookup(g_chuglVM, "float[]");
        g_chuck_types.vec2_array  = g_chuglAPI->type->lookup(g_chuglVM, "vec2[]");
        g_chuck_types.vec3_array  = g_chuglAPI->type->lookup(g_chuglVM, "vec3[]");
        g_chuck_types.vec4_array  = g_chuglAPI->type->lookup(g_chuglVM, "vec4[]");
    }

    // audio frame arena
    Arena::init(&audio_frame_arena, 64 * KILOBYTE);

    // initialize component pool
    // TODO: have a single ChuGL_Context that manages this all
    SG_Init(g_chuglAPI);
    CQ_Init();

    // set up for main thread hook, for running ChuGL on the main thread
    hook = QUERY->create_main_thread_hook(QUERY, chugl_main_loop_hook,
                                          chugl_main_loop_quit, NULL);

    // initialize ChuGL API ========================================
    { // chugl events
        // triggered by main render thread after deepcopy is complete, and safe
        // for chuck to begin updating the scene graph (intentionally left out
        // of CKDocs)
        QUERY->begin_class(QUERY, "NextFrameEvent", "Event");
        DOC_CLASS(
          "Don't instantiate this class directly. Use GG.nextFrame() => now; "
          "instead.");
        ADD_EX("basic/gameloop.ck");

        // no destructor for singleton
        chugl_next_frame_event_data_offset
          = QUERY->add_mvar(QUERY, "int", "@next_frame_event_data", false);

        QUERY->add_mfun(QUERY, event_next_frame_waiting_on, "void", "waiting_on");
        QUERY->end_class(QUERY);
    }

    { // create default ckobjs
        g_builtin_ckobjs.empty_float_array = chugin_createCkFloatArray(NULL, 0, true);

        float init_2d_pos[2] = { 0.0f, 0.0f };
        g_builtin_ckobjs.init_2d_pos
          = chugin_createCkFloatArray(init_2d_pos, ARRAY_LENGTH(init_2d_pos), true);

        glm::vec3 white_color = glm::vec3(1.0f);
        g_builtin_ckobjs.init_white_color
          = chugin_createCkFloat3Array(&white_color, 1, true);

        float white_pixel_data[4]         = { 1.0f, 1.0f, 1.0f, 1.0f };
        g_builtin_ckobjs.white_pixel_data = chugin_createCkFloatArray(
          white_pixel_data, ARRAY_LENGTH(white_pixel_data), true);

        float black_pixel_data[4]         = { 0.0f, 0.0f, 0.0f, 0.0f };
        g_builtin_ckobjs.black_pixel_data = chugin_createCkFloatArray(
          black_pixel_data, ARRAY_LENGTH(black_pixel_data), true);

        float normal_pixel_data[4]         = { 0.5f, 0.5f, 1.0f, 1.0f };
        g_builtin_ckobjs.normal_pixel_data = chugin_createCkFloatArray(
          normal_pixel_data, ARRAY_LENGTH(normal_pixel_data), true);

        float magenta_pixel_data[4]         = { 1.0f, 0.0f, 1.0f, 1.0f };
        g_builtin_ckobjs.magenta_pixel_data = chugin_createCkFloatArray(
          magenta_pixel_data, ARRAY_LENGTH(magenta_pixel_data), true);

        float default_cubemap_data[4 * 6] = {};
        for (int i = 0; i < 6; i++) {
            default_cubemap_data[i * 4 + 0] = 1.0f;
            default_cubemap_data[i * 4 + 1] = 1.0f;
            default_cubemap_data[i * 4 + 2] = 1.0f;
            default_cubemap_data[i * 4 + 3] = 1.0f;
        }
        g_builtin_ckobjs.default_cube_map_data = chugin_createCkFloatArray(
          default_cubemap_data, ARRAY_LENGTH(default_cubemap_data), true);

        // builtin shader includes
        g_builtin_ckobjs.FRAME_UNIFORMS
          = chugin_createCkString(shader_table["FRAME_UNIFORMS"].c_str(), true);
        g_builtin_ckobjs.LIGHTING_UNIFORMS
          = chugin_createCkString(shader_table["LIGHTING_UNIFORMS"].c_str(), true);
        g_builtin_ckobjs.DRAW_UNIFORMS
          = chugin_createCkString(shader_table["DRAW_UNIFORMS"].c_str(), true);
        g_builtin_ckobjs.STANDARD_VERTEX_INPUT
          = chugin_createCkString(shader_table["STANDARD_VERTEX_INPUT"].c_str(), true);
        g_builtin_ckobjs.STANDARD_VERTEX_OUTPUT
          = chugin_createCkString(shader_table["STANDARD_VERTEX_OUTPUT"].c_str(), true);
        g_builtin_ckobjs.STANDARD_VERTEX_SHADER
          = chugin_createCkString(shader_table["STANDARD_VERTEX_SHADER"].c_str(), true);
    }

    ulib_color_query(QUERY);
    ulib_box2d_query(QUERY);

    ulib_window_query(QUERY);
    ulib_component_query(QUERY);
    ulib_texture_query(QUERY);
    ulib_ggen_query(QUERY);
    ulib_light_query(QUERY);
    ulib_imgui_query(QUERY);
    ulib_camera_query(QUERY);
    ulib_buffer_query(QUERY);
    ulib_geometry_query(QUERY);
    ulib_material_query(QUERY);
    ulib_gscene_query(QUERY);
    ulib_mesh_query(QUERY);
    ulib_pass_query(QUERY);
    ulib_text_query(QUERY);

    ulib_video_query(QUERY);

#ifndef CHUGL_FAST_COMPILE
    ulib_assloader_query(QUERY);
#endif

    { // GG static functions
        QUERY->begin_class(QUERY, "GG", "Object");
        ADD_EX("basic/gameloop.ck");

        // svars
        static t_CKUINT gg_log_level_trace = LOG_TRACE;
        static t_CKUINT gg_log_level_debug = LOG_DEBUG;
        static t_CKUINT gg_log_level_info  = LOG_INFO;
        static t_CKUINT gg_log_level_warn  = LOG_WARN;
        static t_CKUINT gg_log_level_error = LOG_ERROR;
        static t_CKUINT gg_log_level_fatal = LOG_FATAL;
        SVAR("int", "LogLevel_Trace", &gg_log_level_trace);
        SVAR("int", "LogLevel_Debug", &gg_log_level_debug);
        SVAR("int", "LogLevel_Info", &gg_log_level_info);
        SVAR("int", "LogLevel_Warn", &gg_log_level_warn);
        SVAR("int", "LogLevel_Error", &gg_log_level_error);
        SVAR("int", "LogLevel_Fatal", &gg_log_level_fatal);

        SFUN(chugl_set_log_level, "void", "logLevel");
        ARG("int", "level");
        DOC_FUNC(
          "Set the log level for ChuGL's renderer and graphics thread. "
          "Levels are: GG.LogLevel_Trace, GG.LogLevel_Debug, GG.LogLevel_Info, "
          "GG.LogLevel_Warn, GG.LogLevel_Error, GG.LogLevel_Fatal. "
          "Setting a log level will allow all messages of that level and higher "
          "to be printed to the console. Default is GG.LogLevel_Error.");

        QUERY->add_sfun(QUERY, chugl_next_frame, "NextFrameEvent", "nextFrame");
        QUERY->doc_func(
          QUERY,
          "Registers the calling shred to be notified when the next frame is "
          "finished rendering. When all graphics shreds are finished calling."
          "Note: this function returns an event that MUST be waited on for "
          "correct behavior, i.e. GG.nextFrame() => now;"
          "See the ChuGL tutorial and examples for more information.");

        QUERY->add_sfun(QUERY, chugl_get_scene, SG_CKNames[SG_COMPONENT_SCENE],
                        "scene");
        QUERY->add_sfun(QUERY, chugl_set_scene, SG_CKNames[SG_COMPONENT_SCENE],
                        "scene");
        QUERY->add_arg(QUERY, SG_CKNames[SG_COMPONENT_SCENE], "scene");

        // QUERY->add_sfun(QUERY, chugl_gc, "void", "gc");
        // QUERY->doc_func(QUERY, "Trigger garbage collection");

        SFUN(chugl_get_fps, "float", "fps");
        DOC_FUNC("FPS of current window, updated every second");

        SFUN(chugl_set_fps, "void", "fps");
        ARG("int", "fps");
        DOC_FUNC(
          "Hard-limit the frame rate of the graphics thread. By default this is 60. "
          "Lowering this value may improve audio performance in chuck, as graphics "
          "shreds will run less frequently, giving the chuck VM more time to compute "
          "audio samples. Set to a negative value or 0 to disable frame rate limiting "
          "and let graphics run as fast as possible (up to the refresh rate of the "
          "monitor)");

        // note: we use window time here instead of ckdt_sec or system_dt_sec
        // (which are laptimes calculated on the audio thread every cycle of
        // nextFrameWaitingOn ) because the latter values are highly unstable, and do
        // not correspond to the actual average FPS of the graphics window. dt is most
        // likely used for graphical animations, and so therefore should be set by the
        // actual render thread, not the audio thread.
        SFUN(chugl_get_dt, "float", "dt");
        DOC_FUNC("return the laptime of the graphics thread's last frame in seconds");

        SFUN(chugl_get_frame_count, "int", "fc");
        DOC_FUNC("return the number of frames rendered since the start of the program");

        SFUN(chugl_get_root_pass, SG_CKNames[SG_COMPONENT_PASS], "rootPass");
        DOC_FUNC("Get the root pass of the current scene");

        SFUN(chugl_get_default_scene_pass, "ScenePass", "scenePass");
        DOC_FUNC("Get the default render pass (renders the main scene)");

        SFUN(chugl_get_default_scene_pass, "ScenePass", "renderPass");
        DOC_FUNC("Identical to GG.scenePass(). Kept for backwards compat");

        SFUN(chugl_get_default_output_pass, "OutputPass", "outputPass");
        DOC_FUNC(
          "Get the default output pass (renders the main scene to the screen, "
          "with default tonemapping and exposure settings");

        SFUN(chugl_get_auto_update_scenegraph, "int", "autoUpdate");
        DOC_FUNC(
          "Returns true if GGen update() functions are automatically called "
          "on all GGens in each active scene graph every frame. Default is true.");

        SFUN(chugl_set_auto_update_scenegraph, "void", "autoUpdate");
        ARG("int", "autoUpdate");
        DOC_FUNC(
          "Set whether GGen update() functions are automatically called "
          "on all GGens in active scene graphs every frame. Default is true.");

        SFUN(gwindow_fullscreen, "void", "fullscreen");
        DOC_FUNC(
          "Shorthand for GWindow.fullscreen(). Added for backwards compatibility");

        SFUN(gwindow_windowed, "void", "windowed");
        ARG("int", "width");
        ARG("int", "height");
        DOC_FUNC("Shorthand for GWindow.windowed(). Added for backwards compatibility");

        SFUN(gwindow_windowed_prev_size, "void", "windowed");
        DOC_FUNC("Shorthand for GWindow.windowed(). Added for backwards compatibility");

        SFUN(chugl_get_default_camera, SG_CKNames[SG_COMPONENT_CAMERA], "camera");
        DOC_FUNC(
          "Shorthand for getting the default Camera that is created upon "
          "startup");

        SFUN(gwindow_set_title, "void", "windowTitle");
        ARG("string", "title");
        DOC_FUNC("Shorthand for GWindow.title(). Added for backwards compatibility");

        SFUN(chugl_get_window_width, "float", "windowWidth");
        DOC_FUNC(
          "Get the width of the current window. Shorthand for GWindow.size().x, added "
          "for backwards compatibility");

        SFUN(chugl_get_window_height, "float", "windowHeight");
        DOC_FUNC(
          "Get the height of the current window. Shorthand for GWindow.size().y, added "
          "for backwards compatibility");

        SFUN(chugl_get_mouse_x, "float", "mouseX");
        DOC_FUNC("Get the x position of the mouse cursor in screen coordinates");

        SFUN(chugl_get_mouse_y, "float", "mouseY");
        DOC_FUNC("Get the y position of the mouse cursor in screen coordinates");

        SFUN(chugl_get_frame_width, "float", "frameWidth");
        DOC_FUNC(
          "Get the width of the current window's framebuffer in pixels. Shorthand for "
          "GWindow.framebufferSize().x, added for backwards compatibility");

        SFUN(chugl_get_frame_height, "float", "frameHeight");
        DOC_FUNC(
          "Get the height of the current window's framebuffer in pixels. Shorthand for "
          "GWindow.framebufferSize().y, added for backwards compatibility");

        SFUN(chugl_unregister_shred, "void", "unregisterShred");
        DOC_FUNC(
          "Unregisters the calling shred from ChuGL so that it is no longer marked as "
          "a graphics shred. Do this if you want a shred to exit a GG.nextFrame() => "
          "now gameloop and move on to other tasks. Otherwise, the window will hang "
          "as it waits for this shred to call GG.nextFrame() again.");

        END_CLASS();
    } // GG

    { // chugin listeners

        // shred destroy listener
        QUERY->register_shreds_watcher(QUERY, chugl_shred_on_destroy_listener,
                                       ckvm_shreds_watch_REMOVE, NULL);
    }

    { // Default components
        // scene (TODO should all GScenes come with these default components?)
        // currently if the user instantiates a GScene it will have nothing
        Chuck_Object* scene_ckobj
          = chugin_createCkObj(SG_CKNames[SG_COMPONENT_SCENE], true);
        SG_Scene* scene = ulib_scene_create(scene_ckobj, true, false);

        // update gg_config
        gg_config.mainScene  = scene->id;
        gg_config.mainCamera = scene->desc.main_camera_id;

        // passRoot()
        SG_Pass* root_pass     = ulib_pass_create(SG_PassType_Root, NULL, true, NULL);
        gg_config.root_pass_id = root_pass->id;
        ulib_component_set_name(root_pass, "ChuGL RootPass");

        // renderPass for main scene
        SG_Pass* render_pass = ulib_pass_create(SG_PassType_Scene, NULL, false, NULL);
        gg_config.default_scene_pass_id = render_pass->id;
        SG_Pass::scene(render_pass, scene);
        ulib_component_set_name(render_pass, "ChuGL Default ScenePass");

        // enable MSAA by default
        render_pass->scene_pass_msaa = 1;

        // connect root to renderPass
        SG_Pass::connect(root_pass, render_pass);

        // set default render texture as output of render pass
        SG_Texture* render_texture
          = SG_GetTexture(g_builtin_textures.default_render_texture_id);
        SG_Pass::colorTarget(render_pass, render_texture);

        // output pass
        Chuck_Object* output_pass_ckobj = chugin_createCkObj("OutputPass", false);
        SG_Pass* output_pass
          = ulib_pass_create_output_pass(NULL, output_pass_ckobj, false, NULL);
        gg_config.default_output_pass_id = output_pass->id;
        ulib_component_set_name(output_pass, "ChuGL Default OutputPass");

        // connect renderPass to outputPass
        SG_Pass::connect(render_pass, output_pass);

        // set render texture as input to output pass
        SG_Material* material = SG_GetMaterial(output_pass->screen_material_id);
        SG_Material::setTexture(material, 0, render_texture);
        CQ_PushCommand_MaterialSetUniform(material, 0);

        // update all passes over cq
        CQ_PushCommand_PassUpdate(root_pass);
        CQ_PushCommand_PassUpdate(render_pass);
        CQ_PushCommand_PassUpdate(output_pass);
    }

    { // default config
        // default to 60fps
        CQ_PushCommand_SetFixedTimestep(gg_config.fixed_timestep_fps);
        CQ_PushCommand_SetChuckVMInfo(g_chuglAPI->vm->srate(g_chuglVM));
    }

    // wasn't that a breeze?
    return true;
}
