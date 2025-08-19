// TODO: improve test by actually tracking memory allocation in audio and graphics component managers
// for now, this test just checks the SG_ID of the first shader and makes sure it is unchanged

"
#include FRAME_UNIFORMS
#include DRAW_UNIFORMS
#include STANDARD_VERTEX_INPUT
#include STANDARD_VERTEX_OUTPUT
#include STANDARD_VERTEX_SHADER

@fragment 
fn fs_main(in : VertexOutput, @builtin(front_facing) is_front: bool) -> @location(0) vec4f
{
    return vec4f(1.0);
}" => string shader_string;

null => Shader@ first_shader;
int first_shader_id;

fun void createShader() {
    ShaderDesc shader_desc;
    shader_string => shader_desc.vertexCode;
    shader_string => shader_desc.fragmentCode;
    Shader custom_shader(shader_desc); // create shader from shader_desc
    if (first_shader == null) {
        custom_shader @=> first_shader;
        first_shader.id() => first_shader_id;
    } 
    <<< "first_shader id changed?", first_shader.id() - first_shader_id, "| custom_shader id increment", custom_shader.id() - first_shader_id >>>;
}

repeat (5) {
    GG.nextFrame() => now;
    createShader();
}