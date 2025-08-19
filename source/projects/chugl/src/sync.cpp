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

#include <condition_variable>
#include <unordered_map>

#include "core/memory.h"
#include "core/spinlock.h"

#include <glfw/include/GLFW/glfw3.h>

#include "ulib_helper.h"

static std::unordered_map<Chuck_VM_Shred*, bool> registeredShreds;

static spinlock waitingShredsLock;
static u64 waitingShreds              = 0; // guarded by waitingShredsLock
static i64 waiting_shreds_frame_count = 0; // guarded by waitingShredsLock

static std::mutex gameLoopLock; // lock for condition variable
static std::condition_variable gameLoopConditionVar;
static bool shouldRender = false;

// ============================================================================
// Shared Audio/Graphics Thread State
// ============================================================================

// Window State (Don't modify directly, use API functions)
struct CHUGL_Window {
    bool closeable   = true;
    bool transparent = false;
    bool floating    = false;
    bool resizable   = true;
    bool decorated   = true;

    // window size (in screen coordinates)
    int window_width = 1280, window_height = 960;

    // last window params before going fullscreen
    int last_window_width  = 1280,
        last_window_height = 960; // last window size before going fullscreen
    int last_window_x = 0, last_window_y = 0;

    // framebuffer size (in pixels)
    int framebuffer_width, framebuffer_height;

    // content scale
    float content_scale_x, content_scale_y;

    // window frame size
    int window_frame_left, window_frame_top, window_frame_right, window_frame_bottom;

    float window_opacity;

    f64 fps; // updated every second by the graphics thread

    // locks
    spinlock window_lock;
};
CHUGL_Window chugl_window;

static f64 window_dt_sec = 0;
static spinlock window_dt_lock;

void CHUGL_Window_dt(f64 dt)
{
    spinlock::lock(&window_dt_lock);
    window_dt_sec = dt;
    spinlock::unlock(&window_dt_lock);
}

f64 CHUGL_Window_dt()
{
    spinlock::lock(&window_dt_lock);
    f64 dt = window_dt_sec;
    spinlock::unlock(&window_dt_lock);
    return dt;
}

void CHUGL_Window_fps(f64 fps)
{
    spinlock::lock(&chugl_window.window_lock);
    chugl_window.fps = fps;
    spinlock::unlock(&chugl_window.window_lock);
}

f64 CHUGL_Window_fps()
{
    spinlock::lock(&chugl_window.window_lock);
    f64 fps = chugl_window.fps;
    spinlock::unlock(&chugl_window.window_lock);
    return fps;
}

void CHUGL_Window_Closeable(bool closeable)
{
    spinlock::lock(&chugl_window.window_lock);
    chugl_window.closeable = closeable;
    spinlock::unlock(&chugl_window.window_lock);
}

bool CHUGL_Window_Closeable()
{
    spinlock::lock(&chugl_window.window_lock);
    bool closeable = chugl_window.closeable;
    spinlock::unlock(&chugl_window.window_lock);
    return closeable;
}

void CHUGL_Window_Floating(bool floating)
{
    spinlock::lock(&chugl_window.window_lock);
    chugl_window.floating = floating;
    spinlock::unlock(&chugl_window.window_lock);
}

bool CHUGL_Window_Floating()
{
    spinlock::lock(&chugl_window.window_lock);
    bool floating = chugl_window.floating;
    spinlock::unlock(&chugl_window.window_lock);
    return floating;
}

void CHUGL_Window_Transparent(bool transparent)
{
    spinlock::lock(&chugl_window.window_lock);
    chugl_window.transparent = transparent;
    spinlock::unlock(&chugl_window.window_lock);
}

bool CHUGL_Window_Transparent()
{
    spinlock::lock(&chugl_window.window_lock);
    bool transparent = chugl_window.transparent;
    spinlock::unlock(&chugl_window.window_lock);
    return transparent;
}

void CHUGL_Window_Resizable(bool resizable)
{
    spinlock::lock(&chugl_window.window_lock);
    chugl_window.resizable = resizable;
    spinlock::unlock(&chugl_window.window_lock);
}

bool CHUGL_Window_Resizable()
{
    spinlock::lock(&chugl_window.window_lock);
    bool resizable = chugl_window.resizable;
    spinlock::unlock(&chugl_window.window_lock);
    return resizable;
}

void CHUGL_Window_Decorated(bool decorated)
{
    spinlock::lock(&chugl_window.window_lock);
    chugl_window.decorated = decorated;
    spinlock::unlock(&chugl_window.window_lock);
}

bool CHUGL_Window_Decorated()
{
    spinlock::lock(&chugl_window.window_lock);
    bool decorated = chugl_window.decorated;
    spinlock::unlock(&chugl_window.window_lock);
    return decorated;
}

void CHUGL_Window_Size(int window_width, int window_height, int framebuffer_width,
                       int framebuffer_height)
{
    spinlock::lock(&chugl_window.window_lock);
    chugl_window.window_width       = window_width;
    chugl_window.window_height      = window_height;
    chugl_window.framebuffer_width  = framebuffer_width;
    chugl_window.framebuffer_height = framebuffer_height;
    spinlock::unlock(&chugl_window.window_lock);
}

void CHUGL_Window_LastWindowParamsBeforeFullscreen(int window_width, int window_height,
                                                   int x, int y)
{
    spinlock::lock(&chugl_window.window_lock);
    chugl_window.last_window_width  = window_width;
    chugl_window.last_window_height = window_height;
    chugl_window.last_window_x      = x;
    chugl_window.last_window_y      = y;
    spinlock::unlock(&chugl_window.window_lock);
}

t_CKVEC4 CHUGL_Window_LastWindowParamsBeforeFullscreen()
{
    t_CKVEC4 params = {};
    spinlock::lock(&chugl_window.window_lock);
    params.x = chugl_window.last_window_width;
    params.y = chugl_window.last_window_height;
    params.z = chugl_window.last_window_x;
    params.w = chugl_window.last_window_y;
    spinlock::unlock(&chugl_window.window_lock);
    return params;
}

t_CKVEC2 CHUGL_Window_WindowSize()
{
    t_CKVEC2 size = {};
    spinlock::lock(&chugl_window.window_lock);
    size.x = chugl_window.window_width;
    size.y = chugl_window.window_height;
    spinlock::unlock(&chugl_window.window_lock);
    return size;
}

t_CKVEC2 CHUGL_Window_FramebufferSize()
{
    t_CKVEC2 size{};
    spinlock::lock(&chugl_window.window_lock);
    size.x = chugl_window.framebuffer_width;
    size.y = chugl_window.framebuffer_height;
    spinlock::unlock(&chugl_window.window_lock);
    return size;
}

void CHUGL_Window_ContentScale(float x, float y)
{
    spinlock::lock(&chugl_window.window_lock);
    chugl_window.content_scale_x = x;
    chugl_window.content_scale_y = y;
    spinlock::unlock(&chugl_window.window_lock);
}

t_CKVEC2 CHUGL_Window_ContentScale()
{
    t_CKVEC2 scale{};
    spinlock::lock(&chugl_window.window_lock);
    scale.x = chugl_window.content_scale_x;
    scale.y = chugl_window.content_scale_y;
    spinlock::unlock(&chugl_window.window_lock);
    return scale;
}

// Mouse State (Don't modify directly, use API functions)
struct CHUGL_Mouse {
    double xpos = 0.0, ypos = 0.0;
    double dx = 0.0, dy = 0.0;

    bool left_button           = false;
    bool right_button          = false;
    bool left_button_click     = false;
    bool right_button_click    = false;
    bool left_button_released  = false;
    bool right_button_released = false;

    double scroll_dx = 0.0, scroll_dy = 0.0;

    spinlock mouse_lock;
};
CHUGL_Mouse chugl_mouse;

void CHUGL_Mouse_Position(double xpos, double ypos)
{
    spinlock::lock(&chugl_mouse.mouse_lock);
    // update deltas
    chugl_mouse.dx   = xpos - chugl_mouse.xpos;
    chugl_mouse.dy   = ypos - chugl_mouse.ypos;
    chugl_mouse.xpos = xpos;
    chugl_mouse.ypos = ypos;
    spinlock::unlock(&chugl_mouse.mouse_lock);
}

t_CKVEC2 CHUGL_Mouse_Position()
{
    t_CKVEC2 pos = {};
    spinlock::lock(&chugl_mouse.mouse_lock);
    pos.x = chugl_mouse.xpos;
    pos.y = chugl_mouse.ypos;
    spinlock::unlock(&chugl_mouse.mouse_lock);
    return pos;
}

t_CKVEC2 CHUGL_Mouse_Delta()
{
    t_CKVEC2 delta = {};
    spinlock::lock(&chugl_mouse.mouse_lock);
    delta.x = chugl_mouse.dx;
    delta.y = chugl_mouse.dy;
    spinlock::unlock(&chugl_mouse.mouse_lock);
    return delta;
}

void CHUGL_Zero_MouseDeltasAndClickState()
{
    spinlock::lock(&chugl_mouse.mouse_lock);
    chugl_mouse.dx                    = 0.0;
    chugl_mouse.dy                    = 0.0;
    chugl_mouse.left_button_click     = false;
    chugl_mouse.right_button_click    = false;
    chugl_mouse.left_button_released  = false;
    chugl_mouse.right_button_released = false;
    chugl_mouse.scroll_dx             = 0.0;
    chugl_mouse.scroll_dy             = 0.0;
    spinlock::unlock(&chugl_mouse.mouse_lock);
}

bool CHUGL_Mouse_LeftButton()
{
    spinlock::lock(&chugl_mouse.mouse_lock);
    bool left_button = chugl_mouse.left_button;
    spinlock::unlock(&chugl_mouse.mouse_lock);
    return left_button;
}

void CHUGL_Mouse_LeftButton(bool left_button)
{
    spinlock::lock(&chugl_mouse.mouse_lock);
    chugl_mouse.left_button          = left_button;
    chugl_mouse.left_button_click    = left_button;
    chugl_mouse.left_button_released = !left_button;
    spinlock::unlock(&chugl_mouse.mouse_lock);
}

bool CHUGL_Mouse_RightButton()
{
    spinlock::lock(&chugl_mouse.mouse_lock);
    bool right_button = chugl_mouse.right_button;
    spinlock::unlock(&chugl_mouse.mouse_lock);
    return right_button;
}

void CHUGL_Mouse_RightButton(bool right_button)
{
    spinlock::lock(&chugl_mouse.mouse_lock);
    chugl_mouse.right_button          = right_button;
    chugl_mouse.right_button_click    = right_button;
    chugl_mouse.right_button_released = !right_button;
    spinlock::unlock(&chugl_mouse.mouse_lock);
}

bool CHUGL_Mouse_LeftButtonClick()
{
    spinlock::lock(&chugl_mouse.mouse_lock);
    bool left_button_click = chugl_mouse.left_button_click;
    spinlock::unlock(&chugl_mouse.mouse_lock);
    return left_button_click;
}

bool CHUGL_Mouse_RightButtonClick()
{
    spinlock::lock(&chugl_mouse.mouse_lock);
    bool right_button_click = chugl_mouse.right_button_click;
    spinlock::unlock(&chugl_mouse.mouse_lock);
    return right_button_click;
}

bool CHUGL_Mouse_LeftButtonReleased()
{
    spinlock::lock(&chugl_mouse.mouse_lock);
    bool left_button_released = chugl_mouse.left_button_released;
    spinlock::unlock(&chugl_mouse.mouse_lock);
    return left_button_released;
}

bool CHUGL_Mouse_RightButtonReleased()
{
    spinlock::lock(&chugl_mouse.mouse_lock);
    bool right_button_released = chugl_mouse.right_button_released;
    spinlock::unlock(&chugl_mouse.mouse_lock);
    return right_button_released;
}

void CHUGL_scroll_delta(double xoffset, double yoffset)
{
    spinlock::lock(&chugl_mouse.mouse_lock);
    chugl_mouse.scroll_dx = xoffset;
    chugl_mouse.scroll_dy = yoffset;
    spinlock::unlock(&chugl_mouse.mouse_lock);
}

t_CKVEC2 CHUGL_scroll_delta()
{
    t_CKVEC2 delta = {};
    spinlock::lock(&chugl_mouse.mouse_lock);
    delta.x = chugl_mouse.scroll_dx;
    delta.y = chugl_mouse.scroll_dy;
    spinlock::unlock(&chugl_mouse.mouse_lock);
    return delta;
}

struct CHUGL_KbKey {
    unsigned char pressed : 1;
    unsigned char released : 1;
};

struct CHUGL_KbKeyState {
    bool down;
    bool pressed;
    bool released;
};

struct {
    // 3 fields per key
    // down (1 if pressed, 0 if not)
    // click (1 on the frame the key is pressed, 0 otherwise)
    // release (1 on the frame the key is released, 0 otherwise)
    spinlock lock;
    CHUGL_KbKey keys[GLFW_KEY_LAST + 1]; // separate for quick memzero on each frame
    bool keys_down[GLFW_KEY_LAST + 1];
} CHUGL_Kb;

// resets the per-frame pressed and released states of all keys
void CHUGL_Kb_ZeroPressedReleased()
{
    spinlock::lock(&CHUGL_Kb.lock);
    memset(CHUGL_Kb.keys, 0, sizeof(CHUGL_Kb.keys));
    spinlock::unlock(&CHUGL_Kb.lock);
}

// called on the frame the key is pressed or released
// if pressed, down = true
// if released, down = false
void CHUGL_Kb_action(int key, bool down)
{
    spinlock::lock(&CHUGL_Kb.lock);
    CHUGL_Kb.keys_down[key]     = down;
    CHUGL_Kb.keys[key].pressed  = down;
    CHUGL_Kb.keys[key].released = !down;
    spinlock::unlock(&CHUGL_Kb.lock);
}

CHUGL_KbKeyState CHUGL_Kb_key(int key)
{
    spinlock::lock(&CHUGL_Kb.lock);
    CHUGL_KbKeyState k = { CHUGL_Kb.keys_down[key], (bool)CHUGL_Kb.keys[key].pressed,
                           (bool)CHUGL_Kb.keys[key].released };
    spinlock::unlock(&CHUGL_Kb.lock);
    return k;
}

// copy all the keys pressed and released
// size_bytes is the size in bytes of the given `keys` array
void CHUGL_Kb_copyAllKeysPressedReleased(CHUGL_KbKey* keys, u64 size_bytes)
{
    ASSERT(size_bytes == sizeof(CHUGL_Kb.keys));
    spinlock::lock(&CHUGL_Kb.lock);
    memcpy(keys, CHUGL_Kb.keys, sizeof(CHUGL_Kb.keys));
    spinlock::unlock(&CHUGL_Kb.lock);
}

void CHUGL_Kb_copyAllKeysHeldDown(bool* keys, u64 size_bytes)
{
    ASSERT(size_bytes == sizeof(CHUGL_Kb.keys_down));
    spinlock::lock(&CHUGL_Kb.lock);
    memcpy(keys, CHUGL_Kb.keys_down, sizeof(CHUGL_Kb.keys_down));
    spinlock::unlock(&CHUGL_Kb.lock);
}

// ============================================================================
// ChuGL Event API
// ============================================================================

#define CHUGL_EventTable                                                               \
    X(NEXT_FRAME = 0, "NextFrameEvent")                                                \
    X(WINDOW_RESIZE, "WindowResizeEvent")                                              \
    X(WINDOW_CLOSE, "WindowCloseEvent")                                                \
    X(CONTENT_SCALE, "ContentScaleChangedEvent")

enum CHUGL_EventType {
#define X(name, str) name,
    CHUGL_EventTable
#undef X
      CHUGL_EVENT_TYPE_COUNT
};

static const char* CHUGL_EventTypeNames[CHUGL_EVENT_TYPE_COUNT] = {
#define X(name, str) str,
    CHUGL_EventTable
#undef X
};

void Event_Broadcast(CHUGL_EventType type, CK_DL_API api, Chuck_VM* vm);
void Event_Broadcast(Chuck_Event* ck_event);
Chuck_Event* Event_Get(CHUGL_EventType type, CK_DL_API api, Chuck_VM* vm);
const char* Event_GetName(CHUGL_EventType type);

// ============================================================================
// Thread Synchronization API
// ============================================================================

bool Sync_HasShredWaited(Chuck_VM_Shred* shred);
void Sync_MarkShredWaited(Chuck_VM_Shred* shred);
void Sync_RegisterShred(Chuck_VM_Shred* shred);
void Sync_UnregisterShred(Chuck_VM_Shred* shred);
int Sync_NumShredsRegistered();
void Sync_WaitOnUpdateDone();
void Sync_SignalUpdateDone();

// ============================================================================
// ChuGL Events Definitions
// ============================================================================

static CBufferSimple* chuckEventQueue              = NULL;
static Chuck_Event* events[CHUGL_EVENT_TYPE_COUNT] = {};

static void Event_Init(CK_DL_API api, Chuck_VM* vm)
{
    if (chuckEventQueue == NULL) chuckEventQueue = api->vm->create_event_buffer(vm);

    for (u32 i = 0; i < CHUGL_EVENT_TYPE_COUNT; i++) {
        if (events[i] != NULL) continue;
        events[i] = (Chuck_Event*)chugin_createCkObj(CHUGL_EventTypeNames[i], true);
    }
}

void Event_Broadcast(CHUGL_EventType type, CK_DL_API api, Chuck_VM* vm)
{
    if (events[type] == NULL) Event_Init(api, vm);
    switch (type) {
        case CHUGL_EventType::NEXT_FRAME: {
            spinlock::lock(&waitingShredsLock);
            waitingShreds = 0;
            waiting_shreds_frame_count++;
            api->vm->queue_event(vm, events[type], 1, chuckEventQueue);
            spinlock::unlock(&waitingShredsLock);
            return;
        }
        default: api->vm->queue_event(vm, events[type], 1, chuckEventQueue);
    }
}

void Event_Broadcast(Chuck_Event* ck_event)
{
    g_chuglAPI->vm->queue_event(g_chuglVM, ck_event, 1, chuckEventQueue);
}

Chuck_Event* Event_Get(CHUGL_EventType type, CK_DL_API api, Chuck_VM* vm)
{
    if (events[type] == NULL) Event_Init(api, vm);
    return events[type];
}

const char* Event_GetName(CHUGL_EventType type)
{
    return CHUGL_EventTypeNames[type];
}

// ============================================================================
// Thread Synchronization Definitions
// ============================================================================

// TODO: prob want to put syncer and SG_component managers into single
// ChuGL_Context struct

bool Sync_IsShredRegistered(Chuck_VM_Shred* shred)
{
    return registeredShreds.find(shred) != registeredShreds.end();
}

bool Sync_HasShredWaited(Chuck_VM_Shred* shred)
{
    if (!Sync_IsShredRegistered(shred)) return true; // never called before
    return registeredShreds[shred];
}

void Sync_MarkShredWaited(Chuck_VM_Shred* shred)
{
    ASSERT(Sync_IsShredRegistered(shred));
    registeredShreds[shred] = true;
}

void Sync_RegisterShred(Chuck_VM_Shred* shred)
{
    registeredShreds[shred] = false;
}

void Sync_UnregisterShred(Chuck_VM_Shred* shred)
{
    registeredShreds.erase(shred);
}

int Sync_NumShredsRegistered()
{
    return (int)registeredShreds.size();
}

void Sync_WaitOnUpdateDone()
{
    std::unique_lock<std::mutex> lock(gameLoopLock);
    gameLoopConditionVar.wait(lock, []() { return shouldRender; });
    shouldRender = false;
}

void Sync_SignalUpdateDone()
{
    std::unique_lock<std::mutex> lock(gameLoopLock);
    shouldRender = true;
    lock.unlock();
    gameLoopConditionVar.notify_all();
}
