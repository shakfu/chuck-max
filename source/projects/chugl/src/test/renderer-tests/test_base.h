#pragma once

#include "core/macros.h"
#include "graphics.h"

#include <glm/glm.hpp>

// base declarations for all examples

// callbacks
typedef void (*Test_OnInit)(GraphicsContext* ctx, GLFWwindow* window);
typedef void (*Test_OnUpdate)(f32 dt);
typedef void (*Test_OnRender)(glm::mat4 proj, glm::mat4 view, glm::vec3 camPos);
typedef void (*Test_OnExit)();

typedef void (*Test_OnWindowResize)(i32 width, i32 height);
typedef void (*Test_OnMouseButton)(i32 button, i32 action, i32 mods);
typedef void (*Test_OnScroll)(f64 xoffset, f64 yoffset);
typedef void (*Test_OnCursorPosition)(f64 xpos, f64 ypos);
typedef void (*Test_OnKey)(i32 key, i32 scancode, i32 action, i32 mods);

struct TestCallbacks {
    Test_OnInit onInit;
    Test_OnUpdate onUpdate;
    Test_OnRender onRender;
    Test_OnExit onExit;

    // input callbacks
    Test_OnWindowResize onWindowResize;
    Test_OnMouseButton onMouseButton;
    Test_OnScroll onScroll;
    Test_OnCursorPosition onCursorPosition;
    Test_OnKey onKey;
};

// entry point
typedef void (*TestEntryPoint)(TestCallbacks* callbacks);

// struct Example {
//     ExampleCallbacks callbacks;
//     const char* name;
// };