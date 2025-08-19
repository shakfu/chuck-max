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
#include "ulib_helper.h"
#include <GLFW/glfw3.h>
#include <chuck/chugin.h>

// monitor (not implemented)
CK_DLL_SFUN(gwindow_monitor_info);
static t_CKINT monitor_info_width_offset            = 0;
static t_CKINT monitor_info_height_offset           = 0;
static t_CKINT monitor_info_refresh_rate_offset     = 0;
static t_CKINT monitor_info_virtual_pos_offset      = 0;
static t_CKINT monitor_info_physical_size_mm_offset = 0;
static t_CKINT monitor_info_content_scale_offset    = 0;
static t_CKINT monitor_info_work_area_offset        = 0;
static t_CKINT monitor_info_name_offset             = 0;

// callbacks
CK_DLL_SFUN(gwindow_close_event);
CK_DLL_SFUN(gwindow_window_resize_event);
CK_DLL_SFUN(gwindow_window_content_scale_event);

// closing
CK_DLL_SFUN(gwindow_set_closeable);
CK_DLL_SFUN(gwindow_get_closeable);
CK_DLL_SFUN(gwindow_close);

// window size
CK_DLL_SFUN(gwindow_fullscreen);
CK_DLL_SFUN(gwindow_fullscreen_width_height);
CK_DLL_SFUN(gwindow_windowed);
CK_DLL_SFUN(gwindow_windowed_prev_size);
CK_DLL_SFUN(gwindow_maximize);

CK_DLL_SFUN(gwindow_get_window_size);
CK_DLL_SFUN(gwindow_get_framebuffer_size);

// content scale
CK_DLL_SFUN(gwindow_get_content_scale);

// size limits
CK_DLL_SFUN(gwindow_set_window_size_limits);

// position
CK_DLL_SFUN(gwindow_set_window_pos);
CK_DLL_SFUN(gwindow_window_center);

// title
CK_DLL_SFUN(gwindow_set_title);

// iconify
CK_DLL_SFUN(gwindow_inconify);
CK_DLL_SFUN(gwindow_restore);

// attributes
CK_DLL_SFUN(gwindow_set_attrib_resizable);
CK_DLL_SFUN(gwindow_set_attrib_decorated);
CK_DLL_SFUN(gwindow_set_attrib_floating);
CK_DLL_SFUN(gwindow_set_attrib_transparent);
CK_DLL_SFUN(gwindow_opacity);

// mouse
CK_DLL_SFUN(gwindow_get_mouse_pos);
CK_DLL_SFUN(gwindow_get_mouse_delta_pos);
CK_DLL_SFUN(gwindow_set_mouse_mode); // normal, disabled, hidden
CK_DLL_SFUN(gwindow_get_mouse_mode); // normal, disabled, hidden
static t_CKINT mouse_mode_normal   = 0;
static t_CKINT mouse_mode_hidden   = 1;
static t_CKINT mouse_mode_disabled = 2;
static t_CKINT mouse_mode_current  = 0;
CK_DLL_SFUN(gwindow_get_mouse_scroll_dx);
CK_DLL_SFUN(gwindow_get_mouse_scroll_dy);
CK_DLL_SFUN(gwindow_get_mouse_scroll);

CK_DLL_SFUN(gwindow_get_mouse_left_button);
CK_DLL_SFUN(gwindow_get_mouse_right_button);
CK_DLL_SFUN(gwindow_get_mouse_left_button_click);
CK_DLL_SFUN(gwindow_get_mouse_right_button_click);
CK_DLL_SFUN(gwindow_get_mouse_left_button_release);
CK_DLL_SFUN(gwindow_get_mouse_right_button_release);

// custom cursor (not working)
CK_DLL_SFUN(gwindow_set_mouse_cursor);
CK_DLL_SFUN(gwindow_revert_mouse_cursor);

// keyboard
CK_DLL_SFUN(gwindow_get_kb);
CK_DLL_SFUN(gwindow_get_kb_pressed);
CK_DLL_SFUN(gwindow_get_kb_released);
CK_DLL_SFUN(gwindow_get_kb_all);
CK_DLL_SFUN(gwindow_get_kb_pressed_all);
CK_DLL_SFUN(gwindow_get_kb_released_all);
CK_DLL_SFUN(gwindow_get_kb_all_with_arr);
CK_DLL_SFUN(gwindow_get_kb_pressed_all_with_arr);
CK_DLL_SFUN(gwindow_get_kb_released_all_with_arr);

void ulib_window_query(Chuck_DL_Query* QUERY)
{
    // BEGIN_CLASS("MonitorInfo", "Object");
    // DOC_CLASS(
    //   "Information about the monitor the window is on. Do not instantiate this "
    //   "class directly, use Gwindow.monitorInfo() instead. This data is NOT "
    //   "updated in real-time, it is only accurate at the time of the call. If "
    //   "the user drags the window to another monitor, you will need to call "
    //   "GWindow.monitorInfo() again to get the updated information.");
    // monitor_info_width_offset = MVAR("int", "width", false);
    // DOC_VAR("width of the monitor in screen coordinates");
    // monitor_info_height_offset = MVAR("int", "height", false);
    // DOC_VAR("height of the monitor in screen coordinates");
    // monitor_info_refresh_rate_offset = MVAR("int", "refreshRate", false);
    // DOC_VAR("refresh rate of the monitor in Hz");
    // monitor_info_virtual_pos_offset = MVAR("vec2", "virtualPos", false);
    // DOC_VAR("position of the monitor on the virtual desktop, in screen coordinates");
    // monitor_info_physical_size_mm_offset = MVAR("vec2", "physicalSize", false);
    // DOC_VAR("physical size of the monitor in millimeters");
    // monitor_info_content_scale_offset = MVAR("vec2", "contentScale", false);
    // DOC_VAR(
    //   "The content scale is the ratio between the current DPI and the "
    //   "platform's default DPI.");
    // monitor_info_work_area_offset = MVAR("vec4", "workArea", false);
    // DOC_VAR(
    //   " The work area is the area of the monitor not occluded by the taskbar "
    //   "or other system UI elements. This data is returned as vec4 with "
    //   "following values: @(xpos, ypos, width, height), in screen coordinates ");
    // monitor_info_name_offset = MVAR("string", "name", false);
    // DOC_VAR("human-readable name of the monitor");
    // END_CLASS(); // MonitorInfo

    // Events ========================================================
    BEGIN_CLASS(CHUGL_EventTypeNames[WINDOW_RESIZE], "Event");
    DOC_CLASS(
      "Event triggered whenever the ChuGL window is resized, either by the "
      "user or programmatically."
      "Don't instantiate directly, use GWindow.resizeEvent() instead");
    END_CLASS(); // WindowResizeEvent

    BEGIN_CLASS(CHUGL_EventTypeNames[WINDOW_CLOSE], "Event");
    DOC_CLASS(
      "Event triggered whenever the user attempts to close the ChuGL window, "
      "useful if GWindow.closeable(false) has been set and you need to "
      "do cleanup (save game, flush logs, etc.) before exiting."
      "Don't instantiate directly, use GWindow.closeEvent() instead");
    END_CLASS(); // WindowCloseEvent

    BEGIN_CLASS(CHUGL_EventTypeNames[CONTENT_SCALE], "Event");
    DOC_CLASS(
      "Event triggered whenever the content scale of the window changes."
      "The content scale is the ratio between the current DPI and the "
      "platform's default DPI."
      "Don't instantiate directly, use GWindow.contentScaleEvent() instead");
    END_CLASS(); // ContentScaleChangedEvent

    // GWindow ========================================================
    BEGIN_CLASS("GWindow", "Object");
    DOC_CLASS("All the properties and methods for window management");
    ADD_EX("basic/gwindow.ck");

    // monitor --------------------------------------------------------
    // SFUN(gwindow_monitor_info, "MonitorInfo", "monitorInfo");
    // DOC_FUNC("Get information about the monitor the window is on");

    // Key Enums ------------------------------------------------------
    static t_CKINT glfw_key_space         = GLFW_KEY_SPACE;
    static t_CKINT glfw_key_apostrophe    = GLFW_KEY_APOSTROPHE;
    static t_CKINT glfw_key_comma         = GLFW_KEY_COMMA;
    static t_CKINT glfw_key_minus         = GLFW_KEY_MINUS;
    static t_CKINT glfw_key_period        = GLFW_KEY_PERIOD;
    static t_CKINT glfw_key_slash         = GLFW_KEY_SLASH;
    static t_CKINT glfw_key_0             = GLFW_KEY_0;
    static t_CKINT glfw_key_1             = GLFW_KEY_1;
    static t_CKINT glfw_key_2             = GLFW_KEY_2;
    static t_CKINT glfw_key_3             = GLFW_KEY_3;
    static t_CKINT glfw_key_4             = GLFW_KEY_4;
    static t_CKINT glfw_key_5             = GLFW_KEY_5;
    static t_CKINT glfw_key_6             = GLFW_KEY_6;
    static t_CKINT glfw_key_7             = GLFW_KEY_7;
    static t_CKINT glfw_key_8             = GLFW_KEY_8;
    static t_CKINT glfw_key_9             = GLFW_KEY_9;
    static t_CKINT glfw_key_semicolon     = GLFW_KEY_SEMICOLON;
    static t_CKINT glfw_key_equal         = GLFW_KEY_EQUAL;
    static t_CKINT glfw_key_a             = GLFW_KEY_A;
    static t_CKINT glfw_key_b             = GLFW_KEY_B;
    static t_CKINT glfw_key_c             = GLFW_KEY_C;
    static t_CKINT glfw_key_d             = GLFW_KEY_D;
    static t_CKINT glfw_key_e             = GLFW_KEY_E;
    static t_CKINT glfw_key_f             = GLFW_KEY_F;
    static t_CKINT glfw_key_g             = GLFW_KEY_G;
    static t_CKINT glfw_key_h             = GLFW_KEY_H;
    static t_CKINT glfw_key_i             = GLFW_KEY_I;
    static t_CKINT glfw_key_j             = GLFW_KEY_J;
    static t_CKINT glfw_key_k             = GLFW_KEY_K;
    static t_CKINT glfw_key_l             = GLFW_KEY_L;
    static t_CKINT glfw_key_m             = GLFW_KEY_M;
    static t_CKINT glfw_key_n             = GLFW_KEY_N;
    static t_CKINT glfw_key_o             = GLFW_KEY_O;
    static t_CKINT glfw_key_p             = GLFW_KEY_P;
    static t_CKINT glfw_key_q             = GLFW_KEY_Q;
    static t_CKINT glfw_key_r             = GLFW_KEY_R;
    static t_CKINT glfw_key_s             = GLFW_KEY_S;
    static t_CKINT glfw_key_t             = GLFW_KEY_T;
    static t_CKINT glfw_key_u             = GLFW_KEY_U;
    static t_CKINT glfw_key_v             = GLFW_KEY_V;
    static t_CKINT glfw_key_w             = GLFW_KEY_W;
    static t_CKINT glfw_key_x             = GLFW_KEY_X;
    static t_CKINT glfw_key_y             = GLFW_KEY_Y;
    static t_CKINT glfw_key_z             = GLFW_KEY_Z;
    static t_CKINT glfw_key_left_bracket  = GLFW_KEY_LEFT_BRACKET;
    static t_CKINT glfw_key_backslash     = GLFW_KEY_BACKSLASH;
    static t_CKINT glfw_key_right_bracket = GLFW_KEY_RIGHT_BRACKET;
    static t_CKINT glfw_key_grave_accent  = GLFW_KEY_GRAVE_ACCENT;
    static t_CKINT glfw_key_world_1       = GLFW_KEY_WORLD_1;
    static t_CKINT glfw_key_world_2       = GLFW_KEY_WORLD_2;
    static t_CKINT glfw_key_escape        = GLFW_KEY_ESCAPE;
    static t_CKINT glfw_key_enter         = GLFW_KEY_ENTER;
    static t_CKINT glfw_key_tab           = GLFW_KEY_TAB;
    static t_CKINT glfw_key_backspace     = GLFW_KEY_BACKSPACE;
    static t_CKINT glfw_key_insert        = GLFW_KEY_INSERT;
    static t_CKINT glfw_key_delete        = GLFW_KEY_DELETE;
    static t_CKINT glfw_key_right         = GLFW_KEY_RIGHT;
    static t_CKINT glfw_key_left          = GLFW_KEY_LEFT;
    static t_CKINT glfw_key_down          = GLFW_KEY_DOWN;
    static t_CKINT glfw_key_up            = GLFW_KEY_UP;
    static t_CKINT glfw_key_page_up       = GLFW_KEY_PAGE_UP;
    static t_CKINT glfw_key_page_down     = GLFW_KEY_PAGE_DOWN;
    static t_CKINT glfw_key_home          = GLFW_KEY_HOME;
    static t_CKINT glfw_key_end           = GLFW_KEY_END;
    static t_CKINT glfw_key_caps_lock     = GLFW_KEY_CAPS_LOCK;
    static t_CKINT glfw_key_scroll_lock   = GLFW_KEY_SCROLL_LOCK;
    static t_CKINT glfw_key_num_lock      = GLFW_KEY_NUM_LOCK;
    static t_CKINT glfw_key_print_screen  = GLFW_KEY_PRINT_SCREEN;
    static t_CKINT glfw_key_pause         = GLFW_KEY_PAUSE;
    static t_CKINT glfw_key_f1            = GLFW_KEY_F1;
    static t_CKINT glfw_key_f2            = GLFW_KEY_F2;
    static t_CKINT glfw_key_f3            = GLFW_KEY_F3;
    static t_CKINT glfw_key_f4            = GLFW_KEY_F4;
    static t_CKINT glfw_key_f5            = GLFW_KEY_F5;
    static t_CKINT glfw_key_f6            = GLFW_KEY_F6;
    static t_CKINT glfw_key_f7            = GLFW_KEY_F7;
    static t_CKINT glfw_key_f8            = GLFW_KEY_F8;
    static t_CKINT glfw_key_f9            = GLFW_KEY_F9;
    static t_CKINT glfw_key_f10           = GLFW_KEY_F10;
    static t_CKINT glfw_key_f11           = GLFW_KEY_F11;
    static t_CKINT glfw_key_f12           = GLFW_KEY_F12;
    static t_CKINT glfw_key_f13           = GLFW_KEY_F13;
    static t_CKINT glfw_key_f14           = GLFW_KEY_F14;
    static t_CKINT glfw_key_f15           = GLFW_KEY_F15;
    static t_CKINT glfw_key_f16           = GLFW_KEY_F16;
    static t_CKINT glfw_key_f17           = GLFW_KEY_F17;
    static t_CKINT glfw_key_f18           = GLFW_KEY_F18;
    static t_CKINT glfw_key_f19           = GLFW_KEY_F19;
    static t_CKINT glfw_key_f20           = GLFW_KEY_F20;
    static t_CKINT glfw_key_f21           = GLFW_KEY_F21;
    static t_CKINT glfw_key_f22           = GLFW_KEY_F22;
    static t_CKINT glfw_key_f23           = GLFW_KEY_F23;
    static t_CKINT glfw_key_f24           = GLFW_KEY_F24;
    static t_CKINT glfw_key_f25           = GLFW_KEY_F25;
    static t_CKINT glfw_key_kp_0          = GLFW_KEY_KP_0;
    static t_CKINT glfw_key_kp_1          = GLFW_KEY_KP_1;
    static t_CKINT glfw_key_kp_2          = GLFW_KEY_KP_2;
    static t_CKINT glfw_key_kp_3          = GLFW_KEY_KP_3;
    static t_CKINT glfw_key_kp_4          = GLFW_KEY_KP_4;
    static t_CKINT glfw_key_kp_5          = GLFW_KEY_KP_5;
    static t_CKINT glfw_key_kp_6          = GLFW_KEY_KP_6;
    static t_CKINT glfw_key_kp_7          = GLFW_KEY_KP_7;
    static t_CKINT glfw_key_kp_8          = GLFW_KEY_KP_8;
    static t_CKINT glfw_key_kp_9          = GLFW_KEY_KP_9;
    static t_CKINT glfw_key_kp_decimal    = GLFW_KEY_KP_DECIMAL;
    static t_CKINT glfw_key_kp_divide     = GLFW_KEY_KP_DIVIDE;
    static t_CKINT glfw_key_kp_multiply   = GLFW_KEY_KP_MULTIPLY;
    static t_CKINT glfw_key_kp_subtract   = GLFW_KEY_KP_SUBTRACT;
    static t_CKINT glfw_key_kp_add        = GLFW_KEY_KP_ADD;
    static t_CKINT glfw_key_kp_enter      = GLFW_KEY_KP_ENTER;
    static t_CKINT glfw_key_kp_equal      = GLFW_KEY_KP_EQUAL;
    static t_CKINT glfw_key_left_shift    = GLFW_KEY_LEFT_SHIFT;
    static t_CKINT glfw_key_left_control  = GLFW_KEY_LEFT_CONTROL;
    static t_CKINT glfw_key_left_alt      = GLFW_KEY_LEFT_ALT;
    static t_CKINT glfw_key_left_super    = GLFW_KEY_LEFT_SUPER;
    static t_CKINT glfw_key_right_shift   = GLFW_KEY_RIGHT_SHIFT;
    static t_CKINT glfw_key_right_control = GLFW_KEY_RIGHT_CONTROL;
    static t_CKINT glfw_key_right_alt     = GLFW_KEY_RIGHT_ALT;
    static t_CKINT glfw_key_right_super   = GLFW_KEY_RIGHT_SUPER;
    static t_CKINT glfw_key_menu          = GLFW_KEY_MENU;
    SVAR("int", "Key_Space", &glfw_key_space);
    SVAR("int", "Key_Apostrophe", &glfw_key_apostrophe);
    SVAR("int", "Key_Comma", &glfw_key_comma);
    SVAR("int", "Key_Minus", &glfw_key_minus);
    SVAR("int", "Key_Period", &glfw_key_period);
    SVAR("int", "Key_Slash", &glfw_key_slash);
    SVAR("int", "Key_0", &glfw_key_0);
    SVAR("int", "Key_1", &glfw_key_1);
    SVAR("int", "Key_2", &glfw_key_2);
    SVAR("int", "Key_3", &glfw_key_3);
    SVAR("int", "Key_4", &glfw_key_4);
    SVAR("int", "Key_5", &glfw_key_5);
    SVAR("int", "Key_6", &glfw_key_6);
    SVAR("int", "Key_7", &glfw_key_7);
    SVAR("int", "Key_8", &glfw_key_8);
    SVAR("int", "Key_9", &glfw_key_9);
    SVAR("int", "Key_Semicolon", &glfw_key_semicolon);
    SVAR("int", "Key_Equal", &glfw_key_equal);
    SVAR("int", "Key_A", &glfw_key_a);
    SVAR("int", "Key_B", &glfw_key_b);
    SVAR("int", "Key_C", &glfw_key_c);
    SVAR("int", "Key_D", &glfw_key_d);
    SVAR("int", "Key_E", &glfw_key_e);
    SVAR("int", "Key_F", &glfw_key_f);
    SVAR("int", "Key_G", &glfw_key_g);
    SVAR("int", "Key_H", &glfw_key_h);
    SVAR("int", "Key_I", &glfw_key_i);
    SVAR("int", "Key_J", &glfw_key_j);
    SVAR("int", "Key_K", &glfw_key_k);
    SVAR("int", "Key_L", &glfw_key_l);
    SVAR("int", "Key_M", &glfw_key_m);
    SVAR("int", "Key_N", &glfw_key_n);
    SVAR("int", "Key_O", &glfw_key_o);
    SVAR("int", "Key_P", &glfw_key_p);
    SVAR("int", "Key_Q", &glfw_key_q);
    SVAR("int", "Key_R", &glfw_key_r);
    SVAR("int", "Key_S", &glfw_key_s);
    SVAR("int", "Key_T", &glfw_key_t);
    SVAR("int", "Key_U", &glfw_key_u);
    SVAR("int", "Key_V", &glfw_key_v);
    SVAR("int", "Key_W", &glfw_key_w);
    SVAR("int", "Key_X", &glfw_key_x);
    SVAR("int", "Key_Y", &glfw_key_y);
    SVAR("int", "Key_Z", &glfw_key_z);
    SVAR("int", "Key_LeftBracket", &glfw_key_left_bracket);
    SVAR("int", "Key_Backslash", &glfw_key_backslash);
    SVAR("int", "Key_RightBracket", &glfw_key_right_bracket);
    SVAR("int", "Key_GraveAccent", &glfw_key_grave_accent);
    SVAR("int", "Key_World1", &glfw_key_world_1);
    SVAR("int", "Key_World2", &glfw_key_world_2);
    SVAR("int", "Key_Escape", &glfw_key_escape);
    SVAR("int", "Key_Enter", &glfw_key_enter);
    SVAR("int", "Key_Tab", &glfw_key_tab);
    SVAR("int", "Key_Backspace", &glfw_key_backspace);
    SVAR("int", "Key_Insert", &glfw_key_insert);
    SVAR("int", "Key_Delete", &glfw_key_delete);
    SVAR("int", "Key_Right", &glfw_key_right);
    SVAR("int", "Key_Left", &glfw_key_left);
    SVAR("int", "Key_Down", &glfw_key_down);
    SVAR("int", "Key_Up", &glfw_key_up);
    SVAR("int", "Key_PageUp", &glfw_key_page_up);
    SVAR("int", "Key_PageDown", &glfw_key_page_down);
    SVAR("int", "Key_Home", &glfw_key_home);
    SVAR("int", "Key_End", &glfw_key_end);
    SVAR("int", "Key_CapsLock", &glfw_key_caps_lock);
    SVAR("int", "Key_ScrollLock", &glfw_key_scroll_lock);
    SVAR("int", "Key_NumLock", &glfw_key_num_lock);
    SVAR("int", "Key_PrintScreen", &glfw_key_print_screen);
    SVAR("int", "Key_Pause", &glfw_key_pause);
    SVAR("int", "Key_F1", &glfw_key_f1);
    SVAR("int", "Key_F2", &glfw_key_f2);
    SVAR("int", "Key_F3", &glfw_key_f3);
    SVAR("int", "Key_F4", &glfw_key_f4);
    SVAR("int", "Key_F5", &glfw_key_f5);
    SVAR("int", "Key_F6", &glfw_key_f6);
    SVAR("int", "Key_F7", &glfw_key_f7);
    SVAR("int", "Key_F8", &glfw_key_f8);
    SVAR("int", "Key_F9", &glfw_key_f9);
    SVAR("int", "Key_F10", &glfw_key_f10);
    SVAR("int", "Key_F11", &glfw_key_f11);
    SVAR("int", "Key_F12", &glfw_key_f12);
    SVAR("int", "Key_F13", &glfw_key_f13);
    SVAR("int", "Key_F14", &glfw_key_f14);
    SVAR("int", "Key_F15", &glfw_key_f15);
    SVAR("int", "Key_F16", &glfw_key_f16);
    SVAR("int", "Key_F17", &glfw_key_f17);
    SVAR("int", "Key_F18", &glfw_key_f18);
    SVAR("int", "Key_F19", &glfw_key_f19);
    SVAR("int", "Key_F20", &glfw_key_f20);
    SVAR("int", "Key_F21", &glfw_key_f21);
    SVAR("int", "Key_F22", &glfw_key_f22);
    SVAR("int", "Key_F23", &glfw_key_f23);
    SVAR("int", "Key_F24", &glfw_key_f24);
    SVAR("int", "Key_F25", &glfw_key_f25);
    SVAR("int", "Keypad_0", &glfw_key_kp_0);
    SVAR("int", "Keypad_1", &glfw_key_kp_1);
    SVAR("int", "Keypad_2", &glfw_key_kp_2);
    SVAR("int", "Keypad_3", &glfw_key_kp_3);
    SVAR("int", "Keypad_4", &glfw_key_kp_4);
    SVAR("int", "Keypad_5", &glfw_key_kp_5);
    SVAR("int", "Keypad_6", &glfw_key_kp_6);
    SVAR("int", "Keypad_7", &glfw_key_kp_7);
    SVAR("int", "Keypad_8", &glfw_key_kp_8);
    SVAR("int", "Keypad_9", &glfw_key_kp_9);
    SVAR("int", "Keypad_Decimal", &glfw_key_kp_decimal);
    SVAR("int", "Keypad_Divide", &glfw_key_kp_divide);
    SVAR("int", "Keypad_Multiply", &glfw_key_kp_multiply);
    SVAR("int", "Keypad_Subtract", &glfw_key_kp_subtract);
    SVAR("int", "Keypad_Add", &glfw_key_kp_add);
    SVAR("int", "Keypad_Enter", &glfw_key_kp_enter);
    SVAR("int", "Keypad_Equal", &glfw_key_kp_equal);
    SVAR("int", "Key_LeftShift", &glfw_key_left_shift);
    SVAR("int", "Key_LeftControl", &glfw_key_left_control);
    SVAR("int", "Key_LeftAlt", &glfw_key_left_alt);
    SVAR("int", "Key_LeftSuper", &glfw_key_left_super);
    SVAR("int", "Key_RightShift", &glfw_key_right_shift);
    SVAR("int", "Key_RightControl", &glfw_key_right_control);
    SVAR("int", "Key_RightAlt", &glfw_key_right_alt);
    SVAR("int", "Key_RightSuper", &glfw_key_right_super);
    SVAR("int", "Key_Menu", &glfw_key_menu);

    // callbacks ------------------------------------------------------
    SFUN(gwindow_close_event, CHUGL_EventTypeNames[WINDOW_CLOSE], "closeEvent");
    DOC_FUNC(
      "Returns the event triggered whenever the user attempts to close this ChuGL "
      "window");

    SFUN(gwindow_window_resize_event, CHUGL_EventTypeNames[WINDOW_RESIZE],
         "resizeEvent");
    DOC_FUNC(
      "Returns the event triggered whenever the ChuGL window is resized, either by the "
      "user or programmatically");

    SFUN(gwindow_window_content_scale_event, "Event", "contentScaleEvent");
    DOC_FUNC(
      "Returns the event triggered whenever the content scale of the window changes."
      "The content scale is the ratio between the current DPI and the "
      "platform's default DPI.");

    // closing --------------------------------------------------------
    SFUN(gwindow_set_closeable, "void", "closeable");
    ARG("int", "closeable");
    DOC_FUNC("Enable or disable the window's close button");

    SFUN(gwindow_get_closeable, "int", "closeable");
    DOC_FUNC("Get the current state of the window's close button");

    SFUN(gwindow_close, "void", "close");
    DOC_FUNC("Close the window");

    // size -----------------------------------------------------------
    SFUN(gwindow_fullscreen, "void", "fullscreen");
    DOC_FUNC("Set the window to fullscreen mode");

    SFUN(gwindow_fullscreen_width_height, "void", "fullscreen");
    ARG("int", "width");
    ARG("int", "height");
    DOC_FUNC("Set the window to fullscreen mode with the specified resolution");

    SFUN(gwindow_maximize, "void", "maximize");
    DOC_FUNC("Set the window to windowed fullscreen mode");

    SFUN(gwindow_windowed, "void", "windowed");
    ARG("int", "width");
    ARG("int", "height");
    DOC_FUNC("Set the window to windowed mode with the specified width and height");

    SFUN(gwindow_windowed_prev_size, "void", "windowed");
    DOC_FUNC(
      "Set the window to windowed mode with the last size before going fullscreen. "
      "Default 1280 x 960");

    SFUN(gwindow_get_window_size, "vec2", "windowSize");
    DOC_FUNC("Get the window size in screen coordinates");

    SFUN(gwindow_get_framebuffer_size, "vec2", "framebufferSize");
    DOC_FUNC("Get the framebuffer size in pixels");

    // content scale --------------------------------------------------
    SFUN(gwindow_get_content_scale, "vec2", "contentScale");
    DOC_FUNC("Get the content scale of current monitor");

    // size limits ----------------------------------------------------
    SFUN(gwindow_set_window_size_limits, "void", "sizeLimits");
    ARG("int", "minWidth");
    ARG("int", "minHeight");
    ARG("int", "maxWidth");
    ARG("int", "maxHeight");
    ARG("vec2", "aspectRatio");
    DOC_FUNC(
      "Set the window size limits, including min/max size and aspect ratio."
      "To disable a limit, pass 0 for the corresponding argument, or @(0, 0)"
      "to disable fixed aspect ratio");

    // position -------------------------------------------------------
    SFUN(gwindow_set_window_pos, "void", "position");
    ARG("int", "x");
    ARG("int", "y");
    DOC_FUNC("Set the window position in screen coordinates");

    SFUN(gwindow_window_center, "void", "center");
    DOC_FUNC("Center the window on its current monitor");

    // title ----------------------------------------------------------
    SFUN(gwindow_set_title, "void", "title");
    ARG("string", "title");
    DOC_FUNC("Set the window title");

    // iconify --------------------------------------------------------
    SFUN(gwindow_inconify, "void", "iconify");
    DOC_FUNC("Iconify the window");

    SFUN(gwindow_restore, "void", "restore");
    DOC_FUNC("Restore the window from iconified state");

    // attributes -----------------------------------------------------
    SFUN(gwindow_set_attrib_resizable, "void", "resizable");
    ARG("int", "resizable");
    DOC_FUNC(
      "Set the window resizable attribute. Must call before GG.nextFrame(). "
      "Default is true.");

    SFUN(gwindow_set_attrib_decorated, "void", "decorated");
    ARG("int", "decorated");
    DOC_FUNC(
      "Set whether the window has decorations such as a border, a close "
      "widget, etc."
      "Must call before GG.nextFrame(). Default is true.");

    SFUN(gwindow_set_attrib_floating, "void", "floating");
    ARG("int", "floating");
    DOC_FUNC(
      "set whether the specified window is floating, also called topmost "
      "or always-on-top. Must call before GG.nextFrame(). Default is false.");

    SFUN(gwindow_set_attrib_transparent, "void", "transparent");
    ARG("int", "transparent");
    DOC_FUNC(
      "set whether the specified window has a transparent framebuffer, "
      "i.e. the window contents is composited with the background using the "
      "window framebuffer alpha channel."
      "Not supported on all platforms. To enable, call GWindow.transparent() "
      "BEFORE GG.nextFrame() is ever called."
      "If platform supports it, you can change opacity via "
      "GWindow.opacity(float)");

    SFUN(gwindow_opacity, "void", "opacity");
    ARG("float", "opacity");
    DOC_FUNC(
      "Set the window opacity, 0.0 is fully transparent, 1.0 is fully opaque."
      "only works if GWindow.transparent() has been called before "
      "GG.nextFrame()"
      "AND the platform supports transparent framebuffers.");

    // mouse ----------------------------------------------------------
    SFUN(gwindow_get_mouse_pos, "vec2", "mousePos");
    DOC_FUNC("Get the current mouse position in screen coordinates");

    SFUN(gwindow_get_mouse_delta_pos, "vec2", "mouseDeltaPos");
    DOC_FUNC("Get the change in mouse position since the last call");

    SVAR("int", "MouseMode_Normal", &mouse_mode_normal);
    DOC_VAR(
      "Normal mouse mode, the cursor is visible and behaves normally. Set via "
      "GWindow.mouseMode()");
    SVAR("int", "MouseMode_Hidden", &mouse_mode_hidden);
    DOC_VAR(
      "Hidden mouse mode, hides the cursor when it is focused and hovering "
      "over the window, but does not lock it to the window. Set via "
      "GWindow.mouseMode(). May not be supported on all platforms.");
    SVAR("int", "MouseMode_Disabled", &mouse_mode_disabled);
    DOC_VAR(
      "Disabled mouse mode, hides the cursor and locks it to the window, "
      "useful for first-person games. Set via GWindow.mouseMode()");

    SFUN(gwindow_set_mouse_mode, "void", "mouseMode");
    ARG("int", "mode");
    DOC_FUNC(
      "Set the mouse mode. Possible values are: GWindow.MouseMode_Normal (0) , "
      "GWindow.MouseMode_Disabled (1), and GWindow.MouseMode_Hidden (2)"
      "Normal mode is the default mode, the cursor is visible and behaves "
      "normally."
      "Disabled mode hides the cursor and locks it to the window, useful for "
      "first-person games."
      "Hidden mode hides the cursor when it is focused and hovering over the "
      "window, but does not lock it to the window.");

    SFUN(gwindow_get_mouse_mode, "int", "mouseMode");
    DOC_FUNC(
      "Get the current mouse mode. Possible values are: GWindow.MouseMode_Normal (0) , "
      "GWindow.MouseMode_Disabled (1), and GWindow.MouseMode_Hidden (2)"
      "Normal mode is the default mode, the cursor is visible and behaves "
      "normally."
      "Disabled mode hides the cursor and locks it to the window, useful for "
      "first-person games."
      "Hidden mode hides the cursor when it is focused and hovering over the "
      "window, but does not lock it to the window.");

    SFUN(gwindow_get_mouse_scroll_dx, "float", "scrollX");
    DOC_FUNC("Get the horizontal scroll delta of the mouse wheel");

    SFUN(gwindow_get_mouse_scroll_dy, "float", "scrollY");
    DOC_FUNC("Get the vertical scroll delta of the mouse wheel");

    SFUN(gwindow_get_mouse_scroll, "vec2", "scroll");
    DOC_FUNC("Get the 2D scroll delta of the mouse wheel");

    SFUN(gwindow_get_mouse_left_button, "int", "mouseLeft");
    DOC_FUNC("Get the state of the left mouse button. 1 if pressed, 0 if not.");

    SFUN(gwindow_get_mouse_right_button, "int", "mouseRight");
    DOC_FUNC("Get the state of the right mouse button. 1 if pressed, 0 if not.");

    SFUN(gwindow_get_mouse_left_button_click, "int", "mouseLeftDown");
    DOC_FUNC("1 on the frame the left mouse button is pressed, 0 otherwise");

    SFUN(gwindow_get_mouse_right_button_click, "int", "mouseRightDown");
    DOC_FUNC("1 on the frame the right mouse button is pressed, 0 otherwise");

    SFUN(gwindow_get_mouse_left_button_release, "int", "mouseLeftUp");
    DOC_FUNC("1 on the frame the left mouse button is released, 0 otherwise");

    SFUN(gwindow_get_mouse_right_button_release, "int", "mouseRightUp");
    DOC_FUNC("1 on the frame the right mouse button is released, 0 otherwise");

    // not working (tried on macOS)
    // SFUN(gwindow_set_mouse_cursor, "void", "customCursor");
    // ARG("int[]", "image_data");
    // ARG("int", "width");
    // ARG("int", "height");
    // ARG("int", "xhot");
    // ARG("int", "yhot");
    // DOC_FUNC(
    //   "Create a custom mouse cursor. The image data is in RGBA. The pixels are "
    //   "arranged canonically as sequential rows, starting from the top-left "
    //   "corner. Each consecutive 4 values represent the red, green, blue, and "
    //   "alpha value for a single pixel. Width and height are the dimensions of "
    //   "the image in pixels. image_data.size() MUST EQUAL width * height * 4."
    //   "each value must be an int in the range 0-255."
    //   "xhot and yhot are the coordinates of the cursor's hot spot, the point "
    //   "within the cursor image that corresponds to the mouse position. Most"
    //   "commonly, this is the top-left corner of the image 0, 0.");

    // SFUN(gwindow_revert_mouse_cursor, "void", "normalCursor");
    // DOC_FUNC("Revert to the default mouse cursor");

    // keyboard -------------------------------------------------------

    SFUN(gwindow_get_kb, "int", "key");
    ARG("int", "key");
    DOC_FUNC(
      "Get the whether the specified key is being held down. Use the GWindow key "
      "constants for param `key`, e.g. GWindow.key(GWindow.Key_Space)");

    SFUN(gwindow_get_kb_pressed, "int", "keyDown");
    ARG("int", "key");
    DOC_FUNC(
      "Get the whether the specified key was pressed this frame. Use the GWindow key "
      "constants for param `key`, e.g. GWindow.keyDown(GWindow.Key_Space)");

    SFUN(gwindow_get_kb_released, "int", "keyUp");
    ARG("int", "key");
    DOC_FUNC(
      "Get the whether the specified key was released this frame. Use the GWindow key "
      "constants for param `key`, e.g. GWindow.keyUp(GWindow.Key_Space)");

    SFUN(gwindow_get_kb_all, "int[]", "keys");
    DOC_FUNC(
      "Returns an array with all keys held down on this frame. "
      "Each value will be a key enum, e.g. GWindow.Key_Space");

    SFUN(gwindow_get_kb_pressed_all, "int[]", "keysDown");
    DOC_FUNC(
      "Returns an array with all keys pressed on this frame. "
      "Each value will be a key enum, e.g. GWindow.Key_Space");

    SFUN(gwindow_get_kb_released_all, "int[]", "keysUp");
    DOC_FUNC(
      "Returns an array with all keys released on this frame. "
      "Each value will be a key enum, e.g. GWindow.Key_Space");

    SFUN(gwindow_get_kb_all_with_arr, "void", "keys");
    ARG("int[]", "keys");
    DOC_FUNC(
      "Populates the array with all keys held down on this frame. "
      "Each value will be a key enum, e.g. GWindow.Key_Space");

    SFUN(gwindow_get_kb_pressed_all_with_arr, "void", "keysDown");
    ARG("int[]", "keys");
    DOC_FUNC(
      "Populates the array with all keys pressed on this frame. "
      "Each value will be a key enum, e.g. GWindow.Key_Space");

    SFUN(gwindow_get_kb_released_all_with_arr, "void", "keysUp");
    ARG("int[]", "keys");
    DOC_FUNC(
      "Populates the array with all keys released on this frame. "
      "Each value will be a key enum, e.g. GWindow.Key_Space");

    END_CLASS(); // GWindow
}

CK_DLL_SFUN(gwindow_monitor_info)
{
    // how to get app->window from here?
    // probably best to have windowPos callback listener, detect
    // when window changes monitors, and update the monitor info
    // in the ChuGL_Window struct
    // audiothread just reads from the struct
}

// ============================================================================
// callbacks
// ============================================================================

CK_DLL_SFUN(gwindow_close_event)
{
    RETURN->v_object = (Chuck_Object*)Event_Get(CHUGL_EventType::WINDOW_CLOSE, API, VM);
}

CK_DLL_SFUN(gwindow_window_resize_event)
{
    RETURN->v_object
      = (Chuck_Object*)Event_Get(CHUGL_EventType::WINDOW_RESIZE, API, VM);
}

CK_DLL_SFUN(gwindow_window_content_scale_event)
{
    RETURN->v_object
      = (Chuck_Object*)Event_Get(CHUGL_EventType::CONTENT_SCALE, API, VM);
}

// ============================================================================
// closing
// ============================================================================

CK_DLL_SFUN(gwindow_set_closeable)
{
    CHUGL_Window_Closeable(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(gwindow_get_closeable)
{
    RETURN->v_int = CHUGL_Window_Closeable();
}

CK_DLL_SFUN(gwindow_close)
{
    // https://www.glfw.org/docs/3.3/intro_guide.html#thread_safety
    // window creation and destruction, and event processing, are not
    // thread-safe.
    CQ_PushCommand_WindowClose();
}

// ============================================================================
// window size
// ============================================================================

CK_DLL_SFUN(gwindow_fullscreen)
{
    CQ_PushCommand_WindowMode(SG_WINDOW_MODE_FULLSCREEN, 0, 0);
}

CK_DLL_SFUN(gwindow_fullscreen_width_height)
{
    t_CKINT width  = GET_NEXT_INT(ARGS);
    t_CKINT height = GET_NEXT_INT(ARGS);
    CQ_PushCommand_WindowMode(SG_WINDOW_MODE_FULLSCREEN, width, height);
}

CK_DLL_SFUN(gwindow_windowed)
{
    t_CKINT width  = GET_NEXT_INT(ARGS);
    t_CKINT height = GET_NEXT_INT(ARGS);
    CQ_PushCommand_WindowMode(SG_WINDOW_MODE_WINDOWED, width, height);
}

CK_DLL_SFUN(gwindow_windowed_prev_size)
{
    CQ_PushCommand_WindowMode(SG_WINDOW_MODE_WINDOWED, -1, -1);
}

CK_DLL_SFUN(gwindow_maximize)
{
    CQ_PushCommand_WindowMode(SG_WINDOW_MODE_WINDOWED_FULLSCREEN, 0, 0);
}

CK_DLL_SFUN(gwindow_get_window_size)
{
    RETURN->v_vec2 = CHUGL_Window_WindowSize();
}

CK_DLL_SFUN(gwindow_get_framebuffer_size)
{
    RETURN->v_vec2 = CHUGL_Window_FramebufferSize();
}

// ============================================================================
// content scale
// ============================================================================

CK_DLL_SFUN(gwindow_get_content_scale)
{
    RETURN->v_vec2 = CHUGL_Window_ContentScale();
}

// ============================================================================
// size limits
// ============================================================================

CK_DLL_SFUN(gwindow_set_window_size_limits)
{
    t_CKINT min_width     = GET_NEXT_INT(ARGS);
    t_CKINT min_height    = GET_NEXT_INT(ARGS);
    t_CKINT max_width     = GET_NEXT_INT(ARGS);
    t_CKINT max_height    = GET_NEXT_INT(ARGS);
    t_CKVEC2 aspect_ratio = GET_NEXT_VEC2(ARGS);

    CQ_PushCommand_WindowSizeLimits(min_width, min_height, max_width, max_height,
                                    (int)aspect_ratio.x, (int)aspect_ratio.y);
}

// ============================================================================
// position
// ============================================================================

CK_DLL_SFUN(gwindow_set_window_pos)
{
    t_CKINT x = GET_NEXT_INT(ARGS);
    t_CKINT y = GET_NEXT_INT(ARGS);
    CQ_PushCommand_WindowPosition(x, y);
}

CK_DLL_SFUN(gwindow_window_center)
{
    CQ_PushCommand_WindowCenter();
}

// ============================================================================
// title
// ============================================================================

CK_DLL_SFUN(gwindow_set_title)
{
    CQ_PushCommand_WindowTitle(API->object->str(GET_NEXT_STRING(ARGS)));
}

// ============================================================================
// iconify
// ============================================================================

CK_DLL_SFUN(gwindow_inconify)
{
    CQ_PushCommand_WindowIconify(true);
}

CK_DLL_SFUN(gwindow_restore)
{
    CQ_PushCommand_WindowIconify(false);
}

// ============================================================================
// attributes
// ============================================================================

CK_DLL_SFUN(gwindow_set_attrib_resizable)
{
    // CQ_PushCommand_WindowAttribute(CHUGL_WINDOW_ATTRIB_RESIZABLE,
    //                                GET_NEXT_INT(ARGS));
    CHUGL_Window_Resizable(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(gwindow_set_attrib_decorated)
{
    // CQ_PushCommand_WindowAttribute(CHUGL_WINDOW_ATTRIB_DECORATED,
    //                                GET_NEXT_INT(ARGS));
    CHUGL_Window_Decorated(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(gwindow_set_attrib_floating)
{
    // CQ_PushCommand_WindowAttribute(CHUGL_WINDOW_ATTRIB_FLOATING,
    //                                GET_NEXT_INT(ARGS));
    CHUGL_Window_Floating(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(gwindow_set_attrib_transparent)
{
    CHUGL_Window_Transparent(GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(gwindow_opacity)
{
    CQ_PushCommand_WindowOpacity(GET_NEXT_FLOAT(ARGS));
}

// ============================================================================
// mouse
// ============================================================================

CK_DLL_SFUN(gwindow_get_mouse_pos)
{
    RETURN->v_vec2 = CHUGL_Mouse_Position();
}

CK_DLL_SFUN(gwindow_get_mouse_delta_pos)
{
    RETURN->v_vec2 = CHUGL_Mouse_Delta();
}

CK_DLL_SFUN(gwindow_set_mouse_mode)
{
    mouse_mode_current = GET_NEXT_INT(ARGS);
    CQ_PushCommand_MouseMode(mouse_mode_current);
}

CK_DLL_SFUN(gwindow_get_mouse_mode)
{
    RETURN->v_int = mouse_mode_current;
}

CK_DLL_SFUN(gwindow_get_mouse_scroll_dx)
{
    RETURN->v_float = CHUGL_scroll_delta().x;
}

CK_DLL_SFUN(gwindow_get_mouse_scroll_dy)
{
    RETURN->v_float = CHUGL_scroll_delta().y;
}

CK_DLL_SFUN(gwindow_get_mouse_scroll)
{
    RETURN->v_vec2 = CHUGL_scroll_delta();
}

CK_DLL_SFUN(gwindow_get_mouse_left_button)
{
    RETURN->v_int = CHUGL_Mouse_LeftButton();
}

CK_DLL_SFUN(gwindow_get_mouse_right_button)
{
    RETURN->v_int = CHUGL_Mouse_RightButton();
}

CK_DLL_SFUN(gwindow_get_mouse_left_button_click)
{
    RETURN->v_int = CHUGL_Mouse_LeftButtonClick();
}

CK_DLL_SFUN(gwindow_get_mouse_right_button_click)
{
    RETURN->v_int = CHUGL_Mouse_RightButtonClick();
}

CK_DLL_SFUN(gwindow_get_mouse_left_button_release)
{
    RETURN->v_int = CHUGL_Mouse_LeftButtonReleased();
}

CK_DLL_SFUN(gwindow_get_mouse_right_button_release)
{
    RETURN->v_int = CHUGL_Mouse_RightButtonReleased();
}

CK_DLL_SFUN(gwindow_set_mouse_cursor)
{
    Chuck_ArrayInt* image_data = (Chuck_ArrayInt*)GET_NEXT_OBJECT(ARGS);
    t_CKINT width              = GET_NEXT_INT(ARGS);
    t_CKINT height             = GET_NEXT_INT(ARGS);
    t_CKINT xhot               = GET_NEXT_INT(ARGS);
    t_CKINT yhot               = GET_NEXT_INT(ARGS);

    int size = API->object->array_int_size(image_data);

    if (size != width * height * 4) {
        CK_LOG(1,
               "GWindow.mouseCursor() image_data.size() must equal width * "
               "height * 4. Could not set custom cursor.");
        return;
    }

    CQ_PushCommand_MouseCursor(API, image_data, width, height, xhot, yhot);
}

CK_DLL_SFUN(gwindow_revert_mouse_cursor)
{
    CQ_PushCommand_MouseCursorNormal();
}

// Keyboard -------------------------------------------------------------------

CK_DLL_SFUN(gwindow_get_kb)
{
    int key = GET_NEXT_INT(ARGS);
    if (key < 0 || key > GLFW_KEY_LAST) {
        RETURN->v_int = 0;
        return;
    }
    RETURN->v_int = CHUGL_Kb_key(key).down ? 1 : 0;
}

CK_DLL_SFUN(gwindow_get_kb_pressed)
{
    int key = GET_NEXT_INT(ARGS);
    if (key < 0 || key > GLFW_KEY_LAST) {
        RETURN->v_int = 0;
        return;
    }
    RETURN->v_int = CHUGL_Kb_key(key).pressed ? 1 : 0;
}

CK_DLL_SFUN(gwindow_get_kb_released)
{
    int key = GET_NEXT_INT(ARGS);
    if (key < 0 || key > GLFW_KEY_LAST) {
        RETURN->v_int = 0;
        return;
    }
    RETURN->v_int = CHUGL_Kb_key(key).released ? 1 : 0;
}

static void ulib_window_get_kb_all(Chuck_ArrayInt* ck_arr)
{
    ASSERT(g_chuglAPI->object->array_int_size(ck_arr) == 0);
    int keys_count = ARRAY_LENGTH(CHUGL_Kb.keys);
    u64 arena_curr = audio_frame_arena.curr;
    bool* keys     = ARENA_PUSH_COUNT(&audio_frame_arena, bool, keys_count);
    u64 size_bytes = audio_frame_arena.curr - arena_curr;

    CHUGL_Kb_copyAllKeysHeldDown(keys, size_bytes);

    for (int i = 0; i < keys_count; i++) {
        if (keys[i]) g_chuglAPI->object->array_int_push_back(ck_arr, i);
    }
}

CK_DLL_SFUN(gwindow_get_kb_all)
{
    Chuck_ArrayInt* ck_arr = (Chuck_ArrayInt*)chugin_createCkObj("int[]", false, SHRED);
    ulib_window_get_kb_all(ck_arr);
    RETURN->v_object = (Chuck_Object*)ck_arr;
}

CK_DLL_SFUN(gwindow_get_kb_all_with_arr)
{
    Chuck_ArrayInt* ck_arr = GET_NEXT_INT_ARRAY(ARGS);
    g_chuglAPI->object->array_int_clear(ck_arr);
    ulib_window_get_kb_all(ck_arr);
    RETURN->v_object = (Chuck_Object*)ck_arr;
}

static void ulib_window_get_kb_pressed_all(Chuck_ArrayInt* ck_arr)
{
    ASSERT(g_chuglAPI->object->array_int_size(ck_arr) == 0);
    int keys_count = ARRAY_LENGTH(CHUGL_Kb.keys);

    u64 arena_curr    = audio_frame_arena.curr;
    CHUGL_KbKey* keys = ARENA_PUSH_COUNT(&audio_frame_arena, CHUGL_KbKey, keys_count);
    u64 size_bytes    = audio_frame_arena.curr - arena_curr;

    CHUGL_Kb_copyAllKeysPressedReleased(keys, size_bytes);

    for (int i = 0; i < keys_count; i++) {
        if (keys[i].pressed) g_chuglAPI->object->array_int_push_back(ck_arr, i);
    }
}

CK_DLL_SFUN(gwindow_get_kb_pressed_all)
{
    Chuck_ArrayInt* ck_arr = (Chuck_ArrayInt*)chugin_createCkObj("int[]", false, SHRED);
    ulib_window_get_kb_pressed_all(ck_arr);
    RETURN->v_object = (Chuck_Object*)ck_arr;
}

CK_DLL_SFUN(gwindow_get_kb_pressed_all_with_arr)
{
    Chuck_ArrayInt* ck_arr = GET_NEXT_INT_ARRAY(ARGS);
    g_chuglAPI->object->array_int_clear(ck_arr);
    ulib_window_get_kb_pressed_all(ck_arr);
    RETURN->v_object = (Chuck_Object*)ck_arr;
}

static void ulib_window_get_kb_released_all(Chuck_ArrayInt* ck_arr)
{
    ASSERT(g_chuglAPI->object->array_int_size(ck_arr) == 0);

    int keys_count = ARRAY_LENGTH(CHUGL_Kb.keys);

    u64 arena_curr    = audio_frame_arena.curr;
    CHUGL_KbKey* keys = ARENA_PUSH_COUNT(&audio_frame_arena, CHUGL_KbKey, keys_count);
    u64 size_bytes    = audio_frame_arena.curr - arena_curr;

    CHUGL_Kb_copyAllKeysPressedReleased(keys, size_bytes);

    for (int i = 0; i < keys_count; i++) {
        if (keys[i].released) g_chuglAPI->object->array_int_push_back(ck_arr, i);
    }
}

CK_DLL_SFUN(gwindow_get_kb_released_all)
{
    Chuck_ArrayInt* ck_arr = (Chuck_ArrayInt*)chugin_createCkObj("int[]", false, SHRED);
    ulib_window_get_kb_released_all(ck_arr);
    RETURN->v_object = (Chuck_Object*)ck_arr;
}

CK_DLL_SFUN(gwindow_get_kb_released_all_with_arr)
{
    Chuck_ArrayInt* ck_arr = GET_NEXT_INT_ARRAY(ARGS);
    g_chuglAPI->object->array_int_clear(ck_arr);
    ulib_window_get_kb_released_all(ck_arr);
    RETURN->v_object = (Chuck_Object*)ck_arr;
}