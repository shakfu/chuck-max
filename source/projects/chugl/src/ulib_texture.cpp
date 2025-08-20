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
#include <chuck/chugin.h>

#include "sg_command.h"
#include "sg_component.h"

#include "ulib_helper.h"

#include "core/file.h"
#include "core/log.h"

#include <stb/stb_image.h>

void ulib_texture_createDefaults(CK_DL_API API);

// TextureSampler ---------------------------------------------------------------------
CK_DLL_CTOR(sampler_ctor);
CK_DLL_SFUN(sampler_nearest);
CK_DLL_SFUN(sampler_linear);

// TextureDesc -----------------------------------------------------------------

static t_CKUINT texture_desc_format_offset    = 0;
static t_CKUINT texture_desc_dimension_offset = 0;
static t_CKUINT texture_desc_width_offset     = 0;
static t_CKUINT texture_desc_height_offset    = 0;
static t_CKUINT texture_desc_depth_offset     = 0;
static t_CKUINT texture_desc_usage_offset     = 0;
// static t_CKUINT texture_desc_samples_offset   = 0; // not exposing for now
static t_CKUINT texture_desc_mips_offset = 0;

static t_CKUINT texture_desc_resizable_offset    = 0;
static t_CKUINT texture_desc_width_ratio_offset  = 0;
static t_CKUINT texture_desc_height_ratio_offset = 0;
CK_DLL_CTOR(texture_desc_ctor);

// TextureWriteDesc -----------------------------------------------------------------
CK_DLL_CTOR(texture_write_desc_ctor);
// dst image location
static t_CKUINT texture_write_desc_mip_offset      = 0;
static t_CKUINT texture_write_desc_offset_x_offset = 0;
static t_CKUINT texture_write_desc_offset_y_offset = 0;
static t_CKUINT texture_write_desc_offset_z_offset = 0;
// dst region size
static t_CKUINT texture_write_desc_width_offset  = 0;
static t_CKUINT texture_write_desc_height_offset = 0;
static t_CKUINT texture_write_desc_depth_offset  = 0;

// TextureLocation -----------------------------------------------------------------
// wrapper around WGPUImageCopyTexture, describes a region of a texture to copy to/from
// NOT storying the actual texture in here so we don't need to worry about refcounting

CK_DLL_CTOR(texture_location_ctor);

static t_CKUINT texture_location_mip_offset      = 0;
static t_CKUINT texture_location_origin_x_offset = 0;
static t_CKUINT texture_location_origin_y_offset = 0;
static t_CKUINT texture_location_origin_z_offset = 0;
// static t_CKUINT texture_location_aspect_offset   = 0;

// TextureWriteDesc -----------------------------------------------------------------
CK_DLL_CTOR(texture_load_desc_ctor);

static t_CKUINT texture_load_desc_flip_y_offset   = 0;
static t_CKUINT texture_load_desc_gen_mips_offset = 0;

// Texture ---------------------------------------------------------------------
CK_DLL_CTOR(texture_ctor);
CK_DLL_CTOR(texture_ctor_with_desc);

CK_DLL_MFUN(texture_get_format);
CK_DLL_MFUN(texture_get_dimension);
CK_DLL_MFUN(texture_get_width);
CK_DLL_MFUN(texture_get_height);
CK_DLL_MFUN(texture_get_depth);
CK_DLL_MFUN(texture_get_usage);
CK_DLL_MFUN(texture_get_mips);
CK_DLL_MFUN(texture_get_resizable);
CK_DLL_MFUN(texture_get_width_ratio);
CK_DLL_MFUN(texture_get_height_ratio);

CK_DLL_MFUN(texture_write);
CK_DLL_MFUN(texture_write_with_desc);
CK_DLL_MFUN(texture_write_external_ptr);

// read texture to CPU
CK_DLL_MFUN(texture_read_to_cpu);
CK_DLL_MFUN(texture_get_data);

// loading images
CK_DLL_SFUN(texture_load_2d_file);
CK_DLL_SFUN(texture_load_2d_raw);
CK_DLL_SFUN(texture_load_2d_file_with_params);
CK_DLL_SFUN(texture_load_cubemap);

// copy texture
CK_DLL_SFUN(texture_copy_texture_to_texture);
CK_DLL_SFUN(texture_copy_texture_to_texture_with_desc);

static void ulib_texture_query(Chuck_DL_Query* QUERY)
{
    { // Sampler (only passed by value)
        QUERY->begin_class(QUERY, "TextureSampler", "Object");
        DOC_CLASS("Texture Sampler -- options for sampling a texture");

        // static vars
        static t_CKINT WRAP_REPEAT    = SG_SAMPLER_WRAP_REPEAT;
        static t_CKINT WRAP_MIRROR    = SG_SAMPLER_WRAP_MIRROR_REPEAT;
        static t_CKINT WRAP_CLAMP     = SG_SAMPLER_WRAP_CLAMP_TO_EDGE;
        static t_CKINT FILTER_NEAREST = SG_SAMPLER_FILTER_NEAREST;
        static t_CKINT FILTER_LINEAR  = SG_SAMPLER_FILTER_LINEAR;
        QUERY->add_svar(QUERY, "int", "Wrap_Repeat", true, &WRAP_REPEAT);
        QUERY->add_svar(QUERY, "int", "Wrap_Mirror", true, &WRAP_MIRROR);
        QUERY->add_svar(QUERY, "int", "Wrap_Clamp", true, &WRAP_CLAMP);
        QUERY->add_svar(QUERY, "int", "Filter_Nearest", true, &FILTER_NEAREST);
        QUERY->add_svar(QUERY, "int", "Filter_Linear", true, &FILTER_LINEAR);

        // member vars
        sampler_offset_wrapU = QUERY->add_mvar(QUERY, "int", "wrapU", false);
        DOC_VAR(
          "U-axis (horizontal) wrap mode. Valid values are TextureSampler.Wrap_Repeat, "
          "TextureSampler.Wrap_Mirror, TextureSampler.Wrap_Clamp");
        sampler_offset_wrapV = QUERY->add_mvar(QUERY, "int", "wrapV", false);
        DOC_VAR(
          "V-axis (vertical) wrap mode. Valid values are TextureSampler.Wrap_Repeat, "
          "TextureSampler.Wrap_Mirror, TextureSampler.Wrap_Clamp");
        sampler_offset_wrapW = QUERY->add_mvar(QUERY, "int", "wrapW", false);
        DOC_VAR(
          "W-axis wrap mode. Valid values are TextureSampler.Wrap_Repeat, "
          "TextureSampler.Wrap_Mirror, TextureSampler.Wrap_Clamp");
        sampler_offset_filterMin = QUERY->add_mvar(QUERY, "int", "filterMin", false);
        DOC_VAR(
          "Minification filter. Valid values are TextureSampler.Filter_Nearest, "
          "TextureSampler.Filter_Linear");
        sampler_offset_filterMag = QUERY->add_mvar(QUERY, "int", "filterMag", false);
        DOC_VAR(
          "Magnification filter. Valid values are TextureSampler.Filter_Nearest, "
          "TextureSampler.Filter_Linear");
        sampler_offset_filterMip = QUERY->add_mvar(QUERY, "int", "filterMip", false);
        DOC_VAR(
          "Mip level filter. Valid values are TextureSampler.Filter_Nearest, "
          "TextureSampler.Filter_Linear");

        // constructor
        QUERY->add_ctor(QUERY, sampler_ctor); // default constructor
        DOC_FUNC(
          "Constructor. Defaults to a sampler with nearest filtering and repeat "
          "wrapping");

        SFUN(sampler_nearest, "TextureSampler", "nearest");
        DOC_FUNC(
          "Helper for creating a TextureSampler with nearest filtering and repeat "
          "wrapping");

        SFUN(sampler_linear, "TextureSampler", "linear");
        DOC_FUNC(
          "Helper for creating a TextureSampler with linear filtering and repeat "
          "wrapping");

        QUERY->end_class(QUERY); // Sampler
    }

    { // TextureDesc
        BEGIN_CLASS("TextureDesc", "Object");
        DOC_CLASS("Texture Descriptor -- options for creating a texture");

        CTOR(texture_desc_ctor);

        // member vars
        texture_desc_format_offset = MVAR("int", "format", false);
        DOC_VAR(
          "Texture format. Valid options are defined in the Texture.Format_* enum. "
          "Default is Texture.Format_RGBA8Unorm");

        texture_desc_dimension_offset = MVAR("int", "dimension", false);
        DOC_VAR(
          "Texture dimension. Valid options are defined in the Texture.Dimension_* "
          "enum. Default is Texture.Dimension_2D");

        texture_desc_width_offset = MVAR("int", "width", false);
        DOC_VAR("Width in texels. Default is 1");

        texture_desc_height_offset = MVAR("int", "height", false);
        DOC_VAR("Height in texels. Default is 1");

        texture_desc_depth_offset = MVAR("int", "depth", false);
        DOC_VAR("Depth in texels. Default is 1");

        texture_desc_usage_offset = MVAR("int", "usage", false);
        DOC_VAR(
          "Bit mask of texture usage flags. Valid flags are defined in the "
          "Texture.Usage_* enum. Default is Texture.Usage_All, which enables all "
          "usages");
        // texture_desc_samples_offset   = MVAR("int", "samples");

        texture_desc_mips_offset = MVAR("int", "mips", false);
        DOC_VAR(
          "0 for false, 1 for true. Set to true to generate a full mip-chain for this "
          "texture. Default is true");

        texture_desc_resizable_offset = MVAR("int", "resizable", false);
        DOC_VAR(
          "0 for false, 1 for true. Set to true to tell the renderer to dynamically "
          "resize this texture when used as a color target in a renderpass. If true, "
          "the size will be determined from .widthRatio and .heightRatio times the "
          "window dimensions. Only relevant to textures that are bound to a renderpass "
          "color output");

        texture_desc_width_ratio_offset = MVAR("float", "widthRatio", false);
        DOC_VAR("See the notes for .resizable. Default 1.0");
        texture_desc_height_ratio_offset = MVAR("float", "heightRatio", false);
        DOC_VAR("See the notes for .resizable. Default 1.0");

        END_CLASS();
    } // end TextureDesc

    { // TextureWriteDesc
        BEGIN_CLASS("TextureWriteDesc", "Object");
        DOC_CLASS("Options for writing to a texture");

        CTOR(texture_write_desc_ctor);

        texture_write_desc_mip_offset = MVAR("int", "mip", false);
        DOC_VAR("Which mip level to write to. Default is 0 (base level)");

        texture_write_desc_offset_x_offset = MVAR("int", "x", false);
        DOC_VAR("X offset of write region. Default 0");

        texture_write_desc_offset_y_offset = MVAR("int", "y", false);
        DOC_VAR("Y offset of write region. Default 0");

        texture_write_desc_offset_z_offset = MVAR("int", "z", false);
        DOC_VAR("Z offset of write region. Default 0");

        texture_write_desc_width_offset = MVAR("int", "width", false);
        DOC_VAR("Width of write region. Default 0");

        texture_write_desc_height_offset = MVAR("int", "height", false);
        DOC_VAR("Height of write region. Default 0");

        texture_write_desc_depth_offset = MVAR("int", "depth", false);
        DOC_VAR("Depth of write region. Default 0");

        END_CLASS();
    };

    { // TextureLocation
        BEGIN_CLASS("TextureLocation", "Object");
        DOC_CLASS(
          "Describes a specific location/offset into a texture. Used for copying "
          "to/from textures");

        CTOR(texture_location_ctor);

        texture_location_mip_offset = MVAR("int", "mip", false);
        DOC_VAR("Mip level of the texture location");

        texture_location_origin_x_offset = MVAR("int", "x", false);
        DOC_VAR("X offset of the location");

        texture_location_origin_y_offset = MVAR("int", "y", false);
        DOC_VAR("Y offset of the location");

        texture_location_origin_z_offset = MVAR("int", "z", false);
        DOC_VAR("Z offset of the location");

        END_CLASS();
    }

    { // TextureLoadDesc
        BEGIN_CLASS("TextureLoadDesc", "Object");
        DOC_CLASS("Options for loading a texture from a file");

        CTOR(texture_load_desc_ctor);

        texture_load_desc_flip_y_offset = MVAR("int", "flip_y", false);
        DOC_VAR("Flip the image vertically before loading. Default false");

        texture_load_desc_gen_mips_offset = MVAR("int", "gen_mips", false);
        DOC_VAR("Generate mipmaps for the texture. Default true");

        END_CLASS();
    }

    // Texture
    {
        BEGIN_CLASS(SG_CKNames[SG_COMPONENT_TEXTURE], SG_CKNames[SG_COMPONENT_BASE]);
        DOC_CLASS(
          "Texture class. Immutable, meaning properties (e.g. format, dimension, size, "
          "mip levels) "
          "cannot be changed after creation. You can, however, write data to the "
          "texture");
        ADD_EX("deep/game_of_life.ck");
        ADD_EX("deep/audio_donut.ck");
        ADD_EX("deep/snowstorm.ck");
        ADD_EX("basic/skybox.ck");
        ADD_EX("deep/webcam_echo.ck");
        ADD_EX("basic/texture_read.ck");

        // svars ---------------
        static t_CKINT texture_usage_copy_src        = WGPUTextureUsage_CopySrc;
        static t_CKINT texture_usage_copy_dst        = WGPUTextureUsage_CopyDst;
        static t_CKINT texture_usage_texture_binding = WGPUTextureUsage_TextureBinding;
        static t_CKINT texture_usage_storage_binding = WGPUTextureUsage_StorageBinding;
        static t_CKINT texture_usage_render_attachment
          = WGPUTextureUsage_RenderAttachment;
        static t_CKINT texture_usage_all = WGPUTextureUsage_All;
        SVAR("int", "Usage_CopySrc", &texture_usage_copy_src);
        DOC_VAR(
          "Texture usage flag: can be used as a source for copy/write operations");
        SVAR("int", "Usage_CopyDst", &texture_usage_copy_dst);
        DOC_VAR(
          "Texture usage flag: can be used destination for copy/write operations");
        SVAR("int", "Usage_TextureBinding", &texture_usage_texture_binding);
        DOC_VAR("Texture usage flag: texture can be bound to a shader");
        SVAR("int", "Usage_StorageBinding", &texture_usage_storage_binding);
        DOC_VAR(
          "Texture usage flag: texture can be bound as a storage texture to a shader");
        SVAR("int", "Usage_RenderAttachment", &texture_usage_render_attachment);
        DOC_VAR(
          "Texture usage flag: texture can be used as a render attachment, i.e. "
          "written to by a render pass");
        SVAR("int", "Usage_All", &texture_usage_all);
        DOC_VAR("Texture usage flag: all usages enabled");

        // 1D textures currently unsupported
        // static t_CKINT texture_dimension_1d = WGPUTextureDimension_1D;
        static t_CKINT texture_dimension_2d = WGPUTextureDimension_2D;
        // 3D textures currently unsupported
        // static t_CKINT texture_dimension_3d = WGPUTextureDimension_3D;
        // SVAR("int", "Dimension_1D", &texture_dimension_1d);
        SVAR("int", "Dimension_2D", &texture_dimension_2d);
        // SVAR("int", "Dimension_3D", &texture_dimension_3d);

        static t_CKINT texture_format_rgba8unorm  = WGPUTextureFormat_RGBA8Unorm;
        static t_CKINT texture_format_rgba16float = WGPUTextureFormat_RGBA16Float;
        static t_CKINT texture_format_rgba32float = WGPUTextureFormat_RGBA32Float;
        static t_CKINT texture_format_r32float    = WGPUTextureFormat_R32Float;
        SVAR("int", "Format_RGBA8Unorm", &texture_format_rgba8unorm);
        SVAR("int", "Format_RGBA16Float", &texture_format_rgba16float); // not
        // supported currently
        SVAR("int", "Format_RGBA32Float", &texture_format_rgba32float);
        SVAR("int", "Format_R32Float", &texture_format_r32float);
        // SVAR("int", "Format_Depth24PlusStencil8",
        // &texture_format_depth24plusstencil8);

        // sfun ------------------------------------------------------------------

        SFUN(texture_load_2d_file, SG_CKNames[SG_COMPONENT_TEXTURE], "load");
        ARG("string", "filepath");
        DOC_FUNC("Load a 2D texture from a file");

        SFUN(texture_load_2d_raw, SG_CKNames[SG_COMPONENT_TEXTURE], "load");
        ARG("int[]", "binary_data");
        DOC_FUNC(
          "Load a 2D texture from raw data. Assumes the texture is 8bits per channel, "
          "RGBA8Unorm format. The raw data is passed as an array of chuck integers. "
          "These integers are casted to be 32-bit, and then loaded from memory as a "
          "raw byte buffer. This method is intended for including sprites in examples, "
          "e.g. so that png data can be encoded directly in the chuck file. If trying "
          "to write generated data to a texture, prefer using Texture.write(...)");

        SFUN(texture_load_2d_file_with_params, SG_CKNames[SG_COMPONENT_TEXTURE],
             "load");
        ARG("string", "filepath");
        ARG("TextureLoadDesc", "load_desc");
        DOC_FUNC("Load a 2D texture from a file with additional parameters");

        SFUN(texture_load_cubemap, SG_CKNames[SG_COMPONENT_TEXTURE], "load");
        ARG("string", "right");
        ARG("string", "left");
        ARG("string", "top");
        ARG("string", "bottom");
        ARG("string", "back");
        ARG("string", "front");
        DOC_FUNC("Load a cubemap texture from 6 filepaths, one for each face");

        SFUN(texture_copy_texture_to_texture, "void", "copy");
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "dst_texture");
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "src_texture");
        DOC_FUNC("Copy the entire src texture to the dst texture at mip level 0");

        SFUN(texture_copy_texture_to_texture_with_desc, "void", "copy");
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "dst_texture");
        ARG("TextureLocation", "dst_location");
        ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "src_texture");
        ARG("TextureLocation", "src_location");
        ARG("int", "size_x");
        ARG("int", "size_y");
        ARG("int", "size_z");
        DOC_FUNC(
          "Copy a region of the src texture to a location in the dst texture. The size "
          "parameter is floored to integers and specifies the 3D dimensions of the "
          "region to copy");

        // mfun ------------------------------------------------------------------

        CTOR(texture_ctor);
        // TODO: add destructor; also within dtor remember to release texture_read_event

        CTOR(texture_ctor_with_desc);
        ARG("TextureDesc", "texture_desc");

        MFUN(texture_write, "void", "write");
        ARG("float[]", "pixel_data");
        DOC_FUNC(
          "Convenience function for writing into a texture. Assumes pixel_data is "
          "being written into the texture origin (0,0,0) with a region equal to the "
          "full texture dimensions (width, height, depth) at mip level 0");

        MFUN(texture_write_with_desc, "void", "write");
        ARG("float[]", "pixel_data");
        ARG("TextureWriteDesc", "write_desc");
        DOC_FUNC(
          "Write pixel data to an arbitrary texture region. The input float data is "
          "automatically converted based on the texture format");

        MFUN(texture_write_external_ptr, "void", "write");
        ARG("int", "ptr");
        DOC_FUNC(
          "Write pixel data to this texture by casting `ptr` to a byte pointer and "
          "reading directly from memory. DANGEROUS. This is a hacky way to get "
          "generated texture data from another chugin. Assumes `ptr` actually points "
          "to a buffer of data large enough and in the correct format for this "
          "texture");

        MFUN(texture_get_format, "int", "format");
        DOC_FUNC(
          "Get the texture format (immutable). Returns a value from the "
          "Texture.Format_XXXXX enum, e.g. Texture.Format_RGBA8Unorm");

        MFUN(texture_get_dimension, "int", "dimension");
        DOC_FUNC(
          "Get the texture dimension (immutable). Returns a value from the "
          "Texture.Dimension_XXXXX enum, e.g. Texture.Dimension_2D");

        MFUN(texture_get_width, "int", "width");
        DOC_FUNC("Get the texture width (immutable)");

        MFUN(texture_get_height, "int", "height");
        DOC_FUNC("Get the texture height (immutable)");

        MFUN(texture_get_depth, "int", "depth");
        DOC_FUNC(
          "Get the texture depth (immutable). For a 2D texture, depth corresponds to "
          "the number of array layers (e.g. depth=6 for a cubemap)");

        MFUN(texture_get_usage, "int", "usage");
        DOC_FUNC(
          "Get the texture usage flags (immutable). Returns a bitmask of usage flgas "
          "from the Texture.Usage_XXXXX enum e.g. Texture.Usage_TextureBinding | "
          "Texture.Usage_RenderAttachment. By default, textures are created with ALL "
          "usages enabled");

        MFUN(texture_get_mips, "int", "mips");
        DOC_FUNC(
          "Get whether this textue has a full mip chain. Returns 1 for true, 0 for "
          "false");

        MFUN(texture_get_resizable, "int", "resizable");
        DOC_FUNC(
          "Get whether this texture is auto-resizing (see TextureDesc.resizable)");

        MFUN(texture_get_width_ratio, "float", "widthRatio");
        DOC_FUNC("Get the width ratio of this texture (see TextureDesc.resizable)");

        MFUN(texture_get_height_ratio, "float", "heightRatio");
        DOC_FUNC("Get the height ratio of this texture (see TextureDesc.resizable)");

        MFUN(texture_read_to_cpu, "Event", "read");
        DOC_FUNC(
          "Initializes an async readback of texture data at mip level 0 from GPU to "
          "CPU. This function returns an Event object which can be waited on until the "
          "data is ready. Access the newly acquired data via `Texture.data()`. "
          "Note that it can take several frames for the data to read back to chuck.");

        MFUN(texture_get_data, "float[]", "data");
        DOC_FUNC(
          "Get the most recently read texture data. This function should be called "
          "after waiting on the Event object returned by `Texture.read()`");

        END_CLASS();
    }

    ulib_texture_createDefaults(QUERY->ck_api(QUERY));
}

// TextureSampler ------------------------------------------------------------------

Chuck_Object* ulib_texture_ckobj_from_sampler(SG_Sampler sampler, bool add_ref,
                                              Chuck_VM_Shred* shred)
{
    CK_DL_API API = g_chuglAPI;

    Chuck_Object* ckobj = chugin_createCkObj("TextureSampler", add_ref, shred);
    OBJ_MEMBER_INT(ckobj, sampler_offset_wrapU)     = sampler.wrapU;
    OBJ_MEMBER_INT(ckobj, sampler_offset_wrapV)     = sampler.wrapV;
    OBJ_MEMBER_INT(ckobj, sampler_offset_wrapW)     = sampler.wrapW;
    OBJ_MEMBER_INT(ckobj, sampler_offset_filterMin) = sampler.filterMin;
    OBJ_MEMBER_INT(ckobj, sampler_offset_filterMag) = sampler.filterMag;
    OBJ_MEMBER_INT(ckobj, sampler_offset_filterMip) = sampler.filterMip;

    return ckobj;
}

CK_DLL_CTOR(sampler_ctor)
{
    // default to repeat wrapping and linear filtering
    OBJ_MEMBER_INT(SELF, sampler_offset_wrapU)     = SG_SAMPLER_WRAP_REPEAT;
    OBJ_MEMBER_INT(SELF, sampler_offset_wrapV)     = SG_SAMPLER_WRAP_REPEAT;
    OBJ_MEMBER_INT(SELF, sampler_offset_wrapW)     = SG_SAMPLER_WRAP_REPEAT;
    OBJ_MEMBER_INT(SELF, sampler_offset_filterMin) = SG_SAMPLER_FILTER_LINEAR;
    OBJ_MEMBER_INT(SELF, sampler_offset_filterMag) = SG_SAMPLER_FILTER_LINEAR;
    OBJ_MEMBER_INT(SELF, sampler_offset_filterMip) = SG_SAMPLER_FILTER_LINEAR;
}

CK_DLL_SFUN(sampler_nearest)
{
    Chuck_Object* ckobj = chugin_createCkObj("TextureSampler", false, SHRED);
    OBJ_MEMBER_INT(ckobj, sampler_offset_wrapU)     = SG_SAMPLER_WRAP_REPEAT;
    OBJ_MEMBER_INT(ckobj, sampler_offset_wrapV)     = SG_SAMPLER_WRAP_REPEAT;
    OBJ_MEMBER_INT(ckobj, sampler_offset_wrapW)     = SG_SAMPLER_WRAP_REPEAT;
    OBJ_MEMBER_INT(ckobj, sampler_offset_filterMin) = SG_SAMPLER_FILTER_NEAREST;
    OBJ_MEMBER_INT(ckobj, sampler_offset_filterMag) = SG_SAMPLER_FILTER_NEAREST;
    OBJ_MEMBER_INT(ckobj, sampler_offset_filterMip) = SG_SAMPLER_FILTER_NEAREST;

    RETURN->v_object = ckobj;
}

CK_DLL_SFUN(sampler_linear)
{
    Chuck_Object* ckobj = chugin_createCkObj("TextureSampler", false, SHRED);
    OBJ_MEMBER_INT(ckobj, sampler_offset_wrapU)     = SG_SAMPLER_WRAP_REPEAT;
    OBJ_MEMBER_INT(ckobj, sampler_offset_wrapV)     = SG_SAMPLER_WRAP_REPEAT;
    OBJ_MEMBER_INT(ckobj, sampler_offset_wrapW)     = SG_SAMPLER_WRAP_REPEAT;
    OBJ_MEMBER_INT(ckobj, sampler_offset_filterMin) = SG_SAMPLER_FILTER_LINEAR;
    OBJ_MEMBER_INT(ckobj, sampler_offset_filterMag) = SG_SAMPLER_FILTER_LINEAR;
    OBJ_MEMBER_INT(ckobj, sampler_offset_filterMip) = SG_SAMPLER_FILTER_LINEAR;

    RETURN->v_object = ckobj;
}

// TextureDesc ---------------------------------------------------------------------

CK_DLL_CTOR(texture_desc_ctor)
{
    OBJ_MEMBER_INT(SELF, texture_desc_format_offset)    = WGPUTextureFormat_RGBA8Unorm;
    OBJ_MEMBER_INT(SELF, texture_desc_dimension_offset) = WGPUTextureDimension_2D;
    OBJ_MEMBER_INT(SELF, texture_desc_width_offset)     = 1;
    OBJ_MEMBER_INT(SELF, texture_desc_height_offset)    = 1;
    OBJ_MEMBER_INT(SELF, texture_desc_depth_offset)     = 1;
    OBJ_MEMBER_INT(SELF, texture_desc_usage_offset)     = WGPUTextureUsage_All;
    // OBJ_MEMBER_INT(SELF, texture_desc_samples_offset) = 1;
    OBJ_MEMBER_INT(SELF, texture_desc_mips_offset) = 1;

    OBJ_MEMBER_INT(SELF, texture_desc_resizable_offset)      = 0;
    OBJ_MEMBER_FLOAT(SELF, texture_desc_width_ratio_offset)  = 1.0;
    OBJ_MEMBER_FLOAT(SELF, texture_desc_height_ratio_offset) = 1.0;
}

static SG_TextureDesc ulib_texture_textureDescFromCkobj(Chuck_Object* ckobj)
{
    CK_DL_API API = g_chuglAPI;

    SG_TextureDesc desc = {};
    desc.format = (WGPUTextureFormat)OBJ_MEMBER_INT(ckobj, texture_desc_format_offset);
    desc.dimension
      = (WGPUTextureDimension)OBJ_MEMBER_INT(ckobj, texture_desc_dimension_offset);
    desc.width  = OBJ_MEMBER_INT(ckobj, texture_desc_width_offset);
    desc.height = OBJ_MEMBER_INT(ckobj, texture_desc_height_offset);
    desc.depth  = OBJ_MEMBER_INT(ckobj, texture_desc_depth_offset);
    desc.usage  = OBJ_MEMBER_INT(ckobj, texture_desc_usage_offset);
    // desc.samples        = OBJ_MEMBER_INT(ckobj, texture_desc_samples_offset);
    desc.gen_mips = OBJ_MEMBER_INT(ckobj, texture_desc_mips_offset) ? true : false;

    desc.resize_mode = OBJ_MEMBER_INT(ckobj, texture_desc_resizable_offset) ?
                         SG_TextureResizeMode_Ratio :
                         SG_TextureResizeMode_Fixed;
    desc.width_ratio = MAX(0, OBJ_MEMBER_FLOAT(ckobj, texture_desc_width_ratio_offset));
    desc.height_ratio
      = MAX(0, OBJ_MEMBER_FLOAT(ckobj, texture_desc_height_ratio_offset));

    // validation happens at final layer SG_CreateTexture
    return desc;
}

// TextureWriteDesc -----------------------------------------------------------------

CK_DLL_CTOR(texture_write_desc_ctor)
{
    OBJ_MEMBER_INT(SELF, texture_write_desc_mip_offset)      = 0;
    OBJ_MEMBER_INT(SELF, texture_write_desc_offset_x_offset) = 0;
    OBJ_MEMBER_INT(SELF, texture_write_desc_offset_y_offset) = 0;
    OBJ_MEMBER_INT(SELF, texture_write_desc_offset_z_offset) = 0;
    OBJ_MEMBER_INT(SELF, texture_write_desc_width_offset)    = 1;
    OBJ_MEMBER_INT(SELF, texture_write_desc_height_offset)   = 1;
    OBJ_MEMBER_INT(SELF, texture_write_desc_depth_offset)    = 1;
}

static SG_TextureWriteDesc ulib_texture_textureWriteDescFromCkobj(Chuck_Object* ckobj)
{
    CK_DL_API API = g_chuglAPI;

    SG_TextureWriteDesc desc = {};
    desc.mip                 = OBJ_MEMBER_INT(ckobj, texture_write_desc_mip_offset);
    desc.offset_x = OBJ_MEMBER_INT(ckobj, texture_write_desc_offset_x_offset);
    desc.offset_y = OBJ_MEMBER_INT(ckobj, texture_write_desc_offset_y_offset);
    desc.offset_z = OBJ_MEMBER_INT(ckobj, texture_write_desc_offset_z_offset);
    desc.width    = OBJ_MEMBER_INT(ckobj, texture_write_desc_width_offset);
    desc.height   = OBJ_MEMBER_INT(ckobj, texture_write_desc_height_offset);
    desc.depth    = OBJ_MEMBER_INT(ckobj, texture_write_desc_depth_offset);

    // validation happens at final write

    return desc;
}

// TextureRegion -----------------------------------------------------------------

CK_DLL_CTOR(texture_location_ctor)
{
    OBJ_MEMBER_INT(SELF, texture_location_mip_offset)      = 0;
    OBJ_MEMBER_INT(SELF, texture_location_origin_x_offset) = 0;
    OBJ_MEMBER_INT(SELF, texture_location_origin_y_offset) = 0;
    OBJ_MEMBER_INT(SELF, texture_location_origin_z_offset) = 0;
}

static SG_TextureLocation ulib_texture_textureLocationFromCkobj(Chuck_Object* ckobj)
{
    if (!ckobj) return {};

    CK_DL_API API = g_chuglAPI;

    SG_TextureLocation location = {};
    location.mip                = OBJ_MEMBER_INT(ckobj, texture_location_mip_offset);
    location.origin_x = OBJ_MEMBER_INT(ckobj, texture_location_origin_x_offset);
    location.origin_y = OBJ_MEMBER_INT(ckobj, texture_location_origin_y_offset);
    location.origin_z = OBJ_MEMBER_INT(ckobj, texture_location_origin_z_offset);

    return location;
}

// TextureLoadDesc -----------------------------------------------------------------

CK_DLL_CTOR(texture_load_desc_ctor)
{
    OBJ_MEMBER_INT(SELF, texture_load_desc_flip_y_offset)   = false;
    OBJ_MEMBER_INT(SELF, texture_load_desc_gen_mips_offset) = true;
}

static SG_TextureLoadDesc ulib_texture_textureLoadDescFromCkobj(Chuck_Object* ckobj)
{
    CK_DL_API API = g_chuglAPI;

    SG_TextureLoadDesc desc = {};
    desc.flip_y             = OBJ_MEMBER_INT(ckobj, texture_load_desc_flip_y_offset);
    desc.gen_mips           = OBJ_MEMBER_INT(ckobj, texture_load_desc_gen_mips_offset);

    return desc;
}

// Texture -----------------------------------------------------------------

// create default pixel textures and samplers
void ulib_texture_createDefaults(CK_DL_API API)
{
    SG_TextureDesc texture_binding_desc = {};
    texture_binding_desc.usage
      = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;

    SG_TextureWriteDesc texture_write_desc = {};

    // white pixel
    {
        SG_Texture* tex
          = SG_CreateTexture(&texture_binding_desc, NULL, NULL, true, "White Pixel");
        // upload pixel data
        CQ_PushCommand_TextureWrite(tex, &texture_write_desc,
                                    g_builtin_ckobjs.white_pixel_data, API);
        // set global
        g_builtin_textures.white_pixel_id = tex->id;
    }
    //  default render texture (hdr)
    {
        SG_TextureDesc render_texture_desc = {};
        render_texture_desc.usage          = WGPUTextureUsage_RenderAttachment
                                    | WGPUTextureUsage_TextureBinding
                                    | WGPUTextureUsage_StorageBinding;
        render_texture_desc.format = WGPUTextureFormat_RGBA16Float;

        // make scale with window dimensions
        render_texture_desc.resize_mode  = SG_TextureResizeMode_Ratio;
        render_texture_desc.width_ratio  = 1.0f;
        render_texture_desc.height_ratio = 1.0f;

        // set global
        SG_Texture* render_tex = SG_CreateTexture(&render_texture_desc, NULL, NULL,
                                                  true, "Default Render Texture");
        g_builtin_textures.default_render_texture_id = render_tex->id;
    }

    { // black pixel
        SG_Texture* tex
          = SG_CreateTexture(&texture_binding_desc, NULL, NULL, true, "Black Pixel");
        // upload pixel data
        CQ_PushCommand_TextureWrite(tex, &texture_write_desc,
                                    g_builtin_ckobjs.black_pixel_data, API);
        // set global
        g_builtin_textures.black_pixel_id = tex->id;
    }

    { // magenta pixel
        SG_Texture* tex
          = SG_CreateTexture(&texture_binding_desc, NULL, NULL, true, "Magenta Pixel");
        // upload pixel data
        CQ_PushCommand_TextureWrite(tex, &texture_write_desc,
                                    g_builtin_ckobjs.magenta_pixel_data, API);
        // set global
        g_builtin_textures.magenta_pixel_id = tex->id;
    }

    { // default normal map
        SG_Texture* tex = SG_CreateTexture(&texture_binding_desc, NULL, NULL, true,
                                           "Default Normal Map Pixel");
        // upload pixel data
        CQ_PushCommand_TextureWrite(tex, &texture_write_desc,
                                    g_builtin_ckobjs.normal_pixel_data, API);
        // set global
        g_builtin_textures.normal_pixel_id = tex->id;
    }

    { // default cube map
        SG_TextureDesc cubemap_desc = {};
        cubemap_desc.depth          = 6;     // 6 faces
        cubemap_desc.gen_mips       = false; // no mips for cubemap

        SG_Texture* tex
          = SG_CreateTexture(&cubemap_desc, NULL, NULL, true, "Default Cubemap");

        // upload pixel data
        SG_TextureWriteDesc cubemap_write_desc = {};
        cubemap_write_desc.depth               = 6; // 6 faces
        CQ_PushCommand_TextureWrite(tex, &cubemap_write_desc,
                                    g_builtin_ckobjs.default_cube_map_data, API);

        // set global
        g_builtin_textures.default_cubemap_id = tex->id;
    }
}

CK_DLL_CTOR(texture_ctor)
{
    SG_TextureDesc desc = {};
    SG_CreateTexture(&desc, SELF, SHRED, false);
}

CK_DLL_CTOR(texture_ctor_with_desc)
{
    SG_TextureDesc desc = ulib_texture_textureDescFromCkobj(GET_NEXT_OBJECT(ARGS));
    SG_CreateTexture(&desc, SELF, SHRED, false);
}

CK_DLL_MFUN(texture_get_format)
{
    RETURN->v_int = GET_TEXTURE(SELF)->desc.format;
}

CK_DLL_MFUN(texture_get_dimension)
{
    RETURN->v_int = GET_TEXTURE(SELF)->desc.dimension;
}

CK_DLL_MFUN(texture_get_width)
{
    RETURN->v_int = GET_TEXTURE(SELF)->desc.width;
}

CK_DLL_MFUN(texture_get_height)
{
    RETURN->v_int = GET_TEXTURE(SELF)->desc.height;
}

CK_DLL_MFUN(texture_get_depth)
{
    RETURN->v_int = GET_TEXTURE(SELF)->desc.depth;
}

CK_DLL_MFUN(texture_get_usage)
{
    RETURN->v_int = GET_TEXTURE(SELF)->desc.usage;
}

CK_DLL_MFUN(texture_get_mips)
{
    RETURN->v_int = GET_TEXTURE(SELF)->desc.gen_mips;
}

CK_DLL_MFUN(texture_get_resizable)
{
    RETURN->v_int = GET_TEXTURE(SELF)->desc.resize_mode;
}

CK_DLL_MFUN(texture_get_width_ratio)
{
    RETURN->v_int = GET_TEXTURE(SELF)->desc.width_ratio;
}

CK_DLL_MFUN(texture_get_height_ratio)
{
    RETURN->v_int = GET_TEXTURE(SELF)->desc.height_ratio;
}

static void ulib_texture_write(SG_Texture* tex, Chuck_ArrayFloat* ck_arr,
                               SG_TextureWriteDesc* desc, Chuck_VM_Shred* SHRED)
{
    CK_DL_API API = g_chuglAPI;

    int num_texels   = desc->width * desc->height * desc->depth;
    int expected_len = num_texels * SG_Texture_numComponentsPerTexel(tex->desc.format);

    { // validation
        char err_msg[256] = {};
        // check offset within image bounds
        if (desc->offset_x + desc->width > tex->desc.width
            || desc->offset_y + desc->height > tex->desc.height
            || desc->offset_z + desc->depth > tex->desc.depth) {
            snprintf(err_msg, sizeof(err_msg),
                     "Texture write region out of bounds. Texture dimensions [%d, %d, "
                     "%d]. Write offsets [%d, %d, %d]. Write region size [%d, %d, %d]",
                     tex->desc.width, tex->desc.height, tex->desc.depth, desc->offset_x,
                     desc->offset_y, desc->offset_z, desc->width, desc->height,
                     desc->depth);
            CK_THROW("TextureWriteOutOfBounds", err_msg, SHRED);
        }

        // check mip level valid (only checking for fixed size textures because how do
        // we know the size of a resizable one?)
        if (tex->desc.resize_mode == SG_TextureResizeMode_Fixed) {
            int max_mips = G_mipLevels(tex->desc.width, tex->desc.height);
            if (desc->mip >= max_mips) {
                snprintf(err_msg, sizeof(err_msg),
                         "Invalid mip level. Texture has %d mips, but tried to "
                         "write to mip level %d",
                         max_mips, desc->mip);
                CK_THROW("TextureWriteInvalidMip", err_msg, SHRED);
            }
        }

        // check ck_array
        int ck_arr_len = API->object->array_float_size(ck_arr);
        if (ck_arr_len < expected_len) {
            snprintf(
              err_msg, sizeof(err_msg),
              "Incorrect number of components in pixel data. Expected %d, got %d",
              expected_len, ck_arr_len);
            CK_THROW("TextureWriteInvalidPixelData", err_msg, SHRED);
        }
    }

    // convert ck array into byte buffer based on texture format
    CQ_PushCommand_TextureWrite(tex, desc, ck_arr, API);
}

CK_DLL_MFUN(texture_write)
{
    SG_Texture* tex          = GET_TEXTURE(SELF);
    SG_TextureWriteDesc desc = {};
    desc.width               = tex->desc.width;
    desc.height              = tex->desc.height;
    desc.depth               = tex->desc.depth;

    ulib_texture_write(tex, GET_NEXT_FLOAT_ARRAY(ARGS), &desc, SHRED);
}

CK_DLL_MFUN(texture_write_with_desc)
{
    SG_Texture* tex          = GET_TEXTURE(SELF);
    Chuck_ArrayFloat* ck_arr = GET_NEXT_FLOAT_ARRAY(ARGS);
    SG_TextureWriteDesc desc
      = ulib_texture_textureWriteDescFromCkobj(GET_NEXT_OBJECT(ARGS));

    ulib_texture_write(tex, ck_arr, &desc, SHRED);
}

CK_DLL_MFUN(texture_write_external_ptr)
{
    SG_Texture* tex          = GET_TEXTURE(SELF);
    SG_TextureWriteDesc desc = {};
    desc.width               = tex->desc.width;
    desc.height              = tex->desc.height;
    desc.depth               = tex->desc.depth;

    void* external_ptr = (void*)GET_NEXT_INT(ARGS);

#ifdef CHUGL_DEBUG
    // hexDump("texture write external ptr", external_ptr, 32);
#endif

    CQ_PushCommand_TextureWriteExternalPtr(tex, &desc, external_ptr);
}

SG_Texture* ulib_texture_load(const char* filepath, SG_TextureLoadDesc* load_desc,
                              Chuck_VM_Shred* shred)
{
    int width, height, num_components;
    if (!stbi_info(filepath, &width, &height, &num_components)) {
        log_warn("Could not load texture file '%s'", filepath);
        log_warn(" |- Reason: %s", stbi_failure_reason());
        log_warn(" |- Defaulting to magenta texture");

        // on failure return magenta texture
        return SG_GetTexture(g_builtin_textures.magenta_pixel_id);
    }

    SG_TextureDesc desc = {};
    desc.width          = width;
    desc.height         = height;
    desc.dimension      = WGPUTextureDimension_2D;
    desc.format         = WGPUTextureFormat_RGBA8Unorm;
    desc.usage          = WGPUTextureUsage_All;
    desc.gen_mips       = load_desc->gen_mips ? true : false;

    SG_Texture* tex
      = SG_CreateTexture(&desc, NULL, shred, false, File_basename(filepath));

    CQ_PushCommand_TextureFromFile(tex, filepath, load_desc);

    return tex;
}

SG_Texture* ulib_texture_load(unsigned char* buffer, int buffer_len,
                              SG_TextureLoadDesc* load_desc, Chuck_VM_Shred* shred)
{
    int width, height, num_components;
    if (!stbi_info_from_memory(buffer, buffer_len, &width, &height, &num_components)) {
        log_warn("Could not load texture file from raw data");
        log_warn(" |- Reason: %s", stbi_failure_reason());
        log_warn(" |- Defaulting to magenta texture");

        // on failure return magenta texture
        return SG_GetTexture(g_builtin_textures.magenta_pixel_id);
    } else {
        log_trace("Preparing %dx%d texture from raw data", width, height);
    }

    SG_TextureDesc desc = {};
    desc.width          = width;
    desc.height         = height;
    desc.dimension      = WGPUTextureDimension_2D;
    desc.format         = WGPUTextureFormat_RGBA8Unorm;
    desc.usage          = WGPUTextureUsage_All;
    desc.gen_mips       = load_desc->gen_mips ? true : false;

    SG_Texture* tex = SG_CreateTexture(&desc, NULL, shred, false, "Raw Data Texture");

    CQ_PushCommand_TextureFromRawData(tex, buffer, buffer_len, load_desc);

    return tex;
}

CK_DLL_MFUN(texture_read_to_cpu)
{
    // the graphics thread won't be triggered until nextFrame() is called
    // so we don't need to do this in a "waitingOn" ck event callback

    SG_Texture* tex = GET_TEXTURE(SELF);
    CQ_PushCommand_CopyTextureToCPU(tex);
    RETURN->v_object = (Chuck_Object*)tex->texture_read_event;
}

CK_DLL_MFUN(texture_get_data)
{
    SG_Texture* tex = GET_TEXTURE(SELF);

    if (API->object->array_float_size(tex->texture_data) == 0) {
        log_warn("Texture[id=%d, name=%s] data empty", tex->id, tex->name);
        log_warn(" |- Did you mean to read the texture data from GPU first?");
        log_warn(" |- e.g. `tex.read() => now;`");
    }

    RETURN->v_object = (Chuck_Object*)tex->texture_data;
}

CK_DLL_SFUN(texture_load_2d_file)
{
    SG_TextureLoadDesc load_desc = {};
    SG_Texture* tex
      = ulib_texture_load(API->object->str(GET_NEXT_STRING(ARGS)), &load_desc, SHRED);
    RETURN->v_object = tex ? tex->ckobj : NULL;
}

CK_DLL_SFUN(texture_load_2d_raw)
{
    SG_TextureLoadDesc load_desc = {};

    Chuck_ArrayInt* ck_arr = GET_NEXT_INT_ARRAY(ARGS);
    t_CKINT ck_arr_len     = API->object->array_int_size(ck_arr);

    u32* arena_data = ARENA_PUSH_COUNT(&audio_frame_arena, u32, ck_arr_len);
    for (int i = 0; i < ck_arr_len; i++)
        arena_data[i] = (u32)API->object->array_int_get_idx(ck_arr, i);

    SG_Texture* tex
      = ulib_texture_load((u8*)arena_data, ck_arr_len * 4, &load_desc, SHRED);
    RETURN->v_object = tex ? tex->ckobj : NULL;
}

CK_DLL_SFUN(texture_load_2d_file_with_params)
{
    const char* filepath = API->object->str(GET_NEXT_STRING(ARGS));
    SG_TextureLoadDesc load_desc
      = ulib_texture_textureLoadDescFromCkobj(GET_NEXT_OBJECT(ARGS));

    SG_Texture* tex = ulib_texture_load(filepath, &load_desc, SHRED);

    RETURN->v_object = tex ? tex->ckobj : NULL;
}

// load cubemap
SG_Texture* ulib_texture_load_cubemap(const char* right_face, const char* left_face,
                                      const char* top_face, const char* bottom_face,
                                      const char* back_face, const char* front_face,
                                      SG_TextureLoadDesc* load_desc,
                                      Chuck_VM_Shred* shred)
{
    const char* filepaths[6]
      = { right_face, left_face, top_face, bottom_face, back_face, front_face };

    int cubemap_width{}, cubemap_height{};
    for (int i = 0; i < 6; i++) {
        int width{}, height{};
        if (!stbi_info(filepaths[i], &width, &height, NULL)) {
            log_warn("Could not load texture file '%s'", filepaths[i]);
            log_warn(" |- Reason: %s", stbi_failure_reason());
            log_warn(" |- Defaulting to magenta texture");

            // on failure return default cubemap
            return SG_GetTexture(g_builtin_textures.default_cubemap_id);
        }

        if (cubemap_width == 0) cubemap_width = width;
        if (cubemap_height == 0) cubemap_height = height;

        // check if all faces have same dimensions
        if (cubemap_width != width || cubemap_height != height) {
            log_warn(
              "Cubemap faces have different dimensions %dx%d vs %dx%d on face %d",
              cubemap_width, cubemap_height, width, height, i);
            log_warn(" |- Defaulting to magenta texture");

            // on failure return default cubemap
            return SG_GetTexture(g_builtin_textures.default_cubemap_id);
        }
    }

    SG_TextureDesc desc = {};
    desc.width          = cubemap_width;
    desc.height         = cubemap_height;
    desc.depth          = 6; // 6 faces
    desc.dimension      = WGPUTextureDimension_2D;
    desc.format         = WGPUTextureFormat_RGBA8Unorm;
    desc.usage          = WGPUTextureUsage_All;
    desc.gen_mips       = false;

    SG_Texture* tex = SG_CreateTexture(&desc, NULL, shred, false);

    CQ_PushCommand_CubemapTextureFromFile(tex, load_desc, right_face, left_face,
                                          top_face, bottom_face, back_face, front_face);

    return tex;
}

CK_DLL_SFUN(texture_load_cubemap)
{
    SG_TextureLoadDesc load_desc = {};
    load_desc.gen_mips           = false; // don't generate mips for cubemap
    const char* right_face       = API->object->str(GET_NEXT_STRING(ARGS));
    const char* left_face        = API->object->str(GET_NEXT_STRING(ARGS));
    const char* top_face         = API->object->str(GET_NEXT_STRING(ARGS));
    const char* bottom_face      = API->object->str(GET_NEXT_STRING(ARGS));
    const char* back_face        = API->object->str(GET_NEXT_STRING(ARGS));
    const char* front_face       = API->object->str(GET_NEXT_STRING(ARGS));

    RETURN->v_object
      = ulib_texture_load_cubemap(right_face, left_face, top_face, bottom_face,
                                  back_face, front_face, &load_desc, SHRED)
          ->ckobj;
}

static const char* ulib_texture_wgpuTextureFormatToString(WGPUTextureFormat format)
{
    switch (format) {
        case WGPUTextureFormat_RGBA8Unorm: return "RGBA8Unorm";
        case WGPUTextureFormat_RGBA16Float: return "RGBA16Float";
        case WGPUTextureFormat_RGBA32Float: return "RGBA32Float";
        case WGPUTextureFormat_R32Float: return "R32Float";
        default: {
            ASSERT(false); // unknown
            return "";
        }
    }
}

static void ulib_texture_copyTextureToTexture(SG_Texture* dst, SG_Texture* src,
                                              SG_TextureLocation dst_loc,
                                              SG_TextureLocation src_loc, int width,
                                              int height, int depth)
{
    { // validation
        // make sure location is within bounds of texture
        if (dst->desc.resize_mode == SG_TextureResizeMode_Fixed) {
            int dst_mips = G_mipLevels(dst->desc.width, dst->desc.height);

            if (dst_loc.origin_x > dst->desc.width
                || dst_loc.origin_y > dst->desc.height
                || dst_loc.origin_z > dst->desc.depth || dst_loc.mip >= dst_mips) {
                log_warn("Could not copy texture[%d] %s to texture[%d] %s", src->id,
                         src->name, dst->id, dst->name);
                log_warn(
                  " |- Reason: destination location [%d, %d, %d] mip %d is out of "
                  "bounds of texture with dimensions [%d, %d, %d] and %d mips",
                  dst_loc.origin_x, dst_loc.origin_y, dst_loc.origin_z, dst_loc.mip,
                  dst->desc.width, dst->desc.height, dst->desc.depth, dst_mips);
                return;
            }
        }

        if (src->desc.resize_mode == SG_TextureResizeMode_Fixed) {
            int src_mips = G_mipLevels(src->desc.width, src->desc.height);
            if (src_loc.origin_x > src->desc.width
                || src_loc.origin_y > src->desc.height
                || src_loc.origin_z > src->desc.depth || src_loc.mip >= src_mips) {
                log_warn("Could not copy texture[%d] %s to texture[%d] %s", src->id,
                         src->name, dst->id, dst->name);
                log_warn(
                  " |- Reason: source location [%d, %d, %d] mip %d is out of bounds "
                  "of texture with dimensions [%d, %d, %d] and %d mips",
                  src_loc.origin_x, src_loc.origin_y, src_loc.origin_z, src_loc.mip,
                  src->desc.width, src->desc.height, src->desc.depth, src_mips);
                return;
            }
        }

        // The two textures must be must either be the same format, or they must only
        // differ by the suffix '-srgb'.
        // TODO: after adding srgb support, change this to check for srgb suffix
        if (dst->desc.format != src->desc.format) {
            const char* dst_format_str
              = ulib_texture_wgpuTextureFormatToString(dst->desc.format);
            const char* src_format_str
              = ulib_texture_wgpuTextureFormatToString(src->desc.format);

            log_warn("Could not copy texture[%d] %s to texture[%d] %s", src->id,
                     src->name, dst->id, dst->name);
            log_warn(" |- Reason: mismatched texture formats %s != %s", dst_format_str,
                     src_format_str);
            return;
        }

        // src.texture must have a usage of GPUTextureUsage.COPY_SRC
        if ((src->desc.usage & WGPUTextureUsage_CopySrc) == 0) {
            log_warn("Could not copy texture[%d] %s to texture[%d] %s", src->id,
                     src->name, dst->id, dst->name);
            log_warn(
              " |- Reason: source texture[%d] does not have the Texture.Usage_CopySrc "
              "permission",
              src->id);
            return;
        }

        // dst.texture must have a usage of GPUTextureUsage.COPY_DST
        if ((dst->desc.usage & WGPUTextureUsage_CopyDst) == 0) {
            log_warn("Could not copy texture[%d] %s to texture[%d] %s", src->id,
                     src->name, dst->id, dst->name);
            log_warn(
              " |- Reason: destination texture[%d] does not have the "
              "Texture.Usage_CopyDst "
              "permission",
              dst->id);
            return;
        }

        // check if dst is too small
        if (dst->desc.width < width || dst->desc.height < height
            || dst->desc.depth < depth) {
            log_warn("Could not copy texture[%d] %s to texture[%d] %s", src->id,
                     src->name, dst->id, dst->name);
            log_warn(
              " |- Reason: destination texture is too small"
              " copy size [%d, %d, %d] > destination size [%d, %d, %d]",
              width, height, depth, dst->desc.width, dst->desc.height, dst->desc.depth);
            return;
        }

        // TODO after supporting compressed textures, check size is multiple of block
        // size
    }

    CQ_PushCommand_CopyTextureToTexture(dst, src, &dst_loc, &src_loc, width, height,
                                        depth);
}

CK_DLL_SFUN(texture_copy_texture_to_texture)
{
    Chuck_Object* dst_ckobj = (GET_NEXT_OBJECT(ARGS));
    Chuck_Object* src_ckobj = (GET_NEXT_OBJECT(ARGS));
    if (!dst_ckobj || !src_ckobj) {
        log_warn("Could not copy texture to texture");
        log_warn(" |- Reason: null texture objects");
        return;
    }
    SG_Texture* dst_texture = GET_TEXTURE(dst_ckobj);
    SG_Texture* src_texture = GET_TEXTURE(src_ckobj);

    SG_TextureLocation dst_location = {};
    SG_TextureLocation src_location = {};
    ulib_texture_copyTextureToTexture(
      dst_texture, src_texture, dst_location, src_location,
      MIN(src_texture->desc.width, dst_texture->desc.width),
      MIN(src_texture->desc.height, dst_texture->desc.height),
      MIN(src_texture->desc.depth, dst_texture->desc.depth));
}

CK_DLL_SFUN(texture_copy_texture_to_texture_with_desc)
{
    Chuck_Object* dst_ckobj = GET_NEXT_OBJECT(ARGS);
    SG_TextureLocation dst_loc
      = ulib_texture_textureLocationFromCkobj(GET_NEXT_OBJECT(ARGS));

    Chuck_Object* src_ckobj = GET_NEXT_OBJECT(ARGS);
    SG_TextureLocation src_loc
      = ulib_texture_textureLocationFromCkobj(GET_NEXT_OBJECT(ARGS));

    t_CKINT copy_size_x = GET_NEXT_INT(ARGS);
    t_CKINT copy_size_y = GET_NEXT_INT(ARGS);
    t_CKINT copy_size_z = GET_NEXT_INT(ARGS);

    if (!dst_ckobj || !src_ckobj) {
        log_warn("Could not copy texture to texture");
        log_warn(" |- Reason: null texture objects");
        return;
    }

    SG_Texture* dst_texture = GET_TEXTURE(dst_ckobj);
    SG_Texture* src_texture = GET_TEXTURE(src_ckobj);

    ulib_texture_copyTextureToTexture(dst_texture, src_texture, dst_loc, src_loc,
                                      copy_size_x, copy_size_y, copy_size_z);
}
