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
// Wrap this in a namespace to keep it separate from the C++ API
namespace cimgui
{
#include <cimgui/cimgui.h>
}

#include "sg_command.h"
#include "sg_component.h"
#include "ulib_helper.h"

// imgui knobs library
#include <imgui-knobs/imgui-knobs.cpp>

static bool verifyInitialization(Chuck_VM_Shred* shred)
{
    static bool printed      = false;
    bool is_shred_registered = Sync_IsShredRegistered(shred);
    if (!is_shred_registered && !printed) {
        printed = true;
        log_warn("UI function called without initialization");
        log_warn(" |- (hint: are you missing a call to GG.nextFrame() => now?)");
    }
    return is_shred_registered;
}

// ============================================================================
// Declarations
// ============================================================================

static void ui_scenegraph_draw_impl(SG_Transform* node)
{
    if (!node) return;

    static char buffer[128] = {};
    snprintf(buffer, ARRAY_LENGTH(buffer), "[%s %d] %s", SG_CKNames[node->type],
             node->id, node->name);

    // note: TreeNode(...) needs a unique string label for each node to disambiguate
    // clicks
    if (cimgui::ImGui_TreeNode(buffer)) {
        cimgui::ImGui_SeparatorText("Transform");

        if (ImGui::DragFloat3("Position", &node->pos[0], .01f)) {
            CQ_PushCommand_SetPosition(node);
        };

        glm::vec3 rot = SG_Transform::eulerRotationRadians(node);
        // if (cimgui::ImGui_DragFloat3("Rotation", &rot[0])) {
        if (ImGui::DragFloat3("Rotation", &rot[0], .01f)) {
            node->rot = glm::quat(rot);
            CQ_PushCommand_SetRotation(node);
        }

        if (ImGui::DragFloat3("Scale", &node->sca[0], .01f)) {
            CQ_PushCommand_SetScale(node);
        }

        // struct SG_OrbitCameraParams {
        //     SphericalCoords spherical = { 6.0f, 0.0f, 0.0f }; // *relative to target*
        //     f32 speed                 = 0.01f;
        //     f32 zoom_speed            = 0.5f;
        //     glm::vec3 target          = glm::vec3(0.0f); // point to look at
        // };

        // struct SG_FlyCameraParams {
        //     f32 speed             = 2.5f;
        //     f32 mouse_sensitivity = 0.003f;
        // };

        // if (node->type == SG_COMPONENT_CAMERA) {
        //     SG_Camera* camera = (SG_Camera*)node;
        //     switch (camera->controller_type) {
        //         case SG_CameraControllerType_Orbit: {
        //             SG_OrbitCameraParams* orbit = &camera->orbit;
        //             ImGui::DragFloat("Orbit Speed", &orbit->speed, 0.01f);
        //             ImGui::DragFloat("Orbit Zoom Speed", &orbit->zoom_speed, 0.01f);
        //             ImGui::DragFloat3("Orbit Target", &orbit->target[0], 0.1f);
        //         } break;
        //         case SG_CameraControllerType_Fly: {
        //         } break;
        //         default: break;
        //     }
        // }

        if (node->type == SG_COMPONENT_MESH) {
            SG_Mesh* mesh = SG_GetMesh(node->id);
            cimgui::ImGui_SeparatorText("Mesh");

            // material info
            SG_Material* material = SG_GetMaterial(mesh->_mat_id);
            if (material) {
                // TODO add modifiable material properties
                // after figuring out material system, (possibly linking uniform name
                // with shader) can display material properties (we know the type
                // already) and give control come back to this after finalizing PBR
                // Material

                snprintf(buffer, ARRAY_LENGTH(buffer), "%s: [%s %d] %s",
                         SG_MaterialTypeNames[material->material_type],
                         SG_CKNames[material->type], material->id, material->name);
                cimgui::ImGui_Text("%s", buffer);

                // material options TODO add rest of builtins
                switch (material->material_type) {
                    case SG_MATERIAL_PHONG: {
                        if (ImGui::ColorEdit3(
                              "Diffuse", (float*)PhongParams::diffuse(material), 0)) {
                            PhongParams::diffuse(
                              material, glm::vec3(*PhongParams::diffuse(material)));
                        }

                        if (ImGui::ColorEdit3(
                              "Specular", (float*)PhongParams::specular(material), 0)) {
                            PhongParams::specular(
                              material, glm::vec3(*PhongParams::specular(material)));
                        }
                        if (ImGui::ColorEdit3(
                              "Emission", (float*)PhongParams::emission(material), 0)) {
                            PhongParams::emission(
                              material, glm::vec3(*PhongParams::emission(material)));
                        }

                        if (ImGui::DragFloat("shine", PhongParams::shininess(material),
                                             0.05f)) {
                            PhongParams::shininess(material,
                                                   *PhongParams::shininess(material));
                        }

                        if (cimgui::ImGui_SliderFloat(
                              "Normal Factor", PhongParams::normalFactor(material),
                              0.0f, 1.0f)) {
                            PhongParams::normalFactor(
                              material, *PhongParams::normalFactor(material));
                        }

                        if (cimgui::ImGui_SliderFloat("AO Factor",
                                                      PhongParams::aoFactor(material),
                                                      0.0f, 1.0f)) {
                            PhongParams::aoFactor(material,
                                                  *PhongParams::aoFactor(material));
                        }

                        // phong envmap params
                        // PhongParams::envmapMethod(material,
                        // SG_ENVMAP_SAMPLE_REFLECT);
                        // PhongParams::envmapBlendMode(material, SG_ENVMAP_BLEND_NONE);
                        static const char* envmapMethods[] = {
                            "None",
                            "Reflect",
                            "Refract",
                        };

                        static const char* envmapBlendModes[] = {
                            "None",
                            "Add",
                            "Multiply",
                            "Mix",
                        };

                        if (cimgui::ImGui_SliderFloat(
                              "Envmap Intensity",
                              PhongParams::envmapIntensity(material), 0.0f, 1.0f)) {
                            PhongParams::envmapIntensity(
                              material, *PhongParams::envmapIntensity(material));
                        }

                        if (cimgui::ImGui_SliderFloat(
                              "Refraction Ratio",
                              PhongParams::envmapRefractionRatio(material), 0.0f,
                              1.0f)) {
                            PhongParams::envmapRefractionRatio(
                              material, *PhongParams::envmapRefractionRatio(material));
                        }

                        if (cimgui::ImGui_ListBox(
                              "Envmap Method",
                              (int*)PhongParams::envmapMethod(material), envmapMethods,
                              3, -1)) {
                            PhongParams::envmapMethod(
                              material, (SG_EnvmapSampleMode)*PhongParams::envmapMethod(
                                          material));
                        }

                        if (cimgui::ImGui_ListBox(
                              "Envmap Blend Mode",
                              (int*)PhongParams::envmapBlendMode(material),
                              envmapBlendModes, 4, -1)) {
                            PhongParams::envmapBlendMode(
                              material,
                              (SG_EnvmapBlendMode)*PhongParams::envmapBlendMode(
                                material));
                        }
                    } break;
                    default: {
                    }
                }
            }

            // geometry info
            SG_Geometry* geometry = SG_GetGeometry(mesh->_geo_id);
            if (geometry) {
                snprintf(buffer, ARRAY_LENGTH(buffer), "Geometry: [%s %d] %s",
                         SG_CKNames[geometry->type], geometry->id, geometry->name);
                cimgui::ImGui_Text("%s", buffer);
            }
        }

        // children
        int numChildren = ARENA_LENGTH(&node->childrenIDs, SG_ID);
        cimgui::ImGui_SeparatorText("Children: ");
        for (int i = 0; i < numChildren; i++) {
            SG_Transform* child
              = SG_GetTransform(*ARENA_GET_TYPE(&node->childrenIDs, SG_ID, i));
            ui_scenegraph_draw_impl(child);
        }

        // pop tree
        cimgui::ImGui_TreePop();
    }
}

// ChuGL Custom Widgets
CK_DLL_SFUN(ui_scenegraph)
{
    if (!verifyInitialization(SHRED)) return;

    // get the root node
    Chuck_Object* root_ckobj = GET_NEXT_OBJECT(ARGS);
    SG_Transform* node
      = SG_GetTransform(OBJ_MEMBER_UINT(root_ckobj, component_offset_id));
    if (!node) return;

    if (!cimgui::ImGui_CollapsingHeader("Scenegraph", 0)) return;

    ui_scenegraph_draw_impl(node);
}

// Config
CK_DLL_SFUN(ui_set_disabled)
{
    CQ_PushCommand_UI_Disabled(GET_NEXT_INT(ARGS));
}

// IO
CK_DLL_SFUN(ui_want_capture_mouse);
CK_DLL_SFUN(ui_want_capture_keyboard);
CK_DLL_SFUN(ui_AddFontFromFileTTF); // TODO: doesn't work, can't add font between
                                    // ImGui::newFrame and ImGui::render

CK_DLL_SFUN(ui_io_get_ConfigFlags);
CK_DLL_SFUN(ui_io_set_ConfigFlags);
CK_DLL_SFUN(ui_io_get_BackendFlags);
CK_DLL_SFUN(ui_io_set_BackendFlags);
CK_DLL_SFUN(ui_io_get_DisplaySize);
CK_DLL_SFUN(ui_io_get_DeltaTime);
CK_DLL_SFUN(ui_io_get_IniSavingRate);
CK_DLL_SFUN(ui_io_set_IniSavingRate);
CK_DLL_SFUN(ui_io_get_IniFilename);
CK_DLL_SFUN(ui_io_set_IniFilename);
CK_DLL_SFUN(ui_io_get_LogFilename);
CK_DLL_SFUN(ui_io_set_LogFilename);
CK_DLL_SFUN(ui_io_get_FontGlobalScale);
CK_DLL_SFUN(ui_io_set_FontGlobalScale);

// ImFontAtlas* Fonts; // <auto>           // Font atlas: load, rasterize and pack one
// or
//                     // more fonts into a single texture.
// bool FontAllowUserScaling; // = false          // Allow user scaling text of
// individual
//                            // window with CTRL+Wheel.
// ImFont* FontDefault; // = NULL           // Font to use on NewFrame(). Use NULL to
// uses
//                      // Fonts->Fonts[0].
// ImVec2 DisplayFramebufferScale; // = (1, 1)         // For retina display or other
//                                 // situations where window coordinates are different
//                                 // from framebuffer coordinates. This generally ends
//                                 up
//                                 // in ImDrawData::FramebufferScale.

// Docking options (when ImGuiConfigFlags_DockingEnable is set)
// bool ConfigDockingNoSplit; // = false          // Simplified docking mode: disable
//                            // window splitting, so docking is limited to merging
//                            // multiple windows together into tab-bars.
// bool
//   ConfigDockingWithShift; // = false          // Enable docking with holding Shift
//   key
//                           // (reduce visual noise, allows dropping in wider space)
// bool ConfigDockingAlwaysTabBar; // = false          // [BETA] [FIXME: This currently
//                                 // creates regression with auto-sizing and general
//                                 // overhead] Make every single floating window
//                                 display
//                                 // within a docking node.
// bool
//   ConfigDockingTransparentPayload; // = false          // [BETA] Make window or
//   viewport
//                                    // transparent when docking and only display
//                                    docking
//                                    // boxes on the target viewport. Useful if
//                                    rendering
//                                    // of multiple viewport cannot be synced. Best
//                                    used
//                                    // with ConfigViewportsNoAutoMerge.

// Viewport options (when ImGuiConfigFlags_ViewportsEnable is set)
// bool
//   ConfigViewportsNoAutoMerge; // = false;         // Set to make all floating imgui
//                               // windows always create their own viewport. Otherwise,
//                               // they are merged into the main host viewports when
//                               // overlapping it. May also set
//                               // ImGuiViewportFlags_NoAutoMerge on individual
//                               viewport.
// bool
//   ConfigViewportsNoTaskBarIcon; // = false          // Disable default OS task bar
//   icon
//                                 // flag for secondary viewports. When a viewport
//                                 doesn't
//                                 // want a task bar icon,
//                                 // ImGuiViewportFlags_NoTaskBarIcon will be set on
//                                 it.
// bool ConfigViewportsNoDecoration; // = true           // Disable default OS window
//                                   // decoration flag for secondary viewports. When a
//                                   // viewport doesn't want window decorations,
//                                   // ImGuiViewportFlags_NoDecoration will be set on
//                                   it.
//                                   // Enabling decoration can create subsequent issues
//                                   at
//                                   // OS levels (e.g. minimum window size).
// bool
//   ConfigViewportsNoDefaultParent; // = false          // Disable default OS parenting
//   to
//                                   // main viewport for secondary viewports. By
//                                   default,
//                                   // viewports are marked with ParentViewportId =
//                                   // <main_viewport>, expecting the platform backend
//                                   to
//                                   // setup a parent/child relationship between the OS
//                                   // windows (some backend may ignore this). Set to
//                                   true
//                                   // if you want the default to be 0, then all
//                                   viewports
//                                   // will be top-level OS windows.

// Miscellaneous options
// bool MouseDrawCursor; // = false          // Request ImGui to draw a mouse cursor for
//                       // you (if you are on a platform without a mouse cursor).
//                       Cannot
//                       // be easily renamed to 'io.ConfigXXX' because this is
//                       frequently
//                       // used by backend implementations.
// bool
//   ConfigMacOSXBehaviors; // = defined(__APPLE__) // Swap Cmd<>Ctrl keys + OS X style
//                          // text editing cursor movement using Alt instead of Ctrl,
//                          // Shortcuts using Cmd/Super instead of Ctrl, Line/Text
//                          Start
//                          // and End using Cmd+Arrows instead of Home/End, Double
//                          click
//                          // selects by word instead of selecting whole text,
//                          // Multi-selection in lists uses Cmd/Super instead of Ctrl.
// bool
//   ConfigInputTrickleEventQueue;  // = true           // Enable input queue trickling:
//                                  // some types of events submitted during the same
//                                  frame
//                                  // (e.g. button down + up) will be spread over
//                                  multiple
//                                  // frames, improving interactions with low
//                                  framerates.
// bool ConfigInputTextCursorBlink; // = true           // Enable blinking cursor
// (optional
//                                  // as some users consider it to be distracting).
// bool
//   ConfigInputTextEnterKeepActive; // = false          // [BETA] Pressing Enter will
//   keep
//                                   // item active and select contents (single-line
//                                   only).
// bool ConfigDragClickToInputText;  // = false          // [BETA] Enable turning
// DragXXX
//                                   // widgets into text input with a simple mouse
//                                   // click-release (without moving). Not desirable on
//                                   // devices without a keyboard.
// bool
//   ConfigWindowsResizeFromEdges; // = true           // Enable resizing of windows
//   from
//                                 // their edges and from the lower-left corner. This
//                                 // requires (io.BackendFlags &
//                                 // ImGuiBackendFlags_HasMouseCursors) because it
//                                 needs
//                                 // mouse cursor feedback. (This used to be a
//                                 per-window
//                                 // ImGuiWindowFlags_ResizeFromAnySide flag)
// bool
//   ConfigWindowsMoveFromTitleBarOnly; // = false       // Enable allowing to move
//   windows
//                                      // only when clicking on their title bar. Does
//                                      not
//                                      // apply to windows without a title bar.
// float ConfigMemoryCompactTimer;      // = 60.0f          // Timer (in seconds) to
// free
//                                 // transient windows/tables memory buffers when
//                                 unused.
//                                 // Set to -1.0f to disable.

// // Inputs Behaviors
// // (other variables, ones which are expected to be tweaked within UI code, are
// exposed
// // in ImGuiStyle)
// float MouseDoubleClickTime; // = 0.30f          // Time for a double-click, in
// seconds. float MouseDoubleClickMaxDist; // = 6.0f           // Distance threshold to
// stay in to
//                                // validate a double-click, in pixels.
// float MouseDragThreshold; // = 6.0f           // Distance threshold before
// considering
//                           // we are dragging.
// float
//   KeyRepeatDelay;    // = 0.275f         // When holding a key/button, time before it
//                      // starts repeating, in seconds (for buttons in Repeat mode,
//                      etc.).
// float KeyRepeatRate; // = 0.050f         // When holding a key/button, rate at which
// it
//                      // repeats, in seconds.

// //------------------------------------------------------------------
// // Debug options
// //------------------------------------------------------------------

// // Option to enable various debug tools showing buttons that will call the
// // IM_DEBUG_BREAK() macro.
// // - The Item Picker tool will be available regardless of this being enabled, in
// order
// // to maximize its discoverability.
// // - Requires a debugger being attached, otherwise IM_DEBUG_BREAK() options will
// appear
// // to crash your application.
// //   e.g. io.ConfigDebugIsDebuggerPresent = ::IsDebuggerPresent() on Win32, or refer
// to
// //   ImOsIsDebuggerPresent() imgui_test_engine/imgui_te_utils.cpp for a Unix
// compatible
// //   version).
// bool ConfigDebugIsDebuggerPresent; // = false          // Enable various tools
// calling
//                                    // IM_DEBUG_BREAK().

// // Tools to test correct Begin/End and BeginChild/EndChild behaviors.
// // - Presently Begin()/End() and BeginChild()/EndChild() needs to ALWAYS be called in
// // tandem, regardless of return value of BeginXXX()
// // - This is inconsistent with other BeginXXX functions and create confusion for many
// // users.
// // - We expect to update the API eventually. In the meanwhile we provide tools to
// // facilitate checking user-code behavior.
// bool ConfigDebugBeginReturnValueOnce; // = false          // First-time calls to
//                                       // Begin()/BeginChild() will return false.
//                                       NEEDS
//                                       // TO BE SET AT APPLICATION BOOT TIME if you
//                                       don't
//                                       // want to miss windows.
// bool
//   ConfigDebugBeginReturnValueLoop; // = false          // Some calls to
//                                    // Begin()/BeginChild() will return false. Will
//                                    cycle
//                                    // through window depths then repeat. Suggested
//                                    use:
//                                    // add "io.ConfigDebugBeginReturnValue =
//                                    io.KeyShift"
//                                    // in your main loop then occasionally press
//                                    SHIFT.
//                                    // Windows should be flickering while running.

// // Option to deactivate io.AddFocusEvent(false) handling.
// // - May facilitate interactions with a debugger when focus loss leads to clearing
// // inputs data.
// // - Backends may have other side-effects on focus loss, so this will reduce
// // side-effects but not necessary remove all of them.
// bool ConfigDebugIgnoreFocusLoss; // = false          // Ignore
// io.AddFocusEvent(false),
//                                  // consequently not calling io.ClearInputKeys() in
//                                  // input processing.

// // Option to audit .ini data
// bool
//   ConfigDebugIniSettings; // = false          // Save .ini data with extra comments
//                           // (particularly helpful for Docking, but makes saving
//                           slower)

// //------------------------------------------------------------------
// // Platform Functions
// // (the imgui_impl_xxxx backend files are setting those up for you)
// //------------------------------------------------------------------

// // Optional: Platform/Renderer backend name (informational only! will be displayed in
// // About Window) + User data for backend/wrappers to store their own stuff.
// const char* BackendPlatformName; // = NULL
// const char* BackendRendererName; // = NULL
// void* BackendPlatformUserData;   // = NULL           // User data for platform
// backend void* BackendRendererUserData;   // = NULL           // User data for
// renderer backend void* BackendLanguageUserData; // = NULL           // User data for
// non C++ programming
//                                // language backend

// // Optional: Access OS clipboard
// // (default to use native Win32 clipboard on Windows, otherwise uses a private
// // clipboard. Override to access OS clipboard on other architectures)
// const char* (*GetClipboardTextFn)(void* user_data);
// void (*SetClipboardTextFn)(void* user_data, const char* text);
// void* ClipboardUserData;

// // Optional: Notify OS Input Method Editor of the screen position of your cursor for
// // text input position (e.g. when using Japanese/Chinese IME on Windows) (default to
// use
// // native imm32 api on Windows)
// void (*SetPlatformImeDataFn)(ImGuiViewport* viewport, ImGuiPlatformImeData* data);

// // Optional: Platform locale
// ImWchar PlatformLocaleDecimalPoint; // '.'              // [Experimental] Configure
//                                     // decimal point e.g. '.' or ',' useful for some
//                                     // languages (e.g. German), generally pulled from
//                                     // *localeconv()->decimal_point

// //------------------------------------------------------------------
// // Input - Call before calling NewFrame()
// //------------------------------------------------------------------
// //------------------------------------------------------------------
// // Output - Updated by NewFrame() or EndFrame()/Render()
// // (when reading from the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
// dispatch
// // your inputs, it is
// //  generally easier and more correct to use their state BEFORE calling NewFrame().
// See
// //  FAQ for details!)
// //------------------------------------------------------------------

// bool WantCaptureMouse; // Set when Dear ImGui will use mouse inputs, in this case do
// not
//                        // dispatch them to your main game/application (either way,
//                        // always pass on mouse inputs to imgui). (e.g. unclicked
//                        mouse
//                        // is hovering over an imgui window, widget is active, mouse
//                        was
//                        // clicked over an imgui window, etc.).
// bool WantCaptureKeyboard; // Set when Dear ImGui will use keyboard inputs, in this
// case
//                           // do not dispatch them to your main game/application
//                           (either
//                           // way, always pass keyboard inputs to imgui). (e.g.
//                           InputText
//                           // active, or an imgui window is focused and navigation is
//                           // enabled, etc.).
// bool WantTextInput; // Mobile/console: when set, you may display an on-screen
// keyboard.
//                     // This is set by Dear ImGui when it wants textual keyboard input
//                     to
//                     // happen (e.g. when a InputText widget is active).
// bool WantSetMousePos; // MousePos has been altered, backend should reposition mouse
// on
//                       // next frame. Rarely used! Set only when
//                       // ImGuiConfigFlags_NavEnableSetMousePos flag is enabled.
// bool
//   WantSaveIniSettings; // When manual .ini load/save is active (io.IniFilename ==
//   NULL),
//                        // this will be set to notify your application that you can
//                        call
//                        // SaveIniSettingsToMemory() and save yourself. Important:
//                        clear
//                        // io.WantSaveIniSettings yourself after saving!
// bool NavActive;        // Keyboard/Gamepad navigation is currently allowed (will
// handle
//                 // ImGuiKey_NavXXX events) = a window is focused and it doesn't use
//                 the
//                 // ImGuiWindowFlags_NoNavInputs flag.
// bool NavVisible; // Keyboard/Gamepad navigation is visible and allowed (will handle
//                  // ImGuiKey_NavXXX events).
// float Framerate; // Estimate of application framerate (rolling average over 60
// frames,
//                  // based on io.DeltaTime), in frame per second. Solely for
//                  convenience.
//                  // Slow applications may not want to use a moving average or may
//                  want
//                  // to reset underlying buffers occasionally.
// int MetricsRenderVertices; // Vertices output during last call to Render()
// int MetricsRenderIndices;  // Indices output during last call to Render() = number of
//                            // triangles * 3
// int MetricsRenderWindows;  // Number of visible windows
// int MetricsActiveWindows;  // Number of active windows
// ImVec2 MouseDelta; // Mouse delta. Note that this is zero if either current or
// previous
//                    // position are invalid (-FLT_MAX,-FLT_MAX), so a
//                    // disappearing/reappearing mouse won't have a huge delta.
// =============== END ImguiIO struct =============

// ImguiIO MVARs -------------------------------------------------------------
// CIMGUI_API void ImGuiIO_AddKeyEvent(ImGuiIO* self, ImGuiKey key, bool down); // Queue
// a new key down/up event. Key should be "translated" (as in, generally ImGuiKey_A
// matches the key end-user would use to emit an 'A' character) CIMGUI_API void
// ImGuiIO_AddKeyAnalogEvent(ImGuiIO* self, ImGuiKey key, bool down, float v); // Queue
// a new key down/up event for analog values (e.g. ImGuiKey_Gamepad_ values). Dead-zones
// should be handled by the backend. CIMGUI_API void ImGuiIO_AddMousePosEvent(ImGuiIO*
// self, float x, float y);                                            // Queue a mouse
// position update. Use -FLT_MAX,-FLT_MAX to signify no mouse (e.g. app not focused and
// not hovered) CIMGUI_API void ImGuiIO_AddMouseButtonEvent(ImGuiIO* self, int button,
// bool down);                                    // Queue a mouse button change
// CIMGUI_API void ImGuiIO_AddMouseWheelEvent(ImGuiIO* self, float wheel_x, float
// wheel_y);                              // Queue a mouse wheel update. wheel_y<0:
// scroll down, wheel_y>0: scroll up, wheel_x<0: scroll right, wheel_x>0: scroll left.
// CIMGUI_API void ImGuiIO_AddMouseSourceEvent(ImGuiIO* self, ImGuiMouseSource source);
// // Queue a mouse source change (Mouse/TouchScreen/Pen) CIMGUI_API void
// ImGuiIO_AddMouseViewportEvent(ImGuiIO* self, ImGuiID id); // Queue a mouse hovered
// viewport. Requires backend to set ImGuiBackendFlags_HasMouseHoveredViewport to call
// this (for multi-viewport support). CIMGUI_API void ImGuiIO_AddFocusEvent(ImGuiIO*
// self, bool focused);                                                   // Queue a
// gain/loss of focus for the application (generally based on OS/platform focus of your
// window) CIMGUI_API void ImGuiIO_AddInputCharacter(ImGuiIO* self, unsigned int c); //
// Queue a new character input CIMGUI_API void ImGuiIO_AddInputCharacterUTF16(ImGuiIO*
// self, ImWchar16 c);                                           // Queue a new
// character input from a UTF-16 character, it can be a surrogate CIMGUI_API void
// ImGuiIO_AddInputCharactersUTF8(ImGuiIO* self, const char* str); // Queue a new
// characters input from a UTF-8 string CIMGUI_API void
// ImGuiIO_SetKeyEventNativeData(ImGuiIO* self, ImGuiKey key, int native_keycode, int
// native_scancode);  // Implied native_legacy_index = -1 CIMGUI_API void
// ImGuiIO_SetKeyEventNativeDataEx(ImGuiIO* self, ImGuiKey key, int native_keycode, int
// native_scancode, int native_legacy_index /* = -1 */); // [Optional] Specify index for
// legacy <1.87 IsKeyXXX() functions with native indices + specify native keycode,
// scancode. CIMGUI_API void ImGuiIO_SetAppAcceptingEvents(ImGuiIO* self, bool
// accepting_events);                                  // Set master flag for accepting
// key/mouse/text events (default to true). Useful if you have native dialog boxes that
// are interrupting your application loop/refresh, and you want to disable events being
// queued while your app is frozen. CIMGUI_API void ImGuiIO_ClearEventsQueue(ImGuiIO*
// self);                                                              // Clear all
// incoming events. CIMGUI_API void ImGuiIO_ClearInputKeys(ImGuiIO* self); // Clear
// current keyboard/mouse/gamepad state + current frame text input buffer. Equivalent to
// releasing all keys/buttons.

// Main
CK_DLL_SFUN(ui_get_style);

// Demo, Debug, Information
CK_DLL_SFUN(ui_ShowDemoWindow);
CK_DLL_SFUN(ui_ShowMetricsWindow);
CK_DLL_SFUN(ui_ShowDebugLogWindow);
CK_DLL_SFUN(ui_showStyleEditor);
CK_DLL_SFUN(ui_ShowIDStackToolWindowEx);
CK_DLL_SFUN(ui_ShowAboutWindow);
CK_DLL_SFUN(ui_ShowStyleSelector);
CK_DLL_SFUN(ui_ShowFontSelector);
CK_DLL_SFUN(ui_ShowUserGuide);
CK_DLL_SFUN(ui_GetVersion);

// Windows
CK_DLL_SFUN(ui_begin);
CK_DLL_SFUN(ui_begin_no_options);
CK_DLL_SFUN(ui_end);

// Child Windows
CK_DLL_SFUN(ui_BeginChild);
CK_DLL_SFUN(ui_BeginChildID);
CK_DLL_SFUN(ui_EndChild);

// Windows Utilities
// - 'current window' = the window we are appending into while inside a
// Begin()/End() block. 'next window' = next window we will Begin() into.
CK_DLL_SFUN(ui_IsWindowAppearing);
CK_DLL_SFUN(ui_IsWindowCollapsed);
CK_DLL_SFUN(ui_IsWindowFocused);
CK_DLL_SFUN(ui_IsWindowHovered);
// CK_DLL_SFUN(ui_GetWindowDrawList);
CK_DLL_SFUN(ui_GetWindowDpiScale);
CK_DLL_SFUN(ui_GetWindowPos);
CK_DLL_SFUN(ui_GetWindowSize);
CK_DLL_SFUN(ui_GetWindowWidth);
CK_DLL_SFUN(ui_GetWindowHeight);
CK_DLL_SFUN(ui_GetWindowViewport);

// TODO add dock builder, viewport

// Window manipulation
CK_DLL_SFUN(ui_SetNextWindowPos);
CK_DLL_SFUN(ui_SetNextWindowPosEx);
CK_DLL_SFUN(ui_SetNextWindowSize);
CK_DLL_SFUN(ui_SetNextWindowSizeConstraints);
CK_DLL_SFUN(ui_SetNextWindowContentSize);
CK_DLL_SFUN(ui_SetNextWindowCollapsed);
CK_DLL_SFUN(ui_SetNextWindowFocus);
CK_DLL_SFUN(ui_SetNextWindowScroll);
CK_DLL_SFUN(ui_SetNextWindowBgAlpha);
CK_DLL_SFUN(ui_SetNextWindowViewport);

// Content region
CK_DLL_SFUN(ui_GetContentRegionAvail); // == GetContentRegionMax() - GetCursorPos()
CK_DLL_SFUN(ui_GetContentRegionMax);   // current content boundaries (typically window
                                       // boundaries including scrolling, or current
                                       // column boundaries), in windows coordinates
CK_DLL_SFUN(ui_GetWindowContentRegionMin); // content boundaries min for the full window
                                           // (roughly (0,0)-Scroll), in window
                                           // coordinates
CK_DLL_SFUN(ui_GetWindowContentRegionMax); // content boundaries max for the full window
                                           // (roughly (0,0)+Size-Scroll) where Size can
                                           // be overridden with
                                           // SetNextWindowContentSize(), in window
                                           // coordinates

// Windows Scrolling
// - Any change of Scroll will be applied at the beginning of next frame in the
// first call to Begin().
// - You may instead use SetNextWindowScroll() prior to calling Begin() to avoid
// this delay, as an alternative to using SetScrollX()/SetScrollY().
CK_DLL_SFUN(ui_GetScrollX);
CK_DLL_SFUN(ui_GetScrollY);
CK_DLL_SFUN(ui_SetScrollX);
CK_DLL_SFUN(ui_SetScrollY);
CK_DLL_SFUN(ui_GetScrollMaxX);
CK_DLL_SFUN(ui_GetScrollMaxY);
CK_DLL_SFUN(ui_SetScrollHereX);
CK_DLL_SFUN(ui_SetScrollHereY);
CK_DLL_SFUN(ui_SetScrollFromPosX);
CK_DLL_SFUN(ui_SetScrollFromPosY);

// Parameters stacks (shared)
// ignoring fonts for now (complicated)
// CIMGUI_API void ImGui_PushFont(ImFont* font);                             //
// use NULL as a shortcut to push default font CIMGUI_API void
// ImGui_PopFont(void);
// CK_DLL_SFUN(ui_PushStyleColor);  // ignoring in favor of vec4 version

CK_DLL_SFUN(ui_PushFont); // disabled, see AddFontFromFileTTF
CK_DLL_SFUN(ui_PopFont);  // disabled, see AddFontFromFileTTF
CK_DLL_SFUN(ui_PushStyleColorImVec4);
CK_DLL_SFUN(ui_PushStyleColorImVec3);
CK_DLL_SFUN(ui_PopStyleColor);
CK_DLL_SFUN(ui_PopStyleColorEx);
CK_DLL_SFUN(ui_PushStyleVar);
CK_DLL_SFUN(ui_PushStyleVarImVec2);
CK_DLL_SFUN(ui_PopStyleVar);
CK_DLL_SFUN(ui_PopStyleVarEx);
CK_DLL_SFUN(ui_PushTabStop);
CK_DLL_SFUN(ui_PopTabStop);
CK_DLL_SFUN(ui_PushButtonRepeat);
CK_DLL_SFUN(ui_PopButtonRepeat);

// Parameters stacks (current window)
CK_DLL_SFUN(ui_PushItemWidth);
CK_DLL_SFUN(ui_PopItemWidth);
CK_DLL_SFUN(ui_SetNextItemWidth);
CK_DLL_SFUN(ui_CalcItemWidth);
CK_DLL_SFUN(ui_PushTextWrapPos);
CK_DLL_SFUN(ui_PopTextWrapPos);

// Style read access
// - Use the ShowStyleEditor() function to interactively see/edit the colors.
// CK_DLL_SFUN(ui_GetFont); // ignoring fonts for now (complicated)
CK_DLL_SFUN(ui_GetFontSize);
CK_DLL_SFUN(ui_GetFontTexUvWhitePixel);
CK_DLL_SFUN(ui_GetColorU32);
CK_DLL_SFUN(ui_GetColorU32Ex);
CK_DLL_SFUN(ui_GetColorU32ImVec4);
// CK_DLL_SFUN(ui_GetColorU32ImU32);  // redundant with GetColorU32Ex
// CK_DLL_SFUN(ui_GetColorU32ImU32Ex); // redundant with GetColorU32Ex
CK_DLL_SFUN(ui_GetStyleColorVec4);

// Layout cursor positioning
// - By "cursor" we mean the current output position.
// - The typical widget behavior is to output themselves at the current cursor
// position, then move the cursor one line down.
// - You can call SameLine() between widgets to undo the last carriage return
// and output at the right of the preceding widget.
// - Attention! We currently have inconsistencies between window-local and
// absolute positions we will aim to fix with future API:
//    - Absolute coordinate:        GetCursorScreenPos(), SetCursorScreenPos(),
//    all ImDrawList:: functions. -> this is the preferred way forward.
//    - Window-local coordinates:   SameLine(), GetCursorPos(), SetCursorPos(),
//    GetCursorStartPos(), GetContentRegionMax(), GetWindowContentRegion*(),
//    PushTextWrapPos()
// - GetCursorScreenPos() = GetCursorPos() + GetWindowPos(). GetWindowPos() is
// almost only ever useful to convert from window-local to absolute coordinates.
CK_DLL_SFUN(ui_GetCursorScreenPos);
CK_DLL_SFUN(ui_SetCursorScreenPos);
CK_DLL_SFUN(ui_GetCursorPos);
CK_DLL_SFUN(ui_GetCursorPosX);
CK_DLL_SFUN(ui_GetCursorPosY);
CK_DLL_SFUN(ui_SetCursorPos);
CK_DLL_SFUN(ui_SetCursorPosX);
CK_DLL_SFUN(ui_SetCursorPosY);
CK_DLL_SFUN(ui_GetCursorStartPos);

// Other layout functions
CK_DLL_SFUN(ui_Separator);
CK_DLL_SFUN(ui_SameLine);
CK_DLL_SFUN(ui_SameLineEx);
CK_DLL_SFUN(ui_NewLine);
CK_DLL_SFUN(ui_Spacing);
CK_DLL_SFUN(ui_Dummy);
CK_DLL_SFUN(ui_Indent);
CK_DLL_SFUN(ui_IndentEx);
CK_DLL_SFUN(ui_Unindent);
CK_DLL_SFUN(ui_UnindentEx);
CK_DLL_SFUN(ui_BeginGroup);
CK_DLL_SFUN(ui_EndGroup);
CK_DLL_SFUN(ui_AlignTextToFramePadding);
CK_DLL_SFUN(ui_GetTextLineHeight);
CK_DLL_SFUN(ui_GetTextLineHeightWithSpacing);
CK_DLL_SFUN(ui_GetFrameHeight);
CK_DLL_SFUN(ui_GetFrameHeightWithSpacing);

// ID stack/scopes
// Read the FAQ (docs/FAQ.md or http://dearimgui.com/faq) for more details about
// how ID are handled in dear imgui.
// - Those questions are answered and impacted by understanding of the ID stack
// system:
//   - "Q: Why is my widget not reacting when I click on it?"
//   - "Q: How can I have widgets with an empty label?"
//   - "Q: How can I have multiple widgets with the same label?"
// - Short version: ID are hashes of the entire ID stack. If you are creating
// widgets in a loop you most likely
//   want to push a unique identifier (e.g. object pointer, loop index) to
//   uniquely differentiate them.
// - You can also use the "Label##foobar" syntax within widget label to
// distinguish them from each others.
// - In this header file we use the "label"/"name" terminology to denote a
// string that will be displayed + used as an ID,
//   whereas "str_id" denote a string that is only used as an ID and not
//   normally displayed.
CK_DLL_SFUN(ui_PushID);
CK_DLL_SFUN(ui_PushIDStr);
// CK_DLL_SFUN(ui_PushIDPtr); // ignore. no pointers in chuck
CK_DLL_SFUN(ui_PushIDInt);
CK_DLL_SFUN(ui_PopID);
CK_DLL_SFUN(ui_GetID);
CK_DLL_SFUN(ui_GetIDStr);
// CK_DLL_SFUN(ui_GetIDPtr);  // ignore. no pointers in chuck

// Widgets: Text (ignoring formated / va_args)
CK_DLL_SFUN(ui_TextUnformatted);
CK_DLL_SFUN(ui_TextUnformattedEx);
// CK_DLL_SFUN(ui_Text);
// CK_DLL_SFUN(ui_TextV);
// CK_DLL_SFUN(ui_TextColored);
CK_DLL_SFUN(ui_TextColoredUnformatted);
// CK_DLL_SFUN(ui_TextColoredV);
// CK_DLL_SFUN(ui_TextDisabled);
CK_DLL_SFUN(ui_TextDisabledUnformatted);
// CK_DLL_SFUN(ui_TextDisabledV);
// CK_DLL_SFUN(ui_TextWrapped);
CK_DLL_SFUN(ui_TextWrappedUnformatted);
// CK_DLL_SFUN(ui_TextWrappedV);
// CK_DLL_SFUN(ui_LabelText);
CK_DLL_SFUN(ui_LabelTextUnformatted);
// CK_DLL_SFUN(ui_LabelTextV);
// CK_DLL_SFUN(ui_BulletText);
CK_DLL_SFUN(ui_BulletTextUnformatted);
// CK_DLL_SFUN(ui_BulletTextV);
CK_DLL_SFUN(ui_SeparatorText);

// Widgets: Main
// - Most widgets return true when the value has been changed or when
// pressed/selected
// - You may also use one of the many IsItemXXX functions (e.g. IsItemActive,
// IsItemHovered, etc.) to query widget state.
CK_DLL_SFUN(ui_Button);
CK_DLL_SFUN(ui_ButtonEx);
CK_DLL_SFUN(ui_SmallButton);
CK_DLL_SFUN(ui_InvisibleButton);
CK_DLL_SFUN(ui_ArrowButton);
CK_DLL_SFUN(ui_Checkbox);
CK_DLL_SFUN(ui_CheckboxFlagsIntPtr);
// CK_DLL_SFUN(ui_CheckboxFlagsUintPtr); // redundant
CK_DLL_SFUN(ui_RadioButton);
CK_DLL_SFUN(ui_RadioButtonIntPtr);
CK_DLL_SFUN(ui_ProgressBar);
CK_DLL_SFUN(ui_Bullet);

// clang-format off
// Widgets: Images 
// - Read about ImTextureID here:
// https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
// - 'uv0' and 'uv1' are texture coordinates. Read about them from the same link 
// above.
// - Note that Image() may add +2.0f to provided size if a border is visible,
// ImageButton() adds style.FramePadding*2.0f to provided size. 

CK_DLL_SFUN(ui_Image);
CK_DLL_SFUN(ui_ImageEx);
CK_DLL_SFUN(ui_ImageButton);
CK_DLL_SFUN(ui_ImageButtonEx);
// clang-format on

// Widgets: Combo Box (Dropdown)
// - The BeginCombo()/EndCombo() api allows you to manage your contents and
// selection state however you want it, by creating e.g. Selectable() items.
// - The old Combo() api are helpers over BeginCombo()/EndCombo() which are kept
// available for convenience purpose. This is analogous to how ListBox are
// created.
CK_DLL_SFUN(ui_BeginCombo);
CK_DLL_SFUN(ui_EndCombo);
CK_DLL_SFUN(ui_ComboChar);
CK_DLL_SFUN(ui_ComboCharEx);
CK_DLL_SFUN(ui_Combo);   // TODO: doesn't work with chuck strings
CK_DLL_SFUN(ui_ComboEx); // TODO: doesn't work with chuck strings
// ignoring callback versions. seems redundant and more complicated
// CK_DLL_SFUN(ui_ComboCallback);
// CK_DLL_SFUN(ui_ComboCallbackEx);

// Widgets: Drag Sliders
CK_DLL_SFUN(ui_DragFloat);
CK_DLL_SFUN(ui_DragFloatEx);
CK_DLL_SFUN(ui_DragFloat2);
CK_DLL_SFUN(ui_DragFloat2Ex);
CK_DLL_SFUN(ui_DragFloat2Speed);
CK_DLL_SFUN(ui_DragFloat3);
CK_DLL_SFUN(ui_DragFloat3Ex);
CK_DLL_SFUN(ui_DragFloat3Speed);
CK_DLL_SFUN(ui_DragFloat4);
CK_DLL_SFUN(ui_DragFloat4Ex);
CK_DLL_SFUN(ui_DragFloat4Speed);
CK_DLL_SFUN(ui_DragFloatRange2);
CK_DLL_SFUN(ui_DragFloatRange2Ex);
CK_DLL_SFUN(ui_DragInt);
CK_DLL_SFUN(ui_DragIntEx);
CK_DLL_SFUN(ui_DragInt2);
CK_DLL_SFUN(ui_DragInt2Ex);
CK_DLL_SFUN(ui_DragInt3);
CK_DLL_SFUN(ui_DragInt3Ex);
CK_DLL_SFUN(ui_DragInt4);
CK_DLL_SFUN(ui_DragInt4Ex);
CK_DLL_SFUN(ui_DragIntRange2);
CK_DLL_SFUN(ui_DragIntRange2Ex);
// CK_DLL_SFUN(ui_DragScalar); // ignore, chuck doesn't have scalar types like
// U8, S16, etc. CK_DLL_SFUN(ui_DragScalarEx); // ignore, same reason
CK_DLL_SFUN(ui_DragScalarN_CKINT);
CK_DLL_SFUN(ui_DragScalarNEx_CKINT);
CK_DLL_SFUN(ui_DragScalarN_CKFLOAT);
CK_DLL_SFUN(ui_DragScalarNEx_CKFLOAT);

// Widgets: Regular Sliders
CK_DLL_SFUN(ui_SliderFloat);
CK_DLL_SFUN(ui_SliderFloatEx);
CK_DLL_SFUN(ui_SliderAngle);
CK_DLL_SFUN(ui_SliderAngleBounds);
CK_DLL_SFUN(ui_SliderAngleEx);
CK_DLL_SFUN(ui_SliderInt);
CK_DLL_SFUN(ui_SliderIntEx);
CK_DLL_SFUN(ui_SliderScalarN_CKINT);
CK_DLL_SFUN(ui_SliderScalarNEx_CKINT);
CK_DLL_SFUN(ui_SliderScalarN_CKFLOAT);
CK_DLL_SFUN(ui_SliderScalarNEx_CKFLOAT);
CK_DLL_SFUN(ui_VSliderFloat);
CK_DLL_SFUN(ui_VSliderFloatEx);
CK_DLL_SFUN(ui_VSliderInt);
CK_DLL_SFUN(ui_VSliderIntEx);

// Widgets: Input with Keyboard
// - If you want to use InputText() with std::string or any custom dynamic
// string type, see misc/cpp/imgui_stdlib.h and comments in imgui_demo.cpp.
// - Most of the ImGuiInputTextFlags flags are only useful for InputText() and
// not for InputFloatX, InputIntX, InputDouble etc.
CK_DLL_SFUN(ui_InputText);
CK_DLL_SFUN(ui_InputTextEx);
CK_DLL_SFUN(ui_InputTextMultiline);
CK_DLL_SFUN(ui_InputTextMultilineEx);
CK_DLL_SFUN(ui_InputTextWithHint);
CK_DLL_SFUN(ui_InputTextWithHintEx);
CK_DLL_SFUN(ui_InputFloat);
CK_DLL_SFUN(ui_InputFloatEx);
CK_DLL_SFUN(ui_InputInt);
CK_DLL_SFUN(ui_InputIntEx);
CK_DLL_SFUN(ui_InputScalarN_CKINT);
CK_DLL_SFUN(ui_InputScalarNEx_CKINT);
CK_DLL_SFUN(ui_InputScalarN_CKFLOAT);
CK_DLL_SFUN(ui_InputScalarNEx_CKFLOAT);

// Widgets: Color Editor/Picker
CK_DLL_SFUN(ui_ColorEdit3);
CK_DLL_SFUN(ui_ColorEdit3_NoFlags);
CK_DLL_SFUN(ui_ColorEdit4);
CK_DLL_SFUN(ui_ColorEdit4_NoFlags);
CK_DLL_SFUN(ui_ColorPicker3);
CK_DLL_SFUN(ui_ColorPicker4);
CK_DLL_SFUN(ui_ColorPicker4_no_ref_col);
CK_DLL_SFUN(ui_ColorButton);
CK_DLL_SFUN(ui_ColorButtonEx);
CK_DLL_SFUN(ui_SetColorEditOptions);

// Widgets: Trees
// - TreeNode functions return true when the node is open, in which case you
// need to also call TreePop() when you are finished displaying the tree node
// contents.
CK_DLL_SFUN(ui_TreeNode);
CK_DLL_SFUN(ui_TreeNodeStrUnformatted);
CK_DLL_SFUN(ui_TreeNodeEx);
CK_DLL_SFUN(ui_TreeNodeExStrUnformatted);
CK_DLL_SFUN(ui_TreePush);
CK_DLL_SFUN(ui_TreePop);
CK_DLL_SFUN(ui_GetTreeNodeToLabelSpacing);
CK_DLL_SFUN(ui_CollapsingHeader);
CK_DLL_SFUN(ui_CollapsingHeaderBoolPtr);
CK_DLL_SFUN(ui_SetNextItemOpen);

// Widgets: Selectables
// - A selectable highlights when hovered, and can display another color when
// selected.
// - Neighbors selectable extend their highlight bounds in order to leave no gap
// between them. This is so a series of selected Selectable appear contiguous.
CK_DLL_SFUN(ui_Selectable);
CK_DLL_SFUN(ui_SelectableEx);
CK_DLL_SFUN(ui_SelectableBoolPtr);
CK_DLL_SFUN(ui_SelectableBoolPtrEx);

// Widgets: List Boxes
CK_DLL_SFUN(ui_BeginListBox);
CK_DLL_SFUN(ui_EndListBox);
CK_DLL_SFUN(ui_ListBox);
CK_DLL_SFUN(ui_ListBox_default);

// Widgets: Data Plotting
// - Consider using ImPlot (https://github.com/epezent/implot) which is much
// better!
// TODO: implement callback versions
CK_DLL_SFUN(ui_PlotLines);
CK_DLL_SFUN(ui_PlotLinesEx);
CK_DLL_SFUN(ui_PlotHistogram);
CK_DLL_SFUN(ui_PlotHistogramEx);

// Widgets: Menus
// - Use BeginMenuBar() on a window ImGuiWindowFlags_MenuBar to append to its
// menu bar.
// - Use BeginMainMenuBar() to create a menu bar at the top of the screen and
// append to it.
// - Use BeginMenu() to create a menu. You can call BeginMenu() multiple time
// with the same identifier to append more items to it.
// - Not that MenuItem() keyboardshortcuts are displayed as a convenience but
// _not processed_ by Dear ImGui at the moment.
CK_DLL_SFUN(ui_BeginMenuBar);
CK_DLL_SFUN(ui_EndMenuBar);
CK_DLL_SFUN(ui_BeginMainMenuBar);
CK_DLL_SFUN(ui_EndMainMenuBar);
CK_DLL_SFUN(ui_BeginMenu);
CK_DLL_SFUN(ui_BeginMenuEx);
CK_DLL_SFUN(ui_EndMenu);
CK_DLL_SFUN(ui_MenuItem);
CK_DLL_SFUN(ui_MenuItemBoolPtr);

// Widgets: Knobs
// Ported from https://github.com/altschuler/imgui-knobs/tree/main
CK_DLL_SFUN(ui_Knob);
CK_DLL_SFUN(ui_KnobInt);
CK_DLL_SFUN(ui_KnobEx);
CK_DLL_SFUN(ui_KnobIntEx);

// Tooltips
CK_DLL_SFUN(ui_BeginItemTooltip);
CK_DLL_SFUN(ui_BeginTooltip);
CK_DLL_SFUN(ui_EndTooltip);
CK_DLL_SFUN(ui_SetTooltipUnformatted);
CK_DLL_SFUN(ui_SetItemTooltipUnformatted);

// Popups, Modals
CK_DLL_SFUN(ui_BeginPopup);
CK_DLL_SFUN(ui_BeginPopupModal);
CK_DLL_SFUN(ui_EndPopup);
CK_DLL_SFUN(ui_OpenPopup);
// CK_DLL_SFUN(ui_OpenPopupID);
CK_DLL_SFUN(ui_OpenPopupOnItemClick);
CK_DLL_SFUN(ui_CloseCurrentPopup);
CK_DLL_SFUN(ui_BeginPopupContextItem);
CK_DLL_SFUN(ui_BeginPopupContextItemEx);
CK_DLL_SFUN(ui_BeginPopupContextWindow);
CK_DLL_SFUN(ui_BeginPopupContextWindowEx);
CK_DLL_SFUN(ui_BeginPopupContextVoid);
CK_DLL_SFUN(ui_BeginPopupContextVoidEx);
CK_DLL_SFUN(ui_IsPopupOpen);

// Tables
CK_DLL_SFUN(ui_BeginTable);
CK_DLL_SFUN(ui_BeginTableEx);
CK_DLL_SFUN(ui_EndTable);
CK_DLL_SFUN(ui_TableNextRow);
CK_DLL_SFUN(ui_TableNextRowEx);
CK_DLL_SFUN(ui_TableNextColumn);
CK_DLL_SFUN(ui_TableSetColumnIndex);

CK_DLL_SFUN(ui_TableSetupColumn);
CK_DLL_SFUN(ui_TableSetupColumnEx);
CK_DLL_SFUN(ui_TableSetupScrollFreeze);
CK_DLL_SFUN(ui_TableHeader);
CK_DLL_SFUN(ui_TableHeadersRow);
CK_DLL_SFUN(ui_TableAngledHeadersRow);

// CK_DLL_SFUN(ui_TableGetSortSpecs);
CK_DLL_SFUN(ui_TableGetColumnCount);
CK_DLL_SFUN(ui_TableGetColumnIndex);
CK_DLL_SFUN(ui_TableGetRowIndex);
CK_DLL_SFUN(ui_TableGetColumnName);
CK_DLL_SFUN(ui_TableGetColumnFlags);
CK_DLL_SFUN(ui_TableSetColumnEnabled);
CK_DLL_SFUN(ui_TableSetBgColor);

// Tab Bars, Tabs
// - Note: Tabs are automatically created by the docking system (when in
// 'docking' branch). Use this to create tab bars/tabs yourself.
CK_DLL_SFUN(ui_BeginTabBar);
CK_DLL_SFUN(ui_EndTabBar);
CK_DLL_SFUN(ui_BeginTabItem);
CK_DLL_SFUN(ui_EndTabItem);
CK_DLL_SFUN(ui_TabItemButton);
CK_DLL_SFUN(ui_SetTabItemClosed);

// Docking
// CK_DLL_SFUN(ui_DockSpace);
// CK_DLL_SFUN(ui_DockSpaceEx);
CK_DLL_SFUN(ui_DockSpaceOverViewport);
// CK_DLL_SFUN(ui_DockSpaceOverViewportEx);
// CK_DLL_SFUN(ui_SetNextWindowDockID);
// CK_DLL_SFUN(ui_SetNextWindowClass);
// CK_DLL_SFUN(ui_GetWindowDockID);
// CK_DLL_SFUN(ui_IsWindowDocked);

// Logging/Capture TODO
// - All text output from the interface can be captured into tty/file/clipboard.
// By default, tree nodes are automatically opened during logging. CIMGUI_API
// void ImGui_LogToTTY(int auto_open_depth /* = -1 */); // start logging to tty
// (stdout) CIMGUI_API void ImGui_LogToFile(int auto_open_depth /* = -1 */,
// const char* filename /* = NULL */);  // start logging to file CIMGUI_API void
// ImGui_LogToClipboard(int auto_open_depth /* = -1 */); // start logging to OS
// clipboard CIMGUI_API void ImGui_LogFinish(void); // stop logging (close file,
// etc.) CIMGUI_API void ImGui_LogButtons(void); // helper to display buttons
// for logging to tty/file/clipboard CIMGUI_API void ImGui_LogText(const char*
// fmt, ...) IM_FMTARGS(1);                                   // pass text data
// straight to log (without being displayed) CIMGUI_API void
// ImGui_LogTextUnformatted(const char* text); // pass text data straight to log
// (without being displayed) CIMGUI_API void ImGui_LogTextV(const char* fmt,
// va_list args) IM_FMTLIST(1);

// Drag and Drop TODO
// CIMGUI_API bool                ImGui_BeginDragDropSource(ImGuiDragDropFlags
// flags /* = 0 */);                      // call after submitting an item which
// may be dragged. when this return true, you can call SetDragDropPayload() +
// EndDragDropSource() CIMGUI_API bool ImGui_SetDragDropPayload(const char*
// type, const void* data, size_t sz, ImGuiCond cond /* = 0 */); // type is a
// user defined string of maximum 32 characters. Strings starting with '_' are
// reserved for dear imgui internal types. Data is copied and held by imgui.
// Return true when payload has been accepted. CIMGUI_API void
// ImGui_EndDragDropSource(void); // only call EndDragDropSource() if
// BeginDragDropSource() returns true! CIMGUI_API bool
// ImGui_BeginDragDropTarget(void); // call after submitting an item that may
// receive a payload. If this returns true, you can call AcceptDragDropPayload()
// + EndDragDropTarget() CIMGUI_API const ImGuiPayload*
// ImGui_AcceptDragDropPayload(const char* type, ImGuiDragDropFlags flags /* = 0
// */);  // accept contents of a given type. If
// ImGuiDragDropFlags_AcceptBeforeDelivery is set you can peek into the payload
// before the mouse button is released. CIMGUI_API void
// ImGui_EndDragDropTarget(void); // only call EndDragDropTarget() if
// BeginDragDropTarget() returns true! CIMGUI_API const ImGuiPayload*
// ImGui_GetDragDropPayload(void); // peek directly into the current payload
// from anywhere. returns NULL when drag and drop is finished or inactive. use
// ImGuiPayload::IsDataType() to test for the payload type.

// Disabling [BETA API]
CK_DLL_SFUN(ui_BeginDisabled_true);
CK_DLL_SFUN(ui_BeginDisabled);
CK_DLL_SFUN(ui_EndDisabled);

// Clipping
CK_DLL_SFUN(ui_PushClipRect);
CK_DLL_SFUN(ui_PopClipRect);

// Focus, Activation
CK_DLL_SFUN(ui_SetItemDefaultFocus);
CK_DLL_SFUN(ui_SetKeyboardFocusHere);
CK_DLL_SFUN(ui_SetKeyboardFocusHereEx);

// Overlapping mode
CK_DLL_SFUN(ui_SetNextItemAllowOverlap);

// Item/Widgets Utilities and Query Functions
// - Most of the functions are referring to the previous Item that has been
// submitted.
// - See Demo Window under "Widgets->Querying Status" for an interactive
// visualization of most of those functions.
CK_DLL_SFUN(ui_IsItemHovered);
CK_DLL_SFUN(ui_IsItemActive);
CK_DLL_SFUN(ui_IsItemFocused);
CK_DLL_SFUN(ui_IsItemClicked);
CK_DLL_SFUN(ui_IsItemClickedEx);
CK_DLL_SFUN(ui_IsItemVisible);
CK_DLL_SFUN(ui_IsItemEdited);
CK_DLL_SFUN(ui_IsItemActivated);
CK_DLL_SFUN(ui_IsItemDeactivated);
CK_DLL_SFUN(ui_IsItemDeactivatedAfterEdit);
CK_DLL_SFUN(ui_IsItemToggledOpen);
CK_DLL_SFUN(ui_IsAnyItemHovered);
CK_DLL_SFUN(ui_IsAnyItemActive);
CK_DLL_SFUN(ui_IsAnyItemFocused);
CK_DLL_SFUN(ui_GetItemID);
CK_DLL_SFUN(ui_GetItemRectMin);
CK_DLL_SFUN(ui_GetItemRectMax);
CK_DLL_SFUN(ui_GetItemRectSize);

// Viewports
CK_DLL_SFUN(ui_GetMainViewport);

// Text Utilities
CK_DLL_SFUN(ui_CalcTextSize);
CK_DLL_SFUN(ui_CalcTextSizeEx);

// Color Utilities
CK_DLL_SFUN(ui_ColorConvertRGBtoHSV);
CK_DLL_SFUN(ui_ColorConvertHSVtoRGB);

// Inputs Utilities: Keyboard/Mouse/Gamepad
// - the ImGuiKey enum contains all possible keyboard, mouse and gamepad inputs
// (e.g. ImGuiKey_A, ImGuiKey_MouseLeft, ImGuiKey_GamepadDpadUp...).
// - before v1.87, we used ImGuiKey to carry native/user indices as defined by
// each backends. About use of those legacy ImGuiKey values:
//  - without IMGUI_DISABLE_OBSOLETE_KEYIO (legacy support): you can still use
//  your legacy native/user indices (< 512) according to how your backend/engine
//  stored them in io.KeysDown[], but need to cast them to ImGuiKey.
//  - with    IMGUI_DISABLE_OBSOLETE_KEYIO (this is the way forward): any use of
//  ImGuiKey will assert with key < 512. GetKeyIndex() is pass-through and
//  therefore deprecated (gone if IMGUI_DISABLE_OBSOLETE_KEYIO is defined).
CK_DLL_SFUN(ui_IsKeyDown);
CK_DLL_SFUN(ui_IsKeyPressed);
CK_DLL_SFUN(ui_IsKeyPressedEx);
CK_DLL_SFUN(ui_IsKeyReleased);
CK_DLL_SFUN(ui_IsKeyChordPressed);
CK_DLL_SFUN(ui_GetKeyPressedAmount);
CK_DLL_SFUN(ui_GetKeyName);
CK_DLL_SFUN(ui_SetNextFrameWantCaptureKeyboard);

// Inputs Utilities: Mouse specific
// - To refer to a mouse button, you may use named enums in your code e.g.
// ImGuiMouseButton_Left, ImGuiMouseButton_Right.
// - You can also use regular integer: it is forever guaranteed that 0=Left,
// 1=Right, 2=Middle.
// - Dragging operations are only reported after mouse has moved a certain
// distance away from the initial clicking position (see 'lock_threshold' and
// 'io.MouseDraggingThreshold')
CK_DLL_SFUN(ui_IsMouseDown);
CK_DLL_SFUN(ui_IsMouseClicked);
CK_DLL_SFUN(ui_IsMouseClickedEx);
CK_DLL_SFUN(ui_IsMouseReleased);
CK_DLL_SFUN(ui_IsMouseDoubleClicked);
CK_DLL_SFUN(ui_GetMouseClickedCount);
CK_DLL_SFUN(ui_IsMouseHoveringRect);
CK_DLL_SFUN(ui_IsMouseHoveringRectEx);
CK_DLL_SFUN(ui_IsMousePosValid);
CK_DLL_SFUN(ui_GetMousePos);
CK_DLL_SFUN(ui_GetMousePosOnOpeningCurrentPopup);
CK_DLL_SFUN(ui_IsMouseDragging);
CK_DLL_SFUN(ui_GetMouseDragDelta);
CK_DLL_SFUN(ui_ResetMouseDragDelta);
CK_DLL_SFUN(ui_ResetMouseDragDeltaEx);
CK_DLL_SFUN(ui_GetMouseCursor);
CK_DLL_SFUN(ui_SetMouseCursor);
CK_DLL_SFUN(ui_SetNextFrameWantCaptureMouse);

// Clipboard Utilities
// - Also see the LogToClipboard() function to capture GUI into clipboard, or
// easily output text data to the clipboard.
CK_DLL_SFUN(ui_GetClipboardText);
CK_DLL_SFUN(ui_SetClipboardText);

// Styles
CK_DLL_SFUN(ui_styleColorsDark);
CK_DLL_SFUN(ui_styleColorsLight);
CK_DLL_SFUN(ui_styleColorsClassic);

// Callbacks -----------------------------------------------------------------

// base callback
CK_DLL_CTOR(ui_callback_ctor);
CK_DLL_DTOR(ui_callback_dtor);

// ImGuiSizeCallback
CK_DLL_MFUN(ui_size_callback);
static t_CKINT ui_size_callback_vt_offset = -1;
// ImGuiSizeCallbackData
static t_CKUINT ui_size_callback_data_ptr_offset = 0;
CK_DLL_MFUN(ui_size_callback_data_get_pos);
CK_DLL_MFUN(ui_size_callback_data_get_current_size);
CK_DLL_MFUN(ui_size_callback_data_get_desired_size);
CK_DLL_MFUN(ui_size_callback_data_set_desired_size);

// ComboCallback
CK_DLL_MFUN(ui_combo_callback);
static t_CKINT ui_combo_callback_vt_offset = -1;

// InputTextCallback
// CK_DLL_MFUN(ui_input_text_callback);
// static t_CKINT ui_input_text_callback_vt_offset = -1;
// InputTextCallbackData (complicated, ignoring for now)
// static t_CKUINT ui_input_text_callback_data_ptr_offset = 0;
// CK_DLL_MFUN(ui_input_text_callback_data_get_event_flag);  //
// ImguiInputTextFlags read-only
// CK_DLL_MFUN(ui_input_text_callback_data_get_flags);       //
// ImGuiInputTextFlags read-only
// CK_DLL_MFUN(ui_input_text_callback_data_get_event_char);  // ImWchar
// read-write CK_DLL_MFUN(ui_input_text_callback_data_set_event_char);
// CK_DLL_MFUN(ui_input_text_callback_data_get_event_key);   // ImGuiKey
// read-only CK_DLL_MFUN(ui_input_text_callback_data_get_buf);         //
// UI_String read-write

// UI_Bool
static t_CKUINT ui_bool_val_offset = 0;
CK_DLL_CTOR(ui_bool_ctor);
CK_DLL_CTOR(ui_bool_ctor_val);
CK_DLL_MFUN(ui_bool_get_value);
CK_DLL_MFUN(ui_bool_set_value);

// UI_String
static t_CKUINT ui_string_ptr_offset = 0;
static t_CKUINT ui_string_cap_offset = 0;
CK_DLL_CTOR(ui_string_ctor);
CK_DLL_CTOR(ui_string_ctor_str);
CK_DLL_DTOR(ui_string_dtor);
CK_DLL_MFUN(ui_string_get_value);
CK_DLL_MFUN(ui_string_set_value);

// UI_Int
static t_CKUINT ui_int_ptr_offset = 0;
CK_DLL_CTOR(ui_int_ctor);
CK_DLL_CTOR(ui_int_ctor_int);
CK_DLL_DTOR(ui_int_dtor);
CK_DLL_MFUN(ui_int_get_value);
CK_DLL_MFUN(ui_int_set_value);

// UI_Int2
static t_CKUINT ui_int2_ptr_offset = 0;
CK_DLL_CTOR(ui_int2_ctor);
CK_DLL_DTOR(ui_int2_dtor);
CK_DLL_MFUN(ui_int2_get_x);
CK_DLL_MFUN(ui_int2_get_y);
CK_DLL_MFUN(ui_int2_set);

// UI_Int3
static t_CKUINT ui_int3_ptr_offset = 0;
CK_DLL_CTOR(ui_int3_ctor);
CK_DLL_DTOR(ui_int3_dtor);
CK_DLL_MFUN(ui_int3_get_x);
CK_DLL_MFUN(ui_int3_get_y);
CK_DLL_MFUN(ui_int3_get_z);
CK_DLL_MFUN(ui_int3_set);

// UI_Int4
static t_CKUINT ui_int4_ptr_offset = 0;
CK_DLL_CTOR(ui_int4_ctor);
CK_DLL_DTOR(ui_int4_dtor);
CK_DLL_MFUN(ui_int4_get_x);
CK_DLL_MFUN(ui_int4_get_y);
CK_DLL_MFUN(ui_int4_get_z);
CK_DLL_MFUN(ui_int4_get_w);
CK_DLL_MFUN(ui_int4_set);

// UI_Float
static t_CKUINT ui_float_ptr_offset = 0;
CK_DLL_CTOR(ui_float_ctor);
CK_DLL_CTOR(ui_float_ctor_float);
CK_DLL_DTOR(ui_float_dtor);
CK_DLL_MFUN(ui_float_get_value);
CK_DLL_MFUN(ui_float_set_value);

// UI_Float2
static t_CKUINT ui_float2_ptr_offset = 0;
CK_DLL_CTOR(ui_float2_ctor);
CK_DLL_CTOR(ui_float2_ctor_with_value);
CK_DLL_CTOR(ui_float2_ctor_with_floats);
CK_DLL_DTOR(ui_float2_dtor);
CK_DLL_MFUN(ui_float2_get_value);
CK_DLL_MFUN(ui_float2_set_value);

// UI_Float3
static t_CKUINT ui_float3_ptr_offset = 0;
CK_DLL_CTOR(ui_float3_ctor);
CK_DLL_CTOR(ui_float3_ctor_with_value);
CK_DLL_CTOR(ui_float3_ctor_with_floats);
CK_DLL_DTOR(ui_float3_dtor);
CK_DLL_MFUN(ui_float3_get_value);
CK_DLL_MFUN(ui_float3_set_value);

// UI_Float4
static t_CKUINT ui_float4_ptr_offset = 0;
CK_DLL_CTOR(ui_float4_ctor);
CK_DLL_CTOR(ui_float4_ctor_with_value);
CK_DLL_CTOR(ui_float4_ctor_with_floats);
CK_DLL_DTOR(ui_float4_dtor);
CK_DLL_MFUN(ui_float4_get_value);
CK_DLL_MFUN(ui_float4_set_value);

// UI_Font
static t_CKUINT ui_font_ptr_offset = 0;
#define GET_NEXT_UI_FONT(ARGS)                                                         \
    (ImFont*)OBJ_MEMBER_UINT(GET_NEXT_OBJECT(ARGS), ui_font_ptr_offset)

// UI_Viewport
static t_CKUINT ui_viewport_ptr_offset = 0;
CK_DLL_MFUN(ui_viewport_get_id);
CK_DLL_MFUN(ui_viewport_get_flags);
CK_DLL_MFUN(ui_viewport_get_pos);
CK_DLL_MFUN(ui_viewport_get_size);
CK_DLL_MFUN(ui_viewport_get_work_pos);
CK_DLL_MFUN(ui_viewport_get_work_size);
CK_DLL_MFUN(ui_viewport_get_dpi_scale);
CK_DLL_MFUN(ui_viewport_get_parent_viewport_id);
// CK_DLL_MFUN(ui_viewport_get_draw_data); TODO drawdata
CK_DLL_MFUN(ui_viewport_get_center);
CK_DLL_MFUN(ui_viewport_get_work_center);

// UI_Style
static t_CKUINT ui_style_ptr_offset = 0;
CK_DLL_MFUN(ui_style_get_alpha);
CK_DLL_MFUN(ui_style_get_disabled_alpha);
CK_DLL_MFUN(ui_style_get_window_padding);
CK_DLL_MFUN(ui_style_get_window_rounding);
CK_DLL_MFUN(ui_style_get_window_border_size);
CK_DLL_MFUN(ui_style_get_window_min_size);
CK_DLL_MFUN(ui_style_get_window_title_align);
CK_DLL_MFUN(ui_style_get_window_menu_button_position);
CK_DLL_MFUN(ui_style_get_child_rounding);
CK_DLL_MFUN(ui_style_get_child_border_size);
CK_DLL_MFUN(ui_style_get_popup_rounding);
CK_DLL_MFUN(ui_style_get_popup_border_size);
CK_DLL_MFUN(ui_style_get_frame_padding);
CK_DLL_MFUN(ui_style_get_frame_rounding);
CK_DLL_MFUN(ui_style_get_frame_border_size);
CK_DLL_MFUN(ui_style_get_item_spacing);
CK_DLL_MFUN(ui_style_get_item_inner_spacing);
CK_DLL_MFUN(ui_style_get_cell_padding);
CK_DLL_MFUN(ui_style_get_touch_extra_padding);
CK_DLL_MFUN(ui_style_get_indent_spacing);
CK_DLL_MFUN(ui_style_get_columns_min_spacing);
CK_DLL_MFUN(ui_style_get_scrollbar_size);
CK_DLL_MFUN(ui_style_get_scrollbar_rounding);
CK_DLL_MFUN(ui_style_get_grab_min_size);
CK_DLL_MFUN(ui_style_get_grab_rounding);
CK_DLL_MFUN(ui_style_get_log_slider_deadzone);
CK_DLL_MFUN(ui_style_get_tab_rounding);
CK_DLL_MFUN(ui_style_get_tab_border_size);
CK_DLL_MFUN(ui_style_get_tab_min_width_for_close_button);
CK_DLL_MFUN(ui_style_get_tab_bar_border_size);
CK_DLL_MFUN(ui_style_get_table_angled_headers_angle);
CK_DLL_MFUN(ui_style_get_table_angled_headers_text_align);
CK_DLL_MFUN(ui_style_get_color_button_position);
CK_DLL_MFUN(ui_style_get_button_text_align);
CK_DLL_MFUN(ui_style_get_selectable_text_align);
CK_DLL_MFUN(ui_style_get_separator_text_border_size);
CK_DLL_MFUN(ui_style_get_separator_text_align);
CK_DLL_MFUN(ui_style_get_separator_text_padding);
CK_DLL_MFUN(ui_style_get_display_window_padding);
CK_DLL_MFUN(ui_style_get_display_safe_area_padding);
CK_DLL_MFUN(ui_style_get_docking_separator_size);
CK_DLL_MFUN(ui_style_get_mouse_cursor_scale);
CK_DLL_MFUN(ui_style_get_anti_aliased_lines);
CK_DLL_MFUN(ui_style_get_anti_aliased_lines_use_tex);
CK_DLL_MFUN(ui_style_get_anti_aliased_fill);
CK_DLL_MFUN(ui_style_get_curve_tessellation_tol);
CK_DLL_MFUN(ui_style_get_circle_tessellation_max_error);
CK_DLL_MFUN(ui_style_get_color);
CK_DLL_MFUN(ui_style_get_hover_stationary_delay);
CK_DLL_MFUN(ui_style_get_hover_delay_short);
CK_DLL_MFUN(ui_style_get_hover_delay_normal);
CK_DLL_MFUN(ui_style_get_hover_flags_for_tooltip_mouse);
CK_DLL_MFUN(ui_style_get_hover_flags_for_tooltip_nav);

CK_DLL_MFUN(ui_style_set_alpha);
CK_DLL_MFUN(ui_style_set_disabled_alpha);
CK_DLL_MFUN(ui_style_set_window_padding);
CK_DLL_MFUN(ui_style_set_window_rounding);
CK_DLL_MFUN(ui_style_set_window_border_size);
CK_DLL_MFUN(ui_style_set_window_min_size);
CK_DLL_MFUN(ui_style_set_window_title_align);
CK_DLL_MFUN(ui_style_set_window_menu_button_position);
CK_DLL_MFUN(ui_style_set_child_rounding);
CK_DLL_MFUN(ui_style_set_child_border_size);
CK_DLL_MFUN(ui_style_set_popup_rounding);
CK_DLL_MFUN(ui_style_set_popup_border_size);
CK_DLL_MFUN(ui_style_set_frame_padding);
CK_DLL_MFUN(ui_style_set_frame_rounding);
CK_DLL_MFUN(ui_style_set_frame_border_size);
CK_DLL_MFUN(ui_style_set_item_spacing);
CK_DLL_MFUN(ui_style_set_item_inner_spacing);
CK_DLL_MFUN(ui_style_set_cell_padding);
CK_DLL_MFUN(ui_style_set_touch_extra_padding);
CK_DLL_MFUN(ui_style_set_indent_spacing);
CK_DLL_MFUN(ui_style_set_columns_min_spacing);
CK_DLL_MFUN(ui_style_set_scrollbar_size);
CK_DLL_MFUN(ui_style_set_scrollbar_rounding);
CK_DLL_MFUN(ui_style_set_grab_min_size);
CK_DLL_MFUN(ui_style_set_grab_rounding);
CK_DLL_MFUN(ui_style_set_log_slider_deadzone);
CK_DLL_MFUN(ui_style_set_tab_rounding);
CK_DLL_MFUN(ui_style_set_tab_border_size);
CK_DLL_MFUN(ui_style_set_tab_min_width_for_close_button);
CK_DLL_MFUN(ui_style_set_tab_bar_border_size);
CK_DLL_MFUN(ui_style_set_table_angled_headers_angle);
CK_DLL_MFUN(ui_style_set_table_angled_headers_text_align);
CK_DLL_MFUN(ui_style_set_color_button_position);
CK_DLL_MFUN(ui_style_set_button_text_align);
CK_DLL_MFUN(ui_style_set_selectable_text_align);
CK_DLL_MFUN(ui_style_set_separator_text_border_size);
CK_DLL_MFUN(ui_style_set_separator_text_align);
CK_DLL_MFUN(ui_style_set_separator_text_padding);
CK_DLL_MFUN(ui_style_set_display_window_padding);
CK_DLL_MFUN(ui_style_set_display_safe_area_padding);
CK_DLL_MFUN(ui_style_set_docking_separator_size);
CK_DLL_MFUN(ui_style_set_mouse_cursor_scale);
CK_DLL_MFUN(ui_style_set_anti_aliased_lines);
CK_DLL_MFUN(ui_style_set_anti_aliased_lines_use_tex);
CK_DLL_MFUN(ui_style_set_anti_aliased_fill);
CK_DLL_MFUN(ui_style_set_curve_tessellation_tol);
CK_DLL_MFUN(ui_style_set_circle_tessellation_max_error);
CK_DLL_MFUN(ui_style_set_color);
CK_DLL_MFUN(ui_style_set_hover_stationary_delay);
CK_DLL_MFUN(ui_style_set_hover_delay_short);
CK_DLL_MFUN(ui_style_set_hover_delay_normal);
CK_DLL_MFUN(ui_style_set_hover_flags_for_tooltip_mouse);
CK_DLL_MFUN(ui_style_set_hover_flags_for_tooltip_nav);

// UI_DrawList
// always defaulting to imgui.igGetWindowDrawList() for now
CK_DLL_SFUN(ui_DrawList_PushClipRect);
CK_DLL_SFUN(ui_DrawList_PushClipRectFullScreen);
CK_DLL_SFUN(ui_DrawList_PopClipRect);
// CK_DLL_SFUN(ui_DrawList_PushTextureID);
// CK_DLL_SFUN(ui_DrawList_PopTextureID);
CK_DLL_SFUN(ui_DrawList_GetClipRectMin);
CK_DLL_SFUN(ui_DrawList_GetClipRectMax);
CK_DLL_SFUN(ui_DrawList_AddLine);
CK_DLL_SFUN(ui_DrawList_AddLineEx);
CK_DLL_SFUN(ui_DrawList_AddRect);
CK_DLL_SFUN(ui_DrawList_AddRectEx);
CK_DLL_SFUN(ui_DrawList_AddRectFilled);
CK_DLL_SFUN(ui_DrawList_AddRectFilledEx);
CK_DLL_SFUN(ui_DrawList_AddRectFilledMultiColor);
CK_DLL_SFUN(ui_DrawList_AddQuad);
CK_DLL_SFUN(ui_DrawList_AddQuadEx);
CK_DLL_SFUN(ui_DrawList_AddQuadFilled);
CK_DLL_SFUN(ui_DrawList_AddTriangle);
CK_DLL_SFUN(ui_DrawList_AddTriangleEx);
CK_DLL_SFUN(ui_DrawList_AddTriangleFilled);
CK_DLL_SFUN(ui_DrawList_AddCircle);
CK_DLL_SFUN(ui_DrawList_AddCircleEx);
CK_DLL_SFUN(ui_DrawList_AddCircleFilled);
CK_DLL_SFUN(ui_DrawList_AddNgon);
CK_DLL_SFUN(ui_DrawList_AddNgonEx);
CK_DLL_SFUN(ui_DrawList_AddNgonFilled);
CK_DLL_SFUN(ui_DrawList_AddEllipse);
CK_DLL_SFUN(ui_DrawList_AddEllipseEx);
CK_DLL_SFUN(ui_DrawList_AddEllipseFilled);
CK_DLL_SFUN(ui_DrawList_AddEllipseFilledEx);
CK_DLL_SFUN(ui_DrawList_AddText);
CK_DLL_SFUN(ui_DrawList_AddTextEx);
// CK_DLL_SFUN(ui_DrawList_AddTextImFontPtr);
// CK_DLL_SFUN(ui_DrawList_AddTextImFontPtrEx);
CK_DLL_SFUN(ui_DrawList_AddBezierCubic);
CK_DLL_SFUN(ui_DrawList_AddBezierQuadratic);
CK_DLL_SFUN(ui_DrawList_AddPolyline);
CK_DLL_SFUN(ui_DrawList_AddConvexPolyFilled);
CK_DLL_SFUN(ui_DrawList_AddConcavePolyFilled);
CK_DLL_SFUN(ui_DrawList_PathClear);
CK_DLL_SFUN(ui_DrawList_PathLineTo);
CK_DLL_SFUN(ui_DrawList_PathLineToMergeDuplicate);
CK_DLL_SFUN(ui_DrawList_PathFillConvex);
CK_DLL_SFUN(ui_DrawList_PathFillConcave);
CK_DLL_SFUN(ui_DrawList_PathStroke);
CK_DLL_SFUN(ui_DrawList_PathArcTo);
CK_DLL_SFUN(ui_DrawList_PathArcToFast);
CK_DLL_SFUN(ui_DrawList_PathEllipticalArcTo);
CK_DLL_SFUN(ui_DrawList_PathEllipticalArcToEx);
CK_DLL_SFUN(ui_DrawList_PathBezierCubicCurveTo);
CK_DLL_SFUN(ui_DrawList_PathBezierQuadraticCurveTo);
CK_DLL_SFUN(ui_DrawList_PathRect);
// TODO image support after we get textures in chugl
// CIMGUI_API void        ImDrawList_AddImage(ImDrawList* self, ImTextureID
// user_texture_id, ImVec2 p_min, ImVec2 p_max); // Implied uv_min = ImVec2(0,
// 0), uv_max = ImVec2(1, 1), col = IM_COL32_WHITE CIMGUI_API void
// ImDrawList_AddImageEx(ImDrawList* self, ImTextureID user_texture_id, ImVec2
// p_min, ImVec2 p_max, ImVec2 uv_min /* = ImVec2(0, 0) */, ImVec2 uv_max /* =
// ImVec2(1, 1) */, ImU32 col /* = IM_COL32_WHITE */); CIMGUI_API void
// ImDrawList_AddImageQuad(ImDrawList* self, ImTextureID user_texture_id, ImVec2
// p1, ImVec2 p2, ImVec2 p3, ImVec2 p4);                        // Implied uv1 =
// ImVec2(0, 0), uv2 = ImVec2(1, 0), uv3 = ImVec2(1, 1), uv4 = ImVec2(0, 1), col
// = IM_COL32_WHITE CIMGUI_API void        ImDrawList_AddImageQuadEx(ImDrawList*
// self, ImTextureID user_texture_id, ImVec2 p1, ImVec2 p2, ImVec2 p3, ImVec2
// p4, ImVec2 uv1 /* = ImVec2(0, 0) */, ImVec2 uv2 /* = ImVec2(1, 0) */, ImVec2
// uv3 /* = ImVec2(1, 1) */, ImVec2 uv4 /* = ImVec2(0, 1) */, ImU32 col /* =
// IM_COL32_WHITE */); CIMGUI_API void ImDrawList_AddImageRounded(ImDrawList*
// self, ImTextureID user_texture_id, ImVec2 p_min, ImVec2 p_max, ImVec2 uv_min,
// ImVec2 uv_max, ImU32 col, float rounding, ImDrawFlags flags /* = 0 */);

// Helpers -------------------------------------------------------------------

// clang-format off

// helper so that CHUGL_UI_VAL_PTR macro doesn't expand ckobj twice
static void* _chugl_ui_val_ptr(Chuck_Object* ckobj, t_CKUINT offset)
{
    return ckobj ?  g_chuglAPI->object->data(ckobj, offset) : NULL;
}

#define CHUGL_UI_VAL_PTR(type, ckobj, offset) (type*) _chugl_ui_val_ptr(ckobj, offset);

// clang-format on

void ulib_imgui_query(Chuck_DL_Query* QUERY)
{
    // UI_Bool ---------------------------------------------------------------
    QUERY->begin_class(QUERY, "UI_Bool", "Object");
    ui_bool_val_offset = QUERY->add_mvar(QUERY, "int", "bool_val", false);

    CTOR(ui_bool_ctor);

    CTOR(ui_bool_ctor_val);
    ARG("int", "val");

    // can't use b/c collision with mvar val
    MFUN(ui_bool_get_value, "int", "val");
    MFUN(ui_bool_set_value, "void", "val");
    ARG("int", "val");

    QUERY->end_class(QUERY); // UI_Bool

    // UI_String ---------------------------------------------------------------
    BEGIN_CLASS("UI_String", "Object");
    ui_string_ptr_offset = MVAR("int", "@ui_string_ptr", false);
    ui_string_cap_offset = MVAR("int", "@ui_string_cap", false);
    CTOR(ui_string_ctor);
    CTOR(ui_string_ctor_str);
    ARG("string", "str");
    DTOR(ui_string_dtor);
    MFUN(ui_string_get_value, "string", "val");
    MFUN(ui_string_set_value, "void", "val");
    ARG("string", "val");
    END_CLASS(); // UI_String

    // UI_Int ---------------------------------------------------------------
    QUERY->begin_class(QUERY, "UI_Int", "Object");
    ui_int_ptr_offset = QUERY->add_mvar(QUERY, "int", "@ui_int_ptr", false);

    QUERY->add_ctor(QUERY, ui_int_ctor);
    CTOR(ui_int_ctor_int);
    ARG("int", "initializer_int");
    QUERY->add_dtor(QUERY, ui_int_dtor);
    QUERY->add_mfun(QUERY, ui_int_get_value, "int", "val");
    QUERY->add_mfun(QUERY, ui_int_set_value, "int", "val");
    QUERY->add_arg(QUERY, "int", "val");
    QUERY->end_class(QUERY); // UI_Int

    // UI_Int2 ---------------------------------------------------------------
    BEGIN_CLASS("UI_Int2", "Object");
    ui_int2_ptr_offset = MVAR("int", "@ui_int2_ptr", false);
    CTOR(ui_int2_ctor);
    DTOR(ui_int2_dtor);
    MFUN(ui_int2_get_x, "int", "x");
    MFUN(ui_int2_get_y, "int", "y");
    MFUN(ui_int2_set, "void", "val");
    ARG("int", "x");
    ARG("int", "y");
    END_CLASS(); // UI_Int2

    // UI_Int3 ---------------------------------------------------------------
    BEGIN_CLASS("UI_Int3", "Object");
    ui_int3_ptr_offset = MVAR("int", "@ui_int3_ptr", false);
    CTOR(ui_int3_ctor);
    DTOR(ui_int3_dtor);
    MFUN(ui_int3_get_x, "int", "x");
    MFUN(ui_int3_get_y, "int", "y");
    MFUN(ui_int3_get_z, "int", "z");
    MFUN(ui_int3_set, "void", "val");
    ARG("int", "x");
    ARG("int", "y");
    ARG("int", "z");
    END_CLASS(); // UI_Int3

    // UI_Int4 ---------------------------------------------------------------
    BEGIN_CLASS("UI_Int4", "Object");
    ui_int4_ptr_offset = MVAR("int", "@ui_int4_ptr", false);
    CTOR(ui_int4_ctor);
    DTOR(ui_int4_dtor);
    MFUN(ui_int4_get_x, "int", "x");
    MFUN(ui_int4_get_y, "int", "y");
    MFUN(ui_int4_get_z, "int", "z");
    MFUN(ui_int4_get_w, "int", "w");
    MFUN(ui_int4_set, "void", "val");
    ARG("int", "x");
    ARG("int", "y");
    ARG("int", "z");
    ARG("int", "w");
    END_CLASS(); // UI_Int4

    // UI_Float ---------------------------------------------------------------
    BEGIN_CLASS("UI_Float", "Object");
    ui_float_ptr_offset = MVAR("int", "@ui_float_ptr", false);
    CTOR(ui_float_ctor);
    CTOR(ui_float_ctor_float);
    ARG("float", "val");
    DTOR(ui_float_dtor);
    MFUN(ui_float_get_value, "float", "val");
    MFUN(ui_float_set_value, "float", "val");
    ARG("float", "val");
    END_CLASS(); // UI_Float

    // UI_Float2 ---------------------------------------------------------------
    BEGIN_CLASS("UI_Float2", "Object");
    ui_float2_ptr_offset = MVAR("int", "@ui_float2_ptr", false);
    CTOR(ui_float2_ctor);

    CTOR(ui_float2_ctor_with_value);
    ARG("vec2", "val");

    CTOR(ui_float2_ctor_with_floats);
    ARG("float", "x");
    ARG("float", "y");

    DTOR(ui_float2_dtor);
    MFUN(ui_float2_get_value, "vec2", "val");
    MFUN(ui_float2_set_value, "vec2", "val");
    ARG("vec2", "val");
    END_CLASS(); // UI_Float2

    // UI_Float3 ---------------------------------------------------------------
    BEGIN_CLASS("UI_Float3", "Object");
    ui_float3_ptr_offset = MVAR("int", "@ui_float3_ptr", false);
    CTOR(ui_float3_ctor);

    CTOR(ui_float3_ctor_with_value);
    ARG("vec3", "val");

    CTOR(ui_float3_ctor_with_floats);
    ARG("float", "x");
    ARG("float", "y");
    ARG("float", "z");

    DTOR(ui_float3_dtor);
    MFUN(ui_float3_get_value, "vec3", "val");
    MFUN(ui_float3_set_value, "vec3", "val");
    ARG("vec3", "val");
    END_CLASS(); // UI_Float3

    // UI_Float4 ---------------------------------------------------------------
    BEGIN_CLASS("UI_Float4", "Object");
    ui_float4_ptr_offset = MVAR("int", "@ui_float4_ptr", false);
    CTOR(ui_float4_ctor);

    CTOR(ui_float4_ctor_with_value);
    ARG("vec4", "val");

    CTOR(ui_float4_ctor_with_floats);
    ARG("float", "x");
    ARG("float", "y");
    ARG("float", "z");
    ARG("float", "w");

    DTOR(ui_float4_dtor);
    MFUN(ui_float4_get_value, "vec4", "val");
    MFUN(ui_float4_set_value, "vec4", "val");
    ARG("vec4", "val");
    END_CLASS(); // UI_Float4

    // UI_Viewport
    // ---------------------------------------------------------------
    BEGIN_CLASS("UI_Viewport", "Object");
    DOC_CLASS(
      "Internal class used by ImGui. DO NOT INSTANTIATE DIRECTLY\n"
      "Currently represents the Platform Window created by the application "
      "which is hosting our Dear ImGui windows.\n"
      "With multi-viewport enabled, we extend this concept to have multiple "
      "active viewports.\n"
      "In the future we will extend this concept further to also represent "
      "Platform Monitor and support a 'no main platform window' operation "
      "mode.\n"
      "About Main Area vs Work Area:\n"
      "  - Main Area = entire viewport.\n"
      "  - Work Area = entire viewport minus sections used by main menu bars "
      "(for platform windows), or by task bar (for platform monitor).\n"
      "  - Windows are generally trying to stay within the Work Area of their "
      "host viewport.\n");
    ui_viewport_ptr_offset = MVAR("int", "@ui_viewport_ptr", false);
    MFUN(ui_viewport_get_id, "int", "id");
    DOC_FUNC("Returns the unique identifier for the viewport.");
    MFUN(ui_viewport_get_flags, "int", "flags");
    DOC_FUNC("Returns the UI_ViewportFlags for the viewport.");
    MFUN(ui_viewport_get_pos, "vec2", "pos");
    DOC_FUNC(
      "Main Area: Position of the viewport (Dear ImGui coordinates are the "
      "same as OS desktop/native coordinates).");
    MFUN(ui_viewport_get_size, "vec2", "size");
    DOC_FUNC("Main Area: Size of the viewport.");
    MFUN(ui_viewport_get_work_pos, "vec2", "workPos");
    DOC_FUNC(
      "Work Area: Position of the viewport minus task bars, menus bars, status "
      "bars (>= Pos).");
    MFUN(ui_viewport_get_work_size, "vec2", "workSize");
    DOC_FUNC(
      "Work Area: Size of the viewport minus task bars, menu bars, status bars "
      "(<= Size).");
    MFUN(ui_viewport_get_dpi_scale, "float", "dpiScale");
    DOC_FUNC("DPI scale: 1.0f = 96 DPI = No extra scale.");
    MFUN(ui_viewport_get_parent_viewport_id, "int", "parentID");
    DOC_FUNC(
      "(Advanced) 0: no parent. Instruct the platform backend to setup a "
      "parent/child relationship between platform windows.");
    // MFUN(ui_viewport_get_draw_data, "void*", "drawData");
    // DOC_FUNC("Draw data to render the viewport.");
    MFUN(ui_viewport_get_center, "vec2", "center");
    DOC_FUNC("Main Area: Center position of the viewport.");
    MFUN(ui_viewport_get_work_center, "vec2", "workCenter");
    DOC_FUNC("Work Area: Center position of the viewport.");
    END_CLASS(); // UI_Viewport

    // UI_Style ---------------------------------------------------------------
    BEGIN_CLASS("UI_Style", "Object");
    DOC_CLASS(
      "Internal class. Do not instantiate directly. Use UI.getStyle() "
      "instead.\n"
      "You may modify the ImGui::GetStyle() main instance during "
      "initialization and before NewFrame()."
      "During the frame, use "
      "UI.pushStyleVar(UI_StyleVar.XXXX)/popStyleVar() to alter the main "
      "style values,"
      "and UI.pushStyleColor(UI_Color.XXX)/popStyleColor() for colors.");
    ui_style_ptr_offset = MVAR("int", "@ui_style_ptr", false);
    MFUN(ui_style_get_alpha, "float", "alpha");
    DOC_FUNC("Global alpha applies to everything in ImGui.");

    MFUN(ui_style_get_disabled_alpha, "float", "disabledAlpha");
    DOC_FUNC(
      "Additional alpha multiplier applied by BeginDisabled(). Multiply over "
      "current value of Alpha.");

    MFUN(ui_style_get_window_padding, "vec2", "windowPadding");
    DOC_FUNC("Padding within a window.");

    MFUN(ui_style_get_window_rounding, "float", "windowRounding");
    DOC_FUNC(
      "Radius of window corners rounding. Set to 0.0f to have rectangular "
      "windows. Large values tend to lead to variety of artifacts and are not "
      "recommended.");

    MFUN(ui_style_get_window_border_size, "float", "windowBorderSize");
    DOC_FUNC(
      "Thickness of border around windows. Generally set to 0.0f or 1.0f. "
      "(Other values are not well tested and more CPU/GPU costly).");

    MFUN(ui_style_get_window_min_size, "vec2", "windowMinSize");
    DOC_FUNC(
      "Minimum window size. This is a global setting. If you want to "
      "constrain individual windows, use SetNextWindowSizeConstraints().");

    MFUN(ui_style_get_window_title_align, "vec2", "windowTitleAlign");
    DOC_FUNC(
      "Alignment for title bar text. Defaults to (0.0f,0.5f) for "
      "left-aligned,vertically centered.");

    MFUN(ui_style_get_window_menu_button_position, "int", "windowMenuButtonPosition");
    DOC_FUNC(
      "Side of the collapsing/docking button in the title bar "
      "(None/Left/Right). "
      "Defaults to UI_Direction.Left.");

    MFUN(ui_style_get_child_rounding, "float", "childRounding");
    DOC_FUNC(
      "Radius of child window corners rounding. Set to 0.0f to have "
      "rectangular "
      "windows.");

    MFUN(ui_style_get_child_border_size, "float", "childBorderSize");
    DOC_FUNC(
      "Thickness of border around child windows. Generally set to 0.0f or "
      "1.0f. "
      "(Other values are not well tested and more CPU/GPU costly).");

    MFUN(ui_style_get_popup_rounding, "float", "popupRounding");
    DOC_FUNC(
      "Radius of popup window corners rounding. (Note that tooltip windows "
      "use WindowRounding)");

    MFUN(ui_style_get_popup_border_size, "float", "popupBorderSize");
    DOC_FUNC(
      "Thickness of border around popup/tooltip windows. Generally set to 0.0f "
      "or 1.0f. (Other values are not well tested and more CPU/GPU costly).");

    MFUN(ui_style_get_frame_padding, "vec2", "framePadding");
    DOC_FUNC("Padding within a framed rectangle (used by most widgets).");

    MFUN(ui_style_get_frame_rounding, "float", "frameRounding");
    DOC_FUNC(
      "Radius of frame corners rounding. Set to 0.0f to have rectangular frame "
      "(used by most widgets).");

    MFUN(ui_style_get_frame_border_size, "float", "frameBorderSize");
    DOC_FUNC(
      "Thickness of border around frames. Generally set to 0.0f or 1.0f. "
      "(Other "
      "values are not well tested and more CPU/GPU costly).");

    MFUN(ui_style_get_item_spacing, "vec2", "itemSpacing");
    DOC_FUNC("Horizontal and vertical spacing between widgets/lines.");

    MFUN(ui_style_get_item_inner_spacing, "vec2", "itemInnerSpacing");
    DOC_FUNC(
      "Horizontal and vertical spacing between within elements of a composed "
      "widget (e.g. a slider and its label).");

    MFUN(ui_style_get_cell_padding, "vec2", "cellPadding");
    DOC_FUNC(
      "Padding within a table cell. Cellpadding.x is locked for entire "
      "table. CellPadding.y may be altered between different rows.");

    MFUN(ui_style_get_touch_extra_padding, "vec2", "touchExtraPadding");
    DOC_FUNC(
      "Expand reactive bounding box for touch-based system where touch "
      "position "
      "is not accurate enough. Unfortunately we don't sort widgets so priority "
      "on overlap will always be given to the first widget. So don't grow this "
      "too much!");

    MFUN(ui_style_get_indent_spacing, "float", "indentSpacing");
    DOC_FUNC(
      "Horizontal indentation when e.g. entering a tree node. Generally == "
      "(FontSize + FramePadding.x*2).");

    MFUN(ui_style_get_columns_min_spacing, "float", "columnsMinSpacing");
    DOC_FUNC(
      "Minimum horizontal spacing between two columns. Preferably > "
      "(FramePadding.x + 1).");

    MFUN(ui_style_get_scrollbar_size, "float", "scrollbarSize");
    DOC_FUNC("Width of the vertical scrollbar, Height of the horizontal scrollbar.");

    MFUN(ui_style_get_scrollbar_rounding, "float", "scrollbarRounding");
    DOC_FUNC("Radius of grab corners for scrollbar.");

    MFUN(ui_style_get_grab_min_size, "float", "grabMinSize");
    DOC_FUNC("Minimum width/height of a grab box for slider/scrollbar.");

    MFUN(ui_style_get_grab_rounding, "float", "grabRounding");
    DOC_FUNC(
      "Radius of grabs corners rounding. Set to 0.0f to have rectangular "
      "slider "
      "grabs.");

    MFUN(ui_style_get_log_slider_deadzone, "float", "logSliderDeadzone");
    DOC_FUNC(
      "The size in pixels of the dead-zone around zero on logarithmic sliders "
      "that cross zero.");

    MFUN(ui_style_get_tab_rounding, "float", "tabRounding");
    DOC_FUNC(
      "Radius of upper corners of a tab. Set to 0.0f to have rectangular "
      "tabs.");

    MFUN(ui_style_get_tab_border_size, "float", "tabBorderSize");
    DOC_FUNC("Thickness of border around tabs.");

    MFUN(ui_style_get_tab_min_width_for_close_button, "float",
         "tabMinWidthForCloseButton");
    DOC_FUNC(
      "Minimum width for close button to appear on an unselected tab when "
      "hovered. Set to 0.0f to always show when hovering, set to FLT_MAX to "
      "never show close button unless selected.");

    MFUN(ui_style_get_tab_bar_border_size, "float", "tabBarBorderSize");
    DOC_FUNC(
      "Thickness of tab-bar separator, which takes on the tab active color to "
      "denote focus.");

    MFUN(ui_style_get_table_angled_headers_angle, "float", "tableAngledHeadersAngle");
    DOC_FUNC(
      "Angle of angled headers (supported values range from -50.0f degrees to "
      "+50.0f degrees).");

    MFUN(ui_style_get_table_angled_headers_text_align, "vec2",
         "tableAngledHeadersTextAlign");
    DOC_FUNC("Alignment of angled headers within the cell");

    MFUN(ui_style_get_color_button_position, "int", "colorButtonPosition");
    DOC_FUNC(
      "Side of the color button in the ColorEdit4 widget (left/right). "
      "Defaults "
      "to UI_Direction.Right.");

    MFUN(ui_style_get_button_text_align, "vec2", "buttonTextAlign");
    DOC_FUNC(
      "Alignment of button text when button is larger than text. Defaults to "
      "(0.5f, 0.5f) (centered).");

    MFUN(ui_style_get_selectable_text_align, "vec2", "selectableTextAlign");
    DOC_FUNC(
      "Alignment of selectable text. Defaults to (0.0f, 0.0f) (top-left "
      "aligned). "
      "It's generally important to keep this left-aligned if you want to lay "
      "multiple items on a same line.");

    MFUN(ui_style_get_separator_text_border_size, "float", "separatorTextBorderSize");
    DOC_FUNC("Thickkness of border in SeparatorText()");

    MFUN(ui_style_get_separator_text_align, "vec2", "separatorTextAlign");
    DOC_FUNC(
      "Alignment of text within the separator. Defaults to (0.0f, 0.5f) (left "
      "aligned, center).");

    MFUN(ui_style_get_separator_text_padding, "vec2", "separatorTextPadding");
    DOC_FUNC(
      "Horizontal offset of text from each edge of the separator + spacing on "
      "other axis. Generally small values. .y is recommended to be == "
      "FramePadding.y.");

    MFUN(ui_style_get_display_window_padding, "vec2", "displayWindowPadding");
    DOC_FUNC(
      "Window position are clamped to be visible within the display area or "
      "monitors by at least this amount. Only applies to regular windows.");

    MFUN(ui_style_get_display_safe_area_padding, "vec2", "displaySafeAreaPadding");
    DOC_FUNC(
      "If you cannot see the edges of your screen (e.g. on a TV) increase the "
      "safe area padding. Apply to popups/tooltips as well regular windows. "
      "NB: "
      "Prefer configuring your TV sets correctly!");

    MFUN(ui_style_get_docking_separator_size, "float", "dockingSeparatorSize");
    DOC_FUNC("Thickness of resizing border between docked windows");

    MFUN(ui_style_get_mouse_cursor_scale, "float", "mouseCursorScale");
    DOC_FUNC(
      "Scale software rendered mouse cursor (when io.MouseDrawCursor is "
      "enabled). "
      "We apply per-monitor DPI scaling over this scale. May be removed "
      "later.");

    MFUN(ui_style_get_anti_aliased_lines, "int", "antiAliasedLines");
    DOC_FUNC(
      "Enable anti-aliased lines/borders. Disable if you are really tight on "
      "CPU/GPU. Latched at the beginning of the frame (copied to ImDrawList).");

    MFUN(ui_style_get_anti_aliased_lines_use_tex, "int", "antiAliasedLinesUseTex");
    DOC_FUNC(
      "Enable anti-aliased lines/borders using textures where possible. "
      "Require "
      "backend to render with bilinear filtering (NOT point/nearest "
      "filtering). "
      "Latched at the beginning of the frame (copied to ImDrawList).");

    MFUN(ui_style_get_anti_aliased_fill, "int", "antiAliasedFill");
    DOC_FUNC(
      "Enable anti-aliased edges around filled shapes (rounded rectangles, "
      "circles, etc.). Disable if you are really tight on CPU/GPU. Latched at "
      "the beginning of the frame (copied to ImDrawList).");

    MFUN(ui_style_get_curve_tessellation_tol, "float", "curveTessellationTol");
    DOC_FUNC(
      "Tessellation tolerance when using PathBezierCurveTo() without a "
      "specific "
      "number of segments. Decrease for highly tessellated curves (higher "
      "quality, more polygons), increase to reduce quality.");

    MFUN(ui_style_get_circle_tessellation_max_error, "float",
         "circleTessellationMaxError");
    DOC_FUNC(
      "Maximum error (in pixels) allowed when using "
      "AddCircle()/AddCircleFilled() "
      "or drawing rounded corner rectangles with no explicit segment count "
      "specified. Decrease for higher quality but more geometry.");

    MFUN(ui_style_get_color, "vec4", "color");
    ARG("int", "ui_color_flag");
    DOC_FUNC("Get the color for a specific UI_Color flag.");

    MFUN(ui_style_get_hover_stationary_delay, "float", "hoverStationaryDelay");
    DOC_FUNC(
      "Delay for IsItemHovered(ImGuiHoveredFlags_Stationary). Time required to "
      "consider mouse stationary.");

    MFUN(ui_style_get_hover_delay_short, "float", "hoverDelayShort");
    DOC_FUNC(
      "Delay for IsItemHovered(ImGuiHoveredFlags_DelayShort). Usually used "
      "along with HoverStationaryDelay.");

    MFUN(ui_style_get_hover_delay_normal, "float", "hoverDelayNormal");
    DOC_FUNC("Delay for IsItemHovered(ImGuiHoveredFlags_DelayNormal). ");

    MFUN(ui_style_get_hover_flags_for_tooltip_mouse, "int",
         "hoverFlagsForTooltipMouse");
    DOC_FUNC(
      "Default flags when using IsItemHovered(ImGuiHoveredFlags_ForTooltip) or "
      "BeginItemTooltip()/SetItemTooltip() while using mouse.");

    MFUN(ui_style_get_hover_flags_for_tooltip_nav, "int", "hoverFlagsForTooltipNav");
    DOC_FUNC(
      "Default flags when using IsItemHovered(ImGuiHoveredFlags_ForTooltip) or "
      "BeginItemTooltip()/SetItemTooltip() while using keyboard/gamepad.");

    MFUN(ui_style_set_alpha, "void", "alpha");
    ARG("float", "alpha");

    MFUN(ui_style_set_disabled_alpha, "void", "disabledAlpha");
    ARG("float", "disabledAlpha");

    MFUN(ui_style_set_window_padding, "void", "windowPadding");
    ARG("vec2", "windowPadding");

    MFUN(ui_style_set_window_rounding, "void", "windowRounding");
    ARG("float", "windowRounding");

    MFUN(ui_style_set_window_border_size, "void", "windowBorderSize");
    ARG("float", "windowBorderSize");

    MFUN(ui_style_set_window_min_size, "void", "windowMinSize");
    ARG("vec2", "windowMinSize");

    MFUN(ui_style_set_window_title_align, "void", "windowTitleAlign");
    ARG("vec2", "windowTitleAlign");

    MFUN(ui_style_set_window_menu_button_position, "void", "windowMenuButtonPosition");
    ARG("int", "ui_direction");
    DOC_FUNC("ui_direction is enum UI_Direction");

    MFUN(ui_style_set_child_rounding, "void", "childRounding");
    ARG("float", "childRounding");

    MFUN(ui_style_set_child_border_size, "void", "childBorderSize");
    ARG("float", "childBorderSize");

    MFUN(ui_style_set_popup_rounding, "void", "popupRounding");
    ARG("float", "popupRounding");

    MFUN(ui_style_set_popup_border_size, "void", "popupBorderSize");
    ARG("float", "popupBorderSize");

    MFUN(ui_style_set_frame_padding, "void", "framePadding");
    ARG("vec2", "framePadding");

    MFUN(ui_style_set_frame_rounding, "void", "frameRounding");
    ARG("float", "frameRounding");

    MFUN(ui_style_set_frame_border_size, "void", "frameBorderSize");
    ARG("float", "frameBorderSize");

    MFUN(ui_style_set_item_spacing, "void", "itemSpacing");
    ARG("vec2", "itemSpacing");

    MFUN(ui_style_set_item_inner_spacing, "void", "itemInnerSpacing");
    ARG("vec2", "itemInnerSpacing");

    MFUN(ui_style_set_cell_padding, "void", "cellPadding");
    ARG("vec2", "cellPadding");

    MFUN(ui_style_set_touch_extra_padding, "void", "touchExtraPadding");
    ARG("vec2", "touchExtraPadding");

    MFUN(ui_style_set_indent_spacing, "void", "indentSpacing");
    ARG("float", "indentSpacing");

    MFUN(ui_style_set_columns_min_spacing, "void", "columnsMinSpacing");
    ARG("float", "columnsMinSpacing");

    MFUN(ui_style_set_scrollbar_size, "void", "scrollbarSize");
    ARG("float", "scrollbarSize");

    MFUN(ui_style_set_scrollbar_rounding, "void", "scrollbarRounding");
    ARG("float", "scrollbarRounding");

    MFUN(ui_style_set_grab_min_size, "void", "grabMinSize");
    ARG("float", "grabMinSize");

    MFUN(ui_style_set_grab_rounding, "void", "grabRounding");
    ARG("float", "grabRounding");

    MFUN(ui_style_set_log_slider_deadzone, "void", "logSliderDeadzone");
    ARG("float", "logSliderDeadzone");

    MFUN(ui_style_set_tab_rounding, "void", "tabRounding");
    ARG("float", "tabRounding");

    MFUN(ui_style_set_tab_border_size, "void", "tabBorderSize");
    ARG("float", "tabBorderSize");

    MFUN(ui_style_set_tab_min_width_for_close_button, "void",
         "tabMinWidthForCloseButton");
    ARG("float", "tabMinWidthForCloseButton");

    MFUN(ui_style_set_tab_bar_border_size, "void", "tabBarBorderSize");
    ARG("float", "tabBarBorderSize");

    MFUN(ui_style_set_table_angled_headers_angle, "void", "tableAngledHeadersAngle");
    ARG("float", "tableAngledHeadersAngle");

    MFUN(ui_style_set_table_angled_headers_text_align, "void",
         "tableAngledHeadersTextAlign");
    ARG("vec2", "tableAngledHeadersTextAlign");

    MFUN(ui_style_set_color_button_position, "void", "colorButtonPosition");
    ARG("int", "ui_direction");

    MFUN(ui_style_set_button_text_align, "void", "buttonTextAlign");
    ARG("vec2", "buttonTextAlign");

    MFUN(ui_style_set_selectable_text_align, "void", "selectableTextAlign");
    ARG("vec2", "selectableTextAlign");

    MFUN(ui_style_set_separator_text_border_size, "void", "separatorTextBorderSize");
    ARG("float", "separatorTextBorderSize");

    MFUN(ui_style_set_separator_text_align, "void", "separatorTextAlign");
    ARG("vec2", "separatorTextAlign");

    MFUN(ui_style_set_separator_text_padding, "void", "separatorTextPadding");
    ARG("vec2", "separatorTextPadding");

    MFUN(ui_style_set_display_window_padding, "void", "displayWindowPadding");
    ARG("vec2", "displayWindowPadding");

    MFUN(ui_style_set_display_safe_area_padding, "void", "displaySafeAreaPadding");
    ARG("vec2", "displaySafeAreaPadding");

    MFUN(ui_style_set_docking_separator_size, "void", "dockingSeparatorSize");
    ARG("float", "dockingSeparatorSize");

    MFUN(ui_style_set_mouse_cursor_scale, "void", "mouseCursorScale");
    ARG("float", "mouseCursorScale");

    MFUN(ui_style_set_anti_aliased_lines, "void", "antiAliasedLines");
    ARG("int", "antiAliasedLines");

    MFUN(ui_style_set_anti_aliased_lines_use_tex, "void", "antiAliasedLinesUseTex");
    ARG("int", "antiAliasedLinesUseTex");

    MFUN(ui_style_set_anti_aliased_fill, "void", "antiAliasedFill");
    ARG("int", "antiAliasedFill");

    MFUN(ui_style_set_curve_tessellation_tol, "void", "curveTessellationTol");
    ARG("float", "curveTessellationTol");

    MFUN(ui_style_set_circle_tessellation_max_error, "void",
         "circleTessellationMaxError");
    ARG("float", "circleTessellationMaxError");

    MFUN(ui_style_set_color, "void", "color");
    ARG("int", "ui_color_flag");
    ARG("vec4", "color");

    MFUN(ui_style_set_hover_stationary_delay, "void", "hoverStationaryDelay");
    ARG("float", "hoverStationaryDelay");

    MFUN(ui_style_set_hover_delay_short, "void", "hoverDelayShort");
    ARG("float", "hoverDelayShort");

    MFUN(ui_style_set_hover_delay_normal, "void", "hoverDelayNormal");
    ARG("float", "hoverDelayNormal");

    MFUN(ui_style_set_hover_flags_for_tooltip_mouse, "void",
         "hoverFlagsForTooltipMouse");
    ARG("int", "hoverFlagsForTooltipMouse");

    MFUN(ui_style_set_hover_flags_for_tooltip_nav, "void", "hoverFlagsForTooltipNav");
    ARG("int", "hoverFlagsForTooltipNav");

    END_CLASS(); // UI_Style

    BEGIN_CLASS("UI_DrawList", "Object");
    DOC_CLASS(
      "All methods add to the current window draw list (igGetWindowDrawList())."
      "This is the low-level list of polygons that ImGui:: functions are "
      "filling. At the end of the frame,"
      "all command lists are passed to your ImGuiIO::RenderDrawListFn function "
      "for rendering."
      "Each dear imgui window contains its own ImDrawList."
      "You can interleave normal ImGui:: calls and adding primitives to the "
      "current draw list."
      "In single viewport mode, top-left is == GetMainViewport()->Pos "
      "(generally 0,0), bottom-right is == GetMainViewport()->Pos+Size "
      "(generally io.DisplaySize)."
      "You are totally free to apply whatever transformation matrix to want to "
      "the data (depending on the use of the transformation you may want to "
      "apply it to ClipRect as well!)"
      "Important: Primitives are always added to the list and not culled "
      "(culling is done at higher-level by ImGui:: functions), if you use this "
      "API a lot consider coarse culling your drawn objects."
      "Filled shapes must always use clockwise winding order. The "
      "anti-aliasing fringe depends on it. Counter-clockwise shapes will "
      "have inward anti-aliasing."
      "For rectangular primitives, \"p_min\" and \"p_max\" represent the "
      "upper-left and lower-right corners."
      "For circle primitives, use \"num_segments == 0\" to automatically "
      "calculate tessellation (preferred)."
      "In older versions (until Dear ImGui 1.77) the AddCircle functions "
      "defaulted to num_segments == 12."
      "In future versions we will use textures to provide cheaper and "
      "higher-quality circles."
      "Use AddNgon() and AddNgonFilled() functions if you need to guarantee a "
      "specific number of sides.");

    SFUN(ui_DrawList_PushClipRect, "void", "pushClipRect");
    ARG("vec2", "clip_rect_min");
    ARG("vec2", "clip_rect_max");
    ARG("int", "intersect_with_current_clip_rect");
    DOC_FUNC(
      "Render-level scissoring. This is passed down to your render function "
      "but not used for CPU-side coarse clipping. Prefer using higher-level "
      "ImGui::PushClipRect() to affect logic (hit-testing and widget culling)");

    SFUN(ui_DrawList_PushClipRectFullScreen, "void", "pushClipRectFullScreen");
    SFUN(ui_DrawList_PopClipRect, "void", "popClipRect");
    SFUN(ui_DrawList_GetClipRectMin, "vec2", "getClipRectMin");
    SFUN(ui_DrawList_GetClipRectMax, "vec2", "getClipRectMax");

    // primitives
    SFUN(ui_DrawList_AddLine, "void", "addLine");
    ARG("vec2", "p1");
    ARG("vec2", "p2");
    ARG("vec4", "col");

    SFUN(ui_DrawList_AddLineEx, "void", "addLineEx");
    ARG("vec2", "p1");
    ARG("vec2", "p2");
    ARG("vec4", "col");
    ARG("float", "thickness");

    SFUN(ui_DrawList_AddRect, "void", "addRect");
    ARG("vec2", "p_min");
    ARG("vec2", "p_max");
    ARG("vec4", "col");

    SFUN(ui_DrawList_AddRectEx, "void", "addRectEx");
    ARG("vec2", "p_min");
    ARG("vec2", "p_max");
    ARG("vec4", "col");
    ARG("float", "rounding");
    ARG("int", "UI_DrawFlags");
    ARG("float", "thickness");
    DOC_FUNC("a: upper-left, b: lower-right (== upper-left + size)");

    SFUN(ui_DrawList_AddRectFilled, "void", "addRectFilled");
    ARG("vec2", "p_min");
    ARG("vec2", "p_max");
    ARG("vec4", "col");

    SFUN(ui_DrawList_AddRectFilledEx, "void", "addRectFilledEx");
    ARG("vec2", "p_min");
    ARG("vec2", "p_max");
    ARG("vec4", "col");
    ARG("float", "rounding");
    ARG("int", "UI_DrawFlags");
    DOC_FUNC("a: upper-left, b: lower-right (== upper-left + size)");

    SFUN(ui_DrawList_AddRectFilledMultiColor, "void", "addRectFilledMultiColor");
    ARG("vec2", "p_min");
    ARG("vec2", "p_max");
    ARG("vec4", "col_upr_left");
    ARG("vec4", "col_upr_right");
    ARG("vec4", "col_bot_right");
    ARG("vec4", "col_bot_left");

    SFUN(ui_DrawList_AddQuad, "void", "addQuad");
    ARG("vec2", "p1");
    ARG("vec2", "p2");
    ARG("vec2", "p3");
    ARG("vec2", "p4");
    ARG("vec4", "col");

    SFUN(ui_DrawList_AddQuadEx, "void", "addQuadEx");
    ARG("vec2", "p1");
    ARG("vec2", "p2");
    ARG("vec2", "p3");
    ARG("vec2", "p4");
    ARG("vec4", "col");
    ARG("float", "thickness");

    SFUN(ui_DrawList_AddQuadFilled, "void", "addQuadFilled");
    ARG("vec2", "p1");
    ARG("vec2", "p2");
    ARG("vec2", "p3");
    ARG("vec2", "p4");
    ARG("vec4", "col");

    SFUN(ui_DrawList_AddTriangle, "void", "addTriangle");
    ARG("vec2", "p1");
    ARG("vec2", "p2");
    ARG("vec2", "p3");
    ARG("vec4", "col");

    SFUN(ui_DrawList_AddTriangleEx, "void", "addTriangleEx");
    ARG("vec2", "p1");
    ARG("vec2", "p2");
    ARG("vec2", "p3");
    ARG("vec4", "col");
    ARG("float", "thickness");

    SFUN(ui_DrawList_AddTriangleFilled, "void", "addTriangleFilled");
    ARG("vec2", "p1");
    ARG("vec2", "p2");
    ARG("vec2", "p3");
    ARG("vec4", "col");

    SFUN(ui_DrawList_AddCircle, "void", "addCircle");
    ARG("vec2", "center");
    ARG("float", "radius");
    ARG("vec4", "col");

    SFUN(ui_DrawList_AddCircleEx, "void", "addCircleEx");
    ARG("vec2", "center");
    ARG("float", "radius");
    ARG("vec4", "col");
    ARG("int", "num_segments");
    ARG("float", "thickness");

    SFUN(ui_DrawList_AddCircleFilled, "void", "addCircleFilled");
    ARG("vec2", "center");
    ARG("float", "radius");
    ARG("vec4", "col");
    ARG("int", "num_segments");

    SFUN(ui_DrawList_AddNgon, "void", "addNgon");
    ARG("vec2", "center");
    ARG("float", "radius");
    ARG("vec4", "col");
    ARG("int", "num_segments");

    SFUN(ui_DrawList_AddNgonEx, "void", "addNgonEx");
    ARG("vec2", "center");
    ARG("float", "radius");
    ARG("vec4", "col");
    ARG("int", "num_segments");
    ARG("float", "thickness");

    SFUN(ui_DrawList_AddNgonFilled, "void", "addNgonFilled");
    ARG("vec2", "center");
    ARG("float", "radius");
    ARG("vec4", "col");
    ARG("int", "num_segments");

    SFUN(ui_DrawList_AddEllipse, "void", "addEllipse");
    ARG("vec2", "center");
    ARG("vec2", "radius");
    ARG("vec4", "col");

    SFUN(ui_DrawList_AddEllipseEx, "void", "addEllipseEx");
    ARG("vec2", "center");
    ARG("vec2", "radius");
    ARG("vec4", "col");
    ARG("float", "rot");
    ARG("int", "num_segments");
    ARG("float", "thickness");

    SFUN(ui_DrawList_AddEllipseFilled, "void", "addEllipseFilled");
    ARG("vec2", "center");
    ARG("vec2", "radius");
    ARG("vec4", "col");

    SFUN(ui_DrawList_AddEllipseFilledEx, "void", "addEllipseFilledEx");
    ARG("vec2", "center");
    ARG("vec2", "radius");
    ARG("vec4", "col");
    ARG("float", "rot");
    ARG("int", "num_segments");

    SFUN(ui_DrawList_AddText, "void", "addText");
    ARG("vec2", "pos");
    ARG("vec4", "col");
    ARG("string", "text_begin");

    SFUN(ui_DrawList_AddTextEx, "void", "addTextEx");
    ARG("vec2", "pos");
    ARG("vec4", "col");
    ARG("string", "text_begin");
    ARG("string", "text_end");

    // SFUN(ui_DrawList_AddTextImFontPtr, "void", "addTextImFontPtr");
    // ARG("ImFont", "font");
    // ARG("float", "font_size");
    // ARG("vec2", "pos");
    // ARG("vec4", "col");
    // ARG("string", "text_begin");
    // DOC_FUNC(
    //   "Implied text_end = NULL, wrap_width = 0.0f, cpu_fine_clip_rect = "
    //   "NULL");

    // SFUN(ui_DrawList_AddTextImFontPtrEx, "void", "addTextImFontPtrEx");
    // ARG("ImFont", "font");
    // ARG("float", "font_size");
    // ARG("vec2", "pos");
    // ARG("vec4", "col");
    // ARG("string", "text_begin");
    // ARG("string", "text_end");
    // ARG("float", "wrap_width");
    // ARG("vec4", "cpu_fine_clip_rect");

    SFUN(ui_DrawList_AddBezierCubic, "void", "addBezierCubic");
    ARG("vec2", "p1");
    ARG("vec2", "p2");
    ARG("vec2", "p3");
    ARG("vec2", "p4");
    ARG("vec4", "col");
    ARG("float", "thickness");
    ARG("int", "num_segments");
    DOC_FUNC("Cubic Bezier (4 control points)");

    SFUN(ui_DrawList_AddBezierQuadratic, "void", "addBezierQuadratic");
    ARG("vec2", "p1");
    ARG("vec2", "p2");
    ARG("vec2", "p3");
    ARG("vec4", "col");
    ARG("float", "thickness");
    ARG("int", "num_segments");
    DOC_FUNC("Quadratic Bezier (3 control points)");

    SFUN(ui_DrawList_AddPolyline, "void", "addPolyline");
    ARG("vec2[]", "points");
    ARG("vec4", "col");
    ARG("int", "UI_DrawFlags");
    ARG("float", "thickness");

    SFUN(ui_DrawList_AddConvexPolyFilled, "void", "addConvexPolyFilled");
    ARG("vec2[]", "points");
    ARG("vec4", "col");

    SFUN(ui_DrawList_AddConcavePolyFilled, "void", "addConcavePolyFilled");
    ARG("vec2[]", "points");
    ARG("vec4", "col");

    SFUN(ui_DrawList_PathClear, "void", "pathClear");

    SFUN(ui_DrawList_PathLineTo, "void", "pathLineTo");
    ARG("vec2", "pos");

    SFUN(ui_DrawList_PathLineToMergeDuplicate, "void", "pathLineToMergeDuplicate");
    ARG("vec2", "pos");

    SFUN(ui_DrawList_PathFillConvex, "void", "pathFillConvex");
    ARG("vec4", "col");

    SFUN(ui_DrawList_PathFillConcave, "void", "pathFillConcave");
    ARG("vec4", "col");

    SFUN(ui_DrawList_PathStroke, "void", "pathStroke");
    ARG("vec4", "col");
    ARG("int", "UI_DrawFlags");
    ARG("float", "thickness");

    SFUN(ui_DrawList_PathArcTo, "void", "pathArcTo");
    ARG("vec2", "center");
    ARG("float", "radius");
    ARG("float", "a_min");
    ARG("float", "a_max");
    ARG("int", "num_segments");

    SFUN(ui_DrawList_PathArcToFast, "void", "pathArcToFast");
    ARG("vec2", "center");
    ARG("float", "radius");
    ARG("int", "a_min_of_12");
    ARG("int", "a_max_of_12");
    DOC_FUNC("Use precomputed angles for a 12 steps circle");

    SFUN(ui_DrawList_PathEllipticalArcTo, "void", "pathEllipticalArcTo");
    ARG("vec2", "center");
    ARG("vec2", "radius");
    ARG("float", "rot");
    ARG("float", "a_min");
    ARG("float", "a_max");

    SFUN(ui_DrawList_PathEllipticalArcToEx, "void", "pathEllipticalArcToEx");
    ARG("vec2", "center");
    ARG("vec2", "radius");
    ARG("float", "rot");
    ARG("int", "a_min");
    ARG("int", "a_max");
    ARG("int", "num_segments");

    SFUN(ui_DrawList_PathBezierCubicCurveTo, "void", "pathBezierCubicCurveTo");
    ARG("vec2", "p2");
    ARG("vec2", "p3");
    ARG("vec2", "p4");
    ARG("int", "num_segments");

    SFUN(ui_DrawList_PathBezierQuadraticCurveTo, "void", "pathBezierQuadraticCurveTo");
    ARG("vec2", "p2");
    ARG("vec2", "p3");
    ARG("int", "num_segments");

    SFUN(ui_DrawList_PathRect, "void", "pathRect");
    ARG("vec2", "rect_min");
    ARG("vec2", "rect_max");
    ARG("float", "rounding");
    ARG("int", "UI_DrawFlags");

    END_CLASS(); // UI_DrawList

    // enums
    QUERY->begin_class(QUERY, "UI_WindowFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for ImGui::Begin().\n(Those are per-window flags. There are "
      "shared flags in ImGuiIO: io.ConfigWindowsResizeFromEdges and "
      "io.ConfigWindowsMoveFromTitleBarOnly).\n");
    static t_CKINT ImGuiWindowFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiWindowFlags_None);
    static t_CKINT ImGuiWindowFlags_NoTitleBar = 1;
    QUERY->add_svar(QUERY, "int", "NoTitleBar", true, &ImGuiWindowFlags_NoTitleBar);
    QUERY->doc_var(QUERY, "Disable title-bar");
    static t_CKINT ImGuiWindowFlags_NoResize = 2;
    QUERY->add_svar(QUERY, "int", "NoResize", true, &ImGuiWindowFlags_NoResize);
    QUERY->doc_var(QUERY, "Disable user resizing with the lower-right grip");
    static t_CKINT ImGuiWindowFlags_NoMove = 4;
    QUERY->add_svar(QUERY, "int", "NoMove", true, &ImGuiWindowFlags_NoMove);
    QUERY->doc_var(QUERY, "Disable user moving the window");
    static t_CKINT ImGuiWindowFlags_NoScrollbar = 8;
    QUERY->add_svar(QUERY, "int", "NoScrollbar", true, &ImGuiWindowFlags_NoScrollbar);
    QUERY->doc_var(QUERY,
                   "Disable scrollbars (window can still scroll with mouse or "
                   "programmatically)");
    static t_CKINT ImGuiWindowFlags_NoScrollWithMouse = 16;
    QUERY->add_svar(QUERY, "int", "NoScrollWithMouse", true,
                    &ImGuiWindowFlags_NoScrollWithMouse);
    QUERY->doc_var(QUERY,
                   "Disable user vertically scrolling with mouse wheel. On "
                   "child window, mouse wheel will be forwarded to the parent "
                   "unless NoScrollbar is also set.");
    static t_CKINT ImGuiWindowFlags_NoCollapse = 32;
    QUERY->add_svar(QUERY, "int", "NoCollapse", true, &ImGuiWindowFlags_NoCollapse);
    QUERY->doc_var(
      QUERY,
      "Disable user collapsing window by double-clicking on it. Also referred "
      "to as Window Menu Button (e.g. within a docking node).");
    static t_CKINT ImGuiWindowFlags_AlwaysAutoResize = 64;
    QUERY->add_svar(QUERY, "int", "AlwaysAutoResize", true,
                    &ImGuiWindowFlags_AlwaysAutoResize);
    QUERY->doc_var(QUERY, "Resize every window to its content every frame");
    static t_CKINT ImGuiWindowFlags_NoBackground = 128;
    QUERY->add_svar(QUERY, "int", "NoBackground", true, &ImGuiWindowFlags_NoBackground);
    QUERY->doc_var(
      QUERY,
      "Disable drawing background color (WindowBg, etc.) and outside border. "
      "Similar as using SetNextWindowBgAlpha(0.0f).");
    static t_CKINT ImGuiWindowFlags_NoSavedSettings = 256;
    QUERY->add_svar(QUERY, "int", "NoSavedSettings", true,
                    &ImGuiWindowFlags_NoSavedSettings);
    QUERY->doc_var(QUERY, "Never load/save settings in .ini file");
    static t_CKINT ImGuiWindowFlags_NoMouseInputs = 512;
    QUERY->add_svar(QUERY, "int", "NoMouseInputs", true,
                    &ImGuiWindowFlags_NoMouseInputs);
    QUERY->doc_var(QUERY, "Disable catching mouse, hovering test with pass through.");
    static t_CKINT ImGuiWindowFlags_MenuBar = 1024;
    QUERY->add_svar(QUERY, "int", "MenuBar", true, &ImGuiWindowFlags_MenuBar);
    QUERY->doc_var(QUERY, "Has a menu-bar");
    static t_CKINT ImGuiWindowFlags_HorizontalScrollbar = 2048;
    QUERY->add_svar(QUERY, "int", "HorizontalScrollbar", true,
                    &ImGuiWindowFlags_HorizontalScrollbar);
    QUERY->doc_var(QUERY,
                   "Allow horizontal scrollbar to appear (off by default). You "
                   "may use SetNextWindowContentSize(ImVec2(width,0.0f)); "
                   "prior to calling Begin() to specify width. Read code in "
                   "imgui_demo in the \"Horizontal Scrolling\" section.");
    static t_CKINT ImGuiWindowFlags_NoFocusOnAppearing = 4096;
    QUERY->add_svar(QUERY, "int", "NoFocusOnAppearing", true,
                    &ImGuiWindowFlags_NoFocusOnAppearing);
    QUERY->doc_var(
      QUERY, "Disable taking focus when transitioning from hidden to visible state");
    static t_CKINT ImGuiWindowFlags_NoBringToFrontOnFocus = 8192;
    QUERY->add_svar(QUERY, "int", "NoBringToFrontOnFocus", true,
                    &ImGuiWindowFlags_NoBringToFrontOnFocus);
    QUERY->doc_var(QUERY,
                   "Disable bringing window to front when taking focus (e.g. "
                   "clicking on it or programmatically giving it focus)");
    static t_CKINT ImGuiWindowFlags_AlwaysVerticalScrollbar = 16384;
    QUERY->add_svar(QUERY, "int", "AlwaysVerticalScrollbar", true,
                    &ImGuiWindowFlags_AlwaysVerticalScrollbar);
    QUERY->doc_var(QUERY,
                   "Always show vertical scrollbar (even if ContentSize.y < Size.y)");
    static t_CKINT ImGuiWindowFlags_AlwaysHorizontalScrollbar = 32768;
    QUERY->add_svar(QUERY, "int", "AlwaysHorizontalScrollbar", true,
                    &ImGuiWindowFlags_AlwaysHorizontalScrollbar);
    QUERY->doc_var(QUERY,
                   "Always show horizontal scrollbar (even if ContentSize.x < Size.x)");
    static t_CKINT ImGuiWindowFlags_NoNavInputs = 65536;
    QUERY->add_svar(QUERY, "int", "NoNavInputs", true, &ImGuiWindowFlags_NoNavInputs);
    QUERY->doc_var(QUERY, "No gamepad/keyboard navigation within the window");
    static t_CKINT ImGuiWindowFlags_NoNavFocus = 131072;
    QUERY->add_svar(QUERY, "int", "NoNavFocus", true, &ImGuiWindowFlags_NoNavFocus);
    QUERY->doc_var(QUERY,
                   "No focusing toward this window with gamepad/keyboard "
                   "navigation (e.g. skipped by CTRL+TAB)");
    static t_CKINT ImGuiWindowFlags_UnsavedDocument = 262144;
    QUERY->add_svar(QUERY, "int", "UnsavedDocument", true,
                    &ImGuiWindowFlags_UnsavedDocument);
    QUERY->doc_var(
      QUERY,
      "Display a dot next to the title. When used in a tab/docking context, "
      "tab is selected when clicking the X + closure is not assumed (will wait "
      "for user to stop submitting the tab). Otherwise closure is assumed when "
      "pressing the X, so if you keep submitting the tab may reappear at end "
      "of tab bar.");
    static t_CKINT ImGuiWindowFlags_NoDocking = 524288;
    QUERY->add_svar(QUERY, "int", "NoDocking", true, &ImGuiWindowFlags_NoDocking);
    QUERY->doc_var(QUERY, "Disable docking of this window");
    static t_CKINT ImGuiWindowFlags_NoNav = 196608;
    QUERY->add_svar(QUERY, "int", "NoNav", true, &ImGuiWindowFlags_NoNav);
    static t_CKINT ImGuiWindowFlags_NoDecoration = 43;
    QUERY->add_svar(QUERY, "int", "NoDecoration", true, &ImGuiWindowFlags_NoDecoration);
    static t_CKINT ImGuiWindowFlags_NoInputs = 197120;
    QUERY->add_svar(QUERY, "int", "NoInputs", true, &ImGuiWindowFlags_NoInputs);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_ChildFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for ImGui::BeginChild().\n(Legacy: bit 0 must always correspond "
      "to ImGuiChildFlags_Border to be backward compatible with old API using "
      "'bool border = false'..\nAbout using AutoResizeX/AutoResizeY flags:.\n- "
      "May be combined with SetNextWindowSizeConstraints() to set a min/max "
      "size for each axis (see \"Demo->Child->Auto-resize with "
      "Constraints\")..\n- Size measurement for a given axis is only performed "
      "when the child window is within visible boundaries, or is just "
      "appearing..\n- This allows BeginChild() to return false when not within "
      "boundaries (e.g. when scrolling), which is more optimal. BUT it won't "
      "update its auto-size while clipped..\nWhile not perfect, it is a better "
      "default behavior as the always-on performance gain is more valuable "
      "than the occasional \"resizing after becoming visible again\" "
      "glitch..\n- You may also use ImGuiChildFlags_AlwaysAutoResize to force "
      "an update even when child window is not in view..\nHOWEVER PLEASE "
      "UNDERSTAND THAT DOING SO WILL PREVENT BeginChild() FROM EVER RETURNING "
      "FALSE, disabling benefits of coarse clipping..\n");
    static t_CKINT ImGuiChildFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiChildFlags_None);
    static t_CKINT ImGuiChildFlags_Border = 1;
    QUERY->add_svar(QUERY, "int", "Border", true, &ImGuiChildFlags_Border);
    QUERY->doc_var(QUERY,
                   "Show an outer border and enable WindowPadding. (IMPORTANT: "
                   "this is always == 1 == true for legacy reason)");
    static t_CKINT ImGuiChildFlags_AlwaysUseWindowPadding = 2;
    QUERY->add_svar(QUERY, "int", "AlwaysUseWindowPadding", true,
                    &ImGuiChildFlags_AlwaysUseWindowPadding);
    QUERY->doc_var(
      QUERY,
      "Pad with style.WindowPadding even if no border are drawn (no padding by "
      "default for non-bordered child windows because it makes more sense)");
    static t_CKINT ImGuiChildFlags_ResizeX = 4;
    QUERY->add_svar(QUERY, "int", "ResizeX", true, &ImGuiChildFlags_ResizeX);
    QUERY->doc_var(
      QUERY,
      "Allow resize from right border (layout direction). Enable .ini saving "
      "(unless ImGuiWindowFlags_NoSavedSettings passed to window flags)");
    static t_CKINT ImGuiChildFlags_ResizeY = 8;
    QUERY->add_svar(QUERY, "int", "ResizeY", true, &ImGuiChildFlags_ResizeY);
    QUERY->doc_var(QUERY, "Allow resize from bottom border (layout direction). \"");
    static t_CKINT ImGuiChildFlags_AutoResizeX = 16;
    QUERY->add_svar(QUERY, "int", "AutoResizeX", true, &ImGuiChildFlags_AutoResizeX);
    QUERY->doc_var(QUERY,
                   "Enable auto-resizing width. Read \"IMPORTANT: Size "
                   "measurement\" details above.");
    static t_CKINT ImGuiChildFlags_AutoResizeY = 32;
    QUERY->add_svar(QUERY, "int", "AutoResizeY", true, &ImGuiChildFlags_AutoResizeY);
    QUERY->doc_var(QUERY,
                   "Enable auto-resizing height. Read \"IMPORTANT: Size "
                   "measurement\" details above.");
    static t_CKINT ImGuiChildFlags_AlwaysAutoResize = 64;
    QUERY->add_svar(QUERY, "int", "AlwaysAutoResize", true,
                    &ImGuiChildFlags_AlwaysAutoResize);
    QUERY->doc_var(QUERY,
                   "Combined with AutoResizeX/AutoResizeY. Always measure size "
                   "even when child is hidden, always return true, always "
                   "disable clipping optimization! NOT RECOMMENDED.");
    static t_CKINT ImGuiChildFlags_FrameStyle = 128;
    QUERY->add_svar(QUERY, "int", "FrameStyle", true, &ImGuiChildFlags_FrameStyle);
    QUERY->doc_var(QUERY,
                   "Style the child window like a framed item: use FrameBg, "
                   "FrameRounding, FrameBorderSize, FramePadding instead of "
                   "ChildBg, ChildRounding, ChildBorderSize, WindowPadding.");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_Cond", "Object");
    QUERY->doc_class(
      QUERY,
      "Enumeration for ImGui::SetNextWindow***(), SetWindow***(), "
      "SetNextItem***() functions.\nRepresent a condition..\nImportant: Treat "
      "as a regular enum! Do NOT combine multiple values using binary "
      "operators! All the functions above treat 0 as a shortcut to "
      "ImGuiCond_Always..\n");
    static t_CKINT ImGuiCond_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiCond_None);
    QUERY->doc_var(QUERY, "No condition (always set the variable), same as _Always");
    static t_CKINT ImGuiCond_Always = 1;
    QUERY->add_svar(QUERY, "int", "Always", true, &ImGuiCond_Always);
    QUERY->doc_var(QUERY, "No condition (always set the variable), same as _None");
    static t_CKINT ImGuiCond_Once = 2;
    QUERY->add_svar(QUERY, "int", "Once", true, &ImGuiCond_Once);
    QUERY->doc_var(QUERY,
                   "Set the variable once per runtime session (only the first "
                   "call will succeed)");
    static t_CKINT ImGuiCond_FirstUseEver = 4;
    QUERY->add_svar(QUERY, "int", "FirstUseEver", true, &ImGuiCond_FirstUseEver);
    QUERY->doc_var(QUERY,
                   "Set the variable if the object/window has no persistently "
                   "saved data (no entry in .ini file)");
    static t_CKINT ImGuiCond_Appearing = 8;
    QUERY->add_svar(QUERY, "int", "Appearing", true, &ImGuiCond_Appearing);
    QUERY->doc_var(QUERY,
                   "Set the variable if the object/window is appearing after "
                   "being hidden/inactive (or the first time)");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_Color", "Object");
    QUERY->doc_class(QUERY,
                     "Enumeration for PushStyleColor() / PopStyleColor().\n A "
                     "color identifier for styling");
    static t_CKINT ImGuiCol_Text = 0;
    QUERY->add_svar(QUERY, "int", "Text", true, &ImGuiCol_Text);
    static t_CKINT ImGuiCol_TextDisabled = 1;
    QUERY->add_svar(QUERY, "int", "TextDisabled", true, &ImGuiCol_TextDisabled);
    static t_CKINT ImGuiCol_WindowBg = 2;
    QUERY->add_svar(QUERY, "int", "WindowBg", true, &ImGuiCol_WindowBg);
    QUERY->doc_var(QUERY, "Background of normal windows");
    static t_CKINT ImGuiCol_ChildBg = 3;
    QUERY->add_svar(QUERY, "int", "ChildBg", true, &ImGuiCol_ChildBg);
    QUERY->doc_var(QUERY, "Background of child windows");
    static t_CKINT ImGuiCol_PopupBg = 4;
    QUERY->add_svar(QUERY, "int", "PopupBg", true, &ImGuiCol_PopupBg);
    QUERY->doc_var(QUERY, "Background of popups, menus, tooltips windows");
    static t_CKINT ImGuiCol_Border = 5;
    QUERY->add_svar(QUERY, "int", "Border", true, &ImGuiCol_Border);
    static t_CKINT ImGuiCol_BorderShadow = 6;
    QUERY->add_svar(QUERY, "int", "BorderShadow", true, &ImGuiCol_BorderShadow);
    static t_CKINT ImGuiCol_FrameBg = 7;
    QUERY->add_svar(QUERY, "int", "FrameBg", true, &ImGuiCol_FrameBg);
    QUERY->doc_var(QUERY,
                   "Background of checkbox, radio button, plot, slider, text input");
    static t_CKINT ImGuiCol_FrameBgHovered = 8;
    QUERY->add_svar(QUERY, "int", "FrameBgHovered", true, &ImGuiCol_FrameBgHovered);
    static t_CKINT ImGuiCol_FrameBgActive = 9;
    QUERY->add_svar(QUERY, "int", "FrameBgActive", true, &ImGuiCol_FrameBgActive);
    static t_CKINT ImGuiCol_TitleBg = 10;
    QUERY->add_svar(QUERY, "int", "TitleBg", true, &ImGuiCol_TitleBg);
    QUERY->doc_var(QUERY, "Title bar");
    static t_CKINT ImGuiCol_TitleBgActive = 11;
    QUERY->add_svar(QUERY, "int", "TitleBgActive", true, &ImGuiCol_TitleBgActive);
    QUERY->doc_var(QUERY, "Title bar when focused");
    static t_CKINT ImGuiCol_TitleBgCollapsed = 12;
    QUERY->add_svar(QUERY, "int", "TitleBgCollapsed", true, &ImGuiCol_TitleBgCollapsed);
    QUERY->doc_var(QUERY, "Title bar when collapsed");
    static t_CKINT ImGuiCol_MenuBarBg = 13;
    QUERY->add_svar(QUERY, "int", "MenuBarBg", true, &ImGuiCol_MenuBarBg);
    static t_CKINT ImGuiCol_ScrollbarBg = 14;
    QUERY->add_svar(QUERY, "int", "ScrollbarBg", true, &ImGuiCol_ScrollbarBg);
    static t_CKINT ImGuiCol_ScrollbarGrab = 15;
    QUERY->add_svar(QUERY, "int", "ScrollbarGrab", true, &ImGuiCol_ScrollbarGrab);
    static t_CKINT ImGuiCol_ScrollbarGrabHovered = 16;
    QUERY->add_svar(QUERY, "int", "ScrollbarGrabHovered", true,
                    &ImGuiCol_ScrollbarGrabHovered);
    static t_CKINT ImGuiCol_ScrollbarGrabActive = 17;
    QUERY->add_svar(QUERY, "int", "ScrollbarGrabActive", true,
                    &ImGuiCol_ScrollbarGrabActive);
    static t_CKINT ImGuiCol_CheckMark = 18;
    QUERY->add_svar(QUERY, "int", "CheckMark", true, &ImGuiCol_CheckMark);
    QUERY->doc_var(QUERY, "Checkbox tick and RadioButton circle");
    static t_CKINT ImGuiCol_SliderGrab = 19;
    QUERY->add_svar(QUERY, "int", "SliderGrab", true, &ImGuiCol_SliderGrab);
    static t_CKINT ImGuiCol_SliderGrabActive = 20;
    QUERY->add_svar(QUERY, "int", "SliderGrabActive", true, &ImGuiCol_SliderGrabActive);
    static t_CKINT ImGuiCol_Button = 21;
    QUERY->add_svar(QUERY, "int", "Button", true, &ImGuiCol_Button);
    static t_CKINT ImGuiCol_ButtonHovered = 22;
    QUERY->add_svar(QUERY, "int", "ButtonHovered", true, &ImGuiCol_ButtonHovered);
    static t_CKINT ImGuiCol_ButtonActive = 23;
    QUERY->add_svar(QUERY, "int", "ButtonActive", true, &ImGuiCol_ButtonActive);
    static t_CKINT ImGuiCol_Header = 24;
    QUERY->add_svar(QUERY, "int", "Header", true, &ImGuiCol_Header);
    QUERY->doc_var(QUERY,
                   "Header* colors are used for CollapsingHeader, TreeNode, "
                   "Selectable, MenuItem");
    static t_CKINT ImGuiCol_HeaderHovered = 25;
    QUERY->add_svar(QUERY, "int", "HeaderHovered", true, &ImGuiCol_HeaderHovered);
    static t_CKINT ImGuiCol_HeaderActive = 26;
    QUERY->add_svar(QUERY, "int", "HeaderActive", true, &ImGuiCol_HeaderActive);
    static t_CKINT ImGuiCol_Separator = 27;
    QUERY->add_svar(QUERY, "int", "Separator", true, &ImGuiCol_Separator);
    static t_CKINT ImGuiCol_SeparatorHovered = 28;
    QUERY->add_svar(QUERY, "int", "SeparatorHovered", true, &ImGuiCol_SeparatorHovered);
    static t_CKINT ImGuiCol_SeparatorActive = 29;
    QUERY->add_svar(QUERY, "int", "SeparatorActive", true, &ImGuiCol_SeparatorActive);
    static t_CKINT ImGuiCol_ResizeGrip = 30;
    QUERY->add_svar(QUERY, "int", "ResizeGrip", true, &ImGuiCol_ResizeGrip);
    QUERY->doc_var(QUERY,
                   "Resize grip in lower-right and lower-left corners of windows.");
    static t_CKINT ImGuiCol_ResizeGripHovered = 31;
    QUERY->add_svar(QUERY, "int", "ResizeGripHovered", true,
                    &ImGuiCol_ResizeGripHovered);
    static t_CKINT ImGuiCol_ResizeGripActive = 32;
    QUERY->add_svar(QUERY, "int", "ResizeGripActive", true, &ImGuiCol_ResizeGripActive);
    static t_CKINT ImGuiCol_Tab = 33;
    QUERY->add_svar(QUERY, "int", "Tab", true, &ImGuiCol_Tab);
    QUERY->doc_var(QUERY, "TabItem in a TabBar");
    static t_CKINT ImGuiCol_TabHovered = 34;
    QUERY->add_svar(QUERY, "int", "TabHovered", true, &ImGuiCol_TabHovered);
    static t_CKINT ImGuiCol_TabActive = 35;
    QUERY->add_svar(QUERY, "int", "TabActive", true, &ImGuiCol_TabActive);
    static t_CKINT ImGuiCol_TabUnfocused = 36;
    QUERY->add_svar(QUERY, "int", "TabUnfocused", true, &ImGuiCol_TabUnfocused);
    static t_CKINT ImGuiCol_TabUnfocusedActive = 37;
    QUERY->add_svar(QUERY, "int", "TabUnfocusedActive", true,
                    &ImGuiCol_TabUnfocusedActive);
    static t_CKINT ImGuiCol_DockingPreview = 38;
    QUERY->add_svar(QUERY, "int", "DockingPreview", true, &ImGuiCol_DockingPreview);
    QUERY->doc_var(QUERY, "Preview overlay color when about to docking something");
    static t_CKINT ImGuiCol_DockingEmptyBg = 39;
    QUERY->add_svar(QUERY, "int", "DockingEmptyBg", true, &ImGuiCol_DockingEmptyBg);
    QUERY->doc_var(QUERY,
                   "Background color for empty node (e.g. CentralNode with no "
                   "window docked into it)");
    static t_CKINT ImGuiCol_PlotLines = 40;
    QUERY->add_svar(QUERY, "int", "PlotLines", true, &ImGuiCol_PlotLines);
    static t_CKINT ImGuiCol_PlotLinesHovered = 41;
    QUERY->add_svar(QUERY, "int", "PlotLinesHovered", true, &ImGuiCol_PlotLinesHovered);
    static t_CKINT ImGuiCol_PlotHistogram = 42;
    QUERY->add_svar(QUERY, "int", "PlotHistogram", true, &ImGuiCol_PlotHistogram);
    static t_CKINT ImGuiCol_PlotHistogramHovered = 43;
    QUERY->add_svar(QUERY, "int", "PlotHistogramHovered", true,
                    &ImGuiCol_PlotHistogramHovered);
    static t_CKINT ImGuiCol_TableHeaderBg = 44;
    QUERY->add_svar(QUERY, "int", "TableHeaderBg", true, &ImGuiCol_TableHeaderBg);
    QUERY->doc_var(QUERY, "Table header background");
    static t_CKINT ImGuiCol_TableBorderStrong = 45;
    QUERY->add_svar(QUERY, "int", "TableBorderStrong", true,
                    &ImGuiCol_TableBorderStrong);
    QUERY->doc_var(QUERY,
                   "Table outer and header borders (prefer using Alpha=1.0 here)");
    static t_CKINT ImGuiCol_TableBorderLight = 46;
    QUERY->add_svar(QUERY, "int", "TableBorderLight", true, &ImGuiCol_TableBorderLight);
    QUERY->doc_var(QUERY, "Table inner borders (prefer using Alpha=1.0 here)");
    static t_CKINT ImGuiCol_TableRowBg = 47;
    QUERY->add_svar(QUERY, "int", "TableRowBg", true, &ImGuiCol_TableRowBg);
    QUERY->doc_var(QUERY, "Table row background (even rows)");
    static t_CKINT ImGuiCol_TableRowBgAlt = 48;
    QUERY->add_svar(QUERY, "int", "TableRowBgAlt", true, &ImGuiCol_TableRowBgAlt);
    QUERY->doc_var(QUERY, "Table row background (odd rows)");
    static t_CKINT ImGuiCol_TextSelectedBg = 49;
    QUERY->add_svar(QUERY, "int", "TextSelectedBg", true, &ImGuiCol_TextSelectedBg);
    static t_CKINT ImGuiCol_DragDropTarget = 50;
    QUERY->add_svar(QUERY, "int", "DragDropTarget", true, &ImGuiCol_DragDropTarget);
    QUERY->doc_var(QUERY, "Rectangle highlighting a drop target");
    static t_CKINT ImGuiCol_NavHighlight = 51;
    QUERY->add_svar(QUERY, "int", "NavHighlight", true, &ImGuiCol_NavHighlight);
    QUERY->doc_var(QUERY, "Gamepad/keyboard: current highlighted item");
    static t_CKINT ImGuiCol_NavWindowingHighlight = 52;
    QUERY->add_svar(QUERY, "int", "NavWindowingHighlight", true,
                    &ImGuiCol_NavWindowingHighlight);
    QUERY->doc_var(QUERY, "Highlight window when using CTRL+TAB");
    static t_CKINT ImGuiCol_NavWindowingDimBg = 53;
    QUERY->add_svar(QUERY, "int", "NavWindowingDimBg", true,
                    &ImGuiCol_NavWindowingDimBg);
    QUERY->doc_var(QUERY,
                   "Darken/colorize entire screen behind the CTRL+TAB window "
                   "list, when active");
    static t_CKINT ImGuiCol_ModalWindowDimBg = 54;
    QUERY->add_svar(QUERY, "int", "ModalWindowDimBg", true, &ImGuiCol_ModalWindowDimBg);
    QUERY->doc_var(QUERY,
                   "Darken/colorize entire screen behind a modal window, when "
                   "one is active");
    static t_CKINT ImGuiCol_COUNT = 55;
    QUERY->add_svar(QUERY, "int", "COUNT", true, &ImGuiCol_COUNT);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_StyleVar", "Object");
    QUERY->doc_class(
      QUERY,
      "Enumeration for PushStyleVar() / PopStyleVar() to temporarily modify "
      "the ImGuiStyle structure..\n- The enum only refers to fields of "
      "ImGuiStyle which makes sense to be pushed/popped inside UI "
      "code..\nDuring initialization or between frames, feel free to just poke "
      "into ImGuiStyle directly..\n- Tip: Use your programming IDE navigation "
      "facilities on the names in the _second column_ below to find the actual "
      "members and their description..\n- In Visual Studio: CTRL+comma "
      "(\"Edit.GoToAll\") can follow symbols inside comments, whereas CTRL+F12 "
      "(\"Edit.GoToImplementation\") cannot..\n- In Visual Studio w/ Visual "
      "Assist installed: ALT+G (\"VAssistX.GoToImplementation\") can also "
      "follow symbols inside comments..\n- In VS Code, CLion, etc.: CTRL+click "
      "can follow symbols inside comments..\n- When changing this enum, you "
      "need to update the associated internal table GStyleVarInfo[] "
      "accordingly. This is where we link enum values to members "
      "offset/type..\n");
    static t_CKINT ImGuiStyleVar_Alpha = 0;
    QUERY->add_svar(QUERY, "int", "Alpha", true, &ImGuiStyleVar_Alpha);
    QUERY->doc_var(QUERY, "float     Alpha");
    static t_CKINT ImGuiStyleVar_DisabledAlpha = 1;
    QUERY->add_svar(QUERY, "int", "DisabledAlpha", true, &ImGuiStyleVar_DisabledAlpha);
    QUERY->doc_var(QUERY, "float     DisabledAlpha");
    static t_CKINT ImGuiStyleVar_WindowPadding = 2;
    QUERY->add_svar(QUERY, "int", "WindowPadding", true, &ImGuiStyleVar_WindowPadding);
    QUERY->doc_var(QUERY, "ImVec2    WindowPadding");
    static t_CKINT ImGuiStyleVar_WindowRounding = 3;
    QUERY->add_svar(QUERY, "int", "WindowRounding", true,
                    &ImGuiStyleVar_WindowRounding);
    QUERY->doc_var(QUERY, "float     WindowRounding");
    static t_CKINT ImGuiStyleVar_WindowBorderSize = 4;
    QUERY->add_svar(QUERY, "int", "WindowBorderSize", true,
                    &ImGuiStyleVar_WindowBorderSize);
    QUERY->doc_var(QUERY, "float     WindowBorderSize");
    static t_CKINT ImGuiStyleVar_WindowMinSize = 5;
    QUERY->add_svar(QUERY, "int", "WindowMinSize", true, &ImGuiStyleVar_WindowMinSize);
    QUERY->doc_var(QUERY, "ImVec2    WindowMinSize");
    static t_CKINT ImGuiStyleVar_WindowTitleAlign = 6;
    QUERY->add_svar(QUERY, "int", "WindowTitleAlign", true,
                    &ImGuiStyleVar_WindowTitleAlign);
    QUERY->doc_var(QUERY, "ImVec2    WindowTitleAlign");
    static t_CKINT ImGuiStyleVar_ChildRounding = 7;
    QUERY->add_svar(QUERY, "int", "ChildRounding", true, &ImGuiStyleVar_ChildRounding);
    QUERY->doc_var(QUERY, "float     ChildRounding");
    static t_CKINT ImGuiStyleVar_ChildBorderSize = 8;
    QUERY->add_svar(QUERY, "int", "ChildBorderSize", true,
                    &ImGuiStyleVar_ChildBorderSize);
    QUERY->doc_var(QUERY, "float     ChildBorderSize");
    static t_CKINT ImGuiStyleVar_PopupRounding = 9;
    QUERY->add_svar(QUERY, "int", "PopupRounding", true, &ImGuiStyleVar_PopupRounding);
    QUERY->doc_var(QUERY, "float     PopupRounding");
    static t_CKINT ImGuiStyleVar_PopupBorderSize = 10;
    QUERY->add_svar(QUERY, "int", "PopupBorderSize", true,
                    &ImGuiStyleVar_PopupBorderSize);
    QUERY->doc_var(QUERY, "float     PopupBorderSize");
    static t_CKINT ImGuiStyleVar_FramePadding = 11;
    QUERY->add_svar(QUERY, "int", "FramePadding", true, &ImGuiStyleVar_FramePadding);
    QUERY->doc_var(QUERY, "ImVec2    FramePadding");
    static t_CKINT ImGuiStyleVar_FrameRounding = 12;
    QUERY->add_svar(QUERY, "int", "FrameRounding", true, &ImGuiStyleVar_FrameRounding);
    QUERY->doc_var(QUERY, "float     FrameRounding");
    static t_CKINT ImGuiStyleVar_FrameBorderSize = 13;
    QUERY->add_svar(QUERY, "int", "FrameBorderSize", true,
                    &ImGuiStyleVar_FrameBorderSize);
    QUERY->doc_var(QUERY, "float     FrameBorderSize");
    static t_CKINT ImGuiStyleVar_ItemSpacing = 14;
    QUERY->add_svar(QUERY, "int", "ItemSpacing", true, &ImGuiStyleVar_ItemSpacing);
    QUERY->doc_var(QUERY, "ImVec2    ItemSpacing");
    static t_CKINT ImGuiStyleVar_ItemInnerSpacing = 15;
    QUERY->add_svar(QUERY, "int", "ItemInnerSpacing", true,
                    &ImGuiStyleVar_ItemInnerSpacing);
    QUERY->doc_var(QUERY, "ImVec2    ItemInnerSpacing");
    static t_CKINT ImGuiStyleVar_IndentSpacing = 16;
    QUERY->add_svar(QUERY, "int", "IndentSpacing", true, &ImGuiStyleVar_IndentSpacing);
    QUERY->doc_var(QUERY, "float     IndentSpacing");
    static t_CKINT ImGuiStyleVar_CellPadding = 17;
    QUERY->add_svar(QUERY, "int", "CellPadding", true, &ImGuiStyleVar_CellPadding);
    QUERY->doc_var(QUERY, "ImVec2    CellPadding");
    static t_CKINT ImGuiStyleVar_ScrollbarSize = 18;
    QUERY->add_svar(QUERY, "int", "ScrollbarSize", true, &ImGuiStyleVar_ScrollbarSize);
    QUERY->doc_var(QUERY, "float     ScrollbarSize");
    static t_CKINT ImGuiStyleVar_ScrollbarRounding = 19;
    QUERY->add_svar(QUERY, "int", "ScrollbarRounding", true,
                    &ImGuiStyleVar_ScrollbarRounding);
    QUERY->doc_var(QUERY, "float     ScrollbarRounding");
    static t_CKINT ImGuiStyleVar_GrabMinSize = 20;
    QUERY->add_svar(QUERY, "int", "GrabMinSize", true, &ImGuiStyleVar_GrabMinSize);
    QUERY->doc_var(QUERY, "float     GrabMinSize");
    static t_CKINT ImGuiStyleVar_GrabRounding = 21;
    QUERY->add_svar(QUERY, "int", "GrabRounding", true, &ImGuiStyleVar_GrabRounding);
    QUERY->doc_var(QUERY, "float     GrabRounding");
    static t_CKINT ImGuiStyleVar_TabRounding = 22;
    QUERY->add_svar(QUERY, "int", "TabRounding", true, &ImGuiStyleVar_TabRounding);
    QUERY->doc_var(QUERY, "float     TabRounding");
    static t_CKINT ImGuiStyleVar_TabBorderSize = 23;
    QUERY->add_svar(QUERY, "int", "TabBorderSize", true, &ImGuiStyleVar_TabBorderSize);
    QUERY->doc_var(QUERY, "float     TabBorderSize");
    static t_CKINT ImGuiStyleVar_TabBarBorderSize = 24;
    QUERY->add_svar(QUERY, "int", "TabBarBorderSize", true,
                    &ImGuiStyleVar_TabBarBorderSize);
    QUERY->doc_var(QUERY, "float     TabBarBorderSize");
    static t_CKINT ImGuiStyleVar_TableAngledHeadersAngle = 25;
    QUERY->add_svar(QUERY, "int", "TableAngledHeadersAngle", true,
                    &ImGuiStyleVar_TableAngledHeadersAngle);
    QUERY->doc_var(QUERY, "float     TableAngledHeadersAngle");
    static t_CKINT ImGuiStyleVar_TableAngledHeadersTextAlign = 26;
    QUERY->add_svar(QUERY, "int", "TableAngledHeadersTextAlign", true,
                    &ImGuiStyleVar_TableAngledHeadersTextAlign);
    QUERY->doc_var(QUERY, "ImVec2  TableAngledHeadersTextAlign");
    static t_CKINT ImGuiStyleVar_ButtonTextAlign = 27;
    QUERY->add_svar(QUERY, "int", "ButtonTextAlign", true,
                    &ImGuiStyleVar_ButtonTextAlign);
    QUERY->doc_var(QUERY, "ImVec2    ButtonTextAlign");
    static t_CKINT ImGuiStyleVar_SelectableTextAlign = 28;
    QUERY->add_svar(QUERY, "int", "SelectableTextAlign", true,
                    &ImGuiStyleVar_SelectableTextAlign);
    QUERY->doc_var(QUERY, "ImVec2    SelectableTextAlign");
    static t_CKINT ImGuiStyleVar_SeparatorTextBorderSize = 29;
    QUERY->add_svar(QUERY, "int", "SeparatorTextBorderSize", true,
                    &ImGuiStyleVar_SeparatorTextBorderSize);
    QUERY->doc_var(QUERY, "float     SeparatorTextBorderSize");
    static t_CKINT ImGuiStyleVar_SeparatorTextAlign = 30;
    QUERY->add_svar(QUERY, "int", "SeparatorTextAlign", true,
                    &ImGuiStyleVar_SeparatorTextAlign);
    QUERY->doc_var(QUERY, "ImVec2    SeparatorTextAlign");
    static t_CKINT ImGuiStyleVar_SeparatorTextPadding = 31;
    QUERY->add_svar(QUERY, "int", "SeparatorTextPadding", true,
                    &ImGuiStyleVar_SeparatorTextPadding);
    QUERY->doc_var(QUERY, "ImVec2    SeparatorTextPadding");
    static t_CKINT ImGuiStyleVar_DockingSeparatorSize = 32;
    QUERY->add_svar(QUERY, "int", "DockingSeparatorSize", true,
                    &ImGuiStyleVar_DockingSeparatorSize);
    QUERY->doc_var(QUERY, "float     DockingSeparatorSize");
    static t_CKINT ImGuiStyleVar_COUNT = 33;
    QUERY->add_svar(QUERY, "int", "COUNT", true, &ImGuiStyleVar_COUNT);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_ButtonFlags", "Object");
    QUERY->doc_class(QUERY,
                     "Flags for InvisibleButton() [extended in imgui_internal.h].\n");
    static t_CKINT ImGuiButtonFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiButtonFlags_None);
    static t_CKINT ImGuiButtonFlags_MouseButtonLeft = 1;
    QUERY->add_svar(QUERY, "int", "MouseButtonLeft", true,
                    &ImGuiButtonFlags_MouseButtonLeft);
    QUERY->doc_var(QUERY, "React on left mouse button (default)");
    static t_CKINT ImGuiButtonFlags_MouseButtonRight = 2;
    QUERY->add_svar(QUERY, "int", "MouseButtonRight", true,
                    &ImGuiButtonFlags_MouseButtonRight);
    QUERY->doc_var(QUERY, "React on right mouse button");
    static t_CKINT ImGuiButtonFlags_MouseButtonMiddle = 4;
    QUERY->add_svar(QUERY, "int", "MouseButtonMiddle", true,
                    &ImGuiButtonFlags_MouseButtonMiddle);
    QUERY->doc_var(QUERY, "React on center mouse button");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_Direction", "Object");
    QUERY->doc_class(QUERY, "A cardinal direction.\n");
    static t_CKINT ImGuiDir_None = -1;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiDir_None);
    static t_CKINT ImGuiDir_Left = 0;
    QUERY->add_svar(QUERY, "int", "Left", true, &ImGuiDir_Left);
    static t_CKINT ImGuiDir_Right = 1;
    QUERY->add_svar(QUERY, "int", "Right", true, &ImGuiDir_Right);
    static t_CKINT ImGuiDir_Up = 2;
    QUERY->add_svar(QUERY, "int", "Up", true, &ImGuiDir_Up);
    static t_CKINT ImGuiDir_Down = 3;
    QUERY->add_svar(QUERY, "int", "Down", true, &ImGuiDir_Down);
    static t_CKINT ImGuiDir_COUNT = 4;
    QUERY->add_svar(QUERY, "int", "COUNT", true, &ImGuiDir_COUNT);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_ComboFlags", "Object");
    QUERY->doc_class(QUERY, "Flags for ImGui::BeginCombo().\n");
    static t_CKINT ImGuiComboFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiComboFlags_None);
    static t_CKINT ImGuiComboFlags_PopupAlignLeft = 1;
    QUERY->add_svar(QUERY, "int", "PopupAlignLeft", true,
                    &ImGuiComboFlags_PopupAlignLeft);
    QUERY->doc_var(QUERY, "Align the popup toward the left by default");
    static t_CKINT ImGuiComboFlags_HeightSmall = 2;
    QUERY->add_svar(QUERY, "int", "HeightSmall", true, &ImGuiComboFlags_HeightSmall);
    QUERY->doc_var(QUERY,
                   "Max ~4 items visible. Tip: If you want your combo popup to be a "
                   "specific size you can use SetNextWindowSizeConstraints() prior to "
                   "calling BeginCombo()");
    static t_CKINT ImGuiComboFlags_HeightRegular = 4;
    QUERY->add_svar(QUERY, "int", "HeightRegular", true,
                    &ImGuiComboFlags_HeightRegular);
    QUERY->doc_var(QUERY, "Max ~8 items visible (default)");
    static t_CKINT ImGuiComboFlags_HeightLarge = 8;
    QUERY->add_svar(QUERY, "int", "HeightLarge", true, &ImGuiComboFlags_HeightLarge);
    QUERY->doc_var(QUERY, "Max ~20 items visible");
    static t_CKINT ImGuiComboFlags_HeightLargest = 16;
    QUERY->add_svar(QUERY, "int", "HeightLargest", true,
                    &ImGuiComboFlags_HeightLargest);
    QUERY->doc_var(QUERY, "As many fitting items as possible");
    static t_CKINT ImGuiComboFlags_NoArrowButton = 32;
    QUERY->add_svar(QUERY, "int", "NoArrowButton", true,
                    &ImGuiComboFlags_NoArrowButton);
    QUERY->doc_var(QUERY, "Display on the preview box without the square arrow button");
    static t_CKINT ImGuiComboFlags_NoPreview = 64;
    QUERY->add_svar(QUERY, "int", "NoPreview", true, &ImGuiComboFlags_NoPreview);
    QUERY->doc_var(QUERY, "Display only a square arrow button");
    static t_CKINT ImGuiComboFlags_WidthFitPreview = 128;
    QUERY->add_svar(QUERY, "int", "WidthFitPreview", true,
                    &ImGuiComboFlags_WidthFitPreview);
    QUERY->doc_var(QUERY, "Width dynamically calculated from preview contents");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_SliderFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for DragFloat(), DragInt(), SliderFloat(), SliderInt() etc..\nWe "
      "use the same sets of flags for DragXXX() and SliderXXX() functions as "
      "the features are the same and it makes it easier to swap them..\n(Those "
      "are per-item flags. There are shared flags in ImGuiIO: "
      "io.ConfigDragClickToInputText).\n");
    static t_CKINT ImGuiSliderFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiSliderFlags_None);
    static t_CKINT ImGuiSliderFlags_AlwaysClamp = 16;
    QUERY->add_svar(QUERY, "int", "AlwaysClamp", true, &ImGuiSliderFlags_AlwaysClamp);
    QUERY->doc_var(
      QUERY,
      "Clamp value to min/max bounds when input manually with CTRL+Click. By "
      "default CTRL+Click allows going out of bounds.");
    static t_CKINT ImGuiSliderFlags_Logarithmic = 32;
    QUERY->add_svar(QUERY, "int", "Logarithmic", true, &ImGuiSliderFlags_Logarithmic);
    QUERY->doc_var(QUERY,
                   "Make the widget logarithmic (linear otherwise). Consider "
                   "using ImGuiSliderFlags_NoRoundToFormat with this if using "
                   "a format-string with small amount of digits.");
    static t_CKINT ImGuiSliderFlags_NoRoundToFormat = 64;
    QUERY->add_svar(QUERY, "int", "NoRoundToFormat", true,
                    &ImGuiSliderFlags_NoRoundToFormat);
    QUERY->doc_var(
      QUERY,
      "Disable rounding underlying value to match precision of the display "
      "format string (e.g. %.3f values are rounded to those 3 digits)");
    static t_CKINT ImGuiSliderFlags_NoInput = 128;
    QUERY->add_svar(QUERY, "int", "NoInput", true, &ImGuiSliderFlags_NoInput);
    QUERY->doc_var(QUERY,
                   "Disable CTRL+Click or Enter key allowing to input text "
                   "directly into the widget");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_InputTextFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for ImGui::InputText().\n(Those are per-item flags. There are "
      "shared flags in ImGuiIO: io.ConfigInputTextCursorBlink and "
      "io.ConfigInputTextEnterKeepActive).\n");
    static t_CKINT ImGuiInputTextFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiInputTextFlags_None);
    static t_CKINT ImGuiInputTextFlags_CharsDecimal = 1;
    QUERY->add_svar(QUERY, "int", "CharsDecimal", true,
                    &ImGuiInputTextFlags_CharsDecimal);
    QUERY->doc_var(QUERY, "Allow 0123456789.+-*/");
    static t_CKINT ImGuiInputTextFlags_CharsHexadecimal = 2;
    QUERY->add_svar(QUERY, "int", "CharsHexadecimal", true,
                    &ImGuiInputTextFlags_CharsHexadecimal);
    QUERY->doc_var(QUERY, "Allow 0123456789ABCDEFabcdef");
    static t_CKINT ImGuiInputTextFlags_CharsUppercase = 4;
    QUERY->add_svar(QUERY, "int", "CharsUppercase", true,
                    &ImGuiInputTextFlags_CharsUppercase);
    QUERY->doc_var(QUERY, "Turn a..z into A..Z");
    static t_CKINT ImGuiInputTextFlags_CharsNoBlank = 8;
    QUERY->add_svar(QUERY, "int", "CharsNoBlank", true,
                    &ImGuiInputTextFlags_CharsNoBlank);
    QUERY->doc_var(QUERY, "Filter out spaces, tabs");
    static t_CKINT ImGuiInputTextFlags_AutoSelectAll = 16;
    QUERY->add_svar(QUERY, "int", "AutoSelectAll", true,
                    &ImGuiInputTextFlags_AutoSelectAll);
    QUERY->doc_var(QUERY, "Select entire text when first taking mouse focus");
    static t_CKINT ImGuiInputTextFlags_EnterReturnsTrue = 32;
    QUERY->add_svar(QUERY, "int", "EnterReturnsTrue", true,
                    &ImGuiInputTextFlags_EnterReturnsTrue);
    QUERY->doc_var(QUERY,
                   "Return 'true' when Enter is pressed (as opposed to every "
                   "time the value was modified). Consider looking at the "
                   "IsItemDeactivatedAfterEdit() function.");
    static t_CKINT ImGuiInputTextFlags_CallbackCompletion = 64;
    QUERY->add_svar(QUERY, "int", "CallbackCompletion", true,
                    &ImGuiInputTextFlags_CallbackCompletion);
    QUERY->doc_var(QUERY, "Callback on pressing TAB (for completion handling)");
    static t_CKINT ImGuiInputTextFlags_CallbackHistory = 128;
    QUERY->add_svar(QUERY, "int", "CallbackHistory", true,
                    &ImGuiInputTextFlags_CallbackHistory);
    QUERY->doc_var(QUERY, "Callback on pressing Up/Down arrows (for history handling)");
    static t_CKINT ImGuiInputTextFlags_CallbackAlways = 256;
    QUERY->add_svar(QUERY, "int", "CallbackAlways", true,
                    &ImGuiInputTextFlags_CallbackAlways);
    QUERY->doc_var(QUERY,
                   "Callback on each iteration. User code may query cursor "
                   "position, modify text buffer.");
    static t_CKINT ImGuiInputTextFlags_CallbackCharFilter = 512;
    QUERY->add_svar(QUERY, "int", "CallbackCharFilter", true,
                    &ImGuiInputTextFlags_CallbackCharFilter);
    QUERY->doc_var(
      QUERY,
      "Callback on character inputs to replace or discard them. Modify "
      "'EventChar' to replace or discard, or return 1 in callback to discard.");
    static t_CKINT ImGuiInputTextFlags_AllowTabInput = 1024;
    QUERY->add_svar(QUERY, "int", "AllowTabInput", true,
                    &ImGuiInputTextFlags_AllowTabInput);
    QUERY->doc_var(QUERY, "Pressing TAB input a '\\t' character into the text field");
    static t_CKINT ImGuiInputTextFlags_CtrlEnterForNewLine = 2048;
    QUERY->add_svar(QUERY, "int", "CtrlEnterForNewLine", true,
                    &ImGuiInputTextFlags_CtrlEnterForNewLine);
    QUERY->doc_var(
      QUERY,
      "In multi-line mode, unfocus with Enter, add new line with Ctrl+Enter "
      "(default is opposite: unfocus with Ctrl+Enter, add line with Enter).");
    static t_CKINT ImGuiInputTextFlags_NoHorizontalScroll = 4096;
    QUERY->add_svar(QUERY, "int", "NoHorizontalScroll", true,
                    &ImGuiInputTextFlags_NoHorizontalScroll);
    QUERY->doc_var(QUERY, "Disable following the cursor horizontally");
    static t_CKINT ImGuiInputTextFlags_AlwaysOverwrite = 8192;
    QUERY->add_svar(QUERY, "int", "AlwaysOverwrite", true,
                    &ImGuiInputTextFlags_AlwaysOverwrite);
    QUERY->doc_var(QUERY, "Overwrite mode");
    static t_CKINT ImGuiInputTextFlags_ReadOnly = 16384;
    QUERY->add_svar(QUERY, "int", "ReadOnly", true, &ImGuiInputTextFlags_ReadOnly);
    QUERY->doc_var(QUERY, "Read-only mode");
    static t_CKINT ImGuiInputTextFlags_Password = 32768;
    QUERY->add_svar(QUERY, "int", "Password", true, &ImGuiInputTextFlags_Password);
    QUERY->doc_var(QUERY, "Password mode, display all characters as '*'");
    static t_CKINT ImGuiInputTextFlags_NoUndoRedo = 65536;
    QUERY->add_svar(QUERY, "int", "NoUndoRedo", true, &ImGuiInputTextFlags_NoUndoRedo);
    QUERY->doc_var(QUERY,
                   "Disable undo/redo. Note that input text owns the text data "
                   "while active, if you want to provide your own undo/redo "
                   "stack you need e.g. to call ClearActiveID().");
    static t_CKINT ImGuiInputTextFlags_CharsScientific = 131072;
    QUERY->add_svar(QUERY, "int", "CharsScientific", true,
                    &ImGuiInputTextFlags_CharsScientific);
    QUERY->doc_var(QUERY, "Allow 0123456789.+-*/eE (Scientific notation input)");
    static t_CKINT ImGuiInputTextFlags_CallbackResize = 262144;
    QUERY->add_svar(QUERY, "int", "CallbackResize", true,
                    &ImGuiInputTextFlags_CallbackResize);
    QUERY->doc_var(
      QUERY,
      "Callback on buffer capacity changes request (beyond 'buf_size' "
      "parameter value), allowing the string to grow. Notify when the string "
      "wants to be resized (for string types which hold a cache of their "
      "Size). You will be provided a new BufSize in the callback and NEED to "
      "honor it. (see misc/cpp/imgui_stdlib.h for an example of using this)");
    static t_CKINT ImGuiInputTextFlags_CallbackEdit = 524288;
    QUERY->add_svar(QUERY, "int", "CallbackEdit", true,
                    &ImGuiInputTextFlags_CallbackEdit);
    QUERY->doc_var(QUERY,
                   "Callback on any edit (note that InputText() already "
                   "returns true on edit, the callback is useful mainly to "
                   "manipulate the underlying buffer while focus is active)");
    static t_CKINT ImGuiInputTextFlags_EscapeClearsAll = 1048576;
    QUERY->add_svar(QUERY, "int", "EscapeClearsAll", true,
                    &ImGuiInputTextFlags_EscapeClearsAll);
    QUERY->doc_var(QUERY,
                   "Escape key clears content if not empty, and deactivate otherwise "
                   "(contrast to default behavior of Escape to revert)");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_ColorEditFlags", "Object");
    QUERY->doc_class(QUERY,
                     "Flags for ColorEdit3() / ColorEdit4() / ColorPicker3() / "
                     "ColorPicker4() / ColorButton().\n");
    static t_CKINT ImGuiColorEditFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiColorEditFlags_None);
    static t_CKINT ImGuiColorEditFlags_NoAlpha = 2;
    QUERY->add_svar(QUERY, "int", "NoAlpha", true, &ImGuiColorEditFlags_NoAlpha);
    QUERY->doc_var(
      QUERY,
      "ColorEdit, ColorPicker, ColorButton: ignore Alpha component (will only "
      "read 3 components from the input pointer).");
    static t_CKINT ImGuiColorEditFlags_NoPicker = 4;
    QUERY->add_svar(QUERY, "int", "NoPicker", true, &ImGuiColorEditFlags_NoPicker);
    QUERY->doc_var(QUERY, "ColorEdit: disable picker when clicking on color square.");
    static t_CKINT ImGuiColorEditFlags_NoOptions = 8;
    QUERY->add_svar(QUERY, "int", "NoOptions", true, &ImGuiColorEditFlags_NoOptions);
    QUERY->doc_var(QUERY,
                   "ColorEdit: disable toggling options menu when "
                   "right-clicking on inputs/small preview.");
    static t_CKINT ImGuiColorEditFlags_NoSmallPreview = 16;
    QUERY->add_svar(QUERY, "int", "NoSmallPreview", true,
                    &ImGuiColorEditFlags_NoSmallPreview);
    QUERY->doc_var(QUERY,
                   "ColorEdit, ColorPicker: disable color square preview next "
                   "to the inputs. (e.g. to show only the inputs)");
    static t_CKINT ImGuiColorEditFlags_NoInputs = 32;
    QUERY->add_svar(QUERY, "int", "NoInputs", true, &ImGuiColorEditFlags_NoInputs);
    QUERY->doc_var(
      QUERY,
      "ColorEdit, ColorPicker: disable inputs sliders/text widgets (e.g. to "
      "show only the small preview color square).");
    static t_CKINT ImGuiColorEditFlags_NoTooltip = 64;
    QUERY->add_svar(QUERY, "int", "NoTooltip", true, &ImGuiColorEditFlags_NoTooltip);
    QUERY->doc_var(QUERY,
                   "ColorEdit, ColorPicker, ColorButton: disable tooltip when "
                   "hovering the preview.");
    static t_CKINT ImGuiColorEditFlags_NoLabel = 128;
    QUERY->add_svar(QUERY, "int", "NoLabel", true, &ImGuiColorEditFlags_NoLabel);
    QUERY->doc_var(
      QUERY,
      "ColorEdit, ColorPicker: disable display of inline text label (the label "
      "is still forwarded to the tooltip and picker).");
    static t_CKINT ImGuiColorEditFlags_NoSidePreview = 256;
    QUERY->add_svar(QUERY, "int", "NoSidePreview", true,
                    &ImGuiColorEditFlags_NoSidePreview);
    QUERY->doc_var(QUERY,
                   "ColorPicker: disable bigger color preview on right side of "
                   "the picker, use small color square preview instead.");
    static t_CKINT ImGuiColorEditFlags_NoDragDrop = 512;
    QUERY->add_svar(QUERY, "int", "NoDragDrop", true, &ImGuiColorEditFlags_NoDragDrop);
    QUERY->doc_var(QUERY,
                   "ColorEdit: disable drag and drop target. ColorButton: "
                   "disable drag and drop source.");
    static t_CKINT ImGuiColorEditFlags_NoBorder = 1024;
    QUERY->add_svar(QUERY, "int", "NoBorder", true, &ImGuiColorEditFlags_NoBorder);
    QUERY->doc_var(QUERY, "ColorButton: disable border (which is enforced by default)");
    static t_CKINT ImGuiColorEditFlags_AlphaBar = 65536;
    QUERY->add_svar(QUERY, "int", "AlphaBar", true, &ImGuiColorEditFlags_AlphaBar);
    QUERY->doc_var(
      QUERY, "ColorEdit, ColorPicker: show vertical alpha bar/gradient in picker.");
    static t_CKINT ImGuiColorEditFlags_AlphaPreview = 131072;
    QUERY->add_svar(QUERY, "int", "AlphaPreview", true,
                    &ImGuiColorEditFlags_AlphaPreview);
    QUERY->doc_var(QUERY,
                   "ColorEdit, ColorPicker, ColorButton: display preview as a "
                   "transparent color over a checkerboard, instead of opaque.");
    static t_CKINT ImGuiColorEditFlags_AlphaPreviewHalf = 262144;
    QUERY->add_svar(QUERY, "int", "AlphaPreviewHalf", true,
                    &ImGuiColorEditFlags_AlphaPreviewHalf);
    QUERY->doc_var(QUERY,
                   "ColorEdit, ColorPicker, ColorButton: display half opaque / "
                   "half checkerboard, instead of opaque.");
    static t_CKINT ImGuiColorEditFlags_HDR = 524288;
    QUERY->add_svar(QUERY, "int", "HDR", true, &ImGuiColorEditFlags_HDR);
    QUERY->doc_var(QUERY,
                   "(WIP) ColorEdit: Currently only disable 0.0f..1.0f limits "
                   "in RGBA edition (note: you probably want to use "
                   "ImGuiColorEditFlags_Float flag as well).");
    static t_CKINT ImGuiColorEditFlags_DisplayRGB = 1048576;
    QUERY->add_svar(QUERY, "int", "DisplayRGB", true, &ImGuiColorEditFlags_DisplayRGB);
    QUERY->doc_var(
      QUERY,
      "[Display]     ColorEdit: override _display_ type among RGB/HSV/Hex. "
      "ColorPicker: select any combination using one or more of RGB/HSV/Hex.");
    static t_CKINT ImGuiColorEditFlags_DisplayHSV = 2097152;
    QUERY->add_svar(QUERY, "int", "DisplayHSV", true, &ImGuiColorEditFlags_DisplayHSV);
    QUERY->doc_var(QUERY, "[Display]     \"");
    static t_CKINT ImGuiColorEditFlags_DisplayHex = 4194304;
    QUERY->add_svar(QUERY, "int", "DisplayHex", true, &ImGuiColorEditFlags_DisplayHex);
    QUERY->doc_var(QUERY, "[Display]     \"");
    static t_CKINT ImGuiColorEditFlags_Uint8 = 8388608;
    QUERY->add_svar(QUERY, "int", "Uint8", true, &ImGuiColorEditFlags_Uint8);
    QUERY->doc_var(QUERY,
                   "[DataType]    ColorEdit, ColorPicker, ColorButton: "
                   "_display_ values formatted as 0..255.");
    static t_CKINT ImGuiColorEditFlags_Float = 16777216;
    QUERY->add_svar(QUERY, "int", "Float", true, &ImGuiColorEditFlags_Float);
    QUERY->doc_var(QUERY,
                   "[DataType]    ColorEdit, ColorPicker, ColorButton: "
                   "_display_ values formatted as 0.0f..1.0f floats instead of "
                   "0..255 integers. No round-trip of value via integers.");
    static t_CKINT ImGuiColorEditFlags_PickerHueBar = 33554432;
    QUERY->add_svar(QUERY, "int", "PickerHueBar", true,
                    &ImGuiColorEditFlags_PickerHueBar);
    QUERY->doc_var(QUERY,
                   "[Picker]      ColorPicker: bar for Hue, rectangle for Sat/Value.");
    static t_CKINT ImGuiColorEditFlags_PickerHueWheel = 67108864;
    QUERY->add_svar(QUERY, "int", "PickerHueWheel", true,
                    &ImGuiColorEditFlags_PickerHueWheel);
    QUERY->doc_var(QUERY,
                   "[Picker]      ColorPicker: wheel for Hue, triangle for Sat/Value.");
    static t_CKINT ImGuiColorEditFlags_InputRGB = 134217728;
    QUERY->add_svar(QUERY, "int", "InputRGB", true, &ImGuiColorEditFlags_InputRGB);
    QUERY->doc_var(QUERY,
                   "[Input]       ColorEdit, ColorPicker: input and output "
                   "data in RGB format.");
    static t_CKINT ImGuiColorEditFlags_InputHSV = 268435456;
    QUERY->add_svar(QUERY, "int", "InputHSV", true, &ImGuiColorEditFlags_InputHSV);
    QUERY->doc_var(QUERY,
                   "[Input]       ColorEdit, ColorPicker: input and output "
                   "data in HSV format.");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_TreeNodeFlags", "Object");
    QUERY->doc_class(QUERY,
                     "Flags for ImGui::TreeNodeEx(), ImGui::CollapsingHeader*().\n");
    static t_CKINT ImGuiTreeNodeFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiTreeNodeFlags_None);
    static t_CKINT ImGuiTreeNodeFlags_Selected = 1;
    QUERY->add_svar(QUERY, "int", "Selected", true, &ImGuiTreeNodeFlags_Selected);
    QUERY->doc_var(QUERY, "Draw as selected");
    static t_CKINT ImGuiTreeNodeFlags_Framed = 2;
    QUERY->add_svar(QUERY, "int", "Framed", true, &ImGuiTreeNodeFlags_Framed);
    QUERY->doc_var(QUERY, "Draw frame with background (e.g. for CollapsingHeader)");
    static t_CKINT ImGuiTreeNodeFlags_AllowOverlap = 4;
    QUERY->add_svar(QUERY, "int", "AllowOverlap", true,
                    &ImGuiTreeNodeFlags_AllowOverlap);
    QUERY->doc_var(QUERY,
                   "Hit testing to allow subsequent widgets to overlap this one");
    static t_CKINT ImGuiTreeNodeFlags_NoTreePushOnOpen = 8;
    QUERY->add_svar(QUERY, "int", "NoTreePushOnOpen", true,
                    &ImGuiTreeNodeFlags_NoTreePushOnOpen);
    QUERY->doc_var(
      QUERY,
      "Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra "
      "indent nor pushing on ID stack");
    static t_CKINT ImGuiTreeNodeFlags_NoAutoOpenOnLog = 16;
    QUERY->add_svar(QUERY, "int", "NoAutoOpenOnLog", true,
                    &ImGuiTreeNodeFlags_NoAutoOpenOnLog);
    QUERY->doc_var(
      QUERY,
      "Don't automatically and temporarily open node when Logging is active "
      "(by default logging will automatically open tree nodes)");
    static t_CKINT ImGuiTreeNodeFlags_DefaultOpen = 32;
    QUERY->add_svar(QUERY, "int", "DefaultOpen", true, &ImGuiTreeNodeFlags_DefaultOpen);
    QUERY->doc_var(QUERY, "Default node to be open");
    static t_CKINT ImGuiTreeNodeFlags_OpenOnDoubleClick = 64;
    QUERY->add_svar(QUERY, "int", "OpenOnDoubleClick", true,
                    &ImGuiTreeNodeFlags_OpenOnDoubleClick);
    QUERY->doc_var(QUERY, "Need double-click to open node");
    static t_CKINT ImGuiTreeNodeFlags_OpenOnArrow = 128;
    QUERY->add_svar(QUERY, "int", "OpenOnArrow", true, &ImGuiTreeNodeFlags_OpenOnArrow);
    QUERY->doc_var(QUERY,
                   "Only open when clicking on the arrow part. If "
                   "ImGuiTreeNodeFlags_OpenOnDoubleClick is also set, "
                   "single-click arrow or double-click all box to open.");
    static t_CKINT ImGuiTreeNodeFlags_Leaf = 256;
    QUERY->add_svar(QUERY, "int", "Leaf", true, &ImGuiTreeNodeFlags_Leaf);
    QUERY->doc_var(QUERY,
                   "No collapsing, no arrow (use as a convenience for leaf nodes).");
    static t_CKINT ImGuiTreeNodeFlags_Bullet = 512;
    QUERY->add_svar(QUERY, "int", "Bullet", true, &ImGuiTreeNodeFlags_Bullet);
    QUERY->doc_var(
      QUERY,
      "Display a bullet instead of arrow. IMPORTANT: node can still be marked "
      "open/close if you don't set the _Leaf flag!");
    static t_CKINT ImGuiTreeNodeFlags_FramePadding = 1024;
    QUERY->add_svar(QUERY, "int", "FramePadding", true,
                    &ImGuiTreeNodeFlags_FramePadding);
    QUERY->doc_var(QUERY,
                   "Use FramePadding (even for an unframed text node) to "
                   "vertically align text baseline to regular widget height. "
                   "Equivalent to calling AlignTextToFramePadding().");
    static t_CKINT ImGuiTreeNodeFlags_SpanAvailWidth = 2048;
    QUERY->add_svar(QUERY, "int", "SpanAvailWidth", true,
                    &ImGuiTreeNodeFlags_SpanAvailWidth);
    QUERY->doc_var(QUERY,
                   "Extend hit box to the right-most edge, even if not framed. "
                   "This is not the default in order to allow adding other "
                   "items on the same line without using AllowOverlap mode.");
    static t_CKINT ImGuiTreeNodeFlags_SpanFullWidth = 4096;
    QUERY->add_svar(QUERY, "int", "SpanFullWidth", true,
                    &ImGuiTreeNodeFlags_SpanFullWidth);
    QUERY->doc_var(QUERY,
                   "Extend hit box to the left-most and right-most edges "
                   "(cover the indent area).");
    static t_CKINT ImGuiTreeNodeFlags_SpanTextWidth = 8192;
    QUERY->add_svar(QUERY, "int", "SpanTextWidth", true,
                    &ImGuiTreeNodeFlags_SpanTextWidth);
    QUERY->doc_var(QUERY,
                   "Narrow hit box + narrow hovering highlight, will only "
                   "cover the label text.");
    static t_CKINT ImGuiTreeNodeFlags_SpanAllColumns = 16384;
    QUERY->add_svar(QUERY, "int", "SpanAllColumns", true,
                    &ImGuiTreeNodeFlags_SpanAllColumns);
    QUERY->doc_var(QUERY,
                   "Frame will span all columns of its container table (text "
                   "will still fit in current column)");
    static t_CKINT ImGuiTreeNodeFlags_NavLeftJumpsBackHere = 32768;
    QUERY->add_svar(QUERY, "int", "NavLeftJumpsBackHere", true,
                    &ImGuiTreeNodeFlags_NavLeftJumpsBackHere);
    QUERY->doc_var(
      QUERY,
      "(WIP) Nav: left direction may move to this TreeNode() from any of its "
      "child (items submitted between TreeNode and TreePop)");
    static t_CKINT ImGuiTreeNodeFlags_CollapsingHeader = 26;
    QUERY->add_svar(QUERY, "int", "CollapsingHeader", true,
                    &ImGuiTreeNodeFlags_CollapsingHeader);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_SelectableFlags", "Object");
    QUERY->doc_class(QUERY, "Flags for ImGui::Selectable().\n");
    static t_CKINT ImGuiSelectableFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiSelectableFlags_None);
    static t_CKINT ImGuiSelectableFlags_DontClosePopups = 1;
    QUERY->add_svar(QUERY, "int", "DontClosePopups", true,
                    &ImGuiSelectableFlags_DontClosePopups);
    QUERY->doc_var(QUERY, "Clicking this doesn't close parent popup window");
    static t_CKINT ImGuiSelectableFlags_SpanAllColumns = 2;
    QUERY->add_svar(QUERY, "int", "SpanAllColumns", true,
                    &ImGuiSelectableFlags_SpanAllColumns);
    QUERY->doc_var(QUERY,
                   "Frame will span all columns of its container table (text "
                   "will still fit in current column)");
    static t_CKINT ImGuiSelectableFlags_AllowDoubleClick = 4;
    QUERY->add_svar(QUERY, "int", "AllowDoubleClick", true,
                    &ImGuiSelectableFlags_AllowDoubleClick);
    QUERY->doc_var(QUERY, "Generate press events on double clicks too");
    static t_CKINT ImGuiSelectableFlags_Disabled = 8;
    QUERY->add_svar(QUERY, "int", "Disabled", true, &ImGuiSelectableFlags_Disabled);
    QUERY->doc_var(QUERY, "Cannot be selected, display grayed out text");
    static t_CKINT ImGuiSelectableFlags_AllowOverlap = 16;
    QUERY->add_svar(QUERY, "int", "AllowOverlap", true,
                    &ImGuiSelectableFlags_AllowOverlap);
    QUERY->doc_var(QUERY,
                   "(WIP) Hit testing to allow subsequent widgets to overlap this one");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_PopupFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for OpenPopup*(), BeginPopupContext*(), IsPopupOpen() "
      "functions..\n- To be backward compatible with older API which took an "
      "'int mouse_button = 1' argument instead of 'ImGuiPopupFlags "
      "flags',.\nwe need to treat small flags values as a mouse button index, "
      "so we encode the mouse button in the first few bits of the flags..\nIt "
      "is therefore guaranteed to be legal to pass a mouse button index in "
      "ImGuiPopupFlags..\n- For the same reason, we exceptionally default the "
      "ImGuiPopupFlags argument of BeginPopupContextXXX functions to 1 instead "
      "of 0..\nIMPORTANT: because the default parameter is 1 "
      "(==ImGuiPopupFlags_MouseButtonRight), if you rely on the default "
      "parameter.\nand want to use another flag, you need to pass in the "
      "ImGuiPopupFlags_MouseButtonRight flag explicitly..\n- Multiple buttons "
      "currently cannot be combined/or-ed in those functions (we could allow "
      "it later)..\n");
    static t_CKINT ImGuiPopupFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiPopupFlags_None);
    static t_CKINT ImGuiPopupFlags_MouseButtonLeft = 0;
    QUERY->add_svar(QUERY, "int", "MouseButtonLeft", true,
                    &ImGuiPopupFlags_MouseButtonLeft);
    QUERY->doc_var(
      QUERY,
      "For BeginPopupContext*(): open on Left Mouse release. Guaranteed to "
      "always be == 0 (same as ImGuiMouseButton_Left)");
    static t_CKINT ImGuiPopupFlags_MouseButtonRight = 1;
    QUERY->add_svar(QUERY, "int", "MouseButtonRight", true,
                    &ImGuiPopupFlags_MouseButtonRight);
    QUERY->doc_var(
      QUERY,
      "For BeginPopupContext*(): open on Right Mouse release. Guaranteed to "
      "always be == 1 (same as ImGuiMouseButton_Right)");
    static t_CKINT ImGuiPopupFlags_MouseButtonMiddle = 2;
    QUERY->add_svar(QUERY, "int", "MouseButtonMiddle", true,
                    &ImGuiPopupFlags_MouseButtonMiddle);
    QUERY->doc_var(
      QUERY,
      "For BeginPopupContext*(): open on Middle Mouse release. Guaranteed to "
      "always be == 2 (same as ImGuiMouseButton_Middle)");
    static t_CKINT ImGuiPopupFlags_NoReopen = 32;
    QUERY->add_svar(QUERY, "int", "NoReopen", true, &ImGuiPopupFlags_NoReopen);
    QUERY->doc_var(QUERY,
                   "For OpenPopup*(), BeginPopupContext*(): don't reopen same popup if "
                   "already open (won't reposition, won't reinitialize navigation)");
    static t_CKINT ImGuiPopupFlags_NoOpenOverExistingPopup = 128;
    QUERY->add_svar(QUERY, "int", "NoOpenOverExistingPopup", true,
                    &ImGuiPopupFlags_NoOpenOverExistingPopup);
    QUERY->doc_var(
      QUERY,
      "For OpenPopup*(), BeginPopupContext*(): don't open if there's already a "
      "popup at the same level of the popup stack");
    static t_CKINT ImGuiPopupFlags_NoOpenOverItems = 256;
    QUERY->add_svar(QUERY, "int", "NoOpenOverItems", true,
                    &ImGuiPopupFlags_NoOpenOverItems);
    QUERY->doc_var(QUERY,
                   "For BeginPopupContextWindow(): don't return true when "
                   "hovering items, only when hovering empty space");
    static t_CKINT ImGuiPopupFlags_AnyPopupId = 1024;
    QUERY->add_svar(QUERY, "int", "AnyPopupId", true, &ImGuiPopupFlags_AnyPopupId);
    QUERY->doc_var(QUERY,
                   "For IsPopupOpen(): ignore the ImGuiID parameter and test "
                   "for any popup.");
    static t_CKINT ImGuiPopupFlags_AnyPopupLevel = 2048;
    QUERY->add_svar(QUERY, "int", "AnyPopupLevel", true,
                    &ImGuiPopupFlags_AnyPopupLevel);
    QUERY->doc_var(QUERY,
                   "For IsPopupOpen(): search/test at any level of the popup "
                   "stack (default test in the current level)");
    static t_CKINT ImGuiPopupFlags_AnyPopup = 3072;
    QUERY->add_svar(QUERY, "int", "AnyPopup", true, &ImGuiPopupFlags_AnyPopup);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_TableFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for ImGui::BeginTable().\n- Important! Sizing policies have "
      "complex and subtle side effects, much more so than you would "
      "expect..\nRead comments/demos carefully + experiment with live demos to "
      "get acquainted with them..\n- The DEFAULT sizing policies are:.\n- "
      "Default to ImGuiTableFlags_SizingFixedFit    if ScrollX is on, or if "
      "host window has ImGuiWindowFlags_AlwaysAutoResize..\n- Default to "
      "ImGuiTableFlags_SizingStretchSame if ScrollX is off..\n- When ScrollX "
      "is off:.\n- Table defaults to ImGuiTableFlags_SizingStretchSame -> all "
      "Columns defaults to ImGuiTableColumnFlags_WidthStretch with same "
      "weight..\n- Columns sizing policy allowed: Stretch (default), "
      "Fixed/Auto..\n- Fixed Columns (if any) will generally obtain their "
      "requested width (unless the table cannot fit them all)..\n- Stretch "
      "Columns will share the remaining width according to their respective "
      "weight..\n- Mixed Fixed/Stretch columns is possible but has various "
      "side-effects on resizing behaviors..\nThe typical use of mixing sizing "
      "policies is: any number of LEADING Fixed columns, followed by one or "
      "two TRAILING Stretch columns..\n(this is because the visible order of "
      "columns have subtle but necessary effects on how they react to manual "
      "resizing)..\n- When ScrollX is on:.\n- Table defaults to "
      "ImGuiTableFlags_SizingFixedFit -> all Columns defaults to "
      "ImGuiTableColumnFlags_WidthFixed.\n- Columns sizing policy allowed: "
      "Fixed/Auto mostly..\n- Fixed Columns can be enlarged as needed. Table "
      "will show a horizontal scrollbar if needed..\n- When using "
      "auto-resizing (non-resizable) fixed columns, querying the content width "
      "to use item right-alignment e.g. SetNextItemWidth(-FLT_MIN) doesn't "
      "make sense, would create a feedback loop..\n- Using Stretch columns "
      "OFTEN DOES NOT MAKE SENSE if ScrollX is on, UNLESS you have specified a "
      "value for 'inner_width' in BeginTable()..\nIf you specify a value for "
      "'inner_width' then effectively the scrolling space is known and Stretch "
      "or mixed Fixed/Stretch columns become meaningful again..\n- Read on "
      "documentation at the top of imgui_tables.cpp for details..\n");
    static t_CKINT ImGuiTableFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiTableFlags_None);
    static t_CKINT ImGuiTableFlags_Resizable = 1;
    QUERY->add_svar(QUERY, "int", "Resizable", true, &ImGuiTableFlags_Resizable);
    QUERY->doc_var(QUERY, "Enable resizing columns.");
    static t_CKINT ImGuiTableFlags_Reorderable = 2;
    QUERY->add_svar(QUERY, "int", "Reorderable", true, &ImGuiTableFlags_Reorderable);
    QUERY->doc_var(QUERY,
                   "Enable reordering columns in header row (need calling "
                   "TableSetupColumn() + TableHeadersRow() to display headers)");
    static t_CKINT ImGuiTableFlags_Hideable = 4;
    QUERY->add_svar(QUERY, "int", "Hideable", true, &ImGuiTableFlags_Hideable);
    QUERY->doc_var(QUERY, "Enable hiding/disabling columns in context menu.");
    static t_CKINT ImGuiTableFlags_Sortable = 8;
    QUERY->add_svar(QUERY, "int", "Sortable", true, &ImGuiTableFlags_Sortable);
    QUERY->doc_var(
      QUERY,
      "Enable sorting. Call TableGetSortSpecs() to obtain sort specs. Also see "
      "ImGuiTableFlags_SortMulti and ImGuiTableFlags_SortTristate.");
    static t_CKINT ImGuiTableFlags_NoSavedSettings = 16;
    QUERY->add_svar(QUERY, "int", "NoSavedSettings", true,
                    &ImGuiTableFlags_NoSavedSettings);
    QUERY->doc_var(QUERY,
                   "Disable persisting columns order, width and sort settings "
                   "in the .ini file.");
    static t_CKINT ImGuiTableFlags_ContextMenuInBody = 32;
    QUERY->add_svar(QUERY, "int", "ContextMenuInBody", true,
                    &ImGuiTableFlags_ContextMenuInBody);
    QUERY->doc_var(
      QUERY,
      "Right-click on columns body/contents will display table context menu. "
      "By default it is available in TableHeadersRow().");
    static t_CKINT ImGuiTableFlags_RowBg = 64;
    QUERY->add_svar(QUERY, "int", "RowBg", true, &ImGuiTableFlags_RowBg);
    QUERY->doc_var(
      QUERY,
      "Set each RowBg color with ImGuiCol_TableRowBg or ImGuiCol_TableRowBgAlt "
      "(equivalent of calling TableSetBgColor with ImGuiTableBgFlags_RowBg0 on "
      "each row manually)");
    static t_CKINT ImGuiTableFlags_BordersInnerH = 128;
    QUERY->add_svar(QUERY, "int", "BordersInnerH", true,
                    &ImGuiTableFlags_BordersInnerH);
    QUERY->doc_var(QUERY, "Draw horizontal borders between rows.");
    static t_CKINT ImGuiTableFlags_BordersOuterH = 256;
    QUERY->add_svar(QUERY, "int", "BordersOuterH", true,
                    &ImGuiTableFlags_BordersOuterH);
    QUERY->doc_var(QUERY, "Draw horizontal borders at the top and bottom.");
    static t_CKINT ImGuiTableFlags_BordersInnerV = 512;
    QUERY->add_svar(QUERY, "int", "BordersInnerV", true,
                    &ImGuiTableFlags_BordersInnerV);
    QUERY->doc_var(QUERY, "Draw vertical borders between columns.");
    static t_CKINT ImGuiTableFlags_BordersOuterV = 1024;
    QUERY->add_svar(QUERY, "int", "BordersOuterV", true,
                    &ImGuiTableFlags_BordersOuterV);
    QUERY->doc_var(QUERY, "Draw vertical borders on the left and right sides.");
    static t_CKINT ImGuiTableFlags_BordersH = 384;
    QUERY->add_svar(QUERY, "int", "BordersH", true, &ImGuiTableFlags_BordersH);
    QUERY->doc_var(QUERY, "Draw horizontal borders.");
    static t_CKINT ImGuiTableFlags_BordersV = 1536;
    QUERY->add_svar(QUERY, "int", "BordersV", true, &ImGuiTableFlags_BordersV);
    QUERY->doc_var(QUERY, "Draw vertical borders.");
    static t_CKINT ImGuiTableFlags_BordersInner = 640;
    QUERY->add_svar(QUERY, "int", "BordersInner", true, &ImGuiTableFlags_BordersInner);
    QUERY->doc_var(QUERY, "Draw inner borders.");
    static t_CKINT ImGuiTableFlags_BordersOuter = 1280;
    QUERY->add_svar(QUERY, "int", "BordersOuter", true, &ImGuiTableFlags_BordersOuter);
    QUERY->doc_var(QUERY, "Draw outer borders.");
    static t_CKINT ImGuiTableFlags_Borders = 1920;
    QUERY->add_svar(QUERY, "int", "Borders", true, &ImGuiTableFlags_Borders);
    QUERY->doc_var(QUERY, "Draw all borders.");
    static t_CKINT ImGuiTableFlags_NoBordersInBody = 2048;
    QUERY->add_svar(QUERY, "int", "NoBordersInBody", true,
                    &ImGuiTableFlags_NoBordersInBody);
    QUERY->doc_var(QUERY,
                   "[ALPHA] Disable vertical borders in columns Body (borders "
                   "will always appear in Headers). -> May move to style");
    static t_CKINT ImGuiTableFlags_NoBordersInBodyUntilResize = 4096;
    QUERY->add_svar(QUERY, "int", "NoBordersInBodyUntilResize", true,
                    &ImGuiTableFlags_NoBordersInBodyUntilResize);
    QUERY->doc_var(
      QUERY,
      "[ALPHA] Disable vertical borders in columns Body until hovered for "
      "resize (borders will always appear in Headers). -> May move to style");
    static t_CKINT ImGuiTableFlags_SizingFixedFit = 8192;
    QUERY->add_svar(QUERY, "int", "SizingFixedFit", true,
                    &ImGuiTableFlags_SizingFixedFit);
    QUERY->doc_var(QUERY,
                   "Columns default to _WidthFixed or _WidthAuto (if resizable "
                   "or not resizable), matching contents width.");
    static t_CKINT ImGuiTableFlags_SizingFixedSame = 16384;
    QUERY->add_svar(QUERY, "int", "SizingFixedSame", true,
                    &ImGuiTableFlags_SizingFixedSame);
    QUERY->doc_var(QUERY,
                   "Columns default to _WidthFixed or _WidthAuto (if resizable or not "
                   "resizable), matching the maximum contents width of all columns. "
                   "Implicitly enable ImGuiTableFlags_NoKeepColumnsVisible.");
    static t_CKINT ImGuiTableFlags_SizingStretchProp = 24576;
    QUERY->add_svar(QUERY, "int", "SizingStretchProp", true,
                    &ImGuiTableFlags_SizingStretchProp);
    QUERY->doc_var(QUERY,
                   "Columns default to _WidthStretch with default weights "
                   "proportional to each columns contents widths.");
    static t_CKINT ImGuiTableFlags_SizingStretchSame = 32768;
    QUERY->add_svar(QUERY, "int", "SizingStretchSame", true,
                    &ImGuiTableFlags_SizingStretchSame);
    QUERY->doc_var(QUERY,
                   "Columns default to _WidthStretch with default weights all "
                   "equal, unless overridden by TableSetupColumn().");
    static t_CKINT ImGuiTableFlags_NoHostExtendX = 65536;
    QUERY->add_svar(QUERY, "int", "NoHostExtendX", true,
                    &ImGuiTableFlags_NoHostExtendX);
    QUERY->doc_var(QUERY,
                   "Make outer width auto-fit to columns, overriding "
                   "outer_size.x value. Only available when ScrollX/ScrollY "
                   "are disabled and Stretch columns are not used.");
    static t_CKINT ImGuiTableFlags_NoHostExtendY = 131072;
    QUERY->add_svar(QUERY, "int", "NoHostExtendY", true,
                    &ImGuiTableFlags_NoHostExtendY);
    QUERY->doc_var(
      QUERY,
      "Make outer height stop exactly at outer_size.y (prevent auto-extending "
      "table past the limit). Only available when ScrollX/ScrollY are "
      "disabled. Data below the limit will be clipped and not visible.");
    static t_CKINT ImGuiTableFlags_NoKeepColumnsVisible = 262144;
    QUERY->add_svar(QUERY, "int", "NoKeepColumnsVisible", true,
                    &ImGuiTableFlags_NoKeepColumnsVisible);
    QUERY->doc_var(
      QUERY,
      "Disable keeping column always minimally visible when ScrollX is off and "
      "table gets too small. Not recommended if columns are resizable.");
    static t_CKINT ImGuiTableFlags_PreciseWidths = 524288;
    QUERY->add_svar(QUERY, "int", "PreciseWidths", true,
                    &ImGuiTableFlags_PreciseWidths);
    QUERY->doc_var(
      QUERY,
      "Disable distributing remainder width to stretched columns (width "
      "allocation on a 100-wide table with 3 columns: Without this flag: "
      "33,33,34. With this flag: 33,33,33). With larger number of columns, "
      "resizing will appear to be less smooth.");
    static t_CKINT ImGuiTableFlags_NoClip = 1048576;
    QUERY->add_svar(QUERY, "int", "NoClip", true, &ImGuiTableFlags_NoClip);
    QUERY->doc_var(
      QUERY,
      "Disable clipping rectangle for every individual columns (reduce draw "
      "command count, items will be able to overflow into other columns). "
      "Generally incompatible with TableSetupScrollFreeze().");
    static t_CKINT ImGuiTableFlags_PadOuterX = 2097152;
    QUERY->add_svar(QUERY, "int", "PadOuterX", true, &ImGuiTableFlags_PadOuterX);
    QUERY->doc_var(QUERY,
                   "Default if BordersOuterV is on. Enable outermost padding. "
                   "Generally desirable if you have headers.");
    static t_CKINT ImGuiTableFlags_NoPadOuterX = 4194304;
    QUERY->add_svar(QUERY, "int", "NoPadOuterX", true, &ImGuiTableFlags_NoPadOuterX);
    QUERY->doc_var(QUERY,
                   "Default if BordersOuterV is off. Disable outermost padding.");
    static t_CKINT ImGuiTableFlags_NoPadInnerX = 8388608;
    QUERY->add_svar(QUERY, "int", "NoPadInnerX", true, &ImGuiTableFlags_NoPadInnerX);
    QUERY->doc_var(
      QUERY,
      "Disable inner padding between columns (double inner padding if "
      "BordersOuterV is on, single inner padding if BordersOuterV is off).");
    static t_CKINT ImGuiTableFlags_ScrollX = 16777216;
    QUERY->add_svar(QUERY, "int", "ScrollX", true, &ImGuiTableFlags_ScrollX);
    QUERY->doc_var(QUERY,
                   "Enable horizontal scrolling. Require 'outer_size' parameter of "
                   "BeginTable() to specify the container size. Changes default sizing "
                   "policy. Because this creates a child window, ScrollY is currently "
                   "generally recommended when using ScrollX.");
    static t_CKINT ImGuiTableFlags_ScrollY = 33554432;
    QUERY->add_svar(QUERY, "int", "ScrollY", true, &ImGuiTableFlags_ScrollY);
    QUERY->doc_var(QUERY,
                   "Enable vertical scrolling. Require 'outer_size' parameter "
                   "of BeginTable() to specify the container size.");
    static t_CKINT ImGuiTableFlags_SortMulti = 67108864;
    QUERY->add_svar(QUERY, "int", "SortMulti", true, &ImGuiTableFlags_SortMulti);
    QUERY->doc_var(QUERY,
                   "Hold shift when clicking headers to sort on multiple column. "
                   "TableGetSortSpecs() may return specs where (SpecsCount > 1).");
    static t_CKINT ImGuiTableFlags_SortTristate = 134217728;
    QUERY->add_svar(QUERY, "int", "SortTristate", true, &ImGuiTableFlags_SortTristate);
    QUERY->doc_var(QUERY,
                   "Allow no sorting, disable default sorting. TableGetSortSpecs() may "
                   "return specs where (SpecsCount == 0).");
    static t_CKINT ImGuiTableFlags_HighlightHoveredColumn = 268435456;
    QUERY->add_svar(QUERY, "int", "HighlightHoveredColumn", true,
                    &ImGuiTableFlags_HighlightHoveredColumn);
    QUERY->doc_var(QUERY,
                   "Highlight column headers when hovered (may evolve into a "
                   "fuller highlight)");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_TableRowFlags", "Object");
    QUERY->doc_class(QUERY, "Flags for ImGui::TableNextRow().\n");
    static t_CKINT ImGuiTableRowFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiTableRowFlags_None);
    static t_CKINT ImGuiTableRowFlags_Headers = 1;
    QUERY->add_svar(QUERY, "int", "Headers", true, &ImGuiTableRowFlags_Headers);
    QUERY->doc_var(QUERY,
                   "Identify header row (set default background color + width of its "
                   "contents accounted differently for auto column width)");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_TableColumnFlags", "Object");
    QUERY->doc_class(QUERY, "Flags for ImGui::TableSetupColumn().\n");
    static t_CKINT ImGuiTableColumnFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiTableColumnFlags_None);
    static t_CKINT ImGuiTableColumnFlags_Disabled = 1;
    QUERY->add_svar(QUERY, "int", "Disabled", true, &ImGuiTableColumnFlags_Disabled);
    QUERY->doc_var(QUERY,
                   "Overriding/master disable flag: hide column, won't show in "
                   "context menu (unlike calling TableSetColumnEnabled() which "
                   "manipulates the user accessible state)");
    static t_CKINT ImGuiTableColumnFlags_DefaultHide = 2;
    QUERY->add_svar(QUERY, "int", "DefaultHide", true,
                    &ImGuiTableColumnFlags_DefaultHide);
    QUERY->doc_var(QUERY, "Default as a hidden/disabled column.");
    static t_CKINT ImGuiTableColumnFlags_DefaultSort = 4;
    QUERY->add_svar(QUERY, "int", "DefaultSort", true,
                    &ImGuiTableColumnFlags_DefaultSort);
    QUERY->doc_var(QUERY, "Default as a sorting column.");
    static t_CKINT ImGuiTableColumnFlags_WidthStretch = 8;
    QUERY->add_svar(QUERY, "int", "WidthStretch", true,
                    &ImGuiTableColumnFlags_WidthStretch);
    QUERY->doc_var(QUERY,
                   "Column will stretch. Preferable with horizontal scrolling "
                   "disabled (default if table sizing policy is "
                   "_SizingStretchSame or _SizingStretchProp).");
    static t_CKINT ImGuiTableColumnFlags_WidthFixed = 16;
    QUERY->add_svar(QUERY, "int", "WidthFixed", true,
                    &ImGuiTableColumnFlags_WidthFixed);
    QUERY->doc_var(QUERY,
                   "Column will not stretch. Preferable with horizontal "
                   "scrolling enabled (default if table sizing policy is "
                   "_SizingFixedFit and table is resizable).");
    static t_CKINT ImGuiTableColumnFlags_NoResize = 32;
    QUERY->add_svar(QUERY, "int", "NoResize", true, &ImGuiTableColumnFlags_NoResize);
    QUERY->doc_var(QUERY, "Disable manual resizing.");
    static t_CKINT ImGuiTableColumnFlags_NoReorder = 64;
    QUERY->add_svar(QUERY, "int", "NoReorder", true, &ImGuiTableColumnFlags_NoReorder);
    QUERY->doc_var(QUERY,
                   "Disable manual reordering this column, this will also "
                   "prevent other columns from crossing over this column.");
    static t_CKINT ImGuiTableColumnFlags_NoHide = 128;
    QUERY->add_svar(QUERY, "int", "NoHide", true, &ImGuiTableColumnFlags_NoHide);
    QUERY->doc_var(QUERY, "Disable ability to hide/disable this column.");
    static t_CKINT ImGuiTableColumnFlags_NoClip = 256;
    QUERY->add_svar(QUERY, "int", "NoClip", true, &ImGuiTableColumnFlags_NoClip);
    QUERY->doc_var(QUERY,
                   "Disable clipping for this column (all NoClip columns will "
                   "render in a same draw command).");
    static t_CKINT ImGuiTableColumnFlags_NoSort = 512;
    QUERY->add_svar(QUERY, "int", "NoSort", true, &ImGuiTableColumnFlags_NoSort);
    QUERY->doc_var(QUERY,
                   "Disable ability to sort on this field (even if "
                   "ImGuiTableFlags_Sortable is set on the table).");
    static t_CKINT ImGuiTableColumnFlags_NoSortAscending = 1024;
    QUERY->add_svar(QUERY, "int", "NoSortAscending", true,
                    &ImGuiTableColumnFlags_NoSortAscending);
    QUERY->doc_var(QUERY, "Disable ability to sort in the ascending direction.");
    static t_CKINT ImGuiTableColumnFlags_NoSortDescending = 2048;
    QUERY->add_svar(QUERY, "int", "NoSortDescending", true,
                    &ImGuiTableColumnFlags_NoSortDescending);
    QUERY->doc_var(QUERY, "Disable ability to sort in the descending direction.");
    static t_CKINT ImGuiTableColumnFlags_NoHeaderLabel = 4096;
    QUERY->add_svar(QUERY, "int", "NoHeaderLabel", true,
                    &ImGuiTableColumnFlags_NoHeaderLabel);
    QUERY->doc_var(QUERY,
                   "TableHeadersRow() will not submit horizontal label for "
                   "this column. Convenient for some small columns. Name will "
                   "still appear in context menu or in angled headers.");
    static t_CKINT ImGuiTableColumnFlags_NoHeaderWidth = 8192;
    QUERY->add_svar(QUERY, "int", "NoHeaderWidth", true,
                    &ImGuiTableColumnFlags_NoHeaderWidth);
    QUERY->doc_var(QUERY,
                   "Disable header text width contribution to automatic column width.");
    static t_CKINT ImGuiTableColumnFlags_PreferSortAscending = 16384;
    QUERY->add_svar(QUERY, "int", "PreferSortAscending", true,
                    &ImGuiTableColumnFlags_PreferSortAscending);
    QUERY->doc_var(QUERY,
                   "Make the initial sort direction Ascending when first "
                   "sorting on this column (default).");
    static t_CKINT ImGuiTableColumnFlags_PreferSortDescending = 32768;
    QUERY->add_svar(QUERY, "int", "PreferSortDescending", true,
                    &ImGuiTableColumnFlags_PreferSortDescending);
    QUERY->doc_var(QUERY,
                   "Make the initial sort direction Descending when first "
                   "sorting on this column.");
    static t_CKINT ImGuiTableColumnFlags_IndentEnable = 65536;
    QUERY->add_svar(QUERY, "int", "IndentEnable", true,
                    &ImGuiTableColumnFlags_IndentEnable);
    QUERY->doc_var(
      QUERY, "Use current Indent value when entering cell (default for column 0).");
    static t_CKINT ImGuiTableColumnFlags_IndentDisable = 131072;
    QUERY->add_svar(QUERY, "int", "IndentDisable", true,
                    &ImGuiTableColumnFlags_IndentDisable);
    QUERY->doc_var(
      QUERY,
      "Ignore current Indent value when entering cell (default for columns > "
      "0). Indentation changes _within_ the cell will still be honored.");
    static t_CKINT ImGuiTableColumnFlags_AngledHeader = 262144;
    QUERY->add_svar(QUERY, "int", "AngledHeader", true,
                    &ImGuiTableColumnFlags_AngledHeader);
    QUERY->doc_var(QUERY,
                   "TableHeadersRow() will submit an angled header row for "
                   "this column. Note this will add an extra row.");
    static t_CKINT ImGuiTableColumnFlags_IsEnabled = 16777216;
    QUERY->add_svar(QUERY, "int", "IsEnabled", true, &ImGuiTableColumnFlags_IsEnabled);
    QUERY->doc_var(QUERY,
                   "Status: is enabled == not hidden by user/api (referred to "
                   "as \"Hide\" in _DefaultHide and _NoHide) flags.");
    static t_CKINT ImGuiTableColumnFlags_IsVisible = 33554432;
    QUERY->add_svar(QUERY, "int", "IsVisible", true, &ImGuiTableColumnFlags_IsVisible);
    QUERY->doc_var(QUERY,
                   "Status: is visible == is enabled AND not clipped by scrolling.");
    static t_CKINT ImGuiTableColumnFlags_IsSorted = 67108864;
    QUERY->add_svar(QUERY, "int", "IsSorted", true, &ImGuiTableColumnFlags_IsSorted);
    QUERY->doc_var(QUERY, "Status: is currently part of the sort specs");
    static t_CKINT ImGuiTableColumnFlags_IsHovered = 134217728;
    QUERY->add_svar(QUERY, "int", "IsHovered", true, &ImGuiTableColumnFlags_IsHovered);
    QUERY->doc_var(QUERY, "Status: is hovered by mouse");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_TableBgTarget", "Object");
    QUERY->doc_class(
      QUERY,
      "Enum for ImGui::TableSetBgColor().\nBackground colors are rendering in "
      "3 layers:.\n- Layer 0: draw with RowBg0 color if set, otherwise draw "
      "with ColumnBg0 if set..\n- Layer 1: draw with RowBg1 color if set, "
      "otherwise draw with ColumnBg1 if set..\n- Layer 2: draw with CellBg "
      "color if set..\nThe purpose of the two row/columns layers is to let you "
      "decide if a background color change should override or blend with the "
      "existing color..\nWhen using ImGuiTableFlags_RowBg on the table, each "
      "row has the RowBg0 color automatically set for odd/even rows..\nIf you "
      "set the color of RowBg0 target, your color will override the existing "
      "RowBg0 color..\nIf you set the color of RowBg1 or ColumnBg1 target, "
      "your color will blend over the RowBg0 color..\n");
    static t_CKINT ImGuiTableBgTarget_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiTableBgTarget_None);
    static t_CKINT ImGuiTableBgTarget_RowBg0 = 1;
    QUERY->add_svar(QUERY, "int", "RowBg0", true, &ImGuiTableBgTarget_RowBg0);
    QUERY->doc_var(QUERY,
                   "Set row background color 0 (generally used for background, "
                   "automatically set when ImGuiTableFlags_RowBg is used)");
    static t_CKINT ImGuiTableBgTarget_RowBg1 = 2;
    QUERY->add_svar(QUERY, "int", "RowBg1", true, &ImGuiTableBgTarget_RowBg1);
    QUERY->doc_var(QUERY,
                   "Set row background color 1 (generally used for selection marking)");
    static t_CKINT ImGuiTableBgTarget_CellBg = 3;
    QUERY->add_svar(QUERY, "int", "CellBg", true, &ImGuiTableBgTarget_CellBg);
    QUERY->doc_var(QUERY, "Set cell background color (top-most color)");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_TabBarFlags", "Object");
    QUERY->doc_class(QUERY, "Flags for ImGui::BeginTabBar().\n");
    static t_CKINT ImGuiTabBarFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiTabBarFlags_None);
    static t_CKINT ImGuiTabBarFlags_Reorderable = 1;
    QUERY->add_svar(QUERY, "int", "Reorderable", true, &ImGuiTabBarFlags_Reorderable);
    QUERY->doc_var(QUERY,
                   "Allow manually dragging tabs to re-order them + New tabs "
                   "are appended at the end of list");
    static t_CKINT ImGuiTabBarFlags_AutoSelectNewTabs = 2;
    QUERY->add_svar(QUERY, "int", "AutoSelectNewTabs", true,
                    &ImGuiTabBarFlags_AutoSelectNewTabs);
    QUERY->doc_var(QUERY, "Automatically select new tabs when they appear");
    static t_CKINT ImGuiTabBarFlags_TabListPopupButton = 4;
    QUERY->add_svar(QUERY, "int", "TabListPopupButton", true,
                    &ImGuiTabBarFlags_TabListPopupButton);
    QUERY->doc_var(QUERY, "Disable buttons to open the tab list popup");
    static t_CKINT ImGuiTabBarFlags_NoCloseWithMiddleMouseButton = 8;
    QUERY->add_svar(QUERY, "int", "NoCloseWithMiddleMouseButton", true,
                    &ImGuiTabBarFlags_NoCloseWithMiddleMouseButton);
    QUERY->doc_var(QUERY,
                   "Disable behavior of closing tabs (that are submitted with "
                   "p_open != NULL) with middle mouse button. You may handle "
                   "this behavior manually on user's side with if "
                   "(IsItemHovered() && IsMouseClicked(2)) *p_open = false.");
    static t_CKINT ImGuiTabBarFlags_NoTabListScrollingButtons = 16;
    QUERY->add_svar(QUERY, "int", "NoTabListScrollingButtons", true,
                    &ImGuiTabBarFlags_NoTabListScrollingButtons);
    QUERY->doc_var(QUERY,
                   "Disable scrolling buttons (apply when fitting policy is "
                   "ImGuiTabBarFlags_FittingPolicyScroll)");
    static t_CKINT ImGuiTabBarFlags_NoTooltip = 32;
    QUERY->add_svar(QUERY, "int", "NoTooltip", true, &ImGuiTabBarFlags_NoTooltip);
    QUERY->doc_var(QUERY, "Disable tooltips when hovering a tab");
    static t_CKINT ImGuiTabBarFlags_FittingPolicyResizeDown = 64;
    QUERY->add_svar(QUERY, "int", "FittingPolicyResizeDown", true,
                    &ImGuiTabBarFlags_FittingPolicyResizeDown);
    QUERY->doc_var(QUERY, "Resize tabs when they don't fit");
    static t_CKINT ImGuiTabBarFlags_FittingPolicyScroll = 128;
    QUERY->add_svar(QUERY, "int", "FittingPolicyScroll", true,
                    &ImGuiTabBarFlags_FittingPolicyScroll);
    QUERY->doc_var(QUERY, "Add scroll buttons when tabs don't fit");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_MouseButton", "Object");
    QUERY->doc_class(QUERY,
                     "Identify a mouse button..\nThose values are guaranteed "
                     "to be stable and we frequently use 0/1 directly. Named "
                     "enums provided for convenience..\n");
    static t_CKINT ImGuiMouseButton_Left = 0;
    QUERY->add_svar(QUERY, "int", "Left", true, &ImGuiMouseButton_Left);
    DOC_VAR("UI_MouseButton.Left = 0");
    static t_CKINT ImGuiMouseButton_Right = 1;
    QUERY->add_svar(QUERY, "int", "Right", true, &ImGuiMouseButton_Right);
    DOC_VAR("UI_MouseButton.Right = 1");
    static t_CKINT ImGuiMouseButton_Middle = 2;
    QUERY->add_svar(QUERY, "int", "Middle", true, &ImGuiMouseButton_Middle);
    DOC_VAR("UI_MouseButton.Middle = 2");
    QUERY->end_class(QUERY);

    { // UI_KnobFlags
        static t_CKINT UI_KnobFlags_NoTitle        = ImGuiKnobFlags_NoTitle;
        static t_CKINT UI_KnobFlags_NoInput        = ImGuiKnobFlags_NoInput;
        static t_CKINT UI_KnobFlags_ValueTooltip   = ImGuiKnobFlags_ValueTooltip;
        static t_CKINT UI_KnobFlags_DragHorizontal = ImGuiKnobFlags_DragHorizontal;
        static t_CKINT UI_KnobFlags_DragVertical   = ImGuiKnobFlags_DragVertical;
        static t_CKINT UI_KnobFlags_Logarithmic    = ImGuiKnobFlags_Logarithmic;
        static t_CKINT UI_KnobFlags_AlwaysClamp    = ImGuiKnobFlags_AlwaysClamp;

        BEGIN_CLASS("UI_KnobFlags", "Object");
        DOC_CLASS(
          "Knob behavior flags. See "
          "https://github.com/altschuler/imgui-knobs/for more info");

        SVAR("int", "NoTitle", &UI_KnobFlags_NoTitle);
        DOC_VAR("Hide the top title.");
        SVAR("int", "NoInput", &UI_KnobFlags_NoInput);
        DOC_VAR("Hide the bottom drag input.");
        SVAR("int", "ValueTooltip", &UI_KnobFlags_ValueTooltip);
        DOC_VAR("Show a tooltip with the current value on hover.");
        SVAR("int", "DragHorizontal", &UI_KnobFlags_DragHorizontal);
        DOC_VAR("Use horizontal dragging only (default is bi-directional).");
        SVAR("int", "DragVertical", &UI_KnobFlags_DragVertical);
        DOC_VAR("Use vertical dragging only (default is bi-directional).");
        SVAR("int", "Logarithmic", &UI_KnobFlags_Logarithmic);
        DOC_VAR(
          "Clamp input values that the user types into the input field. If not set, "
          "it's possible to override the min/max range via the input field.");
        SVAR("int", "AlwaysClamp", &UI_KnobFlags_AlwaysClamp);
        DOC_VAR("Use logarithmic scale for the knob (otherwise linear).");

        END_CLASS();

    } // UI_KnobFlags

    { // UI_KnobVariant
        static t_CKINT UI_KnobVariant_Tick      = ImGuiKnobVariant_Tick;
        static t_CKINT UI_KnobVariant_Dot       = ImGuiKnobVariant_Dot;
        static t_CKINT UI_KnobVariant_Wiper     = ImGuiKnobVariant_Wiper;
        static t_CKINT UI_KnobVariant_WiperOnly = ImGuiKnobVariant_WiperOnly;
        static t_CKINT UI_KnobVariant_WiperDot  = ImGuiKnobVariant_WiperDot;
        static t_CKINT UI_KnobVariant_Stepped   = ImGuiKnobVariant_Stepped;
        static t_CKINT UI_KnobVariant_Space     = ImGuiKnobVariant_Space;

        BEGIN_CLASS("UI_KnobVariant", "Object");
        DOC_CLASS(
          "These flags determine the visual look of the knob. See "
          "https://github.com/altschuler/imgui-knobs/for more info");

        SVAR("int", "Tick", &UI_KnobVariant_Tick);
        SVAR("int", "Dot", &UI_KnobVariant_Dot);
        SVAR("int", "Wiper", &UI_KnobVariant_Wiper);
        SVAR("int", "WiperOnly", &UI_KnobVariant_WiperOnly);
        SVAR("int", "WiperDot", &UI_KnobVariant_WiperDot);
        SVAR("int", "Stepped", &UI_KnobVariant_Stepped);
        SVAR("int", "Space", &UI_KnobVariant_Space);

        END_CLASS();
    } // UI_KnobVariant

    { // UI_Key
        QUERY->begin_class(QUERY, "UI_Key", "Object");
        QUERY->doc_class(
          QUERY,
          "A key identifier (ImGuiKey_XXX or ImGuiMod_XXX value): can represent "
          "Keyboard, Mouse and Gamepad values..\nAll our named keys are >= 512. "
          "Keys value 0 to 511 are left unused as legacy native/opaque key values "
          "(< 1.87)..\nSince >= 1.89 we increased typing (went from int to enum), "
          "some legacy code may need a cast to ImGuiKey..\nRead details about the "
          "1.87 and 1.89 transition : "
          "https:github.com/ocornut/imgui/issues/4921.\nNote that \"Keys\" related "
          "to physical keys and are not the same concept as input \"Characters\", "
          "the later are submitted via io.AddInputCharacter()..\nThe keyboard key "
          "enum values are named after the keys on a standard US keyboard, and on "
          "other keyboard types the keys reported may not match the "
          "keycaps..\nForward declared enum type ImGuiKey");
        static t_CKINT ImGuiKey_None = 0;
        QUERY->add_svar(QUERY, "int", "None", true, &ImGuiKey_None);
        static t_CKINT ImGuiKey_Tab = 512;
        QUERY->add_svar(QUERY, "int", "Tab", true, &ImGuiKey_Tab);
        QUERY->doc_var(QUERY, "== ImGuiKey_NamedKey_BEGIN");
        static t_CKINT ImGuiKey_LeftArrow = 513;
        QUERY->add_svar(QUERY, "int", "LeftArrow", true, &ImGuiKey_LeftArrow);
        static t_CKINT ImGuiKey_RightArrow = 514;
        QUERY->add_svar(QUERY, "int", "RightArrow", true, &ImGuiKey_RightArrow);
        static t_CKINT ImGuiKey_UpArrow = 515;
        QUERY->add_svar(QUERY, "int", "UpArrow", true, &ImGuiKey_UpArrow);
        static t_CKINT ImGuiKey_DownArrow = 516;
        QUERY->add_svar(QUERY, "int", "DownArrow", true, &ImGuiKey_DownArrow);
        static t_CKINT ImGuiKey_PageUp = 517;
        QUERY->add_svar(QUERY, "int", "PageUp", true, &ImGuiKey_PageUp);
        static t_CKINT ImGuiKey_PageDown = 518;
        QUERY->add_svar(QUERY, "int", "PageDown", true, &ImGuiKey_PageDown);
        static t_CKINT ImGuiKey_Home = 519;
        QUERY->add_svar(QUERY, "int", "Home", true, &ImGuiKey_Home);
        static t_CKINT ImGuiKey_End = 520;
        QUERY->add_svar(QUERY, "int", "End", true, &ImGuiKey_End);
        static t_CKINT ImGuiKey_Insert = 521;
        QUERY->add_svar(QUERY, "int", "Insert", true, &ImGuiKey_Insert);
        static t_CKINT ImGuiKey_Delete = 522;
        QUERY->add_svar(QUERY, "int", "Delete", true, &ImGuiKey_Delete);
        static t_CKINT ImGuiKey_Backspace = 523;
        QUERY->add_svar(QUERY, "int", "Backspace", true, &ImGuiKey_Backspace);
        static t_CKINT ImGuiKey_Space = 524;
        QUERY->add_svar(QUERY, "int", "Space", true, &ImGuiKey_Space);
        static t_CKINT ImGuiKey_Enter = 525;
        QUERY->add_svar(QUERY, "int", "Enter", true, &ImGuiKey_Enter);
        static t_CKINT ImGuiKey_Escape = 526;
        QUERY->add_svar(QUERY, "int", "Escape", true, &ImGuiKey_Escape);
        static t_CKINT ImGuiKey_LeftCtrl = 527;
        QUERY->add_svar(QUERY, "int", "LeftCtrl", true, &ImGuiKey_LeftCtrl);
        static t_CKINT ImGuiKey_LeftShift = 528;
        QUERY->add_svar(QUERY, "int", "LeftShift", true, &ImGuiKey_LeftShift);
        static t_CKINT ImGuiKey_LeftAlt = 529;
        QUERY->add_svar(QUERY, "int", "LeftAlt", true, &ImGuiKey_LeftAlt);
        static t_CKINT ImGuiKey_LeftSuper = 530;
        QUERY->add_svar(QUERY, "int", "LeftSuper", true, &ImGuiKey_LeftSuper);
        static t_CKINT ImGuiKey_RightCtrl = 531;
        QUERY->add_svar(QUERY, "int", "RightCtrl", true, &ImGuiKey_RightCtrl);
        static t_CKINT ImGuiKey_RightShift = 532;
        QUERY->add_svar(QUERY, "int", "RightShift", true, &ImGuiKey_RightShift);
        static t_CKINT ImGuiKey_RightAlt = 533;
        QUERY->add_svar(QUERY, "int", "RightAlt", true, &ImGuiKey_RightAlt);
        static t_CKINT ImGuiKey_RightSuper = 534;
        QUERY->add_svar(QUERY, "int", "RightSuper", true, &ImGuiKey_RightSuper);
        static t_CKINT ImGuiKey_Menu = 535;
        QUERY->add_svar(QUERY, "int", "Menu", true, &ImGuiKey_Menu);
        static t_CKINT ImGuiKey_0 = 536;
        QUERY->add_svar(QUERY, "int", "Num0", true, &ImGuiKey_0);
        static t_CKINT ImGuiKey_1 = 537;
        QUERY->add_svar(QUERY, "int", "Num1", true, &ImGuiKey_1);
        static t_CKINT ImGuiKey_2 = 538;
        QUERY->add_svar(QUERY, "int", "Num2", true, &ImGuiKey_2);
        static t_CKINT ImGuiKey_3 = 539;
        QUERY->add_svar(QUERY, "int", "Num3", true, &ImGuiKey_3);
        static t_CKINT ImGuiKey_4 = 540;
        QUERY->add_svar(QUERY, "int", "Num4", true, &ImGuiKey_4);
        static t_CKINT ImGuiKey_5 = 541;
        QUERY->add_svar(QUERY, "int", "Num5", true, &ImGuiKey_5);
        static t_CKINT ImGuiKey_6 = 542;
        QUERY->add_svar(QUERY, "int", "Num6", true, &ImGuiKey_6);
        static t_CKINT ImGuiKey_7 = 543;
        QUERY->add_svar(QUERY, "int", "Num7", true, &ImGuiKey_7);
        static t_CKINT ImGuiKey_8 = 544;
        QUERY->add_svar(QUERY, "int", "Num8", true, &ImGuiKey_8);
        static t_CKINT ImGuiKey_9 = 545;
        QUERY->add_svar(QUERY, "int", "Num9", true, &ImGuiKey_9);
        static t_CKINT ImGuiKey_A = 546;
        QUERY->add_svar(QUERY, "int", "A", true, &ImGuiKey_A);
        static t_CKINT ImGuiKey_B = 547;
        QUERY->add_svar(QUERY, "int", "B", true, &ImGuiKey_B);
        static t_CKINT ImGuiKey_C = 548;
        QUERY->add_svar(QUERY, "int", "C", true, &ImGuiKey_C);
        static t_CKINT ImGuiKey_D = 549;
        QUERY->add_svar(QUERY, "int", "D", true, &ImGuiKey_D);
        static t_CKINT ImGuiKey_E = 550;
        QUERY->add_svar(QUERY, "int", "E", true, &ImGuiKey_E);
        static t_CKINT ImGuiKey_F = 551;
        QUERY->add_svar(QUERY, "int", "F", true, &ImGuiKey_F);
        static t_CKINT ImGuiKey_G = 552;
        QUERY->add_svar(QUERY, "int", "G", true, &ImGuiKey_G);
        static t_CKINT ImGuiKey_H = 553;
        QUERY->add_svar(QUERY, "int", "H", true, &ImGuiKey_H);
        static t_CKINT ImGuiKey_I = 554;
        QUERY->add_svar(QUERY, "int", "I", true, &ImGuiKey_I);
        static t_CKINT ImGuiKey_J = 555;
        QUERY->add_svar(QUERY, "int", "J", true, &ImGuiKey_J);
        static t_CKINT ImGuiKey_K = 556;
        QUERY->add_svar(QUERY, "int", "K", true, &ImGuiKey_K);
        static t_CKINT ImGuiKey_L = 557;
        QUERY->add_svar(QUERY, "int", "L", true, &ImGuiKey_L);
        static t_CKINT ImGuiKey_M = 558;
        QUERY->add_svar(QUERY, "int", "M", true, &ImGuiKey_M);
        static t_CKINT ImGuiKey_N = 559;
        QUERY->add_svar(QUERY, "int", "N", true, &ImGuiKey_N);
        static t_CKINT ImGuiKey_O = 560;
        QUERY->add_svar(QUERY, "int", "O", true, &ImGuiKey_O);
        static t_CKINT ImGuiKey_P = 561;
        QUERY->add_svar(QUERY, "int", "P", true, &ImGuiKey_P);
        static t_CKINT ImGuiKey_Q = 562;
        QUERY->add_svar(QUERY, "int", "Q", true, &ImGuiKey_Q);
        static t_CKINT ImGuiKey_R = 563;
        QUERY->add_svar(QUERY, "int", "R", true, &ImGuiKey_R);
        static t_CKINT ImGuiKey_S = 564;
        QUERY->add_svar(QUERY, "int", "S", true, &ImGuiKey_S);
        static t_CKINT ImGuiKey_T = 565;
        QUERY->add_svar(QUERY, "int", "T", true, &ImGuiKey_T);
        static t_CKINT ImGuiKey_U = 566;
        QUERY->add_svar(QUERY, "int", "U", true, &ImGuiKey_U);
        static t_CKINT ImGuiKey_V = 567;
        QUERY->add_svar(QUERY, "int", "V", true, &ImGuiKey_V);
        static t_CKINT ImGuiKey_W = 568;
        QUERY->add_svar(QUERY, "int", "W", true, &ImGuiKey_W);
        static t_CKINT ImGuiKey_X = 569;
        QUERY->add_svar(QUERY, "int", "X", true, &ImGuiKey_X);
        static t_CKINT ImGuiKey_Y = 570;
        QUERY->add_svar(QUERY, "int", "Y", true, &ImGuiKey_Y);
        static t_CKINT ImGuiKey_Z = 571;
        QUERY->add_svar(QUERY, "int", "Z", true, &ImGuiKey_Z);
        static t_CKINT ImGuiKey_F1 = 572;
        QUERY->add_svar(QUERY, "int", "F1", true, &ImGuiKey_F1);
        static t_CKINT ImGuiKey_F2 = 573;
        QUERY->add_svar(QUERY, "int", "F2", true, &ImGuiKey_F2);
        static t_CKINT ImGuiKey_F3 = 574;
        QUERY->add_svar(QUERY, "int", "F3", true, &ImGuiKey_F3);
        static t_CKINT ImGuiKey_F4 = 575;
        QUERY->add_svar(QUERY, "int", "F4", true, &ImGuiKey_F4);
        static t_CKINT ImGuiKey_F5 = 576;
        QUERY->add_svar(QUERY, "int", "F5", true, &ImGuiKey_F5);
        static t_CKINT ImGuiKey_F6 = 577;
        QUERY->add_svar(QUERY, "int", "F6", true, &ImGuiKey_F6);
        static t_CKINT ImGuiKey_F7 = 578;
        QUERY->add_svar(QUERY, "int", "F7", true, &ImGuiKey_F7);
        static t_CKINT ImGuiKey_F8 = 579;
        QUERY->add_svar(QUERY, "int", "F8", true, &ImGuiKey_F8);
        static t_CKINT ImGuiKey_F9 = 580;
        QUERY->add_svar(QUERY, "int", "F9", true, &ImGuiKey_F9);
        static t_CKINT ImGuiKey_F10 = 581;
        QUERY->add_svar(QUERY, "int", "F10", true, &ImGuiKey_F10);
        static t_CKINT ImGuiKey_F11 = 582;
        QUERY->add_svar(QUERY, "int", "F11", true, &ImGuiKey_F11);
        static t_CKINT ImGuiKey_F12 = 583;
        QUERY->add_svar(QUERY, "int", "F12", true, &ImGuiKey_F12);
        static t_CKINT ImGuiKey_F13 = 584;
        QUERY->add_svar(QUERY, "int", "F13", true, &ImGuiKey_F13);
        static t_CKINT ImGuiKey_F14 = 585;
        QUERY->add_svar(QUERY, "int", "F14", true, &ImGuiKey_F14);
        static t_CKINT ImGuiKey_F15 = 586;
        QUERY->add_svar(QUERY, "int", "F15", true, &ImGuiKey_F15);
        static t_CKINT ImGuiKey_F16 = 587;
        QUERY->add_svar(QUERY, "int", "F16", true, &ImGuiKey_F16);
        static t_CKINT ImGuiKey_F17 = 588;
        QUERY->add_svar(QUERY, "int", "F17", true, &ImGuiKey_F17);
        static t_CKINT ImGuiKey_F18 = 589;
        QUERY->add_svar(QUERY, "int", "F18", true, &ImGuiKey_F18);
        static t_CKINT ImGuiKey_F19 = 590;
        QUERY->add_svar(QUERY, "int", "F19", true, &ImGuiKey_F19);
        static t_CKINT ImGuiKey_F20 = 591;
        QUERY->add_svar(QUERY, "int", "F20", true, &ImGuiKey_F20);
        static t_CKINT ImGuiKey_F21 = 592;
        QUERY->add_svar(QUERY, "int", "F21", true, &ImGuiKey_F21);
        static t_CKINT ImGuiKey_F22 = 593;
        QUERY->add_svar(QUERY, "int", "F22", true, &ImGuiKey_F22);
        static t_CKINT ImGuiKey_F23 = 594;
        QUERY->add_svar(QUERY, "int", "F23", true, &ImGuiKey_F23);
        static t_CKINT ImGuiKey_F24 = 595;
        QUERY->add_svar(QUERY, "int", "F24", true, &ImGuiKey_F24);
        static t_CKINT ImGuiKey_Apostrophe = 596;
        QUERY->add_svar(QUERY, "int", "Apostrophe", true, &ImGuiKey_Apostrophe);
        QUERY->doc_var(QUERY, "'");
        static t_CKINT ImGuiKey_Comma = 597;
        QUERY->add_svar(QUERY, "int", "Comma", true, &ImGuiKey_Comma);
        QUERY->doc_var(QUERY, ",");
        static t_CKINT ImGuiKey_Minus = 598;
        QUERY->add_svar(QUERY, "int", "Minus", true, &ImGuiKey_Minus);
        QUERY->doc_var(QUERY, "-");
        static t_CKINT ImGuiKey_Period = 599;
        QUERY->add_svar(QUERY, "int", "Period", true, &ImGuiKey_Period);
        QUERY->doc_var(QUERY, ".");
        static t_CKINT ImGuiKey_Slash = 600;
        QUERY->add_svar(QUERY, "int", "Slash", true, &ImGuiKey_Slash);
        QUERY->doc_var(QUERY, "/");
        static t_CKINT ImGuiKey_Semicolon = 601;
        QUERY->add_svar(QUERY, "int", "Semicolon", true, &ImGuiKey_Semicolon);
        QUERY->doc_var(QUERY, ";");
        static t_CKINT ImGuiKey_Equal = 602;
        QUERY->add_svar(QUERY, "int", "Equal", true, &ImGuiKey_Equal);
        QUERY->doc_var(QUERY, "=");
        static t_CKINT ImGuiKey_LeftBracket = 603;
        QUERY->add_svar(QUERY, "int", "LeftBracket", true, &ImGuiKey_LeftBracket);
        QUERY->doc_var(QUERY, "[");
        static t_CKINT ImGuiKey_Backslash = 604;
        QUERY->add_svar(QUERY, "int", "Backslash", true, &ImGuiKey_Backslash);
        QUERY->doc_var(QUERY,
                       "\\ (this text inhibit multiline comment caused by backslash)");
        static t_CKINT ImGuiKey_RightBracket = 605;
        QUERY->add_svar(QUERY, "int", "RightBracket", true, &ImGuiKey_RightBracket);
        QUERY->doc_var(QUERY, "]");
        static t_CKINT ImGuiKey_GraveAccent = 606;
        QUERY->add_svar(QUERY, "int", "GraveAccent", true, &ImGuiKey_GraveAccent);
        QUERY->doc_var(QUERY, "`");
        static t_CKINT ImGuiKey_CapsLock = 607;
        QUERY->add_svar(QUERY, "int", "CapsLock", true, &ImGuiKey_CapsLock);
        static t_CKINT ImGuiKey_ScrollLock = 608;
        QUERY->add_svar(QUERY, "int", "ScrollLock", true, &ImGuiKey_ScrollLock);
        static t_CKINT ImGuiKey_NumLock = 609;
        QUERY->add_svar(QUERY, "int", "NumLock", true, &ImGuiKey_NumLock);
        static t_CKINT ImGuiKey_PrintScreen = 610;
        QUERY->add_svar(QUERY, "int", "PrintScreen", true, &ImGuiKey_PrintScreen);
        static t_CKINT ImGuiKey_Pause = 611;
        QUERY->add_svar(QUERY, "int", "Pause", true, &ImGuiKey_Pause);
        static t_CKINT ImGuiKey_Keypad0 = 612;
        QUERY->add_svar(QUERY, "int", "Keypad0", true, &ImGuiKey_Keypad0);
        static t_CKINT ImGuiKey_Keypad1 = 613;
        QUERY->add_svar(QUERY, "int", "Keypad1", true, &ImGuiKey_Keypad1);
        static t_CKINT ImGuiKey_Keypad2 = 614;
        QUERY->add_svar(QUERY, "int", "Keypad2", true, &ImGuiKey_Keypad2);
        static t_CKINT ImGuiKey_Keypad3 = 615;
        QUERY->add_svar(QUERY, "int", "Keypad3", true, &ImGuiKey_Keypad3);
        static t_CKINT ImGuiKey_Keypad4 = 616;
        QUERY->add_svar(QUERY, "int", "Keypad4", true, &ImGuiKey_Keypad4);
        static t_CKINT ImGuiKey_Keypad5 = 617;
        QUERY->add_svar(QUERY, "int", "Keypad5", true, &ImGuiKey_Keypad5);
        static t_CKINT ImGuiKey_Keypad6 = 618;
        QUERY->add_svar(QUERY, "int", "Keypad6", true, &ImGuiKey_Keypad6);
        static t_CKINT ImGuiKey_Keypad7 = 619;
        QUERY->add_svar(QUERY, "int", "Keypad7", true, &ImGuiKey_Keypad7);
        static t_CKINT ImGuiKey_Keypad8 = 620;
        QUERY->add_svar(QUERY, "int", "Keypad8", true, &ImGuiKey_Keypad8);
        static t_CKINT ImGuiKey_Keypad9 = 621;
        QUERY->add_svar(QUERY, "int", "Keypad9", true, &ImGuiKey_Keypad9);
        static t_CKINT ImGuiKey_KeypadDecimal = 622;
        QUERY->add_svar(QUERY, "int", "KeypadDecimal", true, &ImGuiKey_KeypadDecimal);
        static t_CKINT ImGuiKey_KeypadDivide = 623;
        QUERY->add_svar(QUERY, "int", "KeypadDivide", true, &ImGuiKey_KeypadDivide);
        static t_CKINT ImGuiKey_KeypadMultiply = 624;
        QUERY->add_svar(QUERY, "int", "KeypadMultiply", true, &ImGuiKey_KeypadMultiply);
        static t_CKINT ImGuiKey_KeypadSubtract = 625;
        QUERY->add_svar(QUERY, "int", "KeypadSubtract", true, &ImGuiKey_KeypadSubtract);
        static t_CKINT ImGuiKey_KeypadAdd = 626;
        QUERY->add_svar(QUERY, "int", "KeypadAdd", true, &ImGuiKey_KeypadAdd);
        static t_CKINT ImGuiKey_KeypadEnter = 627;
        QUERY->add_svar(QUERY, "int", "KeypadEnter", true, &ImGuiKey_KeypadEnter);
        static t_CKINT ImGuiKey_KeypadEqual = 628;
        QUERY->add_svar(QUERY, "int", "KeypadEqual", true, &ImGuiKey_KeypadEqual);
        static t_CKINT ImGuiKey_AppBack = 629;
        QUERY->add_svar(QUERY, "int", "AppBack", true, &ImGuiKey_AppBack);
        QUERY->doc_var(
          QUERY,
          "Available on some keyboard/mouses. Often referred as \"Browser Back\"");
        static t_CKINT ImGuiKey_AppForward = 630;
        QUERY->add_svar(QUERY, "int", "AppForward", true, &ImGuiKey_AppForward);
        static t_CKINT ImGuiKey_GamepadStart = 631;
        QUERY->add_svar(QUERY, "int", "GamepadStart", true, &ImGuiKey_GamepadStart);
        QUERY->doc_var(QUERY, "Menu (Xbox)      + (Switch)   Start/Options (PS)");
        static t_CKINT ImGuiKey_GamepadBack = 632;
        QUERY->add_svar(QUERY, "int", "GamepadBack", true, &ImGuiKey_GamepadBack);
        QUERY->doc_var(QUERY, "View (Xbox)      - (Switch)   Share (PS)");
        static t_CKINT ImGuiKey_GamepadFaceLeft = 633;
        QUERY->add_svar(QUERY, "int", "GamepadFaceLeft", true,
                        &ImGuiKey_GamepadFaceLeft);
        QUERY->doc_var(
          QUERY,
          "X (Xbox)         Y (Switch)   Square (PS)         Tap: Toggle Menu. "
          "Hold: Windowing mode (Focus/Move/Resize windows)");
        static t_CKINT ImGuiKey_GamepadFaceRight = 634;
        QUERY->add_svar(QUERY, "int", "GamepadFaceRight", true,
                        &ImGuiKey_GamepadFaceRight);
        QUERY->doc_var(QUERY,
                       "B (Xbox)         A (Switch)   Circle (PS)         Cancel / "
                       "Close / Exit");
        static t_CKINT ImGuiKey_GamepadFaceUp = 635;
        QUERY->add_svar(QUERY, "int", "GamepadFaceUp", true, &ImGuiKey_GamepadFaceUp);
        QUERY->doc_var(QUERY,
                       "Y (Xbox)         X (Switch)   Triangle (PS)       Text "
                       "Input / On-screen Keyboard");
        static t_CKINT ImGuiKey_GamepadFaceDown = 636;
        QUERY->add_svar(QUERY, "int", "GamepadFaceDown", true,
                        &ImGuiKey_GamepadFaceDown);
        QUERY->doc_var(QUERY,
                       "A (Xbox)         B (Switch)   Cross (PS)          Activate "
                       "/ Open / Toggle / Tweak");
        static t_CKINT ImGuiKey_GamepadDpadLeft = 637;
        QUERY->add_svar(QUERY, "int", "GamepadDpadLeft", true,
                        &ImGuiKey_GamepadDpadLeft);
        QUERY->doc_var(QUERY,
                       "D-pad Left                                        Move / "
                       "Tweak / Resize Window (in Windowing mode)");
        static t_CKINT ImGuiKey_GamepadDpadRight = 638;
        QUERY->add_svar(QUERY, "int", "GamepadDpadRight", true,
                        &ImGuiKey_GamepadDpadRight);
        QUERY->doc_var(QUERY,
                       "D-pad Right                                       Move / "
                       "Tweak / Resize Window (in Windowing mode)");
        static t_CKINT ImGuiKey_GamepadDpadUp = 639;
        QUERY->add_svar(QUERY, "int", "GamepadDpadUp", true, &ImGuiKey_GamepadDpadUp);
        QUERY->doc_var(QUERY,
                       "D-pad Up                                          Move / "
                       "Tweak / Resize Window (in Windowing mode)");
        static t_CKINT ImGuiKey_GamepadDpadDown = 640;
        QUERY->add_svar(QUERY, "int", "GamepadDpadDown", true,
                        &ImGuiKey_GamepadDpadDown);
        QUERY->doc_var(QUERY,
                       "D-pad Down                                        Move / "
                       "Tweak / Resize Window (in Windowing mode)");
        static t_CKINT ImGuiKey_GamepadL1 = 641;
        QUERY->add_svar(QUERY, "int", "GamepadL1", true, &ImGuiKey_GamepadL1);
        QUERY->doc_var(QUERY,
                       "L Bumper (Xbox)  L (Switch)   L1 (PS)             Tweak "
                       "Slower / Focus Previous (in Windowing mode)");
        static t_CKINT ImGuiKey_GamepadR1 = 642;
        QUERY->add_svar(QUERY, "int", "GamepadR1", true, &ImGuiKey_GamepadR1);
        QUERY->doc_var(QUERY,
                       "R Bumper (Xbox)  R (Switch)   R1 (PS)             Tweak "
                       "Faster / Focus Next (in Windowing mode)");
        static t_CKINT ImGuiKey_GamepadL2 = 643;
        QUERY->add_svar(QUERY, "int", "GamepadL2", true, &ImGuiKey_GamepadL2);
        QUERY->doc_var(QUERY, "L Trig. (Xbox)   ZL (Switch)  L2 (PS) [Analog]");
        static t_CKINT ImGuiKey_GamepadR2 = 644;
        QUERY->add_svar(QUERY, "int", "GamepadR2", true, &ImGuiKey_GamepadR2);
        QUERY->doc_var(QUERY, "R Trig. (Xbox)   ZR (Switch)  R2 (PS) [Analog]");
        static t_CKINT ImGuiKey_GamepadL3 = 645;
        QUERY->add_svar(QUERY, "int", "GamepadL3", true, &ImGuiKey_GamepadL3);
        QUERY->doc_var(QUERY, "L Stick (Xbox)   L3 (Switch)  L3 (PS)");
        static t_CKINT ImGuiKey_GamepadR3 = 646;
        QUERY->add_svar(QUERY, "int", "GamepadR3", true, &ImGuiKey_GamepadR3);
        QUERY->doc_var(QUERY, "R Stick (Xbox)   R3 (Switch)  R3 (PS)");
        static t_CKINT ImGuiKey_GamepadLStickLeft = 647;
        QUERY->add_svar(QUERY, "int", "GamepadLStickLeft", true,
                        &ImGuiKey_GamepadLStickLeft);
        QUERY->doc_var(QUERY,
                       "[Analog]                                          Move "
                       "Window (in Windowing mode)");
        static t_CKINT ImGuiKey_GamepadLStickRight = 648;
        QUERY->add_svar(QUERY, "int", "GamepadLStickRight", true,
                        &ImGuiKey_GamepadLStickRight);
        QUERY->doc_var(QUERY,
                       "[Analog]                                          Move "
                       "Window (in Windowing mode)");
        static t_CKINT ImGuiKey_GamepadLStickUp = 649;
        QUERY->add_svar(QUERY, "int", "GamepadLStickUp", true,
                        &ImGuiKey_GamepadLStickUp);
        QUERY->doc_var(QUERY,
                       "[Analog]                                          Move "
                       "Window (in Windowing mode)");
        static t_CKINT ImGuiKey_GamepadLStickDown = 650;
        QUERY->add_svar(QUERY, "int", "GamepadLStickDown", true,
                        &ImGuiKey_GamepadLStickDown);
        QUERY->doc_var(QUERY,
                       "[Analog]                                          Move "
                       "Window (in Windowing mode)");
        static t_CKINT ImGuiKey_GamepadRStickLeft = 651;
        QUERY->add_svar(QUERY, "int", "GamepadRStickLeft", true,
                        &ImGuiKey_GamepadRStickLeft);
        QUERY->doc_var(QUERY, "[Analog]");
        static t_CKINT ImGuiKey_GamepadRStickRight = 652;
        QUERY->add_svar(QUERY, "int", "GamepadRStickRight", true,
                        &ImGuiKey_GamepadRStickRight);
        QUERY->doc_var(QUERY, "[Analog]");
        static t_CKINT ImGuiKey_GamepadRStickUp = 653;
        QUERY->add_svar(QUERY, "int", "GamepadRStickUp", true,
                        &ImGuiKey_GamepadRStickUp);
        QUERY->doc_var(QUERY, "[Analog]");
        static t_CKINT ImGuiKey_GamepadRStickDown = 654;
        QUERY->add_svar(QUERY, "int", "GamepadRStickDown", true,
                        &ImGuiKey_GamepadRStickDown);
        QUERY->doc_var(QUERY, "[Analog]");
        static t_CKINT ImGuiKey_MouseLeft = 655;
        QUERY->add_svar(QUERY, "int", "MouseLeft", true, &ImGuiKey_MouseLeft);
        static t_CKINT ImGuiKey_MouseRight = 656;
        QUERY->add_svar(QUERY, "int", "MouseRight", true, &ImGuiKey_MouseRight);
        static t_CKINT ImGuiKey_MouseMiddle = 657;
        QUERY->add_svar(QUERY, "int", "MouseMiddle", true, &ImGuiKey_MouseMiddle);
        static t_CKINT ImGuiKey_MouseX1 = 658;
        QUERY->add_svar(QUERY, "int", "MouseX1", true, &ImGuiKey_MouseX1);
        static t_CKINT ImGuiKey_MouseX2 = 659;
        QUERY->add_svar(QUERY, "int", "MouseX2", true, &ImGuiKey_MouseX2);
        static t_CKINT ImGuiKey_MouseWheelX = 660;
        QUERY->add_svar(QUERY, "int", "MouseWheelX", true, &ImGuiKey_MouseWheelX);
        static t_CKINT ImGuiKey_MouseWheelY = 661;
        QUERY->add_svar(QUERY, "int", "MouseWheelY", true, &ImGuiKey_MouseWheelY);
        static t_CKINT ImGuiMod_None = 0;
        QUERY->add_svar(QUERY, "int", "Mod_None", true, &ImGuiMod_None);
        static t_CKINT ImGuiMod_Ctrl = 4096;
        QUERY->add_svar(QUERY, "int", "Mod_Ctrl", true, &ImGuiMod_Ctrl);
        QUERY->doc_var(QUERY, "Ctrl (non-macOS), Cmd (macOS)");
        static t_CKINT ImGuiMod_Shift = 8192;
        QUERY->add_svar(QUERY, "int", "Mod_Shift", true, &ImGuiMod_Shift);
        QUERY->doc_var(QUERY, "Shift");
        static t_CKINT ImGuiMod_Alt = 16384;
        QUERY->add_svar(QUERY, "int", "Mod_Alt", true, &ImGuiMod_Alt);
        QUERY->doc_var(QUERY, "Option/Menu");
        static t_CKINT ImGuiMod_Super = 32768;
        QUERY->add_svar(QUERY, "int", "Mod_Super", true, &ImGuiMod_Super);
        QUERY->doc_var(QUERY, "Windows/Super (non-macOS), Ctrl (macOS)");
        static t_CKINT ImGuiKey_KeysData_SIZE = 154;
        QUERY->add_svar(QUERY, "int", "KeysData_SIZE", true, &ImGuiKey_KeysData_SIZE);
        QUERY->doc_var(QUERY, "Size of KeysData[]: only hold named keys");
        static t_CKINT ImGuiKey_KeysData_OFFSET = 512;
        QUERY->add_svar(QUERY, "int", "KeysData_OFFSET", true,
                        &ImGuiKey_KeysData_OFFSET);
        QUERY->doc_var(QUERY,
                       "Accesses to io.KeysData[] must use (key - "
                       "ImGuiKey_KeysData_OFFSET) index.");
        QUERY->end_class(QUERY);
    } // UI_Key

    { // UI_ConfigFlags
        QUERY->begin_class(QUERY, "UI_ConfigFlags", "Object");
        QUERY->doc_class(QUERY,
                         "Configuration flags stored in io.ConfigFlags. Set by "
                         "user/application..\n");
        static t_CKINT ImGuiConfigFlags_None = 0;
        QUERY->add_svar(QUERY, "int", "None", true, &ImGuiConfigFlags_None);
        static t_CKINT ImGuiConfigFlags_NavEnableKeyboard = 1;
        QUERY->add_svar(QUERY, "int", "NavEnableKeyboard", true,
                        &ImGuiConfigFlags_NavEnableKeyboard);
        QUERY->doc_var(QUERY,
                       "Master keyboard navigation enable flag. Enable full "
                       "Tabbing + directional arrows + space/enter to activate.");
        static t_CKINT ImGuiConfigFlags_NavEnableGamepad = 2;
        QUERY->add_svar(QUERY, "int", "NavEnableGamepad", true,
                        &ImGuiConfigFlags_NavEnableGamepad);
        QUERY->doc_var(QUERY,
                       "Master gamepad navigation enable flag. Backend also needs "
                       "to set ImGuiBackendFlags_HasGamepad.");
        static t_CKINT ImGuiConfigFlags_NavEnableSetMousePos = 4;
        QUERY->add_svar(QUERY, "int", "NavEnableSetMousePos", true,
                        &ImGuiConfigFlags_NavEnableSetMousePos);
        QUERY->doc_var(
          QUERY,
          "Instruct navigation to move the mouse cursor. May be useful on "
          "TV/console systems where moving a virtual mouse is awkward. Will update "
          "io.MousePos and set io.WantSetMousePos=true. If enabled you MUST honor "
          "io.WantSetMousePos requests in your backend, otherwise ImGui will react "
          "as if the mouse is jumping around back and forth.");
        static t_CKINT ImGuiConfigFlags_NavNoCaptureKeyboard = 8;
        QUERY->add_svar(QUERY, "int", "NavNoCaptureKeyboard", true,
                        &ImGuiConfigFlags_NavNoCaptureKeyboard);
        QUERY->doc_var(QUERY,
                       "Instruct navigation to not set the io.WantCaptureKeyboard "
                       "flag when io.NavActive is set.");
        static t_CKINT ImGuiConfigFlags_NoMouse = 16;
        QUERY->add_svar(QUERY, "int", "NoMouse", true, &ImGuiConfigFlags_NoMouse);
        QUERY->doc_var(
          QUERY,
          "Instruct imgui to clear mouse position/buttons in NewFrame(). This "
          "allows ignoring the mouse information set by the backend.");
        static t_CKINT ImGuiConfigFlags_NoMouseCursorChange = 32;
        QUERY->add_svar(QUERY, "int", "NoMouseCursorChange", true,
                        &ImGuiConfigFlags_NoMouseCursorChange);
        QUERY->doc_var(
          QUERY,
          "Instruct backend to not alter mouse cursor shape and visibility. Use if "
          "the backend cursor changes are interfering with yours and you don't "
          "want to use SetMouseCursor() to change mouse cursor. You may want to "
          "honor requests from imgui by reading GetMouseCursor() yourself "
          "instead.");
        static t_CKINT ImGuiConfigFlags_DockingEnable = 64;
        QUERY->add_svar(QUERY, "int", "DockingEnable", true,
                        &ImGuiConfigFlags_DockingEnable);
        QUERY->doc_var(QUERY, "Docking enable flags.");
        static t_CKINT ImGuiConfigFlags_ViewportsEnable = 1024;
        QUERY->add_svar(QUERY, "int", "ViewportsEnable", true,
                        &ImGuiConfigFlags_ViewportsEnable);
        QUERY->doc_var(
          QUERY,
          "Viewport enable flags (require both "
          "ImGuiBackendFlags_PlatformHasViewports + "
          "ImGuiBackendFlags_RendererHasViewports set by the respective backends)");
        static t_CKINT ImGuiConfigFlags_DpiEnableScaleViewports = 16384;
        QUERY->add_svar(QUERY, "int", "DpiEnableScaleViewports", true,
                        &ImGuiConfigFlags_DpiEnableScaleViewports);
        QUERY->doc_var(
          QUERY,
          "[BETA: Don't use] FIXME-DPI: Reposition and resize imgui windows when "
          "the DpiScale of a viewport changed (mostly useful for the main viewport "
          "hosting other window). Note that resizing the main window itself is up "
          "to your application.");
        static t_CKINT ImGuiConfigFlags_DpiEnableScaleFonts = 32768;
        QUERY->add_svar(QUERY, "int", "DpiEnableScaleFonts", true,
                        &ImGuiConfigFlags_DpiEnableScaleFonts);
        QUERY->doc_var(QUERY,
                       "[BETA: Don't use] FIXME-DPI: Request bitmap-scaled fonts "
                       "to match DpiScale. This is a very low-quality workaround. "
                       "The correct way to handle DPI is _currently_ to replace "
                       "the atlas and/or fonts in the Platform_OnChangedViewport "
                       "callback, but this is all early work in progress.");
        static t_CKINT ImGuiConfigFlags_IsSRGB = 1048576;
        QUERY->add_svar(QUERY, "int", "IsSRGB", true, &ImGuiConfigFlags_IsSRGB);
        QUERY->doc_var(QUERY, "Application is SRGB-aware.");
        static t_CKINT ImGuiConfigFlags_IsTouchScreen = 2097152;
        QUERY->add_svar(QUERY, "int", "IsTouchScreen", true,
                        &ImGuiConfigFlags_IsTouchScreen);
        QUERY->doc_var(QUERY,
                       "Application is using a touch screen instead of a mouse.");
        QUERY->end_class(QUERY);
    } // UI_ConfigFlags

    { // UI_BackendFlags
        QUERY->begin_class(QUERY, "UI_BackendFlags", "Object");
        QUERY->doc_class(QUERY,
                         "Backend capabilities flags stored in io.BackendFlags. "
                         "Set by imgui_impl_xxx or custom backend..\n");
        static t_CKINT ImGuiBackendFlags_None = 0;
        QUERY->add_svar(QUERY, "int", "None", true, &ImGuiBackendFlags_None);
        static t_CKINT ImGuiBackendFlags_HasGamepad = 1;
        QUERY->add_svar(QUERY, "int", "HasGamepad", true,
                        &ImGuiBackendFlags_HasGamepad);
        QUERY->doc_var(
          QUERY, "Backend Platform supports gamepad and currently has one connected.");
        static t_CKINT ImGuiBackendFlags_HasMouseCursors = 2;
        QUERY->add_svar(QUERY, "int", "HasMouseCursors", true,
                        &ImGuiBackendFlags_HasMouseCursors);
        QUERY->doc_var(QUERY,
                       "Backend Platform supports honoring GetMouseCursor() value "
                       "to change the OS cursor shape.");
        static t_CKINT ImGuiBackendFlags_HasSetMousePos = 4;
        QUERY->add_svar(QUERY, "int", "HasSetMousePos", true,
                        &ImGuiBackendFlags_HasSetMousePos);
        QUERY->doc_var(QUERY,
                       "Backend Platform supports io.WantSetMousePos requests to "
                       "reposition the OS mouse position (only used if "
                       "ImGuiConfigFlags_NavEnableSetMousePos is set).");
        static t_CKINT ImGuiBackendFlags_RendererHasVtxOffset = 8;
        QUERY->add_svar(QUERY, "int", "RendererHasVtxOffset", true,
                        &ImGuiBackendFlags_RendererHasVtxOffset);
        QUERY->doc_var(
          QUERY,
          "Backend Renderer supports ImDrawCmd::VtxOffset. This enables output of "
          "large meshes (64K+ vertices) while still using 16-bit indices.");
        static t_CKINT ImGuiBackendFlags_PlatformHasViewports = 1024;
        QUERY->add_svar(QUERY, "int", "PlatformHasViewports", true,
                        &ImGuiBackendFlags_PlatformHasViewports);
        QUERY->doc_var(QUERY, "Backend Platform supports multiple viewports.");
        static t_CKINT ImGuiBackendFlags_HasMouseHoveredViewport = 2048;
        QUERY->add_svar(QUERY, "int", "HasMouseHoveredViewport", true,
                        &ImGuiBackendFlags_HasMouseHoveredViewport);
        QUERY->doc_var(
          QUERY,
          "Backend Platform supports calling io.AddMouseViewportEvent() with the "
          "viewport under the mouse. IF POSSIBLE, ignore viewports with the "
          "ImGuiViewportFlags_NoInputs flag (Win32 backend, GLFW 3.30+ backend can "
          "do this, SDL backend cannot). If this cannot be done, Dear ImGui needs "
          "to use a flawed heuristic to find the viewport under.");
        static t_CKINT ImGuiBackendFlags_RendererHasViewports = 4096;
        QUERY->add_svar(QUERY, "int", "RendererHasViewports", true,
                        &ImGuiBackendFlags_RendererHasViewports);
        QUERY->doc_var(QUERY, "Backend Renderer supports multiple viewports.");
        QUERY->end_class(QUERY);
    } // UI_BackendFlags

    QUERY->begin_class(QUERY, "UI_MouseCursor", "Object");
    QUERY->doc_class(
      QUERY,
      "Enumeration for GetMouseCursor().\nUser code may request backend to "
      "display given cursor by calling SetMouseCursor(), which is why we have "
      "some cursors that are marked unused here.\n");
    static t_CKINT ImGuiMouseCursor_None = -1;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiMouseCursor_None);
    static t_CKINT ImGuiMouseCursor_Arrow = 0;
    QUERY->add_svar(QUERY, "int", "Arrow", true, &ImGuiMouseCursor_Arrow);
    static t_CKINT ImGuiMouseCursor_TextInput = 1;
    QUERY->add_svar(QUERY, "int", "TextInput", true, &ImGuiMouseCursor_TextInput);
    QUERY->doc_var(QUERY, "When hovering over InputText, etc.");
    static t_CKINT ImGuiMouseCursor_ResizeAll = 2;
    QUERY->add_svar(QUERY, "int", "ResizeAll", true, &ImGuiMouseCursor_ResizeAll);
    QUERY->doc_var(QUERY, "(Unused by Dear ImGui functions)");
    static t_CKINT ImGuiMouseCursor_ResizeNS = 3;
    QUERY->add_svar(QUERY, "int", "ResizeNS", true, &ImGuiMouseCursor_ResizeNS);
    QUERY->doc_var(QUERY, "When hovering over a horizontal border");
    static t_CKINT ImGuiMouseCursor_ResizeEW = 4;
    QUERY->add_svar(QUERY, "int", "ResizeEW", true, &ImGuiMouseCursor_ResizeEW);
    QUERY->doc_var(QUERY, "When hovering over a vertical border or a column");
    static t_CKINT ImGuiMouseCursor_ResizeNESW = 5;
    QUERY->add_svar(QUERY, "int", "ResizeNESW", true, &ImGuiMouseCursor_ResizeNESW);
    QUERY->doc_var(QUERY, "When hovering over the bottom-left corner of a window");
    static t_CKINT ImGuiMouseCursor_ResizeNWSE = 6;
    QUERY->add_svar(QUERY, "int", "ResizeNWSE", true, &ImGuiMouseCursor_ResizeNWSE);
    QUERY->doc_var(QUERY, "When hovering over the bottom-right corner of a window");
    static t_CKINT ImGuiMouseCursor_Hand = 7;
    QUERY->add_svar(QUERY, "int", "Hand", true, &ImGuiMouseCursor_Hand);
    QUERY->doc_var(QUERY, "(Unused by Dear ImGui functions. Use for e.g. hyperlinks)");
    static t_CKINT ImGuiMouseCursor_NotAllowed = 8;
    QUERY->add_svar(QUERY, "int", "NotAllowed", true, &ImGuiMouseCursor_NotAllowed);
    QUERY->doc_var(QUERY,
                   "When hovering something with disallowed interaction. "
                   "Usually a crossed circle.");
    static t_CKINT ImGuiMouseCursor_COUNT = 9;
    QUERY->add_svar(QUERY, "int", "COUNT", true, &ImGuiMouseCursor_COUNT);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_ViewportFlags", "Object");
    QUERY->doc_class(QUERY,
                     "Flags stored in ImGuiViewport::Flags, giving indications "
                     "to the platform backends..\n");
    static t_CKINT ImGuiViewportFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiViewportFlags_None);
    static t_CKINT ImGuiViewportFlags_IsPlatformWindow = 1;
    QUERY->add_svar(QUERY, "int", "IsPlatformWindow", true,
                    &ImGuiViewportFlags_IsPlatformWindow);
    QUERY->doc_var(QUERY, "Represent a Platform Window");
    static t_CKINT ImGuiViewportFlags_IsPlatformMonitor = 2;
    QUERY->add_svar(QUERY, "int", "IsPlatformMonitor", true,
                    &ImGuiViewportFlags_IsPlatformMonitor);
    QUERY->doc_var(QUERY, "Represent a Platform Monitor (unused yet)");
    static t_CKINT ImGuiViewportFlags_OwnedByApp = 4;
    QUERY->add_svar(QUERY, "int", "OwnedByApp", true, &ImGuiViewportFlags_OwnedByApp);
    QUERY->doc_var(QUERY,
                   "Platform Window: Was created/managed by the user "
                   "application? (rather than our backend)");
    static t_CKINT ImGuiViewportFlags_NoDecoration = 8;
    QUERY->add_svar(QUERY, "int", "NoDecoration", true,
                    &ImGuiViewportFlags_NoDecoration);
    QUERY->doc_var(
      QUERY,
      "Platform Window: Disable platform decorations: title bar, borders, etc. "
      "(generally set all windows, but if ImGuiConfigFlags_ViewportsDecoration "
      "is set we only set this on popups/tooltips)");
    static t_CKINT ImGuiViewportFlags_NoTaskBarIcon = 16;
    QUERY->add_svar(QUERY, "int", "NoTaskBarIcon", true,
                    &ImGuiViewportFlags_NoTaskBarIcon);
    QUERY->doc_var(QUERY,
                   "Platform Window: Disable platform task bar icon (generally "
                   "set on popups/tooltips, or all windows if "
                   "ImGuiConfigFlags_ViewportsNoTaskBarIcon is set)");
    static t_CKINT ImGuiViewportFlags_NoFocusOnAppearing = 32;
    QUERY->add_svar(QUERY, "int", "NoFocusOnAppearing", true,
                    &ImGuiViewportFlags_NoFocusOnAppearing);
    QUERY->doc_var(QUERY, "Platform Window: Don't take focus when created.");
    static t_CKINT ImGuiViewportFlags_NoFocusOnClick = 64;
    QUERY->add_svar(QUERY, "int", "NoFocusOnClick", true,
                    &ImGuiViewportFlags_NoFocusOnClick);
    QUERY->doc_var(QUERY, "Platform Window: Don't take focus when clicked on.");
    static t_CKINT ImGuiViewportFlags_NoInputs = 128;
    QUERY->add_svar(QUERY, "int", "NoInputs", true, &ImGuiViewportFlags_NoInputs);
    QUERY->doc_var(QUERY,
                   "Platform Window: Make mouse pass through so we can drag "
                   "this window while peaking behind it.");
    static t_CKINT ImGuiViewportFlags_NoRendererClear = 256;
    QUERY->add_svar(QUERY, "int", "NoRendererClear", true,
                    &ImGuiViewportFlags_NoRendererClear);
    QUERY->doc_var(QUERY,
                   "Platform Window: Renderer doesn't need to clear the "
                   "framebuffer ahead (because we will fill it entirely).");
    static t_CKINT ImGuiViewportFlags_NoAutoMerge = 512;
    QUERY->add_svar(QUERY, "int", "NoAutoMerge", true, &ImGuiViewportFlags_NoAutoMerge);
    QUERY->doc_var(QUERY,
                   "Platform Window: Avoid merging this window into another "
                   "host window. This can only be set via ImGuiWindowClass "
                   "viewport flags override (because we need to now ahead if "
                   "we are going to create a viewport in the first place!).");
    static t_CKINT ImGuiViewportFlags_TopMost = 1024;
    QUERY->add_svar(QUERY, "int", "TopMost", true, &ImGuiViewportFlags_TopMost);
    QUERY->doc_var(QUERY, "Platform Window: Display on top (for tooltips only).");
    static t_CKINT ImGuiViewportFlags_CanHostOtherWindows = 2048;
    QUERY->add_svar(QUERY, "int", "CanHostOtherWindows", true,
                    &ImGuiViewportFlags_CanHostOtherWindows);
    QUERY->doc_var(
      QUERY,
      "Viewport can host multiple imgui windows (secondary viewports are "
      "associated to a single window).  FIXME: In practice there's still "
      "probably code making the assumption that this is always and only on the "
      "MainViewport. Will fix once we add support for \"no main viewport\".");
    static t_CKINT ImGuiViewportFlags_IsMinimized = 4096;
    QUERY->add_svar(QUERY, "int", "IsMinimized", true, &ImGuiViewportFlags_IsMinimized);
    QUERY->doc_var(
      QUERY,
      "Platform Window: Window is minimized, can skip render. When minimized "
      "we tend to avoid using the viewport pos/size for clipping window or "
      "testing if they are contained in the viewport.");
    static t_CKINT ImGuiViewportFlags_IsFocused = 8192;
    QUERY->add_svar(QUERY, "int", "IsFocused", true, &ImGuiViewportFlags_IsFocused);
    QUERY->doc_var(QUERY,
                   "Platform Window: Window is focused (last call to "
                   "Platform_GetWindowFocus() returned true)");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_FocusedFlags", "Object");
    QUERY->doc_class(QUERY, "Flags for ImGui::IsWindowFocused().\n");
    static t_CKINT ImGuiFocusedFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiFocusedFlags_None);
    static t_CKINT ImGuiFocusedFlags_ChildWindows = 1;
    QUERY->add_svar(QUERY, "int", "ChildWindows", true,
                    &ImGuiFocusedFlags_ChildWindows);
    QUERY->doc_var(QUERY, "Return true if any children of the window is focused");
    static t_CKINT ImGuiFocusedFlags_RootWindow = 2;
    QUERY->add_svar(QUERY, "int", "RootWindow", true, &ImGuiFocusedFlags_RootWindow);
    QUERY->doc_var(QUERY,
                   "Test from root window (top most parent of the current hierarchy)");
    static t_CKINT ImGuiFocusedFlags_AnyWindow = 4;
    QUERY->add_svar(QUERY, "int", "AnyWindow", true, &ImGuiFocusedFlags_AnyWindow);
    QUERY->doc_var(
      QUERY,
      "Return true if any window is focused. Important: If you are trying to "
      "tell how to dispatch your low-level inputs, do NOT use this. Use "
      "'io.WantCaptureMouse' instead! Please read the FAQ!");
    static t_CKINT ImGuiFocusedFlags_NoPopupHierarchy = 8;
    QUERY->add_svar(QUERY, "int", "NoPopupHierarchy", true,
                    &ImGuiFocusedFlags_NoPopupHierarchy);
    QUERY->doc_var(
      QUERY,
      "Do not consider popup hierarchy (do not treat popup emitter as parent "
      "of popup) (when used with _ChildWindows or _RootWindow)");
    static t_CKINT ImGuiFocusedFlags_DockHierarchy = 16;
    QUERY->add_svar(QUERY, "int", "DockHierarchy", true,
                    &ImGuiFocusedFlags_DockHierarchy);
    QUERY->doc_var(
      QUERY,
      "Consider docking hierarchy (treat dockspace host as parent of docked "
      "window) (when used with _ChildWindows or _RootWindow)");
    static t_CKINT ImGuiFocusedFlags_RootAndChildWindows = 3;
    QUERY->add_svar(QUERY, "int", "RootAndChildWindows", true,
                    &ImGuiFocusedFlags_RootAndChildWindows);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_HoveredFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for ImGui::IsItemHovered(), ImGui::IsWindowHovered().\nNote: if "
      "you are trying to check whether your mouse should be dispatched to Dear "
      "ImGui or to your app, you should use 'io.WantCaptureMouse' instead! "
      "Please read the FAQ!.\nNote: windows with the ImGuiWindowFlags_NoInputs "
      "flag are ignored by IsWindowHovered() calls..\n");
    static t_CKINT ImGuiHoveredFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiHoveredFlags_None);
    QUERY->doc_var(QUERY,
                   "Return true if directly over the item/window, not "
                   "obstructed by another window, not obstructed by an active "
                   "popup or modal blocking inputs under them.");
    static t_CKINT ImGuiHoveredFlags_ChildWindows = 1;
    QUERY->add_svar(QUERY, "int", "ChildWindows", true,
                    &ImGuiHoveredFlags_ChildWindows);
    QUERY->doc_var(QUERY,
                   "IsWindowHovered() only: Return true if any children of the "
                   "window is hovered");
    static t_CKINT ImGuiHoveredFlags_RootWindow = 2;
    QUERY->add_svar(QUERY, "int", "RootWindow", true, &ImGuiHoveredFlags_RootWindow);
    QUERY->doc_var(QUERY,
                   "IsWindowHovered() only: Test from root window (top most "
                   "parent of the current hierarchy)");
    static t_CKINT ImGuiHoveredFlags_AnyWindow = 4;
    QUERY->add_svar(QUERY, "int", "AnyWindow", true, &ImGuiHoveredFlags_AnyWindow);
    QUERY->doc_var(QUERY,
                   "IsWindowHovered() only: Return true if any window is hovered");
    static t_CKINT ImGuiHoveredFlags_NoPopupHierarchy = 8;
    QUERY->add_svar(QUERY, "int", "NoPopupHierarchy", true,
                    &ImGuiHoveredFlags_NoPopupHierarchy);
    QUERY->doc_var(QUERY,
                   "IsWindowHovered() only: Do not consider popup hierarchy "
                   "(do not treat popup emitter as parent of popup) (when used "
                   "with _ChildWindows or _RootWindow)");
    static t_CKINT ImGuiHoveredFlags_DockHierarchy = 16;
    QUERY->add_svar(QUERY, "int", "DockHierarchy", true,
                    &ImGuiHoveredFlags_DockHierarchy);
    QUERY->doc_var(QUERY,
                   "IsWindowHovered() only: Consider docking hierarchy (treat "
                   "dockspace host as parent of docked window) (when used with "
                   "_ChildWindows or _RootWindow)");
    static t_CKINT ImGuiHoveredFlags_AllowWhenBlockedByPopup = 32;
    QUERY->add_svar(QUERY, "int", "AllowWhenBlockedByPopup", true,
                    &ImGuiHoveredFlags_AllowWhenBlockedByPopup);
    QUERY->doc_var(QUERY,
                   "Return true even if a popup window is normally blocking "
                   "access to this item/window");
    static t_CKINT ImGuiHoveredFlags_AllowWhenBlockedByActiveItem = 128;
    QUERY->add_svar(QUERY, "int", "AllowWhenBlockedByActiveItem", true,
                    &ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    QUERY->doc_var(QUERY,
                   "Return true even if an active item is blocking access to "
                   "this item/window. Useful for Drag and Drop patterns.");
    static t_CKINT ImGuiHoveredFlags_AllowWhenOverlappedByItem = 256;
    QUERY->add_svar(QUERY, "int", "AllowWhenOverlappedByItem", true,
                    &ImGuiHoveredFlags_AllowWhenOverlappedByItem);
    QUERY->doc_var(
      QUERY,
      "IsItemHovered() only: Return true even if the item uses AllowOverlap "
      "mode and is overlapped by another hoverable item.");
    static t_CKINT ImGuiHoveredFlags_AllowWhenOverlappedByWindow = 512;
    QUERY->add_svar(QUERY, "int", "AllowWhenOverlappedByWindow", true,
                    &ImGuiHoveredFlags_AllowWhenOverlappedByWindow);
    QUERY->doc_var(QUERY,
                   "IsItemHovered() only: Return true even if the position is "
                   "obstructed or overlapped by another window.");
    static t_CKINT ImGuiHoveredFlags_AllowWhenDisabled = 1024;
    QUERY->add_svar(QUERY, "int", "AllowWhenDisabled", true,
                    &ImGuiHoveredFlags_AllowWhenDisabled);
    QUERY->doc_var(QUERY,
                   "IsItemHovered() only: Return true even if the item is disabled");
    static t_CKINT ImGuiHoveredFlags_NoNavOverride = 2048;
    QUERY->add_svar(QUERY, "int", "NoNavOverride", true,
                    &ImGuiHoveredFlags_NoNavOverride);
    QUERY->doc_var(QUERY,
                   "IsItemHovered() only: Disable using gamepad/keyboard "
                   "navigation state when active, always query mouse");
    static t_CKINT ImGuiHoveredFlags_AllowWhenOverlapped = 768;
    QUERY->add_svar(QUERY, "int", "AllowWhenOverlapped", true,
                    &ImGuiHoveredFlags_AllowWhenOverlapped);
    static t_CKINT ImGuiHoveredFlags_RectOnly = 928;
    QUERY->add_svar(QUERY, "int", "RectOnly", true, &ImGuiHoveredFlags_RectOnly);
    static t_CKINT ImGuiHoveredFlags_RootAndChildWindows = 3;
    QUERY->add_svar(QUERY, "int", "RootAndChildWindows", true,
                    &ImGuiHoveredFlags_RootAndChildWindows);
    static t_CKINT ImGuiHoveredFlags_ForTooltip = 4096;
    QUERY->add_svar(QUERY, "int", "ForTooltip", true, &ImGuiHoveredFlags_ForTooltip);
    QUERY->doc_var(QUERY,
                   "Shortcut for standard flags when using IsItemHovered() + "
                   "SetTooltip() sequence.");
    static t_CKINT ImGuiHoveredFlags_Stationary = 8192;
    QUERY->add_svar(QUERY, "int", "Stationary", true, &ImGuiHoveredFlags_Stationary);
    QUERY->doc_var(
      QUERY,
      "Require mouse to be stationary for style.HoverStationaryDelay (~0.15 "
      "sec) _at least one time_. After this, can move on same item/window. "
      "Using the stationary test tends to reduces the need for a long delay.");
    static t_CKINT ImGuiHoveredFlags_DelayNone = 16384;
    QUERY->add_svar(QUERY, "int", "DelayNone", true, &ImGuiHoveredFlags_DelayNone);
    QUERY->doc_var(QUERY,
                   "IsItemHovered() only: Return true immediately (default). "
                   "As this is the default you generally ignore this.");
    static t_CKINT ImGuiHoveredFlags_DelayShort = 32768;
    QUERY->add_svar(QUERY, "int", "DelayShort", true, &ImGuiHoveredFlags_DelayShort);
    QUERY->doc_var(
      QUERY,
      "IsItemHovered() only: Return true after style.HoverDelayShort elapsed "
      "(~0.15 sec) (shared between items) + requires mouse to be stationary "
      "for style.HoverStationaryDelay (once per item).");
    static t_CKINT ImGuiHoveredFlags_DelayNormal = 65536;
    QUERY->add_svar(QUERY, "int", "DelayNormal", true, &ImGuiHoveredFlags_DelayNormal);
    QUERY->doc_var(
      QUERY,
      "IsItemHovered() only: Return true after style.HoverDelayNormal elapsed "
      "(~0.40 sec) (shared between items) + requires mouse to be stationary "
      "for style.HoverStationaryDelay (once per item).");
    static t_CKINT ImGuiHoveredFlags_NoSharedDelay = 131072;
    QUERY->add_svar(QUERY, "int", "NoSharedDelay", true,
                    &ImGuiHoveredFlags_NoSharedDelay);
    QUERY->doc_var(QUERY,
                   "IsItemHovered() only: Disable shared delay system where "
                   "moving from one item to the next keeps the previous timer "
                   "for a short time (standard for tooltips with long delays)");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_DockNodeFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for ImGui::DockSpace(), shared/inherited by child nodes..\n(Some "
      "flags can be applied to individual nodes directly).\nFIXME-DOCK: Also "
      "see ImGuiDockNodeFlagsPrivate_ which may involve using the WIP and "
      "internal DockBuilder api..\n");
    static t_CKINT ImGuiDockNodeFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiDockNodeFlags_None);
    static t_CKINT ImGuiDockNodeFlags_KeepAliveOnly = 1;
    QUERY->add_svar(QUERY, "int", "KeepAliveOnly", true,
                    &ImGuiDockNodeFlags_KeepAliveOnly);
    QUERY->doc_var(
      QUERY,
      "Don't display the dockspace node but keep it alive. Windows docked into "
      "this dockspace node won't be undocked.");
    static t_CKINT ImGuiDockNodeFlags_NoDockingOverCentralNode = 4;
    QUERY->add_svar(QUERY, "int", "NoDockingOverCentralNode", true,
                    &ImGuiDockNodeFlags_NoDockingOverCentralNode);
    QUERY->doc_var(QUERY,
                   "Disable docking over the Central Node, which will be "
                   "always kept empty.");
    static t_CKINT ImGuiDockNodeFlags_PassthruCentralNode = 8;
    QUERY->add_svar(QUERY, "int", "PassthruCentralNode", true,
                    &ImGuiDockNodeFlags_PassthruCentralNode);
    QUERY->doc_var(
      QUERY,
      "Enable passthru dockspace: 1) DockSpace() will render a "
      "ImGuiCol_WindowBg background covering everything excepted the Central "
      "Node when empty. Meaning the host window should probably use "
      "SetNextWindowBgAlpha(0.0f) prior to Begin() when using this. 2) When "
      "Central Node is empty: let inputs pass-through + won't display a "
      "DockingEmptyBg background. See demo for details.");
    static t_CKINT ImGuiDockNodeFlags_NoDockingSplit = 16;
    QUERY->add_svar(QUERY, "int", "NoDockingSplit", true,
                    &ImGuiDockNodeFlags_NoDockingSplit);
    QUERY->doc_var(QUERY, "Disable other windows/nodes from splitting this node.");
    static t_CKINT ImGuiDockNodeFlags_NoResize = 32;
    QUERY->add_svar(QUERY, "int", "NoResize", true, &ImGuiDockNodeFlags_NoResize);
    QUERY->doc_var(
      QUERY,
      "Saved  Disable resizing node using the splitter/separators. Useful with "
      "programmatically setup dockspaces.");
    static t_CKINT ImGuiDockNodeFlags_AutoHideTabBar = 64;
    QUERY->add_svar(QUERY, "int", "AutoHideTabBar", true,
                    &ImGuiDockNodeFlags_AutoHideTabBar);
    QUERY->doc_var(QUERY,
                   "Tab bar will automatically hide when there is a single "
                   "window in the dock node.");
    static t_CKINT ImGuiDockNodeFlags_NoUndocking = 128;
    QUERY->add_svar(QUERY, "int", "NoUndocking", true, &ImGuiDockNodeFlags_NoUndocking);
    QUERY->doc_var(QUERY, "Disable undocking this node.");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_DrawFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for ImDrawList functions.\n(Legacy: bit 0 must always correspond "
      "to ImDrawFlags_Closed to be backward compatible with old API using a "
      "bool. Bits 1..3 must be unused).\n");
    static t_CKINT ImDrawFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImDrawFlags_None);
    static t_CKINT ImDrawFlags_Closed = 1;
    QUERY->add_svar(QUERY, "int", "Closed", true, &ImDrawFlags_Closed);
    QUERY->doc_var(QUERY,
                   "PathStroke(), AddPolyline(): specify that shape should be "
                   "closed (Important: this is always == 1 for legacy reason)");
    static t_CKINT ImDrawFlags_RoundCornersTopLeft = 16;
    QUERY->add_svar(QUERY, "int", "RoundCornersTopLeft", true,
                    &ImDrawFlags_RoundCornersTopLeft);
    QUERY->doc_var(
      QUERY,
      "AddRect(), AddRectFilled(), PathRect(): enable rounding top-left corner "
      "only (when rounding > 0.0f, we default to all corners). Was 0x01.");
    static t_CKINT ImDrawFlags_RoundCornersTopRight = 32;
    QUERY->add_svar(QUERY, "int", "RoundCornersTopRight", true,
                    &ImDrawFlags_RoundCornersTopRight);
    QUERY->doc_var(QUERY,
                   "AddRect(), AddRectFilled(), PathRect(): enable rounding "
                   "top-right corner only (when rounding > 0.0f, we default to "
                   "all corners). Was 0x02.");
    static t_CKINT ImDrawFlags_RoundCornersBottomLeft = 64;
    QUERY->add_svar(QUERY, "int", "RoundCornersBottomLeft", true,
                    &ImDrawFlags_RoundCornersBottomLeft);
    QUERY->doc_var(QUERY,
                   "AddRect(), AddRectFilled(), PathRect(): enable rounding "
                   "bottom-left corner only (when rounding > 0.0f, we default "
                   "to all corners). Was 0x04.");
    static t_CKINT ImDrawFlags_RoundCornersBottomRight = 128;
    QUERY->add_svar(QUERY, "int", "RoundCornersBottomRight", true,
                    &ImDrawFlags_RoundCornersBottomRight);
    QUERY->doc_var(QUERY,
                   "AddRect(), AddRectFilled(), PathRect(): enable rounding "
                   "bottom-right corner only (when rounding > 0.0f, we default "
                   "to all corners). Wax 0x08.");
    static t_CKINT ImDrawFlags_RoundCornersNone = 256;
    QUERY->add_svar(QUERY, "int", "RoundCornersNone", true,
                    &ImDrawFlags_RoundCornersNone);
    QUERY->doc_var(
      QUERY,
      "AddRect(), AddRectFilled(), PathRect(): disable rounding on all corners "
      "(when rounding > 0.0f). This is NOT zero, NOT an implicit flag!");
    static t_CKINT ImDrawFlags_RoundCornersTop = 48;
    QUERY->add_svar(QUERY, "int", "RoundCornersTop", true,
                    &ImDrawFlags_RoundCornersTop);
    static t_CKINT ImDrawFlags_RoundCornersBottom = 192;
    QUERY->add_svar(QUERY, "int", "RoundCornersBottom", true,
                    &ImDrawFlags_RoundCornersBottom);
    static t_CKINT ImDrawFlags_RoundCornersLeft = 80;
    QUERY->add_svar(QUERY, "int", "RoundCornersLeft", true,
                    &ImDrawFlags_RoundCornersLeft);
    static t_CKINT ImDrawFlags_RoundCornersRight = 160;
    QUERY->add_svar(QUERY, "int", "RoundCornersRight", true,
                    &ImDrawFlags_RoundCornersRight);
    static t_CKINT ImDrawFlags_RoundCornersAll = 240;
    QUERY->add_svar(QUERY, "int", "RoundCornersAll", true,
                    &ImDrawFlags_RoundCornersAll);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_DrawListFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for ImDrawList instance. Those are set automatically by ImGui:: "
      "functions from ImGuiIO settings, and generally not manipulated "
      "directly..\nIt is however possible to temporarily alter flags between "
      "calls to ImDrawList:: functions..\n");
    static t_CKINT ImDrawListFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImDrawListFlags_None);
    static t_CKINT ImDrawListFlags_AntiAliasedLines = 1;
    QUERY->add_svar(QUERY, "int", "AntiAliasedLines", true,
                    &ImDrawListFlags_AntiAliasedLines);
    QUERY->doc_var(
      QUERY,
      "Enable anti-aliased lines/borders (*2 the number of triangles for 1.0f "
      "wide line or lines thin enough to be drawn using textures, otherwise *3 "
      "the number of triangles)");
    static t_CKINT ImDrawListFlags_AntiAliasedLinesUseTex = 2;
    QUERY->add_svar(QUERY, "int", "AntiAliasedLinesUseTex", true,
                    &ImDrawListFlags_AntiAliasedLinesUseTex);
    QUERY->doc_var(QUERY,
                   "Enable anti-aliased lines/borders using textures when "
                   "possible. Require backend to render with bilinear "
                   "filtering (NOT point/nearest filtering).");
    static t_CKINT ImDrawListFlags_AntiAliasedFill = 4;
    QUERY->add_svar(QUERY, "int", "AntiAliasedFill", true,
                    &ImDrawListFlags_AntiAliasedFill);
    QUERY->doc_var(QUERY,
                   "Enable anti-aliased edge around filled shapes (rounded "
                   "rectangles, circles).");
    static t_CKINT ImDrawListFlags_AllowVtxOffset = 8;
    QUERY->add_svar(QUERY, "int", "AllowVtxOffset", true,
                    &ImDrawListFlags_AllowVtxOffset);
    QUERY->doc_var(QUERY,
                   "Can emit 'VtxOffset > 0' to allow large meshes. Set when "
                   "'ImGuiBackendFlags_RendererHasVtxOffset' is enabled.");
    QUERY->end_class(QUERY);

    // Callbacks ----------------------------------------------------------

    BEGIN_CLASS("UI_Callback", "Object");
    DOC_CLASS(
      "Base class for ImGui callback functions. Don't use this class "
      "directly.");
    CTOR(ui_callback_ctor);
    DTOR(ui_callback_dtor);
    END_CLASS();

    QUERY->begin_class(QUERY, "UI_SizeCallbackData", "Object");
    ui_size_callback_data_ptr_offset
      = QUERY->add_mvar(QUERY, "int", "@ui_size_callback_data_ptr", false);
    // QUERY->add_mvar(QUERY, "int", "user_data", false);  // IGNORE user data
    // (chuck provides other ways to access external data)
    QUERY->add_mfun(QUERY, ui_size_callback_data_get_pos, "vec2", "getPos");
    QUERY->doc_func(QUERY, "Read-only window position, for reference.");
    QUERY->add_mfun(QUERY, ui_size_callback_data_get_current_size, "vec2",
                    "currentSize");
    QUERY->doc_func(QUERY, "Read-only current window size.");
    QUERY->add_mfun(QUERY, ui_size_callback_data_get_desired_size, "vec2",
                    "desiredSize");
    QUERY->doc_func(QUERY,
                    "Read-write desired size, based on user's mouse "
                    "position. Write to this field to restrain resizing.");
    QUERY->add_mfun(QUERY, ui_size_callback_data_set_desired_size, "vec2",
                    "desiredSize");
    QUERY->add_arg(QUERY, "vec2", "desired_size");
    QUERY->doc_func(QUERY,
                    "Write desired size, based on user's mouse "
                    "position. Write to this field to restrain resizing.");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_SizeCallback", "UI_Callback");
    QUERY->add_mfun(QUERY, ui_size_callback, "void", "handler");
    QUERY->add_arg(QUERY, "UI_SizeCallbackData", "data");
    QUERY->doc_func(QUERY,
                    "Callback function for ImGui::SetNextWindowSizeConstraints()");
    QUERY->end_class(QUERY);

    // update() vt offset
    ui_size_callback_vt_offset = chugin_setVTableOffset("UI_SizeCallback", "handler");

    BEGIN_CLASS("UI_ComboCallback", "UI_Callback");
    MFUN(ui_combo_callback, "void", "handler");
    ARG("int", "idx");
    END_CLASS();

    ui_combo_callback_vt_offset = chugin_setVTableOffset("UI_ComboCallback", "handler");

    // complex, not yet implemented
    // BEGIN_CLASS("UI_InputTextCallback", "UI_Callback");
    // MFUN(ui_input_text_callback, "void", "handler");
    // ARG("UI_InputTextCallbackData", "data");
    // END_CLASS();

    // chugin_setVTableOffset(&ui_input_text_callback_vt_offset,
    // "UI_InputTextCallback",
    //                        "handler");

    { // ImGui Font
        BEGIN_CLASS("UI_Font", "Object");
        DOC_CLASS(
          "Placeholder class for ImFont. WORK IN PROGRESS. Do not instantiate "
          "directly. Create from UI.addFontFromFileTTF()");

        ui_font_ptr_offset = MVAR("int", "@ui_font_ptr", false);

        END_CLASS();
    } // ImGui Font

    // ImGuiIO ---------------------------------------------------------------------
    {
        BEGIN_CLASS("UI_IO", "Object");

        SFUN(ui_io_get_ConfigFlags, "int", "configFlags");
        DOC_FUNC(
          "default = 0. See UI_ConfigFlags enum. Set by user/application. "
          "Gamepad/keyboard navigation options, etc.");

        SFUN(ui_io_set_ConfigFlags, "void", "configFlags");
        ARG("int", "config_flags");
        DOC_FUNC("See UI_ConfigFlags enum");

        SFUN(ui_io_get_BackendFlags, "int", "backendFlags");
        DOC_FUNC(
          "See UI_BackendFlags enum. Set by backend (imgui_impl_xxx files or custom "
          "backend) to communicate features supported by the backend.");

        SFUN(ui_io_set_BackendFlags, "void", "backendFlags");
        ARG("int", "flags");
        DOC_FUNC(
          "See UI_BackendFlags enum. Set by backend (imgui_impl_xxx files or custom "
          "backend) to communicate features supported by the backend.");

        SFUN(ui_io_get_DisplaySize, "vec2", "displaySize");
        DOC_FUNC(
          "Main display size, in pixels (generally == GetMainViewport()->Size). May "
          "change every frame.");

        SFUN(ui_io_get_DeltaTime, "float", "deltaTime");
        DOC_FUNC("Time elapsed since last frame, in seconds. May change every frame.");

        SFUN(ui_io_get_IniSavingRate, "float", "iniSavingRate");
        DOC_FUNC(
          "default 5.0f. Get minimum time between saving positions/sizes to .ini file, "
          "in seconds.");

        SFUN(ui_io_set_IniSavingRate, "void", "iniSavingRate");
        ARG("float", "rate");
        DOC_FUNC(
          "default 5.0f. Set minimum time between saving positions/sizes to .ini file, "
          "in seconds.");

        SFUN(ui_io_get_IniFilename, "string", "iniFilename");
        DOC_FUNC(
          "Get path to .ini file (important: default \"imgui.ini\" is relative to "
          "current working dir!). Set NULL to disable automatic .ini loading/saving or "
          "if you want to manually call LoadIniSettingsXXX() / SaveIniSettingsXXX() "
          "functions.");

        SFUN(ui_io_set_IniFilename, "void", "iniFilename");
        ARG("string", "filename");
        DOC_FUNC(
          "Set path to .ini file (important: default \"imgui.ini\" is relative to "
          "current working dir!). Set NULL to disable automatic .ini loading/saving or "
          "if you want to manually call LoadIniSettingsXXX() / SaveIniSettingsXXX() "
          "functions.");

        SFUN(ui_io_get_LogFilename, "string", "logFilename");
        DOC_FUNC(
          "default \"imgui_log.txt\". Get path to .log file (default parameter to "
          "ImGui::LogToFile when no file is specified).");

        SFUN(ui_io_set_LogFilename, "void", "logFilename");
        ARG("string", "filename");
        DOC_FUNC(
          "default \"imgui_log.txt\" Set path to .log file (default parameter to "
          "ImGui::LogToFile when no file is specified).");

        SFUN(ui_io_get_FontGlobalScale, "float", "fontGlobalScale");
        DOC_FUNC("Default 1.0f. Get global scale of all fonts");

        SFUN(ui_io_set_FontGlobalScale, "void", "fontGlobalScale");
        ARG("float", "scale");
        DOC_FUNC("Default 1.0f. Set global scale of all fonts");

        END_CLASS();
    }

    { // UI ---------------------------------------------------------------------
        QUERY->begin_class(QUERY, "UI", "Object");
        ADD_EX("deep/ckfxr.ck");
        ADD_EX("deep/pokemon_cries.ck");

        // ChuGL widgets
        SFUN(ui_scenegraph, "void", "scenegraph");
        ARG(SG_CKNames[SG_COMPONENT_TRANSFORM], "root");
        QUERY->doc_func(QUERY,
                        "Scene tree widget. "
                        "View data about a given GGen and all its children. "
                        "A simple version of a typical Editor Scenegraph");

        // config
        SFUN(ui_set_disabled, "void", "disabled");
        ARG("int", "disabled");
        DOC_FUNC(
          "Set whether imgui is disabled (does not call call new frame, does not "
          "submit UI draw lists to renderer)"
          "Do this if your application does not render UI and needs the extra "
          "performance from skipping UI overhead. Saves ~2ms on the render thread."
          "While UI.disabled == true, DO NOT call any other UI functions. Doing so "
          "results in undefined behavior");

        // IO helpers
        SFUN(ui_want_capture_mouse, "int", "wantCaptureMouse");
        DOC_FUNC(
          "When wantCaptureMouse=true, the mouse is interacting with UI widgets, "
          "so you know to discard/hide the mouse inputs from your underlying "
          "application.");

        SFUN(ui_want_capture_keyboard, "int", "wantCaptureKeyboard");
        DOC_FUNC(
          "When wantCaptureKeyboard=true, the keyboard is interacting with UI "
          "widgets, so you know to discard/hide the keyboard inputs from your "
          "underlying application.");

        // SFUN(ui_AddFontFromFileTTF, "UI_Font", "addFontFromFileTTF");
        // ARG("string", "filename");
        // ARG("float", "size_pixels");
        // DOC_FUNC("Load a TTF/OTF font.");

        // Main
        SFUN(ui_get_style, "UI_Style", "getStyle");
        DOC_FUNC(
          "access the Style structure (colors, sizes). Always use "
          "PushStyleColor(), PushStyleVar() to modify style mid-frame!");

        // Demo, Debug, Information
        QUERY->add_sfun(QUERY, ui_ShowDemoWindow, "void", "showDemoWindow");
        QUERY->add_arg(QUERY, "UI_Bool", "p_open");
        QUERY->doc_func(QUERY,
                        "create Demo window. demonstrate most ImGui features. call "
                        "this to learn about the library! try to make it always "
                        "available in your application!");
        QUERY->add_sfun(QUERY, ui_ShowMetricsWindow, "void", "showMetricsWindow");
        QUERY->add_arg(QUERY, "UI_Bool", "p_open");
        QUERY->doc_func(
          QUERY,
          "create Metrics/Debugger window. display Dear ImGui internals: windows, "
          "draw commands, various internal state, etc.");
        QUERY->add_sfun(QUERY, ui_ShowDebugLogWindow, "void", "showDebugLogWindow");
        QUERY->add_arg(QUERY, "UI_Bool", "p_open");
        QUERY->doc_func(QUERY,
                        "create Debug Log window. display a simplified log of "
                        "important dear imgui events.");
        QUERY->add_sfun(QUERY, ui_showStyleEditor, "void", "showStyleEditor");
        QUERY->doc_func(QUERY,
                        "add style selector block (not a window), essentially a "
                        "combo listing the default styles.");

        SFUN(ui_ShowIDStackToolWindowEx, "void", "showIDStackToolWindow");
        ARG("UI_Bool", "p_open");
        DOC_FUNC(
          "create Stack Tool window. hover items with mouse to query "
          "information about the source of their unique ID.");

        SFUN(ui_ShowAboutWindow, "void", "showAboutWindow");
        ARG("UI_Bool", "p_open");
        DOC_FUNC(
          "create About window. display Dear ImGui version, credits and "
          "build/system information.");

        SFUN(ui_ShowStyleSelector, "int", "showStyleSelector");
        ARG("string", "label");
        DOC_FUNC(
          "add style selector block (not a window), essentially a combo "
          "listing the default styles.");

        SFUN(ui_ShowFontSelector, "void", "showFontSelector");
        ARG("string", "label");
        DOC_FUNC(
          "add font selector block (not a window), essentially a combo "
          "listing the loaded fonts.");

        SFUN(ui_ShowUserGuide, "void", "showUserGuide");
        DOC_FUNC(
          "add basic help/info block (not a window): how to manipulate ImGui "
          "as an end-user (mouse/keyboard controls).");

        SFUN(ui_GetVersion, "string", "getVersion");
        DOC_FUNC(
          "get the compiled version string e.g. \"1.80 WIP\" (essentially "
          "the value for IMGUI_VERSION from the compiled version of "
          "imgui.cpp)");

        // Windows
        QUERY->add_sfun(QUERY, ui_begin, "int", "begin");
        QUERY->add_arg(QUERY, "string", "name");
        QUERY->add_arg(QUERY, "UI_Bool", "p_open");
        QUERY->add_arg(QUERY, "int", "flags"); // ImGuiWindowFlags
        DOC_FUNC(
          " `flags` of type UI_WindowFlags.  "
          "- UI.begin() = push window to the stack and start appending to it. UI.end() "
          "= "
          "pop window from the stack. "
          "- Passing 'UI_Bool p_open != NULL' shows a window-closing widget in the "
          "upper-right corner of the window, "
          "  which clicking will set the boolean to false when clicked. "
          "- You may append multiple times to the same window during the same frame by "
          "calling Begin()/End() pairs multiple times. "
          "  Some information such as 'flags' or 'p_open' will only be considered by "
          "the "
          "first call to Begin(). "
          "- Begin() return false to indicate the window is collapsed or fully "
          "clipped, so "
          "you may early out and omit submitting "
          "  anything to the window. Always call a matching End() for each Begin() "
          "call, "
          "regardless of its return value! ");

        SFUN(ui_begin_no_options, "int", "begin");
        ARG("string", "name");
        DOC_FUNC("Equivalent to UI.begin(name, null, 0)");

        QUERY->add_sfun(QUERY, ui_end, "void", "end");

        // Child windows
        QUERY->add_sfun(QUERY, ui_BeginChild, "int", "beginChild");
        QUERY->add_arg(QUERY, "string", "str_id");
        QUERY->add_arg(QUERY, "vec2", "size");        // map ImVec2 --> vec2
        QUERY->add_arg(QUERY, "int", "child_flags");  // ImGuiChildFlags
        QUERY->add_arg(QUERY, "int", "window_flags"); // ImGuiWindowFlags
        DOC_FUNC(
          "Manual sizing (each axis can use a different setting e.g. ImVec2(0.0f, "
          "400.0f)):\n"
          "  == 0.0f: use remaining parent window size for this axis.\n"
          "  > 0.0f: use specified size for this axis.\n"
          "  < 0.0f: right/bottom-align to specified distance from available "
          "content boundaries.\n"
          "Specifying UI_ChildFlags.AutoResizeX or UI_ChildFlags.AutoResizeY"
          "makes the sizing automatic based on child contents.\n"
          "Combining both UI_ChildFlags.AutoResizeX _and_ "
          "UI_ChildFlags.AutoResizeY defeats purpose of a scrolling region and is "
          "NOT recommended.\n"
          "BeginChild() returns false to indicate the window is collapsed or fully "
          "clipped, so you may early out and omit submitting anything to the "
          "window. Always call a matching EndChild() for each BeginChild() call, "
          "regardless of its return value.");

        QUERY->add_sfun(QUERY, ui_EndChild, "void", "endChild");

        // Windows utilities
        SFUN(ui_IsWindowAppearing, "int", "isWindowAppearing");

        SFUN(ui_IsWindowCollapsed, "int", "isWindowCollapsed");

        SFUN(ui_IsWindowFocused, "int", "isWindowFocused");
        ARG("int", "ui_focused_flags");
        DOC_FUNC(
          "is current window focused? or its root/child, depending on flags. "
          "flags are of type UI_FocusedFlags.");

        SFUN(ui_IsWindowHovered, "int", "isWindowHovered");
        ARG("int", "ui_hovered_flags");
        DOC_FUNC(
          "is current window hovered and hoverable (e.g. not blocked by a "
          "popup/modal)? flags are of type UI_HoveredFlags.");

        SFUN(ui_GetWindowDpiScale, "float", "getWindowDpiScale");
        DOC_FUNC(
          "get DPI scale currently associated to the current window's viewport.");

        SFUN(ui_GetWindowPos, "vec2", "getWindowPos");
        DOC_FUNC(
          "get current window position in screen space (note: it is unlikely you "
          "need to use this. Consider using current layout pos instead, "
          "GetCursorScreenPos())");

        SFUN(ui_GetWindowSize, "vec2", "getWindowSize");
        DOC_FUNC(
          "get current window size (note: it is unlikely you need to use this. "
          "Consider using GetCursorScreenPos() and e.g. GetContentRegionAvail() "
          "instead)");

        SFUN(ui_GetWindowWidth, "float", "getWindowWidth");
        DOC_FUNC("get current window width (shortcut for GetWindowSize().x)");

        SFUN(ui_GetWindowHeight, "float", "getWindowHeight");
        DOC_FUNC("get current window height (shortcut for GetWindowSize().y)");

        SFUN(ui_GetWindowViewport, "UI_Viewport", "getWindowViewport");
        DOC_FUNC("get viewport currently associated to the current window.");

        // Window manipulation
        QUERY->add_sfun(QUERY, ui_SetNextWindowPos, "void", "setNextWindowPos");
        QUERY->add_arg(QUERY, "vec2", "pos");
        QUERY->add_arg(QUERY, "int", "cond" /*ImGuiCond*/);
        QUERY->doc_func(QUERY, "Implied pivot = ImVec2(0, 0)");

        QUERY->add_sfun(QUERY, ui_SetNextWindowPosEx, "void", "setNextWindowPosEx");
        QUERY->add_arg(QUERY, "vec2", "pos");
        QUERY->add_arg(QUERY, "int", "cond");
        QUERY->add_arg(QUERY, "vec2", "pivot");
        QUERY->doc_func(QUERY,
                        "set next window position. call before Begin(). "
                        "use pivot=(0.5f,0.5f) to center on given point, etc.");

        QUERY->add_sfun(QUERY, ui_SetNextWindowSize, "void", "setNextWindowSize");
        QUERY->add_arg(QUERY, "vec2", "size");
        QUERY->add_arg(QUERY, "int", "cond");
        QUERY->doc_func(QUERY,
                        "set next window size. set axis to 0.0f to force an "
                        "auto-fit on this axis. call before Begin()");

        QUERY->add_sfun(QUERY, ui_SetNextWindowSizeConstraints, "void",
                        "setNextWindowSizeConstraints");
        QUERY->add_arg(QUERY, "vec2", "size_min");
        QUERY->add_arg(QUERY, "vec2", "size_max");
        QUERY->add_arg(QUERY, "UI_SizeCallback", "custom_callback");
        QUERY->doc_func(QUERY,
                        "set next window size limits. use 0.0f or FLT_MAX if you "
                        "don't want limits. Use -1 for both min and max of same "
                        "axis to preserve current size (which itself is a "
                        "constraint). Use callback to apply non-trivial programmatic "
                        "constraints.");

        QUERY->add_sfun(QUERY, ui_SetNextWindowContentSize, "void",
                        "setNextWindowContentSize");
        QUERY->add_arg(QUERY, "vec2", "size");
        QUERY->doc_func(
          QUERY,
          "set next window content size (~ scrollable client area, which enforce "
          "the range of scrollbars). Not including window decorations (title bar, "
          "menu bar, etc.) nor WindowPadding. set an axis to 0.0f to leave it "
          "automatic. call before Begin()");

        QUERY->add_sfun(QUERY, ui_SetNextWindowCollapsed, "void",
                        "setNextWindowCollapsed");
        QUERY->add_arg(QUERY, "int", "collapsed");
        QUERY->add_arg(QUERY, "int", "cond" /*ImGuiCond*/);
        QUERY->doc_func(QUERY, "set next window collapsed state. call before Begin()");

        QUERY->add_sfun(QUERY, ui_SetNextWindowFocus, "void", "setNextWindowFocus");
        QUERY->doc_func(
          QUERY, "set next window to be focused / top-most. call before Begin()");

        QUERY->add_sfun(QUERY, ui_SetNextWindowScroll, "void", "setNextWindowScroll");
        QUERY->add_arg(QUERY, "vec2", "scroll");
        QUERY->doc_func(QUERY,
                        "set next window scrolling value (use < 0.0f to not affect "
                        "a given axis).");

        QUERY->add_sfun(QUERY, ui_SetNextWindowBgAlpha, "void", "setNextWindowBgAlpha");
        QUERY->add_arg(QUERY, "float", "alpha");
        QUERY->doc_func(
          QUERY,
          "set next window background color alpha. helper to easily override the "
          "Alpha component of ImGuiCol_WindowBg/ChildBg/PopupBg. you may also use "
          "ImGuiWindowFlags_NoBackground.");

        // multiple viewports currently unsupported in webgpu
        // Being added in: https://github.com/ocornut/imgui/pull/7557
        // TODO: after viewport support is added for webgpu native, impl this
        // binding QUERY->add_sfun(QUERY, ui_SetNextWindowViewport, "void",
        //                 "setNextWindowViewport");
        // QUERY->add_arg(QUERY, "int", "viewport_id");
        // QUERY->doc_func(QUERY, "set next window viewport");

        // Content region -------------------------------------------------------
        QUERY->add_sfun(QUERY, ui_GetContentRegionAvail, "vec2",
                        "getContentRegionAvail");
        QUERY->doc_func(QUERY, "equivalent to GetContentRegionMax() - GetCursorPos()");

        QUERY->add_sfun(QUERY, ui_GetContentRegionMax, "vec2", "getContentRegionMax");
        QUERY->doc_func(
          QUERY,
          "current content boundaries (typically window boundaries including "
          "scrolling, or current column boundaries), in windows coordinates");

        QUERY->add_sfun(QUERY, ui_GetWindowContentRegionMin, "vec2",
                        "getWindowContentRegionMin");
        QUERY->doc_func(QUERY,
                        "content boundaries min for the full window (roughly "
                        "(0,0)-Scroll), in window coordinates");

        QUERY->add_sfun(QUERY, ui_GetWindowContentRegionMax, "vec2",
                        "getWindowContentRegionMax");
        QUERY->doc_func(QUERY,
                        "content boundaries max for the full window (roughly "
                        "(0,0)+Size-Scroll) where Size can be overridden with "
                        "SetNextWindowContentSize(), in window coordinates");

        // Windows Scrolling ----------------------------------------------------

        QUERY->add_sfun(QUERY, ui_GetScrollX, "float", "getScrollX");
        QUERY->doc_func(QUERY, "get scrolling amount [0 .. GetScrollMaxX()]");

        QUERY->add_sfun(QUERY, ui_GetScrollY, "float", "getScrollY");
        QUERY->doc_func(QUERY, "get scrolling amount [0 .. GetScrollMaxY()]");

        QUERY->add_sfun(QUERY, ui_SetScrollX, "void", "setScrollX");
        QUERY->add_arg(QUERY, "float", "scroll_x");
        QUERY->doc_func(QUERY, "set scrolling amount [0 .. GetScrollMaxX()]");

        QUERY->add_sfun(QUERY, ui_SetScrollY, "void", "setScrollY");
        QUERY->add_arg(QUERY, "float", "scroll_y");
        QUERY->doc_func(QUERY, "set scrolling amount [0 .. GetScrollMaxY()]");

        QUERY->add_sfun(QUERY, ui_GetScrollMaxX, "float", "getScrollMaxX");
        QUERY->doc_func(QUERY,
                        "get maximum scrolling amount ~~ ContentSize.x - "
                        "WindowSize.x - DecorationsSize.x");

        QUERY->add_sfun(QUERY, ui_GetScrollMaxY, "float", "getScrollMaxY");
        QUERY->doc_func(QUERY,
                        "get maximum scrolling amount ~~ ContentSize.y - "
                        "WindowSize.y - DecorationsSize.y");

        QUERY->add_sfun(QUERY, ui_SetScrollHereX, "void", "setScrollHereX");
        QUERY->add_arg(QUERY, "float", "center_x_ratio");
        QUERY->doc_func(QUERY,
                        "adjust scrolling amount to make current cursor position "
                        "visible. center_x_ratio=0.0: left, 0.5: center, 1.0: "
                        "right. When using to make a \"default/current item\" "
                        "visible, consider using SetItemDefaultFocus() instead.");

        QUERY->add_sfun(QUERY, ui_SetScrollHereY, "void", "setScrollHereY");
        QUERY->add_arg(QUERY, "float", "center_y_ratio");
        QUERY->doc_func(QUERY,
                        "adjust scrolling amount to make current cursor position "
                        "visible. center_y_ratio=0.0: top, 0.5: center, 1.0: "
                        "bottom. When using to make a \"default/current item\" "
                        "visible, consider using SetItemDefaultFocus() instead.");

        QUERY->add_sfun(QUERY, ui_SetScrollFromPosX, "void", "setScrollFromPosX");
        QUERY->add_arg(QUERY, "float", "local_x");
        QUERY->add_arg(QUERY, "float", "center_x_ratio");
        QUERY->doc_func(
          QUERY,
          "adjust scrolling amount to make given position visible. Generally "
          "GetCursorStartPos() + offset to compute a valid position.");

        QUERY->add_sfun(QUERY, ui_SetScrollFromPosY, "void", "setScrollFromPosY");
        QUERY->add_arg(QUERY, "float", "local_y");
        QUERY->add_arg(QUERY, "float", "center_y_ratio");
        QUERY->doc_func(
          QUERY,
          "adjust scrolling amount to make given position visible. Generally "
          "GetCursorStartPos() + offset to compute a valid position.");

        // Parameters stacks (shared) ------------------------------------------
        // SFUN(ui_PushFont, "void", "pushFont");
        // ARG("UI_Font", "font");
        // DOC_FUNC(
        //   "Add a new font to use. Use NULL as a shortcut to push the default font");

        // SFUN(ui_PopFont, "void", "popFont");
        // DOC_FUNC("Pop the font stack.");

        QUERY->add_sfun(QUERY, ui_PushStyleColorImVec4, "void", "pushStyleColor");
        QUERY->add_arg(QUERY, "int", "idx" /*ImGuiCol*/);
        QUERY->add_arg(QUERY, "vec4", "color");
        QUERY->doc_func(QUERY, "parameter idx an enum of type UI_Color");

        SFUN(ui_PushStyleColorImVec3, "void", "pushStyleColor");
        ARG("int", "idx" /*ImGuiCol*/);
        ARG("vec3", "color");
        DOC_FUNC("parameter idx an enum of type UI_Color");

        QUERY->add_sfun(QUERY, ui_PopStyleColor, "void", "popStyleColor");
        QUERY->doc_func(QUERY, "implied count = 1");

        QUERY->add_sfun(QUERY, ui_PopStyleColorEx, "void", "popStyleColor");
        QUERY->add_arg(QUERY, "int", "count");

        QUERY->add_sfun(QUERY, ui_PushStyleVar, "void", "pushStyleVar");
        QUERY->add_arg(QUERY, "int", "idx" /*ImGuiStyleVar*/);
        QUERY->add_arg(QUERY, "float", "val");
        QUERY->doc_func(QUERY,
                        "modify a style float variable. always use this if "
                        "you modify the style after NewFrame(). \n Parameter `idx` "
                        "is an enum of type UI_StyleVar");

        QUERY->add_sfun(QUERY, ui_PushStyleVarImVec2, "void", "pushStyleVar");
        QUERY->add_arg(QUERY, "int", "idx" /*ImGuiStyleVar*/);
        QUERY->add_arg(QUERY, "vec2", "val");
        QUERY->doc_func(QUERY,
                        "modify a style ImVec2 variable. always use this if "
                        "you modify the style after NewFrame(). \n Parameter `idx` "
                        "is an enum of type UI_StyleVar");

        QUERY->add_sfun(QUERY, ui_PopStyleVar, "void", "popStyleVar");
        QUERY->doc_func(QUERY, "implied count = 1");

        QUERY->add_sfun(QUERY, ui_PopStyleVarEx, "void", "popStyleVar");
        QUERY->add_arg(QUERY, "int", "count");

        QUERY->add_sfun(QUERY, ui_PushTabStop, "void", "pushTabStop");
        QUERY->add_arg(QUERY, "int", "tab_stop");
        QUERY->doc_func(QUERY,
                        "allow focusing using TAB/Shift-TAB, enabled by default "
                        "but you can disable it for certain widgets");

        QUERY->add_sfun(QUERY, ui_PopTabStop, "void", "popTabStop");

        QUERY->add_sfun(QUERY, ui_PushButtonRepeat, "void", "pushButtonRepeat");
        QUERY->add_arg(QUERY, "int", "repeat");
        QUERY->doc_func(QUERY,
                        "in 'repeat' mode, Button*() functions return repeated "
                        "true in a typematic manner (using "
                        "io.KeyRepeatDelay/io.KeyRepeatRate setting). Note that "
                        "you can call IsItemActive() after any Button() to tell if "
                        "the button is held in the current frame.");

        QUERY->add_sfun(QUERY, ui_PopButtonRepeat, "void", "popButtonRepeat");

        // Parameters stacks (current window) -----------------------------------
        SFUN(ui_PushItemWidth, "void", "pushItemWidth");
        ARG("float", "item_width");
        DOC_FUNC(
          "push width of items for common large \"item+label\" widgets. "
          ">0.0f: width in pixels, <0.0f align xx pixels to the right of "
          "window (so -FLT_MIN always align width to the right side).");

        SFUN(ui_PopItemWidth, "void", "popItemWidth");
        DOC_FUNC("pop width of items for common large \"item+label\" widgets.");

        SFUN(ui_SetNextItemWidth, "void", "setNextItemWidth");
        ARG("float", "item_width");
        DOC_FUNC(
          "set width of the _next_ common large \"item+label\" widget. "
          ">0.0f: width in pixels, <0.0f align xx pixels to the right of "
          "window (so -FLT_MIN always align width to the right side)");

        SFUN(ui_CalcItemWidth, "float", "calcItemWidth");
        DOC_FUNC(
          "width of item given pushed settings and current cursor position. "
          "NOT necessarily the width of last item unlike most 'Item' "
          "functions.");

        SFUN(ui_PushTextWrapPos, "void", "pushTextWrapPos");
        ARG("float", "wrap_local_pos_x");
        DOC_FUNC(
          "push word-wrapping position for Text*() commands. < 0.0f: no "
          "wrapping; 0.0f: wrap to end of window (or column); > 0.0f: wrap "
          "at 'wrap_pos_x' position in window local space");

        SFUN(ui_PopTextWrapPos, "void", "popTextWrapPos");
        DOC_FUNC("pop word-wrapping position for Text*() commands.");

        // Style read access ----------------------------------------------------
        QUERY->add_sfun(QUERY, ui_GetFontSize, "float", "getFontSize");
        QUERY->doc_func(QUERY,
                        "get current font size (= height in pixels) of "
                        "current font with current scale applied");

        QUERY->add_sfun(QUERY, ui_GetFontTexUvWhitePixel, "vec2",
                        "getFontTexUvWhitePixel");
        QUERY->doc_func(QUERY,
                        "get UV coordinate for a white pixel, useful to "
                        "draw custom shapes via the ImDrawList API");

        QUERY->add_sfun(QUERY, ui_GetColorU32, "int", "getColorU32");
        QUERY->add_arg(QUERY, "int", "idx" /*ImGuiCol*/);
        QUERY->doc_func(QUERY,
                        "implied alpha_mul = 1.0f. retrieve given style color with "
                        "style alpha applied and optional extra alpha multiplier, "
                        "packed as a 32-bit value suitable for ImDrawList\n"
                        "Parameter `idx` is an enum of type UI_Color");

        QUERY->add_sfun(QUERY, ui_GetColorU32Ex, "int", "getColorU32");
        QUERY->add_arg(QUERY, "int", "idx" /*ImGuiCol*/);
        QUERY->add_arg(QUERY, "float", "alpha_mul");
        QUERY->doc_func(QUERY,
                        "retrieve given style color with style alpha applied and "
                        "optional extra alpha multiplier, packed as a 32-bit value "
                        "suitable for ImDrawList\n"
                        "Parameter `idx` is an enum of type UI_color");

        QUERY->add_sfun(QUERY, ui_GetColorU32ImVec4, "int", "getColorU32");
        QUERY->add_arg(QUERY, "vec4", "col");
        QUERY->doc_func(QUERY,
                        "retrieve given color with style alpha applied, packed as a "
                        "32-bit value suitable for ImDrawList");

        QUERY->add_sfun(QUERY, ui_GetStyleColorVec4, "vec4", "getStyleColorVec4");
        QUERY->add_arg(QUERY, "int", "idx" /*ImGuiCol*/);
        QUERY->doc_func(QUERY,
                        "retrieve style color as stored in ImGuiStyle structure. "
                        "use to feed back into PushStyleColor(), otherwise use "
                        "GetColorU32() to get style color with style alpha baked "
                        "in.\n"
                        "Parameter `idx` is an enum of type UI_Color");

        // Layout cursor positioning
        QUERY->add_sfun(QUERY, ui_GetCursorScreenPos, "vec2", "getCursorScreenPos");
        QUERY->doc_func(QUERY,
                        "cursor position in absolute coordinates (prefer using "
                        "this, also more useful to work with ImDrawList API)");

        QUERY->add_sfun(QUERY, ui_SetCursorScreenPos, "vec2", "setCursorScreenPos");
        QUERY->add_arg(QUERY, "vec2", "pos");
        QUERY->doc_func(QUERY, "cursor position in absolute coordinates");

        QUERY->add_sfun(QUERY, ui_GetCursorPos, "vec2", "getCursorPos");
        QUERY->doc_func(QUERY,
                        "cursor position in window coordinates (relative to window "
                        "position)");

        QUERY->add_sfun(QUERY, ui_GetCursorPosX, "float", "getCursorPosX");

        QUERY->add_sfun(QUERY, ui_GetCursorPosY, "float", "getCursorPosY");

        QUERY->add_sfun(QUERY, ui_SetCursorPos, "void", "setCursorPos");
        QUERY->add_arg(QUERY, "vec2", "local_pos");

        QUERY->add_sfun(QUERY, ui_SetCursorPosX, "void", "setCursorPosX");
        QUERY->add_arg(QUERY, "float", "local_x");

        QUERY->add_sfun(QUERY, ui_SetCursorPosY, "void", "setCursorPosY");
        QUERY->add_arg(QUERY, "float", "local_y");

        QUERY->add_sfun(QUERY, ui_GetCursorStartPos, "vec2", "getCursorStartPos");
        QUERY->doc_func(QUERY, "initial cursor position, in window coordinates");

        // Other layout functions ------------------------------------------------

        QUERY->add_sfun(QUERY, ui_Separator, "void", "separator");
        QUERY->doc_func(QUERY,
                        "separator, generally horizontal. inside a menu "
                        "bar or in horizontal layout mode, this becomes a "
                        "vertical separator.");

        QUERY->add_sfun(QUERY, ui_SameLine, "void", "sameLine");
        QUERY->doc_func(QUERY, "implied offset_from_start_x = 0.0f, spacing = -1.0f");

        QUERY->add_sfun(QUERY, ui_SameLineEx, "void", "sameLine");
        QUERY->add_arg(QUERY, "float", "offset_from_start_x");
        QUERY->add_arg(QUERY, "float", "spacing");
        QUERY->doc_func(
          QUERY,
          "call between widgets or groups to layout them "
          "horizontally. X position given in window coordinates. "
          "offset_from_start_x == 0 : follow right after previous item. "
          "offset_from_start_x != 0 : align to specified x position (relative to "
          "window/group left). "
          "spacing < 0            : use default spacing if offset_from_start_x == 0, "
          "no spacing if offset_from_start_x != 0. "
          "spacing >= 0           : enforce spacing amount. ");

        QUERY->add_sfun(QUERY, ui_NewLine, "void", "newLine");
        QUERY->doc_func(QUERY,
                        "undo a SameLine() or force a new line when in a "
                        "horizontal-layout context.");

        QUERY->add_sfun(QUERY, ui_Spacing, "void", "spacing");
        QUERY->doc_func(QUERY, "add vertical spacing.");

        QUERY->add_sfun(QUERY, ui_Dummy, "void", "dummy");
        QUERY->add_arg(QUERY, "vec2", "size");
        QUERY->doc_func(QUERY,
                        "add a dummy item of given size. unlike InvisibleButton(), "
                        "Dummy() won't take the mouse click or be navigable into.");

        QUERY->add_sfun(QUERY, ui_Indent, "void", "indent");
        QUERY->doc_func(QUERY, "implied indent_w = 0.0f");

        QUERY->add_sfun(QUERY, ui_IndentEx, "void", "indent");
        QUERY->add_arg(QUERY, "float", "indent_w");
        QUERY->doc_func(QUERY,
                        "move content position toward the right, by indent_w, or "
                        "style.IndentSpacing if indent_w <= 0");

        QUERY->add_sfun(QUERY, ui_Unindent, "void", "unindent");
        QUERY->doc_func(QUERY, "implied indent_w = 0.0f");

        QUERY->add_sfun(QUERY, ui_UnindentEx, "void", "unindent");
        QUERY->add_arg(QUERY, "float", "indent_w");
        QUERY->doc_func(QUERY,
                        "move content position back to the left, by indent_w, or "
                        "style.IndentSpacing if indent_w <= 0");

        QUERY->add_sfun(QUERY, ui_BeginGroup, "void", "beginGroup");
        QUERY->doc_func(QUERY, "lock horizontal starting position");

        QUERY->add_sfun(QUERY, ui_EndGroup, "void", "endGroup");
        QUERY->doc_func(QUERY,
                        "unlock horizontal starting position + capture the whole "
                        "group bounding box into one \"item\" (so you can use "
                        "IsItemHovered() or layout primitives such as SameLine() on "
                        "whole group, etc.)");

        QUERY->add_sfun(QUERY, ui_AlignTextToFramePadding, "void",
                        "alignTextToFramePadding");
        QUERY->doc_func(QUERY,
                        "vertically align upcoming text baseline to FramePadding.y "
                        "so that it will align properly to regularly framed items "
                        "(call if you have text on a line before a framed item)");

        QUERY->add_sfun(QUERY, ui_GetTextLineHeight, "float", "getTextLineHeight");
        QUERY->doc_func(QUERY, "~ FontSize");

        QUERY->add_sfun(QUERY, ui_GetTextLineHeightWithSpacing, "float",
                        "getTextLineHeightWithSpacing");
        QUERY->doc_func(QUERY,
                        "~ FontSize + style.ItemSpacing.y (distance in pixels "
                        "between 2 consecutive lines of text)");

        QUERY->add_sfun(QUERY, ui_GetFrameHeight, "float", "getFrameHeight");
        QUERY->doc_func(QUERY, "~ FontSize + style.FramePadding.y * 2");

        QUERY->add_sfun(QUERY, ui_GetFrameHeightWithSpacing, "float",
                        "getFrameHeightWithSpacing");
        QUERY->doc_func(QUERY,
                        "~ FontSize + style.FramePadding.y * 2 + "
                        "style.ItemSpacing.y (distance in pixels between 2 "
                        "consecutive lines of framed widgets)");

        // ID stack/scopes -------------------------------------------------------

        QUERY->add_sfun(QUERY, ui_PushID, "void", "pushID");
        QUERY->add_arg(QUERY, "string", "str_id");
        QUERY->doc_func(QUERY,
                        "push string into the ID stack (will hash string)."
                        "Read the FAQ (docs/FAQ.md or http://dearimgui.com/faq) "
                        "for more details about how ID are handled in dear imgui.");

        QUERY->add_sfun(QUERY, ui_PushIDStr, "void", "pushID");
        QUERY->add_arg(QUERY, "string", "str_id_begin");
        QUERY->add_arg(QUERY, "string", "str_id_end");
        QUERY->doc_func(QUERY, "push string into the ID stack (will hash string).");

        QUERY->add_sfun(QUERY, ui_PushIDInt, "void", "pushID");
        QUERY->add_arg(QUERY, "int", "int_id");
        QUERY->doc_func(QUERY, "push integer into the ID stack (will hash integer).");

        QUERY->add_sfun(QUERY, ui_PopID, "void", "popID");
        QUERY->doc_func(QUERY, "pop from the ID stack.");

        QUERY->add_sfun(QUERY, ui_GetID, "int", "getID");
        QUERY->add_arg(QUERY, "string", "str_id");
        QUERY->doc_func(QUERY,
                        "calculate unique ID (hash of whole ID stack + given "
                        "parameter). e.g. if you want to query into ImGuiStorage "
                        "yourself");

        QUERY->add_sfun(QUERY, ui_GetIDStr, "int", "getID");
        QUERY->add_arg(QUERY, "string", "str_id_begin");
        QUERY->add_arg(QUERY, "string", "str_id_end");

        // Widgets: Text ---------------------------------------------------------

        QUERY->add_sfun(QUERY, ui_TextUnformatted, "void", "text");
        QUERY->add_arg(QUERY, "string", "text");
        QUERY->doc_func(QUERY, "implied text_end = NULL");

        QUERY->add_sfun(QUERY, ui_TextUnformattedEx, "void", "text");
        QUERY->add_arg(QUERY, "string", "text");
        QUERY->add_arg(QUERY, "string", "text_end");
        QUERY->doc_func(QUERY,
                        "raw text without formatting. Roughly equivalent to "
                        "Text(\"%s\", text) but: A) doesn't require null "
                        "terminated string if 'text_end' is specified, B) it's "
                        "faster, no memory copy is done, no buffer size limits, "
                        "recommended for long chunks of text.");

        QUERY->add_sfun(QUERY, ui_TextColoredUnformatted, "void", "textColored");
        QUERY->add_arg(QUERY, "vec4", "col");
        QUERY->add_arg(QUERY, "string", "text");
        QUERY->doc_func(QUERY,
                        "shortcut for PushStyleColor(ImGuiCol_Text, col); "
                        "Text(fmt, ...); PopStyleColor();");

        QUERY->add_sfun(QUERY, ui_TextDisabledUnformatted, "void", "textDisabled");
        QUERY->add_arg(QUERY, "string", "text");
        QUERY->doc_func(QUERY,
                        "shortcut for PushStyleColor(ImGuiCol_Text, "
                        "style.Colors[ImGuiCol_TextDisabled]); Text(fmt, ...); "
                        "PopStyleColor();");

        QUERY->add_sfun(QUERY, ui_TextWrappedUnformatted, "void", "textWrapped");
        QUERY->add_arg(QUERY, "string", "text");
        QUERY->doc_func(QUERY,
                        "shortcut for PushTextWrapPos(0.0f); Text(fmt, ...); "
                        "PopTextWrapPos();. Note that this won't work on an "
                        "auto-resizing window if there's no other widgets to extend "
                        "the window width, you may need to set a size using "
                        "SetNextWindowSize().");

        QUERY->add_sfun(QUERY, ui_LabelTextUnformatted, "void", "labelText");
        QUERY->add_arg(QUERY, "string", "label");
        QUERY->add_arg(QUERY, "string", "text");
        QUERY->doc_func(QUERY,
                        "display text+label aligned the same way as value+label "
                        "widgets");

        QUERY->add_sfun(QUERY, ui_BulletTextUnformatted, "void", "bulletText");
        QUERY->add_arg(QUERY, "string", "text");
        QUERY->doc_func(QUERY, "shortcut for Bullet()+Text()");

        QUERY->add_sfun(QUERY, ui_SeparatorText, "void", "separatorText");
        QUERY->add_arg(QUERY, "string", "label");
        QUERY->doc_func(QUERY, "currently: formatted text with an horizontal line");

        // Widgets: Main ---------------------------------------------------------

        QUERY->add_sfun(QUERY, ui_Button, "int", "button");
        QUERY->add_arg(QUERY, "string", "label");
        QUERY->doc_func(QUERY, "implied size = ImVec2(0, 0)");

        QUERY->add_sfun(QUERY, ui_ButtonEx, "int", "button");
        QUERY->add_arg(QUERY, "string", "label");
        QUERY->add_arg(QUERY, "vec2", "size");

        QUERY->add_sfun(QUERY, ui_SmallButton, "int", "smallButton");
        QUERY->add_arg(QUERY, "string", "label");
        QUERY->doc_func(QUERY, "button with (FramePadding.y == 0)");

        QUERY->add_sfun(QUERY, ui_InvisibleButton, "int", "invisibleButton");
        QUERY->add_arg(QUERY, "string", "str_id");
        QUERY->add_arg(QUERY, "vec2", "size");
        QUERY->add_arg(QUERY, "int", "flags");
        QUERY->doc_func(QUERY,
                        "flexible button behavior without the visuals, frequently "
                        "useful to build custom behaviors using the public api "
                        "(along with IsItemActive, IsItemHovered, etc.)"
                        "param `flags` is an enum of type UI_ButtonFlags");

        QUERY->add_sfun(QUERY, ui_ArrowButton, "int", "arrowButton");
        QUERY->add_arg(QUERY, "string", "str_id");
        QUERY->add_arg(QUERY, "int", "direction");
        QUERY->doc_func(QUERY,
                        "square button with an arrow shape. param `direction` is "
                        "an enum of type UI_Direction");

        QUERY->add_sfun(QUERY, ui_Checkbox, "int", "checkbox");
        QUERY->add_arg(QUERY, "string", "label");
        QUERY->add_arg(QUERY, "UI_Bool", "v");

        QUERY->add_sfun(QUERY, ui_CheckboxFlagsIntPtr, "int", "checkboxFlags");
        QUERY->add_arg(QUERY, "string", "label");
        QUERY->add_arg(QUERY, "UI_Int", "flags");
        QUERY->add_arg(QUERY, "int", "flags_value");

        QUERY->add_sfun(QUERY, ui_RadioButton, "int", "radioButton");
        QUERY->add_arg(QUERY, "string", "label");
        QUERY->add_arg(QUERY, "int", "active");
        QUERY->doc_func(QUERY,
                        "use with e.g. if (RadioButton(\"one\", my_value==1)) { "
                        "my_value = 1; }");

        QUERY->add_sfun(QUERY, ui_RadioButtonIntPtr, "int", "radioButton");
        QUERY->add_arg(QUERY, "string", "label");
        QUERY->add_arg(QUERY, "UI_Int", "v");
        QUERY->add_arg(QUERY, "int", "v_button");
        QUERY->doc_func(QUERY,
                        "shortcut to handle the above pattern when value is an "
                        "integer");

        QUERY->add_sfun(QUERY, ui_ProgressBar, "void", "progressBar");
        QUERY->add_arg(QUERY, "float", "fraction");
        QUERY->add_arg(QUERY, "vec2", "size_arg");
        QUERY->add_arg(QUERY, "string", "overlay");

        QUERY->add_sfun(QUERY, ui_Bullet, "void", "bullet");
        QUERY->doc_func(QUERY,
                        "draw a small circle + keep the cursor on the same line. "
                        "advance cursor x position by GetTreeNodeToLabelSpacing(), "
                        "same distance that TreeNode() uses");

        // Widgets: Image --------------------------------------------------------
        SFUN(ui_Image, "void", "image");
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "texture");
        ARG("vec2", "size");
        DOC_FUNC("Render an image with the given dimensions");

        SFUN(ui_ImageEx, "void", "image");
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "texture");
        ARG("vec2", "size");
        ARG("vec2", "uv0");
        ARG("vec2", "uv1");
        ARG("vec4", "tint_color");
        ARG("vec4", "border_color");
        DOC_FUNC(
          "Render an image with the given dimensions, uv coordinates, and color "
          "options. uv0 and uv1 are normalized texture coordinates where (0,0) refers "
          "to the upper left and (1,1) the bottom right of the texture. E.g. setting "
          "uv0 = (.5, .5) and uv=(1,1) will render only the bottom-right quadrant. "
          "By default, tint_color = @(1,1,1,1) and border_color = @(0,0,0,0)"
          "To learn more, visit "
          "https://github.com/ocornut/imgui/wiki/"
          "Image-Loading-and-Displaying-Examples");

        SFUN(ui_ImageButton, "int", "imageButton");
        ARG("string", "label");
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "texture");
        ARG("vec2", "size");
        DOC_FUNC("Render a clickable image button with the given dimensions");

        SFUN(ui_ImageButtonEx, "int", "imageButton");
        ARG("string", "label");
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "texture");
        ARG("vec2", "size");
        ARG("vec2", "uv0");
        ARG("vec2", "uv1");
        ARG("vec4", "tint_color");
        ARG("vec4", "background_color");
        DOC_FUNC(
          "Render a clickable image button with the given dimensions, uv coordinates, "
          "and color "
          "options. uv0 and uv1 are normalized texture coordinates where (0,0) refers "
          "to the upper left and (1,1) the bottom right of the texture. E.g. setting "
          "uv0 = (.5, .5) and uv=(1,1) will render only the bottom-right quadrant. "
          "By default, tint_color = @(1,1,1,1) and background_color = @(0,0,0,0)"
          "To learn more, visit "
          "https://github.com/ocornut/imgui/wiki/"
          "Image-Loading-and-Displaying-Examples");

        // Widgets: Combo --------------------------------------------------------

        QUERY->add_sfun(QUERY, ui_BeginCombo, "int", "beginCombo");
        QUERY->add_arg(QUERY, "string", "label");
        QUERY->add_arg(QUERY, "string", "preview_value");
        QUERY->add_arg(QUERY, "int", "flags");
        QUERY->doc_func(
          QUERY,
          "The BeginCombo()/EndCombo() api allows you to manage your contents and "
          "selection state however you want it, by creating e.g. Selectable() "
          "items.\n"
          "// - The old Combo() api are helpers over BeginCombo()/EndCombo() which "
          "are kept available for convenience purpose. This is analogous to how "
          "ListBox are created.\n"
          "`flags` param is an enum of type UI_ComboFlags");

        QUERY->add_sfun(QUERY, ui_EndCombo, "void", "endCombo");
        QUERY->doc_func(QUERY, "only call EndCombo() if BeginCombo() returns true!");

        QUERY->add_sfun(QUERY, ui_ComboChar, "int", "combo");
        QUERY->add_arg(QUERY, "string", "label");
        QUERY->add_arg(QUERY, "UI_Int", "current_item");
        QUERY->add_arg(QUERY, "string[]", "items");
        QUERY->doc_func(QUERY, "implied popup_max_height_in_items = -1");

        QUERY->add_sfun(QUERY, ui_ComboCharEx, "int", "combo");
        QUERY->add_arg(QUERY, "string", "label");
        QUERY->add_arg(QUERY, "UI_Int", "current_item");
        QUERY->add_arg(QUERY, "string[]", "items");
        QUERY->add_arg(QUERY, "int", "popup_max_height_in_items");

        // TODO: ui_Combo doesn't work with chuck strings
        // cannot seem to escape \0 (gets copied literally) by
        // API->object->str(Chuck_String*)

        // QUERY->add_sfun(QUERY, ui_Combo, "int", "combo");
        // QUERY->add_arg(QUERY, "string", "label");
        // QUERY->add_arg(QUERY, "UI_Int", "current_item");
        // QUERY->add_arg(QUERY, "string", "items_separated_by_zeros");
        // QUERY->doc_func(QUERY, "implied popup_max_height_in_items = -1");

        // SFUN(ui_ComboEx, "int", "combo");
        // ARG("string", "label");
        // ARG("UI_Int", "current_item");
        // ARG("string", "items_separated_by_zeros");
        // ARG("int", "popup_max_height_in_items");
        // DOC_FUNC(
        //   "Separate items with \\0 within a string, end "
        //   "item-list with \\0\\0. e.g. \"One\\0Two\\0Three\\0\\0\"");

        // Widgets: Drag ---------------------------------------------------------

        SFUN(ui_DragFloat, "int", "drag");
        ARG("string", "label");
        ARG("UI_Float", "v");
        DOC_FUNC(
          "Implied v_speed = 1.0f, v_min = 0.0f, v_max = 0.0f, format = "
          "\"%.3f\", flags = 0");

        SFUN(ui_DragFloatEx, "int", "drag");
        ARG("string", "label");
        ARG("UI_Float", "v");
        ARG("float", "v_speed");
        ARG("float", "v_min");
        ARG("float", "v_max");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC(
          "If v_min >= v_max we have no bound. Parameter `flags` is an enum of "
          "type UI_SliderFlags");

        SFUN(ui_DragFloat2, "int", "drag");
        ARG("string", "label");
        ARG("UI_Float2", "v");
        DOC_FUNC(
          "Implied v_speed = 1.0f, v_min = 0.0f, v_max = 0.0f, format = "
          "\"%.3f\", flags = 0");

        SFUN(ui_DragFloat2Ex, "int", "drag");
        ARG("string", "label");
        ARG("UI_Float2", "v");
        ARG("float", "v_speed");
        ARG("float", "v_min");
        ARG("float", "v_max");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC(
          "If v_min >= v_max we have no bound. Parameter `flags` is an enum of "
          "type UI_SliderFlags");

        SFUN(ui_DragFloat2Speed, "int", "drag");
        ARG("string", "label");
        ARG("UI_Float2", "v");
        ARG("float", "v_speed");
        DOC_FUNC("Implied v_min = 0.0f, v_max = 0.0f, format = \"%.3f\", flags = 0");

        SFUN(ui_DragFloat3, "int", "drag");
        ARG("string", "label");
        ARG("UI_Float3", "v");
        DOC_FUNC(
          "Implied v_speed = 1.0f, v_min = 0.0f, v_max = 0.0f, format = "
          "\"%.3f\", flags = 0");

        SFUN(ui_DragFloat3Ex, "int", "drag");
        ARG("string", "label");
        ARG("UI_Float3", "v");
        ARG("float", "v_speed");
        ARG("float", "v_min");
        ARG("float", "v_max");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC(
          "If v_min >= v_max we have no bound. Parameter `flags` is an enum of "
          "type UI_SliderFlags");

        SFUN(ui_DragFloat3Speed, "int", "drag");
        ARG("string", "label");
        ARG("UI_Float3", "v");
        ARG("float", "v_speed");
        DOC_FUNC("Implied v_min = 0.0f, v_max = 0.0f, format = \"%.3f\", flags = 0");

        SFUN(ui_DragFloat4, "int", "drag");
        ARG("string", "label");
        ARG("UI_Float4", "v");
        DOC_FUNC(
          "Implied v_speed = 1.0f, v_min = 0.0f, v_max = 0.0f, format = "
          "\"%.3f\", flags = 0");

        SFUN(ui_DragFloat4Speed, "int", "drag");
        ARG("string", "label");
        ARG("UI_Float4", "v");
        ARG("float", "speed");
        DOC_FUNC(
          "Implied v_min = 0.0f, v_max = 0.0f, format = "
          "\"%.3f\", flags = 0");

        SFUN(ui_DragFloat4Ex, "int", "drag");
        ARG("string", "label");
        ARG("UI_Float4", "v");
        ARG("float", "v_speed");
        ARG("float", "v_min");
        ARG("float", "v_max");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC(
          "If v_min >= v_max we have no bound. Parameter `flags` is an enum of "
          "type UI_SliderFlags");

        SFUN(ui_DragFloatRange2, "int", "drag");
        ARG("string", "label");
        ARG("UI_Float", "v_current_min");
        ARG("UI_Float", "v_current_max");
        DOC_FUNC(
          "Implied v_speed = 1.0f, v_min = 0.0f, v_max = 0.0f, format = "
          "\"%.3f\", format_max = NULL, flags = 0");

        SFUN(ui_DragFloatRange2Ex, "int", "drag");
        ARG("string", "label");
        ARG("UI_Float", "v_current_min");
        ARG("UI_Float", "v_current_max");
        ARG("float", "v_speed");
        ARG("float", "v_min");
        ARG("float", "v_max");
        ARG("string", "format");
        ARG("string", "format_max");
        ARG("int", "flags");
        DOC_FUNC("Parameter `flags` is an enum of type UI_SliderFlags");

        SFUN(ui_DragInt, "int", "drag");
        ARG("string", "label");
        ARG("UI_Int", "v");
        DOC_FUNC(
          "Implied v_speed = 1.0f, v_min = 0, v_max = 0, format = \"%d\", flags = "
          "0");

        SFUN(ui_DragIntEx, "int", "drag");
        ARG("string", "label");
        ARG("UI_Int", "v");
        ARG("float", "v_speed");
        ARG("int", "v_min");
        ARG("int", "v_max");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC(
          "If v_min >= v_max we have no bound. Parameter `flags` is an enum of "
          "type UI_SliderFlags");

        SFUN(ui_DragInt2, "int", "drag");
        ARG("string", "label");
        ARG("UI_Int2", "v");
        DOC_FUNC(
          "Implied v_speed = 1.0f, v_min = 0, v_max = 0, format = \"%d\", flags = "
          "0");

        SFUN(ui_DragInt2Ex, "int", "drag");
        ARG("string", "label");
        ARG("UI_Int2", "v");
        ARG("float", "v_speed");
        ARG("int", "v_min");
        ARG("int", "v_max");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC(
          "If v_min >= v_max we have no bound. Parameter `flags` is an enum of "
          "type UI_SliderFlags");

        SFUN(ui_DragInt3, "int", "drag");
        ARG("string", "label");
        ARG("UI_Int3", "v");
        DOC_FUNC(
          "Implied v_speed = 1.0f, v_min = 0, v_max = 0, format = \"%d\", flags = "
          "0");

        SFUN(ui_DragInt3Ex, "int", "drag");
        ARG("string", "label");
        ARG("UI_Int3", "v");
        ARG("float", "v_speed");
        ARG("int", "v_min");
        ARG("int", "v_max");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC(
          "If v_min >= v_max we have no bound. Parameter `flags` is an enum of "
          "type UI_SliderFlags");

        SFUN(ui_DragInt4, "int", "drag");
        ARG("string", "label");
        ARG("UI_Int4", "v");
        DOC_FUNC(
          "Implied v_speed = 1.0f, v_min = 0, v_max = 0, format = \"%d\", flags = "
          "0");

        SFUN(ui_DragInt4Ex, "int", "drag");
        ARG("string", "label");
        ARG("UI_Int4", "v");
        ARG("float", "v_speed");
        ARG("int", "v_min");
        ARG("int", "v_max");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC(
          "If v_min >= v_max we have no bound. Parameter `flags` is an enum of "
          "type UI_SliderFlags");

        SFUN(ui_DragIntRange2, "int", "drag");
        ARG("string", "label");
        ARG("UI_Int", "v_current_min");
        ARG("UI_Int", "v_current_max");
        DOC_FUNC(
          "Implied v_speed = 1.0f, v_min = 0, v_max = 0, format = \"%d\", "
          "format_max = NULL, flags = 0");

        SFUN(ui_DragIntRange2Ex, "int", "drag");
        ARG("string", "label");
        ARG("UI_Int", "v_current_min");
        ARG("UI_Int", "v_current_max");
        ARG("float", "v_speed");
        ARG("int", "v_min");
        ARG("int", "v_max");
        ARG("string", "format");
        ARG("string", "format_max");
        ARG("int", "flags");
        DOC_FUNC("Parameter `flags` is an enum of type UI_SliderFlags");

        SFUN(ui_DragScalarN_CKINT, "int", "drag");
        ARG("string", "label");
        ARG("int[]", "data");

        SFUN(ui_DragScalarNEx_CKINT, "int", "drag");
        ARG("string", "label");
        ARG("int[]", "data");
        ARG("float", "v_speed");
        ARG("int", "v_min");
        ARG("int", "v_max");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC("Parameter `flags` is an enum of type UI_SliderFlags");

        SFUN(ui_DragScalarN_CKFLOAT, "int", "drag");
        ARG("string", "label");
        ARG("float[]", "data");

        SFUN(ui_DragScalarNEx_CKFLOAT, "int", "drag");
        ARG("string", "label");
        ARG("float[]", "data");
        ARG("float", "v_speed");
        ARG("float", "v_min");
        ARG("float", "v_max");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC("Parameter `flags` is an enum of type UI_SliderFlags");

        // Widgets: Slider -------------------------------------------------------

        SFUN(ui_SliderFloat, "int", "slider");
        ARG("string", "label");
        ARG("UI_Float", "v");
        ARG("float", "v_min");
        ARG("float", "v_max");
        DOC_FUNC("Implied format = \"%.3f\", flags = 0");

        SFUN(ui_SliderFloatEx, "int", "slider");
        ARG("string", "label");
        ARG("UI_Float", "v");
        ARG("float", "v_min");
        ARG("float", "v_max");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC(
          "adjust format to decorate the value with a prefix or a suffix for in-"
          "slider labels or unit display. `flags` is an enum of type "
          "UI_SliderFlags");

        SFUN(ui_SliderAngle, "int", "sliderAngle");
        ARG("string", "label");
        ARG("UI_Float", "v_rad");
        DOC_FUNC(
          "Implied v_degrees_min = -360.0f, v_degrees_max = +360.0f, format = "
          "\"%.0f deg\", flags = 0");

        SFUN(ui_SliderAngleBounds, "int", "sliderAngle");
        ARG("string", "label");
        ARG("UI_Float", "v_rad");
        ARG("float", "v_degrees_min");
        ARG("float", "v_degrees_max");

        SFUN(ui_SliderAngleEx, "int", "sliderAngle");
        ARG("string", "label");
        ARG("UI_Float", "v_rad");
        ARG("float", "v_degrees_min");
        ARG("float", "v_degrees_max");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC("Parameter `flags` is an enum of type UI_SliderFlags");

        SFUN(ui_SliderInt, "int", "slider");
        ARG("string", "label");
        ARG("UI_Int", "v");
        ARG("int", "v_min");
        ARG("int", "v_max");
        DOC_FUNC("Implied format = \"%d\", flags = 0");

        SFUN(ui_SliderIntEx, "int", "slider");
        ARG("string", "label");
        ARG("UI_Int", "v");
        ARG("int", "v_min");
        ARG("int", "v_max");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC("Parameter `flags` is an enum of type UI_SliderFlags");

        SFUN(ui_SliderScalarN_CKINT, "int", "slider");
        ARG("string", "label");
        ARG("int[]", "data");
        ARG("int", "v_min");
        ARG("int", "v_max");

        SFUN(ui_SliderScalarNEx_CKINT, "int", "slider");
        ARG("string", "label");
        ARG("int[]", "data");
        ARG("int", "v_min");
        ARG("int", "v_max");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC("Parameter `flags` is an enum of type UI_SliderFlags");

        SFUN(ui_SliderScalarN_CKFLOAT, "int", "slider");
        ARG("string", "label");
        ARG("float[]", "data");
        ARG("float", "v_min");
        ARG("float", "v_max");

        SFUN(ui_SliderScalarNEx_CKFLOAT, "int", "slider");
        ARG("string", "label");
        ARG("float[]", "data");
        ARG("float", "v_min");
        ARG("float", "v_max");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC("Parameter `flags` is an enum of type UI_SliderFlags");

        SFUN(ui_VSliderFloat, "int", "vslider");
        ARG("string", "label");
        ARG("vec2", "size");
        ARG("UI_Float", "v");
        ARG("float", "v_min");
        ARG("float", "v_max");
        DOC_FUNC("Implied format = \"%.3f\", flags = 0");

        SFUN(ui_VSliderFloatEx, "int", "vslider");
        ARG("string", "label");
        ARG("vec2", "size");
        ARG("UI_Float", "v");
        ARG("float", "v_min");
        ARG("float", "v_max");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC("Parameter `flags` is an enum of type UI_SliderFlags");

        SFUN(ui_VSliderInt, "int", "vslider");
        ARG("string", "label");
        ARG("vec2", "size");
        ARG("UI_Int", "v");
        ARG("int", "v_min");
        ARG("int", "v_max");
        DOC_FUNC("Implied format = \"%d\", flags = 0");

        SFUN(ui_VSliderIntEx, "int", "vslider");
        ARG("string", "label");
        ARG("vec2", "size");
        ARG("UI_Int", "v");
        ARG("int", "v_min");
        ARG("int", "v_max");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC("Parameter `flags` is an enum of type UI_SliderFlags");

        // Widgets: Input --------------------------------------------------------

        SFUN(ui_InputText, "int", "inputText");
        ARG("string", "label");
        ARG("UI_String", "buf");
        DOC_FUNC(
          "Implied max_input_length = 256, flags = 0, callback = NULL, user_data = "
          "NULL. `flags` is an enum of type UI_InputTextFlags");

        SFUN(ui_InputTextEx, "int", "inputText");
        ARG("string", "label");
        ARG("UI_String", "buf");
        ARG("int", "max_input_length");
        ARG("int", "flags");
        // ARG("UI_InputTextCallback", "callback");  // ignoring for now
        DOC_FUNC("Parameter `flags` is an enum of type UI_InputTextFlags");

        SFUN(ui_InputTextMultiline, "int", "inputTextMultiline");
        ARG("string", "label");
        ARG("UI_String", "buf");
        DOC_FUNC(
          "Implied size = ImVec2(0, 0), flags = 0, callback = NULL, "
          "user_data = NULL");

        SFUN(ui_InputTextMultilineEx, "int", "inputTextMultiline");
        ARG("string", "label");
        ARG("UI_String", "buf");
        ARG("int", "max_input_length");
        ARG("vec2", "size");
        ARG("int", "flags");
        // ARG("UI_InputTextCallback", "callback");  // ignoring for now
        DOC_FUNC("Parameter `flags` is an enum of type UI_InputTextFlags");

        SFUN(ui_InputTextWithHint, "int", "inputTextWithHint");
        ARG("string", "label");
        ARG("string", "hint");
        ARG("UI_String", "buf");
        DOC_FUNC(
          "Implied max_input_length = 256, flags = 0, callback = NULL, user_data = "
          "NULL.");

        SFUN(ui_InputTextWithHintEx, "int", "inputTextWithHint");
        ARG("string", "label");
        ARG("string", "hint");
        ARG("UI_String", "buf");
        ARG("int", "max_input_length");
        ARG("int", "flags");
        // ARG("UI_InputTextCallback", "callback");  // ignoring for now
        DOC_FUNC("Parameter `flags` is an enum of type UI_InputTextFlags");

        SFUN(ui_InputFloat, "int", "inputFloat");
        ARG("string", "label");
        ARG("UI_Float", "v");
        DOC_FUNC(
          "Implied step = 0.0f, step_fast = 0.0f, format = \"%.3f\", flags = "
          "0");

        SFUN(ui_InputFloatEx, "int", "inputFloat");
        ARG("string", "label");
        ARG("UI_Float", "v");
        ARG("float", "step");
        ARG("float", "step_fast");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC("Parameter `flags` is an enum of type UI_InputTextFlags");

        SFUN(ui_InputScalarN_CKFLOAT, "int", "inputFloat");
        ARG("string", "label");
        ARG("float[]", "data");
        DOC_FUNC("Implied p_step = NULL, p_step_fast = NULL, format = NULL, flags = 0");

        SFUN(ui_InputScalarNEx_CKFLOAT, "int", "inputFloat");
        ARG("string", "label");
        ARG("float[]", "data");
        ARG("float", "step");
        ARG("float", "step_fast");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC("Parameter `flags` is an enum of type UI_InputTextFlags");

        SFUN(ui_InputInt, "int", "inputInt");
        ARG("string", "label");
        ARG("UI_Int", "v");
        DOC_FUNC("Implied step = 1, step_fast = 100, flags = 0");

        SFUN(ui_InputIntEx, "int", "inputInt");
        ARG("string", "label");
        ARG("UI_Int", "v");
        ARG("int", "step");
        ARG("int", "step_fast");
        ARG("int", "flags");
        DOC_FUNC("Parameter `flags` is an enum of type UI_InputTextFlags");

        SFUN(ui_InputScalarN_CKINT, "int", "inputInt");
        ARG("string", "label");
        ARG("int[]", "data");
        DOC_FUNC("Implied p_step = NULL, p_step_fast = NULL, format = NULL, flags = 0");

        SFUN(ui_InputScalarNEx_CKINT, "int", "inputInt");
        ARG("string", "label");
        ARG("int[]", "data");
        ARG("int", "step");
        ARG("int", "step_fast");
        ARG("string", "format");
        ARG("int", "flags");
        DOC_FUNC("Parameter `flags` is an enum of type UI_InputTextFlags");

        // Widgets: Color --------------------------------------------------------

        SFUN(ui_ColorEdit3, "int", "colorEdit");
        ARG("string", "label");
        ARG("UI_Float3", "col");
        ARG("int", "flags");
        DOC_FUNC("`flags` is an enum of type UI_ColorEditFlags");

        SFUN(ui_ColorEdit3_NoFlags, "int", "colorEdit");
        ARG("string", "label");
        ARG("UI_Float3", "col");
        DOC_FUNC("Default rgb color widget");

        SFUN(ui_ColorEdit4, "int", "colorEdit");
        ARG("string", "label");
        ARG("UI_Float4", "col");
        ARG("int", "flags");
        DOC_FUNC("`flags` is an enum of type UI_ColorEditFlags");

        SFUN(ui_ColorEdit4_NoFlags, "int", "colorEdit");
        ARG("string", "label");
        ARG("UI_Float4", "col");
        DOC_FUNC("Default rgba color widget");

        SFUN(ui_ColorPicker3, "int", "colorPicker");
        ARG("string", "label");
        ARG("UI_Float3", "col");
        ARG("int", "flags");
        DOC_FUNC("`flags` is an enum of type UI_ColorEditFlags");

        SFUN(ui_ColorPicker4, "int", "colorPicker");
        ARG("string", "label");
        ARG("UI_Float4", "color");
        ARG("int", "flags");
        ARG("vec4", "reference_color");

        SFUN(ui_ColorPicker4_no_ref_col, "int", "colorPicker");
        ARG("string", "label");
        ARG("UI_Float4", "color");
        ARG("int", "flags");

        SFUN(ui_ColorButton, "int", "colorButton");
        ARG("string", "desc_id");
        ARG("vec4", "col");
        ARG("int", "flags");
        DOC_FUNC(
          "Implied size = ImVec2(0, 0). `flags` is an enum of type "
          "UI_ColorEditFlags");

        SFUN(ui_ColorButtonEx, "int", "colorButton");
        ARG("string", "desc_id");
        ARG("vec4", "col");
        ARG("int", "flags");
        ARG("vec2", "size");
        DOC_FUNC(
          "`flags` is an enum of type UI_ColorEditFlags. display a color "
          "square/button, hover for details, return true when pressed.");

        SFUN(ui_SetColorEditOptions, "void", "setColorEditOptions");
        ARG("int", "flags");
        DOC_FUNC(
          "initialize current options (generally on application startup) if you "
          "want to select a default format, picker type, etc. User will be able to "
          "change many settings, unless you pass the _NoOptions flag to your "
          "calls.");

        // Widgets: Trees --------------------------------------------------------

        SFUN(ui_TreeNode, "int", "treeNode");
        ARG("string", "label");
        DOC_FUNC(
          "return true when the node is open, in which case you need to call "
          "TreePop()");

        SFUN(ui_TreeNodeStrUnformatted, "int", "treeNode");
        ARG("string", "str_id");
        ARG("string", "text");

        SFUN(ui_TreeNodeEx, "int", "treeNode");
        ARG("string", "label");
        ARG("int", "flags");
        DOC_FUNC("`flags` is an enum of type UI_TreeNodeFlags");

        SFUN(ui_TreeNodeExStrUnformatted, "int", "treeNode");
        ARG("string", "str_id");
        ARG("int", "flags");
        ARG("string", "text");

        SFUN(ui_TreePush, "void", "treePush");
        ARG("string", "str_id");
        DOC_FUNC(
          "Indent()+PushID(). Already called by TreeNode() when returning "
          "true, but you can call TreePush/TreePop yourself if desired.");

        SFUN(ui_TreePop, "void", "treePop");
        DOC_FUNC("~ Unindent()+PopID()");

        SFUN(ui_GetTreeNodeToLabelSpacing, "float", "getTreeNodeToLabelSpacing");
        DOC_FUNC(
          "horizontal distance preceding label when using TreeNode*() or Bullet() "
          "== (g.FontSize + style.FramePadding.x*2) for a regular unframed "
          "TreeNode");

        SFUN(ui_CollapsingHeader, "int", "collapsingHeader");
        ARG("string", "label");
        ARG("int", "flags");
        DOC_FUNC(
          "`flags` is an enum of type UI_TreeNodeFlags. if returning 'true' "
          "the header is open. doesn't indent nor push on ID stack. user "
          "doesn't have to call TreePop().");

        SFUN(ui_CollapsingHeaderBoolPtr, "int", "collapsingHeader");
        ARG("string", "label");
        ARG("UI_Bool", "p_visible");
        ARG("int", "flags");
        DOC_FUNC(
          "when 'p_visible != NULL': if '*p_visible==true' display an additional "
          "small close button on upper right of the header which will set the bool "
          "to false when clicked, if '*p_visible==false' don't display the "
          "header.");

        SFUN(ui_SetNextItemOpen, "void", "setNextItemOpen");
        ARG("int", "is_open");
        ARG("int", "cond");
        DOC_FUNC(
          "set next TreeNode/CollapsingHeader open state. `Cond` is an enum of "
          "type UI_Cond");

        // Widgets: Selectable ---------------------------------------------------

        SFUN(ui_Selectable, "int", "selectable");
        ARG("string", "label");
        DOC_FUNC("Implied selected = false, flags = 0, size = ImVec2(0, 0)");

        SFUN(ui_SelectableEx, "int", "selectable");
        ARG("string", "label");
        ARG("int", "selected");
        ARG("int", "flags");
        ARG("vec2", "size");
        DOC_FUNC(
          "`selected` carry the selection state (read-only). Selectable() is "
          "clicked is returns true so you can modify your selection state. "
          "`size.x==0.0`: use remaining width, `size.x>0.0`: specify width. "
          "`size.y==0.0`: use label height, `size.y>0.0`: specify height");

        SFUN(ui_SelectableBoolPtr, "int", "selectable");
        ARG("string", "label");
        ARG("UI_Bool", "p_selected");
        ARG("int", "flags");
        DOC_FUNC(
          "`p_selected` points to the selection state (read-write), as a "
          "convenient "
          "helper. `flags` is an enum of type UI_SelectableFlags");

        SFUN(ui_SelectableBoolPtrEx, "int", "selectable");
        ARG("string", "label");
        ARG("UI_Bool", "p_selected");
        ARG("int", "flags");
        ARG("vec2", "size");
        DOC_FUNC(
          "`p_selected` points to the selection state (read-write), as a "
          "convenient helper. `flags` is an enum of type UI_SelectableFlags");

        // Widgets: List Boxes ---------------------------------------------------

        SFUN(ui_BeginListBox, "int", "beginListBox");
        ARG("string", "label");
        ARG("vec2", "size");
        DOC_FUNC(
          "This is essentially a thin wrapper to using BeginChild/EndChild with "
          "the ImGuiChildFlags_FrameStyle flag for stylistic changes + displaying "
          "a label.\n"
          "You can submit contents and manage your selection state however you "
          "want it, by creating e.g. Selectable() or any other items.\n"
          "The simplified/old ListBox() api are helpers over "
          "BeginListBox()/EndListBox() which are kept available for convenience "
          "purpose. This is analoguous to how Combos are created.\n"
          "Choose frame width:   size.x > 0.0f: custom  /  size.x < 0.0f or "
          "-FLT_MIN: right-align   /  size.x = 0.0f (default): use current "
          "ItemWidth\n"
          "Choose frame height:  size.y > 0.0f: custom  /  size.y < 0.0f or "
          "-FLT_MIN: bottom-align  /  size.y = 0.0f (default): arbitrary default "
          "height which can fit ~7 items"
          "This function opens a framed scrolling region.");

        SFUN(ui_EndListBox, "void", "endListBox");
        DOC_FUNC("only call EndListBox() if BeginListBox() returned true!");

        SFUN(ui_ListBox, "int", "listBox");
        ARG("string", "label");
        ARG("UI_Int", "current_item");
        ARG("string[]", "items");
        ARG("int", "height_in_items");
        DOC_FUNC("set `height_in_items` to -1 to use the default");

        SFUN(ui_ListBox_default, "int", "listBox");
        ARG("string", "label");
        ARG("UI_Int", "current_item");
        ARG("string[]", "items");
        DOC_FUNC("listbox ui widget, uses default item height");

        // Widgets: Data Plotting ------------------------------------------------

        SFUN(ui_PlotLines, "void", "plotLines");
        ARG("string", "label");
        ARG("float[]", "values");
        DOC_FUNC(
          "Implied values_offset = 0, overlay_text = NULL, scale_min = FLT_MAX, "
          "scale_max = FLT_MAX, graph_size = ImVec2(0, 0), stride = sizeof(float)");

        SFUN(ui_PlotLinesEx, "void", "plotLines");
        ARG("string", "label");
        ARG("float[]", "values");
        ARG("int", "values_offset");
        ARG("string", "overlay_text");
        ARG("float", "scale_min");
        ARG("float", "scale_max");
        ARG("vec2", "graph_size");

        SFUN(ui_PlotHistogram, "void", "plotHistogram");
        ARG("string", "label");
        ARG("float[]", "values");
        DOC_FUNC(
          "Implied values_offset = 0, overlay_text = NULL, scale_min = FLT_MAX, "
          "scale_max = FLT_MAX, graph_size = ImVec2(0, 0), stride = sizeof(float)");

        SFUN(ui_PlotHistogramEx, "void", "plotHistogram");
        ARG("string", "label");
        ARG("float[]", "values");
        ARG("int", "values_offset");
        ARG("string", "overlay_text");
        ARG("float", "scale_min");
        ARG("float", "scale_max");
        ARG("vec2", "graph_size");

        // Widgets: Menus --------------------------------------------------------

        SFUN(ui_BeginMenuBar, "int", "beginMenuBar");
        DOC_FUNC(
          "append to menu-bar of current window (requires ImGuiWindowFlags_MenuBar "
          "flag set on parent window)");

        SFUN(ui_EndMenuBar, "void", "endMenuBar");
        DOC_FUNC("only call EndMenuBar() if BeginMenuBar() returns true!");

        SFUN(ui_BeginMainMenuBar, "int", "beginMainMenuBar");
        DOC_FUNC("create and append to a full screen menu-bar.");

        SFUN(ui_EndMainMenuBar, "void", "endMainMenuBar");
        DOC_FUNC("only call EndMainMenuBar() if BeginMainMenuBar() returns true!");

        SFUN(ui_BeginMenu, "int", "beginMenu");
        ARG("string", "label");
        DOC_FUNC("Implied enabled = true");

        SFUN(ui_BeginMenuEx, "int", "beginMenu");
        ARG("string", "label");
        ARG("int", "enabled");
        DOC_FUNC("create a sub-menu entry. only call EndMenu() if this returns true!");

        SFUN(ui_EndMenu, "void", "endMenu");
        DOC_FUNC("only call EndMenu() if BeginMenu() returns true!");

        SFUN(ui_MenuItem, "int", "menuItem");
        ARG("string", "label");
        DOC_FUNC("Implied shortcut = NULL, selected = false, enabled = true");

        SFUN(ui_MenuItemBoolPtr, "int", "menuItem");
        ARG("string", "label");
        ARG("string", "shortcut");
        ARG("UI_Bool", "p_selected");
        ARG("int", "enabled");
        DOC_FUNC(
          "return true when activated + toggle (*p_selected) if p_selected != "
          "NULL");

        SFUN(ui_Knob, "int", "knob");
        ARG("string", "label");
        ARG("UI_Float", "value");
        ARG("float", "min");
        ARG("float", "max");
        DOC_FUNC("Knob Widget. See https://github.com/altschuler/imgui-knobs/");

        SFUN(ui_KnobInt, "int", "knob");
        ARG("string", "label");
        ARG("UI_Int", "value");
        ARG("int", "min");
        ARG("int", "max");
        DOC_FUNC("Knob Widget. See https://github.com/altschuler/imgui-knobs/");

        // bool ImGuiKnobs::Knob(
        // label, *value, min, max,
        // [speed, format, variant, size, flags, steps, angle_min, angle_max])

        SFUN(ui_KnobEx, "int", "knob");
        ARG("string", "label");
        ARG("UI_Float", "value");
        ARG("float", "min");
        ARG("float", "max");
        ARG("float", "speed");
        ARG("string", "format");
        ARG("int", "UI_KnobVariant");
        ARG("float", "size");
        ARG("int", "UI_KnobFlags");
        ARG("int", "steps");
        ARG("float", "angle_min");
        ARG("float", "angle_max");
        DOC_FUNC(
          "Knob Widget. See https://github.com/altschuler/imgui-knobs/ "
          "set `speed=0` to use default sensitivity. "
          "`format` is for numerical formating, e.g. \"%.1fHz\". "
          "`steps` is number of ticks on knob if using UI_KnobVariant.Stepped. "
          "`angle_min` and `angle_max` are in radiams. ");

        SFUN(ui_KnobIntEx, "int", "knob");
        ARG("string", "label");
        ARG("UI_Int", "value");
        ARG("int", "min");
        ARG("int", "max");
        ARG("float", "speed");
        ARG("string", "format");
        ARG("int", "UI_KnobVariant");
        ARG("float", "size");
        ARG("int", "UI_KnobFlags");
        ARG("int", "steps");
        ARG("float", "angle_min");
        ARG("float", "angle_max");
        DOC_FUNC(
          "Knob Widget. See https://github.com/altschuler/imgui-knobs/ "
          "set `speed=0` to use default sensitivity. "
          "`format` is for numerical formating, e.g. \"%.1fHz\". "
          "`steps` is number of ticks on knob if using UI_KnobVariant.Stepped. "
          "`angle_min` and `angle_max` are in radiams. ");

        // Tooltips ---------------------------------------------------------------
        SFUN(ui_BeginItemTooltip, "int", "beginItemTooltip");
        DOC_FUNC(
          "Adds a tooltip on hover for the previous UI item. Remember to call "
          "UI.endToolTip() if this returns true");

        SFUN(ui_BeginTooltip, "int", "beginTooltip");
        DOC_FUNC("begin/append a tooltip window.");

        SFUN(ui_EndTooltip, "void", "endTooltip");
        DOC_FUNC(
          "only call EndTooltip() if BeginTooltip()/BeginItemTooltip() returns "
          "true!");

        SFUN(ui_SetTooltipUnformatted, "void", "tooltip");
        ARG("string", "text");
        DOC_FUNC(
          "set a text-only tooltip. Often used after a ImGui::IsItemHovered() "
          "check. Override any previous call to SetTooltip().\n"
          "SetTooltip() is more or less a shortcut for the 'if (BeginTooltip()) { "
          "Text(...); EndTooltip(); }' idiom (with a subtlety that it discard any "
          "previously submitted tooltip)");

        SFUN(ui_SetItemTooltipUnformatted, "void", "itemTooltip");
        ARG("string", "text");
        DOC_FUNC(
          "set a text-only tooltip if preceding item was hovered. override any "
          "previous call to SetTooltip().\n"
          "itemTooltip() is a shortcut for the 'if "
          "(IsItemHovered(ImGuiHoveredFlags_ForTooltip)) { SetTooltip(...); }' "
          "idiom.");

        // Popups, Modals --------------------------------------------------------

        SFUN(ui_BeginPopup, "int", "beginPopup");
        ARG("string", "str_id");
        ARG("int", "flags");
        DOC_FUNC(
          "return true if the popup is open, and you can start outputting to "
          "it. `flags` is an enum of type UI_WindowFlags\n"
          "beginPopup(): query popup state, if open start appending into the "
          "window. Call endPopup() afterwards if returned true. UI_WindowFlags "
          "are forwarded to the window.");

        SFUN(ui_BeginPopupModal, "int", "beginPopupModal");
        ARG("string", "name");
        ARG("UI_Bool", "p_open");
        ARG("int", "flags");
        DOC_FUNC(
          "return true if the modal is open, and you can start outputting to it. "
          "`flags` is an enum of type UI_WindowFlags\n"
          "beginPopupModal(): block every interaction behind the window, cannot be "
          "closed by user, add a dimming background, has a title bar.");

        SFUN(ui_EndPopup, "void", "endPopup");

        SFUN(ui_OpenPopup, "void", "openPopup");
        ARG("string", "str_id");
        ARG("int", "popup_flags");
        DOC_FUNC(
          "call to mark popup as open (don't call every frame!). `popup_flags` is "
          "an enum of type UI_PopupFlags");

        SFUN(ui_OpenPopupOnItemClick, "void", "openPopupOnItemClick");
        ARG("string", "str_id");
        ARG("int", "popup_flags");
        DOC_FUNC(
          "helper to open popup when clicked on last item. Default to "
          "UI_PopupFlags.MouseButtonRight == 1. (note: actually triggers on the "
          "mouse _released_ event to be consistent with popup behaviors)\n"
          "`popup_flags` is an enum of type UI_PopupFlags");

        SFUN(ui_CloseCurrentPopup, "void", "closeCurrentPopup");
        DOC_FUNC("manually close the popup we have begin-ed into.");

        SFUN(ui_BeginPopupContextItem, "int", "beginPopupContextItem");
        DOC_FUNC("Implied str_id = NULL, popup_flags = 1");

        SFUN(ui_BeginPopupContextItemEx, "int", "beginPopupContextItem");
        ARG("string", "str_id");
        ARG("int", "popup_flags");
        DOC_FUNC(
          "open+begin popup when clicked on last item. Use str_id==NULL to "
          "associate the popup to previous item. If you want to use that on a "
          "non-interactive item such as Text() you need to pass in an explicit ID "
          "here. read comments in .cpp!");

        SFUN(ui_BeginPopupContextWindow, "int", "beginPopupContextWindow");
        DOC_FUNC("Implied str_id = NULL, popup_flags = 1");

        SFUN(ui_BeginPopupContextWindowEx, "int", "beginPopupContextWindow");
        ARG("string", "str_id");
        ARG("int", "popup_flags");
        DOC_FUNC("open+begin popup when clicked on current window.");

        SFUN(ui_BeginPopupContextVoid, "int", "beginPopupContextVoid");
        DOC_FUNC("Implied str_id = NULL, popup_flags = 1");

        SFUN(ui_BeginPopupContextVoidEx, "int", "beginPopupContextVoid");
        ARG("string", "str_id");
        ARG("int", "popup_flags");
        DOC_FUNC("open+begin popup when clicked in void (where there are no windows).");

        SFUN(ui_IsPopupOpen, "int", "isPopupOpen");
        ARG("string", "str_id");
        ARG("int", "flags");
        DOC_FUNC(
          "return true if the popup is open. `flags` is an enum of type "
          "UI_PopupFlags");

        // Tables ----------------------------------------------------------------

        SFUN(ui_BeginTable, "int", "beginTable");
        ARG("string", "str_id");
        ARG("int", "column");
        ARG("int", "flags");
        DOC_FUNC(
          "Implied outer_size = ImVec2(0.0f, 0.0f), inner_width = 0.0f\n"
          "`flags` is an enum of type UI_TableFlags");

        SFUN(ui_BeginTableEx, "int", "beginTable");
        ARG("string", "str_id");
        ARG("int", "column");
        ARG("int", "flags");
        ARG("vec2", "outer_size");
        ARG("float", "inner_width");
        DOC_FUNC("`flags` is an enum of type UI_TableFlags");

        SFUN(ui_EndTable, "void", "endTable");
        DOC_FUNC("only call EndTable() if BeginTable() returns true!");

        SFUN(ui_TableNextRow, "void", "tableNextRow");
        DOC_FUNC("Implied row_flags = 0, min_row_height = 0.0f");

        SFUN(ui_TableNextRowEx, "void", "tableNextRow");
        ARG("int", "row_flags");
        ARG("float", "min_row_height");
        DOC_FUNC("append into the first cell of a new row.");

        SFUN(ui_TableNextColumn, "int", "tableNextColumn");
        DOC_FUNC(
          "append into the next column (or first column of next row if "
          "currently in last column). Return true when column is visible.");

        SFUN(ui_TableSetColumnIndex, "int", "tableSetColumnIndex");
        ARG("int", "column_n");
        DOC_FUNC(
          "append into the specified column. Return true when column is "
          "visible.");

        SFUN(ui_TableSetupColumn, "void", "tableSetupColumn");
        ARG("string", "label");
        ARG("int", "flags");
        DOC_FUNC(
          "Implied init_width_or_weight = 0.0f, user_id = 0\n"
          "`flags` is an enum of type UI_TableColumnFlags");

        SFUN(ui_TableSetupColumnEx, "void", "tableSetupColumn");
        ARG("string", "label");
        ARG("int", "flags");
        ARG("float", "init_width_or_weight");
        ARG("int", "user_id");

        SFUN(ui_TableSetupScrollFreeze, "void", "tableSetupScrollFreeze");
        ARG("int", "cols");
        ARG("int", "rows");
        DOC_FUNC("lock columns/rows so they stay visible when scrolled.");

        SFUN(ui_TableHeader, "void", "tableHeader");
        ARG("string", "label");
        DOC_FUNC("submit one header cell manually (rarely used)");

        SFUN(ui_TableHeadersRow, "void", "tableHeadersRow");
        DOC_FUNC(
          "submit a row with headers cells based on data provided to "
          "TableSetupColumn() + submit context menu");

        SFUN(ui_TableAngledHeadersRow, "void", "tableAngledHeadersRow");
        DOC_FUNC(
          "submit a row with angled headers for every column with the "
          "ImGuiTableColumnFlags_AngledHeader flag. MUST BE FIRST ROW.");

        SFUN(ui_TableGetColumnCount, "int", "tableGetColumnCount");
        DOC_FUNC("return number of columns (value passed to BeginTable)");

        SFUN(ui_TableGetColumnIndex, "int", "tableGetColumnIndex");
        DOC_FUNC("return current column index.");

        SFUN(ui_TableGetRowIndex, "int", "tableGetRowIndex");
        DOC_FUNC("return current row index.");

        SFUN(ui_TableGetColumnName, "string", "tableGetColumnName");
        ARG("int", "column_n");
        DOC_FUNC(
          "return \"\" if column didn't have a name declared by "
          "TableSetupColumn(). Pass -1 to use current column.");

        SFUN(ui_TableGetColumnFlags, "int", "tableGetColumnFlags");
        ARG("int", "column_n");
        DOC_FUNC(
          "return column flags so you can query their Enabled/Visible/Sorted/"
          "Hovered status flags. Pass -1 to use current column.");

        SFUN(ui_TableSetColumnEnabled, "void", "tableSetColumnEnabled");
        ARG("int", "column_n");
        ARG("int", "v");
        DOC_FUNC(
          "change user accessible enabled/disabled state of a column. Set to "
          "false to hide the column. User can use the context menu to change "
          "this themselves (right-click in headers, or right-click in columns "
          "body with UI_TableFlags.ContextMenuInBody)");

        SFUN(ui_TableSetBgColor, "void", "tableSetBgColor");
        ARG("int", "ui_table_bg_target_flag");
        ARG("vec4", "color");
        ARG("int", "column_n");
        DOC_FUNC(
          "change the color of a cell, row, or column. UI_TableBgTarget "
          "flags for details.");

        // Tab Bars, Tabs --------------------------------------------------------

        SFUN(ui_BeginTabBar, "int", "beginTabBar");
        ARG("string", "str_id");
        ARG("int", "flags");
        DOC_FUNC(
          "`flags` is an enum of type UI_TabBarFlags\n"
          "create and append into a TabBar");

        SFUN(ui_EndTabBar, "void", "endTabBar");
        DOC_FUNC("only call EndTabBar() if BeginTabBar() returns true!");

        SFUN(ui_BeginTabItem, "int", "beginTabItem");
        ARG("string", "label");
        ARG("UI_Bool", "p_open");
        ARG("int", "flags");
        DOC_FUNC(
          "create a Tab. Returns true if the Tab is selected.\n"
          "`flags` is an enum of type UI_TabItemFlags");

        SFUN(ui_EndTabItem, "void", "endTabItem");
        DOC_FUNC("only call EndTabItem() if BeginTabItem() returns true!");

        SFUN(ui_TabItemButton, "int", "tabItemButton");
        ARG("string", "label");
        ARG("int", "flags");
        DOC_FUNC(
          "create a Tab behaving like a button. Returns true when clicked.\n"
          "cannot be selected in the tab bar.\n"
          "`flags` is an enum of type UI_TabItemFlags");

        SFUN(ui_SetTabItemClosed, "void", "setTabItemClosed");
        ARG("string", "tab_or_docked_window_label");
        DOC_FUNC(
          "notify TabBar or Docking system of a closed tab/window ahead (useful to "
          "reduce visual flicker on reorderable tab bars). For tab-bar: call after "
          "BeginTabBar() and before Tab submissions. Otherwise call with a window "
          "name.");

        // Docking -------------------------------------------------------------

        SFUN(ui_DockSpaceOverViewport, "int", "dockSpaceOverViewport");
        DOC_FUNC("Implied dockspace_id = 0, viewport = NULL, flags = 0");

        // Disabling -------------------------------------------------------------

        SFUN(ui_BeginDisabled, "void", "beginDisabled");
        ARG("int", "disabled");
        DOC_FUNC(
          "Disable all user interactions and dim items visuals (applying "
          "style.DisabledAlpha over current colors)\n"
          "Those can be nested but it cannot be used to enable an already disabled "
          "section (a single BeginDisabled(true) in the stack is enough to keep "
          "everything disabled)\n"
          "BeginDisabled(false) essentially does nothing useful but is provided to "
          "facilitate use of boolean expressions. If you can avoid calling "
          "BeginDisabled(False)/EndDisabled() best to avoid it.");

        SFUN(ui_BeginDisabled_true, "void", "beginDisabled");
        DOC_FUNC("Equivalent to beginDisabled(true)");

        SFUN(ui_EndDisabled, "void", "endDisabled");

        // Clipping --------------------------------------------------------------

        SFUN(ui_PushClipRect, "void", "pushClipRect");
        ARG("vec2", "clip_rect_min");
        ARG("vec2", "clip_rect_max");
        ARG("int", "intersect_with_current_clip_rect");
        DOC_FUNC(
          "Mouse hovering is affected by ImGui::PushClipRect() calls, unlike "
          "direct calls to ImDrawList::PushClipRect() which are render only.");

        SFUN(ui_PopClipRect, "void", "popClipRect");

        // Focus, Activation -----------------------------------------------------

        SFUN(ui_SetItemDefaultFocus, "void", "setItemDefaultFocus");
        DOC_FUNC("make last item the default focused item of a window.");

        SFUN(ui_SetKeyboardFocusHere, "void", "setKeyboardFocusHere");
        DOC_FUNC("Implied offset = 0");

        SFUN(ui_SetKeyboardFocusHereEx, "void", "setKeyboardFocusHere");
        ARG("int", "offset");
        DOC_FUNC(
          "focus keyboard on the next widget. Use positive 'offset' to access sub "
          "components of a multiple component widget. Use -1 to access previous "
          "widget.");

        // Overlapping Mode -------------------------------------------------------

        SFUN(ui_SetNextItemAllowOverlap, "void", "nextItemAllowOverlap");
        DOC_FUNC(
          "allow next item to be overlapped by a subsequent item. Useful with "
          "invisible buttons, selectable, treenode covering an area where "
          "subsequent items may need to be added. Note that both Selectable() and "
          "TreeNode() have dedicated flags doing this.");

        // Item/Widgets Utilities and Query Functions
        // --------------------------------

        SFUN(ui_IsItemHovered, "int", "isItemHovered");
        ARG("int", "ui_hovered_flags");
        DOC_FUNC(
          "is the last item hovered? (and usable, aka not blocked by a popup, "
          "etc.). See UI_HoveredFlags for more options.");

        SFUN(ui_IsItemActive, "int", "isItemActive");
        DOC_FUNC(
          "is the last item active? (e.g. button being held, text field being "
          "edited. This will continuously return true while holding mouse button "
          "on an item. Items that don't interact will always return false)");

        SFUN(ui_IsItemFocused, "int", "isItemFocused");
        DOC_FUNC("is the last item focused for keyboard/gamepad navigation?");

        SFUN(ui_IsItemClicked, "int", "isItemClicked");
        DOC_FUNC("Implied mouse_button = 0");

        SFUN(ui_IsItemClickedEx, "int", "isItemClicked");
        ARG("int", "mouse_button");
        DOC_FUNC(
          "is the last item hovered and mouse clicked on? (**)  == IsMouseClicked("
          "mouse_button) && IsItemHovered(). Important. (**) this is NOT "
          "equivalent "
          "to the behavior of e.g. Button(). Read comments in function definition."
          "mouse_button is an enum of type UI_MouseButton");

        SFUN(ui_IsItemVisible, "int", "isItemVisible");
        DOC_FUNC(
          "is the last item visible? (items may be out of sight because of "
          "clipping/scrolling)");

        SFUN(ui_IsItemEdited, "int", "isItemEdited");
        DOC_FUNC(
          "did the last item modify its underlying value this frame? or was "
          "pressed? This is generally the same as the 'bool' return value of many "
          "widgets.");

        SFUN(ui_IsItemActivated, "int", "isItemActivated");
        DOC_FUNC("was the last item just made active (item was previously inactive).");

        SFUN(ui_IsItemDeactivated, "int", "isItemDeactivated");
        DOC_FUNC(
          "was the last item just made inactive (item was previously active). "
          "Useful for Undo/Redo patterns with widgets that require continuous "
          "editing.");

        SFUN(ui_IsItemDeactivatedAfterEdit, "int", "isItemDeactivatedAfterEdit");
        DOC_FUNC(
          "was the last item just made inactive and made a value change when it "
          "was "
          "active? (e.g. Slider/Drag moved). Useful for Undo/Redo patterns with "
          "widgets that require continuous editing. Note that you may get false "
          "positives (some widgets such as Combo()/ListBox()/Selectable() will "
          "return true even when clicking an already selected item).");

        SFUN(ui_IsItemToggledOpen, "int", "isItemToggledOpen");
        DOC_FUNC("was the last item open state toggled? set by TreeNode().");

        SFUN(ui_IsAnyItemHovered, "int", "isAnyItemHovered");
        DOC_FUNC("is any item hovered?");

        SFUN(ui_IsAnyItemActive, "int", "isAnyItemActive");
        DOC_FUNC("is any item active?");

        SFUN(ui_IsAnyItemFocused, "int", "isAnyItemFocused");
        DOC_FUNC("is any item focused?");

        SFUN(ui_GetItemID, "int", "getItemID");
        DOC_FUNC("get ID of last item (~~ often same ImGui::GetID(label) beforehand)");

        SFUN(ui_GetItemRectMin, "vec2", "getItemRectMin");
        DOC_FUNC("get upper-left bounding rectangle of the last item (screen space)");

        SFUN(ui_GetItemRectMax, "vec2", "getItemRectMax");
        DOC_FUNC("get lower-right bounding rectangle of the last item (screen space)");

        SFUN(ui_GetItemRectSize, "vec2", "getItemRectSize");
        DOC_FUNC("get size of last item");

        // Viewports ------------------------------------------------------------

        SFUN(ui_GetMainViewport, "UI_Viewport", "getMainViewport");
        DOC_FUNC("Return primary/default viewport. This can never be NULL.");

        // Text Utilities ---------------------------------------------------------

        SFUN(ui_CalcTextSize, "vec2", "calcTextSize");
        ARG("string", "text");
        DOC_FUNC(
          "Implied text_end = NULL, hide_text_after_double_hash = false, "
          "wrap_width = -1.0f");

        SFUN(ui_CalcTextSizeEx, "vec2", "calcTextSize");
        ARG("string", "text");
        ARG("string", "text_end");
        ARG("int", "hide_text_after_double_hash");
        ARG("float", "wrap_width");
        DOC_FUNC("default hide_text_after_double_hash = false, wrap_width = -1.0f");

        // Color Utilities --------------------------------------------------------

        SFUN(ui_ColorConvertRGBtoHSV, "vec3", "convertRGBtoHSV");
        ARG("vec3", "rgb");

        SFUN(ui_ColorConvertHSVtoRGB, "vec3", "convertHSVtoRGB");
        ARG("vec3", "hsv");

        // Inputs Utilities: Keyboard/Mouse/Gamepad --------------------------------

        SFUN(ui_IsKeyDown, "int", "isKeyDown");
        ARG("int", "key");
        DOC_FUNC("is key being held. `key` is an enum of type UI_Key");

        SFUN(ui_IsKeyPressed, "int", "isKeyPressed");
        ARG("int", "key");
        DOC_FUNC("Implied repeat = true. `key` is an enum of type UI_Key");

        SFUN(ui_IsKeyPressedEx, "int", "isKeyPressed");
        ARG("int", "key");
        ARG("int", "repeat");
        DOC_FUNC(
          "was key pressed (went from !Down to Down)? if repeat=true, uses "
          "io.KeyRepeatDelay / KeyRepeatRate. `key` is an enum of type UI_Key");

        SFUN(ui_IsKeyReleased, "int", "isKeyReleased");
        ARG("int", "key");
        DOC_FUNC(
          "was key released (went from Down to !Down)? `key` is an enum of type "
          "UI_Key");

        SFUN(ui_IsKeyChordPressed, "int", "isKeyChordPressed");
        ARG("int", "key_chord");
        DOC_FUNC(
          "was key chord (UI_Key.Mod* + UI_Key) pressed, e.g. you can pass "
          "'UI_Key.Mod_Ctrl | UI_Key.S' as a key-chord. This doesn't do any "
          "routing or "
          "focus check, please consider using Shortcut() function instead.");

        SFUN(ui_GetKeyPressedAmount, "int", "keyPressedAmount");
        ARG("int", "key");
        ARG("float", "repeat_delay");
        ARG("float", "rate");
        DOC_FUNC(
          "uses provided repeat rate/delay. return a count, most often 0 or 1 but "
          "might be >1 if "
          "RepeatRate is small enough that DeltaTime > RepeatRate. `key` is an "
          "enum of type UI_Key");

        SFUN(ui_GetKeyName, "string", "keyName");
        ARG("int", "key");
        DOC_FUNC(
          "`key` is a UI_Key enum, e.g. UI_Key.Backspace. Returns English name of the "
          "key. "
          "Those names are provided for debugging and are not meant to be saved "
          "persistently or compared.");

        SFUN(ui_SetNextFrameWantCaptureKeyboard, "void",
             "setNextFrameWantCaptureKeyboard");
        ARG("int", "want_capture_keyboard");
        DOC_FUNC(
          "Override io.WantCaptureKeyboard flag next frame (said flag is left for "
          "your application to handle, typically when true it instructs your app "
          "to ignore inputs). e.g. force capture keyboard when your widget is "
          "being hovered. This is equivalent to setting \"io.WantCaptureKeyboard = "
          "want_capture_keyboard\"; after the next NewFrame() call.");

        // Inputs Utilities: Mouse specific ---------------------------------------

        SFUN(ui_IsMouseDown, "int", "isMouseDown");
        ARG("int", "button");
        DOC_FUNC("is mouse button held. `button` is an enum of type UI_MouseButton");

        SFUN(ui_IsMouseClicked, "int", "isMouseClicked");
        ARG("int", "button");
        DOC_FUNC("Implied repeat = false. `button` is an enum of type UI_MouseButton");

        SFUN(ui_IsMouseClickedEx, "int", "isMouseClicked");
        ARG("int", "button");
        ARG("int", "repeat");
        DOC_FUNC(
          "did mouse button clicked? (went from !Down to Down). Same as "
          "GetMouseClickedCount() == 1. `button` is an enum of type "
          "UI_MouseButton");

        SFUN(ui_IsMouseReleased, "int", "isMouseReleased");
        ARG("int", "button");
        DOC_FUNC(
          "did mouse button released? (went from Down to !Down). `button` is an "
          "enum of type UI_MouseButton");

        SFUN(ui_IsMouseDoubleClicked, "int", "isMouseDoubleClicked");
        ARG("int", "button");
        DOC_FUNC(
          "did mouse button double-clicked? Same as GetMouseClickedCount() == 2. "
          "`button` is an enum of type UI_MouseButton");

        SFUN(ui_GetMouseClickedCount, "int", "getMouseClickedCount");
        ARG("int", "button");
        DOC_FUNC(
          "return the number of successive mouse-clicks at the time where a click "
          "happen (otherwise 0). `button` is an enum of type UI_MouseButton");

        SFUN(ui_IsMouseHoveringRect, "int", "isMouseHoveringRect");
        ARG("vec2", "r_min");
        ARG("vec2", "r_max");
        DOC_FUNC("Implied clip = true");

        SFUN(ui_IsMouseHoveringRectEx, "int", "isMouseHoveringRect");
        ARG("vec2", "r_min");
        ARG("vec2", "r_max");
        ARG("int", "clip");
        DOC_FUNC(
          "is mouse hovering given bounding rect (in screen space). clipped by "
          "current clipping settings, but disregarding of other consideration of "
          "focus/window ordering/popup-block.");

        SFUN(ui_IsMousePosValid, "int", "isMousePosValid");
        // ARG("vec2", "mouse_pos");
        DOC_FUNC(
          "by convention we use (-FLT_MAX,-FLT_MAX) to denote that there is no "
          "mouse available");

        SFUN(ui_GetMousePos, "vec2", "getMousePos");
        DOC_FUNC(
          "shortcut to ImGui::GetIO().MousePos provided by user, to be "
          "consistent with other calls");

        SFUN(ui_GetMousePosOnOpeningCurrentPopup, "vec2",
             "getMousePosOnOpeningCurrentPopup");
        DOC_FUNC(
          "retrieve mouse position at the time of opening popup we have "
          "BeginPopup() into "
          "(helper to avoid user backing that value themselves)");

        SFUN(ui_IsMouseDragging, "int", "isMouseDragging");
        ARG("int", "button");
        ARG("float", "lock_threshold");
        DOC_FUNC(
          "is mouse dragging? (uses io.MouseDraggingThreshold if lock_threshold < "
          "0.0f). `button` is an enum of type UI_MouseButton");

        SFUN(ui_GetMouseDragDelta, "vec2", "getMouseDragDelta");
        ARG("int", "button");
        ARG("float", "lock_threshold");
        DOC_FUNC(
          "return the delta from the initial clicking position while the mouse "
          "button is pressed or was just released. This is locked and return "
          "0.0f until the mouse moves past a distance threshold at least once "
          "(uses io.MouseDraggingThreshold if lock_threshold < 0.0f)\n"
          "`button` is an enum of type UI_MouseButton");

        SFUN(ui_ResetMouseDragDelta, "void", "resetMouseDragDelta");
        DOC_FUNC("Implied button = 0");

        SFUN(ui_ResetMouseDragDeltaEx, "void", "resetMouseDragDelta");
        ARG("int", "button");
        DOC_FUNC("`button` is an enum of type UI_MouseButton");

        SFUN(ui_GetMouseCursor, "int", "mouseCursor");
        DOC_FUNC(
          "get desired mouse cursor shape. Important: reset in ImGui::NewFrame(), "
          "this is updated during the frame. valid before Render(). If you use "
          "software rendering by setting io.MouseDrawCursor ImGui will render "
          "those for you");

        SFUN(ui_SetMouseCursor, "void", "setMouseCursor");
        ARG("int", "cursor_type");
        DOC_FUNC(
          "set desired mouse cursor shape. `cursor_type` is an enum of type "
          "UI_MouseCursor");

        SFUN(ui_SetNextFrameWantCaptureMouse, "void", "setNextFrameWantCaptureMouse");
        ARG("int", "want_capture_mouse");
        DOC_FUNC(
          "Override io.WantCaptureMouse flag next frame (said flag is left for "
          "your application to handle, typical when true it instucts your app to "
          "ignore inputs). This is equivalent to setting \"io.WantCaptureMouse = "
          "want_capture_mouse;\" after the next NewFrame() call.");

        // Clipboard Utilities ---------------------------------------------------

        SFUN(ui_GetClipboardText, "string", "clipboardText");
        DOC_FUNC("get current text from the clipboard (e.g. result of Ctrl+C)");

        SFUN(ui_SetClipboardText, "void", "clipboardText");
        ARG("string", "text");
        DOC_FUNC("set the clipboard text");

        // styles ----------------------------------------------------------------
        QUERY->add_sfun(QUERY, ui_styleColorsDark, "void", "styleColorsDark");
        QUERY->add_sfun(QUERY, ui_styleColorsLight, "void", "styleColorsLight");
        QUERY->add_sfun(QUERY, ui_styleColorsClassic, "void", "styleColorsClassic");

        QUERY->end_class(QUERY);
    } // UI
}

// ============================================================================
// IO
// ============================================================================

CK_DLL_SFUN(ui_want_capture_mouse)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_GetIO()->WantCaptureMouse;
}

CK_DLL_SFUN(ui_want_capture_keyboard)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_GetIO()->WantCaptureKeyboard;
}

CK_DLL_SFUN(ui_AddFontFromFileTTF)
{
    if (!verifyInitialization(SHRED)) return;
    const char* filename = API->object->str(GET_NEXT_STRING(ARGS));
    float size           = GET_NEXT_FLOAT(ARGS);
    ImFont* font         = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename, size);

    // UI_Font CTOR
    Chuck_Object* font_ckobj = chugin_createCkObj("UI_Font", false, SHRED);
    OBJ_MEMBER_UINT(font_ckobj, ui_font_ptr_offset) = (t_CKUINT)font;

    RETURN->v_object = font_ckobj;
}

CK_DLL_SFUN(ui_io_get_ConfigFlags)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_GetIO()->ConfigFlags;
}

CK_DLL_SFUN(ui_io_set_ConfigFlags)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_GetIO()->ConfigFlags = GET_NEXT_INT(ARGS);
}

CK_DLL_SFUN(ui_io_get_BackendFlags)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_GetIO()->BackendFlags;
}

CK_DLL_SFUN(ui_io_set_BackendFlags)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_GetIO()->BackendFlags = GET_NEXT_INT(ARGS);
}

CK_DLL_SFUN(ui_io_get_DisplaySize)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImVec2 ds = cimgui::ImGui_GetIO()->DisplaySize;
    RETURN->v_vec2    = { ds.x, ds.y };
}

CK_DLL_SFUN(ui_io_get_DeltaTime)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_float = cimgui::ImGui_GetIO()->DeltaTime;
}

CK_DLL_SFUN(ui_io_get_IniSavingRate)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_float = cimgui::ImGui_GetIO()->IniSavingRate;
}

CK_DLL_SFUN(ui_io_set_IniSavingRate)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_GetIO()->IniSavingRate = GET_NEXT_FLOAT(ARGS);
}

CK_DLL_SFUN(ui_io_get_IniFilename)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_string = chugin_createCkString(cimgui::ImGui_GetIO()->IniFilename, false);
}

CK_DLL_SFUN(ui_io_set_IniFilename)
{
    if (!verifyInitialization(SHRED)) return;
    // KNOWN MEM LEAK (but this should be very rarely called so w/e)
    cimgui::ImGui_GetIO()->IniFilename
      = strdup(API->object->str(GET_NEXT_STRING(ARGS)));
}

CK_DLL_SFUN(ui_io_get_LogFilename)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_string = chugin_createCkString(cimgui::ImGui_GetIO()->LogFilename, false);
}

CK_DLL_SFUN(ui_io_set_LogFilename)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_GetIO()->LogFilename
      = strdup(API->object->str(GET_NEXT_STRING(ARGS)));
}

CK_DLL_SFUN(ui_io_get_FontGlobalScale)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_float = cimgui::ImGui_GetIO()->FontGlobalScale;
}

CK_DLL_SFUN(ui_io_set_FontGlobalScale)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_GetIO()->FontGlobalScale = GET_NEXT_FLOAT(ARGS);
}

// ============================================================================
// Main
// ============================================================================

CK_DLL_SFUN(ui_get_style)
{
    if (!verifyInitialization(SHRED)) return;

    Chuck_Object* style_obj = chugin_createCkObj("UI_Style", false, SHRED);
    OBJ_MEMBER_UINT(style_obj, ui_style_ptr_offset)
      = (t_CKUINT)cimgui::ImGui_GetStyle();
    RETURN->v_object = style_obj;
}

// ============================================================================
// Demo, Debug, Information
// ============================================================================
CK_DLL_SFUN(ui_ShowDemoWindow)
{
    if (!verifyInitialization(SHRED)) return;
    bool* p_open = CHUGL_UI_VAL_PTR(bool, GET_NEXT_OBJECT(ARGS), ui_bool_val_offset);
    cimgui::ImGui_ShowDemoWindow(p_open);
}

CK_DLL_SFUN(ui_ShowMetricsWindow)
{
    if (!verifyInitialization(SHRED)) return;
    bool* p_open = CHUGL_UI_VAL_PTR(bool, GET_NEXT_OBJECT(ARGS), ui_bool_val_offset);
    cimgui::ImGui_ShowMetricsWindow(p_open);
}

CK_DLL_SFUN(ui_ShowDebugLogWindow)
{
    if (!verifyInitialization(SHRED)) return;
    bool* p_open = CHUGL_UI_VAL_PTR(bool, GET_NEXT_OBJECT(ARGS), ui_bool_val_offset);
    cimgui::ImGui_ShowDebugLogWindow(p_open);
}

CK_DLL_SFUN(ui_showStyleEditor)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_ShowStyleEditor(NULL);
}

CK_DLL_SFUN(ui_ShowIDStackToolWindowEx)
{
    if (!verifyInitialization(SHRED)) return;
    bool* p_open = CHUGL_UI_VAL_PTR(bool, GET_NEXT_OBJECT(ARGS), ui_bool_val_offset);
    cimgui::ImGui_ShowIDStackToolWindowEx(p_open);
}

CK_DLL_SFUN(ui_ShowAboutWindow)
{
    if (!verifyInitialization(SHRED)) return;
    bool* p_open = CHUGL_UI_VAL_PTR(bool, GET_NEXT_OBJECT(ARGS), ui_bool_val_offset);
    cimgui::ImGui_ShowAboutWindow(p_open);
}

CK_DLL_SFUN(ui_ShowStyleSelector)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    RETURN->v_int     = cimgui::ImGui_ShowStyleSelector(label);
}

CK_DLL_SFUN(ui_ShowFontSelector)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    cimgui::ImGui_ShowFontSelector(label);
}

CK_DLL_SFUN(ui_ShowUserGuide)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_ShowUserGuide();
}

CK_DLL_SFUN(ui_GetVersion)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_string = chugin_createCkString(cimgui::ImGui_GetVersion(), false);
}

// ============================================================================
// structs
// ============================================================================

// UI_Bool -----------------------------------------------------------------

CK_DLL_CTOR(ui_bool_ctor)
{
    OBJ_MEMBER_UINT(SELF, ui_bool_val_offset) = (t_CKUINT)0;
}

CK_DLL_CTOR(ui_bool_ctor_val)
{
    OBJ_MEMBER_UINT(SELF, ui_bool_val_offset) = GET_NEXT_INT(ARGS) ? 1 : 0;
}

CK_DLL_MFUN(ui_bool_get_value)
{
    RETURN->v_int = OBJ_MEMBER_INT(SELF, ui_bool_val_offset);
}

CK_DLL_MFUN(ui_bool_set_value)
{
    OBJ_MEMBER_INT(SELF, ui_bool_val_offset) = GET_NEXT_INT(ARGS);
}

// UI_String -----------------------------------------------------------------

#define UI_STRING_DEFAULT_SIZE 256

CK_DLL_CTOR(ui_string_ctor)
{
    char* s                                     = new char[UI_STRING_DEFAULT_SIZE];
    s[0]                                        = '\0';
    OBJ_MEMBER_UINT(SELF, ui_string_ptr_offset) = (t_CKUINT)s;
    OBJ_MEMBER_UINT(SELF, ui_string_cap_offset) = UI_STRING_DEFAULT_SIZE;
}

CK_DLL_CTOR(ui_string_ctor_str)
{
    const char* ck_str = API->object->str(GET_NEXT_STRING(ARGS));
    size_t ck_str_len  = strlen(ck_str);
    size_t str_cap     = MAX(UI_STRING_DEFAULT_SIZE, ck_str_len + 1);

    char* s = new char[str_cap];
    strncpy(s, ck_str, ck_str_len);
    s[ck_str_len] = '\0';

    OBJ_MEMBER_UINT(SELF, ui_string_ptr_offset) = (t_CKUINT)s;
    OBJ_MEMBER_UINT(SELF, ui_string_cap_offset) = str_cap;
}

CK_DLL_DTOR(ui_string_dtor)
{
    char* s = (char*)OBJ_MEMBER_UINT(SELF, ui_string_ptr_offset);
    delete[] s;
    OBJ_MEMBER_UINT(SELF, ui_string_ptr_offset) = 0;
    OBJ_MEMBER_UINT(SELF, ui_string_cap_offset) = 0;
}

CK_DLL_MFUN(ui_string_get_value)
{
    char* s          = (char*)OBJ_MEMBER_UINT(SELF, ui_string_ptr_offset);
    RETURN->v_string = chugin_createCkString(s, false);
}

CK_DLL_MFUN(ui_string_set_value)
{
    // get current string + cap
    char* ui_str = (char*)OBJ_MEMBER_UINT(SELF, ui_string_ptr_offset);
    t_CKUINT cap = OBJ_MEMBER_UINT(SELF, ui_string_cap_offset);

    const char* ck_str = API->object->str(GET_NEXT_STRING(ARGS));
    size_t ck_str_len  = strlen(ck_str);

    // resize storage if needed
    if (ck_str_len >= cap) {
        size_t new_cap = MAX(cap * 2, ck_str_len + 1);
        ui_str         = (char*)reallocate(ui_str, cap, new_cap);
        OBJ_MEMBER_UINT(SELF, ui_string_ptr_offset) = (t_CKUINT)ui_str;
        OBJ_MEMBER_UINT(SELF, ui_string_cap_offset) = new_cap;
    }

    // copy string
    strncpy(ui_str, ck_str, ck_str_len);
    ui_str[ck_str_len] = '\0';
}

// UI_Int -------------------------------------------------------------------
CK_DLL_CTOR(ui_int_ctor)
{
    int* i                                   = new int(0);
    OBJ_MEMBER_UINT(SELF, ui_int_ptr_offset) = (t_CKUINT)i;
}

CK_DLL_CTOR(ui_int_ctor_int)
{
    int* i                                   = new int(GET_NEXT_INT(ARGS));
    OBJ_MEMBER_UINT(SELF, ui_int_ptr_offset) = (t_CKUINT)i;
}

CK_DLL_DTOR(ui_int_dtor)
{
    int* i = (int*)OBJ_MEMBER_UINT(SELF, ui_int_ptr_offset);
    delete i;
    OBJ_MEMBER_UINT(SELF, ui_int_ptr_offset) = 0;
}

CK_DLL_MFUN(ui_int_get_value)
{
    int* i        = (int*)OBJ_MEMBER_UINT(SELF, ui_int_ptr_offset);
    RETURN->v_int = *i;
}

CK_DLL_MFUN(ui_int_set_value)
{
    int* i        = (int*)OBJ_MEMBER_UINT(SELF, ui_int_ptr_offset);
    *i            = GET_NEXT_INT(ARGS);
    RETURN->v_int = *i;
}

// UI_Int2 -------------------------------------------------------------------

CK_DLL_CTOR(ui_int2_ctor)
{
    int* i                                    = new int[2]{ 0, 0 };
    OBJ_MEMBER_UINT(SELF, ui_int2_ptr_offset) = (t_CKUINT)i;
}

CK_DLL_DTOR(ui_int2_dtor)
{
    int* i = (int*)OBJ_MEMBER_UINT(SELF, ui_int2_ptr_offset);
    delete[] i;
    OBJ_MEMBER_UINT(SELF, ui_int2_ptr_offset) = 0;
}

CK_DLL_MFUN(ui_int2_get_x)
{
    int* i        = (int*)OBJ_MEMBER_UINT(SELF, ui_int2_ptr_offset);
    RETURN->v_int = i[0];
}

CK_DLL_MFUN(ui_int2_get_y)
{
    int* i        = (int*)OBJ_MEMBER_UINT(SELF, ui_int2_ptr_offset);
    RETURN->v_int = i[1];
}

CK_DLL_MFUN(ui_int2_set)
{
    int* i = (int*)OBJ_MEMBER_UINT(SELF, ui_int2_ptr_offset);
    i[0]   = GET_NEXT_INT(ARGS);
    i[1]   = GET_NEXT_INT(ARGS);
}

// UI_Int3 -------------------------------------------------------------------

CK_DLL_CTOR(ui_int3_ctor)
{
    int* i                                    = new int[3]{ 0, 0, 0 };
    OBJ_MEMBER_UINT(SELF, ui_int3_ptr_offset) = (t_CKUINT)i;
}

CK_DLL_DTOR(ui_int3_dtor)
{
    int* i = (int*)OBJ_MEMBER_UINT(SELF, ui_int3_ptr_offset);
    delete[] i;
    OBJ_MEMBER_UINT(SELF, ui_int3_ptr_offset) = 0;
}

CK_DLL_MFUN(ui_int3_get_x)
{
    int* i        = (int*)OBJ_MEMBER_UINT(SELF, ui_int3_ptr_offset);
    RETURN->v_int = i[0];
}

CK_DLL_MFUN(ui_int3_get_y)
{
    int* i        = (int*)OBJ_MEMBER_UINT(SELF, ui_int3_ptr_offset);
    RETURN->v_int = i[1];
}

CK_DLL_MFUN(ui_int3_get_z)
{
    int* i        = (int*)OBJ_MEMBER_UINT(SELF, ui_int3_ptr_offset);
    RETURN->v_int = i[2];
}

CK_DLL_MFUN(ui_int3_set)
{
    int* i = (int*)OBJ_MEMBER_UINT(SELF, ui_int3_ptr_offset);
    i[0]   = GET_NEXT_INT(ARGS);
    i[1]   = GET_NEXT_INT(ARGS);
    i[2]   = GET_NEXT_INT(ARGS);
}

// UI_Int4 -------------------------------------------------------------------

CK_DLL_CTOR(ui_int4_ctor)
{
    int* i                                    = new int[4]{ 0, 0, 0, 0 };
    OBJ_MEMBER_UINT(SELF, ui_int4_ptr_offset) = (t_CKUINT)i;
}

CK_DLL_DTOR(ui_int4_dtor)
{
    int* i = (int*)OBJ_MEMBER_UINT(SELF, ui_int4_ptr_offset);
    delete[] i;
    OBJ_MEMBER_UINT(SELF, ui_int4_ptr_offset) = 0;
}

CK_DLL_MFUN(ui_int4_get_x)
{
    int* i        = (int*)OBJ_MEMBER_UINT(SELF, ui_int4_ptr_offset);
    RETURN->v_int = i[0];
}

CK_DLL_MFUN(ui_int4_get_y)
{
    int* i        = (int*)OBJ_MEMBER_UINT(SELF, ui_int4_ptr_offset);
    RETURN->v_int = i[1];
}

CK_DLL_MFUN(ui_int4_get_z)
{
    int* i        = (int*)OBJ_MEMBER_UINT(SELF, ui_int4_ptr_offset);
    RETURN->v_int = i[2];
}

CK_DLL_MFUN(ui_int4_get_w)
{
    int* i        = (int*)OBJ_MEMBER_UINT(SELF, ui_int4_ptr_offset);
    RETURN->v_int = i[3];
}

CK_DLL_MFUN(ui_int4_set)
{
    int* i = (int*)OBJ_MEMBER_UINT(SELF, ui_int4_ptr_offset);
    i[0]   = GET_NEXT_INT(ARGS);
    i[1]   = GET_NEXT_INT(ARGS);
    i[2]   = GET_NEXT_INT(ARGS);
    i[3]   = GET_NEXT_INT(ARGS);
}

// UI_Float -------------------------------------------------------------------
CK_DLL_CTOR(ui_float_ctor)
{
    float* f                                   = new float(0.0f);
    OBJ_MEMBER_UINT(SELF, ui_float_ptr_offset) = (t_CKUINT)f;
}

CK_DLL_CTOR(ui_float_ctor_float)
{
    float* f                                   = new float(GET_NEXT_FLOAT(ARGS));
    OBJ_MEMBER_UINT(SELF, ui_float_ptr_offset) = (t_CKUINT)f;
}

CK_DLL_DTOR(ui_float_dtor)
{
    float* f = (float*)OBJ_MEMBER_UINT(SELF, ui_float_ptr_offset);
    delete f;
    OBJ_MEMBER_UINT(SELF, ui_float_ptr_offset) = 0;
}

CK_DLL_MFUN(ui_float_get_value)
{
    float* f        = (float*)OBJ_MEMBER_UINT(SELF, ui_float_ptr_offset);
    RETURN->v_float = *f;
}

CK_DLL_MFUN(ui_float_set_value)
{
    float* f        = (float*)OBJ_MEMBER_UINT(SELF, ui_float_ptr_offset);
    *f              = GET_NEXT_FLOAT(ARGS);
    RETURN->v_float = *f;
}

// UI_Float2 -----------------------------------------------------------------

CK_DLL_CTOR(ui_float2_ctor)
{
    float* f                                    = new float[2]{ 0.0f, 0.0f };
    OBJ_MEMBER_UINT(SELF, ui_float2_ptr_offset) = (t_CKUINT)f;
}

CK_DLL_CTOR(ui_float2_ctor_with_value)
{
    t_CKVEC2 v = GET_NEXT_VEC2(ARGS);
    float* f   = new float[2]{ (float)v.x, (float)v.y };
    OBJ_MEMBER_UINT(SELF, ui_float2_ptr_offset) = (t_CKUINT)f;
}

CK_DLL_CTOR(ui_float2_ctor_with_floats)
{
    float x                                     = GET_NEXT_FLOAT(ARGS);
    float y                                     = GET_NEXT_FLOAT(ARGS);
    float* f                                    = new float[2]{ x, y };
    OBJ_MEMBER_UINT(SELF, ui_float2_ptr_offset) = (t_CKUINT)f;
}

CK_DLL_DTOR(ui_float2_dtor)
{
    float* f = (float*)OBJ_MEMBER_UINT(SELF, ui_float2_ptr_offset);
    delete[] f;
    OBJ_MEMBER_UINT(SELF, ui_float2_ptr_offset) = 0;
}

CK_DLL_MFUN(ui_float2_get_value)
{
    float* f       = (float*)OBJ_MEMBER_UINT(SELF, ui_float2_ptr_offset);
    RETURN->v_vec2 = { f[0], f[1] };
}

CK_DLL_MFUN(ui_float2_set_value)
{
    float* f       = (float*)OBJ_MEMBER_UINT(SELF, ui_float2_ptr_offset);
    t_CKVEC2 v     = GET_NEXT_VEC2(ARGS);
    f[0]           = v.x;
    f[1]           = v.y;
    RETURN->v_vec2 = v;
}

// UI_Float3 -----------------------------------------------------------------

CK_DLL_CTOR(ui_float3_ctor)
{
    float* f                                    = new float[3]{ 0.0f, 0.0f, 0.0f };
    OBJ_MEMBER_UINT(SELF, ui_float3_ptr_offset) = (t_CKUINT)f;
}

CK_DLL_CTOR(ui_float3_ctor_with_value)
{
    t_CKVEC3 v = GET_NEXT_VEC3(ARGS);
    float* f   = new float[3]{ (float)v.x, (float)v.y, (float)v.z };
    OBJ_MEMBER_UINT(SELF, ui_float3_ptr_offset) = (t_CKUINT)f;
}

CK_DLL_CTOR(ui_float3_ctor_with_floats)
{
    float x                                     = GET_NEXT_FLOAT(ARGS);
    float y                                     = GET_NEXT_FLOAT(ARGS);
    float z                                     = GET_NEXT_FLOAT(ARGS);
    float* f                                    = new float[3]{ x, y, z };
    OBJ_MEMBER_UINT(SELF, ui_float3_ptr_offset) = (t_CKUINT)f;
}

CK_DLL_DTOR(ui_float3_dtor)
{
    float* f = (float*)OBJ_MEMBER_UINT(SELF, ui_float3_ptr_offset);
    delete[] f;
    OBJ_MEMBER_UINT(SELF, ui_float3_ptr_offset) = 0;
}

CK_DLL_MFUN(ui_float3_get_value)
{
    float* f       = (float*)OBJ_MEMBER_UINT(SELF, ui_float3_ptr_offset);
    RETURN->v_vec3 = { f[0], f[1], f[2] };
}

CK_DLL_MFUN(ui_float3_set_value)
{
    float* f       = (float*)OBJ_MEMBER_UINT(SELF, ui_float3_ptr_offset);
    t_CKVEC3 v     = GET_NEXT_VEC3(ARGS);
    f[0]           = v.x;
    f[1]           = v.y;
    f[2]           = v.z;
    RETURN->v_vec3 = v;
}

// UI_Float4 -----------------------------------------------------------------

CK_DLL_CTOR(ui_float4_ctor)
{
    float* f = new float[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
    OBJ_MEMBER_UINT(SELF, ui_float4_ptr_offset) = (t_CKUINT)f;
}

CK_DLL_CTOR(ui_float4_ctor_with_value)
{
    t_CKVEC4 v = GET_NEXT_VEC4(ARGS);
    float* f   = new float[4]{ (float)v.x, (float)v.y, (float)v.z, (float)v.w };
    OBJ_MEMBER_UINT(SELF, ui_float4_ptr_offset) = (t_CKUINT)f;
}

CK_DLL_CTOR(ui_float4_ctor_with_floats)
{
    float x                                     = GET_NEXT_FLOAT(ARGS);
    float y                                     = GET_NEXT_FLOAT(ARGS);
    float z                                     = GET_NEXT_FLOAT(ARGS);
    float w                                     = GET_NEXT_FLOAT(ARGS);
    float* f                                    = new float[4]{ x, y, z, w };
    OBJ_MEMBER_UINT(SELF, ui_float4_ptr_offset) = (t_CKUINT)f;
}

CK_DLL_DTOR(ui_float4_dtor)
{
    float* f = (float*)OBJ_MEMBER_UINT(SELF, ui_float4_ptr_offset);
    delete[] f;
    OBJ_MEMBER_UINT(SELF, ui_float4_ptr_offset) = 0;
}

CK_DLL_MFUN(ui_float4_get_value)
{
    float* f       = (float*)OBJ_MEMBER_UINT(SELF, ui_float4_ptr_offset);
    RETURN->v_vec4 = { f[0], f[1], f[2], f[3] };
}

CK_DLL_MFUN(ui_float4_set_value)
{
    float* f       = (float*)OBJ_MEMBER_UINT(SELF, ui_float4_ptr_offset);
    t_CKVEC4 v     = GET_NEXT_VEC4(ARGS);
    f[0]           = v.x;
    f[1]           = v.y;
    f[2]           = v.z;
    f[3]           = v.w;
    RETURN->v_vec4 = v;
}

// ============================================================================
// UI_Vieport
// ============================================================================

CK_DLL_MFUN(ui_viewport_get_id)
{
    cimgui::ImGuiViewport* vp
      = (cimgui::ImGuiViewport*)OBJ_MEMBER_UINT(SELF, ui_viewport_ptr_offset);
    RETURN->v_int = vp->ID;
}

CK_DLL_MFUN(ui_viewport_get_flags)
{
    cimgui::ImGuiViewport* vp
      = (cimgui::ImGuiViewport*)OBJ_MEMBER_UINT(SELF, ui_viewport_ptr_offset);
    RETURN->v_int = vp->Flags;
}

CK_DLL_MFUN(ui_viewport_get_pos)
{
    cimgui::ImGuiViewport* vp
      = (cimgui::ImGuiViewport*)OBJ_MEMBER_UINT(SELF, ui_viewport_ptr_offset);
    RETURN->v_vec2 = { vp->Pos.x, vp->Pos.y };
}

CK_DLL_MFUN(ui_viewport_get_size)
{
    cimgui::ImGuiViewport* vp
      = (cimgui::ImGuiViewport*)OBJ_MEMBER_UINT(SELF, ui_viewport_ptr_offset);
    RETURN->v_vec2 = { vp->Size.x, vp->Size.y };
}

CK_DLL_MFUN(ui_viewport_get_work_pos)
{
    cimgui::ImGuiViewport* vp
      = (cimgui::ImGuiViewport*)OBJ_MEMBER_UINT(SELF, ui_viewport_ptr_offset);
    RETURN->v_vec2 = { vp->WorkPos.x, vp->WorkPos.y };
}

CK_DLL_MFUN(ui_viewport_get_work_size)
{
    cimgui::ImGuiViewport* vp
      = (cimgui::ImGuiViewport*)OBJ_MEMBER_UINT(SELF, ui_viewport_ptr_offset);
    RETURN->v_vec2 = { vp->WorkSize.x, vp->WorkSize.y };
}

CK_DLL_MFUN(ui_viewport_get_dpi_scale)
{
    cimgui::ImGuiViewport* vp
      = (cimgui::ImGuiViewport*)OBJ_MEMBER_UINT(SELF, ui_viewport_ptr_offset);
    RETURN->v_float = vp->DpiScale;
}

CK_DLL_MFUN(ui_viewport_get_parent_viewport_id)
{
    cimgui::ImGuiViewport* vp
      = (cimgui::ImGuiViewport*)OBJ_MEMBER_UINT(SELF, ui_viewport_ptr_offset);
    RETURN->v_int = vp->ParentViewportId;
}

CK_DLL_MFUN(ui_viewport_get_center)
{
    cimgui::ImGuiViewport* vp
      = (cimgui::ImGuiViewport*)OBJ_MEMBER_UINT(SELF, ui_viewport_ptr_offset);
    cimgui::ImVec2 center = cimgui::ImGuiViewport_GetCenter(vp);
    RETURN->v_vec2        = { center.x, center.y };
}

CK_DLL_MFUN(ui_viewport_get_work_center)
{
    cimgui::ImGuiViewport* vp
      = (cimgui::ImGuiViewport*)OBJ_MEMBER_UINT(SELF, ui_viewport_ptr_offset);
    cimgui::ImVec2 center = cimgui::ImGuiViewport_GetWorkCenter(vp);
    RETURN->v_vec2        = { center.x, center.y };
}

// ============================================================================
// UI_Style
// ============================================================================

CK_DLL_MFUN(ui_style_get_alpha)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->Alpha;
}

CK_DLL_MFUN(ui_style_get_disabled_alpha)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->DisabledAlpha;
}

CK_DLL_MFUN(ui_style_get_window_padding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_vec2 = { style->WindowPadding.x, style->WindowPadding.y };
}

CK_DLL_MFUN(ui_style_get_window_rounding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->WindowRounding;
}

CK_DLL_MFUN(ui_style_get_window_border_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->WindowBorderSize;
}

CK_DLL_MFUN(ui_style_get_window_min_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_vec2 = { style->WindowMinSize.x, style->WindowMinSize.y };
}

CK_DLL_MFUN(ui_style_get_window_title_align)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_vec2 = { style->WindowTitleAlign.x, style->WindowTitleAlign.y };
}

CK_DLL_MFUN(ui_style_get_window_menu_button_position)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_int = style->WindowMenuButtonPosition;
}

CK_DLL_MFUN(ui_style_get_child_rounding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->ChildRounding;
}

CK_DLL_MFUN(ui_style_get_child_border_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->ChildBorderSize;
}

CK_DLL_MFUN(ui_style_get_popup_rounding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->PopupRounding;
}

CK_DLL_MFUN(ui_style_get_popup_border_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->PopupBorderSize;
}

CK_DLL_MFUN(ui_style_get_frame_padding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_vec2 = { style->FramePadding.x, style->FramePadding.y };
}

CK_DLL_MFUN(ui_style_get_frame_rounding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->FrameRounding;
}

CK_DLL_MFUN(ui_style_get_frame_border_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->FrameBorderSize;
}

CK_DLL_MFUN(ui_style_get_item_spacing)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_vec2 = { style->ItemSpacing.x, style->ItemSpacing.y };
}

CK_DLL_MFUN(ui_style_get_item_inner_spacing)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_vec2 = { style->ItemInnerSpacing.x, style->ItemInnerSpacing.y };
}

CK_DLL_MFUN(ui_style_get_cell_padding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_vec2 = { style->CellPadding.x, style->CellPadding.y };
}

CK_DLL_MFUN(ui_style_get_touch_extra_padding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_vec2 = { style->TouchExtraPadding.x, style->TouchExtraPadding.y };
}

CK_DLL_MFUN(ui_style_get_indent_spacing)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->IndentSpacing;
}

CK_DLL_MFUN(ui_style_get_columns_min_spacing)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->ColumnsMinSpacing;
}

CK_DLL_MFUN(ui_style_get_scrollbar_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->ScrollbarSize;
}

CK_DLL_MFUN(ui_style_get_scrollbar_rounding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->ScrollbarRounding;
}

CK_DLL_MFUN(ui_style_get_grab_min_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->GrabMinSize;
}

CK_DLL_MFUN(ui_style_get_grab_rounding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->GrabRounding;
}

CK_DLL_MFUN(ui_style_get_log_slider_deadzone)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->LogSliderDeadzone;
}

CK_DLL_MFUN(ui_style_get_tab_rounding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->TabRounding;
}

CK_DLL_MFUN(ui_style_get_tab_border_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->TabBorderSize;
}

CK_DLL_MFUN(ui_style_get_tab_min_width_for_close_button)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->TabMinWidthForCloseButton;
}

CK_DLL_MFUN(ui_style_get_tab_bar_border_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->TabBarBorderSize;
}

CK_DLL_MFUN(ui_style_get_table_angled_headers_angle)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->TableAngledHeadersAngle;
}

CK_DLL_MFUN(ui_style_get_table_angled_headers_text_align)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_vec2
      = { style->TableAngledHeadersTextAlign.x, style->TableAngledHeadersTextAlign.y };
}

CK_DLL_MFUN(ui_style_get_color_button_position)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_int = style->ColorButtonPosition;
}

CK_DLL_MFUN(ui_style_get_button_text_align)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_vec2 = { style->ButtonTextAlign.x, style->ButtonTextAlign.y };
}

CK_DLL_MFUN(ui_style_get_selectable_text_align)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_vec2 = { style->SelectableTextAlign.x, style->SelectableTextAlign.y };
}

CK_DLL_MFUN(ui_style_get_separator_text_border_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->SeparatorTextBorderSize;
}

CK_DLL_MFUN(ui_style_get_separator_text_align)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_vec2 = { style->SeparatorTextAlign.x, style->SeparatorTextAlign.y };
}

CK_DLL_MFUN(ui_style_get_separator_text_padding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_vec2 = { style->SeparatorTextPadding.x, style->SeparatorTextPadding.y };
}

CK_DLL_MFUN(ui_style_get_display_window_padding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_vec2 = { style->DisplayWindowPadding.x, style->DisplayWindowPadding.y };
}

CK_DLL_MFUN(ui_style_get_display_safe_area_padding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_vec2
      = { style->DisplaySafeAreaPadding.x, style->DisplaySafeAreaPadding.y };
}

CK_DLL_MFUN(ui_style_get_docking_separator_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->DockingSeparatorSize;
}

CK_DLL_MFUN(ui_style_get_mouse_cursor_scale)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->MouseCursorScale;
}

CK_DLL_MFUN(ui_style_get_anti_aliased_lines)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_int = style->AntiAliasedLines;
}

CK_DLL_MFUN(ui_style_get_anti_aliased_lines_use_tex)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_int = style->AntiAliasedLinesUseTex;
}

CK_DLL_MFUN(ui_style_get_anti_aliased_fill)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_int = style->AntiAliasedFill;
}

CK_DLL_MFUN(ui_style_get_curve_tessellation_tol)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->CurveTessellationTol;
}

CK_DLL_MFUN(ui_style_get_circle_tessellation_max_error)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->CircleTessellationMaxError;
}

CK_DLL_MFUN(ui_style_get_color)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    int idx        = GET_NEXT_INT(ARGS);
    RETURN->v_vec4 = { style->Colors[idx].x, style->Colors[idx].y, style->Colors[idx].z,
                       style->Colors[idx].w };
}

CK_DLL_MFUN(ui_style_get_hover_stationary_delay)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->HoverStationaryDelay;
}

CK_DLL_MFUN(ui_style_get_hover_delay_short)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->HoverDelayShort;
}

CK_DLL_MFUN(ui_style_get_hover_delay_normal)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_float = style->HoverDelayNormal;
}

CK_DLL_MFUN(ui_style_get_hover_flags_for_tooltip_mouse)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_int = style->HoverFlagsForTooltipMouse;
}

CK_DLL_MFUN(ui_style_get_hover_flags_for_tooltip_nav)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    RETURN->v_int = style->HoverFlagsForTooltipNav;
}

CK_DLL_MFUN(ui_style_set_alpha)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->Alpha = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_disabled_alpha)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->DisabledAlpha = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_window_padding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    t_CKVEC2 v             = GET_NEXT_VEC2(ARGS);
    style->WindowPadding.x = v.x;
    style->WindowPadding.y = v.y;
}

CK_DLL_MFUN(ui_style_set_window_rounding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->WindowRounding = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_window_border_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->WindowBorderSize = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_window_min_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    t_CKVEC2 v             = GET_NEXT_VEC2(ARGS);
    style->WindowMinSize.x = v.x;
    style->WindowMinSize.y = v.y;
}

CK_DLL_MFUN(ui_style_set_window_title_align)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    t_CKVEC2 v                = GET_NEXT_VEC2(ARGS);
    style->WindowTitleAlign.x = v.x;
    style->WindowTitleAlign.y = v.y;
}

CK_DLL_MFUN(ui_style_set_window_menu_button_position)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->WindowMenuButtonPosition = GET_NEXT_INT(ARGS);
}

CK_DLL_MFUN(ui_style_set_child_rounding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->ChildRounding = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_child_border_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->ChildBorderSize = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_popup_rounding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->PopupRounding = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_popup_border_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->PopupBorderSize = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_frame_padding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    t_CKVEC2 v            = GET_NEXT_VEC2(ARGS);
    style->FramePadding.x = v.x;
    style->FramePadding.y = v.y;
}

CK_DLL_MFUN(ui_style_set_frame_rounding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->FrameRounding = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_frame_border_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->FrameBorderSize = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_item_spacing)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    t_CKVEC2 v           = GET_NEXT_VEC2(ARGS);
    style->ItemSpacing.x = v.x;
    style->ItemSpacing.y = v.y;
}

CK_DLL_MFUN(ui_style_set_item_inner_spacing)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    t_CKVEC2 v                = GET_NEXT_VEC2(ARGS);
    style->ItemInnerSpacing.x = v.x;
    style->ItemInnerSpacing.y = v.y;
}

CK_DLL_MFUN(ui_style_set_cell_padding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    t_CKVEC2 v           = GET_NEXT_VEC2(ARGS);
    style->CellPadding.x = v.x;
    style->CellPadding.y = v.y;
}

CK_DLL_MFUN(ui_style_set_touch_extra_padding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    t_CKVEC2 v                 = GET_NEXT_VEC2(ARGS);
    style->TouchExtraPadding.x = v.x;
    style->TouchExtraPadding.y = v.y;
}

CK_DLL_MFUN(ui_style_set_indent_spacing)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->IndentSpacing = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_columns_min_spacing)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->ColumnsMinSpacing = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_scrollbar_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->ScrollbarSize = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_scrollbar_rounding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->ScrollbarRounding = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_grab_min_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->GrabMinSize = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_grab_rounding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->GrabRounding = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_log_slider_deadzone)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->LogSliderDeadzone = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_tab_rounding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->TabRounding = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_tab_border_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->TabBorderSize = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_tab_min_width_for_close_button)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->TabMinWidthForCloseButton = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_tab_bar_border_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->TabBarBorderSize = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_table_angled_headers_angle)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->TableAngledHeadersAngle = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_table_angled_headers_text_align)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    t_CKVEC2 v                           = GET_NEXT_VEC2(ARGS);
    style->TableAngledHeadersTextAlign.x = v.x;
    style->TableAngledHeadersTextAlign.y = v.y;
}

CK_DLL_MFUN(ui_style_set_color_button_position)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->ColorButtonPosition = GET_NEXT_INT(ARGS);
}

CK_DLL_MFUN(ui_style_set_button_text_align)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    t_CKVEC2 v               = GET_NEXT_VEC2(ARGS);
    style->ButtonTextAlign.x = v.x;
    style->ButtonTextAlign.y = v.y;
}

CK_DLL_MFUN(ui_style_set_selectable_text_align)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    t_CKVEC2 v                   = GET_NEXT_VEC2(ARGS);
    style->SelectableTextAlign.x = v.x;
    style->SelectableTextAlign.y = v.y;
}

CK_DLL_MFUN(ui_style_set_separator_text_border_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->SeparatorTextBorderSize = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_separator_text_align)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    t_CKVEC2 v                  = GET_NEXT_VEC2(ARGS);
    style->SeparatorTextAlign.x = v.x;
    style->SeparatorTextAlign.y = v.y;
}

CK_DLL_MFUN(ui_style_set_separator_text_padding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    t_CKVEC2 v                    = GET_NEXT_VEC2(ARGS);
    style->SeparatorTextPadding.x = v.x;
    style->SeparatorTextPadding.y = v.y;
}

CK_DLL_MFUN(ui_style_set_display_window_padding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    t_CKVEC2 v                    = GET_NEXT_VEC2(ARGS);
    style->DisplayWindowPadding.x = v.x;
    style->DisplayWindowPadding.y = v.y;
}

CK_DLL_MFUN(ui_style_set_display_safe_area_padding)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    t_CKVEC2 v                      = GET_NEXT_VEC2(ARGS);
    style->DisplaySafeAreaPadding.x = v.x;
    style->DisplaySafeAreaPadding.y = v.y;
}

CK_DLL_MFUN(ui_style_set_docking_separator_size)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->DockingSeparatorSize = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_mouse_cursor_scale)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->MouseCursorScale = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_anti_aliased_lines)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->AntiAliasedLines = GET_NEXT_INT(ARGS);
}

CK_DLL_MFUN(ui_style_set_anti_aliased_lines_use_tex)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->AntiAliasedLinesUseTex = GET_NEXT_INT(ARGS);
}

CK_DLL_MFUN(ui_style_set_anti_aliased_fill)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->AntiAliasedFill = GET_NEXT_INT(ARGS);
}

CK_DLL_MFUN(ui_style_set_curve_tessellation_tol)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->CurveTessellationTol = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_circle_tessellation_max_error)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->CircleTessellationMaxError = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_color)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    int idx            = GET_NEXT_INT(ARGS);
    t_CKVEC4 v         = GET_NEXT_VEC4(ARGS);
    style->Colors[idx] = { (float)v.x, (float)v.y, (float)v.z, (float)v.w };
}

CK_DLL_MFUN(ui_style_set_hover_stationary_delay)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->HoverStationaryDelay = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_hover_delay_short)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->HoverDelayShort = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_hover_delay_normal)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->HoverDelayNormal = (float)GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(ui_style_set_hover_flags_for_tooltip_mouse)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->HoverFlagsForTooltipMouse = GET_NEXT_INT(ARGS);
}

CK_DLL_MFUN(ui_style_set_hover_flags_for_tooltip_nav)
{
    cimgui::ImGuiStyle* style
      = (cimgui::ImGuiStyle*)OBJ_MEMBER_UINT(SELF, ui_style_ptr_offset);
    style->HoverFlagsForTooltipNav = GET_NEXT_INT(ARGS);
}

// ============================================================================
// UI_DrawList
// ============================================================================

#define CKVEC2_TO_IMVEC2(v) { (float)v.x, (float)v.y }
#define CKVEC3_TO_IMVEC3(v) { (float)v.x, (float)v.y, (float)v.z }
#define CKVEC4_TO_IMVEC4(v) { (float)v.x, (float)v.y, (float)v.z, (float)v.w }
#define CKVEC4_TO_IMU32(v) (cimgui::ImGui_ColorConvertFloat4ToU32(CKVEC4_TO_IMVEC4(v)))

CK_DLL_SFUN(ui_DrawList_PushClipRect)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list         = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 clip_rect_min                = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 clip_rect_max                = GET_NEXT_VEC2(ARGS);
    bool intersect_with_current_clip_rect = GET_NEXT_INT(ARGS);

    cimgui::ImDrawList_PushClipRect(draw_list, CKVEC2_TO_IMVEC2(clip_rect_min),
                                    CKVEC2_TO_IMVEC2(clip_rect_max),
                                    intersect_with_current_clip_rect);
}

CK_DLL_SFUN(ui_DrawList_PushClipRectFullScreen)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    cimgui::ImDrawList_PushClipRectFullScreen(draw_list);
}

CK_DLL_SFUN(ui_DrawList_PopClipRect)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    cimgui::ImDrawList_PopClipRect(draw_list);
}

CK_DLL_SFUN(ui_DrawList_GetClipRectMin)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    cimgui::ImVec2 clip_rect_min  = cimgui::ImDrawList_GetClipRectMin(draw_list);
    RETURN->v_vec2                = { clip_rect_min.x, clip_rect_min.y };
}

CK_DLL_SFUN(ui_DrawList_GetClipRectMax)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    cimgui::ImVec2 clip_rect_max  = cimgui::ImDrawList_GetClipRectMax(draw_list);
    RETURN->v_vec2                = { clip_rect_max.x, clip_rect_max.y };
}

CK_DLL_SFUN(ui_DrawList_AddLine)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 p1                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p2                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);

    cimgui::ImDrawList_AddLine(draw_list, CKVEC2_TO_IMVEC2(p1), CKVEC2_TO_IMVEC2(p2),
                               CKVEC4_TO_IMU32(col));
}

CK_DLL_SFUN(ui_DrawList_AddLineEx)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 p1                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p2                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);
    float thickness               = GET_NEXT_FLOAT(ARGS);

    cimgui::ImDrawList_AddLineEx(draw_list, CKVEC2_TO_IMVEC2(p1), CKVEC2_TO_IMVEC2(p2),
                                 CKVEC4_TO_IMU32(col), thickness);
}

CK_DLL_SFUN(ui_DrawList_AddRect)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 p_min                = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p_max                = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);

    cimgui::ImDrawList_AddRect(draw_list, CKVEC2_TO_IMVEC2(p_min),
                               CKVEC2_TO_IMVEC2(p_max), CKVEC4_TO_IMU32(col));
}

CK_DLL_SFUN(ui_DrawList_AddRectEx)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 p_min                = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p_max                = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);
    float rounding                = GET_NEXT_FLOAT(ARGS);
    int rounding_corners_flags    = GET_NEXT_INT(ARGS);
    float thickness               = GET_NEXT_FLOAT(ARGS);

    cimgui::ImDrawList_AddRectEx(draw_list, CKVEC2_TO_IMVEC2(p_min),
                                 CKVEC2_TO_IMVEC2(p_max), CKVEC4_TO_IMU32(col),
                                 rounding, rounding_corners_flags, thickness);
}

CK_DLL_SFUN(ui_DrawList_AddRectFilled)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 p_min                = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p_max                = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);

    cimgui::ImDrawList_AddRectFilled(draw_list, CKVEC2_TO_IMVEC2(p_min),
                                     CKVEC2_TO_IMVEC2(p_max), CKVEC4_TO_IMU32(col));
}

CK_DLL_SFUN(ui_DrawList_AddRectFilledEx)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 p_min                = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p_max                = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);
    float rounding                = GET_NEXT_FLOAT(ARGS);
    int rounding_corners_flags    = GET_NEXT_INT(ARGS);

    cimgui::ImDrawList_AddRectFilledEx(draw_list, CKVEC2_TO_IMVEC2(p_min),
                                       CKVEC2_TO_IMVEC2(p_max), CKVEC4_TO_IMU32(col),
                                       rounding, rounding_corners_flags);
}

CK_DLL_SFUN(ui_DrawList_AddRectFilledMultiColor)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 p_min                = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p_max                = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col_up_left          = GET_NEXT_VEC4(ARGS);
    t_CKVEC4 col_up_right         = GET_NEXT_VEC4(ARGS);
    t_CKVEC4 col_down_right       = GET_NEXT_VEC4(ARGS);
    t_CKVEC4 col_down_left        = GET_NEXT_VEC4(ARGS);

    cimgui::ImDrawList_AddRectFilledMultiColor(
      draw_list, CKVEC2_TO_IMVEC2(p_min), CKVEC2_TO_IMVEC2(p_max),
      CKVEC4_TO_IMU32(col_up_left), CKVEC4_TO_IMU32(col_up_right),
      CKVEC4_TO_IMU32(col_down_right), CKVEC4_TO_IMU32(col_down_left));
}

CK_DLL_SFUN(ui_DrawList_AddQuad)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 p1                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p2                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p3                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p4                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);

    cimgui::ImDrawList_AddQuad(draw_list, CKVEC2_TO_IMVEC2(p1), CKVEC2_TO_IMVEC2(p2),
                               CKVEC2_TO_IMVEC2(p3), CKVEC2_TO_IMVEC2(p4),
                               CKVEC4_TO_IMU32(col));
}

CK_DLL_SFUN(ui_DrawList_AddQuadEx)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 p1                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p2                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p3                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p4                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);
    float thickness               = GET_NEXT_FLOAT(ARGS);

    cimgui::ImDrawList_AddQuadEx(draw_list, CKVEC2_TO_IMVEC2(p1), CKVEC2_TO_IMVEC2(p2),
                                 CKVEC2_TO_IMVEC2(p3), CKVEC2_TO_IMVEC2(p4),
                                 CKVEC4_TO_IMU32(col), thickness);
}

CK_DLL_SFUN(ui_DrawList_AddQuadFilled)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 p1                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p2                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p3                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p4                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);

    cimgui::ImDrawList_AddQuadFilled(draw_list, CKVEC2_TO_IMVEC2(p1),
                                     CKVEC2_TO_IMVEC2(p2), CKVEC2_TO_IMVEC2(p3),
                                     CKVEC2_TO_IMVEC2(p4), CKVEC4_TO_IMU32(col));
}

CK_DLL_SFUN(ui_DrawList_AddTriangle)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 p1                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p2                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p3                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);

    cimgui::ImDrawList_AddTriangle(draw_list, CKVEC2_TO_IMVEC2(p1),
                                   CKVEC2_TO_IMVEC2(p2), CKVEC2_TO_IMVEC2(p3),
                                   CKVEC4_TO_IMU32(col));
}

CK_DLL_SFUN(ui_DrawList_AddTriangleEx)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 p1                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p2                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p3                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);
    float thickness               = GET_NEXT_FLOAT(ARGS);

    cimgui::ImDrawList_AddTriangleEx(draw_list, CKVEC2_TO_IMVEC2(p1),
                                     CKVEC2_TO_IMVEC2(p2), CKVEC2_TO_IMVEC2(p3),
                                     CKVEC4_TO_IMU32(col), thickness);
}

CK_DLL_SFUN(ui_DrawList_AddTriangleFilled)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 p1                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p2                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p3                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);

    cimgui::ImDrawList_AddTriangleFilled(draw_list, CKVEC2_TO_IMVEC2(p1),
                                         CKVEC2_TO_IMVEC2(p2), CKVEC2_TO_IMVEC2(p3),
                                         CKVEC4_TO_IMU32(col));
}

CK_DLL_SFUN(ui_DrawList_AddCircle)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 center               = GET_NEXT_VEC2(ARGS);
    float radius                  = GET_NEXT_FLOAT(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);

    cimgui::ImDrawList_AddCircle(draw_list, CKVEC2_TO_IMVEC2(center), radius,
                                 CKVEC4_TO_IMU32(col));
}

CK_DLL_SFUN(ui_DrawList_AddCircleEx)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 center               = GET_NEXT_VEC2(ARGS);
    float radius                  = GET_NEXT_FLOAT(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);
    int num_segments              = GET_NEXT_INT(ARGS);
    float thickness               = GET_NEXT_FLOAT(ARGS);

    cimgui::ImDrawList_AddCircleEx(draw_list, CKVEC2_TO_IMVEC2(center), radius,
                                   CKVEC4_TO_IMU32(col), num_segments, thickness);
}

CK_DLL_SFUN(ui_DrawList_AddCircleFilled)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 center               = GET_NEXT_VEC2(ARGS);
    float radius                  = GET_NEXT_FLOAT(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);
    t_CKINT num_segments          = GET_NEXT_INT(ARGS);

    cimgui::ImDrawList_AddCircleFilled(draw_list, CKVEC2_TO_IMVEC2(center), radius,
                                       CKVEC4_TO_IMU32(col), num_segments);
}

CK_DLL_SFUN(ui_DrawList_AddNgon)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 center               = GET_NEXT_VEC2(ARGS);
    float radius                  = GET_NEXT_FLOAT(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);
    int num_segments              = GET_NEXT_INT(ARGS);

    cimgui::ImDrawList_AddNgon(draw_list, CKVEC2_TO_IMVEC2(center), radius,
                               CKVEC4_TO_IMU32(col), num_segments);
}

CK_DLL_SFUN(ui_DrawList_AddNgonEx)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 center               = GET_NEXT_VEC2(ARGS);
    float radius                  = GET_NEXT_FLOAT(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);
    int num_segments              = GET_NEXT_INT(ARGS);
    float thickness               = GET_NEXT_FLOAT(ARGS);

    cimgui::ImDrawList_AddNgonEx(draw_list, CKVEC2_TO_IMVEC2(center), radius,
                                 CKVEC4_TO_IMU32(col), num_segments, thickness);
}

CK_DLL_SFUN(ui_DrawList_AddNgonFilled)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 center               = GET_NEXT_VEC2(ARGS);
    float radius                  = GET_NEXT_FLOAT(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);
    int num_segments              = GET_NEXT_INT(ARGS);

    cimgui::ImDrawList_AddNgonFilled(draw_list, CKVEC2_TO_IMVEC2(center), radius,
                                     CKVEC4_TO_IMU32(col), num_segments);
}

CK_DLL_SFUN(ui_DrawList_AddEllipse)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 center               = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 radius               = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);

    cimgui::ImDrawList_AddEllipse(draw_list, CKVEC2_TO_IMVEC2(center),
                                  CKVEC2_TO_IMVEC2(radius), CKVEC4_TO_IMU32(col));
}

CK_DLL_SFUN(ui_DrawList_AddEllipseEx)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 center               = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 radius               = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);
    t_CKFLOAT rot                 = GET_NEXT_FLOAT(ARGS);
    int num_segments              = GET_NEXT_INT(ARGS);
    float thickness               = GET_NEXT_FLOAT(ARGS);

    cimgui::ImDrawList_AddEllipseEx(draw_list, CKVEC2_TO_IMVEC2(center),
                                    CKVEC2_TO_IMVEC2(radius), CKVEC4_TO_IMU32(col), rot,
                                    num_segments, thickness);
}

CK_DLL_SFUN(ui_DrawList_AddEllipseFilled)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 center               = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 radius               = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);

    cimgui::ImDrawList_AddEllipseFilled(draw_list, CKVEC2_TO_IMVEC2(center),
                                        CKVEC2_TO_IMVEC2(radius), CKVEC4_TO_IMU32(col));
}

CK_DLL_SFUN(ui_DrawList_AddEllipseFilledEx)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 center               = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 radius               = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);
    t_CKFLOAT rot                 = GET_NEXT_FLOAT(ARGS);
    int num_segments              = GET_NEXT_INT(ARGS);

    cimgui::ImDrawList_AddEllipseFilledEx(draw_list, CKVEC2_TO_IMVEC2(center),
                                          CKVEC2_TO_IMVEC2(radius),
                                          CKVEC4_TO_IMU32(col), rot, num_segments);
}

CK_DLL_SFUN(ui_DrawList_AddText)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 pos                  = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);
    const char* text              = API->object->str(GET_NEXT_STRING(ARGS));

    cimgui::ImDrawList_AddText(draw_list, CKVEC2_TO_IMVEC2(pos), CKVEC4_TO_IMU32(col),
                               text);
}

CK_DLL_SFUN(ui_DrawList_AddTextEx)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 pos                  = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);
    const char* text              = API->object->str(GET_NEXT_STRING(ARGS));
    const char* text_end          = API->object->str(GET_NEXT_STRING(ARGS));

    cimgui::ImDrawList_AddTextEx(draw_list, CKVEC2_TO_IMVEC2(pos), CKVEC4_TO_IMU32(col),
                                 text, text_end);
}

// CK_DLL_SFUN(ui_DrawList_AddTextImFontPtr)
// {
// }

// CK_DLL_SFUN(ui_DrawList_AddTextImFontPtrEx)
// {
// }

CK_DLL_SFUN(ui_DrawList_AddBezierCubic)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 p1                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p2                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p3                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p4                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);
    float thickness               = GET_NEXT_FLOAT(ARGS);
    int num_segments              = GET_NEXT_INT(ARGS);

    cimgui::ImDrawList_AddBezierCubic(
      draw_list, CKVEC2_TO_IMVEC2(p1), CKVEC2_TO_IMVEC2(p2), CKVEC2_TO_IMVEC2(p3),
      CKVEC2_TO_IMVEC2(p4), CKVEC4_TO_IMU32(col), thickness, num_segments);
}

CK_DLL_SFUN(ui_DrawList_AddBezierQuadratic)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 p1                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p2                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p3                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);
    float thickness               = GET_NEXT_FLOAT(ARGS);
    int num_segments              = GET_NEXT_INT(ARGS);

    cimgui::ImDrawList_AddBezierQuadratic(
      draw_list, CKVEC2_TO_IMVEC2(p1), CKVEC2_TO_IMVEC2(p2), CKVEC2_TO_IMVEC2(p3),
      CKVEC4_TO_IMU32(col), thickness, num_segments);
}

CK_DLL_SFUN(ui_DrawList_AddPolyline)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    Chuck_ArrayVec2* points       = (Chuck_ArrayVec2*)GET_NEXT_OBJECT(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);
    int flags                     = GET_NEXT_INT(ARGS);
    float thickness               = GET_NEXT_FLOAT(ARGS);

    int num_items = API->object->array_vec2_size(points);

    cimgui::ImVec2* items
      = ARENA_PUSH_COUNT(&audio_frame_arena, cimgui::ImVec2, num_items);

    for (int i = 0; i < num_items; i++) {
        items[i] = CKVEC2_TO_IMVEC2(API->object->array_vec2_get_idx(points, i));
    }

    cimgui::ImDrawList_AddPolyline(draw_list, items, num_items, CKVEC4_TO_IMU32(col),
                                   flags, thickness);
}

CK_DLL_SFUN(ui_DrawList_AddConvexPolyFilled)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    Chuck_ArrayVec2* points       = (Chuck_ArrayVec2*)GET_NEXT_OBJECT(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);

    int num_items = API->object->array_vec2_size(points);

    cimgui::ImVec2* items
      = ARENA_PUSH_COUNT(&audio_frame_arena, cimgui::ImVec2, num_items);

    for (int i = 0; i < num_items; i++) {
        items[i] = CKVEC2_TO_IMVEC2(API->object->array_vec2_get_idx(points, i));
    }

    cimgui::ImDrawList_AddConvexPolyFilled(draw_list, items, num_items,
                                           CKVEC4_TO_IMU32(col));
}

CK_DLL_SFUN(ui_DrawList_AddConcavePolyFilled)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    Chuck_ArrayVec2* points       = (Chuck_ArrayVec2*)GET_NEXT_OBJECT(ARGS);
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);

    int num_items = API->object->array_vec2_size(points);

    cimgui::ImVec2* items
      = ARENA_PUSH_COUNT(&audio_frame_arena, cimgui::ImVec2, num_items);

    for (int i = 0; i < num_items; i++) {
        items[i] = CKVEC2_TO_IMVEC2(API->object->array_vec2_get_idx(points, i));
    }

    cimgui::ImDrawList_AddConcavePolyFilled(draw_list, items, num_items,
                                            CKVEC4_TO_IMU32(col));
}

CK_DLL_SFUN(ui_DrawList_PathClear)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    cimgui::ImDrawList_PathClear(draw_list);
}

CK_DLL_SFUN(ui_DrawList_PathLineTo)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 pos                  = GET_NEXT_VEC2(ARGS);

    cimgui::ImDrawList_PathLineTo(draw_list, CKVEC2_TO_IMVEC2(pos));
}

CK_DLL_SFUN(ui_DrawList_PathLineToMergeDuplicate)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 pos                  = GET_NEXT_VEC2(ARGS);

    cimgui::ImDrawList_PathLineToMergeDuplicate(draw_list, CKVEC2_TO_IMVEC2(pos));
}

CK_DLL_SFUN(ui_DrawList_PathFillConvex)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);

    cimgui::ImDrawList_PathFillConvex(draw_list, CKVEC4_TO_IMU32(col));
}

CK_DLL_SFUN(ui_DrawList_PathFillConcave)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);

    cimgui::ImDrawList_PathFillConcave(draw_list, CKVEC4_TO_IMU32(col));
}

CK_DLL_SFUN(ui_DrawList_PathStroke)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC4 col                  = GET_NEXT_VEC4(ARGS);
    int flags                     = GET_NEXT_INT(ARGS);
    float thickness               = GET_NEXT_FLOAT(ARGS);

    cimgui::ImDrawList_PathStroke(draw_list, CKVEC4_TO_IMU32(col), flags, thickness);
}

CK_DLL_SFUN(ui_DrawList_PathArcTo)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 center               = GET_NEXT_VEC2(ARGS);
    float radius                  = GET_NEXT_FLOAT(ARGS);
    float a_min                   = GET_NEXT_FLOAT(ARGS);
    float a_max                   = GET_NEXT_FLOAT(ARGS);
    t_CKINT num_segments          = GET_NEXT_INT(ARGS);

    cimgui::ImDrawList_PathArcTo(draw_list, CKVEC2_TO_IMVEC2(center), radius, a_min,
                                 a_max, num_segments);
}

CK_DLL_SFUN(ui_DrawList_PathArcToFast)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 center               = GET_NEXT_VEC2(ARGS);
    float radius                  = GET_NEXT_FLOAT(ARGS);
    t_CKINT a_min                 = GET_NEXT_INT(ARGS);
    t_CKINT a_max                 = GET_NEXT_INT(ARGS);

    cimgui::ImDrawList_PathArcToFast(draw_list, CKVEC2_TO_IMVEC2(center), radius, a_min,
                                     a_max);
}

CK_DLL_SFUN(ui_DrawList_PathEllipticalArcTo)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 center               = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 radius               = GET_NEXT_VEC2(ARGS);
    float angle                   = GET_NEXT_FLOAT(ARGS);
    float a_min                   = GET_NEXT_FLOAT(ARGS);
    float a_max                   = GET_NEXT_FLOAT(ARGS);

    cimgui::ImDrawList_PathEllipticalArcTo(draw_list, CKVEC2_TO_IMVEC2(center),
                                           CKVEC2_TO_IMVEC2(radius), angle, a_min,
                                           a_max);
}

CK_DLL_SFUN(ui_DrawList_PathEllipticalArcToEx)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 center               = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 radius               = GET_NEXT_VEC2(ARGS);
    float angle                   = GET_NEXT_FLOAT(ARGS);
    float a_min                   = GET_NEXT_FLOAT(ARGS);
    float a_max                   = GET_NEXT_FLOAT(ARGS);
    int num_segments              = GET_NEXT_INT(ARGS);

    cimgui::ImDrawList_PathEllipticalArcToEx(draw_list, CKVEC2_TO_IMVEC2(center),
                                             CKVEC2_TO_IMVEC2(radius), angle, a_min,
                                             a_max, num_segments);
}

CK_DLL_SFUN(ui_DrawList_PathBezierCubicCurveTo)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 p2                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p3                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p4                   = GET_NEXT_VEC2(ARGS);
    int num_segments              = GET_NEXT_INT(ARGS);

    cimgui::ImDrawList_PathBezierCubicCurveTo(draw_list, CKVEC2_TO_IMVEC2(p2),
                                              CKVEC2_TO_IMVEC2(p3),
                                              CKVEC2_TO_IMVEC2(p4), num_segments);
}

CK_DLL_SFUN(ui_DrawList_PathBezierQuadraticCurveTo)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 p2                   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 p3                   = GET_NEXT_VEC2(ARGS);
    int num_segments              = GET_NEXT_INT(ARGS);

    cimgui::ImDrawList_PathBezierQuadraticCurveTo(draw_list, CKVEC2_TO_IMVEC2(p2),
                                                  CKVEC2_TO_IMVEC2(p3), num_segments);
}

CK_DLL_SFUN(ui_DrawList_PathRect)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImDrawList* draw_list = cimgui::ImGui_GetWindowDrawList();
    t_CKVEC2 rect_min             = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 rect_max             = GET_NEXT_VEC2(ARGS);
    float rounding                = GET_NEXT_FLOAT(ARGS);
    int rounding_corners_flags    = GET_NEXT_INT(ARGS);

    cimgui::ImDrawList_PathRect(draw_list, CKVEC2_TO_IMVEC2(rect_min),
                                CKVEC2_TO_IMVEC2(rect_max), rounding,
                                rounding_corners_flags);
}

// ============================================================================
// Windows
// ============================================================================

CK_DLL_SFUN(ui_begin)
{
    if (!verifyInitialization(SHRED)) return;
    const char* name      = API->object->str(GET_NEXT_STRING(ARGS));
    const char* safe_name = name;
    if (!name || strlen(name) == 0) {
        safe_name = "##empty";
    }

    bool* p_open = CHUGL_UI_VAL_PTR(bool, GET_NEXT_OBJECT(ARGS), ui_bool_val_offset);

    int flags = GET_NEXT_INT(ARGS);

    bool ret      = cimgui::ImGui_Begin(safe_name, p_open, flags);
    RETURN->v_int = ret;
}

CK_DLL_SFUN(ui_begin_no_options)
{
    if (!verifyInitialization(SHRED)) return;

    const char* name      = API->object->str(GET_NEXT_STRING(ARGS));
    const char* safe_name = name;
    if (!name || strlen(name) == 0) {
        safe_name = "##empty";
    }
    RETURN->v_int = (t_CKINT)cimgui::ImGui_Begin(safe_name, NULL, 0);
}

CK_DLL_SFUN(ui_end)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_End();
}

// ============================================================================
// Child Windows
// ============================================================================
CK_DLL_SFUN(ui_BeginChild)
{
    if (!verifyInitialization(SHRED)) return;

    const char* str_id = API->object->str(GET_NEXT_STRING(ARGS));
    t_CKVEC2 size      = GET_NEXT_VEC2(ARGS);
    int child_flags    = GET_NEXT_INT(ARGS);
    int window_flags   = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_BeginChild(str_id, { (float)size.x, (float)size.y },
                                             child_flags, window_flags);
}

CK_DLL_SFUN(ui_EndChild)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_EndChild();
}

// ============================================================================
// Windows Utilities
// ============================================================================

CK_DLL_SFUN(ui_IsWindowAppearing)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsWindowAppearing();
}

CK_DLL_SFUN(ui_IsWindowCollapsed)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsWindowCollapsed();
}

CK_DLL_SFUN(ui_IsWindowFocused)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsWindowFocused(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(ui_IsWindowHovered)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsWindowHovered(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(ui_GetWindowDpiScale)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_float = cimgui::ImGui_GetWindowDpiScale();
}

CK_DLL_SFUN(ui_GetWindowPos)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImVec2 pos = cimgui::ImGui_GetWindowPos();
    RETURN->v_vec2     = { pos.x, pos.y };
}

CK_DLL_SFUN(ui_GetWindowSize)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImVec2 size = cimgui::ImGui_GetWindowSize();
    RETURN->v_vec2      = { size.x, size.y };
}

CK_DLL_SFUN(ui_GetWindowWidth)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_float = cimgui::ImGui_GetWindowWidth();
}

CK_DLL_SFUN(ui_GetWindowHeight)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_float = cimgui::ImGui_GetWindowHeight();
}

CK_DLL_SFUN(ui_GetWindowViewport)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGuiViewport* vp = cimgui::ImGui_GetWindowViewport();
    if (vp) {
        Chuck_Object* ui_vp = chugin_createCkObj("UI_Viewport", false, SHRED);
        OBJ_MEMBER_UINT(ui_vp, ui_viewport_ptr_offset) = (t_CKUINT)vp;
        RETURN->v_object                               = ui_vp;
    } else {
        RETURN->v_object = NULL;
    }
}

// ============================
// Window Manipulation
// ============================
CK_DLL_SFUN(ui_SetNextWindowPos)
{
    if (!verifyInitialization(SHRED)) return;
    t_CKVEC2 pos = GET_NEXT_VEC2(ARGS);
    t_CKINT cond = GET_NEXT_INT(ARGS);
    cimgui::ImGui_SetNextWindowPos({ (float)pos.x, (float)pos.y }, cond);
}

CK_DLL_SFUN(ui_SetNextWindowPosEx)
{
    if (!verifyInitialization(SHRED)) return;
    t_CKVEC2 pos   = GET_NEXT_VEC2(ARGS);
    t_CKINT cond   = GET_NEXT_INT(ARGS);
    t_CKVEC2 pivot = GET_NEXT_VEC2(ARGS);
    cimgui::ImGui_SetNextWindowPosEx({ (float)pos.x, (float)pos.y }, cond,
                                     { (float)pivot.x, (float)pivot.y });
}

CK_DLL_SFUN(ui_SetNextWindowSize)
{
    if (!verifyInitialization(SHRED)) return;
    t_CKVEC2 size = GET_NEXT_VEC2(ARGS);
    t_CKINT cond  = GET_NEXT_INT(ARGS);
    cimgui::ImGui_SetNextWindowSize({ (float)size.x, (float)size.y }, cond);
}

static void uiSizeCallbackHandler(cimgui::ImGuiSizeCallbackData* data)
{
    static Chuck_VM* VM  = g_chuglVM;
    static CK_DL_API API = g_chuglAPI;

    Chuck_Object* ui_size_callback_ckobj = (Chuck_Object*)data->UserData;
    // must use shred associated with UI_SizeCallback instance
    Chuck_VM_Shred* origin_shred = chugin_getOriginShred(ui_size_callback_ckobj);

    Chuck_Object* size_callback_data = chugin_createCkObj("UI_SizeCallbackData", false);
    OBJ_MEMBER_INT(size_callback_data, ui_size_callback_data_ptr_offset)
      = (t_CKINT)data;

    // put callback data into an argument
    Chuck_DL_Arg theArg;
    theArg.kind           = kindof_INT;
    theArg.value.v_object = size_callback_data;

    // invoke the update function in immediate mode
    API->vm->invoke_mfun_immediate_mode(
      ui_size_callback_ckobj, ui_size_callback_vt_offset, VM, origin_shred, &theArg, 1);
}

CK_DLL_SFUN(ui_SetNextWindowSizeConstraints)
{
    if (!verifyInitialization(SHRED)) return;
    t_CKVEC2 size_min              = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 size_max              = GET_NEXT_VEC2(ARGS);
    Chuck_Object* ui_size_callback = GET_NEXT_OBJECT(ARGS);

    // anonymous callback function
    cimgui::ImGui_SetNextWindowSizeConstraints({ (float)size_min.x, (float)size_min.y },
                                               { (float)size_max.x, (float)size_max.y },
                                               uiSizeCallbackHandler, ui_size_callback);
}

CK_DLL_SFUN(ui_SetNextWindowContentSize)
{
    if (!verifyInitialization(SHRED)) return;
    t_CKVEC2 size = GET_NEXT_VEC2(ARGS);
    cimgui::ImGui_SetNextWindowContentSize({ (float)size.x, (float)size.y });
}

CK_DLL_SFUN(ui_SetNextWindowCollapsed)
{
    if (!verifyInitialization(SHRED)) return;
    t_CKINT collapsed = GET_NEXT_INT(ARGS);
    t_CKINT cond      = GET_NEXT_INT(ARGS);
    cimgui::ImGui_SetNextWindowCollapsed(collapsed, cond);
}

CK_DLL_SFUN(ui_SetNextWindowFocus)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_SetNextWindowFocus();
}

CK_DLL_SFUN(ui_SetNextWindowScroll)
{
    if (!verifyInitialization(SHRED)) return;
    t_CKVEC2 scroll = GET_NEXT_VEC2(ARGS);
    cimgui::ImGui_SetNextWindowScroll({ (float)scroll.x, (float)scroll.y });
}

CK_DLL_SFUN(ui_SetNextWindowBgAlpha)
{
    if (!verifyInitialization(SHRED)) return;
    t_CKFLOAT alpha = GET_NEXT_FLOAT(ARGS);
    cimgui::ImGui_SetNextWindowBgAlpha(alpha);
}

// waiting for multiple viewports support in webgpu
// CK_DLL_SFUN(ui_SetNextWindowViewport)
// {
//    t_CKINT viewport_id = GET_NEXT_INT(ARGS);
//    cimgui::ImGui_SetNextWindowViewport(viewport_id);
// }

// ============================================================================
// Content region
// ============================================================================

CK_DLL_SFUN(ui_GetContentRegionAvail)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImVec2 avail = cimgui::ImGui_GetContentRegionAvail();
    RETURN->v_vec2       = { avail.x, avail.y };
}

CK_DLL_SFUN(ui_GetContentRegionMax)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImVec2 max = cimgui::ImGui_GetContentRegionMax();
    RETURN->v_vec2     = { max.x, max.y };
}
CK_DLL_SFUN(ui_GetWindowContentRegionMin)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImVec2 min = cimgui::ImGui_GetWindowContentRegionMin();
    RETURN->v_vec2     = { min.x, min.y };
}

CK_DLL_SFUN(ui_GetWindowContentRegionMax)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImVec2 max = cimgui::ImGui_GetWindowContentRegionMax();
    RETURN->v_vec2     = { max.x, max.y };
}

// ============================================================================
// Windows Scrolling
// ============================================================================

CK_DLL_SFUN(ui_GetScrollX)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_float = cimgui::ImGui_GetScrollX();
}

CK_DLL_SFUN(ui_GetScrollY)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_float = cimgui::ImGui_GetScrollY();
}

CK_DLL_SFUN(ui_SetScrollX)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_SetScrollX((float)GET_NEXT_FLOAT(ARGS));
}

CK_DLL_SFUN(ui_SetScrollY)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_SetScrollY((float)GET_NEXT_FLOAT(ARGS));
}

CK_DLL_SFUN(ui_GetScrollMaxX)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_float = cimgui::ImGui_GetScrollMaxX();
}

CK_DLL_SFUN(ui_GetScrollMaxY)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_float = cimgui::ImGui_GetScrollMaxY();
}

CK_DLL_SFUN(ui_SetScrollHereX)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_SetScrollHereX((float)GET_NEXT_FLOAT(ARGS));
}

CK_DLL_SFUN(ui_SetScrollHereY)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_SetScrollHereY((float)GET_NEXT_FLOAT(ARGS));
}

CK_DLL_SFUN(ui_SetScrollFromPosX)
{
    if (!verifyInitialization(SHRED)) return;
    float local_x        = (float)GET_NEXT_FLOAT(ARGS);
    float center_x_ratio = (float)GET_NEXT_FLOAT(ARGS);
    cimgui::ImGui_SetScrollFromPosX(local_x, center_x_ratio);
}

CK_DLL_SFUN(ui_SetScrollFromPosY)
{
    if (!verifyInitialization(SHRED)) return;
    float local_y        = (float)GET_NEXT_FLOAT(ARGS);
    float center_y_ratio = (float)GET_NEXT_FLOAT(ARGS);
    cimgui::ImGui_SetScrollFromPosY(local_y, center_y_ratio);
}

// ============================================================================
// Parameters stacks (shared)
// ============================================================================

CK_DLL_SFUN(ui_PushFont)
{
    if (!verifyInitialization(SHRED)) return;
    ImFont* font = GET_NEXT_UI_FONT(ARGS);
    ImGui::PushFont(font);
}

CK_DLL_SFUN(ui_PopFont)
{
    if (!verifyInitialization(SHRED)) return;
    ImGui::PopFont();
}

CK_DLL_SFUN(ui_PushStyleColorImVec4)
{
    if (!verifyInitialization(SHRED)) return;
    int idx        = GET_NEXT_INT(ARGS);
    t_CKVEC4 color = GET_NEXT_VEC4(ARGS);
    cimgui::ImGui_PushStyleColorImVec4(
      idx, { (float)color.x, (float)color.y, (float)color.z, (float)color.w });
}

CK_DLL_SFUN(ui_PushStyleColorImVec3)
{
    if (!verifyInitialization(SHRED)) return;
    int idx        = GET_NEXT_INT(ARGS);
    t_CKVEC3 color = GET_NEXT_VEC3(ARGS);
    cimgui::ImGui_PushStyleColorImVec4(
      idx, { (float)color.x, (float)color.y, (float)color.z, 1.0f });
}

CK_DLL_SFUN(ui_PopStyleColor)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_PopStyleColor();
}
CK_DLL_SFUN(ui_PopStyleColorEx)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_PopStyleColorEx(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(ui_PushStyleVar)
{
    if (!verifyInitialization(SHRED)) return;
    int idx   = GET_NEXT_INT(ARGS);
    float val = GET_NEXT_FLOAT(ARGS);
    cimgui::ImGui_PushStyleVar(idx, val);
}

CK_DLL_SFUN(ui_PushStyleVarImVec2)
{
    if (!verifyInitialization(SHRED)) return;
    int idx      = GET_NEXT_INT(ARGS);
    t_CKVEC2 val = GET_NEXT_VEC2(ARGS);
    cimgui::ImGui_PushStyleVarImVec2(idx, { (float)val.x, (float)val.y });
}

CK_DLL_SFUN(ui_PopStyleVar)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_PopStyleVar();
}

CK_DLL_SFUN(ui_PopStyleVarEx)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_PopStyleVarEx(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(ui_PushTabStop)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_PushTabStop(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(ui_PopTabStop)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_PopTabStop();
}

CK_DLL_SFUN(ui_PushButtonRepeat)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_PushButtonRepeat(GET_NEXT_INT(ARGS));
}
CK_DLL_SFUN(ui_PopButtonRepeat)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_PopButtonRepeat();
}

// ============================================================================
// Parameters stacks (current window)
// ============================================================================

CK_DLL_SFUN(ui_PushItemWidth)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_PushItemWidth((float)GET_NEXT_FLOAT(ARGS));
}

CK_DLL_SFUN(ui_PopItemWidth)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_PopItemWidth();
}

CK_DLL_SFUN(ui_SetNextItemWidth)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_SetNextItemWidth((float)GET_NEXT_FLOAT(ARGS));
}

CK_DLL_SFUN(ui_CalcItemWidth)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_float = cimgui::ImGui_CalcItemWidth();
}

CK_DLL_SFUN(ui_PushTextWrapPos)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_PushTextWrapPos((float)GET_NEXT_FLOAT(ARGS));
}

CK_DLL_SFUN(ui_PopTextWrapPos)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_PopTextWrapPos();
}

// ============================================================================
// Style read access
// ============================================================================
CK_DLL_SFUN(ui_GetFontSize)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_float = cimgui::ImGui_GetFontSize();
}

CK_DLL_SFUN(ui_GetFontTexUvWhitePixel)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImVec2 uv = cimgui::ImGui_GetFontTexUvWhitePixel();
    RETURN->v_vec2    = { uv.x, uv.y };
}

CK_DLL_SFUN(ui_GetColorU32)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_GetColorU32(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(ui_GetColorU32Ex)
{
    if (!verifyInitialization(SHRED)) return;
    t_CKINT idx     = GET_NEXT_INT(ARGS);
    t_CKFLOAT alpha = GET_NEXT_FLOAT(ARGS);
    RETURN->v_int   = cimgui::ImGui_GetColorU32Ex(idx, (float)alpha);
}

CK_DLL_SFUN(ui_GetColorU32ImVec4)
{
    if (!verifyInitialization(SHRED)) return;
    t_CKVEC4 col  = GET_NEXT_VEC4(ARGS);
    RETURN->v_int = cimgui::ImGui_GetColorU32ImVec4(
      { (float)col.x, (float)col.y, (float)col.z, (float)col.w });
}

CK_DLL_SFUN(ui_GetStyleColorVec4)
{
    if (!verifyInitialization(SHRED)) return;
    t_CKINT idx               = GET_NEXT_INT(ARGS);
    const cimgui::ImVec4* col = cimgui::ImGui_GetStyleColorVec4(idx);
    RETURN->v_vec4            = { col->x, col->y, col->z, col->w };
}

// ============================================================================
// Layout cursor positioning
// ============================================================================
CK_DLL_SFUN(ui_GetCursorScreenPos)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImVec2 pos = cimgui::ImGui_GetCursorScreenPos();
    RETURN->v_vec2     = { pos.x, pos.y };
}

CK_DLL_SFUN(ui_SetCursorScreenPos)
{
    if (!verifyInitialization(SHRED)) return;
    t_CKVEC2 pos = GET_NEXT_VEC2(ARGS);
    cimgui::ImGui_SetCursorScreenPos({ (float)pos.x, (float)pos.y });
}

CK_DLL_SFUN(ui_GetCursorPos)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImVec2 pos = cimgui::ImGui_GetCursorPos();
    RETURN->v_vec2     = { pos.x, pos.y };
}

CK_DLL_SFUN(ui_GetCursorPosX)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_GetCursorPosX();
}

CK_DLL_SFUN(ui_GetCursorPosY)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_GetCursorPosY();
}

CK_DLL_SFUN(ui_SetCursorPos)
{
    if (!verifyInitialization(SHRED)) return;
    t_CKVEC2 local_pos = GET_NEXT_VEC2(ARGS);
    cimgui::ImGui_SetCursorPos({ (float)local_pos.x, (float)local_pos.y });
}

CK_DLL_SFUN(ui_SetCursorPosX)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_SetCursorPosX((float)GET_NEXT_FLOAT(ARGS));
}

CK_DLL_SFUN(ui_SetCursorPosY)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_SetCursorPosY((float)GET_NEXT_FLOAT(ARGS));
}

CK_DLL_SFUN(ui_GetCursorStartPos)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImVec2 pos = cimgui::ImGui_GetCursorStartPos();
    RETURN->v_vec2     = { pos.x, pos.y };
}

// ============================================================================
// Other layout functions
// ============================================================================
CK_DLL_SFUN(ui_Separator)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_Separator();
}

CK_DLL_SFUN(ui_SameLine)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_SameLine();
}

CK_DLL_SFUN(ui_SameLineEx)
{
    if (!verifyInitialization(SHRED)) return;
    float offset_from_start_x = (float)GET_NEXT_FLOAT(ARGS);
    float spacing             = (float)GET_NEXT_FLOAT(ARGS);
    cimgui::ImGui_SameLineEx(offset_from_start_x, spacing);
}

CK_DLL_SFUN(ui_NewLine)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_NewLine();
}

CK_DLL_SFUN(ui_Spacing)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_Spacing();
}

CK_DLL_SFUN(ui_Dummy)
{
    if (!verifyInitialization(SHRED)) return;
    t_CKVEC2 size = GET_NEXT_VEC2(ARGS);
    cimgui::ImGui_Dummy({ (float)size.x, (float)size.y });
}

CK_DLL_SFUN(ui_Indent)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_Indent();
}

CK_DLL_SFUN(ui_IndentEx)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_IndentEx((float)GET_NEXT_FLOAT(ARGS));
}

CK_DLL_SFUN(ui_Unindent)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_Unindent();
}

CK_DLL_SFUN(ui_UnindentEx)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_UnindentEx((float)GET_NEXT_FLOAT(ARGS));
}

CK_DLL_SFUN(ui_BeginGroup)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_BeginGroup();
}

CK_DLL_SFUN(ui_EndGroup)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_EndGroup();
}

CK_DLL_SFUN(ui_AlignTextToFramePadding)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_AlignTextToFramePadding();
}

CK_DLL_SFUN(ui_GetTextLineHeight)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_float = cimgui::ImGui_GetTextLineHeight();
}

CK_DLL_SFUN(ui_GetTextLineHeightWithSpacing)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_float = cimgui::ImGui_GetTextLineHeightWithSpacing();
}

CK_DLL_SFUN(ui_GetFrameHeight)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_float = cimgui::ImGui_GetFrameHeight();
}

CK_DLL_SFUN(ui_GetFrameHeightWithSpacing)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_float = cimgui::ImGui_GetFrameHeightWithSpacing();
}

// ============================================================================
// ID stack/scopes
// ============================================================================

CK_DLL_SFUN(ui_PushID)
{
    if (!verifyInitialization(SHRED)) return;
    Chuck_String* ck_string = GET_NEXT_STRING(ARGS);
    cimgui::ImGui_PushID(API->object->str(ck_string));
}

CK_DLL_SFUN(ui_PushIDStr)
{
    if (!verifyInitialization(SHRED)) return;
    Chuck_String* ck_string_begin = GET_NEXT_STRING(ARGS);
    Chuck_String* ck_string_end   = GET_NEXT_STRING(ARGS);
    cimgui::ImGui_PushIDStr(API->object->str(ck_string_begin),
                            API->object->str(ck_string_end));
}

CK_DLL_SFUN(ui_PushIDInt)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_PushIDInt(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(ui_PopID)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_PopID();
}

CK_DLL_SFUN(ui_GetID)
{
    if (!verifyInitialization(SHRED)) return;
    Chuck_String* ck_string = GET_NEXT_STRING(ARGS);
    RETURN->v_int           = cimgui::ImGui_GetID(API->object->str(ck_string));
}

CK_DLL_SFUN(ui_GetIDStr)
{
    if (!verifyInitialization(SHRED)) return;
    Chuck_String* ck_string_begin = GET_NEXT_STRING(ARGS);
    Chuck_String* ck_string_end   = GET_NEXT_STRING(ARGS);
    RETURN->v_int = cimgui::ImGui_GetIDStr(API->object->str(ck_string_begin),
                                           API->object->str(ck_string_end));
}

// ============================================================================
// Widgets: Text
// ============================================================================

CK_DLL_SFUN(ui_TextUnformatted)
{
    if (!verifyInitialization(SHRED)) return;
    const char* text = API->object->str(GET_NEXT_STRING(ARGS));
    cimgui::ImGui_TextUnformatted(text);
}

CK_DLL_SFUN(ui_TextUnformattedEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* text     = API->object->str(GET_NEXT_STRING(ARGS));
    const char* text_end = API->object->str(GET_NEXT_STRING(ARGS));
    cimgui::ImGui_TextUnformattedEx(text, text_end);
}

CK_DLL_SFUN(ui_TextColoredUnformatted)
{
    if (!verifyInitialization(SHRED)) return;
    t_CKVEC4 col     = GET_NEXT_VEC4(ARGS);
    const char* text = API->object->str(GET_NEXT_STRING(ARGS));
    cimgui::ImGui_TextColoredUnformatted(
      { (float)col.x, (float)col.y, (float)col.z, (float)col.w }, text);
}

CK_DLL_SFUN(ui_TextDisabledUnformatted)
{
    if (!verifyInitialization(SHRED)) return;
    const char* text = API->object->str(GET_NEXT_STRING(ARGS));
    cimgui::ImGui_TextDisabledUnformatted(text);
}

CK_DLL_SFUN(ui_TextWrappedUnformatted)
{
    if (!verifyInitialization(SHRED)) return;
    const char* text = API->object->str(GET_NEXT_STRING(ARGS));
    cimgui::ImGui_TextWrappedUnformatted(text);
}

CK_DLL_SFUN(ui_LabelTextUnformatted)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    const char* text  = API->object->str(GET_NEXT_STRING(ARGS));
    cimgui::ImGui_LabelTextUnformatted(label, text);
}

CK_DLL_SFUN(ui_BulletTextUnformatted)
{
    if (!verifyInitialization(SHRED)) return;
    const char* text = API->object->str(GET_NEXT_STRING(ARGS));
    cimgui::ImGui_BulletTextUnformatted(text);
}

CK_DLL_SFUN(ui_SeparatorText)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    cimgui::ImGui_SeparatorText(label);
}

// ============================================================================
// Callbacks
// ============================================================================

CK_DLL_CTOR(ui_callback_ctor)
{
    // origin shred needed for API->vm->invoke_mfun_immediate_mode
    chugin_setOriginShred(SELF, SHRED);
}

CK_DLL_DTOR(ui_callback_dtor)
{
    chugin_removeFromOriginShredMap(SELF);
}

// user defined callback
CK_DLL_MFUN(ui_size_callback)
{
}

CK_DLL_MFUN(ui_size_callback_data_get_pos)
{
    cimgui::ImGuiSizeCallbackData* data
      = (cimgui::ImGuiSizeCallbackData*)OBJ_MEMBER_UINT(
        SELF, ui_size_callback_data_ptr_offset);
    RETURN->v_vec2 = { data->Pos.x, data->Pos.y };
}

CK_DLL_MFUN(ui_size_callback_data_get_current_size)
{
    cimgui::ImGuiSizeCallbackData* data
      = (cimgui::ImGuiSizeCallbackData*)OBJ_MEMBER_UINT(
        SELF, ui_size_callback_data_ptr_offset);
    RETURN->v_vec2 = { data->CurrentSize.x, data->CurrentSize.y };
}

CK_DLL_MFUN(ui_size_callback_data_get_desired_size)
{
    cimgui::ImGuiSizeCallbackData* data
      = (cimgui::ImGuiSizeCallbackData*)OBJ_MEMBER_UINT(
        SELF, ui_size_callback_data_ptr_offset);
    RETURN->v_vec2 = { data->DesiredSize.x, data->DesiredSize.y };
}

CK_DLL_MFUN(ui_size_callback_data_set_desired_size)
{
    cimgui::ImGuiSizeCallbackData* data
      = (cimgui::ImGuiSizeCallbackData*)OBJ_MEMBER_UINT(
        SELF, ui_size_callback_data_ptr_offset);
    t_CKVEC2 desired_size = GET_NEXT_VEC2(ARGS);
    data->DesiredSize     = { (float)desired_size.x, (float)desired_size.y };
}

// user defined callback
CK_DLL_MFUN(ui_combo_callback)
{
}

// ============================================================================
// Widgets: Main
// ============================================================================

CK_DLL_SFUN(ui_Button)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    RETURN->v_int     = cimgui::ImGui_Button(label);
}

CK_DLL_SFUN(ui_ButtonEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    t_CKVEC2 size     = GET_NEXT_VEC2(ARGS);
    RETURN->v_int     = cimgui::ImGui_ButtonEx(label, { (float)size.x, (float)size.y });
}

CK_DLL_SFUN(ui_SmallButton)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    RETURN->v_int     = cimgui::ImGui_SmallButton(label);
}

CK_DLL_SFUN(ui_InvisibleButton)
{
    if (!verifyInitialization(SHRED)) return;
    const char* str_id = API->object->str(GET_NEXT_STRING(ARGS));
    t_CKVEC2 size      = GET_NEXT_VEC2(ARGS);
    int flags          = GET_NEXT_INT(ARGS);
    RETURN->v_int
      = cimgui::ImGui_InvisibleButton(str_id, { (float)size.x, (float)size.y }, flags);
}

CK_DLL_SFUN(ui_ArrowButton)
{
    if (!verifyInitialization(SHRED)) return;
    const char* str_id = API->object->str(GET_NEXT_STRING(ARGS));
    int direction      = GET_NEXT_INT(ARGS);
    RETURN->v_int      = cimgui::ImGui_ArrowButton(str_id, direction);
}

CK_DLL_SFUN(ui_Checkbox)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    bool* b = CHUGL_UI_VAL_PTR(bool, GET_NEXT_OBJECT(ARGS), ui_bool_val_offset);

    RETURN->v_int = cimgui::ImGui_Checkbox(label, b);
}

CK_DLL_SFUN(ui_CheckboxFlagsIntPtr)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    int* flags        = (int*)OBJ_MEMBER_UINT(obj, ui_int_ptr_offset);

    int flags_value = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_CheckboxFlagsIntPtr(label, flags, flags_value);
}

CK_DLL_SFUN(ui_RadioButton)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    int active        = GET_NEXT_INT(ARGS);
    RETURN->v_int     = cimgui::ImGui_RadioButton(label, active);
}

CK_DLL_SFUN(ui_RadioButtonIntPtr)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    int* v            = (int*)OBJ_MEMBER_UINT(obj, ui_int_ptr_offset);

    int v_button = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_RadioButtonIntPtr(label, v, v_button);
}

CK_DLL_SFUN(ui_ProgressBar)
{
    if (!verifyInitialization(SHRED)) return;
    float fraction      = GET_NEXT_FLOAT(ARGS);
    t_CKVEC2 size       = GET_NEXT_VEC2(ARGS);
    const char* overlay = API->object->str(GET_NEXT_STRING(ARGS)); // NULL = no overlay
    cimgui::ImGui_ProgressBar(fraction, { (float)size.x, (float)size.y }, overlay);
}

CK_DLL_SFUN(ui_Bullet)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_Bullet();
}

// ============================================================================
// Widgets: Image
// ============================================================================
CK_DLL_SFUN(ui_Image)
{
    if (!verifyInitialization(SHRED)) return;
    ImTextureID id
      = (ImTextureID)OBJ_MEMBER_UINT(GET_NEXT_OBJECT(ARGS), component_offset_id);
    t_CKVEC2 size = GET_NEXT_VEC2(ARGS);
    ImGui::Image(id, { (float)size.x, (float)size.y });
}

CK_DLL_SFUN(ui_ImageEx)
{
    if (!verifyInitialization(SHRED)) return;
    ImTextureID id
      = (ImTextureID)OBJ_MEMBER_UINT(GET_NEXT_OBJECT(ARGS), component_offset_id);
    t_CKVEC2 size   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 uv0    = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 uv1    = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 tint   = GET_NEXT_VEC4(ARGS);
    t_CKVEC4 border = GET_NEXT_VEC4(ARGS);
    ImGui::Image(
      id, { (float)size.x, (float)size.y }, { (float)uv0.x, (float)uv0.y },
      { (float)uv1.x, (float)uv1.y },
      { (float)tint.x, (float)tint.y, (float)tint.z, (float)tint.w },
      { (float)border.x, (float)border.y, (float)border.z, (float)border.w });
}

CK_DLL_SFUN(ui_ImageButton)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    ImTextureID id
      = (ImTextureID)OBJ_MEMBER_UINT(GET_NEXT_OBJECT(ARGS), component_offset_id);
    t_CKVEC2 size = GET_NEXT_VEC2(ARGS);
    RETURN->v_int = ImGui::ImageButton(label, id, { (float)size.x, (float)size.y });
}

CK_DLL_SFUN(ui_ImageButtonEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    ImTextureID id
      = (ImTextureID)OBJ_MEMBER_UINT(GET_NEXT_OBJECT(ARGS), component_offset_id);
    t_CKVEC2 size   = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 uv0    = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 uv1    = GET_NEXT_VEC2(ARGS);
    t_CKVEC4 tint   = GET_NEXT_VEC4(ARGS);
    t_CKVEC4 border = GET_NEXT_VEC4(ARGS);
    RETURN->v_int   = ImGui::ImageButton(
      label, id, { (float)size.x, (float)size.y }, { (float)uv0.x, (float)uv0.y },
      { (float)uv1.x, (float)uv1.y },
      { (float)border.x, (float)border.y, (float)border.z, (float)border.w },
      { (float)tint.x, (float)tint.y, (float)tint.z, (float)tint.w });
}

// ============================================================================
// Widgets: Combo Box (Dropdown)
// ============================================================================
CK_DLL_SFUN(ui_BeginCombo)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label         = API->object->str(GET_NEXT_STRING(ARGS));
    const char* preview_value = API->object->str(GET_NEXT_STRING(ARGS));
    int flags                 = GET_NEXT_INT(ARGS);
    RETURN->v_int             = cimgui::ImGui_BeginCombo(label, preview_value, flags);
}

CK_DLL_SFUN(ui_EndCombo)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_EndCombo();
}

CK_DLL_SFUN(ui_ComboChar)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    int* current_item = (int*)OBJ_MEMBER_UINT(obj, ui_int_ptr_offset);

    Chuck_ArrayInt* ck_string_array = (Chuck_ArrayInt*)GET_NEXT_OBJECT(ARGS);
    int num_items                   = API->object->array_int_size(ck_string_array);

    // copy array of chuck_string to array of const char*
    const char** items = ARENA_PUSH_COUNT(&audio_frame_arena, const char*, num_items);

    for (int i = 0; i < num_items; i++) {
        items[i] = API->object->str(
          (Chuck_String*)API->object->array_int_get_idx(ck_string_array, i));
    }

    RETURN->v_int = cimgui::ImGui_ComboChar(label, current_item, items, num_items);
}

CK_DLL_SFUN(ui_ComboCharEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    int* current_item = (int*)OBJ_MEMBER_UINT(obj, ui_int_ptr_offset);

    Chuck_ArrayInt* ck_string_array = (Chuck_ArrayInt*)GET_NEXT_OBJECT(ARGS);
    int num_items                   = API->object->array_int_size(ck_string_array);

    int popup_max_height_in_items = GET_NEXT_INT(ARGS);

    // copy array of chuck_string to array of const char*
    const char** items = ARENA_PUSH_COUNT(&audio_frame_arena, const char*, num_items);

    for (int i = 0; i < num_items; i++) {
        items[i] = API->object->str(
          (Chuck_String*)API->object->array_int_get_idx(ck_string_array, i));
    }

    RETURN->v_int = cimgui::ImGui_ComboCharEx(label, current_item, items, num_items,
                                              popup_max_height_in_items);
}

CK_DLL_SFUN(ui_Combo)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    int* current_item = (int*)OBJ_MEMBER_UINT(obj, ui_int_ptr_offset);

    const char* items_separated_by_zeros = API->object->str(GET_NEXT_STRING(ARGS));
    printf("items_separated_by_zeros: %s\n", items_separated_by_zeros);

    RETURN->v_int = cimgui::ImGui_Combo(label, current_item, items_separated_by_zeros);
}

CK_DLL_SFUN(ui_ComboEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    int* current_item = (int*)OBJ_MEMBER_UINT(obj, ui_int_ptr_offset);

    const char* items_separated_by_zeros = API->object->str(GET_NEXT_STRING(ARGS));

    int popup_max_height_in_items = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_ComboEx(label, current_item, items_separated_by_zeros,
                                          popup_max_height_in_items);
}

// ============================================================================
// Widgets: Drag Sliders
// ============================================================================

#define UI_DRAG_EX_IMPL_FLOAT(func, type, ptr_offset)                                  \
    {                                                                                  \
        const char* label  = API->object->str(GET_NEXT_STRING(ARGS));                  \
        Chuck_Object* obj  = GET_NEXT_OBJECT(ARGS);                                    \
        type* v            = (type*)OBJ_MEMBER_UINT(obj, ptr_offset);                  \
        float v_speed      = GET_NEXT_FLOAT(ARGS);                                     \
        float v_min        = GET_NEXT_FLOAT(ARGS);                                     \
        float v_max        = GET_NEXT_FLOAT(ARGS);                                     \
        const char* format = API->object->str(GET_NEXT_STRING(ARGS));                  \
        int flags          = GET_NEXT_INT(ARGS);                                       \
        RETURN->v_int = cimgui::func(label, v, v_speed, v_min, v_max, format, flags);  \
    }

#define UI_DRAG_EX_IMPL_INT(func, type, ptr_offset)                                    \
    {                                                                                  \
        const char* label  = API->object->str(GET_NEXT_STRING(ARGS));                  \
        Chuck_Object* obj  = GET_NEXT_OBJECT(ARGS);                                    \
        type* v            = (type*)OBJ_MEMBER_UINT(obj, ptr_offset);                  \
        float v_speed      = GET_NEXT_FLOAT(ARGS);                                     \
        int v_min          = GET_NEXT_INT(ARGS);                                       \
        int v_max          = GET_NEXT_INT(ARGS);                                       \
        const char* format = API->object->str(GET_NEXT_STRING(ARGS));                  \
        int flags          = GET_NEXT_INT(ARGS);                                       \
        RETURN->v_int = cimgui::func(label, v, v_speed, v_min, v_max, format, flags);  \
    }

#define UI_DRAG_IMPL(func, type, ptr_offset)                                           \
    {                                                                                  \
        const char* label = API->object->str(GET_NEXT_STRING(ARGS));                   \
        Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);                                     \
        type* v           = (type*)OBJ_MEMBER_UINT(obj, ptr_offset);                   \
        RETURN->v_int     = cimgui::func(label, v);                                    \
    }

CK_DLL_SFUN(ui_DragFloat)
{
    if (!verifyInitialization(SHRED)) return;
    UI_DRAG_IMPL(ImGui_DragFloat, float, ui_float_ptr_offset);
}

CK_DLL_SFUN(ui_DragFloatEx)
{
    if (!verifyInitialization(SHRED)) return;
    UI_DRAG_EX_IMPL_FLOAT(ImGui_DragFloatEx, float, ui_float_ptr_offset);
}

CK_DLL_SFUN(ui_DragFloat2)
{
    if (!verifyInitialization(SHRED)) return;
    UI_DRAG_IMPL(ImGui_DragFloat2, float, ui_float2_ptr_offset);
}

CK_DLL_SFUN(ui_DragFloat2Ex)
{
    if (!verifyInitialization(SHRED)) return;
    UI_DRAG_EX_IMPL_FLOAT(ImGui_DragFloat2Ex, float, ui_float2_ptr_offset);
}

CK_DLL_SFUN(ui_DragFloat2Speed)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    float* v          = (float*)OBJ_MEMBER_UINT(obj, ui_float2_ptr_offset);
    float v_speed     = GET_NEXT_FLOAT(ARGS);
    RETURN->v_int
      = cimgui::ImGui_DragFloat2Ex(label, v, v_speed, 0.0f, 0.0f, "%.3f", 0);
}

CK_DLL_SFUN(ui_DragFloat3)
{
    if (!verifyInitialization(SHRED)) return;
    UI_DRAG_IMPL(ImGui_DragFloat3, float, ui_float3_ptr_offset);
}

CK_DLL_SFUN(ui_DragFloat3Ex)
{
    if (!verifyInitialization(SHRED)) return;
    UI_DRAG_EX_IMPL_FLOAT(ImGui_DragFloat3Ex, float, ui_float3_ptr_offset);
}

CK_DLL_SFUN(ui_DragFloat3Speed)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    float* v          = (float*)OBJ_MEMBER_UINT(obj, ui_float3_ptr_offset);
    float v_speed     = GET_NEXT_FLOAT(ARGS);
    RETURN->v_int
      = cimgui::ImGui_DragFloat3Ex(label, v, v_speed, 0.0f, 0.0f, "%.3f", 0);
}

CK_DLL_SFUN(ui_DragFloat4)
{
    if (!verifyInitialization(SHRED)) return;
    UI_DRAG_IMPL(ImGui_DragFloat4, float, ui_float4_ptr_offset);
}

CK_DLL_SFUN(ui_DragFloat4Ex)
{
    if (!verifyInitialization(SHRED)) return;
    UI_DRAG_EX_IMPL_FLOAT(ImGui_DragFloat4Ex, float, ui_float4_ptr_offset);
}

CK_DLL_SFUN(ui_DragFloat4Speed)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    float* v          = (float*)OBJ_MEMBER_UINT(obj, ui_float4_ptr_offset);
    float v_speed     = GET_NEXT_FLOAT(ARGS);
    RETURN->v_int
      = cimgui::ImGui_DragFloat4Ex(label, v, v_speed, 0.0f, 0.0f, "%.3f", 0);
}

CK_DLL_SFUN(ui_DragFloatRange2)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label     = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* obj_min = GET_NEXT_OBJECT(ARGS);
    float* v_current_min  = (float*)OBJ_MEMBER_UINT(obj_min, ui_float2_ptr_offset);
    Chuck_Object* obj_max = GET_NEXT_OBJECT(ARGS);
    float* v_current_max  = (float*)OBJ_MEMBER_UINT(obj_max, ui_float2_ptr_offset);

    RETURN->v_int = cimgui::ImGui_DragFloatRange2(label, v_current_min, v_current_max);
}

CK_DLL_SFUN(ui_DragFloatRange2Ex)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label     = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* obj_min = GET_NEXT_OBJECT(ARGS);
    float* v_current_min  = (float*)OBJ_MEMBER_UINT(obj_min, ui_float2_ptr_offset);
    Chuck_Object* obj_max = GET_NEXT_OBJECT(ARGS);
    float* v_current_max  = (float*)OBJ_MEMBER_UINT(obj_max, ui_float2_ptr_offset);
    float v_speed         = GET_NEXT_FLOAT(ARGS);
    float v_min           = GET_NEXT_FLOAT(ARGS);
    float v_max           = GET_NEXT_FLOAT(ARGS);
    const char* format    = API->object->str(GET_NEXT_STRING(ARGS));
    const char* format_max
      = API->object->str(GET_NEXT_STRING(ARGS)); // NULL = same format as min
    float power = GET_NEXT_FLOAT(ARGS);
    RETURN->v_int
      = cimgui::ImGui_DragFloatRange2Ex(label, v_current_min, v_current_max, v_speed,
                                        v_min, v_max, format, format_max, power);
}

CK_DLL_SFUN(ui_DragInt)
{
    if (!verifyInitialization(SHRED)) return;
    UI_DRAG_IMPL(ImGui_DragInt, int, ui_int_ptr_offset);
}

CK_DLL_SFUN(ui_DragIntEx)
{
    if (!verifyInitialization(SHRED)) return;
    UI_DRAG_EX_IMPL_INT(ImGui_DragIntEx, int, ui_int_ptr_offset);
}

CK_DLL_SFUN(ui_DragInt2)
{
    if (!verifyInitialization(SHRED)) return;
    UI_DRAG_IMPL(ImGui_DragInt2, int, ui_int2_ptr_offset);
}

CK_DLL_SFUN(ui_DragInt2Ex)
{
    if (!verifyInitialization(SHRED)) return;
    UI_DRAG_EX_IMPL_INT(ImGui_DragInt2Ex, int, ui_int2_ptr_offset);
}

CK_DLL_SFUN(ui_DragInt3)
{
    if (!verifyInitialization(SHRED)) return;
    UI_DRAG_IMPL(ImGui_DragInt3, int, ui_int3_ptr_offset);
}

CK_DLL_SFUN(ui_DragInt3Ex)
{
    if (!verifyInitialization(SHRED)) return;
    UI_DRAG_EX_IMPL_INT(ImGui_DragInt3Ex, int, ui_int3_ptr_offset);
}

CK_DLL_SFUN(ui_DragInt4)
{
    if (!verifyInitialization(SHRED)) return;
    UI_DRAG_IMPL(ImGui_DragInt4, int, ui_int4_ptr_offset);
}

CK_DLL_SFUN(ui_DragInt4Ex)
{
    if (!verifyInitialization(SHRED)) return;
    UI_DRAG_EX_IMPL_INT(ImGui_DragInt4Ex, int, ui_int4_ptr_offset);
}

CK_DLL_SFUN(ui_DragIntRange2)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label     = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* obj_min = GET_NEXT_OBJECT(ARGS);
    int* v_current_min    = (int*)OBJ_MEMBER_UINT(obj_min, ui_int2_ptr_offset);
    Chuck_Object* obj_max = GET_NEXT_OBJECT(ARGS);
    int* v_current_max    = (int*)OBJ_MEMBER_UINT(obj_max, ui_int2_ptr_offset);

    RETURN->v_int = cimgui::ImGui_DragIntRange2(label, v_current_min, v_current_max);
}

CK_DLL_SFUN(ui_DragIntRange2Ex)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label     = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* obj_min = GET_NEXT_OBJECT(ARGS);
    int* v_current_min    = (int*)OBJ_MEMBER_UINT(obj_min, ui_int2_ptr_offset);
    Chuck_Object* obj_max = GET_NEXT_OBJECT(ARGS);
    int* v_current_max    = (int*)OBJ_MEMBER_UINT(obj_max, ui_int2_ptr_offset);
    float v_speed         = GET_NEXT_FLOAT(ARGS);
    int v_min             = GET_NEXT_INT(ARGS);
    int v_max             = GET_NEXT_INT(ARGS);
    const char* format    = API->object->str(GET_NEXT_STRING(ARGS));
    const char* format_max
      = API->object->str(GET_NEXT_STRING(ARGS)); // NULL = same format as min
    int flags = GET_NEXT_INT(ARGS);
    RETURN->v_int
      = cimgui::ImGui_DragIntRange2Ex(label, v_current_min, v_current_max, v_speed,
                                      v_min, v_max, format, format_max, flags);
}

CK_DLL_SFUN(ui_DragScalarN_CKINT)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label        = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_ArrayInt* ck_array = (Chuck_ArrayInt*)GET_NEXT_OBJECT(ARGS);

    int num_components = API->object->array_int_size(ck_array);

    int* v = ARENA_PUSH_COUNT(&audio_frame_arena, int, num_components);

    for (int i = 0; i < num_components; ++i) {
        v[i] = API->object->array_int_get_idx(ck_array, i);
    }

    RETURN->v_int
      = cimgui::ImGui_DragScalarN(label, cimgui::ImGuiDataType_S32, v, num_components);

    API->object->array_int_clear(ck_array);

    // copy back
    for (int i = 0; i < num_components; ++i) {
        API->object->array_int_push_back(ck_array, v[i]);
    }
}

CK_DLL_SFUN(ui_DragScalarNEx_CKINT)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label        = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_ArrayInt* ck_array = (Chuck_ArrayInt*)GET_NEXT_OBJECT(ARGS);

    int num_components = API->object->array_int_size(ck_array);

    int* v = ARENA_PUSH_COUNT(&audio_frame_arena, int, num_components);

    for (int i = 0; i < num_components; ++i) {
        v[i] = API->object->array_int_get_idx(ck_array, i);
    }

    float speed        = GET_NEXT_FLOAT(ARGS);
    int v_min          = GET_NEXT_INT(ARGS);
    int v_max          = GET_NEXT_INT(ARGS);
    const char* format = API->object->str(GET_NEXT_STRING(ARGS));
    int flags          = GET_NEXT_INT(ARGS);

    RETURN->v_int
      = cimgui::ImGui_DragScalarNEx(label, cimgui::ImGuiDataType_S32, v, num_components,
                                    speed, &v_min, &v_max, format, flags);

    // copy back
    API->object->array_int_clear(ck_array);
    for (int i = 0; i < num_components; ++i) {
        API->object->array_int_push_back(ck_array, v[i]);
    }
}

CK_DLL_SFUN(ui_DragScalarN_CKFLOAT)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label          = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_ArrayFloat* ck_array = (Chuck_ArrayFloat*)GET_NEXT_OBJECT(ARGS);

    int num_components = API->object->array_float_size(ck_array);

    float* v = ARENA_PUSH_COUNT(&audio_frame_arena, float, num_components);

    for (int i = 0; i < num_components; ++i) {
        v[i] = API->object->array_float_get_idx(ck_array, i);
    }

    RETURN->v_int = cimgui::ImGui_DragScalarN(label, cimgui::ImGuiDataType_Float, v,
                                              num_components);

    API->object->array_float_clear(ck_array);

    // copy back
    for (int i = 0; i < num_components; ++i) {
        API->object->array_float_push_back(ck_array, v[i]);
    }
}

CK_DLL_SFUN(ui_DragScalarNEx_CKFLOAT)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label          = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_ArrayFloat* ck_array = (Chuck_ArrayFloat*)GET_NEXT_OBJECT(ARGS);

    int num_components = API->object->array_float_size(ck_array);

    // copy chuck array to arena
    float* v = ARENA_PUSH_COUNT(&audio_frame_arena, float, num_components);
    for (int i = 0; i < num_components; ++i) {
        v[i] = API->object->array_float_get_idx(ck_array, i);
    }

    // get other params
    float speed        = GET_NEXT_FLOAT(ARGS);
    float v_min        = GET_NEXT_FLOAT(ARGS);
    float v_max        = GET_NEXT_FLOAT(ARGS);
    const char* format = API->object->str(GET_NEXT_STRING(ARGS));
    int flags          = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_DragScalarNEx(label, cimgui::ImGuiDataType_Float, v,
                                                num_components, speed, &v_min, &v_max,
                                                format, flags);

    // copy back to chuck array (no set in API, have to clear and re-push)
    API->object->array_float_clear(ck_array);
    for (int i = 0; i < num_components; ++i) {
        API->object->array_float_push_back(ck_array, v[i]);
    }
}

// ============================================================================
// Widgets: Slider
// ============================================================================

CK_DLL_SFUN(ui_SliderFloat)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    float* v          = (float*)OBJ_MEMBER_UINT(obj, ui_float_ptr_offset);

    float v_min = GET_NEXT_FLOAT(ARGS);
    float v_max = GET_NEXT_FLOAT(ARGS);

    RETURN->v_int = cimgui::ImGui_SliderFloat(label, v, v_min, v_max);
}

CK_DLL_SFUN(ui_SliderFloatEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    float* v          = (float*)OBJ_MEMBER_UINT(obj, ui_float_ptr_offset);

    float v_min        = GET_NEXT_FLOAT(ARGS);
    float v_max        = GET_NEXT_FLOAT(ARGS);
    const char* format = API->object->str(GET_NEXT_STRING(ARGS)); // NULL = "%.3f"
    int flags          = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_SliderFloatEx(label, v, v_min, v_max, format, flags);
}

CK_DLL_SFUN(ui_SliderAngle)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    float* v          = (float*)OBJ_MEMBER_UINT(obj, ui_float_ptr_offset);

    RETURN->v_int = cimgui::ImGui_SliderAngle(label, v);
}

CK_DLL_SFUN(ui_SliderAngleBounds)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj   = GET_NEXT_OBJECT(ARGS);
    float* v            = (float*)OBJ_MEMBER_UINT(obj, ui_float_ptr_offset);
    float v_degrees_min = GET_NEXT_FLOAT(ARGS);
    float v_degrees_max = GET_NEXT_FLOAT(ARGS);

    RETURN->v_int = cimgui::ImGui_SliderAngleEx(label, v, v_degrees_min, v_degrees_max,
                                                "%.0f deg", 0);
}

CK_DLL_SFUN(ui_SliderAngleEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    float* v          = (float*)OBJ_MEMBER_UINT(obj, ui_float_ptr_offset);

    float v_degrees_min = GET_NEXT_FLOAT(ARGS);
    float v_degrees_max = GET_NEXT_FLOAT(ARGS);

    const char* format = API->object->str(GET_NEXT_STRING(ARGS)); // NULL = "%.3f"

    int flags = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_SliderAngleEx(label, v, v_degrees_min, v_degrees_max,
                                                format, flags);
}

CK_DLL_SFUN(ui_SliderInt)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    int* v            = (int*)OBJ_MEMBER_UINT(obj, ui_int_ptr_offset);

    int v_min = GET_NEXT_INT(ARGS);
    int v_max = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_SliderInt(label, v, v_min, v_max);
}

CK_DLL_SFUN(ui_SliderIntEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    int* v            = (int*)OBJ_MEMBER_UINT(obj, ui_int_ptr_offset);

    int v_min          = GET_NEXT_INT(ARGS);
    int v_max          = GET_NEXT_INT(ARGS);
    const char* format = API->object->str(GET_NEXT_STRING(ARGS)); // NULL = "%.0f"
    int flags          = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_SliderIntEx(label, v, v_min, v_max, format, flags);
}

CK_DLL_SFUN(ui_SliderScalarN_CKINT)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label        = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_ArrayInt* ck_array = (Chuck_ArrayInt*)GET_NEXT_OBJECT(ARGS);

    int num_components = API->object->array_int_size(ck_array);

    int* v = ARENA_PUSH_COUNT(&audio_frame_arena, int, num_components);

    for (int i = 0; i < num_components; ++i) {
        v[i] = API->object->array_int_get_idx(ck_array, i);
    }

    int v_min = GET_NEXT_INT(ARGS);
    int v_max = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_SliderScalarN(label, cimgui::ImGuiDataType_S32, v,
                                                num_components, &v_min, &v_max);

    API->object->array_int_clear(ck_array);

    // copy back
    for (int i = 0; i < num_components; ++i) {
        API->object->array_int_push_back(ck_array, v[i]);
    }
}

CK_DLL_SFUN(ui_SliderScalarNEx_CKINT)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label        = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_ArrayInt* ck_array = (Chuck_ArrayInt*)GET_NEXT_OBJECT(ARGS);

    int num_components = API->object->array_int_size(ck_array);

    int* v = ARENA_PUSH_COUNT(&audio_frame_arena, int, num_components);

    for (int i = 0; i < num_components; ++i) {
        v[i] = API->object->array_int_get_idx(ck_array, i);
    }

    int v_min          = GET_NEXT_INT(ARGS);
    int v_max          = GET_NEXT_INT(ARGS);
    const char* format = API->object->str(GET_NEXT_STRING(ARGS)); // NULL = "%.0f"
    int flags          = GET_NEXT_INT(ARGS);

    RETURN->v_int
      = cimgui::ImGui_SliderScalarNEx(label, cimgui::ImGuiDataType_S32, v,
                                      num_components, &v_min, &v_max, format, flags);

    // copy back
    API->object->array_int_clear(ck_array);
    for (int i = 0; i < num_components; ++i) {
        API->object->array_int_push_back(ck_array, v[i]);
    }
}

CK_DLL_SFUN(ui_SliderScalarN_CKFLOAT)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label          = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_ArrayFloat* ck_array = (Chuck_ArrayFloat*)GET_NEXT_OBJECT(ARGS);

    int num_components = API->object->array_float_size(ck_array);

    float* v = ARENA_PUSH_COUNT(&audio_frame_arena, float, num_components);

    for (int i = 0; i < num_components; ++i) {
        v[i] = API->object->array_float_get_idx(ck_array, i);
    }

    float v_min = GET_NEXT_FLOAT(ARGS);
    float v_max = GET_NEXT_FLOAT(ARGS);

    RETURN->v_int = cimgui::ImGui_SliderScalarN(label, cimgui::ImGuiDataType_Float, v,
                                                num_components, &v_min, &v_max);

    API->object->array_float_clear(ck_array);

    // copy back
    for (int i = 0; i < num_components; ++i) {
        API->object->array_float_push_back(ck_array, v[i]);
    }
}

CK_DLL_SFUN(ui_SliderScalarNEx_CKFLOAT)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label          = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_ArrayFloat* ck_array = (Chuck_ArrayFloat*)GET_NEXT_OBJECT(ARGS);

    int num_components = API->object->array_float_size(ck_array);

    float* v = ARENA_PUSH_COUNT(&audio_frame_arena, float, num_components);

    for (int i = 0; i < num_components; ++i) {
        v[i] = API->object->array_float_get_idx(ck_array, i);
    }

    float v_min        = GET_NEXT_FLOAT(ARGS);
    float v_max        = GET_NEXT_FLOAT(ARGS);
    const char* format = API->object->str(GET_NEXT_STRING(ARGS)); // NULL = "%.0f"
    int flags          = GET_NEXT_INT(ARGS);

    RETURN->v_int
      = cimgui::ImGui_SliderScalarNEx(label, cimgui::ImGuiDataType_Float, v,
                                      num_components, &v_min, &v_max, format, flags);

    // copy back
    API->object->array_float_clear(ck_array);
    for (int i = 0; i < num_components; ++i) {
        API->object->array_float_push_back(ck_array, v[i]);
    }
}

CK_DLL_SFUN(ui_VSliderFloat)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    t_CKVEC2 size     = GET_NEXT_VEC2(ARGS);

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    float* v          = (float*)OBJ_MEMBER_UINT(obj, ui_float_ptr_offset);

    float v_min = GET_NEXT_FLOAT(ARGS);
    float v_max = GET_NEXT_FLOAT(ARGS);

    RETURN->v_int = cimgui::ImGui_VSliderFloat(label, { (float)size.x, (float)size.y },
                                               v, v_min, v_max);
}

CK_DLL_SFUN(ui_VSliderFloatEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    t_CKVEC2 size     = GET_NEXT_VEC2(ARGS);

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    float* v          = (float*)OBJ_MEMBER_UINT(obj, ui_float_ptr_offset);

    float v_min        = GET_NEXT_FLOAT(ARGS);
    float v_max        = GET_NEXT_FLOAT(ARGS);
    const char* format = API->object->str(GET_NEXT_STRING(ARGS)); // NULL = "%.3f"
    int flags          = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_VSliderFloatEx(
      label, { (float)size.x, (float)size.y }, v, v_min, v_max, format, flags);
}

CK_DLL_SFUN(ui_VSliderInt)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    t_CKVEC2 size     = GET_NEXT_VEC2(ARGS);

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    int* v            = (int*)OBJ_MEMBER_UINT(obj, ui_int_ptr_offset);

    int v_min = GET_NEXT_INT(ARGS);
    int v_max = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_VSliderInt(label, { (float)size.x, (float)size.y }, v,
                                             v_min, v_max);
}

CK_DLL_SFUN(ui_VSliderIntEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    t_CKVEC2 size     = GET_NEXT_VEC2(ARGS);

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    int* v            = (int*)OBJ_MEMBER_UINT(obj, ui_int_ptr_offset);

    int v_min          = GET_NEXT_INT(ARGS);
    int v_max          = GET_NEXT_INT(ARGS);
    const char* format = API->object->str(GET_NEXT_STRING(ARGS)); // NULL = "%.0f"
    int flags          = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_VSliderIntEx(label, { (float)size.x, (float)size.y },
                                               v, v_min, v_max, format, flags);
}

// ============================================================================
// Widgets: Input with Keyboard
// ============================================================================

static char* UI_String_ResizeBuffer(Chuck_Object* obj, size_t new_cap, CK_DL_API API)
{
    char* buf      = (char*)OBJ_MEMBER_UINT(obj, ui_string_ptr_offset);
    size_t buf_cap = OBJ_MEMBER_UINT(obj, ui_string_cap_offset);

    if (new_cap >= buf_cap) {
        new_cap = MAX(new_cap + 1, buf_cap * 2);
        buf     = (char*)reallocate(buf, buf_cap, new_cap);
        OBJ_MEMBER_UINT(obj, ui_string_ptr_offset) = (t_CKUINT)buf;
        OBJ_MEMBER_UINT(obj, ui_string_cap_offset) = new_cap;
    }

    return buf;
}

CK_DLL_SFUN(ui_InputText)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    char* buf         = (char*)OBJ_MEMBER_UINT(obj, ui_string_ptr_offset);
    int buf_size      = OBJ_MEMBER_INT(obj, ui_string_cap_offset);

    RETURN->v_int = cimgui::ImGui_InputText(label, buf, buf_size, 0);
}

CK_DLL_SFUN(ui_InputTextEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj    = GET_NEXT_OBJECT(ARGS);
    size_t input_max_len = GET_NEXT_INT(ARGS);
    int flags            = GET_NEXT_INT(ARGS);

    char* buf = UI_String_ResizeBuffer(obj, input_max_len, API);

    // RETURN->v_int = cimgui::ImGui_InputTextEx(label, buf, input_max_len,
    // flags, NULL, NULL);
    RETURN->v_int = cimgui::ImGui_InputText(label, buf, input_max_len, flags);
}

CK_DLL_SFUN(ui_InputTextMultiline)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);

    char* buf    = UI_String_ResizeBuffer(obj, strlen(label), API);
    int buf_size = OBJ_MEMBER_INT(obj, ui_string_cap_offset);

    RETURN->v_int = cimgui::ImGui_InputTextMultiline(label, buf, buf_size);
}
CK_DLL_SFUN(ui_InputTextMultilineEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj    = GET_NEXT_OBJECT(ARGS);
    size_t input_max_len = GET_NEXT_INT(ARGS);
    t_CKVEC2 size        = GET_NEXT_VEC2(ARGS);
    int flags            = GET_NEXT_INT(ARGS);

    char* buf = UI_String_ResizeBuffer(obj, input_max_len, API);

    RETURN->v_int = cimgui::ImGui_InputTextMultilineEx(
      label, buf, input_max_len, { (float)size.x, (float)size.y }, flags, NULL, NULL);
}
CK_DLL_SFUN(ui_InputTextWithHint)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    const char* hint  = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    char* buf         = (char*)OBJ_MEMBER_UINT(obj, ui_string_ptr_offset);
    int buf_size      = OBJ_MEMBER_INT(obj, ui_string_cap_offset);

    RETURN->v_int = cimgui::ImGui_InputTextWithHint(label, hint, buf, buf_size, 0);
}
CK_DLL_SFUN(ui_InputTextWithHintEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label    = API->object->str(GET_NEXT_STRING(ARGS));
    const char* hint     = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* obj    = GET_NEXT_OBJECT(ARGS);
    size_t input_max_len = GET_NEXT_INT(ARGS);
    int flags            = GET_NEXT_INT(ARGS);

    char* buf = UI_String_ResizeBuffer(obj, input_max_len, API);

    RETURN->v_int = cimgui::ImGui_InputTextWithHintEx(label, hint, buf, input_max_len,
                                                      flags, NULL, NULL);
}

CK_DLL_SFUN(ui_InputFloat)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label      = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* ui_float = GET_NEXT_OBJECT(ARGS);
    float* v               = (float*)OBJ_MEMBER_UINT(ui_float, ui_float_ptr_offset);

    RETURN->v_int = cimgui::ImGui_InputFloat(label, v);
}

CK_DLL_SFUN(ui_InputFloatEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label      = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* ui_float = GET_NEXT_OBJECT(ARGS);
    float* v               = (float*)OBJ_MEMBER_UINT(ui_float, ui_float_ptr_offset);
    float step             = GET_NEXT_FLOAT(ARGS);
    float step_fast        = GET_NEXT_FLOAT(ARGS);
    const char* format     = API->object->str(GET_NEXT_STRING(ARGS));
    int flags              = GET_NEXT_INT(ARGS);

    RETURN->v_int
      = cimgui::ImGui_InputFloatEx(label, v, step, step_fast, format, flags);
}

CK_DLL_SFUN(ui_InputInt)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label    = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* ui_int = GET_NEXT_OBJECT(ARGS);
    int* v               = (int*)OBJ_MEMBER_UINT(ui_int, ui_int_ptr_offset);

    RETURN->v_int = cimgui::ImGui_InputInt(label, v);
}

CK_DLL_SFUN(ui_InputIntEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label    = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* ui_int = GET_NEXT_OBJECT(ARGS);
    int* v               = (int*)OBJ_MEMBER_UINT(ui_int, ui_int_ptr_offset);
    int step             = GET_NEXT_INT(ARGS);
    int step_fast        = GET_NEXT_INT(ARGS);
    int flags            = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_InputIntEx(label, v, step, step_fast, flags);
}
CK_DLL_SFUN(ui_InputScalarN_CKINT)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label        = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_ArrayInt* ck_array = (Chuck_ArrayInt*)GET_NEXT_OBJECT(ARGS);
    int num_components       = API->object->array_int_size(ck_array);

    int* v = ARENA_PUSH_COUNT(&audio_frame_arena, int, num_components);
    for (int i = 0; i < num_components; ++i) {
        v[i] = API->object->array_int_get_idx(ck_array, i);
    }

    RETURN->v_int
      = cimgui::ImGui_InputScalarN(label, cimgui::ImGuiDataType_S32, v, num_components);

    // copy back
    API->object->array_int_clear(ck_array);
    for (int i = 0; i < num_components; ++i) {
        API->object->array_int_push_back(ck_array, v[i]);
    }
}

CK_DLL_SFUN(ui_InputScalarNEx_CKINT)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label        = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_ArrayInt* ck_array = (Chuck_ArrayInt*)GET_NEXT_OBJECT(ARGS);
    int num_components       = API->object->array_int_size(ck_array);

    int* v = ARENA_PUSH_COUNT(&audio_frame_arena, int, num_components);
    for (int i = 0; i < num_components; ++i) {
        v[i] = API->object->array_int_get_idx(ck_array, i);
    }

    int step           = GET_NEXT_INT(ARGS);
    int step_fast      = GET_NEXT_INT(ARGS);
    const char* format = API->object->str(GET_NEXT_STRING(ARGS));
    int flags          = GET_NEXT_INT(ARGS);

    RETURN->v_int
      = cimgui::ImGui_InputScalarNEx(label, cimgui::ImGuiDataType_S32, v,
                                     num_components, &step, &step_fast, format, flags);

    // copy back
    API->object->array_int_clear(ck_array);
    for (int i = 0; i < num_components; ++i) {
        API->object->array_int_push_back(ck_array, v[i]);
    }
}

CK_DLL_SFUN(ui_InputScalarN_CKFLOAT)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label          = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_ArrayFloat* ck_array = (Chuck_ArrayFloat*)GET_NEXT_OBJECT(ARGS);
    int num_components         = API->object->array_float_size(ck_array);

    float* v = ARENA_PUSH_COUNT(&audio_frame_arena, float, num_components);
    for (int i = 0; i < num_components; ++i) {
        v[i] = API->object->array_float_get_idx(ck_array, i);
    }

    RETURN->v_int = cimgui::ImGui_InputScalarN(label, cimgui::ImGuiDataType_Float, v,
                                               num_components);

    // copy back
    API->object->array_float_clear(ck_array);
    for (int i = 0; i < num_components; ++i) {
        API->object->array_float_push_back(ck_array, v[i]);
    }
}

CK_DLL_SFUN(ui_InputScalarNEx_CKFLOAT)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label          = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_ArrayFloat* ck_array = (Chuck_ArrayFloat*)GET_NEXT_OBJECT(ARGS);
    int num_components         = API->object->array_float_size(ck_array);

    float* v = ARENA_PUSH_COUNT(&audio_frame_arena, float, num_components);
    for (int i = 0; i < num_components; ++i) {
        v[i] = API->object->array_float_get_idx(ck_array, i);
    }

    float step         = GET_NEXT_FLOAT(ARGS);
    float step_fast    = GET_NEXT_FLOAT(ARGS);
    const char* format = API->object->str(GET_NEXT_STRING(ARGS));
    int flags          = GET_NEXT_INT(ARGS);

    RETURN->v_int
      = cimgui::ImGui_InputScalarNEx(label, cimgui::ImGuiDataType_Float, v,
                                     num_components, &step, &step_fast, format, flags);

    // copy back
    API->object->array_float_clear(ck_array);
    for (int i = 0; i < num_components; ++i) {
        API->object->array_float_push_back(ck_array, v[i]);
    }
}

// ============================================================================
// Widgets: Color Editor/Picker
// ============================================================================

CK_DLL_SFUN(ui_ColorEdit3)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    float* col        = (float*)OBJ_MEMBER_UINT(obj, ui_float3_ptr_offset);
    int flags         = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_ColorEdit3(label, col, flags);
}

CK_DLL_SFUN(ui_ColorEdit3_NoFlags)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    float* col        = (float*)OBJ_MEMBER_UINT(obj, ui_float3_ptr_offset);

    RETURN->v_int = cimgui::ImGui_ColorEdit3(label, col, 0);
}

CK_DLL_SFUN(ui_ColorEdit4)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    float* col        = (float*)OBJ_MEMBER_UINT(obj, ui_float4_ptr_offset);
    int flags         = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_ColorEdit4(label, col, flags);
}

CK_DLL_SFUN(ui_ColorEdit4_NoFlags)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    float* col        = (float*)OBJ_MEMBER_UINT(obj, ui_float4_ptr_offset);

    RETURN->v_int = cimgui::ImGui_ColorEdit4(label, col, 0);
}

CK_DLL_SFUN(ui_ColorPicker3)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    float* col        = (float*)OBJ_MEMBER_UINT(obj, ui_float3_ptr_offset);
    int flags         = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_ColorPicker3(label, col, flags);
}

CK_DLL_SFUN(ui_ColorPicker4)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    float* col        = (float*)OBJ_MEMBER_UINT(obj, ui_float4_ptr_offset);
    int flags         = GET_NEXT_INT(ARGS);
    t_CKVEC4 ref_col  = GET_NEXT_VEC4(ARGS);

    float ref_color_arr[4]
      = { (float)ref_col.x, (float)ref_col.y, (float)ref_col.z, (float)ref_col.w };

    RETURN->v_int = cimgui::ImGui_ColorPicker4(label, col, flags, ref_color_arr);
}

CK_DLL_SFUN(ui_ColorPicker4_no_ref_col)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    float* col        = (float*)OBJ_MEMBER_UINT(obj, ui_float4_ptr_offset);
    int flags         = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_ColorPicker4(label, col, flags, NULL);
}

CK_DLL_SFUN(ui_ColorButton)
{
    if (!verifyInitialization(SHRED)) return;
    const char* desc = API->object->str(GET_NEXT_STRING(ARGS));
    t_CKVEC4 color   = GET_NEXT_VEC4(ARGS);
    int flags        = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_ColorButton(
      desc, { (float)color.x, (float)color.y, (float)color.z, (float)color.w }, flags);
}

CK_DLL_SFUN(ui_ColorButtonEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* desc = API->object->str(GET_NEXT_STRING(ARGS));
    t_CKVEC4 color   = GET_NEXT_VEC4(ARGS);
    int flags        = GET_NEXT_INT(ARGS);
    t_CKVEC2 size    = GET_NEXT_VEC2(ARGS);

    RETURN->v_int = cimgui::ImGui_ColorButtonEx(
      desc, { (float)color.x, (float)color.y, (float)color.z, (float)color.w }, flags,
      { (float)size.x, (float)size.y });
}

CK_DLL_SFUN(ui_SetColorEditOptions)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_SetColorEditOptions(GET_NEXT_INT(ARGS));
}

// ============================================================================
// Widgets: Trees
// ============================================================================

CK_DLL_SFUN(ui_TreeNode)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    RETURN->v_int     = cimgui::ImGui_TreeNode(label);
}

CK_DLL_SFUN(ui_TreeNodeStrUnformatted)
{
    if (!verifyInitialization(SHRED)) return;
    const char* str_id = API->object->str(GET_NEXT_STRING(ARGS));
    const char* text   = API->object->str(GET_NEXT_STRING(ARGS));
    RETURN->v_int      = cimgui::ImGui_TreeNodeStrUnformatted(str_id, text);
}

CK_DLL_SFUN(ui_TreeNodeEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    int flags         = GET_NEXT_INT(ARGS);
    RETURN->v_int     = cimgui::ImGui_TreeNodeEx(label, flags);
}

CK_DLL_SFUN(ui_TreeNodeExStrUnformatted)
{
    if (!verifyInitialization(SHRED)) return;
    const char* str_id = API->object->str(GET_NEXT_STRING(ARGS));
    int flags          = GET_NEXT_INT(ARGS);
    const char* text   = API->object->str(GET_NEXT_STRING(ARGS));
    RETURN->v_int      = cimgui::ImGui_TreeNodeExStrUnformatted(str_id, flags, text);
}

CK_DLL_SFUN(ui_TreePush)
{
    if (!verifyInitialization(SHRED)) return;
    const char* str_id = API->object->str(GET_NEXT_STRING(ARGS));
    cimgui::ImGui_TreePush(str_id);
}

CK_DLL_SFUN(ui_TreePop)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_TreePop();
}

CK_DLL_SFUN(ui_GetTreeNodeToLabelSpacing)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_float = cimgui::ImGui_GetTreeNodeToLabelSpacing();
}

CK_DLL_SFUN(ui_CollapsingHeader)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    int flags         = GET_NEXT_INT(ARGS);
    RETURN->v_int     = cimgui::ImGui_CollapsingHeader(label, flags);
}

CK_DLL_SFUN(ui_CollapsingHeaderBoolPtr)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    bool* v = CHUGL_UI_VAL_PTR(bool, GET_NEXT_OBJECT(ARGS), ui_bool_val_offset);

    int flags     = GET_NEXT_INT(ARGS);
    RETURN->v_int = cimgui::ImGui_CollapsingHeaderBoolPtr(label, v, flags);
}

CK_DLL_SFUN(ui_SetNextItemOpen)
{
    if (!verifyInitialization(SHRED)) return;
    bool is_open = GET_NEXT_INT(ARGS);
    int cond     = GET_NEXT_INT(ARGS);
    cimgui::ImGui_SetNextItemOpen(is_open, cond);
}

// ============================================================================
// Widgets: Selectable
// ============================================================================

CK_DLL_SFUN(ui_Selectable)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    RETURN->v_int     = cimgui::ImGui_Selectable(label);
}

CK_DLL_SFUN(ui_SelectableEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    bool selected     = GET_NEXT_INT(ARGS);
    int flags         = GET_NEXT_INT(ARGS);
    t_CKVEC2 size     = GET_NEXT_VEC2(ARGS);

    RETURN->v_int = cimgui::ImGui_SelectableEx(label, selected, flags,
                                               { (float)size.x, (float)size.y });
}

CK_DLL_SFUN(ui_SelectableBoolPtr)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    bool* v = CHUGL_UI_VAL_PTR(bool, GET_NEXT_OBJECT(ARGS), ui_bool_val_offset);

    int flags = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_SelectableBoolPtr(label, v, flags);
}

CK_DLL_SFUN(ui_SelectableBoolPtrEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    bool* v = CHUGL_UI_VAL_PTR(bool, GET_NEXT_OBJECT(ARGS), ui_bool_val_offset);

    int flags     = GET_NEXT_INT(ARGS);
    t_CKVEC2 size = GET_NEXT_VEC2(ARGS);

    RETURN->v_int = cimgui::ImGui_SelectableBoolPtrEx(label, v, flags,
                                                      { (float)size.x, (float)size.y });
}

// ============================================================================
// Widgets: List Boxes
// ============================================================================

CK_DLL_SFUN(ui_BeginListBox)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    t_CKVEC2 size     = GET_NEXT_VEC2(ARGS);
    RETURN->v_int = cimgui::ImGui_BeginListBox(label, { (float)size.x, (float)size.y });
}

CK_DLL_SFUN(ui_EndListBox)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_EndListBox();
}

CK_DLL_SFUN(ui_ListBox)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label        = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* ui_int_obj = GET_NEXT_OBJECT(ARGS);
    int* current_item        = (int*)OBJ_MEMBER_UINT(ui_int_obj, ui_int_ptr_offset);
    Chuck_ArrayInt* items    = (Chuck_ArrayInt*)GET_NEXT_OBJECT(ARGS);
    int height_in_items      = GET_NEXT_INT(ARGS);

    // copy chuck string array to arena
    int num_items = API->object->array_int_size(items);
    const char** items_arr
      = ARENA_PUSH_COUNT(&audio_frame_arena, const char*, num_items);

    for (int i = 0; i < num_items; ++i) {
        items_arr[i]
          = API->object->str((Chuck_String*)API->object->array_int_get_idx(items, i));
    }

    RETURN->v_int = cimgui::ImGui_ListBox(label, current_item, items_arr, num_items,
                                          height_in_items);
}

CK_DLL_SFUN(ui_ListBox_default)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label        = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_Object* ui_int_obj = GET_NEXT_OBJECT(ARGS);
    int* current_item        = (int*)OBJ_MEMBER_UINT(ui_int_obj, ui_int_ptr_offset);
    Chuck_ArrayInt* items    = (Chuck_ArrayInt*)GET_NEXT_OBJECT(ARGS);

    // copy chuck string array to arena
    int num_items = API->object->array_int_size(items);
    const char** items_arr
      = ARENA_PUSH_COUNT(&audio_frame_arena, const char*, num_items);

    for (int i = 0; i < num_items; ++i) {
        items_arr[i]
          = API->object->str((Chuck_String*)API->object->array_int_get_idx(items, i));
    }

    RETURN->v_int
      = cimgui::ImGui_ListBox(label, current_item, items_arr, num_items, -1);
}

// ============================================================================
// Widgets: Data Plotting
// ============================================================================

CK_DLL_SFUN(ui_PlotLines)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label        = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_ArrayFloat* values = (Chuck_ArrayFloat*)GET_NEXT_OBJECT(ARGS);

    // copy chuck array to arena (Convert from double to float)
    int num_values    = API->object->array_float_size(values);
    float* values_arr = ARENA_PUSH_COUNT(&audio_frame_arena, float, num_values);
    for (int i = 0; i < num_values; ++i) {
        values_arr[i] = API->object->array_float_get_idx(values, i);
    }

    cimgui::ImGui_PlotLines(label, values_arr, num_values);
}

CK_DLL_SFUN(ui_PlotLinesEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label        = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_ArrayFloat* values = (Chuck_ArrayFloat*)GET_NEXT_OBJECT(ARGS);
    int values_offset        = GET_NEXT_INT(ARGS);
    const char* overlay_text = API->object->str(GET_NEXT_STRING(ARGS));
    float scale_min          = GET_NEXT_FLOAT(ARGS);
    float scale_max          = GET_NEXT_FLOAT(ARGS);
    t_CKVEC2 graph_size      = GET_NEXT_VEC2(ARGS);

    // copy chuck array to arena
    int num_values    = API->object->array_float_size(values);
    float* values_arr = ARENA_PUSH_COUNT(&audio_frame_arena, float, num_values);
    for (int i = 0; i < num_values; ++i) {
        values_arr[i] = API->object->array_float_get_idx(values, i);
    }

    cimgui::ImGui_PlotLinesEx(
      label, values_arr, num_values, values_offset, overlay_text, scale_min, scale_max,
      { (float)graph_size.x, (float)graph_size.y }, sizeof(float));
}

CK_DLL_SFUN(ui_PlotHistogram)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label        = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_ArrayFloat* values = (Chuck_ArrayFloat*)GET_NEXT_OBJECT(ARGS);

    // copy chuck array to arena
    int num_values    = API->object->array_float_size(values);
    float* values_arr = ARENA_PUSH_COUNT(&audio_frame_arena, float, num_values);
    for (int i = 0; i < num_values; ++i) {
        values_arr[i] = API->object->array_float_get_idx(values, i);
    }

    cimgui::ImGui_PlotHistogram(label, values_arr, num_values);
}

CK_DLL_SFUN(ui_PlotHistogramEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label        = API->object->str(GET_NEXT_STRING(ARGS));
    Chuck_ArrayFloat* values = (Chuck_ArrayFloat*)GET_NEXT_OBJECT(ARGS);
    int values_offset        = GET_NEXT_INT(ARGS);
    const char* overlay_text = API->object->str(GET_NEXT_STRING(ARGS));
    float scale_min          = GET_NEXT_FLOAT(ARGS);
    float scale_max          = GET_NEXT_FLOAT(ARGS);
    t_CKVEC2 graph_size      = GET_NEXT_VEC2(ARGS);

    // copy chuck array to arena
    int num_values    = API->object->array_float_size(values);
    float* values_arr = ARENA_PUSH_COUNT(&audio_frame_arena, float, num_values);
    for (int i = 0; i < num_values; ++i) {
        values_arr[i] = API->object->array_float_get_idx(values, i);
    }

    cimgui::ImGui_PlotHistogramEx(
      label, values_arr, num_values, values_offset, overlay_text, scale_min, scale_max,
      { (float)graph_size.x, (float)graph_size.y }, sizeof(float));
}

// ============================================================================
// Widgets: Menus
// ============================================================================

CK_DLL_SFUN(ui_BeginMenuBar)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_BeginMenuBar();
}

CK_DLL_SFUN(ui_EndMenuBar)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_EndMenuBar();
}

CK_DLL_SFUN(ui_BeginMainMenuBar)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_BeginMainMenuBar();
}

CK_DLL_SFUN(ui_EndMainMenuBar)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_EndMainMenuBar();
}

CK_DLL_SFUN(ui_BeginMenu)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    RETURN->v_int     = cimgui::ImGui_BeginMenu(label);
}

CK_DLL_SFUN(ui_BeginMenuEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    bool enabled      = GET_NEXT_INT(ARGS);
    RETURN->v_int     = cimgui::ImGui_BeginMenuEx(label, enabled);
}

CK_DLL_SFUN(ui_EndMenu)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_EndMenu();
}

CK_DLL_SFUN(ui_MenuItem)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    RETURN->v_int     = cimgui::ImGui_MenuItem(label);
}

CK_DLL_SFUN(ui_MenuItemBoolPtr)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label    = API->object->str(GET_NEXT_STRING(ARGS));
    const char* shortcut = API->object->str(GET_NEXT_STRING(ARGS));

    bool* selected = CHUGL_UI_VAL_PTR(bool, GET_NEXT_OBJECT(ARGS), ui_bool_val_offset);

    bool enabled  = GET_NEXT_INT(ARGS);
    RETURN->v_int = cimgui::ImGui_MenuItemBoolPtr(label, shortcut, selected, enabled);
}

CK_DLL_SFUN(ui_Knob)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    float* v          = (float*)OBJ_MEMBER_UINT(obj, ui_float_ptr_offset);

    float v_min = GET_NEXT_FLOAT(ARGS);
    float v_max = GET_NEXT_FLOAT(ARGS);

    RETURN->v_int = ImGuiKnobs::Knob(label, v, v_min, v_max);
}

CK_DLL_SFUN(ui_KnobInt)
{
    if (!verifyInitialization(SHRED)) return;

    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    int* v            = (int*)OBJ_MEMBER_UINT(obj, ui_int_ptr_offset);

    int v_min = GET_NEXT_INT(ARGS);
    int v_max = GET_NEXT_INT(ARGS);

    RETURN->v_int = ImGuiKnobs::KnobInt(label, v, v_min, v_max);
}

CK_DLL_SFUN(ui_KnobEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    float* v          = (float*)OBJ_MEMBER_UINT(obj, ui_float_ptr_offset);

    float v_min     = GET_NEXT_FLOAT(ARGS);
    float v_max     = GET_NEXT_FLOAT(ARGS);
    float speed     = GET_NEXT_FLOAT(ARGS);
    const char* fmt = API->object->str(GET_NEXT_STRING(ARGS));
    int variant     = GET_NEXT_INT(ARGS);
    float size      = GET_NEXT_FLOAT(ARGS);
    int flags       = GET_NEXT_INT(ARGS);
    int steps       = GET_NEXT_INT(ARGS);
    float angle_min = GET_NEXT_FLOAT(ARGS);
    float angle_max = GET_NEXT_FLOAT(ARGS);

    RETURN->v_int = ImGuiKnobs::Knob(label, v, v_min, v_max, speed, fmt, variant, size,
                                     flags, steps, angle_min, angle_max);
}

CK_DLL_SFUN(ui_KnobIntEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    Chuck_Object* obj = GET_NEXT_OBJECT(ARGS);
    int* v            = (int*)OBJ_MEMBER_UINT(obj, ui_int_ptr_offset);

    int v_min = GET_NEXT_INT(ARGS);
    int v_max = GET_NEXT_INT(ARGS);

    float speed     = GET_NEXT_FLOAT(ARGS);
    const char* fmt = API->object->str(GET_NEXT_STRING(ARGS));
    int variant     = GET_NEXT_INT(ARGS);
    float size      = GET_NEXT_FLOAT(ARGS);
    int flags       = GET_NEXT_INT(ARGS);
    int steps       = GET_NEXT_INT(ARGS);
    float angle_min = GET_NEXT_FLOAT(ARGS);
    float angle_max = GET_NEXT_FLOAT(ARGS);

    RETURN->v_int = ImGuiKnobs::KnobInt(label, v, v_min, v_max, speed, fmt, variant,
                                        size, flags, steps, angle_min, angle_max);
}

// ============================================================================
// Tooltips
// ============================================================================

CK_DLL_SFUN(ui_BeginItemTooltip)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_BeginItemTooltip();
}

CK_DLL_SFUN(ui_BeginTooltip)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_BeginTooltip();
}
CK_DLL_SFUN(ui_EndTooltip)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_EndTooltip();
}

CK_DLL_SFUN(ui_SetTooltipUnformatted)
{
    if (!verifyInitialization(SHRED)) return;
    const char* text = API->object->str(GET_NEXT_STRING(ARGS));
    cimgui::ImGui_SetTooltipUnformatted(text);
}

CK_DLL_SFUN(ui_SetItemTooltipUnformatted)
{
    if (!verifyInitialization(SHRED)) return;
    const char* text = API->object->str(GET_NEXT_STRING(ARGS));
    cimgui::ImGui_SetItemTooltipUnformatted(text);
}

// ============================================================================
// Popups, Modals
// ============================================================================

CK_DLL_SFUN(ui_BeginPopup)
{
    if (!verifyInitialization(SHRED)) return;
    const char* str_id = API->object->str(GET_NEXT_STRING(ARGS));
    int flags          = GET_NEXT_INT(ARGS);
    RETURN->v_int      = cimgui::ImGui_BeginPopup(str_id, flags);
}

CK_DLL_SFUN(ui_BeginPopupModal)
{
    if (!verifyInitialization(SHRED)) return;
    const char* name = API->object->str(GET_NEXT_STRING(ARGS));

    bool* p_open = CHUGL_UI_VAL_PTR(bool, GET_NEXT_OBJECT(ARGS), ui_bool_val_offset);

    int flags = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_BeginPopupModal(name, p_open, flags);
}

CK_DLL_SFUN(ui_EndPopup)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_EndPopup();
}

CK_DLL_SFUN(ui_OpenPopup)
{
    if (!verifyInitialization(SHRED)) return;
    const char* str_id = API->object->str(GET_NEXT_STRING(ARGS));
    int flags          = GET_NEXT_INT(ARGS);
    cimgui::ImGui_OpenPopup(str_id, flags);
}

CK_DLL_SFUN(ui_OpenPopupOnItemClick)
{
    if (!verifyInitialization(SHRED)) return;
    const char* str_id = API->object->str(GET_NEXT_STRING(ARGS));
    int popup_flags    = GET_NEXT_INT(ARGS);
    cimgui::ImGui_OpenPopupOnItemClick(str_id, popup_flags);
}

CK_DLL_SFUN(ui_CloseCurrentPopup)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_CloseCurrentPopup();
}

CK_DLL_SFUN(ui_BeginPopupContextItem)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_BeginPopupContextItem();
}

CK_DLL_SFUN(ui_BeginPopupContextItemEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* str_id = API->object->str(GET_NEXT_STRING(ARGS));
    int popup_flags    = GET_NEXT_INT(ARGS);
    RETURN->v_int      = cimgui::ImGui_BeginPopupContextItemEx(str_id, popup_flags);
}

CK_DLL_SFUN(ui_BeginPopupContextWindow)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_BeginPopupContextWindow();
}

CK_DLL_SFUN(ui_BeginPopupContextWindowEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* str_id = API->object->str(GET_NEXT_STRING(ARGS));
    int popup_flags    = GET_NEXT_INT(ARGS);
    RETURN->v_int      = cimgui::ImGui_BeginPopupContextWindowEx(str_id, popup_flags);
}

CK_DLL_SFUN(ui_BeginPopupContextVoid)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_BeginPopupContextVoid();
}

CK_DLL_SFUN(ui_BeginPopupContextVoidEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* str_id = API->object->str(GET_NEXT_STRING(ARGS));
    int popup_flags    = GET_NEXT_INT(ARGS);
    RETURN->v_int      = cimgui::ImGui_BeginPopupContextVoidEx(str_id, popup_flags);
}

CK_DLL_SFUN(ui_IsPopupOpen)
{
    if (!verifyInitialization(SHRED)) return;
    const char* str_id = API->object->str(GET_NEXT_STRING(ARGS));
    int flags          = GET_NEXT_INT(ARGS);
    RETURN->v_int      = cimgui::ImGui_IsPopupOpen(str_id, flags);
}

// ============================================================================
// Tables
// ============================================================================

CK_DLL_SFUN(ui_BeginTable)
{
    if (!verifyInitialization(SHRED)) return;
    const char* str_id = API->object->str(GET_NEXT_STRING(ARGS));
    int column         = GET_NEXT_INT(ARGS);
    int flags          = GET_NEXT_INT(ARGS);

    RETURN->v_int = cimgui::ImGui_BeginTable(str_id, column, flags);
}

CK_DLL_SFUN(ui_BeginTableEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* str_id  = API->object->str(GET_NEXT_STRING(ARGS));
    int column          = GET_NEXT_INT(ARGS);
    int flags           = GET_NEXT_INT(ARGS);
    t_CKVEC2 outer_size = GET_NEXT_VEC2(ARGS);
    float inner_width   = GET_NEXT_FLOAT(ARGS);

    RETURN->v_int = cimgui::ImGui_BeginTableEx(
      str_id, column, flags, { (float)outer_size.x, (float)outer_size.y }, inner_width);
}

CK_DLL_SFUN(ui_EndTable)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_EndTable();
}

CK_DLL_SFUN(ui_TableNextRow)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_TableNextRow();
}

CK_DLL_SFUN(ui_TableNextRowEx)
{
    if (!verifyInitialization(SHRED)) return;
    int row_flags        = GET_NEXT_INT(ARGS);
    float min_row_height = GET_NEXT_FLOAT(ARGS);
    cimgui::ImGui_TableNextRowEx(row_flags, min_row_height);
}

CK_DLL_SFUN(ui_TableNextColumn)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_TableNextColumn();
}

CK_DLL_SFUN(ui_TableSetColumnIndex)
{
    if (!verifyInitialization(SHRED)) return;
    int column_n  = GET_NEXT_INT(ARGS);
    RETURN->v_int = cimgui::ImGui_TableSetColumnIndex(column_n);
}

CK_DLL_SFUN(ui_TableSetupColumn)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    int flags         = GET_NEXT_INT(ARGS);

    cimgui::ImGui_TableSetupColumn(label, flags);
}

CK_DLL_SFUN(ui_TableSetupColumnEx)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label          = API->object->str(GET_NEXT_STRING(ARGS));
    int flags                  = GET_NEXT_INT(ARGS);
    float init_width_or_weight = GET_NEXT_FLOAT(ARGS);
    int user_id                = GET_NEXT_INT(ARGS);

    cimgui::ImGui_TableSetupColumnEx(label, flags, init_width_or_weight, user_id);
}

CK_DLL_SFUN(ui_TableSetupScrollFreeze)
{
    if (!verifyInitialization(SHRED)) return;
    int cols = GET_NEXT_INT(ARGS);
    int rows = GET_NEXT_INT(ARGS);
    cimgui::ImGui_TableSetupScrollFreeze(cols, rows);
}

CK_DLL_SFUN(ui_TableHeader)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    cimgui::ImGui_TableHeader(label);
}

CK_DLL_SFUN(ui_TableHeadersRow)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_TableHeadersRow();
}

CK_DLL_SFUN(ui_TableAngledHeadersRow)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_TableAngledHeadersRow();
}

CK_DLL_SFUN(ui_TableGetColumnCount)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_TableGetColumnCount();
}

CK_DLL_SFUN(ui_TableGetColumnIndex)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_TableGetColumnIndex();
}

CK_DLL_SFUN(ui_TableGetRowIndex)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_TableGetRowIndex();
}

CK_DLL_SFUN(ui_TableGetColumnName)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_string = chugin_createCkString(
      cimgui::ImGui_TableGetColumnName(GET_NEXT_INT(ARGS)), false);
}

CK_DLL_SFUN(ui_TableGetColumnFlags)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_TableGetColumnFlags(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(ui_TableSetColumnEnabled)
{
    if (!verifyInitialization(SHRED)) return;
    int column_n = GET_NEXT_INT(ARGS);
    bool enabled = GET_NEXT_INT(ARGS);
    cimgui::ImGui_TableSetColumnEnabled(column_n, enabled);
}

CK_DLL_SFUN(ui_TableSetBgColor)
{
    if (!verifyInitialization(SHRED)) return;
    int target     = GET_NEXT_INT(ARGS);
    t_CKVEC4 color = GET_NEXT_VEC4(ARGS);
    int column_n   = GET_NEXT_INT(ARGS);

    cimgui::ImU32 color_u32 = cimgui::ImGui_ColorConvertFloat4ToU32(
      { (float)color.x, (float)color.y, (float)color.z, (float)color.w });

    cimgui::ImGui_TableSetBgColor(target, color_u32, column_n);
}

// ============================================================================
// Tab Bar, Tab Item
// ============================================================================

CK_DLL_SFUN(ui_BeginTabBar)
{
    if (!verifyInitialization(SHRED)) return;
    const char* str_id = API->object->str(GET_NEXT_STRING(ARGS));
    int flags          = GET_NEXT_INT(ARGS);
    RETURN->v_int      = cimgui::ImGui_BeginTabBar(str_id, flags);
}

CK_DLL_SFUN(ui_EndTabBar)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_EndTabBar();
}

CK_DLL_SFUN(ui_BeginTabItem)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));

    bool* p_open = CHUGL_UI_VAL_PTR(bool, GET_NEXT_OBJECT(ARGS), ui_bool_val_offset);

    int flags     = GET_NEXT_INT(ARGS);
    RETURN->v_int = cimgui::ImGui_BeginTabItem(label, p_open, flags);
}

CK_DLL_SFUN(ui_EndTabItem)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_EndTabItem();
}

CK_DLL_SFUN(ui_TabItemButton)
{
    if (!verifyInitialization(SHRED)) return;
    const char* label = API->object->str(GET_NEXT_STRING(ARGS));
    int flags         = GET_NEXT_INT(ARGS);
    RETURN->v_int     = cimgui::ImGui_TabItemButton(label, flags);
}

CK_DLL_SFUN(ui_SetTabItemClosed)
{
    if (!verifyInitialization(SHRED)) return;
    const char* tab_or_docked_window_label = API->object->str(GET_NEXT_STRING(ARGS));
    cimgui::ImGui_SetTabItemClosed(tab_or_docked_window_label);
}

// ============================================================================
// Docking
// ============================================================================

CK_DLL_SFUN(ui_DockSpaceOverViewport)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_DockSpaceOverViewport();
}

// ============================================================================
// Disabling
// ============================================================================

CK_DLL_SFUN(ui_BeginDisabled)
{
    if (!verifyInitialization(SHRED)) return;
    int disabled = GET_NEXT_INT(ARGS);
    cimgui::ImGui_BeginDisabled(disabled);
}

CK_DLL_SFUN(ui_BeginDisabled_true)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_BeginDisabled(true);
}

CK_DLL_SFUN(ui_EndDisabled)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_EndDisabled();
}

// ============================================================================
// Clipping
// ============================================================================

CK_DLL_SFUN(ui_PushClipRect)
{
    if (!verifyInitialization(SHRED)) return;
    t_CKVEC2 clip_rect_min                = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 clip_rect_max                = GET_NEXT_VEC2(ARGS);
    bool intersect_with_current_clip_rect = GET_NEXT_INT(ARGS);
    cimgui::ImGui_PushClipRect({ (float)clip_rect_min.x, (float)clip_rect_min.y },
                               { (float)clip_rect_max.x, (float)clip_rect_max.y },
                               intersect_with_current_clip_rect);
}

CK_DLL_SFUN(ui_PopClipRect)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_PopClipRect();
}

// ============================================================================
// Focus, Activation
// ============================================================================

CK_DLL_SFUN(ui_SetItemDefaultFocus)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_SetItemDefaultFocus();
}

CK_DLL_SFUN(ui_SetKeyboardFocusHere)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_SetKeyboardFocusHere();
}

CK_DLL_SFUN(ui_SetKeyboardFocusHereEx)
{
    if (!verifyInitialization(SHRED)) return;
    int offset = GET_NEXT_INT(ARGS);
    cimgui::ImGui_SetKeyboardFocusHereEx(offset);
}

// ============================================================================
// Overlapping Mode
// ============================================================================

CK_DLL_SFUN(ui_SetNextItemAllowOverlap)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_SetNextItemAllowOverlap();
}

// ============================================================================
// Item/Widgets Utilities
// ============================================================================

CK_DLL_SFUN(ui_IsItemHovered)
{
    if (!verifyInitialization(SHRED)) return;
    int flags     = GET_NEXT_INT(ARGS);
    RETURN->v_int = cimgui::ImGui_IsItemHovered(flags);
}

CK_DLL_SFUN(ui_IsItemActive)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsItemActive();
}

CK_DLL_SFUN(ui_IsItemFocused)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsItemFocused();
}

CK_DLL_SFUN(ui_IsItemClicked)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsItemClicked();
}

CK_DLL_SFUN(ui_IsItemClickedEx)
{
    if (!verifyInitialization(SHRED)) return;
    int button    = GET_NEXT_INT(ARGS);
    RETURN->v_int = cimgui::ImGui_IsItemClickedEx(button);
}

CK_DLL_SFUN(ui_IsItemVisible)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsItemVisible();
}

CK_DLL_SFUN(ui_IsItemEdited)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsItemEdited();
}

CK_DLL_SFUN(ui_IsItemActivated)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsItemActivated();
}

CK_DLL_SFUN(ui_IsItemDeactivated)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsItemDeactivated();
}

CK_DLL_SFUN(ui_IsItemDeactivatedAfterEdit)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsItemDeactivatedAfterEdit();
}

CK_DLL_SFUN(ui_IsItemToggledOpen)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsItemToggledOpen();
}

CK_DLL_SFUN(ui_IsAnyItemHovered)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsAnyItemHovered();
}

CK_DLL_SFUN(ui_IsAnyItemActive)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsAnyItemActive();
}

CK_DLL_SFUN(ui_IsAnyItemFocused)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsAnyItemFocused();
}

CK_DLL_SFUN(ui_GetItemID)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_GetItemID();
}

CK_DLL_SFUN(ui_GetItemRectMin)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImVec2 v = cimgui::ImGui_GetItemRectMin();
    RETURN->v_vec2   = { v.x, v.y };
}

CK_DLL_SFUN(ui_GetItemRectMax)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImVec2 v = cimgui::ImGui_GetItemRectMax();
    RETURN->v_vec2   = { v.x, v.y };
}

CK_DLL_SFUN(ui_GetItemRectSize)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImVec2 v = cimgui::ImGui_GetItemRectSize();
    RETURN->v_vec2   = { v.x, v.y };
}

// ============================================================================
// Viewport
// ============================================================================

CK_DLL_SFUN(ui_GetMainViewport)
{
    if (!verifyInitialization(SHRED)) return;
    Chuck_Object* vp_obj = chugin_createCkObj("UI_Viewport", false, SHRED);
    OBJ_MEMBER_UINT(vp_obj, ui_viewport_ptr_offset)
      = (t_CKUINT)cimgui::ImGui_GetMainViewport();
    RETURN->v_object = vp_obj;
}

// ============================================================================
// Text Utilities
// ============================================================================

CK_DLL_SFUN(ui_CalcTextSize)
{
    const char* text    = API->object->str(GET_NEXT_STRING(ARGS));
    cimgui::ImVec2 size = cimgui::ImGui_CalcTextSize(text);
    RETURN->v_vec2      = { size.x, size.y };
}

CK_DLL_SFUN(ui_CalcTextSizeEx)
{
    const char* text                 = API->object->str(GET_NEXT_STRING(ARGS));
    const char* text_end             = API->object->str(GET_NEXT_STRING(ARGS));
    bool hide_text_after_double_hash = GET_NEXT_INT(ARGS);
    float wrap_width                 = GET_NEXT_FLOAT(ARGS);
    cimgui::ImVec2 size              = cimgui::ImGui_CalcTextSizeEx(
      text, text_end, hide_text_after_double_hash, wrap_width);
    RETURN->v_vec2 = { size.x, size.y };
}

// ============================================================================
// Color Utilities
// ============================================================================

CK_DLL_SFUN(ui_ColorConvertRGBtoHSV)
{
    t_CKVEC3 rgb = GET_NEXT_VEC3(ARGS);
    float h, s, v;
    cimgui::ImGui_ColorConvertRGBtoHSV((float)rgb.x, (float)rgb.y, (float)rgb.z, &h, &s,
                                       &v);

    RETURN->v_vec3 = { h, s, v };
}

CK_DLL_SFUN(ui_ColorConvertHSVtoRGB)
{
    t_CKVEC3 hsv = GET_NEXT_VEC3(ARGS);
    float r, g, b;
    cimgui::ImGui_ColorConvertHSVtoRGB((float)hsv.x, (float)hsv.y, (float)hsv.z, &r, &g,
                                       &b);

    RETURN->v_vec3 = { r, g, b };
}

// ============================================================================
// Inputs Utilities
// ============================================================================

CK_DLL_SFUN(ui_IsKeyDown)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsKeyDown(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(ui_IsKeyPressed)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsKeyPressed(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(ui_IsKeyPressedEx)
{
    if (!verifyInitialization(SHRED)) return;
    int key       = GET_NEXT_INT(ARGS);
    int repeat    = GET_NEXT_INT(ARGS);
    RETURN->v_int = cimgui::ImGui_IsKeyPressedEx(key, repeat);
}

CK_DLL_SFUN(ui_IsKeyReleased)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsKeyReleased(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(ui_IsKeyChordPressed)
{
    if (!verifyInitialization(SHRED)) return;
    int keychord  = GET_NEXT_INT(ARGS);
    RETURN->v_int = cimgui::ImGui_IsKeyChordPressed(keychord);
}

CK_DLL_SFUN(ui_GetKeyPressedAmount)
{
    if (!verifyInitialization(SHRED)) return;
    int key            = GET_NEXT_INT(ARGS);
    float repeat_delay = GET_NEXT_FLOAT(ARGS);
    float rate         = GET_NEXT_FLOAT(ARGS);
    RETURN->v_int      = cimgui::ImGui_GetKeyPressedAmount(key, repeat_delay, rate);
}

CK_DLL_SFUN(ui_GetKeyName)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_string
      = chugin_createCkString(cimgui::ImGui_GetKeyName(GET_NEXT_INT(ARGS)), false);
}

CK_DLL_SFUN(ui_SetNextFrameWantCaptureKeyboard)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_SetNextFrameWantCaptureKeyboard(GET_NEXT_INT(ARGS));
}

// ============================================================================
// Mouse Utilities
// ============================================================================

CK_DLL_SFUN(ui_IsMouseDown)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsMouseDown(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(ui_IsMouseClicked)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsMouseClicked(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(ui_IsMouseClickedEx)
{
    if (!verifyInitialization(SHRED)) return;
    int button    = GET_NEXT_INT(ARGS);
    bool repeat   = GET_NEXT_INT(ARGS);
    RETURN->v_int = cimgui::ImGui_IsMouseClickedEx(button, repeat);
}

CK_DLL_SFUN(ui_IsMouseReleased)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsMouseReleased(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(ui_IsMouseDoubleClicked)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsMouseDoubleClicked(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(ui_GetMouseClickedCount)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_GetMouseClickedCount(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(ui_IsMouseHoveringRect)
{
    if (!verifyInitialization(SHRED)) return;
    t_CKVEC2 r_min = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 r_max = GET_NEXT_VEC2(ARGS);
    RETURN->v_int  = cimgui::ImGui_IsMouseHoveringRect(
      { (float)r_min.x, (float)r_min.y }, { (float)r_max.x, (float)r_max.y });
}

CK_DLL_SFUN(ui_IsMouseHoveringRectEx)
{
    if (!verifyInitialization(SHRED)) return;
    t_CKVEC2 r_min = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 r_max = GET_NEXT_VEC2(ARGS);
    bool clip      = GET_NEXT_INT(ARGS);
    RETURN->v_int  = cimgui::ImGui_IsMouseHoveringRectEx(
      { (float)r_min.x, (float)r_min.y }, { (float)r_max.x, (float)r_max.y }, clip);
}

CK_DLL_SFUN(ui_IsMousePosValid)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_IsMousePosValid(NULL);
}

CK_DLL_SFUN(ui_GetMousePos)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImVec2 pos = cimgui::ImGui_GetMousePos();
    RETURN->v_vec2     = { pos.x, pos.y };
}

CK_DLL_SFUN(ui_GetMousePosOnOpeningCurrentPopup)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImVec2 pos = cimgui::ImGui_GetMousePosOnOpeningCurrentPopup();
    RETURN->v_vec2     = { pos.x, pos.y };
}

CK_DLL_SFUN(ui_IsMouseDragging)
{
    if (!verifyInitialization(SHRED)) return;
    int button           = GET_NEXT_INT(ARGS);
    float lock_threshold = GET_NEXT_FLOAT(ARGS);
    RETURN->v_int        = cimgui::ImGui_IsMouseDragging(button, lock_threshold);
}

CK_DLL_SFUN(ui_GetMouseDragDelta)
{
    if (!verifyInitialization(SHRED)) return;
    int button           = GET_NEXT_INT(ARGS);
    float lock_threshold = GET_NEXT_FLOAT(ARGS);
    cimgui::ImVec2 delta = cimgui::ImGui_GetMouseDragDelta(button, lock_threshold);
    RETURN->v_vec2       = { delta.x, delta.y };
}

CK_DLL_SFUN(ui_ResetMouseDragDelta)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_ResetMouseDragDelta();
}

CK_DLL_SFUN(ui_ResetMouseDragDeltaEx)
{
    if (!verifyInitialization(SHRED)) return;
    int button = GET_NEXT_INT(ARGS);
    cimgui::ImGui_ResetMouseDragDeltaEx(button);
}

CK_DLL_SFUN(ui_GetMouseCursor)
{
    if (!verifyInitialization(SHRED)) return;
    RETURN->v_int = cimgui::ImGui_GetMouseCursor();
}

CK_DLL_SFUN(ui_SetMouseCursor)
{
    if (!verifyInitialization(SHRED)) return;
    int cursor_type = GET_NEXT_INT(ARGS);
    cimgui::ImGui_SetMouseCursor(cursor_type);
}

CK_DLL_SFUN(ui_SetNextFrameWantCaptureMouse)
{
    if (!verifyInitialization(SHRED)) return;
    cimgui::ImGui_SetNextFrameWantCaptureMouse(GET_NEXT_INT(ARGS));
}

// ============================================================================
// Clipboard Utilities
// ============================================================================

CK_DLL_SFUN(ui_GetClipboardText)
{
    RETURN->v_string = chugin_createCkString(cimgui::ImGui_GetClipboardText(), false);
}

CK_DLL_SFUN(ui_SetClipboardText)
{
    const char* text = API->object->str(GET_NEXT_STRING(ARGS));
    cimgui::ImGui_SetClipboardText(text);
}

// ============================================================================
// styles
// ============================================================================
CK_DLL_SFUN(ui_styleColorsDark)
{
    cimgui::ImGui_StyleColorsDark(NULL);
}

CK_DLL_SFUN(ui_styleColorsLight)
{
    cimgui::ImGui_StyleColorsLight(NULL);
}

CK_DLL_SFUN(ui_styleColorsClassic)
{
    cimgui::ImGui_StyleColorsClassic(NULL);
}
