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

#define GET_PASS(ckobj) SG_GetPass(OBJ_MEMBER_UINT(ckobj, component_offset_id))

// external API
SG_Pass* ulib_pass_create(SG_PassType pass_type, Chuck_Object* pass_ckobj, bool add_ref,
                          Chuck_VM_Shred* shred);

SG_Material* chugl_createInternalMaterial(SG_MaterialType material_type,
                                          SG_Shader* shader)
{
    // internal materials are not connected to chuck objects
    SG_Material* material = SG_CreateMaterial(NULL, material_type);
    SG_Material::shader(material, shader);

    CQ_PushCommand_MaterialCreate(material);

    return material;
}

// Pass
CK_DLL_CTOR(pass_ctor);
CK_DLL_MFUN(pass_get_next);
CK_DLL_GFUN(pass_op_gruck);   // add pass
CK_DLL_GFUN(pass_op_ungruck); // remove pass

// renderpass

CK_DLL_MFUN(renderpass_set_color_target);
CK_DLL_MFUN(renderpass_get_color_target);

CK_DLL_MFUN(renderpass_set_color_target_clear_on_load);
CK_DLL_MFUN(renderpass_get_color_target_clear_on_load);

CK_DLL_MFUN(renderpass_set_viewport_normalized);
CK_DLL_MFUN(renderpass_set_viewport_normalized_vec4);
CK_DLL_MFUN(renderpass_set_viewport);
CK_DLL_MFUN(renderpass_set_viewport_vec4);

CK_DLL_MFUN(renderpass_set_scissor_normalized);
CK_DLL_MFUN(renderpass_set_scissor_normalized_vec4);
CK_DLL_MFUN(renderpass_set_scissor);
CK_DLL_MFUN(renderpass_set_scissor_vec4);

// TODO add scissor and viewport

// ScenePass
CK_DLL_CTOR(scenepass_ctor_with_scene);
CK_DLL_MFUN(scenepass_set_scene_and_camera);
CK_DLL_MFUN(scenepass_get_scene);
CK_DLL_MFUN(scenepass_get_camera);

CK_DLL_MFUN(scenepass_get_msaa);
CK_DLL_MFUN(scenepass_set_msaa);

// TODO add set/get HDR?

// ScenePass
CK_DLL_CTOR(screenpass_ctor_with_shader);

CK_DLL_MFUN(screenpass_get_material);
CK_DLL_MFUN(screenpass_set_shader);

// TODO get_target

// OutputPass : ScreenPass
CK_DLL_CTOR(outputpass_ctor);
CK_DLL_MFUN(outputpass_set_input_texture);
CK_DLL_MFUN(outputpass_set_tonemap);
CK_DLL_MFUN(outputpass_get_tonemap);
CK_DLL_MFUN(outputpass_set_gamma);
CK_DLL_MFUN(outputpass_get_gamma);
CK_DLL_MFUN(outputpass_set_exposure);
CK_DLL_MFUN(outputpass_get_exposure);
CK_DLL_MFUN(outputpass_get_sampler);
CK_DLL_MFUN(outputpass_set_sampler);

// ComputePass
CK_DLL_CTOR(computepass_ctor_with_shader);
CK_DLL_MFUN(computepass_set_shader);
CK_DLL_MFUN(computepass_set_uniform_float);
CK_DLL_MFUN(computepass_set_uniform_float2);
CK_DLL_MFUN(computepass_set_uniform_float3);
CK_DLL_MFUN(computepass_set_uniform_float4);
CK_DLL_MFUN(computepass_set_texture);
CK_DLL_MFUN(computepass_set_storage_buffer);
CK_DLL_MFUN(computepass_set_storage_texture);
CK_DLL_MFUN(computepass_set_uniform_int);
CK_DLL_MFUN(computepass_set_workgroup);

// BloomPass
CK_DLL_MFUN(bloompass_set_input_render_texture);
CK_DLL_MFUN(bloompass_get_output_render_texture);
CK_DLL_MFUN(bloompass_set_internal_blend);
CK_DLL_MFUN(bloompass_set_final_blend);
CK_DLL_MFUN(bloompass_get_internal_blend);
CK_DLL_MFUN(bloompass_get_final_blend);
CK_DLL_MFUN(bloompass_set_num_levels);
CK_DLL_MFUN(bloompass_get_num_levels);
CK_DLL_MFUN(bloompass_set_threshold);
CK_DLL_MFUN(bloompass_get_threshold);

/*
==optimize==
currently hijacking the ChuGL material system to update screenpass shader params
- we implicitly create a material every time a shader is set on the ScreenPass
- this material generates an R_Pipeline which is NOT used for rendering,
and will be a dud in R_RenderScene when iterating over pipelines
- but you shouldn't have many ScreenPasses anyways, accepting the cost for now
*/

// Other
// WindowTexture

const char* ulib_pass_classname(SG_PassType pass_type)
{
    switch (pass_type) {
        case SG_PassType_Root: return SG_CKNames[SG_COMPONENT_PASS];
        case SG_PassType_Render: return "RenderPass";
        case SG_PassType_Scene: return "ScenePass";
        case SG_PassType_Screen: return "ScreenPass";
        case SG_PassType_Compute: return "ComputePass";
        case SG_PassType_Bloom: return "BloomPass";
        default: ASSERT(false);
    }
    return NULL;
}

void ulib_pass_query(Chuck_DL_Query* QUERY)
{
    BEGIN_CLASS(SG_CKNames[SG_COMPONENT_PASS], SG_CKNames[SG_COMPONENT_BASE]);
    DOC_CLASS("Base class for all passes, used to describe a render graph");

    CTOR(pass_ctor);

    MFUN(pass_get_next, SG_CKNames[SG_COMPONENT_PASS], "next");
    DOC_FUNC("Get the GPass this is connected to");

    QUERY->add_op_overload_binary(QUERY, pass_op_gruck, SG_CKNames[SG_COMPONENT_PASS],
                                  "-->", SG_CKNames[SG_COMPONENT_PASS], "lhs",
                                  SG_CKNames[SG_COMPONENT_PASS], "rhs");

    // overload GGen --< GGen
    QUERY->add_op_overload_binary(QUERY, pass_op_ungruck, SG_CKNames[SG_COMPONENT_PASS],
                                  "--<", SG_CKNames[SG_COMPONENT_PASS], "lhs",
                                  SG_CKNames[SG_COMPONENT_PASS], "rhs");
    END_CLASS();

    // RenderPass --------------------------------------------------------------
    {
        BEGIN_CLASS(ulib_pass_classname(SG_PassType_Render),
                    SG_CKNames[SG_COMPONENT_PASS]);
        DOC_CLASS(
          "Base class containing shared methods for ScenePass and ScreenPass. "
          "Don't instantiate this directly.");

        MFUN(renderpass_set_color_target, "void", "colorOutput");
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "color_texture");
        DOC_FUNC(
          "Set the target texture to render to. Setting to null will render to the "
          "window");

        MFUN(renderpass_get_color_target, SG_CKNames[SG_COMPONENT_TEXTURE],
             "colorOutput");
        DOC_FUNC("Get the target texture to render to.");

        MFUN(renderpass_set_color_target_clear_on_load, "void", "clear");
        ARG("int", "clear");
        DOC_FUNC(
          "Set whether the framebuffer's color target should be cleared each "
          "frame. Default true.");

        MFUN(renderpass_get_color_target_clear_on_load, "int", "clear");
        DOC_FUNC("Get whether the framebuffer's color target is cleared each frame");

        MFUN(renderpass_set_viewport_normalized, "void", "viewportNormalized");
        ARG("float", "x");
        ARG("float", "y");
        ARG("float", "w");
        ARG("float", "h");
        DOC_FUNC(
          "Set the viewport this pass will render to, in normalized in coordinates. "
          "x,y are the starting point. w,h are the width and height of the viewport. "
          "All values are multiplied by the dimensions of the output render texture, "
          "e.g. .viewportNormalized(.5, .5, .5, .5) will render to the bottom-right "
          "quad of the texture");

        MFUN(renderpass_set_viewport_normalized_vec4, "void", "viewportNormalized");
        ARG("vec4", "v");
        DOC_FUNC(
          "Set the viewport this pass will render to, in normalized in coordinates. "
          "x,y are the starting point. w,h are the width and height of the viewport. "
          "All values are multiplied by the dimensions of the output render texture, "
          "e.g. .viewportNormalized(.5, .5, .5, .5) will render to the bottom-right "
          "quad of the texture");

        MFUN(renderpass_set_viewport, "void", "viewport");
        ARG("float", "x");
        ARG("float", "y");
        ARG("float", "w");
        ARG("float", "h");
        DOC_FUNC(
          "Set the viewport this pass will render to, in pixels. "
          "x,y are the starting point. w,h are the width and height of the viewport. "
          "e.g. .viewport(500, 200, 100, 100) will render a 100x100 pixel region "
          "starting at the pixel coordinates (500, 200)");

        MFUN(renderpass_set_viewport_vec4, "void", "viewport");
        ARG("vec4", "vp");
        DOC_FUNC(
          "Set the viewport this pass will render to, in pixels. "
          "x,y are the starting point. w,h are the width and height of the viewport. "
          "e.g. .viewport(500, 200, 100, 100) will render a 100x100 pixel region "
          "starting at the pixel coordinates (500, 200)");

        MFUN(renderpass_set_scissor_normalized, "void", "scissorNormalized");
        ARG("float", "x");
        ARG("float", "y");
        ARG("float", "w");
        ARG("float", "h");
        DOC_FUNC(
          "Sets the scissor rectangle used during the rasterization stage. After "
          "transformation into viewport coordinates any fragments which fall outside "
          "the scissor rectangle will be discarded."
          "All values are multiplied by the dimensions of the output render texture, "
          "e.g. .scissorNormalized(.5, .5, .5, .5) will cut out only the bottom-right "
          "quad of the texture");

        MFUN(renderpass_set_scissor_normalized_vec4, "void", "scissorNormalized");
        ARG("vec4", "s");
        DOC_FUNC(
          "Sets the scissor rectangle used during the rasterization stage. After "
          "transformation into viewport coordinates any fragments which fall outside "
          "the scissor rectangle will be discarded."
          "All values are multiplied by the dimensions of the output render texture, "
          "e.g. .scissorNormalized(.5, .5, .5, .5) will cut out only the bottom-right "
          "quad of the texture");

        MFUN(renderpass_set_scissor, "void", "scissor");
        ARG("float", "x");
        ARG("float", "y");
        ARG("float", "w");
        ARG("float", "h");
        DOC_FUNC(
          "Sets the scissor rectangle (in pixels) used during the rasterization stage. "
          "After transformation into viewport coordinates any fragments which fall "
          "outside the scissor rectangle will be discarded.");

        MFUN(renderpass_set_scissor_vec4, "void", "scissor");
        ARG("vec4", "s");
        DOC_FUNC(
          "Sets the scissor rectangle (in pixels) used during the rasterization stage. "
          "After transformation into viewport coordinates any fragments which fall "
          "outside the scissor rectangle will be discarded.");

        END_CLASS();
    }

    { // ScenePass --------------------------------------------------------------
        BEGIN_CLASS(ulib_pass_classname(SG_PassType_Scene),
                    ulib_pass_classname(SG_PassType_Render));
        DOC_CLASS("Pass to render a GScene from the perspective of a GCamera");

        CTOR(scenepass_ctor_with_scene);
        ARG(SG_CKNames[SG_COMPONENT_SCENE], "scene");
        DOC_FUNC(
          "Constructor that sets the scene to render. Will render from the scene's "
          "main camera, scene.camera()");

        MFUN(scenepass_set_scene_and_camera, "void", "scene");
        ARG(SG_CKNames[SG_COMPONENT_SCENE], "scene");
        ARG(SG_CKNames[SG_COMPONENT_CAMERA], "camera");
        DOC_FUNC(
          "Set the scene and camera to render from. camera *must* be a part of the "
          "scene. Can pass `null` as the camera arg to use the scene's main camera, "
          "scene.camera()");

        MFUN(scenepass_get_scene, SG_CKNames[SG_COMPONENT_SCENE], "scene");
        DOC_FUNC("Get the scene this pass is rendering");

        MFUN(scenepass_get_camera, SG_CKNames[SG_COMPONENT_CAMERA], "camera");
        DOC_FUNC(
          "Get the camera used for rendering the scene. If not set, will default to "
          "the scene's main camera");

        MFUN(scenepass_get_msaa, "int", "msaa");
        DOC_FUNC("Returns whether or not this scenepass has MSAA antialiasing enabled");

        MFUN(scenepass_set_msaa, "void", "msaa");
        ARG("int", "msaa");
        DOC_FUNC(
          "Set whether this scenepass should perform MSAA antialiasing. If true will "
          "render the scene into a 4xMSAA texture and perform an MSAA resolve "
          "into the texture set with ScenePass.colorOutput(). Default false.");

        END_CLASS();
    }

    // ScreenPass ----------------------------------------------------------------
    {
        BEGIN_CLASS(ulib_pass_classname(SG_PassType_Screen),
                    ulib_pass_classname(SG_PassType_Render));
        DOC_CLASS(
          "Screen pass for applying screen shaders and visual effects to the entire "
          "screen ");

        CTOR(screenpass_ctor_with_shader);
        ARG(SG_CKNames[SG_COMPONENT_SHADER], "screen_shader");

        MFUN(screenpass_get_material, SG_CKNames[SG_COMPONENT_MATERIAL], "material");
        DOC_FUNC(
          "Get the internal material of this pass. Use the material to bind uniforms "
          "to the screen shader");

        MFUN(screenpass_set_shader, "void", "shader");
        ARG(SG_CKNames[SG_COMPONENT_SHADER], "shader");
        DOC_FUNC(
          "Set the screen shader to apply to the screen. In your screen shader be sure "
          "to #include SCREEN_PASS_VERTEX_SHADER which supplies your fragment shader "
          "with a full-screen quad");

        END_CLASS();
    }

    // OutputPass ----------------------------------------------------------------
    {
        BEGIN_CLASS("OutputPass", ulib_pass_classname(SG_PassType_Screen));
        DOC_FUNC(
          "Default ChuGL output pass; place at end of render graph. Applies "
          "tonemapping and gamma correction to the input texture");

        static t_CKUINT TONEMAP_NONE      = 0;
        static t_CKUINT TONEMAP_LINEAR    = 1;
        static t_CKUINT TONEMAP_REINHARD  = 2;
        static t_CKUINT TONEMAP_CINEON    = 3;
        static t_CKUINT TONEMAP_ACES      = 4;
        static t_CKUINT TONEMAP_UNCHARTED = 5;
        SVAR("int", "ToneMap_None", &TONEMAP_NONE);
        SVAR("int", "ToneMap_Linear", &TONEMAP_LINEAR);
        SVAR("int", "ToneMap_Reinhard", &TONEMAP_REINHARD);
        SVAR("int", "ToneMap_Cineon", &TONEMAP_CINEON);
        SVAR("int", "ToneMap_ACES", &TONEMAP_ACES);
        SVAR("int", "ToneMap_Uncharted", &TONEMAP_UNCHARTED);

        CTOR(outputpass_ctor);

        MFUN(outputpass_set_input_texture, "void", "input");
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "input_texture");
        DOC_FUNC("Set the input texture to apply tonemapping and gamma correction to");

        MFUN(outputpass_set_tonemap, "void", "tonemap");
        ARG("int", "tonemap_type");
        DOC_FUNC(
          "Set the tonemapping algorithm to apply to the input texture. Choose a value "
          "from the OutputPass.ToneMap_* enum");

        MFUN(outputpass_get_tonemap, "int", "tonemap");
        DOC_FUNC("Get the tonemapping algorithm applied to the input texture");

        MFUN(outputpass_set_gamma, "void", "gamma");
        ARG("int", "gamma");
        DOC_FUNC(
          "Get whether or not the pass is applying gamma correction. Default true");

        MFUN(outputpass_get_gamma, "int", "gamma");
        DOC_FUNC("Set whether or not to apply gamma correction. Default true");

        MFUN(outputpass_set_exposure, "void", "exposure");
        ARG("float", "exposure");
        DOC_FUNC("Set the exposure value for the tonemapping algorithm");

        MFUN(outputpass_get_exposure, "float", "exposure");
        DOC_FUNC("Get the exposure value for the tonemapping algorithm");

        MFUN(outputpass_get_sampler, "TextureSampler", "sampler");
        DOC_FUNC("Get the sampler used for the input texture");

        MFUN(outputpass_set_sampler, "void", "sampler");
        ARG("TextureSampler", "sampler");
        DOC_FUNC("Set the sampler used for the input texture");

        END_CLASS();
    }

    // ComputePass --------------------------------------------------------------
    {
        BEGIN_CLASS("ComputePass", SG_CKNames[SG_COMPONENT_PASS]);
        DOC_CLASS(
          "Compute pass for running compute shaders. Note that unlike Materials, "
          "all Compute Pass bindings must be bound under @group(0), NOT @group(1)");
        ADD_EX("rendergraph/boids_compute.ck");
        ADD_EX("rendergraph/slime.ck");

        CTOR(computepass_ctor_with_shader);
        ARG(SG_CKNames[SG_COMPONENT_SHADER], "shader");
        DOC_FUNC("Create a compute pass with the given shader");

        MFUN(computepass_set_shader, "void", "shader");
        ARG(SG_CKNames[SG_COMPONENT_SHADER], "shader");
        DOC_FUNC("Set the compute shader to run");

        MFUN(computepass_set_uniform_float, "void", "uniformFloat");
        ARG("int", "location");
        ARG("float", "uniform_value");

        MFUN(computepass_set_uniform_float2, "void", "uniformFloat2");
        ARG("int", "location");
        ARG("vec2", "uniform_value");

        MFUN(computepass_set_uniform_float3, "void", "uniformFloat3");
        ARG("int", "location");
        ARG("vec3", "uniform_value");

        MFUN(computepass_set_uniform_float4, "void", "uniformFloat4");
        ARG("int", "location");
        ARG("vec4", "uniform_value");

        MFUN(computepass_set_texture, "void", "texture");
        ARG("int", "location");
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "texture");

        MFUN(computepass_set_storage_buffer, "void", "storageBuffer");
        ARG("int", "location");
        ARG("StorageBuffer", "buffer");

        MFUN(computepass_set_storage_texture, "void", "storageTexture");
        ARG("int", "location");
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "texture");

        MFUN(computepass_set_uniform_int, "void", "uniformInt");
        ARG("int", "location");
        ARG("int", "uniform_value");

        MFUN(computepass_set_workgroup, "void", "workgroup");
        ARG("int", "x");
        ARG("int", "y");
        ARG("int", "z");
        DOC_FUNC(
          "Set the workgroup size for the compute shader. Used to determine the "
          "dimensions of the compute pass dispatch");

        END_CLASS();
    }

    { // BloomPass
        BEGIN_CLASS(ulib_pass_classname(SG_PassType_Bloom),
                    SG_CKNames[SG_COMPONENT_PASS]);
        DOC_FUNC("Bloom pass for applying bloom to a render texture");
        ADD_EX("deep/soundbulb.ck");
        ADD_EX("rendergraph/bloom.ck");

        MFUN(bloompass_set_input_render_texture, "void", "input");
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "bloom_texture");
        DOC_FUNC("Set the render texture to apply bloom to");

        MFUN(bloompass_get_output_render_texture, SG_CKNames[SG_COMPONENT_TEXTURE],
             "colorOutput");
        DOC_FUNC("Get the render texture that the bloom pass writes to");

        MFUN(bloompass_set_internal_blend, "void", "radius");
        ARG("float", "blend_factor");
        DOC_FUNC(
          "Set the blend factor between mip levels of the bloom texture during "
          "upsample");

        MFUN(bloompass_set_final_blend, "void", "intensity");
        ARG("float", "blend_factor");
        DOC_FUNC(
          "Set the blend factor between the bloom texture and the original image");

        MFUN(bloompass_get_internal_blend, "float", "radius");
        DOC_FUNC("Get the blend factor between mip levels of the bloom texture");

        MFUN(bloompass_get_final_blend, "float", "intensity");
        DOC_FUNC(
          "Get the blend factor between the bloom texture and the original image");

        MFUN(bloompass_set_num_levels, "void", "levels");
        ARG("int", "num_levels");
        DOC_FUNC(
          "Number of blur passes to apply to the bloom texture. "
          "Clamped between 0 and 16.");

        MFUN(bloompass_get_num_levels, "int", "levels");
        DOC_FUNC("Get the number of blur passes applied to the bloom texture.");

        MFUN(bloompass_set_threshold, "void", "threshold");
        ARG("float", "threshold");
        DOC_FUNC(
          "Set the threshold for the bloom pass (colors with all rgb values below "
          "threshold are not bloomed)");

        MFUN(bloompass_get_threshold, "float", "threshold");
        DOC_FUNC("Get the threshold for the bloom pass");

        END_CLASS();
    }
}

SG_Pass* ulib_pass_create(SG_PassType pass_type, Chuck_Object* pass_ckobj, bool add_ref,
                          Chuck_VM_Shred* shred)
{
    CK_DL_API API = g_chuglAPI;
    if (pass_ckobj == NULL)
        pass_ckobj = chugin_createCkObj(ulib_pass_classname(pass_type), add_ref, shred);

    SG_Pass* pass = SG_CreatePass(pass_ckobj, pass_type);
    ASSERT(pass->pass_type == pass_type);
    ASSERT(pass->type == SG_COMPONENT_PASS);
    OBJ_MEMBER_UINT(pass_ckobj, component_offset_id) = pass->id;

    switch (pass->pass_type) {
        case SG_PassType_Root:
        case SG_PassType_Render:
        case SG_PassType_Scene: {
        } break;
        case SG_PassType_Screen: {
            SG_Material* mat = ulib_material_create(SG_MATERIAL_CUSTOM, shred);
            SG_Pass::screenMaterial(pass, mat);
        } break;
        case SG_PassType_Compute: {
            // the material is internal, ckobj=NULL so no need to refcount
            SG_Material* mat = chugl_createInternalMaterial(SG_MATERIAL_COMPUTE, NULL);
            pass->compute_material_id = mat->id;
        } break;
        case SG_PassType_Bloom: {
            SG_Shader* bloom_downsample_shader = SG_GetShader(
              g_material_builtin_shaders.bloom_downsample_screen_shader_id);
            SG_Shader* bloom_upsample_shader = SG_GetShader(
              g_material_builtin_shaders.bloom_upsample_screen_shader_id);

            // create default output render texture, which is always resized to match
            // input texture
            SG_TextureDesc output_render_texture_desc = {};
            output_render_texture_desc.usage = WGPUTextureUsage_RenderAttachment
                                               | WGPUTextureUsage_TextureBinding
                                               | WGPUTextureUsage_StorageBinding;
            output_render_texture_desc.format       = WGPUTextureFormat_RGBA16Float;
            output_render_texture_desc.resize_mode  = SG_TextureResizeMode_Ratio;
            output_render_texture_desc.width_ratio  = 1.0f;
            output_render_texture_desc.height_ratio = 1.0f;
            SG_Texture* output_render_texture
              = SG_CreateTexture(&output_render_texture_desc, NULL, shred, false);

            SG_Material* bloom_downsample_mat = chugl_createInternalMaterial(
              SG_MATERIAL_COMPUTE, bloom_downsample_shader);
            SG_Material* bloom_upsample_mat = chugl_createInternalMaterial(
              SG_MATERIAL_COMPUTE, bloom_upsample_shader);

            // initialize uniforms for downsample mat
            SG_Material::uniformFloat(bloom_downsample_mat, 2, 0.0); // threshold
            CQ_PushCommand_MaterialSetUniform(bloom_downsample_mat, 2);

            // initialize uniforms for upsample mat
            SG_Material::uniformFloat(bloom_upsample_mat, 4, 0.85); // internal blend
            CQ_PushCommand_MaterialSetUniform(bloom_upsample_mat, 4);
            SG_Material::uniformFloat(bloom_upsample_mat, 5, 0.2); // final blend
            CQ_PushCommand_MaterialSetUniform(bloom_upsample_mat, 5);

            // update pass
            pass->bloom_downsample_material_id = bloom_downsample_mat->id;
            pass->bloom_upsample_material_id   = bloom_upsample_mat->id;
            SG_Pass::bloomOutputRenderTexture(pass, output_render_texture);
        } break;
        default: UNREACHABLE
    }

    CQ_PushCommand_PassUpdate(pass);
    return pass;
}

CK_DLL_CTOR(pass_ctor)
{
    for (u8 pass_type = SG_PassType_Root; pass_type < SG_PassType_Count; pass_type++) {
        if (chugin_typeEquals(SELF, ulib_pass_classname((SG_PassType)pass_type))) {
            ulib_pass_create((SG_PassType)pass_type, SELF, false, SHRED);
            break;
        }
    }
}

CK_DLL_MFUN(pass_get_next)
{
    SG_Pass* pass      = GET_PASS(SELF);
    SG_Pass* next_pass = SG_GetPass(pass->next_pass_id);

    RETURN->v_object = next_pass ? next_pass->ckobj : NULL;
}

CK_DLL_GFUN(pass_op_gruck)
{
    // get the arguments
    Chuck_Object* lhs = GET_NEXT_OBJECT(ARGS);
    Chuck_Object* rhs = GET_NEXT_OBJECT(ARGS);

    if (!lhs) {
        log_warn("Cannot connect [null] --> GPass");
        return;
    }

    // get internal representation
    ASSERT(lhs);
    SG_Pass* lhs_pass = SG_GetPass(OBJ_MEMBER_UINT(lhs, component_offset_id));
    SG_Pass* rhs_pass
      = rhs ? SG_GetPass(OBJ_MEMBER_UINT(rhs, component_offset_id)) : NULL;

    if (!SG_Pass::connect(lhs_pass, rhs_pass)) {
        log_warn(
          "warning GPass --> GPass failed! Cannot form cycles in the GPass chain");
    }

    // command
    CQ_PushCommand_PassUpdate(lhs_pass);

    // return RHS
    RETURN->v_object = rhs;
}

CK_DLL_GFUN(pass_op_ungruck)
{
    // get the arguments
    Chuck_Object* lhs = GET_NEXT_OBJECT(ARGS);
    Chuck_Object* rhs = GET_NEXT_OBJECT(ARGS);

    // get internal representation
    SG_Pass* lhs_pass = SG_GetPass(OBJ_MEMBER_UINT(lhs, component_offset_id));
    SG_Pass* rhs_pass = SG_GetPass(OBJ_MEMBER_UINT(rhs, component_offset_id));

    SG_Pass::disconnect(lhs_pass, rhs_pass);

    // command
    CQ_PushCommand_PassUpdate(lhs_pass);

    // return RHS
    RETURN->v_object = rhs;
}

// ============================================================================
// RenderPass
// ============================================================================

CK_DLL_MFUN(renderpass_set_color_target)
{
    SG_Pass* pass = GET_PASS(SELF);

    Chuck_Object* target = GET_NEXT_OBJECT(ARGS);
    SG_Texture* texture
      = target ? SG_GetTexture(OBJ_MEMBER_UINT(target, component_offset_id)) : NULL;
    SG_Pass::colorTarget(pass, texture);

    // command TODO
    CQ_PushCommand_PassUpdate(pass);
}

CK_DLL_MFUN(renderpass_get_color_target)
{
    SG_Pass* pass = GET_PASS(SELF);

    SG_Texture* texture = SG_GetTexture(pass->color_target_id);
    RETURN->v_object    = texture ? texture->ckobj : NULL;
}

CK_DLL_MFUN(renderpass_set_color_target_clear_on_load)
{
    SG_Pass* pass                    = GET_PASS(SELF);
    pass->color_target_clear_on_load = GET_NEXT_INT(ARGS);
    CQ_PushCommand_PassUpdate(pass);
}

CK_DLL_MFUN(renderpass_get_color_target_clear_on_load)
{
    RETURN->v_int = GET_PASS(SELF)->color_target_clear_on_load;
}

CK_DLL_MFUN(renderpass_set_viewport_normalized)
{
    SG_Pass* pass             = GET_PASS(SELF);
    pass->viewport_normalized = true;
    pass->viewport_x          = GET_NEXT_FLOAT(ARGS);
    pass->viewport_y          = GET_NEXT_FLOAT(ARGS);
    pass->viewport_w          = GET_NEXT_FLOAT(ARGS);
    pass->viewport_h          = GET_NEXT_FLOAT(ARGS);
    CQ_PushCommand_PassUpdate(pass);
}

CK_DLL_MFUN(renderpass_set_viewport_normalized_vec4)
{
    SG_Pass* pass             = GET_PASS(SELF);
    t_CKVEC4 vp               = GET_NEXT_VEC4(ARGS);
    pass->viewport_normalized = true;
    pass->viewport_x          = vp.x;
    pass->viewport_y          = vp.y;
    pass->viewport_w          = vp.z;
    pass->viewport_h          = vp.w;
    CQ_PushCommand_PassUpdate(pass);
}

CK_DLL_MFUN(renderpass_set_viewport)
{
    SG_Pass* pass             = GET_PASS(SELF);
    pass->viewport_normalized = false;
    pass->viewport_x          = GET_NEXT_FLOAT(ARGS);
    pass->viewport_y          = GET_NEXT_FLOAT(ARGS);
    pass->viewport_w          = GET_NEXT_FLOAT(ARGS);
    pass->viewport_h          = GET_NEXT_FLOAT(ARGS);
    CQ_PushCommand_PassUpdate(pass);
}

CK_DLL_MFUN(renderpass_set_viewport_vec4)
{
    SG_Pass* pass             = GET_PASS(SELF);
    t_CKVEC4 vp               = GET_NEXT_VEC4(ARGS);
    pass->viewport_normalized = false;
    pass->viewport_x          = vp.x;
    pass->viewport_y          = vp.y;
    pass->viewport_w          = vp.z;
    pass->viewport_h          = vp.w;
    CQ_PushCommand_PassUpdate(pass);
}

CK_DLL_MFUN(renderpass_set_scissor_normalized)
{
    SG_Pass* pass            = GET_PASS(SELF);
    pass->scissor_normalized = true;
    pass->scissor_x          = GET_NEXT_FLOAT(ARGS);
    pass->scissor_y          = GET_NEXT_FLOAT(ARGS);
    pass->scissor_w          = GET_NEXT_FLOAT(ARGS);
    pass->scissor_h          = GET_NEXT_FLOAT(ARGS);
    CQ_PushCommand_PassUpdate(pass);
}

CK_DLL_MFUN(renderpass_set_scissor_normalized_vec4)
{
    SG_Pass* pass            = GET_PASS(SELF);
    t_CKVEC4 vp              = GET_NEXT_VEC4(ARGS);
    pass->scissor_normalized = true;
    pass->scissor_x          = vp.x;
    pass->scissor_y          = vp.y;
    pass->scissor_w          = vp.z;
    pass->scissor_h          = vp.w;
    CQ_PushCommand_PassUpdate(pass);
}

CK_DLL_MFUN(renderpass_set_scissor)
{
    SG_Pass* pass            = GET_PASS(SELF);
    pass->scissor_normalized = false;
    pass->scissor_x          = GET_NEXT_FLOAT(ARGS);
    pass->scissor_y          = GET_NEXT_FLOAT(ARGS);
    pass->scissor_w          = GET_NEXT_FLOAT(ARGS);
    pass->scissor_h          = GET_NEXT_FLOAT(ARGS);
    CQ_PushCommand_PassUpdate(pass);
}

CK_DLL_MFUN(renderpass_set_scissor_vec4)
{
    SG_Pass* pass            = GET_PASS(SELF);
    t_CKVEC4 vp              = GET_NEXT_VEC4(ARGS);
    pass->scissor_normalized = false;
    pass->scissor_x          = vp.x;
    pass->scissor_y          = vp.y;
    pass->scissor_w          = vp.z;
    pass->scissor_h          = vp.w;
    CQ_PushCommand_PassUpdate(pass);
}

// ============================================================================
// ScenePass
// ============================================================================

CK_DLL_CTOR(scenepass_ctor_with_scene)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass->pass_type == SG_PassType_Scene);
    Chuck_Object* scene = GET_NEXT_OBJECT(ARGS);

    SG_Scene* sg_scene
      = scene ? SG_GetScene(OBJ_MEMBER_UINT(scene, component_offset_id)) : NULL;
    SG_Pass::scene(pass, sg_scene);

    CQ_PushCommand_PassUpdate(pass);
}

CK_DLL_MFUN(scenepass_get_camera)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass->pass_type == SG_PassType_Scene);
    SG_Camera* sg_camera = NULL;

    if (pass->camera_id) {
        sg_camera = SG_GetCamera(pass->camera_id);
    } else if (pass->scene_id) {
        SG_Scene* scene = SG_GetScene(pass->scene_id);
        sg_camera       = SG_GetCamera(scene->desc.main_camera_id);
    }

    RETURN->v_object = sg_camera ? sg_camera->ckobj : NULL;
}

CK_DLL_MFUN(scenepass_set_scene_and_camera)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass->pass_type == SG_PassType_Scene);
    Chuck_Object* scene  = GET_NEXT_OBJECT(ARGS);
    Chuck_Object* camera = GET_NEXT_OBJECT(ARGS);

    SG_Scene* sg_scene
      = scene ? SG_GetScene(OBJ_MEMBER_UINT(scene, component_offset_id)) : NULL;
    SG_ID scene_id    = sg_scene ? sg_scene->id : 0;
    SG_Camera* sg_cam = camera ? GET_CAMERA(camera) : NULL;

    if (sg_cam && sg_cam->scene_id != scene_id) {
        log_warn(
          "ScenePass.scene(GScene, GCamera) failed: camera does not belong to scene. "
          "Suggestion: connect with `camera --> scene;`");
        return;
    }

    SG_Pass::scene(pass, sg_scene);
    SG_Pass::camera(pass, sg_cam);

    CQ_PushCommand_PassUpdate(pass);
}

CK_DLL_MFUN(scenepass_get_scene)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass->pass_type == SG_PassType_Scene);

    SG_Scene* sg_scene = SG_GetScene(pass->scene_id);
    RETURN->v_object   = sg_scene ? sg_scene->ckobj : NULL;
}

CK_DLL_MFUN(scenepass_get_msaa)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass->pass_type == SG_PassType_Scene);
    RETURN->v_int = pass->scene_pass_msaa;
}

CK_DLL_MFUN(scenepass_set_msaa)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass->pass_type == SG_PassType_Scene);
    int msaa              = GET_NEXT_INT(ARGS);
    pass->scene_pass_msaa = msaa ? 1 : 0;

    CQ_PushCommand_PassUpdate(pass);
}

// ============================================================================
// ScreenPass
// ============================================================================

CK_DLL_CTOR(screenpass_ctor_with_shader)
{
    // get the arguments
    Chuck_Object* screen_shader = GET_NEXT_OBJECT(ARGS);
    if (screen_shader == NULL) return;

    SG_Shader* shader
      = SG_GetShader(OBJ_MEMBER_UINT(screen_shader, component_offset_id));
    ASSERT(shader);
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass && pass->pass_type == SG_PassType_Screen);

    SG_Material* screen_material = SG_GetMaterial(pass->screen_material_id);
    ASSERT(screen_material);
    ulib_material_set_shader(screen_material, shader);
}

CK_DLL_MFUN(screenpass_get_material)
{
    SG_Pass* pass    = GET_PASS(SELF);
    RETURN->v_object = SG_GetMaterial(pass->screen_material_id)->ckobj;
}

CK_DLL_MFUN(screenpass_set_shader)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass->pass_type == SG_PassType_Screen);

    Chuck_Object* shader_obj = GET_NEXT_OBJECT(ARGS);
    SG_Shader* shader
      = shader_obj ? SG_GetShader(OBJ_MEMBER_UINT(shader_obj, component_offset_id)) :
                     NULL;

    SG_Material* mat = SG_GetMaterial(pass->screen_material_id);
    ASSERT(mat);

    ulib_material_set_shader(mat, shader);

    CQ_PushCommand_PassUpdate(pass);
}

// ============================================================================
// OutputPass
// ============================================================================

SG_Pass* ulib_pass_create_output_pass(SG_Pass* pass, Chuck_Object* ckobj, bool add_ref,
                                      Chuck_VM_Shred* shred)
{
    if (pass == NULL)
        pass = ulib_pass_create(SG_PassType_Screen, ckobj, add_ref, shred);
    SG_Material* mat = SG_GetMaterial(pass->screen_material_id);

    SG_Shader* output_pass_shader
      = SG_GetShader(g_material_builtin_shaders.output_pass_shader_id);
    ASSERT(output_pass_shader);

    ulib_material_set_shader(mat, output_pass_shader);

    // set output_pass uniforms
    SG_Material::setTexture(mat, 0, SG_GetTexture(g_builtin_textures.white_pixel_id));
    SG_Material::setSampler(mat, 1, SG_SAMPLER_DEFAULT); // sampler
    SG_Material::uniformFloat(mat, 2, 2.2);              // gamma
    SG_Material::uniformFloat(mat, 3, 1.0);              // exposure
    // SG_Material::uniformInt(mat, 4, 4); // TONEMAP_ACES
    SG_Material::uniformInt(mat, 4, 5); // TONEMAP_UNCHARTED
    ulib_material_cq_update_all_uniforms(mat);

    return pass;
}

CK_DLL_CTOR(outputpass_ctor)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass && pass->pass_type == SG_PassType_Screen);
    ulib_pass_create_output_pass(pass, SELF, false, SHRED);
}

CK_DLL_MFUN(outputpass_set_input_texture)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass->pass_type == SG_PassType_Screen);

    SG_Texture* input_texture
      = SG_GetTexture(OBJ_MEMBER_UINT(GET_NEXT_OBJECT(ARGS), component_offset_id));
    if (!input_texture) return;

    SG_Material* material = SG_GetMaterial(pass->screen_material_id);
    ASSERT(material);

    // set uniform
    SG_Material::setTexture(material, 0, input_texture);
    CQ_PushCommand_MaterialSetUniform(material, 0);
}

CK_DLL_MFUN(outputpass_set_tonemap)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass->pass_type == SG_PassType_Screen);

    t_CKINT tonemap_type = GET_NEXT_INT(ARGS);

    SG_Material* material = SG_GetMaterial(pass->screen_material_id);
    ASSERT(material);

    // set uniform
    SG_Material::uniformInt(material, 4, tonemap_type);
    CQ_PushCommand_MaterialSetUniform(material, 4);
}

CK_DLL_MFUN(outputpass_get_tonemap)
{
    SG_Pass* pass         = GET_PASS(SELF);
    SG_Material* material = SG_GetMaterial(pass->screen_material_id);
    RETURN->v_int         = material->uniforms[4].as.i;
}

CK_DLL_MFUN(outputpass_set_gamma)
{
    SG_Pass* pass         = GET_PASS(SELF);
    int gamma             = GET_NEXT_INT(ARGS) ? 1 : 0;
    SG_Material* material = SG_GetMaterial(pass->screen_material_id);

    // set uniform
    // TODO can use CODE(...) macro to replace 2 with OUTPUT_PASS_GAMMA_BINDING
    SG_Material::uniformInt(material, 2, gamma);
    CQ_PushCommand_MaterialSetUniform(material, 2);
}

CK_DLL_MFUN(outputpass_get_gamma)
{
    SG_Pass* pass         = GET_PASS(SELF);
    SG_Material* material = SG_GetMaterial(pass->screen_material_id);
    RETURN->v_int         = material->uniforms[2].as.i;
}

CK_DLL_MFUN(outputpass_set_exposure)
{
    SG_Pass* pass         = GET_PASS(SELF);
    t_CKFLOAT exposure    = GET_NEXT_FLOAT(ARGS);
    SG_Material* material = SG_GetMaterial(pass->screen_material_id);

    // set uniform
    SG_Material::uniformFloat(material, 3, exposure);
    CQ_PushCommand_MaterialSetUniform(material, 3);
}

CK_DLL_MFUN(outputpass_get_exposure)
{
    SG_Pass* pass         = GET_PASS(SELF);
    SG_Material* material = SG_GetMaterial(pass->screen_material_id);
    RETURN->v_float       = material->uniforms[3].as.f;
}

CK_DLL_MFUN(outputpass_get_sampler)
{
    SG_Pass* pass         = GET_PASS(SELF);
    SG_Material* material = SG_GetMaterial(pass->screen_material_id);

    RETURN->v_object
      = ulib_texture_ckobj_from_sampler(material->uniforms[1].as.sampler, false, SHRED);
}

CK_DLL_MFUN(outputpass_set_sampler)
{
    SG_Pass* pass         = GET_PASS(SELF);
    SG_Material* material = SG_GetMaterial(pass->screen_material_id);

    // set uniform
    SG_Material::setSampler(material, 1, SG_Sampler::fromCkObj(GET_NEXT_OBJECT(ARGS)));
    CQ_PushCommand_MaterialSetUniform(material, 1);
}

// ============================================================================
// ComputePass
// ============================================================================

CK_DLL_CTOR(computepass_ctor_with_shader)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass && pass->pass_type == SG_PassType_Compute);
    SG_Shader* shader
      = SG_GetShader(OBJ_MEMBER_UINT(GET_NEXT_OBJECT(ARGS), component_offset_id));
    SG_Material* mat = SG_GetMaterial(pass->compute_material_id);
    ulib_material_set_shader(mat, shader);
}

CK_DLL_MFUN(computepass_set_shader)
{
    SG_Pass* pass = GET_PASS(SELF);
    SG_Shader* shader
      = SG_GetShader(OBJ_MEMBER_UINT(GET_NEXT_OBJECT(ARGS), component_offset_id));
    SG_Material* mat = SG_GetMaterial(pass->compute_material_id);
    ulib_material_set_shader(mat, shader);
}

CK_DLL_MFUN(computepass_set_uniform_float)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass->pass_type == SG_PassType_Compute);

    t_CKINT location        = GET_NEXT_INT(ARGS);
    t_CKFLOAT uniform_value = GET_NEXT_FLOAT(ARGS);

    SG_Material* material = SG_GetMaterial(pass->compute_material_id);

    // set uniform
    SG_Material::uniformFloat(material, location, uniform_value);
    CQ_PushCommand_MaterialSetUniform(material, location);
}

CK_DLL_MFUN(computepass_set_uniform_float2)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass->pass_type == SG_PassType_Compute);

    t_CKINT location       = GET_NEXT_INT(ARGS);
    t_CKVEC2 uniform_value = GET_NEXT_VEC2(ARGS);

    SG_Material* material = SG_GetMaterial(pass->compute_material_id);

    // set uniform
    SG_Material::uniformVec2f(material, location, { uniform_value.x, uniform_value.y });
    CQ_PushCommand_MaterialSetUniform(material, location);
}

CK_DLL_MFUN(computepass_set_uniform_float3)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass->pass_type == SG_PassType_Compute);

    t_CKINT location       = GET_NEXT_INT(ARGS);
    t_CKVEC3 uniform_value = GET_NEXT_VEC3(ARGS);

    SG_Material* material = SG_GetMaterial(pass->compute_material_id);

    SG_Material::uniformVec3f(material, location,
                              { uniform_value.x, uniform_value.y, uniform_value.z });
    CQ_PushCommand_MaterialSetUniform(material, location);
}

CK_DLL_MFUN(computepass_set_uniform_float4)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass->pass_type == SG_PassType_Compute);

    t_CKINT location       = GET_NEXT_INT(ARGS);
    t_CKVEC4 uniform_value = GET_NEXT_VEC4(ARGS);

    SG_Material* material = SG_GetMaterial(pass->compute_material_id);

    // set uniform
    SG_Material::uniformVec4f(
      material, location,
      { uniform_value.x, uniform_value.y, uniform_value.z, uniform_value.w });
    CQ_PushCommand_MaterialSetUniform(material, location);
}

CK_DLL_MFUN(computepass_set_texture)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass->pass_type == SG_PassType_Compute);

    t_CKINT location        = GET_NEXT_INT(ARGS);
    Chuck_Object* tex_ckobj = GET_NEXT_OBJECT(ARGS);
    if (!tex_ckobj) return;

    SG_Texture* tex = SG_GetTexture(OBJ_MEMBER_UINT(tex_ckobj, component_offset_id));
    SG_Material* material = SG_GetMaterial(pass->compute_material_id);

    SG_Material::setTexture(material, location, tex);
    CQ_PushCommand_MaterialSetUniform(material, location);
}

CK_DLL_MFUN(computepass_set_storage_buffer)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass->pass_type == SG_PassType_Compute);

    t_CKINT location     = GET_NEXT_INT(ARGS);
    Chuck_Object* buffer = GET_NEXT_OBJECT(ARGS);

    SG_Material* material = SG_GetMaterial(pass->compute_material_id);
    SG_Buffer* sg_buffer  = SG_GetBuffer(OBJ_MEMBER_UINT(buffer, component_offset_id));

    // set storage buffer
    SG_Material::storageBuffer(material, location, sg_buffer);
    CQ_PushCommand_MaterialSetUniform(material, location);
}

CK_DLL_MFUN(computepass_set_storage_texture)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass->pass_type == SG_PassType_Compute);

    t_CKINT location        = GET_NEXT_INT(ARGS);
    Chuck_Object* tex_ckobj = GET_NEXT_OBJECT(ARGS);
    if (!tex_ckobj) return;

    SG_Texture* tex = SG_GetTexture(OBJ_MEMBER_UINT(tex_ckobj, component_offset_id));
    SG_Material* material = SG_GetMaterial(pass->compute_material_id);

    SG_Material::setStorageTexture(material, location, tex);
    CQ_PushCommand_MaterialSetUniform(material, location);
}

CK_DLL_MFUN(computepass_set_workgroup)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass->pass_type == SG_PassType_Compute);

    u32 x = GET_NEXT_INT(ARGS);
    u32 y = GET_NEXT_INT(ARGS);
    u32 z = GET_NEXT_INT(ARGS);

    pass->compute_workgroup = { x, y, z };
    CQ_PushCommand_PassUpdate(pass);
}

CK_DLL_MFUN(computepass_set_uniform_int)
{
    SG_Pass* pass = GET_PASS(SELF);
    ASSERT(pass->pass_type == SG_PassType_Compute);

    t_CKINT location      = GET_NEXT_INT(ARGS);
    t_CKINT uniform_value = GET_NEXT_INT(ARGS);

    SG_Material* material = SG_GetMaterial(pass->compute_material_id);

    // set uniform
    SG_Material::uniformInt(material, location, uniform_value);
    CQ_PushCommand_MaterialSetUniform(material, location);
}

// ============================================================================
// BloomPass
// ============================================================================

CK_DLL_MFUN(bloompass_set_input_render_texture)
{
    SG_Pass* pass = GET_PASS(SELF);

    SG_Texture* bloom_texture
      = SG_GetTexture(OBJ_MEMBER_UINT(GET_NEXT_OBJECT(ARGS), component_offset_id));

    SG_Pass::bloomInputRenderTexture(pass, bloom_texture);

    CQ_PushCommand_PassUpdate(pass);
}

CK_DLL_MFUN(bloompass_get_output_render_texture)
{
    SG_Pass* pass = GET_PASS(SELF);

    SG_Texture* bloom_texture = SG_GetTexture(pass->bloom_output_render_texture_id);

    RETURN->v_object = bloom_texture ? bloom_texture->ckobj : NULL;
}

CK_DLL_MFUN(bloompass_set_internal_blend)
{
    SG_Pass* pass   = GET_PASS(SELF);
    t_CKFLOAT blend = GET_NEXT_FLOAT(ARGS);

    SG_Material* bloom_upsample_material
      = SG_GetMaterial(pass->bloom_upsample_material_id);

    SG_Material::uniformFloat(bloom_upsample_material, 4, blend);
    CQ_PushCommand_MaterialSetUniform(bloom_upsample_material, 4);
}

CK_DLL_MFUN(bloompass_set_final_blend)
{
    SG_Pass* pass   = GET_PASS(SELF);
    t_CKFLOAT blend = GET_NEXT_FLOAT(ARGS);

    SG_Material* bloom_upsample_material
      = SG_GetMaterial(pass->bloom_upsample_material_id);

    SG_Material::uniformFloat(bloom_upsample_material, 5, blend);
    CQ_PushCommand_MaterialSetUniform(bloom_upsample_material, 5);
}

CK_DLL_MFUN(bloompass_get_internal_blend)
{
    SG_Pass* pass = GET_PASS(SELF);

    SG_Material* bloom_upsample_material
      = SG_GetMaterial(pass->bloom_upsample_material_id);

    RETURN->v_float = bloom_upsample_material->uniforms[4].as.f;
}

CK_DLL_MFUN(bloompass_get_final_blend)
{
    SG_Pass* pass = GET_PASS(SELF);

    SG_Material* bloom_upsample_material
      = SG_GetMaterial(pass->bloom_upsample_material_id);

    RETURN->v_float = bloom_upsample_material->uniforms[5].as.f;
}

CK_DLL_MFUN(bloompass_set_num_levels)
{
    SG_Pass* pass               = GET_PASS(SELF);
    pass->bloom_num_blur_levels = GET_NEXT_INT(ARGS);
    CLAMP(pass->bloom_num_blur_levels, 1, 16);

    CQ_PushCommand_PassUpdate(pass);
}

CK_DLL_MFUN(bloompass_get_num_levels)
{
    SG_Pass* pass = GET_PASS(SELF);
    RETURN->v_int = pass->bloom_num_blur_levels;
}

CK_DLL_MFUN(bloompass_set_threshold)
{
    SG_Pass* pass       = GET_PASS(SELF);
    t_CKFLOAT threshold = GET_NEXT_FLOAT(ARGS);

    SG_Material* bloom_downsample_material
      = SG_GetMaterial(pass->bloom_downsample_material_id);

    SG_Material::uniformFloat(bloom_downsample_material, 2, threshold);
    CQ_PushCommand_MaterialSetUniform(bloom_downsample_material, 2);
}

CK_DLL_MFUN(bloompass_get_threshold)
{
    SG_Pass* pass = GET_PASS(SELF);
    SG_Material* bloom_downsample_material
      = SG_GetMaterial(pass->bloom_downsample_material_id);

    RETURN->v_float = bloom_downsample_material->uniforms[2].as.f;
}
