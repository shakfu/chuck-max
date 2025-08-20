#include "test_base.h"

#include "core/log.h"

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_wgpu.h>
#include <imgui/imgui.h>

static GraphicsContext* gctx = NULL;
static GLFWwindow* window    = NULL;

// Vertex
// position
// Normals
// UVs

static size_t Lines3D_Generate(glm::vec3 init_normal, float line_width,
                               glm::vec3* in_points, size_t in_point_count,
                               glm::vec3* out_points, size_t out_point_cap);

struct Uniforms {
    glm::mat4x4 world;             // at byte offset 0
    glm::mat4x4 view;              // at byte offset 64
    glm::mat4x4 proj;              // at byte offset 128
    float screen_width;            // at byte offset 192
    float screen_height;           // at byte offset 196
    uint32_t line_point_count;     // at byte offset 200
    float line_width       = 0.1f; // at byte offset 204
    float line_width_ratio = 0.5f; // at byte offset 208
    float _pad0[3];
};

static const char* lines2D_shader = R"glsl(

    struct Uniforms {
        world           : mat4x4<f32>,
        view            : mat4x4<f32>,
        proj            : mat4x4<f32>,
        screen_width    : f32,
        screen_height   : f32,
        line_point_count: u32, // number of line points (half the number of vertices in the line strip)
        line_width      : f32,
        line_width_ratio : f32, // how much of the line to extrude in miter direction vs -miter (defaults to 0.5)
    }

    struct PositionStorage {
        positionCount : u32,
        positions : array<f32>,
    }

    @group(0) @binding(0) var<uniform> uniforms : Uniforms;
    @group(0) @binding(1) var<storage, read> positions : array<f32>;
    // @binding(2) @group(0) var<storage, read> colors : U32s;
    // @binding(3) @group(0) var<storage, read> indices : U32s;

    struct VertexOutput {
        @builtin(position) position : vec4<f32>,
        @location(0) color : vec4<f32>
    }

    @vertex
    fn vs_main(@builtin(vertex_index) vertex_id : u32) -> VertexOutput {

        var output : VertexOutput;

        var pos_idx = (vertex_id / 2u) + 1u; // add 1 to skip sentinel start point
        var this_pos = vec2f(
            positions[3u * pos_idx + 0u],  // x
            positions[3u * pos_idx + 1u]   // y
        );
        var pos = this_pos;

        // get even/odd (odd vertices are expanded down, even vertices are expanded up)
        var orientation : f32 = 0.0;
        var miter_mod : f32 = 1.0;
        if (vertex_id % 2u == 0u) {
            orientation = 1.0;
            miter_mod = uniforms.line_width_ratio;
        } else {
            orientation = -1.0;
            miter_mod = 1.0 - uniforms.line_width_ratio;
        }

        var prev_pos = vec2f(
            positions[3u * (pos_idx - 1u) + 0u],  // x
            positions[3u * (pos_idx - 1u) + 1u],  // y
        );
        var next_pos = vec2f(
            positions[3u * (pos_idx + 1u) + 0u],  // x
            positions[3u * (pos_idx + 1u) + 1u],  // y
        );

        var prev_dir = normalize(this_pos - prev_pos);
        var next_dir = normalize(next_pos - this_pos);
        var prev_dir_perp = orientation * vec2f(-prev_dir.y, prev_dir.x);
        var next_dir_perp = orientation * vec2f(-next_dir.y, next_dir.x);

        var miter_dir = normalize(prev_dir_perp + next_dir_perp);
        var miter_length = miter_mod * uniforms.line_width / dot(miter_dir, prev_dir_perp);

        // adjust position
        pos += miter_length * miter_dir;

        // color debug
        output.color = vec4<f32>(0.0, 1.0, 0.0, 1.0);

        var position = vec4f(pos, 0.0, 1.0);
        position = uniforms.proj * uniforms.view * uniforms.world * position;

        // var color_u32 = colors.values[vertex.vertexID];
        // var color = vec4<f32>(
        //     f32((color_u32 >>  0u) & 0xFFu) / 255.0,
        //     f32((color_u32 >>  8u) & 0xFFu) / 255.0,
        //     f32((color_u32 >> 16u) & 0xFFu) / 255.0,
        //     f32((color_u32 >> 24u) & 0xFFu) / 255.0,
        // );

        output.position = position;

        return output;
    }


    struct FragmentInput {
        @location(0) color : vec4<f32>,
        @builtin(front_facing) is_front_facing : bool
    }

    struct FragmentOutput {
        @location(0) color : vec4<f32>
    }

    @fragment
    fn fs_main(fragment : FragmentInput) -> FragmentOutput {
        var output : FragmentOutput;
        output.color = fragment.color;

        // if (fragment.is_front_facing) {
        //     output.color = vec4<f32>(1.0, 0.0, 0.0, 1.0);
        // } else {
        //     output.color = vec4<f32>(0.0, 1.0, 0.0, 1.0);
        // }

        return output;
    }
)glsl";

static const char* lines3D_shader = R"glsl(

    struct Uniforms {
        world           : mat4x4<f32>,
        view            : mat4x4<f32>,
        proj            : mat4x4<f32>,
        screen_width    : f32,
        screen_height   : f32,
        line_point_count: u32, // number of line points (half the number of vertices in the line strip)
        line_width      : f32,
        line_width_ratio : f32, // how much of the line to extrude in miter direction vs -miter (defaults to 0.5)
    }

    @group(0) @binding(0) var<uniform> uniforms : Uniforms;
    @group(0) @binding(1) var<storage, read> positions : array<f32>;

    struct VertexOutput {
        @builtin(position) position : vec4<f32>,
        @location(0) color : vec4<f32>
    }

    @vertex
    fn vs_main(@builtin(vertex_index) vertex_id : u32) -> VertexOutput {
        var output : VertexOutput;
        var model_pos = vec4f(
            positions[vertex_id * 3u + 0u],  // x
            positions[vertex_id * 3u + 1u],  // y
            positions[vertex_id * 3u + 2u],  // z
            1.0
        );

        output.position = uniforms.proj * uniforms.view * uniforms.world * model_pos;
        output.color = vec4<f32>(0.0, 0.0, 1.0, 1.0);   
        return output;
    }


    struct FragmentInput {
        @location(0) color : vec4<f32>,
        @builtin(front_facing) is_front_facing : bool
    }

    struct FragmentOutput {
        @location(0) color : vec4<f32>
    }

    @fragment
    fn fs_main(fragment : FragmentInput) -> FragmentOutput {
        var output : FragmentOutput;
        output.color = fragment.color;
        return output;
    }
)glsl";

// renderer state
struct LinesRenderState {
    WGPURenderPipeline pipeline;
    WGPUBuffer uniform_buffer;
    WGPUBuffer positions;
    WGPUBindGroup bind_group;
};

static LinesRenderState lines2D_state = {};
static LinesRenderState lines3D_state = {};

#define MAX_LINE_POINTS 1024
static glm::vec3 line_points[MAX_LINE_POINTS] = {};
// static u32 line_colors[MAX_LINE_POINTS]       = {}; // TODO
static size_t num_points      = 3;
static float line_width       = 0.01f;
static float line_width_ratio = 0.5f;
static bool line_loop         = false;

static glm::vec3 lines3d_output[(MAX_LINE_POINTS + 3) * 2] = {};
static size_t lines3d_output_count                         = 0;
static glm::vec3 lines3d_input[]                           = {
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 1.0f, 1.0f),
    glm::vec3(0.0f, 2.0f, 1.0f),
};

static LinesRenderState createPipeline(const char* shader)
{
    LinesRenderState state = {};

    // init render pipelinetrue
    WGPUPrimitiveState primitiveState = {};
    primitiveState.topology           = WGPUPrimitiveTopology_TriangleStrip;
    // primitiveState.topology         = WGPUPrimitiveTopology_LineStrip;
    primitiveState.stripIndexFormat = WGPUIndexFormat_Uint32; // always 32bit
    primitiveState.frontFace        = WGPUFrontFace_CCW;
    primitiveState.cullMode         = WGPUCullMode_None;

    WGPUBlendComponent blend_component
      = { WGPUBlendOperation_Add, WGPUBlendFactor_SrcAlpha,
          WGPUBlendFactor_OneMinusSrcAlpha };
    WGPUBlendState blendState = { blend_component, blend_component };

    WGPUColorTargetState colorTargetState = {};
    colorTargetState.format               = gctx->swapChainFormat;
    colorTargetState.blend                = &blendState;
    colorTargetState.writeMask            = WGPUColorWriteMask_All;

    WGPUDepthStencilState depth_stencil_state
      = G_createDepthStencilState(WGPUTextureFormat_Depth24PlusStencil8, true);

    // Setup shader module
    WGPUShaderModule vertexShaderModule
      = G_createShaderModule(gctx, shader, "vertex shader");
    WGPUShaderModule fragmentShaderModule
      = G_createShaderModule(gctx, shader, "frag shader");
    defer({
        wgpuShaderModuleRelease(vertexShaderModule);
        wgpuShaderModuleRelease(fragmentShaderModule);
        vertexShaderModule   = NULL;
        fragmentShaderModule = NULL;
    });

    // vertex state
    WGPUVertexState vertexState = {};
    vertexState.bufferCount     = 0; // programmable vertex pulling, no vertex buffers
    vertexState.buffers         = NULL;
    vertexState.module          = vertexShaderModule;
    vertexState.entryPoint      = "vs_main";

    // fragment state
    WGPUFragmentState fragmentState = {};
    fragmentState.module            = fragmentShaderModule;
    fragmentState.entryPoint        = "fs_main";
    fragmentState.targetCount       = 1;
    fragmentState.targets           = &colorTargetState;

    // multisample state
    WGPUMultisampleState multisampleState = G_createMultisampleState(1);

    WGPURenderPipelineDescriptor pipeline_desc = {};
    pipeline_desc.layout                       = NULL; // auto
    pipeline_desc.primitive                    = primitiveState;
    pipeline_desc.vertex                       = vertexState;
    pipeline_desc.fragment                     = &fragmentState;
    pipeline_desc.depthStencil                 = &depth_stencil_state;
    pipeline_desc.multisample                  = multisampleState;

    state.pipeline = wgpuDeviceCreateRenderPipeline(gctx->device, &pipeline_desc);
    ASSERT(state.pipeline);

    WGPUBindGroupLayout layout
      = wgpuRenderPipelineGetBindGroupLayout(state.pipeline, 0);

    {
        // create uniform buffer
        WGPUBufferDescriptor uniform_buffer_desc = {};
        uniform_buffer_desc.size                 = sizeof(Uniforms);
        uniform_buffer_desc.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;

        state.uniform_buffer
          = wgpuDeviceCreateBuffer(gctx->device, &uniform_buffer_desc);
        ASSERT(state.uniform_buffer);

        // create positions storage buffer
        WGPUBufferDescriptor positions_buffer_desc = {};
        positions_buffer_desc.size                 = MEGABYTE;
        positions_buffer_desc.usage = WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst;
        state.positions = wgpuDeviceCreateBuffer(gctx->device, &positions_buffer_desc);

        // create bind group entry for uniforms
        WGPUBindGroupEntry entries[2]     = {};
        WGPUBindGroupEntry* uniform_entry = &entries[0];
        uniform_entry->binding            = 0;
        uniform_entry->buffer             = state.uniform_buffer;
        uniform_entry->offset             = 0;
        uniform_entry->size               = sizeof(Uniforms);

        // create bind group entry for positions
        WGPUBindGroupEntry* positions_entry = &entries[1];
        positions_entry->binding            = 1;
        positions_entry->buffer             = state.positions;
        positions_entry->offset             = 0;
        positions_entry->size               = positions_buffer_desc.size;

        // create bind group
        WGPUBindGroupDescriptor bg_desc = {};
        bg_desc.layout                  = layout;
        bg_desc.entryCount              = ARRAY_LENGTH(entries);
        bg_desc.entries                 = entries;
        ASSERT(bg_desc.entryCount == 2);

        state.bind_group = wgpuDeviceCreateBindGroup(gctx->device, &bg_desc);
    }
    return state;
}

static void _Test_Lines2D_onInit(GraphicsContext* ctx, GLFWwindow* w)
{
    line_points[1] = glm::vec3(-0.5f, -0.5f, 0.0f);
    line_points[2] = glm::vec3(0.0f, 0.5f, 0.0f);
    line_points[3] = glm::vec3(0.5f, -0.5f, 0.0f);

    gctx   = ctx;
    window = w;

    lines2D_state = createPipeline(lines2D_shader);
    lines3D_state = createPipeline(lines3D_shader);
}

// update all uniform and storage buffers
// Note: in actual app, would only update storage buffers (vertices, indices)
// if they actually changed
static void updateBuffers(glm::mat4 proj, glm::mat4 view, glm::vec3 camPos)
{

    //   struct Uniforms {
    //     glm::mat4x4 world;   // at byte offset 0
    //     glm::mat4x4 view;    // at byte offset 64
    //     glm::mat4x4 proj;    // at byte offset 128
    //     float screen_width;  // at byte offset 192
    //     float screen_height; // at byte offset 196
    //     float _pad0[2];
    // };

    UNUSED_VAR(camPos);

    // update uniform buffer
    int screen_width, screen_height;
    glfwGetWindowSize(window, &screen_width, &screen_height);
    Uniforms uniforms         = {};
    uniforms.world            = glm::mat4(1.0f);
    uniforms.view             = view;
    uniforms.proj             = proj;
    uniforms.screen_width     = (float)screen_width;
    uniforms.screen_height    = (float)screen_height;
    uniforms.line_point_count = num_points;
    uniforms.line_width       = line_width;
    uniforms.line_width_ratio = line_width_ratio;
    wgpuQueueWriteBuffer(gctx->queue, lines2D_state.uniform_buffer, 0, &uniforms,
                         sizeof(Uniforms));

    { // update positions buffer

        // create sentinal start point
        if (line_loop) {
            line_points[0] = line_points[num_points]; // copy last point
        } else {
            glm::vec3 diff = line_points[2] - line_points[1];
            line_points[0] = line_points[1] - diff; // extend in opposite direction
        }

        // sentinal end points
        if (line_loop) {
            line_points[num_points + 1] = line_points[1]; // copy first point
            line_points[num_points + 2] = line_points[2]; // copy second point
        } else {
            glm::vec3 diff = line_points[num_points] - line_points[num_points - 1];
            line_points[num_points + 1]
              = line_points[num_points] + diff; // extend in same direction
        }

        wgpuQueueWriteBuffer(gctx->queue, lines2D_state.positions, 0, line_points,
                             sizeof(line_points));
    }

    { // 3D line test
        wgpuQueueWriteBuffer(gctx->queue, lines3D_state.uniform_buffer, 0, &uniforms,
                             sizeof(Uniforms));

        lines3d_output_count = Lines3D_Generate(
          glm::vec3(1.0f, 0.0f, 0.0f), 0.1f, lines3d_input, ARRAY_LENGTH(lines3d_input),
          lines3d_output, ARRAY_LENGTH(lines3d_output));

        wgpuQueueWriteBuffer(gctx->queue, lines3D_state.positions, 0, lines3d_output,
                             sizeof(lines3d_output));
    }
}

static void _Test_Lines2D_onUpdate(f32 dt)
{
}

static void drawUI(WGPURenderPassEncoder pass)
{
    using namespace ImGui;
    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // enable docking to main window
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(),
                                 ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::ShowDemoWindow(NULL);

    // docking:
    // https://gist.github.com/AidanSun05/b342f1bf023e931695473e343569759c

    ImGui::Begin("Lines2D");

    DragFloat("Line Width", &line_width, 0.001f, 0.0f, 1.0f);

    SliderFloat("Line Width Ratio", &line_width_ratio, 0.0f, 1.0f);
    SetItemTooltip(
      "How much of the line to extrude in miter direction vs -miter (defaults "
      "to 0.5)");

    Checkbox("Loop", &line_loop);

    // modify indices 1 --> num_points-2. Endpoints (0 and num_points-1) are
    // reserved for line loop / sentinel endpoints
    for (size_t i = 0; i < num_points; ++i) {
        int idx = i + 1;
        PushID(idx);
        DragFloat3("##point", (float*)&line_points[idx], 0.01f);
        SameLine();
        if (Button("Remove")) {
            for (size_t j = idx; j < num_points - 1; ++j) {
                line_points[j] = line_points[j + 1];
            }
            num_points = MAX(num_points - 1, 0);
        }
        PopID();
    }
    if (Button("Add Point")) {
        num_points = MIN(num_points + 1, MAX_LINE_POINTS);
    }

    // listbox display all line points, including sentinals
    for (size_t i = 0; i < num_points + 2; ++i) {
        Text("Point %zu: (%.2f, %.2f)", i, line_points[i].x, line_points[i].y);
    }
    Separator();

    // for (size_t i = 0; i < ; ++i) {
    //     int idx = i + 1;
    //     PushID(idx);
    //     DragFloat3("##lines_3d_point", (float*)&line_points[idx], 0.01f);
    //     SameLine();
    //     if (Button("Remove##lines_3d")) {
    //         for (size_t j = idx; j < num_points - 1; ++j) {
    //             line_points[j] = line_points[j + 1];
    //         }
    //         num_points = MAX(num_points - 1, 0);
    //     }
    //     PopID();
    // }
    // if (Button("Add Point##lines_3d")) {
    //     num_points = MIN(num_points + 1, MAX_LINE_POINTS);
    // }

    // display lines3D points
    for (size_t i = 0; i < lines3d_output_count; ++i) {
        Text("Point %zu: (%.2f, %.2f, %.2f)", i, lines3d_output[i].x,
             lines3d_output[i].y, lines3d_output[i].z);
    }

    ImGui::End();

    // NEXT: add UI for adding/removing points, using multi-component drag
    // sliders

    ImGui::Render();
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), pass);
}

static void _Test_Lines2D_onRender(glm::mat4 proj, glm::mat4 view, glm::vec3 camPos)
{
    updateBuffers(proj, view, camPos);

    GraphicsContext::prepareFrame(gctx);

    WGPURenderPassEncoder renderPass
      = wgpuCommandEncoderBeginRenderPass(gctx->commandEncoder, &gctx->renderPassDesc);

    wgpuRenderPassEncoderSetPipeline(renderPass, lines2D_state.pipeline);

    wgpuRenderPassEncoderSetBindGroup(renderPass, 0, lines2D_state.bind_group, 0, 0);

    // wgpuRenderPassEncoderDraw(
    // WGPURenderPassEncoder renderPassEncoder,
    // uint32_t vertexCount,
    // uint32_t instanceCount,
    // uint32_t firstVertex,
    // uint32_t firstInstance);

    const u32 vertex_count = line_loop ? 2 * (num_points + 1) : 2 * num_points;
    wgpuRenderPassEncoderDraw(renderPass, vertex_count, 1, 0, 0);

    // draw lines3d
    wgpuRenderPassEncoderSetPipeline(renderPass, lines3D_state.pipeline);
    wgpuRenderPassEncoderSetBindGroup(renderPass, 0, lines3D_state.bind_group, 0, 0);
    wgpuRenderPassEncoderDraw(renderPass, lines3d_output_count, 1, 0, 0);

    drawUI(renderPass);

    wgpuRenderPassEncoderEnd(renderPass);

    GraphicsContext::presentFrame(gctx);
}

static void _Test_Lines2D_onExit()
{
}

// 3D line rendering algorithm
// output vertex list (just positions) to be fed into
// triangle strip topology renderer
/// @param init_normal: initial normal vector for the first line segment
static size_t Lines3D_Generate(glm::vec3 init_normal, float line_width,
                               glm::vec3* in_points, size_t in_point_count,
                               glm::vec3* out_points, size_t out_point_cap)
{
#define PUSH_VERTEX(v)                                                                 \
    {                                                                                  \
        out_points[vertex_id++] = (v);                                                 \
        printf("Vertex %zu: (%.2f, %.2f, %.2f)\n", vertex_id, out_points[vertex_id].x, \
               out_points[vertex_id].y, out_points[vertex_id].z);                      \
    }

    // make sure we have enough memory for output verctices + sentinel points
    ASSERT(out_point_cap >= 2 * (in_point_count + 3));

    // TODO sentinels

    size_t vertex_id = 0;
    // starting vertex
    init_normal = glm::normalize(init_normal);
    PUSH_VERTEX(in_points[0] + init_normal * line_width);
    PUSH_VERTEX(in_points[0] - init_normal * line_width);

    // loop through all points except last
    for (size_t i = 1; i < in_point_count - 1; ++i) {
        glm::vec3 prev_p = in_points[i - 1];
        glm::vec3 curr_p = in_points[i];
        glm::vec3 next_p = in_points[i + 1];

        glm::vec3 prev_v0  = out_points[vertex_id - 2];
        glm::vec3 prev_v1  = out_points[vertex_id - 1];
        glm::vec3 prev_dir = glm::normalize(prev_p - curr_p);
        // calculate normal for this segment, on the plane defined
        // by curr_p and the previous two vertices
        // plane is defined by these two vectors
        glm::vec3 plane_n = glm::normalize(
          glm::cross(prev_v0 - curr_p,
                     prev_v1 - curr_p)); // TODO: maybe can get rid of prev_v1
                                         // and cross with prev_p instead
        glm::vec3 segment_n = glm::normalize(glm::cross(plane_n, prev_dir));

        // calculate miter direction using azaday formula
        // project next_p onto plane defined by curr_p and prev_p
        glm::vec3 next_p_proj = next_p - glm::dot(next_p - curr_p, plane_n) * plane_n;
        // cone length
        float L                  = glm::length(next_p - curr_p);
        glm::vec3 prev_p_on_cone = curr_p + L * prev_dir;

        glm::vec3 dir_prev_p_to_proj
          = glm::normalize(next_p_proj - prev_p_on_cone);                      // d1
        float dist_prev_p_to_next_p    = glm::length(next_p - prev_p_on_cone); // |d2|
        glm::vec3 dir_prev_p_to_next_p = glm::normalize(next_p - prev_p_on_cone); // d2

        // calculate radius of cone that would be formed by the miter
        float r = dist_prev_p_to_next_p
                  / (2.0f * glm::dot(dir_prev_p_to_proj, dir_prev_p_to_next_p));
        // calculate origin of bottom of cone (center of triangle)
        glm::vec3 cone_bottom_origin = prev_p_on_cone + r * dir_prev_p_to_proj;
        glm::vec3 dir_miter          = glm::normalize(cone_bottom_origin - curr_p);

        // calculate miter length (todo add line_width_ratio option)
        float miter_length = line_width / glm::dot(dir_miter, segment_n);

        // add vertices
        PUSH_VERTEX(curr_p + miter_length * dir_miter);
        PUSH_VERTEX(curr_p - miter_length * dir_miter);
    }

    // final vertex
    // get normal of plane formed by last two points
    glm::vec3 curr_p   = in_points[in_point_count - 1];
    glm::vec3 prev_p   = in_points[in_point_count - 2];
    glm::vec3 prev_dir = glm::normalize(prev_p - curr_p);
    glm::vec3 prev_v0  = out_points[vertex_id - 2];
    glm::vec3 prev_v1  = out_points[vertex_id - 1];
    glm::vec3 prev_plane_n
      = glm::normalize(glm::cross(prev_v0 - prev_p, prev_v1 - prev_p));
    glm::vec3 segment_n = glm::normalize(glm::cross(prev_plane_n, prev_dir));
    PUSH_VERTEX(curr_p + line_width * segment_n);
    PUSH_VERTEX(curr_p - line_width * segment_n);

    ASSERT(vertex_id % 2 == 0);
    ASSERT(vertex_id == 2 * in_point_count);

#undef PUSH_VERTEX
    return vertex_id;
}

void Test_Lines2D(TestCallbacks* callbacks)
{
    *callbacks          = {};
    callbacks->onInit   = _Test_Lines2D_onInit;
    callbacks->onUpdate = _Test_Lines2D_onUpdate;
    callbacks->onRender = _Test_Lines2D_onRender;
    callbacks->onExit   = _Test_Lines2D_onExit;
}
