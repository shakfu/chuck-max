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

#include "sg_command.h"
#include "sg_component.h"

#include "shaders.h"

#define GET_SHADER(ckobj)                                                              \
    (ckobj ? SG_GetShader(OBJ_MEMBER_UINT(ckobj, component_offset_id)) : NULL)
#define GET_MATERIAL(ckobj) SG_GetMaterial(OBJ_MEMBER_UINT(ckobj, component_offset_id))

void chugl_initDefaultMaterials();

CK_DLL_CTOR(shader_desc_ctor);
static t_CKUINT shader_desc_vertex_string_offset     = 0;
static t_CKUINT shader_desc_fragment_string_offset   = 0;
static t_CKUINT shader_desc_vertex_filepath_offset   = 0;
static t_CKUINT shader_desc_fragment_filepath_offset = 0;
static t_CKUINT shader_desc_vertex_layout_offset     = 0;
static t_CKUINT shader_desc_compute_string_offset    = 0;
static t_CKUINT shader_desc_compute_filepath_offset  = 0;
static t_CKUINT shader_desc_is_lit                   = 0;
static t_CKUINT shader_desc_uses_envmap              = 0;

CK_DLL_CTOR(shader_ctor_default);
CK_DLL_CTOR(shader_ctor);

CK_DLL_MFUN(shader_get_vertex_string);
CK_DLL_MFUN(shader_get_fragment_string);
CK_DLL_MFUN(shader_get_vertex_filepath);
CK_DLL_MFUN(shader_get_fragment_filepath);
CK_DLL_MFUN(shader_get_vertex_layout);
CK_DLL_MFUN(shader_get_lit);
CK_DLL_MFUN(shader_get_uses_envmap);

CK_DLL_CTOR(material_ctor);
CK_DLL_CTOR(material_ctor_with_shader);

// material pso
CK_DLL_MFUN(material_get_shader);
CK_DLL_MFUN(material_set_shader);
CK_DLL_MFUN(material_get_cullmode);
CK_DLL_MFUN(material_set_cullmode);
CK_DLL_MFUN(material_set_topology);
CK_DLL_MFUN(material_get_topology);
CK_DLL_MFUN(material_set_transparent);
CK_DLL_MFUN(material_get_transparent);

// material uniforms
CK_DLL_MFUN(material_uniform_remove);
CK_DLL_MFUN(material_uniform_active_locations);

CK_DLL_MFUN(material_set_uniform_float);
CK_DLL_MFUN(material_get_uniform_float);
CK_DLL_MFUN(material_set_uniform_float2);
CK_DLL_MFUN(material_get_uniform_float2);
CK_DLL_MFUN(material_set_uniform_float3);
CK_DLL_MFUN(material_get_uniform_float3);
CK_DLL_MFUN(material_set_uniform_float4);
CK_DLL_MFUN(material_get_uniform_float4);

CK_DLL_MFUN(material_set_uniform_int);
CK_DLL_MFUN(material_get_uniform_int);
CK_DLL_MFUN(material_set_uniform_int2);
CK_DLL_MFUN(material_get_uniform_int2);
CK_DLL_MFUN(material_set_uniform_int3);
CK_DLL_MFUN(material_get_uniform_int3);
CK_DLL_MFUN(material_set_uniform_int4);
CK_DLL_MFUN(material_get_uniform_int4);

// getting back storage buffers is tricky because it may have been modified by shader
// so no getter for now (until we figure out compute shaders)
/*
possible impl for storage buffer getter:
- StorageBufferEvent to handle async buffer map from gpu --> render thread --> audio
thread
- GPUBuffer component exposed through chugl API that can be queried for buffer data
    - setStorageBuffer() instead of taking a ck_FloatArray, takes a GPUBuffer component
*/
CK_DLL_MFUN(material_set_storage_buffer);
CK_DLL_MFUN(material_set_storage_buffer_vec2);
CK_DLL_MFUN(material_set_storage_buffer_vec3);
CK_DLL_MFUN(material_set_storage_buffer_vec4);
CK_DLL_MFUN(material_set_storage_buffer_integer);
CK_DLL_MFUN(material_set_storage_buffer_external);
CK_DLL_MFUN(material_set_sampler);
CK_DLL_MFUN(material_set_texture);
CK_DLL_MFUN(material_set_storage_texture);

CK_DLL_MFUN(material_get_sampler);
CK_DLL_MFUN(material_get_texture);

CK_DLL_CTOR(lines2d_material_ctor);
CK_DLL_MFUN(lines2d_material_get_thickness);
CK_DLL_MFUN(lines2d_material_set_thickness);
CK_DLL_MFUN(lines2d_material_get_color);
CK_DLL_MFUN(lines2d_material_set_color);
CK_DLL_MFUN(lines2d_material_get_extrusion);
CK_DLL_MFUN(lines2d_material_set_extrusion);
CK_DLL_MFUN(lines2d_material_get_loop);
CK_DLL_MFUN(lines2d_material_set_loop);

// flat material
CK_DLL_CTOR(flat_material_ctor);
CK_DLL_MFUN(flat_material_get_color);
CK_DLL_MFUN(flat_material_set_color);
CK_DLL_MFUN(flat_material_set_color_rgba);
CK_DLL_MFUN(flat_material_get_alpha);
CK_DLL_MFUN(flat_material_set_alpha);
CK_DLL_MFUN(flat_material_get_sampler);
CK_DLL_MFUN(flat_material_set_sampler);
CK_DLL_MFUN(flat_material_get_color_map);
CK_DLL_MFUN(flat_material_set_color_map);

CK_DLL_MFUN(flat_material_set_texture_offset);
CK_DLL_MFUN(flat_material_set_texture_scale);
CK_DLL_MFUN(flat_material_get_texture_offset);
CK_DLL_MFUN(flat_material_get_texture_scale);

CK_DLL_CTOR(uv_material_ctor);

CK_DLL_CTOR(normal_material_ctor);
CK_DLL_MFUN(normal_material_set_worldspace_normals);
CK_DLL_MFUN(normal_material_get_worldspace_normals);

CK_DLL_CTOR(wireframe_material_ctor);
CK_DLL_MFUN(wireframe_material_set_thickness);
CK_DLL_MFUN(wireframe_material_get_thickness);
CK_DLL_MFUN(wireframe_material_set_alpha_cutoff);
CK_DLL_MFUN(wireframe_material_get_alpha_cutoff);
CK_DLL_MFUN(wireframe_material_set_color);
CK_DLL_MFUN(wireframe_material_get_color);

// phong ---------------------------------------------------------------------
CK_DLL_CTOR(phong_material_ctor);

CK_DLL_MFUN(phong_material_get_specular_color);
CK_DLL_MFUN(phong_material_set_specular_color);

CK_DLL_MFUN(phong_material_get_diffuse_color);
CK_DLL_MFUN(phong_material_set_diffuse_color);

CK_DLL_MFUN(phong_material_get_alpha);
CK_DLL_MFUN(phong_material_set_alpha);

CK_DLL_MFUN(phong_material_get_log_shininess);
CK_DLL_MFUN(phong_material_set_log_shininess);

CK_DLL_MFUN(phong_material_get_emission_color);
CK_DLL_MFUN(phong_material_set_emission_color);

CK_DLL_MFUN(phong_material_get_normal_factor);
CK_DLL_MFUN(phong_material_set_normal_factor);

CK_DLL_MFUN(phong_material_get_ao_factor);
CK_DLL_MFUN(phong_material_set_ao_factor);

CK_DLL_MFUN(phong_material_get_albedo_tex);
CK_DLL_MFUN(phong_material_set_albedo_tex);

CK_DLL_MFUN(phong_material_get_specular_tex);
CK_DLL_MFUN(phong_material_set_specular_tex);

CK_DLL_MFUN(phong_material_get_ao_tex);
CK_DLL_MFUN(phong_material_set_ao_tex);

CK_DLL_MFUN(phong_material_get_emissive_tex);
CK_DLL_MFUN(phong_material_set_emissive_tex);

CK_DLL_MFUN(phong_material_get_normal_tex);
CK_DLL_MFUN(phong_material_set_normal_tex);

// envmap
CK_DLL_MFUN(phong_material_get_envmap_method);
CK_DLL_MFUN(phong_material_set_envmap_method);
CK_DLL_MFUN(phong_material_get_envmap_refraction_ratio);
CK_DLL_MFUN(phong_material_set_envmap_refraction_ratio);
CK_DLL_MFUN(phong_material_get_envmap_blend_mode);
CK_DLL_MFUN(phong_material_set_envmap_blend_mode);
CK_DLL_MFUN(phong_material_get_envmap_intensity);
CK_DLL_MFUN(phong_material_set_envmap_intensity);

// pbr ---------------------------------------------------------------------
CK_DLL_CTOR(pbr_material_ctor);

CK_DLL_MFUN(pbr_material_get_albedo);
CK_DLL_MFUN(pbr_material_set_albedo);

CK_DLL_MFUN(pbr_material_get_emissive);
CK_DLL_MFUN(pbr_material_set_emissive);

CK_DLL_MFUN(pbr_material_get_metallic);
CK_DLL_MFUN(pbr_material_set_metallic);

CK_DLL_MFUN(pbr_material_get_roughness);
CK_DLL_MFUN(pbr_material_set_roughness);

CK_DLL_MFUN(pbr_material_get_normal_factor);
CK_DLL_MFUN(pbr_material_set_normal_factor);

CK_DLL_MFUN(pbr_material_get_ao_factor);
CK_DLL_MFUN(pbr_material_set_ao_factor);

CK_DLL_MFUN(pbr_material_get_albedo_tex);
CK_DLL_MFUN(pbr_material_set_albedo_tex);

CK_DLL_MFUN(pbr_material_get_normal_tex);
CK_DLL_MFUN(pbr_material_set_normal_tex);

CK_DLL_MFUN(pbr_material_get_ao_tex);
CK_DLL_MFUN(pbr_material_set_ao_tex);

CK_DLL_MFUN(pbr_material_get_mr_tex);
CK_DLL_MFUN(pbr_material_set_mr_tex);

CK_DLL_MFUN(pbr_material_get_emissive_tex);
CK_DLL_MFUN(pbr_material_set_emissive_tex);

// skybox ---------------------------------------------------------------------
CK_DLL_CTOR(skybox_material_ctor);

static_assert(sizeof(WGPUVertexFormat) == sizeof(int),
              "WGPUVertexFormat size mismatch");

void ulib_material_cq_update_all_uniforms(SG_Material* material)
{
    int num_uniforms = ARRAY_LENGTH(material->uniforms);
    for (int i = 0; i < num_uniforms; i++) {
        SG_MaterialUniform* uniform = &material->uniforms[i];
        switch (uniform->type) {
            case SG_MATERIAL_UNIFORM_NONE: break;
            case SG_MATERIAL_UNIFORM_STORAGE_BUFFER: {
                ASSERT(false); // not implemented
            } break;
            default: {
                CQ_PushCommand_MaterialSetUniform(material, i);
            }
        }
    }
}

void ulib_material_query(Chuck_DL_Query* QUERY)
{
    // TODO today: documentation

    BEGIN_CLASS("VertexFormat", "Object");
    DOC_CLASS("Vertex format enum. Used to describe vertex data layout in ShaderDesc.");

    static t_CKINT format_undefined = WGPUVertexFormat_Undefined;
    static t_CKINT format_float     = WGPUVertexFormat_Float32;
    static t_CKINT format_float2    = WGPUVertexFormat_Float32x2;
    static t_CKINT format_float3    = WGPUVertexFormat_Float32x3;
    static t_CKINT format_float4    = WGPUVertexFormat_Float32x4;
    static t_CKINT format_int       = WGPUVertexFormat_Sint32;
    static t_CKINT format_int2      = WGPUVertexFormat_Sint32x2;
    static t_CKINT format_int3      = WGPUVertexFormat_Sint32x3;
    static t_CKINT format_int4      = WGPUVertexFormat_Sint32x4;

    SVAR("int", "Undefined", &format_undefined);
    SVAR("int", "Float", &format_float);
    SVAR("int", "Float2", &format_float2);
    SVAR("int", "Float3", &format_float3);
    SVAR("int", "Float4", &format_float4);
    SVAR("int", "Int", &format_int);
    SVAR("int", "Int2", &format_int2);
    SVAR("int", "Int3", &format_int3);
    SVAR("int", "Int4", &format_int4);

    END_CLASS();

    // ShaderDesc -----------------------------------------------------
    BEGIN_CLASS("ShaderDesc", "Object");
    DOC_CLASS(
      "Shader description object. Used to create a Shader component."
      "If creating a material shader, set either vertexCode or vertexPath, and "
      "either fragmentCode or "
      "fragmentPath. `vertexLayout` field describes the vertex data layout "
      "of buffers going into the vertex shader--use the VertexFormat enum. "
      "For compute shaders, set either computeCode or computePath.");

    CTOR(shader_desc_ctor);

    shader_desc_vertex_string_offset = MVAR("string", "vertexCode", false);
    DOC_VAR("Vertex shader string. Set if passing a raw shader code (not a filepath)");

    shader_desc_fragment_string_offset = MVAR("string", "fragmentCode", false);
    DOC_VAR(
      "Fragment shader string. Set if passing a raw shader code (not a filepath)");

    shader_desc_vertex_filepath_offset = MVAR("string", "vertexPath", false);
    DOC_VAR("Vertex shader filepath. Set if passing a filepath to a shader file");

    shader_desc_fragment_filepath_offset = MVAR("string", "fragmentPath", false);
    DOC_VAR("Fragment shader filepath. Set if passing a filepath to a shader file");

    shader_desc_vertex_layout_offset = MVAR("int[]", "vertexLayout", false);
    DOC_VAR(
      "Array of VertexFormat enums describing the vertex data layout."
      "E.g. if your vertex shader takes a vec3 position and a vec2 uv, set "
      "`vertexLayout` to [VertexFormat.Float3, VertexFormat.Float2]. "
      "By default this field is set to the standard vertex layout, "
      "[VertexFormat.Float3, "
      "VertexFormat.Float3, VertexFormat.Float2]. "
      "This corresponds to position, normal, uv.");

    shader_desc_compute_string_offset = MVAR("string", "computeCode", false);
    DOC_VAR("Compute shader string. Set if passing a raw shader code (not a filepath)");

    shader_desc_compute_filepath_offset = MVAR("string", "computePath", false);
    DOC_VAR("Compute shader filepath. Set if passing a filepath to a shader file");

    shader_desc_is_lit = MVAR("int", "lit", false);
    DOC_VAR(
      "set to true if the shader is lit (uses lighting calculations). If set, the "
      "renderer will pass in lighting information as part of the per-frame uniforms."
      "you can access these in your shader via the `#include LIGHTING_UNIFORMS` "
      "macro.");

    shader_desc_uses_envmap = MVAR("int", "usesEnvMap", false);
    DOC_VAR(
      "set to true if the shader uses an environment map. If set, the renderer will "
      "pass in the environment map (set on GG.scene().envMap()) as part of the "
      "per-frame uniforms. You can access these in your shader via the `#include "
      "ENVIRONMENT_MAP_UNIFORMS` macro.");

    END_CLASS();

    // Shader -----------------------------------------------------
    BEGIN_CLASS(SG_CKNames[SG_COMPONENT_SHADER], SG_CKNames[SG_COMPONENT_BASE]);
    DOC_CLASS(
      "Shader component. Immutable. Create by passing a ShaderDesc. E.g. new "
      "Shader(ShaderDesc)");

    // svars -----------------------------------------------------
    // cannot declare static non-primitive objects
    // SVAR("string", "FRAME_UNIFORMS", g_builtin_ckobjs.FRAME_UNIFORMS);
    // DOC_VAR(
    //   "Frame uniforms string. Can be included in a shader via #include
    //   FRAME_UNIFORMS");

    // SVAR("string", "LIGHTING_UNIFORMS", g_builtin_ckobjs.LIGHTING_UNIFORMS);
    // DOC_VAR(
    //   "Lighting uniforms string. Can be included in a shader via #include "
    //   "LIGHTING_UNIFORMS");

    // SVAR("string", "DRAW_UNIFORMS", g_builtin_ckobjs.DRAW_UNIFORMS);
    // DOC_VAR(
    //   "Draw uniforms string. Can be included in a shader via #include
    //   DRAW_UNIFORMS");

    // SVAR("string", "STANDARD_VERTEX_INPUT", g_builtin_ckobjs.STANDARD_VERTEX_INPUT);
    // DOC_VAR(
    //   "Standard vertex input string. Can be included in a shader via #include "
    //   "STANDARD_VERTEX_INPUT");

    // SVAR("string", "STANDARD_VERTEX_OUTPUT",
    // g_builtin_ckobjs.STANDARD_VERTEX_OUTPUT); DOC_VAR(
    //   "Standard vertex output string. Can be included in a shader via #include "
    //   "STANDARD_VERTEX_OUTPUT");

    // SVAR("string", "STANDARD_VERTEX_SHADER",
    // g_builtin_ckobjs.STANDARD_VERTEX_SHADER); DOC_VAR(
    //   "Standard vertex shader string. Can be included in a shader via #include "
    //   "STANDARD_VERTEX_SHADER");

    // ctors -----------------------------------------------------

    CTOR(shader_ctor_default);

    CTOR(shader_ctor);
    ARG("ShaderDesc", "shader_desc");
    DOC_FUNC("Create a Shader component. Immutable.");
    ADD_EX("deep/audio_donut.ck");

    MFUN(shader_get_vertex_string, "string", "vertexCode");
    DOC_FUNC("Get the vertex shader string passed in the ShaderDesc at creation.");

    MFUN(shader_get_fragment_string, "string", "fragmentCode");
    DOC_FUNC("Get the fragment shader string passed in the ShaderDesc at creation.");

    MFUN(shader_get_vertex_filepath, "string", "vertexPath");
    DOC_FUNC("Get the vertex shader filepath passed in the ShaderDesc at creation.");

    MFUN(shader_get_fragment_filepath, "string", "fragmentPath");
    DOC_FUNC("Get the fragment shader filepath passed in the ShaderDesc at creation.");

    MFUN(shader_get_vertex_layout, "int[]", "vertexLayout");
    DOC_FUNC("Get the vertex layout passed in the ShaderDesc at creation.");

    MFUN(shader_get_lit, "int", "lit");
    DOC_FUNC("Get whether the shader is lit (uses lighting calculations).");

    MFUN(shader_get_uses_envmap, "int", "usesEnvMap");
    DOC_FUNC("Get whether the shader uses an environment map.");

    END_CLASS();

    // Material -----------------------------------------------------
    BEGIN_CLASS(SG_CKNames[SG_COMPONENT_MATERIAL], SG_CKNames[SG_COMPONENT_BASE]);
    DOC_CLASS(
      "Chugl material types. Represents uniforms/textures/buffers that are passed into "
      "a given shader");
    ADD_EX("basic/geo_and_mat.ck");
    ADD_EX("basic/transparency.ck");

    CTOR(material_ctor);

    CTOR(material_ctor_with_shader);
    ARG(SG_CKNames[SG_COMPONENT_SHADER], "shader");
    DOC_FUNC("Create a Material component with the given Shader.");

    // svars
    static t_CKINT cullmode_none  = WGPUCullMode_None;
    static t_CKINT cullmode_front = WGPUCullMode_Front;
    static t_CKINT cullmode_back  = WGPUCullMode_Back;
    SVAR("int", "Cull_None", &cullmode_none);
    DOC_VAR("No culling.");
    SVAR("int", "Cull_Front", &cullmode_front);
    DOC_VAR("Cull front faces.");
    SVAR("int", "Cull_Back", &cullmode_back);
    DOC_VAR("Cull back faces.");

    static t_CKINT topology_pointlist     = WGPUPrimitiveTopology_PointList;
    static t_CKINT topology_linelist      = WGPUPrimitiveTopology_LineList;
    static t_CKINT topology_linestrip     = WGPUPrimitiveTopology_LineStrip;
    static t_CKINT topology_trianglelist  = WGPUPrimitiveTopology_TriangleList;
    static t_CKINT topology_trianglestrip = WGPUPrimitiveTopology_TriangleStrip;
    SVAR("int", "Topology_PointList", &topology_pointlist);
    DOC_VAR("Interpret each vertex as a point.");
    SVAR("int", "Topology_LineList", &topology_linelist);
    DOC_VAR("Interpret each pair of vertices as a line.");
    SVAR("int", "Topology_LineStrip", &topology_linestrip);
    DOC_VAR(
      "Each vertex after the first defines a line primitive between it and the "
      "previous vertex.");
    SVAR("int", "Topology_TriangleList", &topology_trianglelist);
    DOC_VAR("Interpret each triplet of vertices as a triangle.");
    SVAR("int", "Topology_TriangleStrip", &topology_trianglestrip);
    DOC_VAR(
      "Each vertex after the first two defines a triangle primitive between it and the "
      "previous two vertices.");

    // pso modifiers (shouldn't be set often, so we lump all together in a single
    // command that copies the entire PSO struct)
    MFUN(material_get_shader, SG_CKNames[SG_COMPONENT_SHADER], "shader");
    DOC_FUNC("Get the shader of the material.");

    MFUN(material_set_shader, "void", "shader");
    ARG(SG_CKNames[SG_COMPONENT_SHADER], "shader");
    DOC_FUNC("Set the shader of the material.");

    MFUN(material_get_cullmode, "int", "cullMode");
    DOC_FUNC(
      "Get the cull mode of the material. Material.Cull_None, Material.Cull_Front, or "
      "Material.Cull_Back.");

    MFUN(material_set_cullmode, "void", "cullMode");
    ARG("int", "cullMode");
    DOC_FUNC(
      "Set the cull mode of the material. valid options: Material.Cull_None, "
      "Material.Cull_Front, or Material.Cull_Back.");

    MFUN(material_set_topology, "void", "topology");
    ARG("int", "topology");
    DOC_FUNC(
      "Set the primitive topology of the material. valid options: "
      "Material.Topology_PointList, Material.Topology_LineList, "
      "Material.Topology_LineStrip, Material.Topology_TriangleList, or "
      "Material.Topology_TriangleStrip.");

    MFUN(material_get_topology, "int", "topology");
    DOC_FUNC(
      "Get the primitive topology of the material. Material.Topology_PointList, "
      "Material.Topology_LineList, Material.Topology_LineStrip, "
      "Material.Topology_TriangleList, or Material.Topology_TriangleStrip.");

    MFUN(material_set_transparent, "void", "transparent");
    ARG("int", "is_transparent");

    MFUN(material_get_transparent, "int", "transparent");

    // uniforms

    // TODO
    // MFUN(material_uniform_remove, "void", "removeUniform");
    // ARG("int", "location");

    MFUN(material_uniform_active_locations, "int[]", "activeUniformLocations");
    DOC_FUNC(
      "Get list of active uniform locations, i.e. uniform locations that have been set "
      "(bind group entry locations)");

    MFUN(material_set_uniform_float, "void", "uniformFloat");
    ARG("int", "location");
    ARG("float", "uniform_value");
    DOC_FUNC("Set a float uniform value at the given location.");

    MFUN(material_get_uniform_float, "float", "uniformFloat");
    ARG("int", "location");
    DOC_FUNC("Get a float uniform value at the given location.");

    MFUN(material_set_uniform_float2, "void", "uniformFloat2");
    ARG("int", "location");
    ARG("vec2", "uniform_value");
    DOC_FUNC("Set a vec2 uniform value at the given location.");

    MFUN(material_get_uniform_float2, "vec2", "uniformFloat2");
    ARG("int", "location");
    DOC_FUNC("Get a vec2 uniform value at the given location.");

    MFUN(material_set_uniform_float3, "void", "uniformFloat3");
    ARG("int", "location");
    ARG("vec3", "uniform_value");
    DOC_FUNC("Set a vec3 uniform value at the given location.");

    MFUN(material_get_uniform_float3, "vec3", "uniformFloat3");
    ARG("int", "location");
    DOC_FUNC("Get a vec3 uniform value at the given location.");

    MFUN(material_set_uniform_float4, "void", "uniformFloat4");
    ARG("int", "location");
    ARG("vec4", "uniform_value");
    DOC_FUNC("Set a vec4 uniform value at the given location.");

    MFUN(material_get_uniform_float4, "vec4", "uniformFloat4");
    ARG("int", "location");
    DOC_FUNC("Get a vec4 uniform value at the given location.");

    MFUN(material_set_uniform_int, "void", "uniformInt");
    ARG("int", "location");
    ARG("int", "uniform_value");
    DOC_FUNC("Set an int uniform value at the given location.");

    MFUN(material_get_uniform_int, "int", "uniformInt");
    ARG("int", "location");
    DOC_FUNC("Get an int uniform value at the given location.");

    MFUN(material_set_uniform_int2, "void", "uniformInt2");
    ARG("int", "location");
    ARG("int", "x");
    ARG("int", "y");
    DOC_FUNC("Set an vec2<i32> uniform value at the given location.");

    MFUN(material_get_uniform_int2, "int[]", "uniformInt2");
    ARG("int", "location");
    DOC_FUNC("Get an vec2<i32> uniform value at the given location.");

    MFUN(material_set_uniform_int3, "void", "uniformInt3");
    ARG("int", "location");
    ARG("int", "x");
    ARG("int", "y");
    ARG("int", "z");
    DOC_FUNC("Set an vec3<i32> uniform value at the given location.");

    MFUN(material_get_uniform_int3, "int[]", "uniformInt3");
    ARG("int", "location");
    DOC_FUNC("Get an vec3<i32> uniform value at the given location.");

    MFUN(material_set_uniform_int4, "void", "uniformInt4");
    ARG("int", "location");
    ARG("int", "x");
    ARG("int", "y");
    ARG("int", "z");
    ARG("int", "w");
    DOC_FUNC("Set an vec4<i32> uniform value at the given location.");

    MFUN(material_get_uniform_int4, "int[]", "uniformInt4");
    ARG("int", "location");
    DOC_FUNC("Get an vec4<i32> uniform value at the given location.");

    // storage buffers
    MFUN(material_set_storage_buffer, "void", "storageBuffer");
    ARG("int", "location");
    ARG("float[]", "storageBuffer");
    DOC_FUNC("Bind the given array data as a storage buffer at the given location.");

    MFUN(material_set_storage_buffer_vec2, "void", "storageBuffer");
    ARG("int", "location");
    ARG("vec2[]", "storageBuffer");
    DOC_FUNC("Bind the given array data as a storage buffer at the given location.");

    MFUN(material_set_storage_buffer_vec3, "void", "storageBuffer");
    ARG("int", "location");
    ARG("vec3[]", "storageBuffer");
    DOC_FUNC("Bind the given array data as a storage buffer at the given location.");

    MFUN(material_set_storage_buffer_vec4, "void", "storageBuffer");
    ARG("int", "location");
    ARG("vec4[]", "storageBuffer");
    DOC_FUNC("Bind the given array data as a storage buffer at the given location.");

    MFUN(material_set_storage_buffer_integer, "void", "storageBuffer");
    ARG("int", "location");
    ARG("int[]", "storageBuffer");
    DOC_FUNC("Bind the given array data as a storage buffer at the given location.");

    // external storage buffer
    MFUN(material_set_storage_buffer_external, "void", "storageBuffer");
    ARG("int", "location");
    ARG("StorageBuffer", "storageBuffer");
    DOC_FUNC("Bind a storage buffer at the given location.");

    MFUN(material_set_sampler, "void", "sampler");
    ARG("int", "location");
    ARG("TextureSampler", "sampler");
    DOC_FUNC("Bind a sampler at the given location.");

    MFUN(material_get_sampler, "TextureSampler", "sampler");
    ARG("int", "location");
    DOC_FUNC("Get the sampler at the given location.");

    MFUN(material_set_texture, "void", "texture");
    ARG("int", "location");
    ARG("Texture", "texture");
    DOC_FUNC("Bind a texture at the given location.");

    MFUN(material_get_texture, "Texture", "texture");
    ARG("int", "location");
    DOC_FUNC("Get the texture at the given location.");

    MFUN(material_set_storage_texture, "void", "storageTexture");
    ARG("int", "location");
    ARG("Texture", "texture");
    DOC_FUNC(
      "Binds a storage texture at the given location. Defaults to the textures base "
      "mip level 0.");

    // abstract class, no destructor or constructor
    END_CLASS();

    // Lines2DMaterial -----------------------------------------------------
    BEGIN_CLASS(SG_MaterialTypeNames[SG_MATERIAL_LINES2D],
                SG_CKNames[SG_COMPONENT_MATERIAL]);
    DOC_CLASS(
      "2D line material. You probably don't need to instantiate this directly. Use "
      "GLines instead");

    CTOR(lines2d_material_ctor);

    MFUN(lines2d_material_get_thickness, "float", "width");
    DOC_FUNC("Get the thickness of the lines in the material.");

    MFUN(lines2d_material_set_thickness, "void", "width");
    ARG("float", "thickness");
    DOC_FUNC("Set the thickness of the lines in the material.");

    MFUN(lines2d_material_get_color, "vec3", "color");
    DOC_FUNC("Get the line color");

    MFUN(lines2d_material_set_color, "void", "color");
    ARG("vec3", "color");
    DOC_FUNC("Set the line color");

    // MFUN(lines2d_material_get_extrusion, "float", "extrusion");
    // DOC_FUNC(
    //   "Get the miter extrusion ratio of the line. Varies from 0.0 to 1.0. A value of
    //   " "0.5 means the line width is split evenly on each side of each line segment "
    //   "position.");

    // MFUN(lines2d_material_set_extrusion, "void", "extrusion");
    // ARG("float", "extrusion");
    // DOC_FUNC(
    //   "Set the miter extrusion ratio of the line. Varies from 0.0 to 1.0. A value of
    //   " "0.5 means the line width is split evenly on each side of each line segment "
    //   "position.");

    // MFUN(lines2d_material_get_loop, "int", "loop");
    // DOC_FUNC("Get whether the line segments form a closed loop");

    // MFUN(lines2d_material_set_loop, "void", "loop");
    // ARG("int", "loop");
    // DOC_FUNC(
    //   "Set whether the line segments form a closed loop. Set via material.loop(true)
    //   " "or material.loop(false)");

    END_CLASS();

    // FlatMaterial -----------------------------------------------------
    {
        BEGIN_CLASS(SG_MaterialTypeNames[SG_MATERIAL_FLAT],
                    SG_CKNames[SG_COMPONENT_MATERIAL]);
        DOC_CLASS("Simple flat-shaded material (not affected by lighting).");
        ADD_EX("deep/sprite_animation.ck");

        CTOR(flat_material_ctor);

        // color uniform
        MFUN(flat_material_get_color, "vec3", "color");
        DOC_FUNC("Get the rgba color of the material.");

        MFUN(flat_material_get_alpha, "float", "alpha");
        DOC_FUNC("Get the alpha-channel of the material's color");

        MFUN(flat_material_set_alpha, "void", "alpha");
        ARG("float", "alpha");
        DOC_FUNC("Set the alpha-channel of the material's color");

        MFUN(flat_material_set_color, "void", "color");
        ARG("vec3", "color");
        DOC_FUNC("Set material color uniform as an rgb. Alpha set to 1.0.");

        MFUN(flat_material_set_color_rgba, "void", "color");
        ARG("vec4", "color");
        DOC_FUNC("Set rgba the material color");

        MFUN(flat_material_get_sampler, "TextureSampler", "sampler");
        DOC_FUNC("Get the sampler of the material.");

        MFUN(flat_material_set_sampler, "void", "sampler");
        ARG("TextureSampler", "sampler");
        DOC_FUNC("Set the sampler of the material.");

        MFUN(flat_material_get_color_map, "Texture", "colorMap");
        DOC_FUNC("Get the color map of the material.");

        MFUN(flat_material_set_color_map, "void", "colorMap");
        ARG("Texture", "colorMap");
        DOC_FUNC("Set the color map of the material.");

        MFUN(flat_material_set_texture_offset, "void", "offset");
        ARG("vec2", "offset");
        DOC_FUNC(
          "Set the texture sampler offset of the material. Default (0, 0). Useful for "
          "scrolling textures or sampling a subregion of a texture atlas. E.g. An "
          "offset of (0.5, 0.5) will begin sampling from the center of the texture.");

        MFUN(flat_material_get_texture_offset, "vec2", "offset");
        DOC_FUNC("Get the texture sampler offset of the material.");

        MFUN(flat_material_set_texture_scale, "void", "scale");
        ARG("vec2", "scale");
        DOC_FUNC(
          "Set the texture sampler scale of the material. Default (1, 1). Useful for "
          "scaling textures or sampling a subregion of a texture atlas. E.g. A scale "
          "of (0.5, 0.5) will sample 1/4 of the texture.");

        MFUN(flat_material_get_texture_scale, "vec2", "scale");
        DOC_FUNC("Get the texture sampler scale of the material.");

        END_CLASS();
    }

    // UV Material -----------------------------------------------------

    BEGIN_CLASS(SG_MaterialTypeNames[SG_MATERIAL_UV],
                SG_CKNames[SG_COMPONENT_MATERIAL]);
    DOC_CLASS("Visualize UV coordinates of a mesh.");

    CTOR(uv_material_ctor);

    END_CLASS();

    // Normal Material -----------------------------------------------------

    BEGIN_CLASS(SG_MaterialTypeNames[SG_MATERIAL_NORMAL],
                SG_CKNames[SG_COMPONENT_MATERIAL]);
    DOC_CLASS("Visualize normals of a mesh.");

    CTOR(normal_material_ctor);

    MFUN(normal_material_set_worldspace_normals, "void", "worldspaceNormals");
    ARG("int", "use_worldspace_normals");
    DOC_FUNC(
      "Set whether to use worldspace normals. If false, visualizes normals in local "
      "object space.");

    MFUN(normal_material_get_worldspace_normals, "int", "worldspaceNormals");
    DOC_FUNC(
      "Get whether to use worldspace normals. If false, visualizes normals in local "
      "object space.");

    END_CLASS();

    // Wireframe Material -----------------------------------------------------
    {

        BEGIN_CLASS(SG_MaterialTypeNames[SG_MATERIAL_WIREFRAME],
                    SG_CKNames[SG_COMPONENT_MATERIAL]);
        DOC_CLASS(
          "View a mesh as wireframe. Uses barycentric coordinates to draw lines. "
          "WARNING: May not work with indexed geometry, rendering some or all faces "
          "incorrectly as solid. Known issues with SphereGeometry, CircleGeometry, "
          "KnotGeometry, CylinderGeometry. In those cases, set Material.topology() to "
          "Material.Topology_LineList or Material.Topology_LineStrip. You will no "
          "longer be able to set thickness, but the wireframe will render correctly.");
        // TODO problem is that indexed draws may assign every vertex of a triangle the
        // same barycentric coordinates, so the wireframe will be drawn as a solid
        // triangle. possible workaround: switch to vertex-pulling buffers, and compute
        // the proper vertex index, as in
        // https://webgpu.github.io/webgpu-samples/?sample=wireframe#main.ts

        CTOR(wireframe_material_ctor);

        MFUN(wireframe_material_set_thickness, "void", "thickness");
        ARG("float", "thickness");
        DOC_FUNC("Set the thickness of the wireframe lines.");

        MFUN(wireframe_material_get_thickness, "float", "thickness");
        DOC_FUNC("Get the thickness of the wireframe lines.");

        MFUN(wireframe_material_set_alpha_cutoff, "void", "alphaCutoff");
        ARG("float", "alphaCutoff");
        DOC_FUNC(
          "Set the alpha cutoff value for the wireframe material. Default 0.5. "
          "Fragments with alpha values below this threshold will be discarded.");

        MFUN(wireframe_material_get_alpha_cutoff, "float", "alphaCutoff");
        DOC_FUNC("Get the alpha cutoff value for the wireframe material.");

        MFUN(wireframe_material_set_color, "void", "color");
        ARG("vec3", "color");
        DOC_FUNC("Set the color of the wireframe lines.");

        MFUN(wireframe_material_get_color, "vec3", "color");
        DOC_FUNC("Get the color of the wireframe lines.");

        END_CLASS();
    }

    // Phong Material -----------------------------------------------------
    {
        BEGIN_CLASS(SG_MaterialTypeNames[SG_MATERIAL_PHONG],
                    SG_CKNames[SG_COMPONENT_MATERIAL]);
        DOC_CLASS("Phong specular shading model");
        ADD_EX("basic/skybox.ck");

        CTOR(phong_material_ctor);

        PHONG_MATERIAL_METHODS(phong);

        // static methods
        static t_CKUINT envmap_method_none       = SG_ENVMAP_SAMPLE_NONE;
        static t_CKUINT envmap_method_reflection = SG_ENVMAP_SAMPLE_REFLECT;
        static t_CKUINT envmap_method_refraction = SG_ENVMAP_SAMPLE_REFRACT;

        static t_CKUINT envmap_blend_none     = SG_ENVMAP_BLEND_NONE;
        static t_CKUINT envmap_blend_add      = SG_ENVMAP_BLEND_ADD;
        static t_CKUINT envmap_blend_multiply = SG_ENVMAP_BLEND_MULTIPLY;
        static t_CKUINT envmap_blend_mix      = SG_ENVMAP_BLEND_MIX;

        SVAR("int", "EnvmapMethod_None", &envmap_method_none);
        DOC_VAR("No environment map sampling.");
        SVAR("int", "EnvmapMethod_Reflection", &envmap_method_reflection);
        DOC_VAR("Sample the environment map via reflection (like chrome or a mirror)");
        SVAR("int", "EnvmapMethod_Refraction", &envmap_method_refraction);
        DOC_VAR("Sample the environment map via refraction (like glass or water)");

        SVAR("int", "EnvmapBlend_None", &envmap_blend_none);
        DOC_VAR("No environment map blending.");
        SVAR("int", "EnvmapBlend_Add", &envmap_blend_add);
        DOC_VAR("Add the environment map color to the material color.");
        SVAR("int", "EnvmapBlend_Multiply", &envmap_blend_multiply);
        DOC_VAR("Multiply the environment map color with the material color.");
        SVAR("int", "EnvmapBlend_Mix", &envmap_blend_mix);
        DOC_VAR(
          "Linearly mix the environment map color with the material color. An "
          "intensity of 0.0 will show only the material color, 1.0 will show only the "
          "environment map color.");

        END_CLASS();
    }

    // PBR Material -----------------------------------------------------
    {
        BEGIN_CLASS(SG_MaterialTypeNames[SG_MATERIAL_PBR],
                    SG_CKNames[SG_COMPONENT_MATERIAL]);
        ADD_EX("basic/pbr.ck");

        CTOR(pbr_material_ctor);

        MFUN(pbr_material_get_albedo, "vec3", "color");
        DOC_FUNC("Get the albedo color of the material.");

        MFUN(pbr_material_set_albedo, "void", "color");
        ARG("vec3", "albedo");
        DOC_FUNC("Set the albedo color of the material.");

        MFUN(pbr_material_get_emissive, "vec3", "emissive");
        DOC_FUNC("Get the emissive color of the material.");

        MFUN(pbr_material_set_emissive, "void", "emissive");
        ARG("vec3", "emissive");
        DOC_FUNC("Set the emissive color of the material.");

        MFUN(pbr_material_get_metallic, "float", "metallic");
        DOC_FUNC("Get the metallic factor of the material.");

        MFUN(pbr_material_set_metallic, "void", "metallic");
        ARG("float", "metallic");

        MFUN(pbr_material_get_roughness, "float", "roughness");
        DOC_FUNC("Get the roughness factor of the material.");

        MFUN(pbr_material_set_roughness, "void", "roughness");
        ARG("float", "roughness");
        DOC_FUNC("Set the roughness factor of the material.");

        MFUN(pbr_material_get_normal_factor, "float", "normalFactor");
        DOC_FUNC(
          "Get the normal factor of the material. Scales effect of normal map. Default "
          "1.0");

        MFUN(pbr_material_set_normal_factor, "void", "normalFactor");
        ARG("float", "normalFactor");
        DOC_FUNC(
          "Set the normal factor of the material. Scales effect of normal map. Default "
          "1.0");

        MFUN(pbr_material_get_ao_factor, "float", "aoFactor");
        DOC_FUNC("Get the ambient occlusion factor of the material. Default 1.0");

        MFUN(pbr_material_set_ao_factor, "void", "aoFactor");
        ARG("float", "aoFactor");
        DOC_FUNC("Set the ambient occlusion factor of the material. Default 1.0");

        MFUN(pbr_material_get_albedo_tex, SG_CKNames[SG_COMPONENT_TEXTURE], "colorMap");
        DOC_FUNC("Get the albedo texture of the material.");

        MFUN(pbr_material_set_albedo_tex, "void", "colorMap");
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "albedoTexture");
        DOC_FUNC("Set the albedo texture of the material.");

        MFUN(pbr_material_get_normal_tex, SG_CKNames[SG_COMPONENT_TEXTURE],
             "normalMap");
        DOC_FUNC("Get the normal texture of the material.");

        MFUN(pbr_material_set_normal_tex, "void", "normalMap");
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "normalTexture");
        DOC_FUNC("Set the normal texture of the material.");

        MFUN(pbr_material_get_ao_tex, SG_CKNames[SG_COMPONENT_TEXTURE], "aoMap");
        DOC_FUNC("Get the ambient occlusion texture of the material.");

        MFUN(pbr_material_set_ao_tex, "void", "aoMap");
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "aoTexture");
        DOC_FUNC("Set the ambient occlusion texture of the material.");

        MFUN(pbr_material_get_mr_tex, SG_CKNames[SG_COMPONENT_TEXTURE], "mrMap");
        DOC_FUNC("Get the metallic-roughness texture of the material.");

        MFUN(pbr_material_set_mr_tex, "void", "mrMap");
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "mrTexture");
        DOC_FUNC("Set the metallic-roughness texture of the material.");

        MFUN(pbr_material_get_emissive_tex, SG_CKNames[SG_COMPONENT_TEXTURE],
             "emissiveMap");
        DOC_FUNC("Get the emissive texture of the material.");

        MFUN(pbr_material_set_emissive_tex, "void", "emissiveMap");
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "emissiveTexture");
        DOC_FUNC("Set the emissive texture of the material.");

        // abstract class, no destructor or constructor
        END_CLASS();
    }

    { // SkyboxMaterial

        BEGIN_CLASS(SG_MaterialTypeNames[SG_MATERIAL_SKYBOX],
                    SG_CKNames[SG_COMPONENT_MATERIAL]);

        CTOR(skybox_material_ctor);

        END_CLASS();
    }

    // initialize default components
    chugl_initDefaultMaterials();
}

// Shader ===================================================================

CK_DLL_CTOR(shader_desc_ctor)
{
    // chuck doesn't initialize class member vars in constructor. create manually
    // NOTE: these should be reference counted (add_ref=true)
    // create string members (empty string)
    OBJ_MEMBER_STRING(SELF, shader_desc_vertex_string_offset)
      = chugin_createCkString("", true);
    OBJ_MEMBER_STRING(SELF, shader_desc_fragment_string_offset)
      = chugin_createCkString("", true);
    OBJ_MEMBER_STRING(SELF, shader_desc_vertex_filepath_offset)
      = chugin_createCkString("", true);
    OBJ_MEMBER_STRING(SELF, shader_desc_fragment_filepath_offset)
      = chugin_createCkString("", true);
    OBJ_MEMBER_STRING(SELF, shader_desc_compute_string_offset)
      = chugin_createCkString("", true);
    OBJ_MEMBER_STRING(SELF, shader_desc_compute_filepath_offset)
      = chugin_createCkString("", true);

    WGPUVertexFormat default_format[]
      = { WGPUVertexFormat_Float32x3, WGPUVertexFormat_Float32x3,
          WGPUVertexFormat_Float32x2 };

    OBJ_MEMBER_INT_ARRAY(SELF, shader_desc_vertex_layout_offset)
      = chugin_createCkIntArray((int*)default_format, ARRAY_LENGTH(default_format),
                                true);
}

CK_DLL_CTOR(shader_ctor_default)
{
    CK_THROW("ShaderConstructor",
             "Shader default constructor not allowed. use Shader(ShaderDesc) instead",
             SHRED);
}

CK_DLL_CTOR(shader_ctor)
{
    Chuck_Object* shader_desc = GET_NEXT_OBJECT(ARGS);

    WGPUVertexFormat vertex_layout[SG_GEOMETRY_MAX_VERTEX_ATTRIBUTES] = {};
    int vertex_layout_len = chugin_copyCkIntArray(
      OBJ_MEMBER_INT_ARRAY(shader_desc, shader_desc_vertex_layout_offset),
      (int*)vertex_layout, ARRAY_LENGTH(vertex_layout));

    // log_error("vertex layout len %d", vertex_layout_len);

    // validate vertex layouts
    bool passed_undefined_format = false;
    for (int i = 0; i < vertex_layout_len; i++) {
        WGPUVertexFormat format = vertex_layout[i];

        bool format_is_undefined   = (format == WGPUVertexFormat_Undefined);
        bool format_is_valid_float = (format >= WGPUVertexFormat_Float32
                                      && format <= WGPUVertexFormat_Float32x4);
        bool format_is_valid_int
          = (format >= WGPUVertexFormat_Sint32 && format <= WGPUVertexFormat_Sint32x4);

        if (format_is_undefined) passed_undefined_format = true;

        // vertex formats must be contiguous
        // e.g. [Float2, Undefined, Float3] is NOT allowed
        if (passed_undefined_format && !format_is_undefined) {
            // TODO this is incorrectly triggered in RELEASE mode on mac
            // log_error("error on index %d.", i);
            // for (int tmp = 0; tmp < vertex_layout_len; tmp++) {
            //     log_error("vertexFormat %d: %d", tmp, vertex_layout[tmp]);
            // }
            // log_warn(
            //   "VertexFormats given to ShaderDesc.vertexLayout must be contiguous ");
            // log_warn(
            //   "  | E.g. [VertexFormat.Float, VertexFormat.Undefined, "
            //   "VertexFormat.Float] is not allowed ");
        }

        if (!(format_is_valid_float || format_is_valid_int || format_is_undefined)) {
            log_warn("Invalid VertexFormat %d given to ShaderDesc.vertexLayout ",
                     format);
            log_warn(
              "  | Valid formats include VertexFormat.Float, VertexFormat.Int, "
              "VertexFormat.Int2 etc. ");
        }
    }

    SG_ShaderIncludes includes = {};
    includes.lit               = OBJ_MEMBER_INT(shader_desc, shader_desc_is_lit);
    includes.uses_env_map      = OBJ_MEMBER_INT(shader_desc, shader_desc_uses_envmap);

    // create shader on audio side
    SG_Shader* shader = SG_CreateShader(
      SELF,
      API->object->str(
        OBJ_MEMBER_STRING(shader_desc, shader_desc_vertex_string_offset)),
      API->object->str(
        OBJ_MEMBER_STRING(shader_desc, shader_desc_fragment_string_offset)),
      API->object->str(
        OBJ_MEMBER_STRING(shader_desc, shader_desc_vertex_filepath_offset)),
      API->object->str(
        OBJ_MEMBER_STRING(shader_desc, shader_desc_fragment_filepath_offset)),
      vertex_layout, vertex_layout_len,
      API->object->str(
        OBJ_MEMBER_STRING(shader_desc, shader_desc_compute_string_offset)),
      API->object->str(
        OBJ_MEMBER_STRING(shader_desc, shader_desc_compute_filepath_offset)),
      includes);

    // save component id
    OBJ_MEMBER_UINT(SELF, component_offset_id) = shader->id;

    // push to command queue
    CQ_PushCommand_ShaderCreate(shader);
}

CK_DLL_MFUN(shader_get_vertex_string)
{
    SG_Shader* shader = GET_SHADER(SELF);
    // return new string (no ref count needed; chuck VM function call mechanism will
    // take it from here)
    RETURN->v_string = chugin_createCkString(shader->vertex_string_owned, false);
}

CK_DLL_MFUN(shader_get_fragment_string)
{
    SG_Shader* shader = GET_SHADER(SELF);
    // return new string (no ref count needed; chuck VM function call mechanism will
    // take it from here)
    RETURN->v_string = chugin_createCkString(shader->fragment_string_owned, false);
}

CK_DLL_MFUN(shader_get_vertex_filepath)
{
    SG_Shader* shader = GET_SHADER(SELF);
    // return new string (no ref count needed; chuck VM function call mechanism will
    // take it from here)
    RETURN->v_string = chugin_createCkString(shader->vertex_filepath_owned, false);
}

CK_DLL_MFUN(shader_get_fragment_filepath)
{
    SG_Shader* shader = GET_SHADER(SELF);
    // return new string (no ref count needed; chuck VM function call mechanism will
    // take it from here)
    RETURN->v_string = chugin_createCkString(shader->fragment_filepath_owned, false);
}

CK_DLL_MFUN(shader_get_vertex_layout)
{
    SG_Shader* shader = GET_SHADER(SELF);
    RETURN->v_object  = (Chuck_Object*)chugin_createCkIntArray(
      (int*)shader->vertex_layout, ARRAY_LENGTH(shader->vertex_layout));
}

CK_DLL_MFUN(shader_get_lit)
{
    SG_Shader* shader = GET_SHADER(SELF);
    RETURN->v_int     = (t_CKINT)shader->includes.lit ? 1 : 0;
}

CK_DLL_MFUN(shader_get_uses_envmap)
{
    SG_Shader* shader = GET_SHADER(SELF);
    RETURN->v_int     = (t_CKINT)shader->includes.uses_env_map ? 1 : 0;
}

// Material ===================================================================

CK_DLL_CTOR(material_ctor)
{
    // we don't use the ulib_material_create here because Material is inherited by
    // other material types, which need different initizialization

    SG_Material* material = SG_CreateMaterial(SELF, SG_MATERIAL_CUSTOM);
    ASSERT(material->type == SG_COMPONENT_MATERIAL);

    OBJ_MEMBER_UINT(SELF, component_offset_id) = material->id;

    CQ_PushCommand_MaterialCreate(material);
}

CK_DLL_CTOR(material_ctor_with_shader)
{
    Chuck_Object* shader_ckobj = GET_NEXT_OBJECT(ARGS);
    SG_Shader* shader          = GET_SHADER(shader_ckobj);

    SG_Material* material = SG_CreateMaterial(SELF, SG_MATERIAL_CUSTOM);
    OBJ_MEMBER_UINT(SELF, component_offset_id) = material->id;

    CQ_PushCommand_MaterialCreate(material);

    ulib_material_set_shader(material, shader);
}

CK_DLL_MFUN(material_get_shader)
{
    SG_Material* material = GET_MATERIAL(SELF);
    SG_Shader* shader     = SG_GetShader(material->pso.sg_shader_id);
    RETURN->v_object      = shader ? shader->ckobj : NULL;
}

void ulib_material_set_shader(SG_Material* material, SG_Shader* shader)
{
    SG_Material::shader(material, shader);
    CQ_PushCommand_MaterialUpdatePSO(material);
}

CK_DLL_MFUN(material_set_shader)
{
    Chuck_Object* shader  = GET_NEXT_OBJECT(ARGS);
    SG_Material* material = GET_MATERIAL(SELF);

    ulib_material_set_shader(material, GET_SHADER(shader));
}

CK_DLL_MFUN(material_get_cullmode)
{
    SG_Material* material = GET_MATERIAL(SELF);
    RETURN->v_int         = (t_CKINT)material->pso.cull_mode;
}

CK_DLL_MFUN(material_set_cullmode)
{
    SG_Material* material   = GET_MATERIAL(SELF);
    t_CKINT cull_mode       = GET_NEXT_INT(ARGS);
    material->pso.cull_mode = (WGPUCullMode)cull_mode;

    CQ_PushCommand_MaterialUpdatePSO(material);
}

CK_DLL_MFUN(material_set_topology)
{
    SG_Material* material            = GET_MATERIAL(SELF);
    t_CKINT primitive_topology       = GET_NEXT_INT(ARGS);
    material->pso.primitive_topology = (WGPUPrimitiveTopology)primitive_topology;

    CQ_PushCommand_MaterialUpdatePSO(material);
}

CK_DLL_MFUN(material_get_topology)
{
    SG_Material* material = GET_MATERIAL(SELF);
    RETURN->v_int         = (t_CKINT)material->pso.primitive_topology;
}

CK_DLL_MFUN(material_set_transparent)
{
    SG_Material* material     = GET_MATERIAL(SELF);
    material->pso.transparent = GET_NEXT_INT(ARGS) ? 1 : 0;
    CQ_PushCommand_MaterialUpdatePSO(material);
}

CK_DLL_MFUN(material_get_transparent)
{
    SG_Material* material = GET_MATERIAL(SELF);
    RETURN->v_int         = material->pso.transparent;
}

CK_DLL_MFUN(material_uniform_active_locations)
{
    SG_Material* material = GET_MATERIAL(SELF);

    int active_locations[CHUGL_MATERIAL_MAX_BINDINGS];
    int active_locations_count = 0;

    for (int i = 0; i < CHUGL_MATERIAL_MAX_BINDINGS; i++) {
        if (material->uniforms[i].type != SG_MATERIAL_UNIFORM_NONE) {
            active_locations[active_locations_count++] = i;
        }
    }

    RETURN->v_object = (Chuck_Object*)chugin_createCkIntArray(active_locations,
                                                              active_locations_count);
}

CK_DLL_MFUN(material_uniform_remove)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);

    SG_Material::removeUniform(material, location);

    // TODO push to command queue
}

CK_DLL_MFUN(material_set_uniform_float)
{
    SG_Material* material   = GET_MATERIAL(SELF);
    t_CKINT location        = GET_NEXT_INT(ARGS);
    t_CKFLOAT uniform_value = GET_NEXT_FLOAT(ARGS);
    float uniform_value_f32 = (float)uniform_value;

    SG_Material::setUniform(material, location, &uniform_value_f32,
                            SG_MATERIAL_UNIFORM_FLOAT);

    CQ_PushCommand_MaterialSetUniform(material, location);
}

CK_DLL_MFUN(material_get_uniform_float)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);

    if (material->uniforms[location].type != SG_MATERIAL_UNIFORM_FLOAT) {
        CK_THROW("MaterialGetUniformFloat", "Uniform location is not a float", SHRED);
    }

    RETURN->v_float = material->uniforms[location].as.f;
}

CK_DLL_MFUN(material_set_uniform_float2)
{
    SG_Material* material       = GET_MATERIAL(SELF);
    t_CKINT location            = GET_NEXT_INT(ARGS);
    t_CKVEC2 uniform_value      = GET_NEXT_VEC2(ARGS);
    glm::vec2 uniform_value_f32 = { uniform_value.x, uniform_value.y };

    SG_Material::setUniform(material, location, &uniform_value_f32,
                            SG_MATERIAL_UNIFORM_VEC2F);

    CQ_PushCommand_MaterialSetUniform(material, location);
}

CK_DLL_MFUN(material_get_uniform_float2)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);

    if (material->uniforms[location].type != SG_MATERIAL_UNIFORM_VEC2F) {
        CK_THROW("MaterialGetUniformFloat2", "Uniform location is not a vec2", SHRED);
    }

    glm::vec2 uniform_value = material->uniforms[location].as.vec2f;
    RETURN->v_vec2          = { uniform_value.x, uniform_value.y };
}

CK_DLL_MFUN(material_set_uniform_float3)
{
    SG_Material* material       = GET_MATERIAL(SELF);
    t_CKINT location            = GET_NEXT_INT(ARGS);
    t_CKVEC3 uniform_value      = GET_NEXT_VEC3(ARGS);
    glm::vec3 uniform_value_f32 = { uniform_value.x, uniform_value.y, uniform_value.z };

    SG_Material::setUniform(material, location, &uniform_value_f32,
                            SG_MATERIAL_UNIFORM_VEC3F);

    CQ_PushCommand_MaterialSetUniform(material, location);
}

CK_DLL_MFUN(material_get_uniform_float3)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);

    if (material->uniforms[location].type != SG_MATERIAL_UNIFORM_VEC3F) {
        CK_THROW("MaterialGetUniformFloat3", "Uniform location is not a vec3", SHRED);
    }

    glm::vec3 uniform_value = material->uniforms[location].as.vec3f;
    RETURN->v_vec3          = { uniform_value.x, uniform_value.y, uniform_value.z };
}

CK_DLL_MFUN(material_set_uniform_float4)
{
    SG_Material* material  = GET_MATERIAL(SELF);
    t_CKINT location       = GET_NEXT_INT(ARGS);
    t_CKVEC4 uniform_value = GET_NEXT_VEC4(ARGS);
    glm::vec4 uniform_value_f32
      = { uniform_value.x, uniform_value.y, uniform_value.z, uniform_value.w };

    SG_Material::setUniform(material, location, &uniform_value_f32,
                            SG_MATERIAL_UNIFORM_VEC4F);

    CQ_PushCommand_MaterialSetUniform(material, location);
}

CK_DLL_MFUN(material_get_uniform_float4)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);

    if (material->uniforms[location].type != SG_MATERIAL_UNIFORM_VEC4F) {
        CK_THROW("MaterialGetUniformFloat4", "Uniform location is not a vec4", SHRED);
    }

    glm::vec4 uniform_value = material->uniforms[location].as.vec4f;
    RETURN->v_vec4
      = { uniform_value.x, uniform_value.y, uniform_value.z, uniform_value.w };
}

CK_DLL_MFUN(material_set_uniform_int)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);
    t_CKINT uniform_value = GET_NEXT_INT(ARGS);
    i32 uniform_value_i32 = (i32)uniform_value;

    SG_Material::setUniform(material, location, &uniform_value_i32,
                            SG_MATERIAL_UNIFORM_INT);

    CQ_PushCommand_MaterialSetUniform(material, location);
}

CK_DLL_MFUN(material_get_uniform_int)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);

    if (material->uniforms[location].type != SG_MATERIAL_UNIFORM_INT) {
        CK_THROW("MaterialGetUniformInt", "Uniform location is not an int", SHRED);
    }

    RETURN->v_int = material->uniforms[location].as.i;
}

CK_DLL_MFUN(material_set_uniform_int2)
{
    SG_Material* material    = GET_MATERIAL(SELF);
    t_CKINT location         = GET_NEXT_INT(ARGS);
    t_CKINT x                = GET_NEXT_INT(ARGS);
    t_CKINT y                = GET_NEXT_INT(ARGS);
    glm::ivec2 uniform_value = { x, y };

    SG_Material::setUniform(material, location, &uniform_value,
                            SG_MATERIAL_UNIFORM_IVEC2);

    CQ_PushCommand_MaterialSetUniform(material, location);
}

CK_DLL_MFUN(material_get_uniform_int2)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);

    if (material->uniforms[location].type != SG_MATERIAL_UNIFORM_IVEC2) {
        CK_THROW("MaterialGetUniformInt2", "Uniform location is not an int2", SHRED);
    }

    RETURN->v_object = (Chuck_Object*)chugin_createCkIntArray(
      (int*)&material->uniforms[location].as.ivec2, 2);
}

CK_DLL_MFUN(material_set_uniform_int3)
{
    SG_Material* material    = GET_MATERIAL(SELF);
    t_CKINT location         = GET_NEXT_INT(ARGS);
    t_CKINT x                = GET_NEXT_INT(ARGS);
    t_CKINT y                = GET_NEXT_INT(ARGS);
    t_CKINT z                = GET_NEXT_INT(ARGS);
    glm::ivec3 uniform_value = { x, y, z };

    SG_Material::setUniform(material, location, &uniform_value,
                            SG_MATERIAL_UNIFORM_IVEC3);

    CQ_PushCommand_MaterialSetUniform(material, location);
}

CK_DLL_MFUN(material_get_uniform_int3)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);

    if (material->uniforms[location].type != SG_MATERIAL_UNIFORM_IVEC3) {
        CK_THROW("MaterialGetUniformInt3", "Uniform location is not an int3", SHRED);
    }

    RETURN->v_object = (Chuck_Object*)chugin_createCkIntArray(
      (int*)&material->uniforms[location].as.ivec3, 3);
}

CK_DLL_MFUN(material_set_uniform_int4)
{
    SG_Material* material    = GET_MATERIAL(SELF);
    t_CKINT location         = GET_NEXT_INT(ARGS);
    t_CKINT x                = GET_NEXT_INT(ARGS);
    t_CKINT y                = GET_NEXT_INT(ARGS);
    t_CKINT z                = GET_NEXT_INT(ARGS);
    t_CKINT w                = GET_NEXT_INT(ARGS);
    glm::ivec4 uniform_value = { x, y, z, w };

    SG_Material::setUniform(material, location, &uniform_value,
                            SG_MATERIAL_UNIFORM_IVEC4);

    CQ_PushCommand_MaterialSetUniform(material, location);
}

CK_DLL_MFUN(material_get_uniform_int4)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);

    if (material->uniforms[location].type != SG_MATERIAL_UNIFORM_IVEC4) {
        CK_THROW("MaterialGetUniformInt4", "Uniform location is not an int4", SHRED);
    }

    RETURN->v_object = (Chuck_Object*)chugin_createCkIntArray(
      (int*)&material->uniforms[location].as.ivec4, 4);
}

CK_DLL_MFUN(material_set_storage_buffer)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);
    Chuck_Object* ck_arr  = GET_NEXT_OBJECT(ARGS);

    SG_Material::setStorageBuffer(material, location);

    CQ_PushCommand_MaterialSetStorageBuffer(material, location, ck_arr,
                                            SG_MATERIAL_UNIFORM_FLOAT);
}

CK_DLL_MFUN(material_set_storage_buffer_vec2)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);
    Chuck_Object* ck_arr  = GET_NEXT_OBJECT(ARGS);

    SG_Material::setStorageBuffer(material, location);

    CQ_PushCommand_MaterialSetStorageBuffer(material, location, ck_arr,
                                            SG_MATERIAL_UNIFORM_VEC2F);
}

CK_DLL_MFUN(material_set_storage_buffer_vec3)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);
    Chuck_Object* ck_arr  = GET_NEXT_OBJECT(ARGS);

    SG_Material::setStorageBuffer(material, location);

    CQ_PushCommand_MaterialSetStorageBuffer(material, location, ck_arr,
                                            SG_MATERIAL_UNIFORM_VEC3F);
}

CK_DLL_MFUN(material_set_storage_buffer_vec4)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);
    Chuck_Object* ck_arr  = GET_NEXT_OBJECT(ARGS);

    SG_Material::setStorageBuffer(material, location);

    CQ_PushCommand_MaterialSetStorageBuffer(material, location, ck_arr,
                                            SG_MATERIAL_UNIFORM_VEC4F);
}

CK_DLL_MFUN(material_set_storage_buffer_integer)
{
    SG_Material* material  = GET_MATERIAL(SELF);
    t_CKINT location       = GET_NEXT_INT(ARGS);
    Chuck_ArrayInt* ck_arr = GET_NEXT_INT_ARRAY(ARGS);

    SG_Material::setStorageBuffer(material, location);

    CQ_PushCommand_MaterialSetStorageBuffer(material, location, (Chuck_Object*)ck_arr,
                                            SG_MATERIAL_UNIFORM_INT);
}

CK_DLL_MFUN(material_set_storage_buffer_external)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);
    Chuck_Object* ckobj   = GET_NEXT_OBJECT(ARGS);
    SG_Buffer* buffer     = SG_GetBuffer(OBJ_MEMBER_UINT(ckobj, component_offset_id));

    SG_Material::storageBuffer(material, location, buffer);

    CQ_PushCommand_MaterialSetUniform(material, location);
}

CK_DLL_MFUN(material_set_sampler)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);
    SG_Sampler sampler    = SG_Sampler::fromCkObj(GET_NEXT_OBJECT(ARGS));

    SG_Material::setSampler(material, location, sampler);

    CQ_PushCommand_MaterialSetUniform(material, location);
}

CK_DLL_MFUN(material_get_sampler)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);

    if (material->uniforms[location].type != SG_MATERIAL_UNIFORM_SAMPLER) {
        CK_THROW("MaterialGetSampler", "Material bind location is not a sampler",
                 SHRED);
    }

    RETURN->v_object = ulib_texture_ckobj_from_sampler(
      material->uniforms[location].as.sampler, false, SHRED);
}

CK_DLL_MFUN(material_set_texture)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);
    Chuck_Object* ckobj   = GET_NEXT_OBJECT(ARGS);
    SG_Texture* tex       = SG_GetTexture(OBJ_MEMBER_UINT(ckobj, component_offset_id));

    SG_Material::setTexture(material, location, tex);

    CQ_PushCommand_MaterialSetUniform(material, location);
}

CK_DLL_MFUN(material_get_texture)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);

    if (material->uniforms[location].type != SG_MATERIAL_UNIFORM_TEXTURE
        && material->uniforms[location].type != SG_MATERIAL_STORAGE_TEXTURE) {
        CK_THROW("MaterialGetTexture", "Material bind location is not a texture",
                 SHRED);
    }

    SG_Texture* tex  = SG_GetTexture(material->uniforms[location].as.texture_id);
    RETURN->v_object = tex ? tex->ckobj : NULL;
}

CK_DLL_MFUN(material_set_storage_texture)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT location      = GET_NEXT_INT(ARGS);
    Chuck_Object* ckobj   = GET_NEXT_OBJECT(ARGS);
    SG_Texture* tex       = SG_GetTexture(OBJ_MEMBER_UINT(ckobj, component_offset_id));

    SG_Material::setStorageTexture(material, location, tex);

    CQ_PushCommand_MaterialSetUniform(material, location);
}

// Lines2DMaterial ===================================================================

static void ulib_material_init_uniforms_and_pso(SG_Material* material)
{
    switch (material->material_type) {
        case SG_MATERIAL_CUSTOM: {
            // do nothing
        } break;
        case SG_MATERIAL_LINES2D: {
            SG_Material::uniformFloat(material, 0, 0.1f); // thickness
            CQ_PushCommand_MaterialSetUniform(material, 0);

            SG_Material::uniformVec3f(material, 1, glm::vec3(1.0f)); // color
            CQ_PushCommand_MaterialSetUniform(material, 1);

            // SG_Material::uniformInt(material, 2, 0); // loop
            // CQ_PushCommand_MaterialSetUniform(material, 2);

            // SG_Material::uniformFloat(material, 3, 0.5f); // extrusion
            // CQ_PushCommand_MaterialSetUniform(material, 3);

            // shader
            SG_Shader* lines2d_shader
              = SG_GetShader(g_material_builtin_shaders.lines2d_shader_id);
            ASSERT(lines2d_shader);

            ulib_material_set_shader(material, lines2d_shader);

            // set pso
            material->pso.primitive_topology = WGPUPrimitiveTopology_TriangleStrip;
            CQ_PushCommand_MaterialUpdatePSO(material);
        } break;
        case SG_MATERIAL_FLAT: {
            SG_Shader* flat_shader
              = SG_GetShader(g_material_builtin_shaders.flat_shader_id);
            ulib_material_set_shader(material, flat_shader);

            // set uniform
            SG_Material::uniformVec4f(material, 0, glm::vec4(1.0f));
            SG_Material::setSampler(material, 1, SG_SAMPLER_DEFAULT);
            SG_Material::setTexture(
              material, 2,
              SG_GetTexture(g_builtin_textures.white_pixel_id));     // color map
            SG_Material::uniformVec2f(material, 3, glm::vec4(0.0f)); // texture offset
            SG_Material::uniformVec2f(material, 4, glm::vec4(1.0f)); // texture scale

            ulib_material_cq_update_all_uniforms(material);
        } break;
        case SG_MATERIAL_UV: {
            // init shader
            SG_Shader* shader = SG_GetShader(g_material_builtin_shaders.uv_shader_id);
            ASSERT(shader);

            ulib_material_set_shader(material, shader);
        } break;
        case SG_MATERIAL_NORMAL: {
            // init shader
            SG_Shader* shader
              = SG_GetShader(g_material_builtin_shaders.normal_shader_id);
            ASSERT(shader);

            ulib_material_set_shader(material, shader);

            SG_Material::uniformInt(material, 0, 1); // use_worldspace_normals
            CQ_PushCommand_MaterialSetUniform(material, 0);
        } break;
        case SG_MATERIAL_WIREFRAME: {
            // init shader
            SG_Shader* shader
              = SG_GetShader(g_material_builtin_shaders.wireframe_shader_id);
            ASSERT(shader);

            ulib_material_set_shader(material, shader);

            SG_Material::uniformFloat(material, 0, 2.0); // default thickness
            CQ_PushCommand_MaterialSetUniform(material, 0);

            SG_Material::uniformFloat(material, 1, .5); // default alpha cutoff
            CQ_PushCommand_MaterialSetUniform(material, 1);

            SG_Material::uniformVec3f(material, 2, glm::vec3(1.0)); // default color
            CQ_PushCommand_MaterialSetUniform(material, 2);
        } break;
        case SG_MATERIAL_PHONG: {
            SG_Shader* shader
              = SG_GetShader(g_material_builtin_shaders.phong_shader_id);
            ulib_material_set_shader(material, shader);

            // init uniforms
            {
                PhongParams::specular(material, glm::vec3(.2f));
                PhongParams::diffuse(material, glm::vec3(1.0f));
                PhongParams::shininess(material, 64.0f);
                PhongParams::emission(material, glm::vec3(1.0f));
                PhongParams::normalFactor(material, 1.0f);
                PhongParams::aoFactor(material, 1.0f);

                // textures
                PhongParams::sampler(material, SG_SAMPLER_DEFAULT);
                PhongParams::albedoTex(
                  material, SG_GetTexture(g_builtin_textures.white_pixel_id));
                PhongParams::specularTex(
                  material, SG_GetTexture(g_builtin_textures.white_pixel_id));
                PhongParams::aoTex(material,
                                   SG_GetTexture(g_builtin_textures.white_pixel_id));
                PhongParams::emissiveTex(
                  material, SG_GetTexture(g_builtin_textures.black_pixel_id));
                PhongParams::normalTex(
                  material, SG_GetTexture(g_builtin_textures.normal_pixel_id));

                // envmap
                PhongParams::envmapMethod(material, SG_ENVMAP_SAMPLE_REFLECT);
                PhongParams::envmapBlendMode(material, SG_ENVMAP_BLEND_NONE);
                PhongParams::envmapRefractionRatio(material, 0.5f);
                PhongParams::envmapIntensity(material, 1.0f);
            }
        } break;
        case SG_MATERIAL_PBR: {
            // init shader
            SG_Shader* shader = SG_GetShader(g_material_builtin_shaders.pbr_shader_id);
            ASSERT(shader);

            ulib_material_set_shader(material, shader);

            // init uniforms
            {
                SG_Material::setSampler(material, 0,
                                        SG_SAMPLER_DEFAULT); // texture sampler
                SG_Material::setTexture(
                  material, 1,
                  SG_GetTexture(g_builtin_textures.white_pixel_id)); // albedo
                SG_Material::setTexture(
                  material, 2,
                  SG_GetTexture(g_builtin_textures.normal_pixel_id)); // normal
                SG_Material::setTexture(
                  material, 3,
                  SG_GetTexture(g_builtin_textures.white_pixel_id)); // ao
                SG_Material::setTexture(
                  material, 4,
                  SG_GetTexture(g_builtin_textures.white_pixel_id)); // mr
                SG_Material::setTexture(
                  material, 5,
                  SG_GetTexture(g_builtin_textures.black_pixel_id));     // emissive
                SG_Material::uniformVec4f(material, 6, glm::vec4(1.0f)); // albedo
                SG_Material::uniformVec3f(material, 7, glm::vec3(0.0f)); // emissive
                SG_Material::uniformFloat(material, 8, 0.0f);            // metallic
                SG_Material::uniformFloat(material, 9, 0.5f);            // roughness
                SG_Material::uniformFloat(material, 10, 1.0f); // normal factor
                SG_Material::uniformFloat(material, 11, 1.0f); // ao factor

                ulib_material_cq_update_all_uniforms(material);
            }
        } break;
        case SG_MATERIAL_SKYBOX: {
            // init shader
            SG_Shader* shader
              = SG_GetShader(g_material_builtin_shaders.skybox_shader_id);
            ASSERT(shader);

            ulib_material_set_shader(material, shader);

            // init uniforms
            SG_Material::setSampler(material, 0,
                                    SG_SAMPLER_DEFAULT); // envmap sampler
            ulib_material_cq_update_all_uniforms(material);
        } break;
        default: ASSERT(false);
    }
}

SG_Material* ulib_material_create(SG_MaterialType type, Chuck_VM_Shred* shred)
{
    CK_DL_API API = g_chuglAPI;

    Chuck_Object* ckobj = NULL;
    if (shred)
        ckobj = chugin_createCkObj(SG_MaterialTypeNames[type], false, shred);
    else
        ckobj = chugin_createCkObj(SG_MaterialTypeNames[type], false);

    SG_Material* material = SG_CreateMaterial(ckobj, type);
    ASSERT(material->type == SG_COMPONENT_MATERIAL);
    OBJ_MEMBER_UINT(ckobj, component_offset_id) = material->id;

    CQ_PushCommand_MaterialCreate(material);

    ulib_material_init_uniforms_and_pso(material);

    return material;
}

CK_DLL_CTOR(lines2d_material_ctor)
{
    SG_Material* material   = GET_MATERIAL(SELF);
    material->material_type = SG_MATERIAL_LINES2D;

    ulib_material_init_uniforms_and_pso(material);
}

CK_DLL_MFUN(lines2d_material_get_thickness)
{
    RETURN->v_float = GET_MATERIAL(SELF)->uniforms[0].as.f;
}

CK_DLL_MFUN(lines2d_material_set_thickness)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKFLOAT thickness   = GET_NEXT_FLOAT(ARGS);

    SG_Material::uniformFloat(material, 0, (f32)thickness);
    CQ_PushCommand_MaterialSetUniform(material, 0);
}

CK_DLL_MFUN(lines2d_material_get_color)
{
    glm::vec3 color = GET_MATERIAL(SELF)->uniforms[1].as.vec3f;
    RETURN->v_vec3  = { color.r, color.g, color.b };
}

CK_DLL_MFUN(lines2d_material_set_color)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKVEC3 color        = GET_NEXT_VEC3(ARGS);

    SG_Material::uniformVec3f(material, 1, glm::vec3(color.x, color.y, color.z));
    CQ_PushCommand_MaterialSetUniform(material, 1);
}

CK_DLL_MFUN(lines2d_material_get_extrusion)
{
    RETURN->v_float = GET_MATERIAL(SELF)->uniforms[3].as.f;
}

CK_DLL_MFUN(lines2d_material_set_extrusion)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKFLOAT extrusion   = GET_NEXT_FLOAT(ARGS);

    SG_Material::uniformFloat(material, 3, (f32)extrusion);
    CQ_PushCommand_MaterialSetUniform(material, 3);
}

CK_DLL_MFUN(lines2d_material_get_loop)
{
    RETURN->v_int = GET_MATERIAL(SELF)->uniforms[2].as.i;
}

CK_DLL_MFUN(lines2d_material_set_loop)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKINT loop          = GET_NEXT_INT(ARGS);

    SG_Material::uniformInt(material, 2, loop ? 1 : 0);
    CQ_PushCommand_MaterialSetUniform(material, 2);
}

// FlatMaterial ===================================================================

CK_DLL_CTOR(flat_material_ctor)
{
    SG_Material* material   = GET_MATERIAL(SELF);
    material->material_type = SG_MATERIAL_FLAT;
    ulib_material_init_uniforms_and_pso(material);
}

CK_DLL_MFUN(flat_material_get_color)
{
    glm::vec4 color = GET_MATERIAL(SELF)->uniforms[0].as.vec4f;
    RETURN->v_vec3  = { color.r, color.g, color.b };
}

CK_DLL_MFUN(flat_material_get_alpha)
{
    glm::vec4 color = GET_MATERIAL(SELF)->uniforms[0].as.vec4f;
    RETURN->v_float = color.a;
}

CK_DLL_MFUN(flat_material_set_alpha)
{
    SG_Material* material            = GET_MATERIAL(SELF);
    material->uniforms[0].as.vec4f.a = GET_NEXT_FLOAT(ARGS);
    CQ_PushCommand_MaterialSetUniform(material, 0);
}

CK_DLL_MFUN(flat_material_set_color)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKVEC3 color        = GET_NEXT_VEC3(ARGS);

    SG_Material::uniformVec4f(material, 0, glm::vec4(color.x, color.y, color.z, 1));
    CQ_PushCommand_MaterialSetUniform(material, 0);
}

CK_DLL_MFUN(flat_material_set_color_rgba)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKVEC4 color        = GET_NEXT_VEC4(ARGS);

    SG_Material::uniformVec4f(material, 0,
                              glm::vec4(color.x, color.y, color.z, color.w));
    CQ_PushCommand_MaterialSetUniform(material, 0);
}

CK_DLL_MFUN(flat_material_get_sampler)
{
    RETURN->v_object = ulib_texture_ckobj_from_sampler(
      GET_MATERIAL(SELF)->uniforms[1].as.sampler, false, SHRED);
}

CK_DLL_MFUN(flat_material_set_sampler)
{
    SG_Material* material = GET_MATERIAL(SELF);
    Chuck_Object* ckobj   = GET_NEXT_OBJECT(ARGS);

    SG_Material::setSampler(material, 1, SG_Sampler::fromCkObj(ckobj));
    CQ_PushCommand_MaterialSetUniform(material, 1);
}

CK_DLL_MFUN(flat_material_get_color_map)
{
    RETURN->v_object
      = SG_GetTexture(GET_MATERIAL(SELF)->uniforms[2].as.texture_id)->ckobj;
}

CK_DLL_MFUN(flat_material_set_color_map)
{
    SG_Material* material = GET_MATERIAL(SELF);
    Chuck_Object* ckobj   = GET_NEXT_OBJECT(ARGS);
    SG_Texture* tex       = ckobj ?
                              SG_GetTexture(OBJ_MEMBER_UINT(ckobj, component_offset_id)) :
                              SG_GetTexture(g_builtin_textures.white_pixel_id);

    SG_Material::setTexture(material, 2, tex);
    CQ_PushCommand_MaterialSetUniform(material, 2);
}

CK_DLL_MFUN(flat_material_set_texture_offset)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKVEC2 offset       = GET_NEXT_VEC2(ARGS);
    SG_Material::uniformVec2f(material, 3, glm::vec2(offset.x, offset.y));
    CQ_PushCommand_MaterialSetUniform(material, 3);
}

CK_DLL_MFUN(flat_material_set_texture_scale)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKVEC2 scale        = GET_NEXT_VEC2(ARGS);
    SG_Material::uniformVec2f(material, 4, glm::vec2(scale.x, scale.y));
    CQ_PushCommand_MaterialSetUniform(material, 4);
}

CK_DLL_MFUN(flat_material_get_texture_offset)
{
    SG_Material* material = GET_MATERIAL(SELF);
    RETURN->v_vec2
      = { material->uniforms[3].as.vec2f.x, material->uniforms[3].as.vec2f.y };
}

CK_DLL_MFUN(flat_material_get_texture_scale)
{
    SG_Material* material = GET_MATERIAL(SELF);
    RETURN->v_vec2
      = { material->uniforms[4].as.vec2f.x, material->uniforms[4].as.vec2f.y };
}

// UVMaterial ===================================================================

CK_DLL_CTOR(uv_material_ctor)
{
    SG_Material* material   = GET_MATERIAL(SELF);
    material->material_type = SG_MATERIAL_UV;

    ulib_material_init_uniforms_and_pso(material);
}

// NormalMaterial ===================================================================

CK_DLL_CTOR(normal_material_ctor)
{
    SG_Material* material   = GET_MATERIAL(SELF);
    material->material_type = SG_MATERIAL_NORMAL;

    ulib_material_init_uniforms_and_pso(material);
}

CK_DLL_MFUN(normal_material_set_worldspace_normals)
{
    SG_Material* material          = GET_MATERIAL(SELF);
    t_CKINT use_worldspace_normals = GET_NEXT_INT(ARGS);

    SG_Material::uniformInt(material, 0, use_worldspace_normals ? 1 : 0);
    CQ_PushCommand_MaterialSetUniform(material, 0);
}

CK_DLL_MFUN(normal_material_get_worldspace_normals)
{
    RETURN->v_int = GET_MATERIAL(SELF)->uniforms[0].as.i;
}

// WireframeMaterial ===================================================================

CK_DLL_CTOR(wireframe_material_ctor)
{
    SG_Material* material   = GET_MATERIAL(SELF);
    material->material_type = SG_MATERIAL_WIREFRAME;

    ulib_material_init_uniforms_and_pso(material);
}

CK_DLL_MFUN(wireframe_material_set_thickness)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKFLOAT thickness   = GET_NEXT_FLOAT(ARGS);

    SG_Material::uniformFloat(material, 0, (f32)thickness);
    CQ_PushCommand_MaterialSetUniform(material, 0);
}

CK_DLL_MFUN(wireframe_material_get_thickness)
{
    RETURN->v_float = GET_MATERIAL(SELF)->uniforms[0].as.f;
}

CK_DLL_MFUN(wireframe_material_set_alpha_cutoff)
{
    SG_Material* material  = GET_MATERIAL(SELF);
    t_CKFLOAT alpha_cutoff = GET_NEXT_FLOAT(ARGS);

    SG_Material::uniformFloat(material, 1, (f32)alpha_cutoff);
    CQ_PushCommand_MaterialSetUniform(material, 1);
}

CK_DLL_MFUN(wireframe_material_get_alpha_cutoff)
{
    RETURN->v_float = GET_MATERIAL(SELF)->uniforms[1].as.f;
}

CK_DLL_MFUN(wireframe_material_set_color)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKVEC3 color        = GET_NEXT_VEC3(ARGS);

    SG_Material::uniformVec3f(material, 2, glm::vec3(color.x, color.y, color.z));
    CQ_PushCommand_MaterialSetUniform(material, 2);
}

CK_DLL_MFUN(wireframe_material_get_color)
{
    glm::vec3 color = GET_MATERIAL(SELF)->uniforms[2].as.vec3f;
    RETURN->v_vec3  = { color.r, color.g, color.b };
}

// PhongMaterial ===================================================================

CK_DLL_CTOR(phong_material_ctor)
{
    SG_Material* material   = GET_MATERIAL(SELF);
    material->material_type = SG_MATERIAL_PHONG;

    ulib_material_init_uniforms_and_pso(material);
}

CK_DLL_MFUN(phong_material_get_specular_color)
{
    glm::vec3 color = *PhongParams::specular(GET_MATERIAL(SELF));
    RETURN->v_vec3  = { color.r, color.g, color.b };
}

CK_DLL_MFUN(phong_material_set_specular_color)
{
    t_CKVEC3 color = GET_NEXT_VEC3(ARGS);
    PhongParams::specular(GET_MATERIAL(SELF), glm::vec3(color.x, color.y, color.z));
}

CK_DLL_MFUN(phong_material_get_diffuse_color)
{
    glm::vec4 color = *PhongParams::diffuse(GET_MATERIAL(SELF));
    RETURN->v_vec3  = { color.r, color.g, color.b };
}

CK_DLL_MFUN(phong_material_set_diffuse_color)
{
    t_CKVEC3 color = GET_NEXT_VEC3(ARGS);
    PhongParams::diffuse(GET_MATERIAL(SELF), glm::vec3(color.x, color.y, color.z));
}

CK_DLL_MFUN(phong_material_get_alpha)
{
    RETURN->v_float = PhongParams::diffuse(GET_MATERIAL(SELF))->a;
}

CK_DLL_MFUN(phong_material_set_alpha)
{
    PhongParams::alpha(GET_MATERIAL(SELF), GET_NEXT_FLOAT(ARGS));
}

CK_DLL_MFUN(phong_material_get_log_shininess)
{
    RETURN->v_float = glm::log2(*PhongParams::shininess(GET_MATERIAL(SELF)));
}

CK_DLL_MFUN(phong_material_set_log_shininess)
{
    t_CKFLOAT shininess = GET_NEXT_FLOAT(ARGS);
    PhongParams::shininess(GET_MATERIAL(SELF), glm::pow(2.0f, (f32)shininess));
}

CK_DLL_MFUN(phong_material_get_emission_color)
{
    glm::vec3 color = *PhongParams::emission(GET_MATERIAL(SELF));
    RETURN->v_vec3  = { color.r, color.g, color.b };
}

CK_DLL_MFUN(phong_material_set_emission_color)
{
    t_CKVEC3 color = GET_NEXT_VEC3(ARGS);
    PhongParams::emission(GET_MATERIAL(SELF), glm::vec3(color.x, color.y, color.z));
}

CK_DLL_MFUN(phong_material_get_normal_factor)
{
    RETURN->v_float = *PhongParams::normalFactor(GET_MATERIAL(SELF));
}

CK_DLL_MFUN(phong_material_set_normal_factor)
{
    PhongParams::normalFactor(GET_MATERIAL(SELF), GET_NEXT_FLOAT(ARGS));
}

CK_DLL_MFUN(phong_material_get_ao_factor)
{
    RETURN->v_float = *PhongParams::aoFactor(GET_MATERIAL(SELF));
}

CK_DLL_MFUN(phong_material_set_ao_factor)
{
    PhongParams::aoFactor(GET_MATERIAL(SELF), GET_NEXT_FLOAT(ARGS));
}

CK_DLL_MFUN(phong_material_get_albedo_tex)
{
    SG_Texture* tex  = PhongParams::albedoTex(GET_MATERIAL(SELF));
    RETURN->v_object = tex ? tex->ckobj : NULL;
}

CK_DLL_MFUN(phong_material_set_albedo_tex)
{
    Chuck_Object* ckobj = GET_NEXT_OBJECT(ARGS);
    SG_Texture* tex
      = ckobj ? SG_GetTexture(OBJ_MEMBER_UINT(ckobj, component_offset_id)) : NULL;
    PhongParams::albedoTex(GET_MATERIAL(SELF), tex);
}

CK_DLL_MFUN(phong_material_get_specular_tex)
{
    SG_Texture* tex  = PhongParams::specularTex(GET_MATERIAL(SELF));
    RETURN->v_object = tex ? tex->ckobj : NULL;
}

CK_DLL_MFUN(phong_material_set_specular_tex)
{
    Chuck_Object* ckobj = GET_NEXT_OBJECT(ARGS);
    SG_Texture* tex
      = ckobj ? SG_GetTexture(OBJ_MEMBER_UINT(ckobj, component_offset_id)) : NULL;
    PhongParams::specularTex(GET_MATERIAL(SELF), tex);
}

CK_DLL_MFUN(phong_material_get_ao_tex)
{
    SG_Texture* tex  = PhongParams::aoTex(GET_MATERIAL(SELF));
    RETURN->v_object = tex ? tex->ckobj : NULL;
}

CK_DLL_MFUN(phong_material_set_ao_tex)
{
    Chuck_Object* ckobj = GET_NEXT_OBJECT(ARGS);
    SG_Texture* tex
      = ckobj ? SG_GetTexture(OBJ_MEMBER_UINT(ckobj, component_offset_id)) : NULL;
    PhongParams::aoTex(GET_MATERIAL(SELF), tex);
}

CK_DLL_MFUN(phong_material_get_emissive_tex)
{
    SG_Texture* tex  = PhongParams::emissiveTex(GET_MATERIAL(SELF));
    RETURN->v_object = tex ? tex->ckobj : NULL;
}

CK_DLL_MFUN(phong_material_set_emissive_tex)
{
    Chuck_Object* ckobj = GET_NEXT_OBJECT(ARGS);
    SG_Texture* tex
      = ckobj ? SG_GetTexture(OBJ_MEMBER_UINT(ckobj, component_offset_id)) : NULL;
    PhongParams::emissiveTex(GET_MATERIAL(SELF), tex);
}

CK_DLL_MFUN(phong_material_get_normal_tex)
{
    SG_Texture* tex  = PhongParams::normalTex(GET_MATERIAL(SELF));
    RETURN->v_object = tex ? tex->ckobj : NULL;
}

CK_DLL_MFUN(phong_material_set_normal_tex)
{
    Chuck_Object* ckobj = GET_NEXT_OBJECT(ARGS);
    SG_Texture* tex
      = ckobj ? SG_GetTexture(OBJ_MEMBER_UINT(ckobj, component_offset_id)) : NULL;
    PhongParams::normalTex(GET_MATERIAL(SELF), tex);
}

CK_DLL_MFUN(phong_material_get_envmap_method)
{
    RETURN->v_int = *PhongParams::envmapMethod(GET_MATERIAL(SELF));
}

CK_DLL_MFUN(phong_material_set_envmap_method)
{
    PhongParams::envmapMethod(GET_MATERIAL(SELF),
                              (SG_EnvmapSampleMode)GET_NEXT_INT(ARGS));
}

CK_DLL_MFUN(phong_material_get_envmap_refraction_ratio)
{
    RETURN->v_float = *PhongParams::envmapRefractionRatio(GET_MATERIAL(SELF));
}

CK_DLL_MFUN(phong_material_set_envmap_refraction_ratio)
{
    PhongParams::envmapRefractionRatio(GET_MATERIAL(SELF), GET_NEXT_FLOAT(ARGS));
}

CK_DLL_MFUN(phong_material_get_envmap_blend_mode)
{
    RETURN->v_int = *PhongParams::envmapBlendMode(GET_MATERIAL(SELF));
}

CK_DLL_MFUN(phong_material_set_envmap_blend_mode)
{
    PhongParams::envmapBlendMode(GET_MATERIAL(SELF),
                                 (SG_EnvmapBlendMode)GET_NEXT_INT(ARGS));
}

CK_DLL_MFUN(phong_material_get_envmap_intensity)
{
    RETURN->v_float = *PhongParams::envmapIntensity(GET_MATERIAL(SELF));
}

CK_DLL_MFUN(phong_material_set_envmap_intensity)
{
    PhongParams::envmapIntensity(GET_MATERIAL(SELF), GET_NEXT_FLOAT(ARGS));
}

// PBRMaterial ===================================================================

CK_DLL_CTOR(pbr_material_ctor)
{
    SG_Material* material   = GET_MATERIAL(SELF);
    material->material_type = SG_MATERIAL_PBR;

    ulib_material_init_uniforms_and_pso(material);
}

CK_DLL_MFUN(pbr_material_get_albedo)
{
    SG_Material* material = GET_MATERIAL(SELF);
    glm::vec4 color       = material->uniforms[6].as.vec4f;

    RETURN->v_vec3 = { color.r, color.g, color.b };
}

CK_DLL_MFUN(pbr_material_set_albedo)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKVEC3 color        = GET_NEXT_VEC3(ARGS);

    SG_Material::uniformVec4f(material, 6, glm::vec4(color.x, color.y, color.z, 1));
    CQ_PushCommand_MaterialSetUniform(material, 6);
}

CK_DLL_MFUN(pbr_material_get_emissive)
{
    SG_Material* material = GET_MATERIAL(SELF);
    glm::vec3 color       = material->uniforms[7].as.vec3f;

    RETURN->v_vec3 = { color.r, color.g, color.b };
}

CK_DLL_MFUN(pbr_material_set_emissive)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKVEC3 color        = GET_NEXT_VEC3(ARGS);

    SG_Material::uniformVec3f(material, 7, glm::vec3(color.x, color.y, color.z));
    CQ_PushCommand_MaterialSetUniform(material, 7);
}

CK_DLL_MFUN(pbr_material_get_metallic)
{
    SG_Material* material = GET_MATERIAL(SELF);
    RETURN->v_float       = material->uniforms[8].as.f;
}

CK_DLL_MFUN(pbr_material_set_metallic)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKFLOAT metallic    = GET_NEXT_FLOAT(ARGS);

    SG_Material::uniformFloat(material, 8, (f32)metallic);
    CQ_PushCommand_MaterialSetUniform(material, 8);
}

CK_DLL_MFUN(pbr_material_get_roughness)
{
    SG_Material* material = GET_MATERIAL(SELF);
    RETURN->v_float       = material->uniforms[9].as.f;
}

CK_DLL_MFUN(pbr_material_set_roughness)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKFLOAT roughness   = GET_NEXT_FLOAT(ARGS);

    SG_Material::uniformFloat(material, 9, (f32)roughness);
    CQ_PushCommand_MaterialSetUniform(material, 9);
}

CK_DLL_MFUN(pbr_material_get_normal_factor)
{
    SG_Material* material = GET_MATERIAL(SELF);
    RETURN->v_float       = material->uniforms[10].as.f;
}

CK_DLL_MFUN(pbr_material_set_normal_factor)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKFLOAT normal      = GET_NEXT_FLOAT(ARGS);

    SG_Material::uniformFloat(material, 10, (f32)normal);
    CQ_PushCommand_MaterialSetUniform(material, 10);
}

CK_DLL_MFUN(pbr_material_get_ao_factor)
{
    SG_Material* material = GET_MATERIAL(SELF);
    RETURN->v_float       = material->uniforms[11].as.f;
}

CK_DLL_MFUN(pbr_material_set_ao_factor)
{
    SG_Material* material = GET_MATERIAL(SELF);
    t_CKFLOAT ao          = GET_NEXT_FLOAT(ARGS);

    SG_Material::uniformFloat(material, 11, (f32)ao);
    CQ_PushCommand_MaterialSetUniform(material, 11);
}

CK_DLL_MFUN(pbr_material_get_albedo_tex)
{
    SG_Material* material = GET_MATERIAL(SELF);
    SG_Texture* tex       = SG_GetTexture(material->uniforms[1].as.texture_id);
    RETURN->v_object      = tex ? tex->ckobj : NULL;
}

CK_DLL_MFUN(pbr_material_set_albedo_tex)
{
    SG_Material* material = GET_MATERIAL(SELF);
    Chuck_Object* ckobj   = GET_NEXT_OBJECT(ARGS);
    SG_Texture* tex       = ckobj ?
                              SG_GetTexture(OBJ_MEMBER_INT(ckobj, component_offset_id)) :
                              SG_GetTexture(g_builtin_textures.white_pixel_id);

    SG_Material::setTexture(material, 1, tex);
    CQ_PushCommand_MaterialSetUniform(material, 1);
}

CK_DLL_MFUN(pbr_material_get_normal_tex)
{
    SG_Material* material = GET_MATERIAL(SELF);
    SG_Texture* tex       = SG_GetTexture(material->uniforms[2].as.texture_id);
    RETURN->v_object      = tex ? tex->ckobj : NULL;
}

CK_DLL_MFUN(pbr_material_set_normal_tex)
{
    SG_Material* material = GET_MATERIAL(SELF);
    Chuck_Object* ckobj   = GET_NEXT_OBJECT(ARGS);
    SG_Texture* tex       = ckobj ?
                              SG_GetTexture(OBJ_MEMBER_INT(ckobj, component_offset_id)) :
                              SG_GetTexture(g_builtin_textures.normal_pixel_id);

    SG_Material::setTexture(material, 2, tex);
    CQ_PushCommand_MaterialSetUniform(material, 2);
}

CK_DLL_MFUN(pbr_material_get_ao_tex)
{
    SG_Material* material = GET_MATERIAL(SELF);
    SG_Texture* tex       = SG_GetTexture(material->uniforms[3].as.texture_id);
    RETURN->v_object      = tex ? tex->ckobj : NULL;
}

CK_DLL_MFUN(pbr_material_set_ao_tex)
{
    SG_Material* material = GET_MATERIAL(SELF);
    Chuck_Object* ckobj   = GET_NEXT_OBJECT(ARGS);
    SG_Texture* tex       = ckobj ?
                              SG_GetTexture(OBJ_MEMBER_INT(ckobj, component_offset_id)) :
                              SG_GetTexture(g_builtin_textures.white_pixel_id);

    SG_Material::setTexture(material, 3, tex);
    CQ_PushCommand_MaterialSetUniform(material, 3);
}

CK_DLL_MFUN(pbr_material_get_mr_tex)
{
    SG_Material* material = GET_MATERIAL(SELF);
    SG_Texture* tex       = SG_GetTexture(material->uniforms[4].as.texture_id);
    RETURN->v_object      = tex ? tex->ckobj : NULL;
}

CK_DLL_MFUN(pbr_material_set_mr_tex)
{
    SG_Material* material = GET_MATERIAL(SELF);
    Chuck_Object* ckobj   = GET_NEXT_OBJECT(ARGS);
    SG_Texture* tex       = ckobj ?
                              SG_GetTexture(OBJ_MEMBER_INT(ckobj, component_offset_id)) :
                              SG_GetTexture(g_builtin_textures.white_pixel_id);

    SG_Material::setTexture(material, 4, tex);
    CQ_PushCommand_MaterialSetUniform(material, 4);
}

CK_DLL_MFUN(pbr_material_get_emissive_tex)
{
    SG_Material* material = GET_MATERIAL(SELF);
    SG_Texture* tex       = SG_GetTexture(material->uniforms[5].as.texture_id);
    RETURN->v_object      = tex ? tex->ckobj : NULL;
}

CK_DLL_MFUN(pbr_material_set_emissive_tex)
{
    SG_Material* material = GET_MATERIAL(SELF);
    Chuck_Object* ckobj   = GET_NEXT_OBJECT(ARGS);
    SG_Texture* tex       = ckobj ?
                              SG_GetTexture(OBJ_MEMBER_INT(ckobj, component_offset_id)) :
                              SG_GetTexture(g_builtin_textures.black_pixel_id);

    SG_Material::setTexture(material, 5, tex);
    CQ_PushCommand_MaterialSetUniform(material, 5);
}

// SkyboxMaterial ===================================================================

CK_DLL_CTOR(skybox_material_ctor)
{
    SG_Material* material   = GET_MATERIAL(SELF);
    material->material_type = SG_MATERIAL_SKYBOX;

    ulib_material_init_uniforms_and_pso(material);
}

// init default materials ========================================================

struct CHUGL_ShaderDesc {
    const char* vertex_string     = "";
    const char* fragment_string   = "";
    const char* vertex_filepath   = "";
    const char* fragment_filepath = "";
    WGPUVertexFormat* vertex_layout;
    int vertex_layout_count;
    const char* compute_string   = "";
    const char* compute_filepath = "";
    SG_ShaderIncludes includes;
};

// used for creating internal shaders, so add_refcount is true
static SG_ID chugl_createShader(CHUGL_ShaderDesc* shader_desc, const char* name)
{
    CK_DL_API API = g_chuglAPI;
    Chuck_Object* shader_ckobj
      = chugin_createCkObj(SG_CKNames[SG_COMPONENT_SHADER], true);

    // create shader on audio side
    SG_Shader* shader
      = SG_CreateShader(shader_ckobj, shader_desc->vertex_string,
                        shader_desc->fragment_string, shader_desc->vertex_filepath,
                        shader_desc->fragment_filepath, shader_desc->vertex_layout,
                        shader_desc->vertex_layout_count, shader_desc->compute_string,
                        shader_desc->compute_filepath, shader_desc->includes);

    // save component id
    OBJ_MEMBER_UINT(shader_ckobj, component_offset_id) = shader->id;

    // push to command queue
    CQ_PushCommand_ShaderCreate(shader);

    ulib_component_set_name(shader, name);

    return shader->id;
}

void chugl_initDefaultMaterials()
{
    static WGPUVertexFormat standard_vertex_layout[] = {
        WGPUVertexFormat_Float32x3, // position
        WGPUVertexFormat_Float32x3, // normal
        WGPUVertexFormat_Float32x2, // uv
    };

    static WGPUVertexFormat gtext_vertex_layout[] = {
        WGPUVertexFormat_Float32x2, // position
        WGPUVertexFormat_Float32x2, // uv
        WGPUVertexFormat_Sint32,    // glyph_index
    };

    {
        CHUGL_ShaderDesc lines_2d_shader_desc = {};
        lines_2d_shader_desc.vertex_string    = lines2d_shader_string;
        lines_2d_shader_desc.fragment_string  = lines2d_shader_string;
        g_material_builtin_shaders.lines2d_shader_id
          = chugl_createShader(&lines_2d_shader_desc, "Lines2D");
    }

    {
        CHUGL_ShaderDesc flat_shader_desc    = {};
        flat_shader_desc.vertex_string       = flat_shader_string;
        flat_shader_desc.fragment_string     = flat_shader_string;
        flat_shader_desc.vertex_layout       = standard_vertex_layout;
        flat_shader_desc.vertex_layout_count = ARRAY_LENGTH(standard_vertex_layout);
        g_material_builtin_shaders.flat_shader_id
          = chugl_createShader(&flat_shader_desc, "FlatMaterial");
    }

    {

        CHUGL_ShaderDesc gtext_shader_desc    = {};
        gtext_shader_desc.vertex_string       = gtext_shader_string;
        gtext_shader_desc.fragment_string     = gtext_shader_string;
        gtext_shader_desc.vertex_layout       = gtext_vertex_layout;
        gtext_shader_desc.vertex_layout_count = ARRAY_LENGTH(gtext_vertex_layout);
        g_material_builtin_shaders.gtext_shader_id
          = chugl_createShader(&gtext_shader_desc, "GText");
    }

    {
        CHUGL_ShaderDesc output_pass_shader_desc = {};
        output_pass_shader_desc.vertex_string    = output_pass_shader_string;
        output_pass_shader_desc.fragment_string  = output_pass_shader_string;
        g_material_builtin_shaders.output_pass_shader_id
          = chugl_createShader(&output_pass_shader_desc, "Tonemap");
    }

    {
        CHUGL_ShaderDesc bloom_downsample_screen_shader_desc = {};
        bloom_downsample_screen_shader_desc.vertex_string
          = bloom_downsample_screen_shader;
        bloom_downsample_screen_shader_desc.fragment_string
          = bloom_downsample_screen_shader;

        g_material_builtin_shaders.bloom_downsample_screen_shader_id
          = chugl_createShader(&bloom_downsample_screen_shader_desc, "BloomDownsample");
    }

    {
        CHUGL_ShaderDesc bloom_upsample_screen_shader_desc = {};
        bloom_upsample_screen_shader_desc.vertex_string = bloom_upsample_screen_shader;
        bloom_upsample_screen_shader_desc.fragment_string
          = bloom_upsample_screen_shader;

        g_material_builtin_shaders.bloom_upsample_screen_shader_id
          = chugl_createShader(&bloom_upsample_screen_shader_desc, "BloomUpsample");
    }

    { // pbr material
        CHUGL_ShaderDesc pbr_shader_desc    = {};
        pbr_shader_desc.vertex_string       = pbr_shader_string;
        pbr_shader_desc.fragment_string     = pbr_shader_string;
        pbr_shader_desc.vertex_layout       = standard_vertex_layout;
        pbr_shader_desc.vertex_layout_count = ARRAY_LENGTH(standard_vertex_layout);
        pbr_shader_desc.includes.lit        = true;
        g_material_builtin_shaders.pbr_shader_id
          = chugl_createShader(&pbr_shader_desc, "PBR");
    }

    { // uv material
        CHUGL_ShaderDesc uv_shader_desc    = {};
        uv_shader_desc.vertex_string       = uv_shader_string;
        uv_shader_desc.fragment_string     = uv_shader_string;
        uv_shader_desc.vertex_layout       = standard_vertex_layout;
        uv_shader_desc.vertex_layout_count = ARRAY_LENGTH(standard_vertex_layout);
        g_material_builtin_shaders.uv_shader_id
          = chugl_createShader(&uv_shader_desc, "UV");
    }

    { // normal material
        CHUGL_ShaderDesc normal_shader_desc    = {};
        normal_shader_desc.vertex_string       = normal_shader_string;
        normal_shader_desc.fragment_string     = normal_shader_string;
        normal_shader_desc.vertex_layout       = standard_vertex_layout;
        normal_shader_desc.vertex_layout_count = ARRAY_LENGTH(standard_vertex_layout);
        g_material_builtin_shaders.normal_shader_id
          = chugl_createShader(&normal_shader_desc, "Normal");
    }

    { // wireframe material
        CHUGL_ShaderDesc wireframe_shader_desc = {};
        wireframe_shader_desc.vertex_string    = wireframe_shader_string;
        wireframe_shader_desc.fragment_string  = wireframe_shader_string;
        wireframe_shader_desc.vertex_layout    = standard_vertex_layout;
        wireframe_shader_desc.vertex_layout_count
          = ARRAY_LENGTH(standard_vertex_layout);
        g_material_builtin_shaders.wireframe_shader_id
          = chugl_createShader(&wireframe_shader_desc, "WireFrame");
    }

    { // phong material
        CHUGL_ShaderDesc phong_shader_desc      = {};
        phong_shader_desc.vertex_string         = phong_shader_string;
        phong_shader_desc.fragment_string       = phong_shader_string;
        phong_shader_desc.vertex_layout         = standard_vertex_layout;
        phong_shader_desc.vertex_layout_count   = ARRAY_LENGTH(standard_vertex_layout);
        phong_shader_desc.includes.lit          = true;
        phong_shader_desc.includes.uses_env_map = true;
        phong_shader_desc.includes.shadows      = true;
        g_material_builtin_shaders.phong_shader_id
          = chugl_createShader(&phong_shader_desc, "Phong");
    }

    { // points material
        CHUGL_ShaderDesc points_shader_desc = {};
        points_shader_desc.vertex_string    = points_shader_string;
        points_shader_desc.fragment_string  = points_shader_string;
        g_material_builtin_shaders.points_shader_id
          = chugl_createShader(&points_shader_desc, "GPoints");
    }

    { // skybox material
        CHUGL_ShaderDesc skybox_shader_desc      = {};
        skybox_shader_desc.vertex_string         = skybox_shader_string;
        skybox_shader_desc.fragment_string       = skybox_shader_string;
        skybox_shader_desc.includes.uses_env_map = true;
        g_material_builtin_shaders.skybox_shader_id
          = chugl_createShader(&skybox_shader_desc, "Skybox");
    }

    { // TODO maybe just keep this on graphics side
        CHUGL_ShaderDesc b2_debug_solid_polygon_shader_desc = {};
        b2_debug_solid_polygon_shader_desc.vertex_string
          = b2_solid_polygon_shader_string;
        b2_debug_solid_polygon_shader_desc.fragment_string
          = b2_solid_polygon_shader_string;
        g_material_builtin_shaders.b2_debug_solid_polygon_shader_id
          = chugl_createShader(&b2_debug_solid_polygon_shader_desc,
                               "b2DebugSolidPolygon");
    }
}
