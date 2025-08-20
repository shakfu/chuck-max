#include SCREEN_PASS_VERTEX_SHADER

@fragment 
fn fs_main(in : VertexOutput) -> @location(0) vec4f {
    return vec4f(in.v_uv, 0.0, 1.0);
}