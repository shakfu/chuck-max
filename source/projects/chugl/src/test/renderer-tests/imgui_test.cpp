#include "test_base.h"

#include <GLFW/glfw3.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_wgpu.h>
#include <imgui/imgui.h>
#include <sokol/sokol_time.h>
#include <webgpu/webgpu.h>

static GraphicsContext* gctx = NULL;
static GLFWwindow* window    = NULL;

struct TickStats {
    u64 min   = UINT64_MAX;
    u64 max   = 0;
    u64 total = 0;

    void update(u64 ticks)
    {
        min = ticks < min ? ticks : min;
        max = ticks > max ? ticks : max;
        total += ticks;
    }
};

struct ImGui_Stats {
    u64 fc = 0;
    TickStats new_frame;
    TickStats command;
    TickStats render;
    TickStats draw;

    void update(u64 new_frame_time, u64 command_time, u64 render_time,
                u64 draw_time)
    {
        new_frame.update(new_frame_time);
        command.update(command_time);
        render.update(render_time);
        draw.update(draw_time);
        fc++;
    }

    void print()
    {
        printf("Frame count: %llu\n", fc);
        printf("New frame: min: %f, max: %f, avg: %f\n", stm_ms(new_frame.min),
               stm_ms(new_frame.max), stm_ms(new_frame.total / fc));
        printf("Command: min: %f, max: %f, avg: %f\n", stm_ms(command.min),
               stm_ms(command.max), stm_ms(command.total / fc));
        printf("Render: min: %f, max: %f, avg: %f\n", stm_ms(render.min),
               stm_ms(render.max), stm_ms(render.total / fc));
        printf("Draw: min: %f, max: %f, avg: %f\n", stm_ms(draw.min),
               stm_ms(draw.max), stm_ms(draw.total / fc));
    }
};

static ImGui_Stats stats = {};

static void _Test_ImGUI_onInit(GraphicsContext* ctx, GLFWwindow* w)
{
    gctx   = ctx;
    window = w;
    stm_setup();
}

static bool show_demo_window    = true;
static bool show_another_window = true;
static ImVec4 clear_color       = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

static void _Test_ImGUI_SizeCallback(ImGuiSizeCallbackData* data)
{
    printf("Size callback: %f, %f\n", data->DesiredSize.x, data->DesiredSize.y);
}

static void _Test_ImGUI_onRender(glm::mat4 proj, glm::mat4 view,
                                 glm::vec3 camPos)
{

    // Start the Dear ImGui frame
    u64 frame_start = stm_now();
    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    u64 frame_start_time = stm_since(frame_start);

    ImGuiIO& io = ImGui::GetIO();

    u64 imgui_command_start = stm_now();

    // 1. Show the big demo window (Most of the sample code is in
    // ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
    // ImGui!).
    if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair
    // to create a named window.
    {
        static float f     = 0.0f;
        static int counter = 0;

        printf("newframe\n");
        ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2(200, 200),
                                            _Test_ImGUI_SizeCallback, NULL);

        ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!"
                                       // and append into it.

        ImGui::Text("This is some useful text."); // Display some text (you can
                                                  // use a format strings too)
        ImGui::Checkbox(
          "Demo Window",
          &show_demo_window); // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat(
          "float", &f, 0.0f,
          1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3(
          "clear color",
          (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button(
              "Button")) // Buttons return true when clicked (most widgets
                         // return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    u64 imgui_command_time = stm_since(imgui_command_start);

    // Rendering (prepares ImDrawList)
    u64 imgui_render_start = stm_now();
    ImGui::Render();
    u64 imgui_render_time = stm_since(imgui_render_start);

    // pass encoder
    WGPUCommandEncoderDescriptor enc_desc = {};
    WGPUCommandEncoder encoder
      = wgpuDeviceCreateCommandEncoder(gctx->device, &enc_desc);

    // start render pass (depends on color attachment and pass descriptor)
    // Need a different render pass encoder for different color attachment views
    // and depth stencil views
    WGPURenderPassColorAttachment color_attachments = {};
    color_attachments.loadOp                        = WGPULoadOp_Clear;
    color_attachments.storeOp                       = WGPUStoreOp_Store;
    color_attachments.clearValue
      = { clear_color.x * clear_color.w, clear_color.y * clear_color.w,
          clear_color.z * clear_color.w, clear_color.w };
    color_attachments.view
      = wgpuSwapChainGetCurrentTextureView(gctx->swapChain);

    WGPURenderPassDescriptor render_pass_desc = {};
    render_pass_desc.colorAttachmentCount     = 1;
    render_pass_desc.colorAttachments         = &color_attachments;
    render_pass_desc.depthStencilAttachment   = &gctx->depthStencilAttachment;

    WGPURenderPassEncoder pass
      = wgpuCommandEncoderBeginRenderPass(encoder, &render_pass_desc);

    // actual draw call
    u64 imgui_draw_start = stm_now();
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), pass);
    u64 imgui_draw_time = stm_since(imgui_draw_start);

    // multiple viewports (not working)
    // if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    //     GLFWwindow* backup_current_context = glfwGetCurrentContext();
    //     ImGui::UpdatePlatformWindows();
    //     ImGui::RenderPlatformWindowsDefault();
    //     glfwMakeContextCurrent(backup_current_context);
    // }

    // end render pass
    wgpuRenderPassEncoderEnd(pass);

    // submit command buffer
    WGPUCommandBufferDescriptor cmd_buffer_desc = {};
    WGPUCommandBuffer cmd_buffer
      = wgpuCommandEncoderFinish(encoder, &cmd_buffer_desc);
    WGPUQueue queue = wgpuDeviceGetQueue(gctx->device);
    wgpuQueueSubmit(queue, 1, &cmd_buffer);

#ifndef __EMSCRIPTEN__
    wgpuSwapChainPresent(gctx->swapChain);
#endif

    wgpuTextureViewRelease(color_attachments.view);
    wgpuRenderPassEncoderRelease(pass);
    wgpuCommandEncoderRelease(encoder);
    wgpuCommandBufferRelease(cmd_buffer);

    stats.update(frame_start_time, imgui_command_time, imgui_render_time,
                 imgui_draw_time);
    if (stats.fc % 100 == 0) stats.print();
}

void Test_ImGUI(TestCallbacks* callbacks)
{
    *callbacks          = {};
    callbacks->onInit   = _Test_ImGUI_onInit;
    callbacks->onRender = _Test_ImGUI_onRender;
}