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

// TODO add ambient to scene

CK_DLL_CTOR(ulib_light_ctor);
CK_DLL_CTOR(ulib_light_ctor_with_type);

CK_DLL_MFUN(ulib_light_set_type);
CK_DLL_MFUN(ulib_light_get_type);

CK_DLL_MFUN(ulib_light_set_color);
CK_DLL_MFUN(ulib_light_get_color);

CK_DLL_MFUN(ulib_light_set_intensity);
CK_DLL_MFUN(ulib_light_get_intensity);

// shadows---------------------
CK_DLL_MFUN(ulib_light_generates_shadow_set);
CK_DLL_MFUN(ulib_light_generates_shadow_get);
CK_DLL_MFUN(ulib_light_shadow_add_mesh);
CK_DLL_MFUN(ulib_light_shadow_remove_mesh);
CK_DLL_MFUN(ulib_light_shadow_bias_set);
CK_DLL_MFUN(ulib_light_shadow_bias_get);

// ---------------------shadows

CK_DLL_CTOR(ulib_dir_light_ctor);
CK_DLL_MFUN(ulib_dir_light_shadow_size_set);
CK_DLL_MFUN(ulib_dir_light_shadow_size_get);

CK_DLL_CTOR(ulib_point_light_ctor);
CK_DLL_MFUN(ulib_point_light_get_radius);
CK_DLL_MFUN(ulib_point_light_set_radius);
CK_DLL_MFUN(ulib_point_light_get_falloff_exponent);
CK_DLL_MFUN(ulib_point_light_set_falloff_exponent);

CK_DLL_CTOR(ulib_spot_light_ctor);
CK_DLL_MFUN(ulib_spot_light_get_falloff_exponent);
CK_DLL_MFUN(ulib_spot_light_set_falloff_exponent);
CK_DLL_MFUN(ulib_spot_light_get_radius);
CK_DLL_MFUN(ulib_spot_light_set_radius);
CK_DLL_MFUN(ulib_spot_light_get_angle_max);
CK_DLL_MFUN(ulib_spot_light_set_angle_max);
CK_DLL_MFUN(ulib_spot_light_get_angle_min);
CK_DLL_MFUN(ulib_spot_light_set_angle_min);
CK_DLL_MFUN(ulib_spot_light_get_angular_falloff);
CK_DLL_MFUN(ulib_spot_light_set_angular_falloff);

#define GET_LIGHT(ckobj) SG_GetLight(OBJ_MEMBER_UINT(ckobj, component_offset_id))

SG_Light* ulib_light_create(Chuck_Object* ckobj, SG_LightType type)
{
    CK_DL_API API = g_chuglAPI;

    // execute change on audio thread side
    SG_Light* light = SG_CreateLight(ckobj);
    SG_Light::lightType(light, type);
    // save SG_ID
    OBJ_MEMBER_UINT(ckobj, component_offset_id) = light->id;

    CQ_PushCommand_LightUpdate(light);
    return light;
}

static void ulib_light_query(Chuck_DL_Query* QUERY)
{
    BEGIN_CLASS(SG_CKNames[SG_COMPONENT_LIGHT], SG_CKNames[SG_COMPONENT_TRANSFORM]);
    DOC_CLASS("Base class for all light components.");
    ADD_EX("basic/light.ck");
    ADD_EX("basic/pbr.ck");
    ADD_EX("deep/shadows.ck");

    static t_CKINT light_type_directional = SG_LightType_Directional;
    static t_CKINT light_type_point       = SG_LightType_Point;
    static t_CKINT light_type_spot        = SG_LightType_Spot;
    SVAR("int", "Directional", &light_type_directional);
    SVAR("int", "Point", &light_type_point);
    SVAR("int", "Spot", &light_type_spot);

    // -------------------------

    CTOR(ulib_light_ctor);
    DOC_FUNC("By default initializes a GDirLight");

    CTOR(ulib_light_ctor_with_type);
    ARG("int", "type");
    DOC_FUNC(
      "Initialize a specific light type (GLight.Directional, GLight.Point, "
      "GLight.Spot). Prefer using the child classes instead, "
      "GDirLight, GPointLight, etc.");

    MFUN(ulib_light_set_type, "void", "mode");
    ARG("int", "type");
    DOC_FUNC(
      "Set the light type. Use GLight.Directional, GLight.Point, or GLight.Spot");

    MFUN(ulib_light_get_type, "int", "mode");
    DOC_FUNC(
      "Get the light type. Returns a light enum: GLight.Directional, GLight.Point, or "
      "GLight.Spot");

    MFUN(ulib_light_set_color, "void", "color");
    ARG("vec3", "color");
    DOC_FUNC("Set the light color.");

    MFUN(ulib_light_get_color, "vec3", "color");
    DOC_FUNC("Get the light color.");

    MFUN(ulib_light_set_intensity, "void", "intensity");
    ARG("float", "intensity");
    DOC_FUNC(
      "Set the light intensity. Default 1.0. Use 0 to turn off the light. The "
      "intensity is multiplied by the color in final lighting calculations.");

    MFUN(ulib_light_get_intensity, "float", "intensity");
    DOC_FUNC("Get the light intensity.");

    MFUN(ulib_light_generates_shadow_set, "void", "shadow");
    ARG("int", "generate_shadows");
    DOC_FUNC("Set whether or not this light source generates shadows");

    MFUN(ulib_light_generates_shadow_get, "int", "shadow");
    DOC_FUNC("Get whether or not this light source generates shadows");

    MFUN(ulib_light_shadow_add_mesh, "void", "shadowAdd");
    ARG(SG_CKNames[SG_COMPONENT_TRANSFORM], "mesh");
    ARG("int", "add_children");
    DOC_FUNC(
      "Add shadow casters, i.e. meshes that will be rendered into this light source's "
      "shadow map. These meshes will cast a shadow onto other meshes with .shadowed() "
      "== true");

    MFUN(ulib_light_shadow_remove_mesh, "void", "shadowRemove");
    ARG(SG_CKNames[SG_COMPONENT_TRANSFORM], "mesh");
    ARG("int", "add_children");
    DOC_FUNC(
      "Remove shadow casters, i.e. remove meshes from the list of what will be "
      "rendered into this light source's shadow map.");

    MFUN(ulib_light_shadow_bias_set, "void", "shadowBias");
    ARG("float", "bias");
    DOC_FUNC(
      "Shadow map bias, how much to add or subtract from the normalized depth when "
      "deciding whether a surface is in shadow. The default is 0.0001. Very tiny "
      "adjustments here (in the order of 0.00005) may help reduce artifacts in "
      "shadows");

    MFUN(ulib_light_shadow_bias_get, "float", "shadowBias");
    DOC_FUNC("Get the bias for shadow maps generated by this light source");

    END_CLASS();

    // TODO document

    // point light ------------------------------------------------------------

    BEGIN_CLASS("GPointLight", SG_CKNames[SG_COMPONENT_LIGHT]);
    DOC_CLASS("Point light component.");

    CTOR(ulib_point_light_ctor);

    MFUN(ulib_point_light_get_radius, "float", "radius");
    DOC_FUNC("Get the point light radius.");

    MFUN(ulib_point_light_set_radius, "void", "radius");
    ARG("float", "radius");
    DOC_FUNC("Set the point light radius.");

    MFUN(ulib_point_light_get_falloff_exponent, "float", "falloff");
    DOC_FUNC(
      "Get the point light falloff exponent, i.e. how quickly the light intensity "
      "ramps down to 0. A value of 1.0 means linear, 2.0 means quadratic. Default "
      "is 2.0");

    MFUN(ulib_point_light_set_falloff_exponent, "void", "falloff");
    ARG("float", "falloff_exponent");
    DOC_FUNC(
      "Set the point light falloff exponent, i.e. how quickly the light intensity "
      "ramps down to 0. A value of 1.0 means linear, 2.0 means quadratic. Default "
      "is 2.0");

    END_CLASS();

    // directional light ------------------------------------------------------

    BEGIN_CLASS("GDirLight", SG_CKNames[SG_COMPONENT_LIGHT]);
    DOC_CLASS("Directional light component. Rotate with the GGen rotation methods.");

    CTOR(ulib_dir_light_ctor);

    MFUN(ulib_dir_light_shadow_size_set, "void", "shadowBounds");
    ARG("float", "size");
    ARG("float", "depth");
    DOC_FUNC(
      "Sets the size and depth of the orthographic projection used during shadow "
      "mapping. "
      "Larger values of `size` increase the amount of area covered by this directional "
      "light, but decrease overall shadow resolution. Ideally you want the size to be "
      "the minimal amount that still covers all necessary geometry in the scene. "
      "`depth` "
      "is used to near and far clipping planes. near = -depth/2, far = depth/2");

    MFUN(ulib_dir_light_shadow_size_get, "vec3", "shadowBounds");
    DOC_FUNC(
      "Returns the dimensions of the orthographic view frustrum used during shadow "
      "mapping. .x and .y are the width/height, .z is the depth. During rendering, "
      "the near plane is -z/2, and the far plane is z/2");

    END_CLASS();

    { // Spotlight
        BEGIN_CLASS("GSpotLight", SG_CKNames[SG_COMPONENT_LIGHT]);
        DOC_CLASS("Spotlight component. Rotate with the GGen rotation methods.");

        CTOR(ulib_spot_light_ctor);

        MFUN(ulib_spot_light_get_radius, "float", "range");
        DOC_FUNC("Get the light range");

        MFUN(ulib_spot_light_set_radius, "void", "range");
        ARG("float", "range");
        DOC_FUNC(
          "Set the light range. i.e. the distance at which intensity reaches 0.");

        MFUN(ulib_spot_light_get_falloff_exponent, "float", "falloff");
        DOC_FUNC("Get the light falloff exponent");

        MFUN(ulib_spot_light_set_falloff_exponent, "void", "falloff");
        ARG("float", "falloff");
        DOC_FUNC(
          "Set the light falloff exponent, i.e. how quickly the light intensity "
          "ramps down to 0 with distance. A value of 1.0 means linear, 2.0 means "
          "quadratic. Default is 2.0");

        MFUN(ulib_spot_light_get_angle_max, "float", "angleMax");
        DOC_FUNC("Get the maximum angle (in radians) of the spotlight");

        MFUN(ulib_spot_light_set_angle_max, "void", "angleMax");
        ARG("float", "radians");
        DOC_FUNC(
          "Set the maximum angle (in radians) of the spotlight. At this angle the "
          "spotlights intensity will reach 0");

        MFUN(ulib_spot_light_get_angle_min, "float", "angleMin");
        DOC_FUNC(
          "Get the angle (in radians) of the spotlight at which attenuation will "
          "begin.");

        MFUN(ulib_spot_light_set_angle_min, "void", "angleMin");
        ARG("float", "radians");
        DOC_FUNC(
          "Set the angle (in radians) of the spotlight at which attenuation will "
          "begin. Should be less than angleMax");

        MFUN(ulib_spot_light_get_angular_falloff, "float", "angularFalloff");
        DOC_FUNC(
          "Get the rate at which light intensity falls off from angleMin to angleMax. "
          "Default 2.0");

        MFUN(ulib_spot_light_set_angular_falloff, "void", "angularFalloff");
        ARG("float", "exponent");
        DOC_FUNC(
          "Set the rate at which light intensity falls off from angleMin to angleMax. "
          "Default 2.0");

        END_CLASS();
    }
}

CK_DLL_CTOR(ulib_light_ctor)
{
    ulib_light_create(SELF, SG_LightType_Directional);
}

CK_DLL_CTOR(ulib_light_ctor_with_type)
{
    t_CKINT type = GET_NEXT_INT(ARGS);
    ulib_light_create(SELF, (SG_LightType)type);
}

CK_DLL_MFUN(ulib_light_set_type)
{
    SG_Light* light = GET_LIGHT(SELF);
    t_CKINT type    = GET_NEXT_INT(ARGS);

    SG_Light::lightType(light, (SG_LightType)type);

    CQ_PushCommand_LightUpdate(light);
}

CK_DLL_MFUN(ulib_light_get_type)
{
    SG_Light* light = GET_LIGHT(SELF);
    RETURN->v_int   = (t_CKINT)light->desc.type;
}

CK_DLL_MFUN(ulib_light_set_color)
{
    SG_Light* light = GET_LIGHT(SELF);
    t_CKVEC3 color  = GET_NEXT_VEC3(ARGS);

    light->desc.color = { color.x, color.y, color.z };

    CQ_PushCommand_LightUpdate(light);
}

CK_DLL_MFUN(ulib_light_get_color)
{
    SG_Light* light = GET_LIGHT(SELF);
    RETURN->v_vec3  = { light->desc.color.r, light->desc.color.g, light->desc.color.b };
}

CK_DLL_MFUN(ulib_light_set_intensity)
{
    SG_Light* light     = GET_LIGHT(SELF);
    t_CKFLOAT intensity = GET_NEXT_FLOAT(ARGS);

    light->desc.intensity = intensity;

    CQ_PushCommand_LightUpdate(light);
}

CK_DLL_MFUN(ulib_light_get_intensity)
{
    SG_Light* light = GET_LIGHT(SELF);
    RETURN->v_float = light->desc.intensity;
}

CK_DLL_MFUN(ulib_light_generates_shadow_get)
{
    SG_Light* light = GET_LIGHT(SELF);
    RETURN->v_int   = light->desc.generates_shadows;
}

CK_DLL_MFUN(ulib_light_generates_shadow_set)
{
    SG_Light* light = GET_LIGHT(SELF);

    if (light->desc.type == SG_LightType_Point) {
        log_warn("PointLight Shadows are not implemented.");
        return;
    }

    light->desc.generates_shadows = GET_NEXT_INT(ARGS) ? 1 : 0;
    CQ_PushCommand_LightUpdate(light);
}

CK_DLL_MFUN(ulib_light_shadow_add_mesh)
{
    SG_Light* light     = GET_LIGHT(SELF);
    SG_Transform* xform = GET_XFORM(GET_NEXT_OBJECT(ARGS));
    bool add_children   = GET_NEXT_INT(ARGS);

    CQ_PushCommand_ShadowAddMesh(light, xform, add_children, true);
}

CK_DLL_MFUN(ulib_light_shadow_remove_mesh)
{
    SG_Light* light     = GET_LIGHT(SELF);
    SG_Transform* xform = GET_XFORM(GET_NEXT_OBJECT(ARGS));
    bool add_children   = GET_NEXT_INT(ARGS);

    CQ_PushCommand_ShadowAddMesh(light, xform, add_children, false);
}

CK_DLL_MFUN(ulib_light_shadow_bias_set)
{
    SG_Light* light  = GET_LIGHT(SELF);
    light->desc.bias = GET_NEXT_FLOAT(ARGS);
    CQ_PushCommand_LightUpdate(light);
}

CK_DLL_MFUN(ulib_light_shadow_bias_get)
{
    RETURN->v_float = GET_LIGHT(SELF)->desc.bias;
}

CK_DLL_CTOR(ulib_point_light_ctor)
{
    ulib_light_create(SELF, SG_LightType_Point);
}

CK_DLL_MFUN(ulib_point_light_get_radius)
{
    RETURN->v_float = GET_LIGHT(SELF)->desc.radius;
}

CK_DLL_MFUN(ulib_point_light_set_radius)
{
    SG_Light* light    = GET_LIGHT(SELF);
    t_CKFLOAT radius   = GET_NEXT_FLOAT(ARGS);
    light->desc.radius = radius;

    CQ_PushCommand_LightUpdate(light);
}

CK_DLL_MFUN(ulib_point_light_get_falloff_exponent)
{
    RETURN->v_float = GET_LIGHT(SELF)->desc.falloff;
}

CK_DLL_MFUN(ulib_point_light_set_falloff_exponent)
{
    SG_Light* light     = GET_LIGHT(SELF);
    t_CKFLOAT falloff   = GET_NEXT_FLOAT(ARGS);
    light->desc.falloff = falloff;

    CQ_PushCommand_LightUpdate(light);
}

CK_DLL_CTOR(ulib_dir_light_ctor)
{
    ulib_light_create(SELF, SG_LightType_Directional);
}

CK_DLL_MFUN(ulib_dir_light_shadow_size_set)
{
    SG_Light* light                          = GET_LIGHT(SELF);
    light->desc.dirlight_shadow_bounds.size  = GET_NEXT_FLOAT(ARGS);
    light->desc.dirlight_shadow_bounds.depth = GET_NEXT_FLOAT(ARGS);
    CQ_PushCommand_LightUpdate(light);
}

CK_DLL_MFUN(ulib_dir_light_shadow_size_get)
{
    SG_Light* light = GET_LIGHT(SELF);
    RETURN->v_vec3  = {
        light->desc.dirlight_shadow_bounds.size,
        light->desc.dirlight_shadow_bounds.size,
        light->desc.dirlight_shadow_bounds.depth,
    };
}

// Spotlight =====================================================

CK_DLL_CTOR(ulib_spot_light_ctor)
{
    ulib_light_create(SELF, SG_LightType_Spot);
}

CK_DLL_MFUN(ulib_spot_light_get_falloff_exponent)
{
    RETURN->v_float = GET_LIGHT(SELF)->desc.falloff;
}

CK_DLL_MFUN(ulib_spot_light_set_falloff_exponent)
{
    SG_Light* light     = GET_LIGHT(SELF);
    light->desc.falloff = GET_NEXT_FLOAT(ARGS);
    CQ_PushCommand_LightUpdate(light);
}

CK_DLL_MFUN(ulib_spot_light_get_radius)
{
    RETURN->v_float = GET_LIGHT(SELF)->desc.radius;
}

CK_DLL_MFUN(ulib_spot_light_set_radius)
{
    SG_Light* light    = GET_LIGHT(SELF);
    light->desc.radius = GET_NEXT_FLOAT(ARGS);
    CQ_PushCommand_LightUpdate(light);
}

CK_DLL_MFUN(ulib_spot_light_get_angle_max)
{
    RETURN->v_float = GET_LIGHT(SELF)->desc.angle_max;
}

CK_DLL_MFUN(ulib_spot_light_set_angle_max)
{
    SG_Light* light       = GET_LIGHT(SELF);
    light->desc.angle_max = GET_NEXT_FLOAT(ARGS);
    CQ_PushCommand_LightUpdate(light);
}

CK_DLL_MFUN(ulib_spot_light_get_angle_min)
{
    RETURN->v_float = GET_LIGHT(SELF)->desc.angle_min;
}

CK_DLL_MFUN(ulib_spot_light_set_angle_min)
{
    SG_Light* light       = GET_LIGHT(SELF);
    light->desc.angle_min = GET_NEXT_FLOAT(ARGS);
    CQ_PushCommand_LightUpdate(light);
}

CK_DLL_MFUN(ulib_spot_light_get_angular_falloff)
{
    RETURN->v_float = GET_LIGHT(SELF)->desc.angle_falloff;
}

CK_DLL_MFUN(ulib_spot_light_set_angular_falloff)
{
    SG_Light* light           = GET_LIGHT(SELF);
    light->desc.angle_falloff = GET_NEXT_FLOAT(ARGS);
    CQ_PushCommand_LightUpdate(light);
}
