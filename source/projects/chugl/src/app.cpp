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

#include <stdlib.h>
#include <time.h>

#include <box2d/box2d.h>
// necessary for copying from command
static_assert(sizeof(u32) == sizeof(b2WorldId), "b2WorldId != u32");

#include <GLFW/glfw3.h>
#include <chuck/chugin.h>
#include <glfw3webgpu/glfw3webgpu.h>
#include <glm/gtx/string_cast.hpp>

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_wgpu.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h> // ImPool<>, ImHashData

#include <nanotime/nanotime.h>
#include <sokol/sokol_time.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#endif

// #include "camera.cpp"
#include "graphics.h"
#include "r_component.h"
#include "sg_command.h"
#include "sg_component.h"

#include "core/hashmap.h"
#include "core/log.h"

#include "compressed_fonts.h"

// Usage:
//  static ImDrawDataSnapshot snapshot; // Important: make persistent accross
//  frames to reuse buffers. snapshot.SnapUsingSwap(ImGui::GetDrawData(),
//  ImGui::GetTime());
//  [...]
//  ImGui_ImplDX11_RenderDrawData(&snapshot.DrawData);
// Source: https://github.com/ocornut/imgui/issues/1860

struct ImDrawDataSnapshotEntry {
    ImDrawList* SrcCopy = NULL; // Drawlist owned by main context
    ImDrawList* OurCopy = NULL; // Our copy
    double LastUsedTime = 0.0;
};

struct ImDrawDataSnapshot {
    // Members
    ImDrawData DrawData;
    ImPool<ImDrawDataSnapshotEntry> Cache;
    float MemoryCompactTimer = 20.0f; // Discard unused data after 20 seconds

    // Functions
    ~ImDrawDataSnapshot()
    {
        Clear();
    }
    void Clear();
    void SnapUsingSwap(ImDrawData* src,
                       double current_time); // Efficient snapshot by swapping data,
                                             // meaning "src_list" is unusable.
    // void                          SnapUsingCopy(ImDrawData* src, double
    // current_time); // Deep-copy snapshop

    // Internals
    ImGuiID GetDrawListID(ImDrawList* src_list)
    {
        return ImHashData(&src_list, sizeof(src_list));
    } // Hash pointer
    ImDrawDataSnapshotEntry* GetOrAddEntry(ImDrawList* src_list)
    {
        return Cache.GetOrAddByKey(GetDrawListID(src_list));
    }
};

void ImDrawDataSnapshot::Clear()
{
    for (int n = 0; n < Cache.GetMapSize(); n++)
        if (ImDrawDataSnapshotEntry* entry = Cache.TryGetMapData(n))
            IM_DELETE(entry->OurCopy);
    Cache.Clear();
    DrawData.Clear();
}

void ImDrawDataSnapshot::SnapUsingSwap(ImDrawData* src, double current_time)
{
    ImDrawData* dst = &DrawData;
    IM_ASSERT(src != dst && src->Valid);

    // Copy all fields except CmdLists[]
    ImVector<ImDrawList*> backup_draw_list;
    backup_draw_list.swap(src->CmdLists);
    IM_ASSERT(src->CmdLists.Data == NULL);
    *dst = *src;
    backup_draw_list.swap(src->CmdLists);

    // Swap and mark as used
    for (ImDrawList* src_list : src->CmdLists) {
        ImDrawDataSnapshotEntry* entry = GetOrAddEntry(src_list);
        if (entry->OurCopy == NULL) {
            entry->SrcCopy = src_list;
            entry->OurCopy = IM_NEW(ImDrawList)(src_list->_Data);
        }
        IM_ASSERT(entry->SrcCopy == src_list);
        entry->SrcCopy->CmdBuffer.swap(entry->OurCopy->CmdBuffer); // Cheap swap
        entry->SrcCopy->IdxBuffer.swap(entry->OurCopy->IdxBuffer);
        entry->SrcCopy->VtxBuffer.swap(entry->OurCopy->VtxBuffer);
        entry->SrcCopy->CmdBuffer.reserve(
          entry->OurCopy->CmdBuffer.Capacity); // Preserve bigger size to avoid reallocs
                                               // for two consecutive frames
        entry->SrcCopy->IdxBuffer.reserve(entry->OurCopy->IdxBuffer.Capacity);
        entry->SrcCopy->VtxBuffer.reserve(entry->OurCopy->VtxBuffer.Capacity);
        entry->LastUsedTime = current_time;
        dst->CmdLists.push_back(entry->OurCopy);
    }

    // Cleanup unused data
    const double gc_threshold = current_time - MemoryCompactTimer;
    for (int n = 0; n < Cache.GetMapSize(); n++)
        if (ImDrawDataSnapshotEntry* entry = Cache.TryGetMapData(n)) {
            if (entry->LastUsedTime > gc_threshold) continue;
            IM_DELETE(entry->OurCopy);
            Cache.Remove(GetDrawListID(entry->SrcCopy), entry);
        }
};

static ImDrawDataSnapshot snapshot;

static WGPUTextureView ImGui_ImplWGPU_GetTextureId(ImTextureID id, void* user)
{
    G_Cache* cache = (G_Cache*)user;
    ASSERT(cache->initialized == 0XDEADBEEF);
    R_Texture* tex = Component_GetTexture(*(SG_ID*)&id);
    ASSERT(tex && tex->gpu_texture);

    G_CacheTextureViewDesc desc = { 0 };
    desc = { tex->gpu_texture, WGPUTextureViewDimension_2D, 0, 1, 0, 1 };
    return cache->textureView(desc);
}

struct TickStats {
    u64 fc    = 0;
    u64 min   = UINT64_MAX;
    u64 max   = 0;
    u64 total = 0;

    void update(u64 ticks)
    {
        min = ticks < min ? ticks : min;
        max = ticks > max ? ticks : max;
        total += ticks;
        if (++fc % 60 == 0) {
            print("");
            // fc    = 0;
            // total = 0;
        }
    }

    void print(const char* name)
    {
        printf("%s: min: %f, max: %f, avg: %f\n", name, stm_ms(min), stm_ms(max),
               stm_ms(total / fc));
    }
};

TickStats critical_section_stats = {};

static int mini(int x, int y)
{
    return x < y ? x : y;
}

static int maxi(int x, int y)
{
    return x > y ? x : y;
}

GLFWmonitor* getCurrentMonitor(GLFWwindow* window)
{
    int nmonitors, i;
    int wx, wy, ww, wh;
    int mx, my, mw, mh;
    int overlap, bestoverlap;
    GLFWmonitor* bestmonitor;
    GLFWmonitor** monitors;
    const GLFWvidmode* mode;

    bestoverlap = 0;
    bestmonitor = NULL;

    glfwGetWindowPos(window, &wx, &wy);
    glfwGetWindowSize(window, &ww, &wh);
    monitors = glfwGetMonitors(&nmonitors);

    for (i = 0; i < nmonitors; i++) {
        mode = glfwGetVideoMode(monitors[i]);
        glfwGetMonitorPos(monitors[i], &mx, &my);
        mw = mode->width;
        mh = mode->height;

        overlap = maxi(0, mini(wx + ww, mx + mw) - maxi(wx, mx))
                  * maxi(0, mini(wy + wh, my + mh) - maxi(wy, my));

        if (bestoverlap < overlap) {
            bestoverlap = overlap;
            bestmonitor = monitors[i];
        }
    }

    return bestmonitor;
}

struct App;

static void _R_HandleCommand(App* app, SG_Command* command);

static void _R_RenderScene(App* app, R_Scene* scene, R_Pass* pass, R_Camera* camera,
                           G_DrawCallListID dc_list);

static void _R_glfwErrorCallback(int error, const char* description)
{
    log_warn("GLFW Error[%i]: %s\n", error, description);
}

static int frame_buffer_width  = 0;
static int frame_buffer_height = 0;
static bool resized_this_frame = false;
struct App {
    GLFWwindow* window;
    GraphicsContext gctx; // pass as pointer?
    int window_fb_width;
    int window_fb_height;
    int window_width;
    int window_height;

    // Chuck Context
    Chuck_VM* ckvm;
    CK_DL_API ckapi;
    int ck_srate;

    // frame metrics
    u64 fc;
    f64 lastTime;
    f64 dt;
    bool show_fps_title = true;

    // timer for fixed timestep
    nanotime_step_data stepper;
    int stepper_fps = 60; // default to 60fps

    // mouse state
    double mouse_x = 0, mouse_y = 0;
    bool mouse_left = 0, mouse_right = 0;

    // imgui
    bool imgui_disabled = false;

    // box2D physics
    b2_SimulateDesc b2_sim_desc;

    // FreeType
    FT_Library FTLibrary;
    R_Font* default_font;

    // memory
    Arena frameArena;

    // render graph
    SG_ID root_pass_id;
    G_Graph rendergraph;

    // ============================================================================
    // App API
    // ============================================================================

    static void init(App* app, Chuck_VM* vm, CK_DL_API api)
    {
        ASSERT(app->ckvm == NULL && app->ckapi == NULL);
        ASSERT(app->window == NULL);

        app->ckvm  = vm;
        app->ckapi = api;

        Arena::init(&app->frameArena, MEGABYTE); // 1MB

        // init rendergraph
        app->rendergraph.init();

        // b2 sim defaults
        ASSERT(app->b2_sim_desc.substeps == 4);
    }

    // static void emscriptenMainLoop(void* arg)
    // {
    //     App* app = (App*)arg;
    //     gameloop(app);
    // }

    static void start(App* app)
    {
        ASSERT(app->window == NULL);

        // seed random number generator ===========================
        srand((unsigned int)time(0));

        { // Initialize window
            glfwSetErrorCallback(_R_glfwErrorCallback);
            if (!glfwInit()) {
                log_fatal("Failed to initialize GLFW\n");
                return;
            }

            // Create the window without an OpenGL context
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            t_CKVEC2 window_size = CHUGL_Window_WindowSize();
            glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER,
                           CHUGL_Window_Transparent() ? GLFW_TRUE : GLFW_FALSE);
            glfwWindowHint(GLFW_DECORATED,
                           CHUGL_Window_Decorated() ? GLFW_TRUE : GLFW_FALSE);
            glfwWindowHint(GLFW_RESIZABLE,
                           CHUGL_Window_Resizable() ? GLFW_TRUE : GLFW_FALSE);
            glfwWindowHint(GLFW_FLOATING,
                           CHUGL_Window_Floating() ? GLFW_TRUE : GLFW_FALSE);

            app->window = glfwCreateWindow((int)window_size.x, (int)window_size.y,
                                           "ChuGL " CHUGL_VERSION_STRING, NULL, NULL);

            // TODO: set window user pointer to CHUGL_App

            if (!app->window) {
                log_fatal("Failed to create GLFW window\n");
                glfwTerminate();
                return;
            }
        }

        // init graphics context
        if (!GraphicsContext::init(&app->gctx, app->window)) {
            log_fatal("Failed to initialize graphics context\n");
            return;
        }

        { // Initialize FT and builtin fonts
            FT_Error error = FT_Init_FreeType(&app->FTLibrary);
            if (error) {
                log_fatal("Failed to initialize FreeType\n");
                return;
            }

            R_Font* builtin_font
              = Component_GetFont(&app->gctx, app->FTLibrary, "chugl:cousine-regular");
            ASSERT(builtin_font);

            app->default_font = builtin_font; // safe to store ptr because all fonts are
                                              // kept in static array
        }

        // initialize R_Component manager
        Component_Init(&app->gctx);

        { // initialize imgui
            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            (void)io;
            io.ConfigFlags
              |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
            io.ConfigFlags
              |= ImGuiConfigFlags_NavEnableGamepad;           // Enable Gamepad Controls
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport

            // load builtin fonts
            io.Fonts->AddFontDefault();
            // io.Fonts->AddFontFromFileTTF(
            //   "/Users/Andrew/Google-Drive/Stanford/chugl/assets/fonts/DroidSans.ttf",
            //   16);
            ImFontConfig font_cfg = ImFontConfig();
            font_cfg.SizePixels   = 16.0f;
            ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name),
                           "CousineRegular.ttf, %dpx", (int)font_cfg.SizePixels);
            io.Fonts->AddFontFromMemoryCompressedBase85TTF( // works but no filename
              cousine_regular_compressed_data_base85, font_cfg.SizePixels, &font_cfg);

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();
            // ImGui::StyleColorsLight();
        }

        { // set window callbacks
            glfwSetWindowUserPointer(app->window, app);
            glfwSetMouseButtonCallback(app->window, _mouseButtonCallback);
            glfwSetScrollCallback(app->window, _scrollCallback);
            glfwSetCursorPosCallback(app->window, _cursorPositionCallback);
            glfwSetKeyCallback(app->window, _keyCallback);
            glfwSetWindowCloseCallback(app->window, _closeCallback);
            glfwSetWindowContentScaleCallback(app->window, _contentScaleCallback);
            // set initial content scale
            float content_scale_x, content_scale_y;
            glfwGetWindowContentScale(app->window, &content_scale_x, &content_scale_y);
            CHUGL_Window_ContentScale(content_scale_x, content_scale_y);

            glfwPollEvents(); // call poll events first to get correct
                              //   framebuffer size (glfw bug:
                              //   https://github.com/glfw/glfw/issues/1968)
        }

        // Setup ImGui Platform/Renderer backends
        {
            ImGui_ImplGlfw_InitForOther(app->window, true);
#ifdef __EMSCRIPTEN__
            ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
            ImGui_ImplWGPU_InitInfo init_info;
            init_info.Device             = app->gctx.device;
            init_info.NumFramesInFlight  = 3;
            init_info.RenderTargetFormat = app->gctx.surface_format;
            // init_info.DepthStencilFormat = app->gctx.depthTextureDesc.format;
            ImGui_ImplWGPU_Init(&init_info);
        }

        // trigger window resize callback to set up imgui
        int width, height;
        glfwGetFramebufferSize(app->window, &width, &height);
        _onFramebufferResize(app->window, width, height);

        // initialize imgui frame (should be threadsafe as long as graphics
        // shreds start with GG.nextFrame() => now)
        ImGui_ImplWGPU_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // main loop
        log_trace("entering  main loop");
#ifdef __EMSCRIPTEN__
        // https://emscripten.org/docs/api_reference/emscripten.h.html#c.emscripten_set_main_loop_arg
        // can't have an infinite loop in emscripten
        // instead pass a callback to emscripten_set_main_loop_arg
        emscripten_set_main_loop_arg(
          [](void* runner) {
              App::emscriptenMainLoop(runner);
              //   gameloop(app);
              // if (glfwWindowShouldClose(app->window)) {
              //     if (app->callbacks.onExit) app->callbacks.onExit();
              //     emscripten_cancel_main_loop(); // unregister the main loop
              // }
          },
          app, // user data (void *)
          -1,  // FPS (negative means use browser's requestAnimationFrame)
          true // simulate infinite loop (prevents code after this from exiting)
        );
#else

        nanotime_step_init(&app->stepper,
                           (u64)(NANOTIME_NSEC_PER_SEC / app->stepper_fps),
                           nanotime_now_max(), nanotime_now, nanotime_sleep);
        while (!glfwWindowShouldClose(app->window)) {
            // frame metrics ----------------------------
            {
                _calculateFPS(app->window, app->show_fps_title);

                ++app->fc;
                f64 currentTime = glfwGetTime();

                // first frame prevent huge dt
                if (app->lastTime == 0) app->lastTime = currentTime;

                app->dt       = currentTime - app->lastTime;
                app->lastTime = currentTime;
            }

            _mainLoop(app); // chuck loop

            Arena::clear(&app->frameArena);

            // fixed timestep (this might be helpful for finishing box2d
            // integration later)
            if (app->stepper_fps > 0) {
                nanotime_step(&app->stepper);
            }
        }
#endif

        log_trace("Exiting main loop");
    }

    static void end(App* app)
    {
        // free R_Components
        Component_Free();

        // release graphics context
        GraphicsContext::release(&app->gctx);

        // destroy imgui
        // actually don't do this lol to prevent data race with chuck UI shreds that are
        // still running
        // ImGui_ImplWGPU_Shutdown(); ImGui_ImplGlfw_Shutdown();
        // ImGui::DestroyContext();

        // destroy window
        glfwDestroyWindow(app->window);

        // terminate GLFW
        glfwTerminate();

        // free memory
        Arena::free(&app->frameArena);
    }

    // ============================================================================
    // App Internal Functions
    // ============================================================================

    static void _mainLoop(App* app)
    {
        // Render Loop ===========================================
        static u64 prev_lap_time{ stm_now() };

        // ======================
        // enter critical section
        // ======================
        // waiting for audio synchronization (see cgl_update_event_waiting_on)
        // (i.e., when all registered GG.nextFrame() are called on their
        // respective shreds)
        Sync_WaitOnUpdateDone();

        // question: why does putting this AFTER time calculation cause
        // everything to be so choppy at high FPS? hypothesis: puts time
        // calculation into the critical region time is updated only when all
        // chuck shreds are on wait queue guaranteeing that when they awake,
        // they'll be using fresh dt data

        // calculate dt
        u64 dt_ticks = stm_laptime(&prev_lap_time);
        f64 dt_sec   = stm_sec(dt_ticks);
        CHUGL_Window_dt(dt_sec);

        /* two locks here:
        1 for writing/swapping the command queues
            - this lock is grabbed by chuck every time we do a CGL call
            - supports writing CGL commands whenever, even outside game loop
        1 for the condition_var used to synchronize audio and graphics each
        frame
            - combined with the chuck-side update_event, allows for writing
        frame-accurate cgl commands
            - exposes a gameloop to chuck, gauranteed to be executed once per
        frame deadlock shouldn't happen because both locks are never held at the
        same time */
        bool do_ui = !app->imgui_disabled;

        {
            CQ_SwapQueues(); // ~ .0001ms

            // u64 critical_start = stm_now();
            // Rendering
            if (do_ui) {
                ImGui::Render();

                // copy imgui draw data for rendering later
                snapshot.SnapUsingSwap(ImGui::GetDrawData(), ImGui::GetTime());
            }

            // imgui and window callbacks
            CHUGL_Zero_MouseDeltasAndClickState();
            CHUGL_Kb_ZeroPressedReleased();
            glfwPollEvents();

            if (do_ui) {
                // reset imgui
                ImGui_ImplWGPU_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                // enable docking to main window
                ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(),
                                             ImGuiDockNodeFlags_PassthruCentralNode);
            }
            // ~2.15ms (15%) In DEBUG mode!
            // critical_section_stats.update(stm_since(critical_start));

            // physics
            // we intentionally are NOT having a fixed timestep for the sake of
            // simplicity.
            // instead, rely on vsync + stable framerate
            // https://gafferongames.com/post/fix_your_timestep/
            b2WorldId b2_world_id = *(b2WorldId*)&app->b2_sim_desc.world_id;
            if (b2World_IsValid(b2_world_id)) {
                b2World_Step(b2_world_id, app->b2_sim_desc.rate * app->dt,
                             app->b2_sim_desc.substeps);
                // log_trace("simulating b2 substeps: %d rate: %f",
                //           app->b2_sim_desc.substeps, app->b2_sim_desc.rate);

                // debug draw
                // log_trace("drawing debug");
                // b2World_Draw(app->b2_world_id, &debug_draw);
            }
        }

        // done swapping the double buffer, let chuck know it's good to continue
        // pushing commands this wakes all shreds currently waiting on
        // GG.nextFrame()

        // grabs waitingShredsLock
        Event_Broadcast(CHUGL_EventType::NEXT_FRAME, app->ckapi, app->ckvm);

        // ====================
        // end critical section
        // ====================

        // handle window resize (special case b/c needs to happen before
        // GraphicsContext::prepareFrame, unlike the rest of glfwPollEvents())
        // Doing window resize AFTER surface is already prepared causes crash.
        // Normally you glfwPollEvents() at the start of the frame, but
        // dearImGUI hooks into glfwPollEvents, and modifies imgui state, so
        // glfwPollEvents() must happen in the critial region, after
        // GraphicsContext::prepareFrame
        {
            resized_this_frame = false;
            int width, height;
            glfwGetFramebufferSize(app->window, &width, &height);
            if (width != frame_buffer_width || height != frame_buffer_height) {
                frame_buffer_width  = width;
                frame_buffer_height = height;
                resized_this_frame  = true;

                _onFramebufferResize(app->window, width, height);
            }
        }

        // now apply changes from the command queue chuck is NO Longer writing
        // to. This executes all commands on the command queue, performs actions
        // from CK code essentially applying a diff to bring graphics state up
        // to date with what is done in CK code
        { // flush command queue
            SG_Command* cmd = NULL;
            while (CQ_ReadCommandQueueIter(&cmd)) _R_HandleCommand(app, cmd);
            CQ_ReadCommandQueueClear();
        }

        // garbage collection! delete GPU-side data for any scenegraph objects
        // that were deleted in chuck
        // renderer.ProcessDeletionQueue(
        //   &scene); // IMPORTANT: should happen after flushing command queue

        // now renderer can work on drawing the copied scenegraph
        // renderer.RenderScene(&scene, scene.GetMainCamera());

        // if window minimized, don't render
        bool minimized = glfwGetWindowAttrib(app->window, GLFW_ICONIFIED);
        if (minimized || !GraphicsContext::prepareFrame(&app->gctx)) {
            return;
        }

        { // update webcam textures
            size_t webcam_idx = 0;
            R_Webcam* webcam  = NULL;
            while (Component_WebcamIter(&webcam_idx, &webcam)) {
                R_Webcam::updateTexture(&app->gctx, webcam);
            }
        }

        { // decode all current video textures
            // ==optimize== threadpool for decoding
            size_t video_idx = 0;
            R_Video* video   = NULL;
            while (Component_VideoIter(&video_idx, &video)) {
                if (video->plm) {
                    // log_info("decoding video %d, dt: %f", video->id, dt_sec);
                    plm_decode_last_frame_only(video->plm, dt_sec * video->rate);
                }
            }
        }

        // get fresh window info
        f32 aspect = (app->window_fb_width > 0 && app->window_fb_height > 0) ?
                       (f32)app->window_fb_width / (f32)app->window_fb_height :
                       1.0f;

        // begin walking render graph
        R_Pass* root_pass = Component_GetPass(app->root_pass_id);
        R_Pass* pass      = Component_GetPass(root_pass->sg_pass.next_pass_id);

        char string_buff[128] = {};

        // populate the parts of frameUniforms that are consistent across all passes in
        // a frame
        FrameUniforms frameUniforms = {};
        frameUniforms.time          = app->lastTime;
        frameUniforms.delta_time    = app->dt;
        frameUniforms.resolution
          = glm::ivec3(app->window_fb_width, app->window_fb_height, 1);
        frameUniforms.frame_count = app->fc;
        frameUniforms.sample_rate = app->ck_srate;
        frameUniforms.mouse       = glm::vec2(app->mouse_x / app->window_width,
                                              1.0 - app->mouse_y / app->window_height);
        frameUniforms.mouse_click
          = glm::ivec2(app->mouse_left ? 1 : 0, app->mouse_right ? 1 : 0);

        while (pass) {
            R_Scene* scene   = NULL;
            R_Camera* camera = NULL;

            switch (pass->sg_pass.pass_type) {
                case SG_PassType_Render: break;
                case SG_PassType_Scene: {
                    // get the target scene
                    scene = Component_GetScene(pass->sg_pass.scene_id);
                    if (!scene) break;

                    // defaults to scene main camera
                    camera = pass->sg_pass.camera_id != 0 ?
                               Component_GetCamera(pass->sg_pass.camera_id) :
                               Component_GetCamera(scene->sg_scene_desc.main_camera_id);
                    if (!camera) {
                        log_warn("ScenePass[%d:%s] of Scene[%d:%s] has no camera",
                                 pass->id, pass->name, scene->id, scene->name);
                        break;
                    }
                    ASSERT(camera->scene_id == scene->id);

                    R_Texture* r_tex
                      = Component_GetTexture(pass->sg_pass.color_target_id);

                    // resize texture
                    R_Texture::resize(r_tex, app->window_fb_width,
                                      app->window_fb_height, app->gctx.device);

                    // defaults to swapchain current view if null
                    WGPUTexture color_target
                      = r_tex ? r_tex->gpu_texture : app->gctx.surface_texture.texture;

                    ASSERT(scene && color_target && camera);

                    R_Scene::update(scene, &app->gctx, app->fc, &app->frameArena,
                                    &app->rendergraph, &frameUniforms);
                    ASSERT(scene->last_fc_updated == app->fc);

                    // shadow pass --------------------------------------------
                    snprintf(string_buff, sizeof(string_buff),
                             "Scene Shadow Pass[%d] %s", pass->id, pass->name);
                    // app->rendergraph.addRenderPass(string_buff);
                    // app->rendergraph
                    //   .renderPassDepthTarget()

                    // mesh pass ----------------------------------------------
                    R_Pass::updateScenePass(pass, color_target, app->gctx.device);
                    snprintf(string_buff, sizeof(string_buff),
                             "ScenePass[%d:%s] for Scene[%d:%s]", pass->id,
                             pass->sg_pass.name, scene->id, scene->name);
                    app->rendergraph.addRenderPass(string_buff);
                    if (pass->sg_pass.scene_pass_msaa) {
                        app->rendergraph.renderPassColorTarget(pass->msaa_color_target,
                                                               0);
                        if (r_tex) {
                            app->rendergraph.renderPassResolveTarget(color_target, 0);
                        } else {
                            app->rendergraph.renderPassResolveTarget(
                              app->gctx.backbufferView, app->gctx.surface_format);
                        }
                    } else {
                        if (r_tex) {
                            app->rendergraph.renderPassColorTarget(color_target, 0);
                        } else {
                            app->rendergraph.renderPassColorTarget(
                              app->gctx.backbufferView, app->gctx.surface_format);
                        }
                    }

                    app->rendergraph.renderPassColorOp(
                      WGPUColor{ 0.0f, 0.0f, 0.0f, 1.0f },
                      pass->sg_pass.color_target_clear_on_load ? WGPULoadOp_Clear :
                                                                 WGPULoadOp_Load,
                      WGPUStoreOp_Store);

                    // TODO store depth texture somewhere else...
                    app->rendergraph.renderPassDepthTarget(pass->depth_texture);

                    // viewport and scissor
                    u32 color_height = wgpuTextureGetHeight(color_target);
                    u32 color_width  = wgpuTextureGetWidth(color_target);
                    if (pass->sg_pass.viewport_normalized) {
                        aspect = app->rendergraph.viewport(
                          pass->sg_pass.viewport_x * color_width,
                          pass->sg_pass.viewport_y * color_height,
                          pass->sg_pass.viewport_w * color_width,
                          pass->sg_pass.viewport_h * color_height, color_target);
                    } else {
                        aspect = app->rendergraph.viewport(
                          pass->sg_pass.viewport_x, pass->sg_pass.viewport_y,
                          pass->sg_pass.viewport_w, pass->sg_pass.viewport_h,
                          color_target);
                    }
                    if (pass->sg_pass.scissor_normalized) {
                        app->rendergraph.scissor(pass->sg_pass.scissor_x * color_width,
                                                 pass->sg_pass.scissor_y * color_height,
                                                 pass->sg_pass.scissor_w * color_width,
                                                 pass->sg_pass.scissor_h * color_height,
                                                 color_target);
                    } else {
                        app->rendergraph.scissor(pass->sg_pass.scissor_x,
                                                 pass->sg_pass.scissor_y,
                                                 pass->sg_pass.scissor_w,
                                                 pass->sg_pass.scissor_h, color_target);
                    }

                    G_DrawCallListID dc_list
                      = app->rendergraph.renderPassAddDrawCallList();
                    _R_RenderScene(app, scene, pass, camera, dc_list);
                } break;
                case SG_PassType_Screen: {
                    R_Material* material
                      = Component_GetMaterial(pass->sg_pass.screen_material_id);
                    R_Shader* screen_shader
                      = Component_GetShader(material->pso.sg_shader_id);
                    bool missing_screen_shader = (screen_shader == NULL);
                    if (missing_screen_shader) break;

                    // if user supplied a render texture, render to that instead
                    // otherwise default to backbuffer
                    R_Texture* r_tex
                      = Component_GetTexture(pass->sg_pass.color_target_id);

                    // resize texture
                    R_Texture::resize(r_tex, app->window_fb_width,
                                      app->window_fb_height, app->gctx.device);

                    WGPUTexture color_target = NULL;

                    // set G_Pass
                    app->rendergraph.addRenderPass(pass->sg_pass.name);
                    // defaults to swapchain current view if null
                    if (r_tex) {
                        color_target = r_tex->gpu_texture;
                        app->rendergraph.renderPassColorTarget(r_tex->gpu_texture, 0);
                    } else {
                        color_target = app->gctx.surface_texture.texture;
                        app->rendergraph.renderPassColorTarget(
                          app->gctx.backbufferView, app->gctx.surface_format);
                    }

                    app->rendergraph.renderPassColorOp(
                      WGPUColor{ 0.0f, 0.0f, 0.0f, 1.0f },
                      pass->sg_pass.color_target_clear_on_load ? WGPULoadOp_Clear :
                                                                 WGPULoadOp_Load,
                      WGPUStoreOp_Store);

                    // viewport and scissor
                    u32 color_height = wgpuTextureGetHeight(color_target);
                    u32 color_width  = wgpuTextureGetWidth(color_target);
                    if (pass->sg_pass.viewport_normalized) {
                        aspect = app->rendergraph.viewport(
                          pass->sg_pass.viewport_x * color_width,
                          pass->sg_pass.viewport_y * color_height,
                          pass->sg_pass.viewport_w * color_width,
                          pass->sg_pass.viewport_h * color_height, color_target);
                    } else {
                        aspect = app->rendergraph.viewport(
                          pass->sg_pass.viewport_x, pass->sg_pass.viewport_y,
                          pass->sg_pass.viewport_w, pass->sg_pass.viewport_h,
                          color_target);
                    }
                    if (pass->sg_pass.scissor_normalized) {
                        app->rendergraph.scissor(pass->sg_pass.scissor_x * color_width,
                                                 pass->sg_pass.scissor_y * color_height,
                                                 pass->sg_pass.scissor_w * color_width,
                                                 pass->sg_pass.scissor_h * color_height,
                                                 color_target);
                    } else {
                        app->rendergraph.scissor(pass->sg_pass.scissor_x,
                                                 pass->sg_pass.scissor_y,
                                                 pass->sg_pass.scissor_w,
                                                 pass->sg_pass.scissor_h, color_target);
                    }

                    G_DrawCallListID dc_list
                      = app->rendergraph.renderPassAddDrawCallList();

                    const int screen_pass_binding_location = 1;
                    // create draw call
                    if (material) {
                        G_DrawCall* d = app->rendergraph.addDraw(dc_list);
                        R_BindFrameUniforms(pass->frame_uniform_buffer, &app->gctx, d,
                                            &app->rendergraph, screen_shader, NULL);
                        d->sort_key = G_SortKey::create(false, G_RenderingLayer_World,
                                                        material->id, 0, 1);
                        d->vertex_count   = 3;
                        d->instance_count = 1;
                        R_Material::createBindGroupEntries(
                          material, screen_pass_binding_location, &app->rendergraph, d,
                          &app->gctx);
                        d->pipelineDesc(material->pso.sg_shader_id,
                                        material->pso.cull_mode,
                                        material->pso.primitive_topology, false);
                    }
                } break;
                case SG_PassType_Compute: {
                    R_Material* compute_material
                      = Component_GetMaterial(pass->sg_pass.compute_material_id);
                    R_Shader* compute_shader
                      = Component_GetShader(compute_material->pso.sg_shader_id);

                    // validation
                    if (compute_material) {
                        ASSERT(compute_material->pso.sg_shader_id
                               == (compute_shader ? compute_shader->id : 0));
                    }

                    bool valid_compute_pass = compute_material && compute_shader;
                    if (!valid_compute_pass) break;

                    app->rendergraph.addComputePass(
                      pass->name, compute_shader->compute_shader_module,
                      pass->sg_pass.compute_workgroup.x,
                      pass->sg_pass.compute_workgroup.y,
                      pass->sg_pass.compute_workgroup.z);

                    const int compute_pass_binding_location = 0;
                    R_Material::createBindGroupEntries(
                      compute_material, compute_pass_binding_location,
                      &app->rendergraph, NULL, &app->gctx);
                }; break;
                case SG_PassType_Bloom: {
                    R_Texture* input_texture = Component_GetTexture(
                      pass->sg_pass.bloom_input_render_texture_id);

                    R_Texture* output_texture = Component_GetTexture(
                      pass->sg_pass.bloom_output_render_texture_id);

                    if (!input_texture || !output_texture) break;
                    if (!input_texture->desc.gen_mips
                        || !output_texture->desc.gen_mips) {
                        log_warn(
                          "BloomPass requires `.gen_mips` to be enabled on input and "
                          "output textures. Bypassing.");
                        break;
                    }

                    // resize output texture

                    u32 input_width  = wgpuTextureGetWidth(input_texture->gpu_texture);
                    u32 input_height = wgpuTextureGetHeight(input_texture->gpu_texture);
                    R_Texture::resize(output_texture, input_width, input_height,
                                      app->gctx.device);

                    glm::uvec2 full_res_size = glm::uvec2(input_width, input_height);
                    ASSERT(sizeof(full_res_size) == 2 * sizeof(u32));
                    ASSERT(input_texture->desc.usage
                           & WGPUTextureUsage_RenderAttachment);

                    SG_Sampler bloom_sampler = {
                        // bilinear, clamp to edge
                        SG_SAMPLER_WRAP_CLAMP_TO_EDGE, SG_SAMPLER_WRAP_CLAMP_TO_EDGE,
                        SG_SAMPLER_WRAP_CLAMP_TO_EDGE, SG_SAMPLER_FILTER_LINEAR,
                        SG_SAMPLER_FILTER_LINEAR,      SG_SAMPLER_FILTER_LINEAR,
                    };

                    u32 bloom_mip_levels
                      = G_mipLevelsLimit(input_width, input_height, 1);
                    bloom_mip_levels
                      = MIN(bloom_mip_levels, pass->sg_pass.bloom_num_blur_levels);

                    if (bloom_mip_levels == 0) break;

                    { // downscale
                        R_Material* bloom_downscale_material = Component_GetMaterial(
                          pass->sg_pass.bloom_downsample_material_id);

                        // set the material uniforms that only need to be set once,
                        // not per mip level dispatch
                        R_Material::setSamplerBinding(
                          &app->gctx, bloom_downscale_material, 1, bloom_sampler);

                        R_Material::setUniformBinding( // full resolution
                          &app->gctx, bloom_downscale_material, 3, &full_res_size,
                          sizeof(full_res_size));

                        // downsample, writing from from mip level i --> i + 1
                        for (int i = 0; i < bloom_mip_levels - 1; i++) {
                            char render_pass_label[64] = {};
                            snprintf(render_pass_label, sizeof(render_pass_label),
                                     "bloom downscale to mip level %d", i + 1);

                            R_Material::bindTexture(&app->gctx,
                                                    bloom_downscale_material, 0,
                                                    { input_texture->id, i, 1 });

                            // set G_Pass
                            app->rendergraph.addRenderPass(render_pass_label);
                            app->rendergraph
                              .renderPassColorTarget( // set color target to mip level i
                                                      // + 1
                                input_texture->gpu_texture, i + 1);
                            app->rendergraph.renderPassColorOp(
                              WGPUColor{ 0.0f, 0.0f, 0.0f, 1.0f }, WGPULoadOp_Clear,
                              WGPUStoreOp_Store);
                            G_DrawCallListID dc_list
                              = app->rendergraph.renderPassAddDrawCallList();

                            // create draw call
                            G_DrawCall* d = app->rendergraph.addDraw(dc_list);
                            d->sort_key
                              = G_SortKey::create(false, G_RenderingLayer_World,
                                                  bloom_downscale_material->id, 0, 1);
                            d->vertex_count   = 3;
                            d->instance_count = 1;
                            R_Material::createBindGroupEntries(bloom_downscale_material,
                                                               0, &app->rendergraph, d,
                                                               &app->gctx);
                            d->pipelineDesc(
                              bloom_downscale_material->pso.sg_shader_id,
                              bloom_downscale_material->pso.cull_mode,
                              bloom_downscale_material->pso.primitive_topology, false);
                        } // end for
                    } // end downscale

                    { // upscale
                        R_Material* bloom_upscale_material = Component_GetMaterial(
                          pass->sg_pass.bloom_upsample_material_id);

                        // set the material uniforms that only need to be set once,
                        // not per mip level dispatch
                        R_Material::setSamplerBinding(
                          &app->gctx, bloom_upscale_material, 1, bloom_sampler);

                        R_Material::setUniformBinding( // full resolution
                          &app->gctx, bloom_upscale_material, 3, &full_res_size,
                          sizeof(full_res_size));

                        bool first_upsample = true;
                        // rendering from mip level i + 1 --> i
                        for (int i = bloom_mip_levels - 2; i >= 0; i--) {
                            ASSERT(i >= 0);
                            char upsample_pass_label[64] = {};
                            snprintf(upsample_pass_label, sizeof(upsample_pass_label),
                                     "bloom upsample to mip level %d", i);

                            R_Material::bindTexture(
                              &app->gctx, bloom_upscale_material, 0,
                              { first_upsample ? input_texture->id : output_texture->id,
                                i + 1, 1 });

                            R_Material::bindTexture(&app->gctx, bloom_upscale_material,
                                                    2, { input_texture->id, i, 1 });

                            // set G_Pass
                            app->rendergraph.addRenderPass(upsample_pass_label);
                            app->rendergraph
                              .renderPassColorTarget( // set color target to mip level i
                                output_texture->gpu_texture, i);
                            app->rendergraph.renderPassColorOp(
                              WGPUColor{ 0.0f, 0.0f, 0.0f, 1.0f }, WGPULoadOp_Clear,
                              WGPUStoreOp_Store);
                            G_DrawCallListID dc_list
                              = app->rendergraph.renderPassAddDrawCallList();

                            // create draw call
                            G_DrawCall* d = app->rendergraph.addDraw(dc_list);
                            d->sort_key
                              = G_SortKey::create(false, G_RenderingLayer_World,
                                                  bloom_upscale_material->id, 0, 1);
                            d->vertex_count   = 3;
                            d->instance_count = 1;
                            R_Material::createBindGroupEntries(bloom_upscale_material,
                                                               0, &app->rendergraph, d,
                                                               &app->gctx);
                            d->pipelineDesc(
                              bloom_upscale_material->pso.sg_shader_id,
                              bloom_upscale_material->pso.cull_mode,
                              bloom_upscale_material->pso.primitive_topology, false);

                            first_upsample = false;
                        } // end for
                    } // end upsample
                } break;
                default: ASSERT(false);
            }

            { // write per-frame uniforms
                if (camera) {
                    frameUniforms.projection
                      = R_Camera::projectionMatrix(camera, aspect);
                    frameUniforms.view = R_Camera::viewMatrix(camera);
                    frameUniforms.projection_view_inverse_no_translation
                      = glm::inverse(frameUniforms.projection
                                     * glm::mat4(glm::mat3(frameUniforms.view)));
                    frameUniforms.camera_pos = camera->_pos;
                } else {
                    FrameUniforms_ZeroCameraFields(&frameUniforms);
                }

                if (scene) {
                    frameUniforms.ambient_light    = scene->sg_scene_desc.ambient_light;
                    frameUniforms.num_lights       = R_Scene::numLights(scene);
                    frameUniforms.background_color = scene->sg_scene_desc.bg_color;
                } else {
                    FrameUniforms_ZeroLightingFields(&frameUniforms);
                }
                wgpuQueueWriteBuffer(app->gctx.queue, pass->frame_uniform_buffer, 0,
                                     &frameUniforms, sizeof(frameUniforms));
            }

            pass = Component_GetPass(pass->sg_pass.next_pass_id);
        }

        // TODO: consolidate with GraphicsContext::present/prepareFrame
        // and with imgui pass
        app->rendergraph.executeAndReset(app->gctx.device, app->gctx.commandEncoder);

        // imgui render pass
        if (do_ui && !resized_this_frame) {
            WGPURenderPassColorAttachment imgui_color_attachment = {};
            imgui_color_attachment.view       = app->gctx.backbufferView;
            imgui_color_attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
            imgui_color_attachment.loadOp
              = WGPULoadOp_Load; // DON'T clear the previous frame
            imgui_color_attachment.storeOp = WGPUStoreOp_Store;

            WGPURenderPassDescriptor imgui_render_pass_desc = {};
            imgui_render_pass_desc.label                    = "ImGUI Render Pass";
            imgui_render_pass_desc.colorAttachmentCount     = 1;
            imgui_render_pass_desc.colorAttachments         = &imgui_color_attachment;
            imgui_render_pass_desc.depthStencilAttachment   = NULL;

            WGPURenderPassEncoder render_pass = wgpuCommandEncoderBeginRenderPass(
              app->gctx.commandEncoder, &imgui_render_pass_desc);

            ImGui_ImplWGPU_RenderDrawData(&snapshot.DrawData, render_pass,
                                          ImGui_ImplWGPU_GetTextureId,
                                          (void*)&app->rendergraph.cache);

            wgpuRenderPassEncoderEnd(render_pass);
            wgpuRenderPassEncoderRelease(render_pass);
        }

        GraphicsContext::presentFrame(&app->gctx);
    }

    static void _calculateFPS(GLFWwindow* window, bool print_to_title)
    {
        static f64 lastTime{ glfwGetTime() };
        static u64 frameCount{};
        static char title[256]{};

        // Measure speed
        f64 currentTime = glfwGetTime();
        f64 delta       = currentTime - lastTime;
        frameCount++;
        if (delta >= 1.0) { // If last cout was more than 1 sec ago
            f64 fps = frameCount / delta;
            CHUGL_Window_fps(fps);
            if (print_to_title) {
                snprintf(title, sizeof(title),
                         "ChuGL " CHUGL_VERSION_STRING " FPS: %.2f", fps);
                glfwSetWindowTitle(window, title);
            }

            frameCount = 0;
            lastTime   = currentTime;
        }
    }

    static void _closeCallback(GLFWwindow* window)
    {
        App* app = (App*)glfwGetWindowUserPointer(window);
        log_trace("closing window");

        // ChuGL
        // broadcast WindowCloseEvent
        Event_Broadcast(CHUGL_EventType::WINDOW_CLOSE, app->ckapi, app->ckvm);
        // block closeable
        if (!CHUGL_Window_Closeable()) glfwSetWindowShouldClose(window, GLFW_FALSE);
    }

    static void _contentScaleCallback(GLFWwindow* window, float xscale, float yscale)
    {
        App* app = (App*)glfwGetWindowUserPointer(window);

        // broadcast to chuck
        CHUGL_Window_ContentScale(xscale, yscale);
        // update content scale
        Event_Broadcast(CHUGL_EventType::CONTENT_SCALE, app->ckapi, app->ckvm);
    }

    static void _keyCallback(GLFWwindow* window, int key, int scancode, int action,
                             int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS
            && CHUGL_Window_Closeable()) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            return;
        }

        App* app = (App*)glfwGetWindowUserPointer(window);
        UNUSED_VAR(app);

        if (action == GLFW_PRESS) {
            CHUGL_Kb_action(key, true);
        } else if (action == GLFW_RELEASE) {
            CHUGL_Kb_action(key, false);
        }
    }

    // this is deliberately NOT made a glfw callback because glfwPollEvents()
    // happens AFTER GraphicsContext::PrepareFrame(), after render surface has
    // already been set window resize needs to be handled before the frame is
    // prepared
    static void _onFramebufferResize(GLFWwindow* window, int width, int height)
    {
        log_trace("window resized: %d, %d", width, height);

        App* app              = (App*)glfwGetWindowUserPointer(window);
        app->window_fb_width  = width;
        app->window_fb_height = height;

        // causes inconsistent crash on window resize
        // ImGui_ImplWGPU_InvalidateDeviceObjects();

        GraphicsContext::resize(&app->gctx, width, height);

        // update size stats
        glfwGetWindowSize(window, &app->window_width, &app->window_height);
        CHUGL_Window_Size(app->window_width, app->window_height, width, height);
        // broadcast to chuck
        Event_Broadcast(CHUGL_EventType::WINDOW_RESIZE, app->ckapi, app->ckvm);
    }

    static void _mouseButtonCallback(GLFWwindow* window, int button, int action,
                                     int mods)
    {
        // log_debug("mouse button callback");

        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse) return;

        App* app = (App*)glfwGetWindowUserPointer(window);
        UNUSED_VAR(app);

        // update chugl state
        switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT:
                app->mouse_left = (action == GLFW_PRESS);
                CHUGL_Mouse_LeftButton(action == GLFW_PRESS);
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                app->mouse_right = (action == GLFW_PRESS);
                CHUGL_Mouse_RightButton(action == GLFW_PRESS);
                break;
        }
    }

    static void _scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse) return;

        App* app = (App*)glfwGetWindowUserPointer(window);
        UNUSED_VAR(app);

        CHUGL_scroll_delta(xoffset, yoffset);
    }

    static void _cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
    {
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse) return;

        App* app = (App*)glfwGetWindowUserPointer(window);
        UNUSED_VAR(app);
        app->mouse_x = xpos;
        app->mouse_y = ypos;

        CHUGL_Mouse_Position(xpos, ypos);
    }
};

struct SceneDrawCall {
    SG_ID shader_id;
    SG_ID material_id;
    SG_ID geo_id;
};

// move this into R_Scene, call build drawcall struct?
static void _R_RenderScene(App* app, R_Scene* scene, R_Pass* pass, R_Camera* camera,
                           G_DrawCallListID dc_list)
{
    // form draw call list and sort
    size_t hashmap_idx_DONT_USE = 0;
    GeometryToXforms* primitive = NULL;
    while (
      hashmap_iter(scene->geo_to_xform, &hashmap_idx_DONT_USE, (void**)&primitive)) {
        int instance_count = GeometryToXforms::count(primitive);
        ASSERT(instance_count > 0);

        // Get shader id from material
        R_Material* material = Component_GetMaterial(primitive->key.mat_id);
        bool is_transparent  = material->pso.transparent;
        ASSERT(material);
        R_Geometry* geo  = Component_GetGeometry(primitive->key.geo_id);
        SG_ID shader_id  = material->pso.sg_shader_id;
        R_Shader* shader = Component_GetShader(shader_id);

        // add to draw call list
        G_DrawCall* d = is_transparent ? app->rendergraph.templateDraw() :
                                         app->rendergraph.addDraw(dc_list);

        // populate index buffer
        d->index_count    = R_Geometry::indexCount(geo);
        bool indexed_draw = (d->index_count > 0);
        if (indexed_draw) {
            d->index_buffer        = geo->gpu_index_buffer.buf;
            d->index_buffer_offset = 0;
            d->index_buffer_size   = geo->gpu_index_buffer.size;
        } else {
            // TODO come up with a better way to set a custom number of vertices to draw
            // having -1 actually mean ALL is confusing 2 different states.
            u32 vertex_count                = R_Geometry::vertexCount(geo);
            bool user_provided_vertex_count = geo->vertex_count >= 0;
            d->vertex_count
              = user_provided_vertex_count ? geo->vertex_count : vertex_count;
        }

        // set pso
        d->pipelineDesc(shader_id, material->pso.cull_mode,
                        material->pso.primitive_topology, is_transparent);

        { // set bindgroups
            // set frame uniforms
            R_BindFrameUniforms(pass->frame_uniform_buffer, &app->gctx, d,
                                &app->rendergraph, shader, scene);

            // set material uniforms
            // ==optimize== can sort/cache material bindgroupentries per frame
            // so we don't need to recreate multiple times for a single material
            R_Material::createBindGroupEntries(material, PER_MATERIAL_GROUP,
                                               &app->rendergraph, d, &app->gctx);

            // @group(3) bindings are set below, and are different for transparent vs
            // opaque
            GeometryToXforms::updateStorageBuffer(&app->gctx, scene, primitive,
                                                  &app->gctx.limits);

            // set @group(4) pulled-vertex attribs
            R_Geometry::addPullBindGroupEntries(geo, &app->rendergraph, d);
        }

        // set vertex attributes
        for (int vertex_slot = 0; vertex_slot < ARRAY_LENGTH(geo->gpu_vertex_buffers);
             ++vertex_slot) {
            GPU_Buffer* gpu_buffer = &geo->gpu_vertex_buffers[vertex_slot];
            if (gpu_buffer->buf && gpu_buffer->size > 0)
                app->rendergraph.vertexBuffer(d, vertex_slot, gpu_buffer->buf, 0,
                                              gpu_buffer->size);
        }

        // drawcall fields that are different between opaque and transparent materials
        if (material->pso.transparent) {
            // NOTE: draw transparent materials individually, no instancing
            d->instance_count = 1;

            ASSERT(camera->_stale == R_Transform_STALE_NONE);
            glm::vec3 cam_pos     = glm::vec3(camera->world[3]);
            glm::vec3 cam_forward = camera->world * glm::vec4(0, 0, -1, 0);
            cam_forward           = glm::normalize(cam_forward);

            for (int instance_idx = 0; instance_idx < instance_count; ++instance_idx) {
                G_DrawCall* td     = app->rendergraph.addTemplatedDraw(dc_list);
                td->instance_count = 1;

                DrawUniforms* draw_uniforms
                  = GeometryToXforms::drawUniform(primitive, instance_idx);
                glm::vec3 world_pos    = glm::vec3(draw_uniforms->model[3]);
                float dist_from_camera = 0.0;
                switch (camera->params.camera_type) {
                    case SG_CameraType_PERPSECTIVE: {
                        dist_from_camera = glm::distance(
                          cam_pos,
                          world_pos); // needs to be distance, not distance^2 so we can
                                      // normalize against camera far plane
                    } break;
                    case SG_CameraType_ORTHOGRAPHIC: {
                        dist_from_camera = glm::dot(cam_forward, world_pos);
                    } break;
                }

                td->sort_key
                  = G_SortKey::create(true, G_RenderingLayer_World, material->id,
                                      dist_from_camera, camera->params.far_plane);

                // set @group(2) per-draw bindings (xform matrices)
                // note: must modify the template here because each draw requires a
                // different storage buffer offset
                td->bg_list[2].start = ARENA_LENGTH(
                  app->rendergraph.bind_group_entry_list + 2, G_CacheBindGroupEntry);
                app->rendergraph.bindBuffer(
                  td, PER_DRAW_GROUP, 0, primitive->xform_storage_buffer.buf,
                  instance_idx * primitive->push_size, sizeof(DrawUniforms));
            }
        } else {
            d->instance_count = instance_count;
            float dist_from_camera
              = 0.0; // ==optimize== sort opaque geometry front-to-back
            d->sort_key = G_SortKey::create(false, G_RenderingLayer_World, material->id,
                                            dist_from_camera, camera->params.far_plane);

            // set @group(3) per-draw bindings (xform matrices)
            app->rendergraph.bindBuffer(d, PER_DRAW_GROUP, 0,
                                        primitive->xform_storage_buffer.buf, 0,
                                        primitive->xform_storage_buffer.size);
        }
    }

    { // skybox pass
        R_Material* skybox_material
          = Component_GetMaterial(scene->sg_scene_desc.skybox_material_id);
        if (!skybox_material) return;

        G_DrawCall* d = app->rendergraph.addDraw(dc_list);
        d->sort_key
          = G_SortKey::create(false, G_RenderingLayer_Background, skybox_material->id,
                              camera->params.far_plane, camera->params.far_plane);
        d->vertex_count   = 3;
        d->instance_count = 1;
        d->pipelineDesc(skybox_material->pso.sg_shader_id,
                        skybox_material->pso.cull_mode,
                        skybox_material->pso.primitive_topology,
                        false // not transparent
        );

        R_Shader* skybox_shader
          = Component_GetShader(skybox_material->pso.sg_shader_id);
        R_BindFrameUniforms(pass->frame_uniform_buffer, &app->gctx, d,
                            &app->rendergraph, skybox_shader, scene);
        R_Material::createBindGroupEntries(skybox_material, PER_MATERIAL_GROUP,
                                           &app->rendergraph, d, &app->gctx);
    }
}

// pool of pending mapped buffers
// currently only used for reading texture data back to CPU
// simple, assuming there won't be many outstanding requests
struct BufferMapAsyncData {
    WGPUBuffer buffer; // if null, this slot is not in use
    SG_ID texture_id;
    int size_bytes;
};

static Arena buffer_map_async_data_arena{};
static BufferMapAsyncData* BufferMapAsyncData_Add(int* index)
{
    // linear search for first available slot
    for (int i = 0; i < ARENA_LENGTH(&buffer_map_async_data_arena, BufferMapAsyncData);
         i++) {
        BufferMapAsyncData* data
          = ARENA_GET_TYPE(&buffer_map_async_data_arena, BufferMapAsyncData, i);
        if (!data->buffer) {
            *index = i;
            return data;
        }
    }
    // else at this point we need to add a new one
    *index = ARENA_LENGTH(&buffer_map_async_data_arena, BufferMapAsyncData);
    return ARENA_PUSH_ZERO_TYPE(&buffer_map_async_data_arena, BufferMapAsyncData);
}

static BufferMapAsyncData* BufferMapAsyncData_Get(int index)
{
    ASSERT(index < ARENA_LENGTH(&buffer_map_async_data_arena, BufferMapAsyncData));
    return ARENA_GET_TYPE(&buffer_map_async_data_arena, BufferMapAsyncData, index);
}

// TODO make sure switch statement is in correct order?
static void _R_HandleCommand(App* app, SG_Command* command)
{
    switch (command->type) {
        case SG_COMMAND_SET_FIXED_TIMESTEP: {
            SG_Command_SetFixedTimestep* cmd = (SG_Command_SetFixedTimestep*)command;
            app->stepper_fps                 = cmd->fps;
            if (cmd->fps > 0) {
                nanotime_step_init(&app->stepper,
                                   (u64)(NANOTIME_NSEC_PER_SEC / app->stepper_fps),
                                   nanotime_now_max(), nanotime_now, nanotime_sleep);
            }
        } break;
        case SG_COMMAND_SET_CHUCK_VM_INFO: {
            SG_Command_SetChuckVMInfo* cmd = (SG_Command_SetChuckVMInfo*)command;
            app->ck_srate                  = cmd->srate;
        } break;
        case SG_COMMAND_WINDOW_CLOSE: {
            glfwSetWindowShouldClose(app->window, GLFW_TRUE);
            break;
        }
        case SG_COMMAND_WINDOW_MODE: {
            SG_Command_WindowMode* cmd = (SG_Command_WindowMode*)command;
            switch (cmd->mode) {
                case SG_WINDOW_MODE_FULLSCREEN: {
                    // check if already fullscreen
                    if (glfwGetWindowMonitor(app->window)) break;

                    { // save previous window params before going fullscreen
                        int windowed_width, windowed_height;
                        glfwGetWindowSize(app->window, &windowed_width,
                                          &windowed_height);
                        int windowed_x, windowed_y;
                        glfwGetWindowPos(app->window, &windowed_x, &windowed_y);

                        CHUGL_Window_LastWindowParamsBeforeFullscreen(
                          windowed_width, windowed_height, windowed_x, windowed_y);

                        log_trace(
                          "going fullscreen, saving last windowed params: %d, %d",
                          windowed_width, windowed_height);
                    }

                    GLFWmonitor* monitor    = getCurrentMonitor(app->window);
                    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                    glfwSetWindowMonitor(app->window, monitor, 0, 0, mode->width,
                                         mode->height, mode->refreshRate);
                    // set fullscreen resolution if specified
                    if (cmd->height > 0 && cmd->width > 0) {
                        glfwSetWindowSize(app->window, cmd->width, cmd->height);
                    }
                } break;
                case SG_WINDOW_MODE_WINDOWED: {
                    // get previous position
                    int xpos, ypos;
                    glfwGetWindowPos(app->window, &xpos, &ypos);

                    // get last known window size if cmd does not specify new dimensions
                    int width  = cmd->width;
                    int height = cmd->height;
                    if (width <= 0 || height <= 0) {
                        t_CKVEC4 last_size
                          = CHUGL_Window_LastWindowParamsBeforeFullscreen();
                        width  = last_size.x;
                        height = last_size.y;
                        xpos   = last_size.z;
                        ypos   = last_size.w;
                    }

                    log_trace("windowed: %d, %d. position: %d, %d", width, height, xpos,
                              ypos);

                    glfwSetWindowMonitor(app->window, NULL, xpos, ypos, width, height,
                                         GLFW_DONT_CARE);
                } break;
                case SG_WINDOW_MODE_WINDOWED_FULLSCREEN: {
                    GLFWmonitor* monitor    = getCurrentMonitor(app->window);
                    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                    int mx, my;
                    glfwGetMonitorPos(monitor, &mx, &my);
                    glfwSetWindowMonitor(app->window, NULL, mx, my, mode->width,
                                         mode->height, GLFW_DONT_CARE);
                } break;
            }
        } break;
        case SG_COMMAND_WINDOW_SIZE_LIMITS: {
            SG_Command_WindowSizeLimits* cmd = (SG_Command_WindowSizeLimits*)command;
            glfwSetWindowSizeLimits(
              app->window, (cmd->min_width <= 0) ? GLFW_DONT_CARE : cmd->min_width,
              (cmd->min_height <= 0) ? GLFW_DONT_CARE : cmd->min_height,
              (cmd->max_width <= 0) ? GLFW_DONT_CARE : cmd->max_width,
              (cmd->max_height <= 0) ? GLFW_DONT_CARE : cmd->max_height);
            glfwSetWindowAspectRatio(
              app->window,
              (cmd->aspect_ratio_x <= 0) ? GLFW_DONT_CARE : cmd->aspect_ratio_x,
              (cmd->aspect_ratio_y <= 0) ? GLFW_DONT_CARE : cmd->aspect_ratio_y);
            // reset size to constrain to new limits
            int width, height;
            glfwGetWindowSize(app->window, &width, &height);
            glfwSetWindowSize(app->window, width, height);
            break;
        }
        case SG_COMMAND_WINDOW_POSITION: {
            SG_Command_WindowPosition* cmd = (SG_Command_WindowPosition*)command;
            // set relative to currenet monitor
            GLFWmonitor* monitor = getCurrentMonitor(app->window);
            int mx, my;
            glfwGetMonitorPos(monitor, &mx, &my);
            glfwSetWindowPos(app->window, mx + cmd->x, my + cmd->y);
            break;
        }
        case SG_COMMAND_WINDOW_CENTER: {
            // center window on current monitor
            GLFWmonitor* monitor    = getCurrentMonitor(app->window);
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            int mx, my;
            glfwGetMonitorPos(monitor, &mx, &my);
            int wx, wy;
            glfwGetWindowSize(app->window, &wx, &wy);
            int xpos = mx + (mode->width - wx) / 2;
            int ypos = my + (mode->height - wy) / 2;
            glfwSetWindowPos(app->window, xpos, ypos);
            break;
        }
        case SG_COMMAND_WINDOW_TITLE: {
            SG_Command_WindowTitle* cmd = (SG_Command_WindowTitle*)command;
            glfwSetWindowTitle(app->window,
                               (char*)CQ_ReadCommandGetOffset(cmd->title_offset));
            app->show_fps_title = false; // disable default FPS title
            break;
        }
        case SG_COMMAND_WINDOW_ICONIFY: {
            SG_Command_WindowIconify* cmd = (SG_Command_WindowIconify*)command;
            if (cmd->iconify)
                glfwIconifyWindow(app->window);
            else
                glfwRestoreWindow(app->window);
            break;
        }
        case SG_COMMAND_WINDOW_ATTRIBUTE: {
            ASSERT(false); // not implemented, changing window attributes after
                           // window creation on macOS causes window to
                           // disappear and freeze
            // SG_Command_WindowAttribute* cmd
            //   = (SG_Command_WindowAttribute*)command;
            // switch (cmd->attrib) {
            //     case CHUGL_WINDOW_ATTRIB_DECORATED:
            //         glfwSetWindowAttrib(app->window, GLFW_DECORATED,
            //                             cmd->value ? GLFW_TRUE : GLFW_FALSE);
            //         break;
            //     case CHUGL_WINDOW_ATTRIB_RESIZABLE:
            //         glfwSetWindowAttrib(app->window, GLFW_RESIZABLE,
            //                             cmd->value ? GLFW_TRUE : GLFW_FALSE);
            //         break;
            //     case CHUGL_WINDOW_ATTRIB_FLOATING:
            //         glfwSetWindowAttrib(app->window, GLFW_FLOATING,
            //                             cmd->value ? GLFW_TRUE : GLFW_FALSE);
            //         break;
            //     default: break;
            // }
        }
        case SG_COMMAND_WINDOW_OPACITY: {
            SG_Command_WindowOpacity* cmd = (SG_Command_WindowOpacity*)command;
            glfwSetWindowOpacity(app->window, cmd->opacity);
            break;
        }
        case SG_COMMAND_MOUSE_MODE: {
            SG_Command_MouseMode* cmd = (SG_Command_MouseMode*)command;
            switch (cmd->mode) {
                case 0:
                    glfwSetInputMode(app->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    break;
                case 1:
                    // doesn't work on macos?
                    glfwSetInputMode(app->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                    break;
                case 2:
                    glfwSetInputMode(app->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    // raw mouse motion
                    if (glfwRawMouseMotionSupported())
                        glfwSetInputMode(app->window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
                    break;
            }
            break;
        }
        case SG_COMMAND_MOUSE_CURSOR: {
            SG_Command_MouseCursor* cmd = (SG_Command_MouseCursor*)command;
            if (cmd->mouse_cursor_image_offset == 0 || cmd->width == 0
                || cmd->height == 0) {
                // default to normal cursor
                glfwSetCursor(app->window, NULL);
            } else {
                log_trace("setting custom cursor");
                // create cursor
                GLFWimage image = {};
                image.width     = cmd->width;
                image.height    = cmd->height;
                image.pixels    = (unsigned char*)CQ_ReadCommandGetOffset(
                  cmd->mouse_cursor_image_offset);

                // print image
                // for (int i = 0; i < image.width * image.height; i++)
                //     printf("%d %d %d %d\n", image.pixels[i * 4],
                //            image.pixels[i * 4 + 1], image.pixels[i * 4 + 2],
                //            image.pixels[i * 4 + 3]);

                GLFWcursor* cursor = glfwCreateCursor(&image, cmd->xhot, cmd->yhot);
                if (!cursor) log_error("failed to create cursor");
                glfwSetCursor(app->window, cursor);

                // static unsigned char pixels[16 * 16 * 4];
                // memset(pixels, 0xff, sizeof(pixels));

                // GLFWimage image;
                // image.width  = 16;
                // image.height = 16;
                // image.pixels = pixels;

                // GLFWcursor* cursor = glfwCreateCursor(&image, 0, 0);
                // glfwSetCursor(app->window, cursor);
            }
            break;
        }
        case SG_COMMAND_MOUSE_CURSOR_NORMAL: {
            log_trace("setting normal cursor");
            glfwSetCursor(app->window, NULL);
            break;
        }
        case SG_COMMAND_UI_DISABLED: {
            SG_Command_UI_Disabled* cmd = (SG_Command_UI_Disabled*)command;
            app->imgui_disabled         = cmd->disabled;
            break;
        }
        // b2 ----------------------
        case SG_COMMAND_b2_WORLD_SET: {
            SG_Command_b2World_Set* cmd = (SG_Command_b2World_Set*)command;
            app->b2_sim_desc            = cmd->desc;
        } break;
        // component --------------
        case SG_COMMAND_COMPONENT_UPDATE_NAME: {
            SG_Command_ComponentUpdateName* cmd
              = (SG_Command_ComponentUpdateName*)command;
            R_Component* component = Component_GetComponent(cmd->sg_id);
            char* new_name         = (char*)CQ_ReadCommandGetOffset(cmd->name_offset);
            COPY_STRING(component->name, new_name);

            // if backed by a wgpu type, update the label
            switch (component->type) {
                case SG_COMPONENT_BASE: break; // nothing to do
                case SG_COMPONENT_TRANSFORM: break;
                case SG_COMPONENT_SCENE: break;
                case SG_COMPONENT_GEOMETRY: {
                    // TODO
                } break;
                case SG_COMPONENT_SHADER: {
                    // TODO
                } break;
                case SG_COMPONENT_MATERIAL: {
                    // TODO
                } break;
                case SG_COMPONENT_TEXTURE: {
                    // TODO
                } break;
                case SG_COMPONENT_MESH: break;
                case SG_COMPONENT_CAMERA: break;
                case SG_COMPONENT_PASS: {
                    // TODO
                } break;
                case SG_COMPONENT_BUFFER: {
                    // TODO
                } break;
                case SG_COMPONENT_LIGHT: break;
                default: {
                    ASSERT(FALSE);
                } break;
            }
        } break;
        case SG_COMMAND_COMPONENT_FREE: {
            Component_FreeComponent(((SG_Command_ComponentFree*)command)->id);
        } break;
        case SG_COMMAND_CREATE_XFORM:
            Component_CreateTransform((SG_Command_CreateXform*)command);
            break;
        case SG_COMMAND_ADD_CHILD: {
            SG_Command_AddChild* cmd = (SG_Command_AddChild*)command;
            R_Transform::addChild(Component_GetXform(cmd->parent_id),
                                  Component_GetXform(cmd->child_id));
        } break;
        case SG_COMMAND_REMOVE_CHILD: {
            SG_Command_RemoveChild* cmd = (SG_Command_RemoveChild*)command;
            R_Transform::removeChild(Component_GetXform(cmd->parent),
                                     Component_GetXform(cmd->child));
        } break;
        case SG_COMMAND_REMOVE_ALL_CHILDREN: {
            SG_Command_RemoveAllChildren* cmd = (SG_Command_RemoveAllChildren*)command;
            R_Transform::removeAllChildren(Component_GetXform(cmd->parent));
        } break;
        case SG_COMMAND_SET_POSITION: {
            SG_Command_SetPosition* cmd = (SG_Command_SetPosition*)command;
            R_Transform::pos(Component_GetXform(cmd->sg_id), cmd->pos);
            break;
        }
        case SG_COMMAND_SET_ROTATATION: {
            SG_Command_SetRotation* cmd = (SG_Command_SetRotation*)command;
            R_Transform::rot(Component_GetXform(cmd->sg_id), cmd->rot);
            break;
        }
        case SG_COMMAND_SET_SCALE: {
            SG_Command_SetScale* cmd = (SG_Command_SetScale*)command;
            R_Transform::sca(Component_GetXform(cmd->sg_id), cmd->sca);
            break;
        }
        // scene ----------------------
        case SG_COMMAND_SCENE_UPDATE: {
            SG_Command_SceneUpdate* cmd = (SG_Command_SceneUpdate*)command;
            R_Scene* scene              = Component_GetScene(cmd->sg_id);
            if (!scene)
                scene = Component_CreateScene(&app->gctx, cmd->sg_id, &cmd->desc);
            scene->sg_scene_desc = cmd->desc;
        } break;
        // shaders ----------------------
        case SG_COMMAND_SHADER_CREATE: {
            SG_Command_ShaderCreate* cmd = (SG_Command_ShaderCreate*)command;
            Component_CreateShader(&app->gctx, cmd);
        } break;
        case SG_COMMAND_MATERIAL_CREATE: {
            SG_Command_MaterialCreate* cmd = (SG_Command_MaterialCreate*)command;
            Component_CreateMaterial(&app->gctx, cmd);
        } break;
        case SG_COMMAND_MATERIAL_UPDATE_PSO: {
            SG_Command_MaterialUpdatePSO* cmd = (SG_Command_MaterialUpdatePSO*)command;
            R_Material* material              = Component_GetMaterial(cmd->sg_id);
            material->pso                     = cmd->pso;
        } break;
        case SG_COMMAND_MATERIAL_SET_UNIFORM: {
            SG_Command_MaterialSetUniform* cmd
              = (SG_Command_MaterialSetUniform*)command;
            R_Material* material = Component_GetMaterial(cmd->sg_id);

            switch (cmd->uniform.type) {
                // basic uniform
                case SG_MATERIAL_UNIFORM_FLOAT:
                case SG_MATERIAL_UNIFORM_VEC2F:
                case SG_MATERIAL_UNIFORM_VEC3F:
                case SG_MATERIAL_UNIFORM_VEC4F:
                case SG_MATERIAL_UNIFORM_INT:
                case SG_MATERIAL_UNIFORM_IVEC2:
                case SG_MATERIAL_UNIFORM_IVEC3:
                case SG_MATERIAL_UNIFORM_IVEC4: {
                    SG_MaterialUniformPtrAndSize u_ptr_size
                      = SG_MaterialUniform::data(&cmd->uniform);
                    R_Material::setBinding(&app->gctx, material, cmd->location,
                                           R_BIND_UNIFORM, u_ptr_size.ptr,
                                           u_ptr_size.size);
                } break;
                case SG_MATERIAL_UNIFORM_TEXTURE: {
                    // defaults to entire mip chain
                    R_Texture* texture
                      = Component_GetTexture(cmd->uniform.as.texture_id);
                    R_Material::bindTexture(
                      &app->gctx, material, cmd->location,
                      { texture->id, 0,
                        INT32_MAX }); // using INT_MAX to mean the entire mip chain. In
                                      // R_Material::createBindGroup... we clamp this to
                                      // the actual #mips in the texture.
                                      // hacky, can move to a proper enum flag in struct
                                      // R_TextureBinding when its needed
                } break;
                case SG_MATERIAL_UNIFORM_SAMPLER: {
                    R_Material::setSamplerBinding(&app->gctx, material, cmd->location,
                                                  cmd->uniform.as.sampler);
                } break;
                case SG_MATERIAL_UNIFORM_STORAGE_BUFFER_EXTERNAL: {
                    R_Buffer* buffer
                      = Component_GetBuffer(cmd->uniform.as.storage_buffer_id);
                    R_Material::setExternalStorageBinding(
                      &app->gctx, material, cmd->location, &buffer->gpu_buffer);
                } break;
                case SG_MATERIAL_STORAGE_TEXTURE: {
                    // currently only support mip level 0
                    R_Material::bindTexture(&app->gctx, material, cmd->location,
                                            { cmd->uniform.as.texture_id, 0, 1 });
                } break;
                default: ASSERT(false);
            } // end uniform type switch
        } break;
        case SG_COMMAND_MATERIAL_SET_STORAGE_BUFFER: {
            SG_Command_MaterialSetStorageBuffer* cmd
              = (SG_Command_MaterialSetStorageBuffer*)command;
            R_Material* material = Component_GetMaterial(cmd->sg_id);
            void* data           = CQ_ReadCommandGetOffset(cmd->data_offset);
            R_Material::setBinding(&app->gctx, material, cmd->location, R_BIND_STORAGE,
                                   data, cmd->data_size_bytes);
        } break;
        // mesh -------------------------
        case SG_COMMAND_MESH_UPDATE: {
            SG_Command_MeshUpdate* cmd = (SG_Command_MeshUpdate*)command;
            R_Transform* mesh          = Component_GetMesh(cmd->mesh_id);
            if (!mesh) {
                mesh = Component_CreateMesh(cmd->mesh_id, cmd->geo_id, cmd->mat_id);
            }
            R_Transform::updateMesh(mesh, cmd->geo_id, cmd->mat_id);
        } break;
        case SG_COMMAND_CAMERA_CREATE: {
            SG_Command_CameraCreate* cmd = (SG_Command_CameraCreate*)command;
            Component_CreateCamera(&app->gctx, cmd);
        } break;
        case SG_COMMAND_CAMERA_SET_PARAMS: {
            SG_Command_CameraSetParams* cmd = (SG_Command_CameraSetParams*)command;
            R_Camera* camera                = Component_GetCamera(cmd->camera_id);
            camera->params                  = cmd->params;
        } break;
        // text
        case SG_COMMAND_TEXT_REBUILD: {
            SG_Command_TextRebuild* cmd = (SG_Command_TextRebuild*)command;
            Component_CreateText(&app->gctx, app->FTLibrary, cmd, app->default_font);
        } break;
        case SG_COMMAND_TEXT_DEFAULT_FONT: {
            SG_Command_TextDefaultFont* cmd = (SG_Command_TextDefaultFont*)command;
            R_Font* default_font            = Component_GetFont(
              &app->gctx, app->FTLibrary,
              (char*)CQ_ReadCommandGetOffset(cmd->font_path_str_offset));
            if (default_font) app->default_font = default_font;
        } break;
        // pass
        case SG_COMMAND_PASS_CREATE: {
            ASSERT(false);
            SG_Command_PassCreate* cmd = (SG_Command_PassCreate*)command;
            Component_CreatePass(cmd->pass_id, app->gctx.device);
            if (cmd->pass_type == SG_PassType_Root) {
                app->root_pass_id = cmd->pass_id;
            }
        } break;
        case SG_COMMAND_PASS_UPDATE: {
            SG_Command_PassUpdate* cmd = (SG_Command_PassUpdate*)command;
            R_Pass* pass               = Component_GetPass(cmd->pass.id);

            if (!pass) pass = Component_CreatePass(cmd->pass.id, app->gctx.device);

            pass->sg_pass = cmd->pass; // copy

            if (cmd->pass.pass_type == SG_PassType_Root) app->root_pass_id = pass->id;
        } break;
        case SG_COMMAND_PASS_CONNECT: {
            // wait, we just reuse PassUpdate for this too
        } break;
        case SG_COMMAND_PASS_DISCONNECT: {
            // wait, we just reuse PassUpdate for this too
        } break;
        // Geometry ---------------------
        case SG_COMMAND_GEO_CREATE: {
            SG_Command_GeoCreate* cmd = (SG_Command_GeoCreate*)command;
            Component_CreateGeometry(&app->gctx, cmd->sg_id);
        } break;
        case SG_COMMAND_GEO_SET_VERTEX_ATTRIBUTE: {
            SG_Command_GeoSetVertexAttribute* cmd
              = (SG_Command_GeoSetVertexAttribute*)command;
            R_Geometry::setVertexAttribute(
              &app->gctx, Component_GetGeometry(cmd->sg_id), cmd->location,
              cmd->num_components, CQ_ReadCommandGetOffset(cmd->data_offset),
              cmd->data_size_bytes);
        } break;
        case SG_COMMAND_GEO_SET_PULLED_VERTEX_ATTRIBUTE: {
            SG_Command_GeometrySetPulledVertexAttribute* cmd
              = (SG_Command_GeometrySetPulledVertexAttribute*)command;
            R_Geometry* geo = Component_GetGeometry(cmd->sg_id);

            void* data = CQ_ReadCommandGetOffset(cmd->data_offset);

            R_Geometry::setPulledVertexAttribute(&app->gctx, geo, cmd->location, data,
                                                 cmd->data_bytes);
        } break;
        case SG_COMMAND_GEO_SET_VERTEX_COUNT: {
            SG_Command_GeometrySetVertexCount* cmd
              = (SG_Command_GeometrySetVertexCount*)command;
            R_Geometry* geo   = Component_GetGeometry(cmd->sg_id);
            geo->vertex_count = cmd->count;
        } break;
        case SG_COMMAND_GEO_SET_INDICES_COUNT: {
            SG_Command_GeometrySetIndicesCount* cmd
              = (SG_Command_GeometrySetIndicesCount*)command;
            R_Geometry* geo   = Component_GetGeometry(cmd->sg_id);
            geo->vertex_count = cmd->count;
        } break;
        case SG_COMMAND_GEO_SET_INDICES: {
            SG_Command_GeoSetIndices* cmd = (SG_Command_GeoSetIndices*)command;
            R_Geometry* geo               = Component_GetGeometry(cmd->sg_id);

            u32* indices = (u32*)CQ_ReadCommandGetOffset(cmd->indices_offset);

            R_Geometry::setIndices(&app->gctx, geo, indices, cmd->index_count);
        } break;

        // textures ---------------------
        case SG_COMMAND_TEXTURE_CREATE: {
            SG_Command_TextureCreate* cmd = (SG_Command_TextureCreate*)command;
            Component_CreateTexture(&app->gctx, cmd, app->window_fb_width,
                                    app->window_fb_height);
        } break;
        case SG_COMMAND_TEXTURE_WRITE: {
            SG_Command_TextureWrite* cmd = (SG_Command_TextureWrite*)command;
            R_Texture* texture           = Component_GetTexture(cmd->sg_id);
            void* data                   = CQ_ReadCommandGetOffset(cmd->data_offset);
            R_Texture::write(&app->gctx, texture, &cmd->write_desc, data,
                             cmd->data_size_bytes);
        } break;
        case SG_COMMAND_TEXTURE_FROM_FILE: {
            SG_Command_TextureFromFile* cmd = (SG_Command_TextureFromFile*)command;
            R_Texture* texture              = Component_GetTexture(cmd->sg_id);
            const char* path
              = (const char*)CQ_ReadCommandGetOffset(cmd->filepath_offset);
            R_Texture::load(&app->gctx, texture, path, cmd->flip_vertically,
                            cmd->gen_mips);
        } break;
        case SG_COMMAND_TEXTURE_FROM_RAW_DATA: {
            SG_Command_TextureFromRawData* cmd
              = (SG_Command_TextureFromRawData*)command;
            R_Texture* texture = Component_GetTexture(cmd->sg_id);
            u8* buffer         = (u8*)CQ_ReadCommandGetOffset(cmd->buffer_offset);
            R_Texture::load(&app->gctx, texture, buffer, cmd->buffer_len,
                            cmd->flip_vertically, cmd->gen_mips);
        } break;
        case SG_COMMAND_CUBEMAP_TEXTURE_FROM_FILE: {
            SG_Command_CubemapTextureFromFile* cmd
              = (SG_Command_CubemapTextureFromFile*)command;
            R_Texture* texture = Component_GetTexture(cmd->sg_id);
            const char* right_path
              = (const char*)CQ_ReadCommandGetOffset(cmd->right_face_offset);
            const char* left_path
              = (const char*)CQ_ReadCommandGetOffset(cmd->left_face_offset);
            const char* top_path
              = (const char*)CQ_ReadCommandGetOffset(cmd->top_face_offset);
            const char* bottom_path
              = (const char*)CQ_ReadCommandGetOffset(cmd->bottom_face_offset);
            const char* back_path
              = (const char*)CQ_ReadCommandGetOffset(cmd->back_face_offset);
            const char* front_path
              = (const char*)CQ_ReadCommandGetOffset(cmd->front_face_offset);
            R_Texture::loadCubemap(&app->gctx, texture, right_path, left_path, top_path,
                                   bottom_path, back_path, front_path,
                                   cmd->flip_vertically);
        } break;
        case SG_COMMAND_COPY_TEXTURE_TO_TEXTURE: {
            SG_Command_CopyTextureToTexture* cmd
              = (SG_Command_CopyTextureToTexture*)command;

            R_Texture* src_texture   = Component_GetTexture(cmd->src_texture_id);
            R_Texture* dst_texture   = Component_GetTexture(cmd->dst_texture_id);
            WGPUImageCopyTexture src = SG_TextureLocation::wgpuImageCopyTexture(
              cmd->src_location, src_texture->gpu_texture);
            WGPUImageCopyTexture dst = SG_TextureLocation::wgpuImageCopyTexture(
              cmd->dst_location, dst_texture->gpu_texture);
            WGPUExtent3D copy_size
              = { (u32)cmd->width, (u32)cmd->height, (u32)cmd->depth };

            // TODO: have command encoder be created at START of frame, while processing
            // all the render commands then can reuse in main render pass, don't need to
            // duplicate
            WGPUCommandEncoder cmd_encoder
              = wgpuDeviceCreateCommandEncoder(app->gctx.device, NULL);
            wgpuCommandEncoderCopyTextureToTexture(cmd_encoder, &src, &dst, &copy_size);
            WGPUCommandBuffer command_buffer
              = wgpuCommandEncoderFinish(cmd_encoder, NULL);
            ASSERT(command_buffer != NULL);
            WGPU_RELEASE_RESOURCE(CommandEncoder, cmd_encoder)

            // Sumbit commmand buffer
            wgpuQueueSubmit(app->gctx.queue, 1, &command_buffer);

            WGPU_RELEASE_RESOURCE(CommandBuffer, command_buffer)
        } break;
        case SG_COMMAND_COPY_TEXTURE_TO_CPU: {
            SG_Command_CopyTextureToCPU* cmd = (SG_Command_CopyTextureToCPU*)command;
            R_Texture* tex                   = Component_GetTexture(cmd->id);
            // Experimentation for the "Playing with buffer" chapter

            // TODO string arena in graphics.h for building 1-time labels (use
            // asprintf?)
            char label[256] = {};
            snprintf(label, sizeof(label) - 1, "Mapped Buffer for Texture[%d] %s",
                     tex->id, tex->name);
            WGPUBufferDescriptor bufferDesc = {};
            bufferDesc.label                = label;
            bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_MapRead;
            bufferDesc.size  = NEXT_MULT4(R_Texture::sizeBytes(tex));
            WGPUBuffer mapped_buffer
              = wgpuDeviceCreateBuffer(app->gctx.device, &bufferDesc);

            { // gpu command
                WGPUCommandEncoder cmd_encoder
                  = wgpuDeviceCreateCommandEncoder(app->gctx.device, NULL);

                // currently only support copying entire texture at mip 0
                WGPUImageCopyTexture copy_location = {};
                copy_location.texture              = tex->gpu_texture;

                // TODO share command encoder across entire CQ flush

                // TODO allow specifying a certain region
                // for now just copying the entire texture
                WGPUImageCopyBuffer copy_buffer = {};
                copy_buffer.buffer              = mapped_buffer;
                copy_buffer.layout.bytesPerRow
                  = tex->desc.width * G_bytesPerTexel(tex->desc.format);
                copy_buffer.layout.rowsPerImage = tex->desc.height;
                WGPUExtent3D copy_size // size in texels
                  = { (u32)tex->desc.width, (u32)tex->desc.height, 1 };
                // copy to mapped buffer
                wgpuCommandEncoderCopyTextureToBuffer(cmd_encoder, &copy_location,
                                                      &copy_buffer, &copy_size);

                WGPUCommandBuffer command_buffer
                  = wgpuCommandEncoderFinish(cmd_encoder, NULL);
                ASSERT(command_buffer != NULL);
                WGPU_RELEASE_RESOURCE(CommandEncoder, cmd_encoder)

                // Sumbit commmand buffer
                wgpuQueueSubmit(app->gctx.queue, 1, &command_buffer);

                WGPU_RELEASE_RESOURCE(CommandBuffer, command_buffer)
            }

            { // map buffer
                auto onBufferMapped = [](WGPUBufferMapAsyncStatus status, void* udata) {
                    BufferMapAsyncData* data = BufferMapAsyncData_Get((intptr_t)udata);
                    R_Texture* tex           = Component_GetTexture(data->texture_id);

                    if (status != WGPUBufferMapAsyncStatus_Success) {
                        CQ_PushCommand_G2A_TextureRead(tex->id, NULL, 0, status);
                    }

                    // Get a pointer to wherever the driver mapped the GPU memory to
                    // the RAM
                    u8* bufferData = (u8*)wgpuBufferGetConstMappedRange(
                      data->buffer, 0, data->size_bytes);

                    void* copied_buffer_data = malloc(data->size_bytes);
                    memcpy(copied_buffer_data, bufferData, data->size_bytes);

                    // std::cout << "bufferData = [";
                    // for (int i = 0; i < 16; ++i) {
                    //     if (i > 0) std::cout << ", ";
                    //     std::cout << (int)bufferData[i];
                    // }
                    // std::cout << "]" << std::endl;

                    // Then do not forget to unmap the memory
                    wgpuBufferUnmap(data->buffer);

                    // this also removes from buffer map async data arena
                    // by setting buffer to null
                    WGPU_RELEASE_RESOURCE(Buffer, data->buffer);

                    // send data back to CQ
                    CQ_PushCommand_G2A_TextureRead(tex->id, copied_buffer_data,
                                                   data->size_bytes, status);
                };

                // == optimize == use mapped buffer pool

                int index                = 0;
                BufferMapAsyncData* data = BufferMapAsyncData_Add(&index);
                data->buffer             = mapped_buffer;
                data->texture_id         = tex->id;
                data->size_bytes         = R_Texture::sizeBytes(tex);

                wgpuBufferMapAsync(mapped_buffer, WGPUMapMode_Read, 0, bufferDesc.size,
                                   onBufferMapped, (void*)(intptr_t)index);
            }

        } break;
        // buffers ----------------------
        case SG_COMMAND_BUFFER_UPDATE: {
            SG_Command_BufferUpdate* cmd = (SG_Command_BufferUpdate*)command;
            R_Buffer* buffer             = Component_GetBuffer(cmd->buffer_id);
            if (!buffer) {
                buffer = Component_CreateBuffer(cmd->buffer_id);
            }

            // rebuild if necessary
            GPU_Buffer::resizeNoCopy(&app->gctx, &buffer->gpu_buffer, cmd->desc.size,
                                     cmd->desc.usage);
        } break;
        case SG_COMMAND_BUFFER_WRITE: {
            SG_Command_BufferWrite* cmd = (SG_Command_BufferWrite*)command;
            R_Buffer* buffer            = Component_GetBuffer(cmd->buffer_id);
            void* data                  = CQ_ReadCommandGetOffset(cmd->data_offset);

            GPU_Buffer::write(&app->gctx, &buffer->gpu_buffer, buffer->gpu_buffer.usage,
                              cmd->offset_bytes, data, cmd->data_size_bytes);

        } break;
        case SG_COMMAND_LIGHT_UPDATE: {
            SG_Command_LightUpdate* cmd = (SG_Command_LightUpdate*)command;
            R_Light* light              = Component_GetLight(cmd->light_id);
            if (!light)
                light = Component_CreateLight(cmd->light_id, &cmd->desc,
                                              app->gctx.device, &app->gctx.limits);
            light->desc = cmd->desc; // copy light properties
        } break;
        case SG_COMMAND_SHADOW_ADD_MESH: {
            SG_Command_ShadowAddMesh* cmd = (SG_Command_ShadowAddMesh*)command;
            R_Light* light                = Component_GetLight(cmd->light_id);
            light->shadowAddMesh(
              (SG_ID*)CQ_ReadCommandGetOffset(cmd->mesh_id_list_offset),
              cmd->mesh_id_list_len, cmd->add);
        } break;
        case SG_COMMAND_MESH_SET_SHADOWED: {
            SG_Command_MeshSetShadowed* cmd = (SG_Command_MeshSetShadowed*)command;
            R_Transform* mesh               = Component_GetMesh(cmd->mesh_id);
            if (mesh->receives_shadows != cmd->shadowed) {
                mesh->receives_shadows = cmd->shadowed;
                R_Scene::markPrimitiveStale(Component_GetScene(mesh->scene_id), mesh);
            }
        } break;
        case SG_COMMAND_VIDEO_UPDATE: {
            SG_Command_VideoUpdate* cmd = (SG_Command_VideoUpdate*)command;
            R_Video* video              = Component_GetVideo(cmd->video_id);
            // TODO handle updating video file path here
            if (!video)
                Component_CreateVideo(
                  &app->gctx, cmd->video_id,
                  (const char*)CQ_ReadCommandGetOffset(cmd->path_offset),
                  cmd->rgba_video_texture_id);
        } break;
        case SG_COMMAND_VIDEO_SEEK: {
            SG_Command_VideoSeek* cmd = (SG_Command_VideoSeek*)command;
            R_Video* video            = Component_GetVideo(cmd->video_id);
            if (video && video->plm) {
                plm_seek(video->plm, cmd->time_secs, false);
            }
        } break;
        case SG_COMMAND_VIDEO_RATE: {
            SG_Command_VideoRate* cmd = (SG_Command_VideoRate*)command;
            R_Video* video            = Component_GetVideo(cmd->video_id);
            if (video && video->plm) {
                video->rate = cmd->rate;
                plm_set_loop(video->plm, cmd->loop);
            }
        } break;
        case SG_COMMAND_WEBCAM_CREATE: {
            SG_Command_WebcamCreate* cmd = (SG_Command_WebcamCreate*)command;
            Component_CreateWebcam(cmd);
        } break;
        case SG_COMMAND_WEBCAM_UPDATE: {
            SG_Command_WebcamUpdate* cmd = (SG_Command_WebcamUpdate*)command;
            R_Webcam::update(cmd);
        } break;
        default: {
            log_error("unhandled command type: %d", command->type);
            ASSERT(false);
        }
    }
}
