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

#include "sync.cpp"

// Query function for base component class

CK_DLL_CTOR(component_ctor)
{
    // base class. don't allow direct instantiation
    // Chuck_DL_Api::Type componentType
    //   = API->type->lookup(VM, SG_CKNames[SG_COMPONENT_BASE]);
    // Chuck_DL_Api::Type thisType = API->object->get_type(SELF);
    // // test for conditions to create a component
    // if (API->type->is_equal(
    //       thisType,
    //       componentType) // this type is a Component (subclasses are handled
    //       on their own)
    //     || API->type->origin_hint(thisType)
    //          == ckte_origin_USERDEFINED // this type is defined .ck file
    //     || API->type->origin_hint(thisType)
    //          == ckte_origin_IMPORT // .ck file included in search path
    // ) {
    //   API->vm->throw_exception(
    //     "InstantatingAbstractBaseClassViolation",
    //     ""
    //     "Please replace this line with .nextFrame() => now;",
    //     SHRED);
    // }

    // SG_Component* component                   = new SG_Component();
    // OBJ_MEMBER_INT(SELF, component_offset_id) = (t_CKUINT)component;
}

CK_DLL_DTOR(component_dtor)
{
    SG_ComponentFree(SG_GetComponent(OBJ_MEMBER_UINT(SELF, component_offset_id)));
}

CK_DLL_MFUN(component_get_id)
{
    SG_Component* component
      = SG_GetComponent(OBJ_MEMBER_UINT(SELF, component_offset_id));
    RETURN->v_uint = component->id;
}

CK_DLL_MFUN(component_get_name)
{
    SG_Component* component
      = SG_GetComponent(OBJ_MEMBER_UINT(SELF, component_offset_id));
    RETURN->v_string = chugin_createCkString(component->name, false);
}

void ulib_component_set_name(SG_Component* component, const char* name)
{
    snprintf(component->name, sizeof(component->name), "%s", name);
    CQ_PushCommand_ComponentUpdateName(component);
}

CK_DLL_MFUN(component_set_name)
{
    SG_Component* component
      = SG_GetComponent(OBJ_MEMBER_UINT(SELF, component_offset_id));

    Chuck_String* name = GET_NEXT_STRING(ARGS);
    const char* ck_str = API->object->str(name);

    ulib_component_set_name(component, ck_str);

    RETURN->v_string = name;
}

static void ulib_component_query(Chuck_DL_Query* QUERY)
{
    QUERY->begin_class(QUERY, SG_CKNames[SG_COMPONENT_BASE], "Object");
    DOC_CLASS(
      "Base class for ALL chugl scenegraph components. Don't instantiate "
      "directly. Contains methods for setting and getting the name of the component "
      "(useful for debugging) and the unique id which chugl assigns");
    // member vars
    component_offset_id = QUERY->add_mvar(QUERY, "int", "@component_ptr", false);

    DTOR(component_dtor);

    // member functions
    QUERY->add_mfun(QUERY, component_get_id, "int", "id");
    DOC_FUNC("Get the unique chugl id of this component");

    QUERY->add_mfun(QUERY, component_get_name, "string", "name");
    DOC_FUNC("Get the name of this component");

    QUERY->add_mfun(QUERY, component_set_name, "string", "name");
    QUERY->add_arg(QUERY, "string", "name");
    DOC_FUNC("Set the name of this component");

    QUERY->end_class(QUERY); // Component
}

// ============================================================================
// GGen
// ============================================================================

CK_DLL_CTOR(ggen_ctor);
CK_DLL_DTOR(ggen_dtor);

// internal
CK_DLL_MFUN(ggen_update);

// transform API
CK_DLL_MFUN(ggen_get_right);
CK_DLL_MFUN(ggen_get_forward);
CK_DLL_MFUN(ggen_get_up);

// Position
CK_DLL_MFUN(ggen_get_pos_x);
CK_DLL_MFUN(ggen_set_pos_x);

CK_DLL_MFUN(ggen_get_pos_y);
CK_DLL_MFUN(ggen_set_pos_y);

CK_DLL_MFUN(ggen_get_pos_z);
CK_DLL_MFUN(ggen_set_pos_z);

CK_DLL_MFUN(ggen_set_pos);
CK_DLL_MFUN(ggen_set_pos3_no_vec);
CK_DLL_MFUN(ggen_get_pos);
CK_DLL_MFUN(ggen_set_pos_vec2);
CK_DLL_MFUN(ggen_set_pos2_no_vec2);

CK_DLL_MFUN(ggen_get_pos_world);
CK_DLL_MFUN(ggen_set_pos_world);
CK_DLL_MFUN(ggen_set_pos_world_vec2);

CK_DLL_MFUN(ggen_translate);
CK_DLL_MFUN(ggen_translate_x);
CK_DLL_MFUN(ggen_translate_y);
CK_DLL_MFUN(ggen_translate_z);

// Rotation
CK_DLL_MFUN(ggen_get_rot_x);
CK_DLL_MFUN(ggen_set_rot_x);

CK_DLL_MFUN(ggen_get_rot_y);
CK_DLL_MFUN(ggen_set_rot_y);

CK_DLL_MFUN(ggen_get_rot_z);
CK_DLL_MFUN(ggen_set_rot_z);

CK_DLL_MFUN(ggen_set_rot);
CK_DLL_MFUN(ggen_get_rot);

CK_DLL_MFUN(ggen_rotate);
CK_DLL_MFUN(ggen_rotate_x);
CK_DLL_MFUN(ggen_rotate_y);
CK_DLL_MFUN(ggen_rotate_z);

CK_DLL_MFUN(ggen_rot_on_local_axis);
CK_DLL_MFUN(ggen_rot_on_world_axis);

CK_DLL_MFUN(ggen_lookat_vec3);
CK_DLL_MFUN(ggen_lookat_vec3_with_up);
CK_DLL_MFUN(ggen_lookat_dir);
// CK_DLL_MFUN(ggen_rotate_by);  // no rotate by design because converting from
// euler angles to quat is ambiguous

// Scale
CK_DLL_MFUN(ggen_get_scale_x);
CK_DLL_MFUN(ggen_set_scale_x);

CK_DLL_MFUN(ggen_get_scale_y);
CK_DLL_MFUN(ggen_set_scale_y);

CK_DLL_MFUN(ggen_get_scale_z);
CK_DLL_MFUN(ggen_set_scale_z);

CK_DLL_MFUN(ggen_get_scale);
CK_DLL_MFUN(ggen_set_scale);
CK_DLL_MFUN(ggen_set_scale_uniform);

CK_DLL_MFUN(ggen_get_scale_world);
CK_DLL_MFUN(ggen_set_scale_world);
CK_DLL_MFUN(ggen_set_scale_world_uniform);

CK_DLL_MFUN(ggen_set_scale_2d);

// transformation matrix API
CK_DLL_MFUN(ggen_local_pos_to_world_pos);

// parent-child scenegraph API
CK_DLL_MFUN(ggen_get_parent);
CK_DLL_MFUN(ggen_get_child_default);
CK_DLL_MFUN(ggen_get_child);
CK_DLL_MFUN(ggen_get_num_children);
CK_DLL_MFUN(ggen_get_scene);
CK_DLL_GFUN(ggen_op_gruck);   // add child
CK_DLL_GFUN(ggen_op_ungruck); // remove child
CK_DLL_MFUN(ggen_detach_parent);
CK_DLL_MFUN(ggen_detach_children);
CK_DLL_MFUN(ggen_detach_all);

// shadow API
CK_DLL_MFUN(gmesh_get_shadowed);
CK_DLL_MFUN(gmesh_set_shadowed);
CK_DLL_MFUN(ggen_set_shadowed_all_children);

static void ulib_ggen_query(Chuck_DL_Query* QUERY)
{
    { // GGen
        QUERY->begin_class(QUERY, SG_CKNames[SG_COMPONENT_TRANSFORM],
                           SG_CKNames[SG_COMPONENT_BASE]);
        DOC_CLASS(
          "Chugl graphics generator. Can be grucked to the scenegraph. Contains "
          "methods for transform manipulation");
        ADD_EX("basic/orbits.ck");
        ADD_EX("deep/ik.ck");

        QUERY->add_ctor(QUERY, ggen_ctor);
        QUERY->add_dtor(QUERY, ggen_dtor);

        QUERY->add_mfun(QUERY, ggen_update, "void", "update");
        QUERY->add_arg(QUERY, "float", "dt");
        QUERY->doc_func(QUERY,
                        "This method is automatically invoked once per frame "
                        "for all GGens connected to the scene graph."
                        "Override this method in custom GGen classes to "
                        "implement your own update logic.");

        QUERY->add_mfun(QUERY, ggen_get_right, "vec3", "right");
        QUERY->doc_func(QUERY, "Get the right vector of this GGen in world space");

        QUERY->add_mfun(QUERY, ggen_get_forward, "vec3", "forward");
        QUERY->doc_func(QUERY, "Get the forward vector of this GGen in world space");

        QUERY->add_mfun(QUERY, ggen_get_up, "vec3", "up");
        QUERY->doc_func(QUERY, "Get the up vector of this GGen in world space");

        // Position
        // ===============================================================

        // float posX()
        QUERY->add_mfun(QUERY, ggen_get_pos_x, "float", "posX");
        QUERY->doc_func(QUERY, "Get X position of this GGen in local space");

        // float posX(float)
        QUERY->add_mfun(QUERY, ggen_set_pos_x, SG_CKNames[SG_COMPONENT_TRANSFORM],
                        "posX");
        QUERY->add_arg(QUERY, "float", "pos");
        QUERY->doc_func(QUERY, "Set X position of this GGen in local space");

        // float posY()
        QUERY->add_mfun(QUERY, ggen_get_pos_y, "float", "posY");
        QUERY->doc_func(QUERY, "Get Y position of this GGen in local space");

        // float posY(float)
        QUERY->add_mfun(QUERY, ggen_set_pos_y, SG_CKNames[SG_COMPONENT_TRANSFORM],
                        "posY");
        QUERY->add_arg(QUERY, "float", "pos");
        QUERY->doc_func(QUERY, "Set Y position of this GGen in local space");

        // float posZ()
        QUERY->add_mfun(QUERY, ggen_get_pos_z, "float", "posZ");
        QUERY->doc_func(QUERY, "Get Z position of this GGen in local space");

        // float posZ(float)
        QUERY->add_mfun(QUERY, ggen_set_pos_z, SG_CKNames[SG_COMPONENT_TRANSFORM],
                        "posZ");
        QUERY->add_arg(QUERY, "float", "pos");
        QUERY->doc_func(QUERY, "Set Z position of this GGen in local space");

        // vec3 pos()
        QUERY->add_mfun(QUERY, ggen_get_pos, "vec3", "pos");
        QUERY->doc_func(QUERY, "Get object position in local space");

        // vec3 pos( vec3 )
        QUERY->add_mfun(QUERY, ggen_set_pos, SG_CKNames[SG_COMPONENT_TRANSFORM], "pos");
        QUERY->add_arg(QUERY, "vec3", "pos");
        QUERY->doc_func(QUERY, "Set object position in local space");

        MFUN(ggen_set_pos3_no_vec, SG_CKNames[SG_COMPONENT_TRANSFORM], "pos");
        ARG("float", "x");
        ARG("float", "y");
        ARG("float", "z");
        DOC_FUNC("Set object position in local space");

        QUERY->add_mfun(QUERY, ggen_set_pos_vec2, SG_CKNames[SG_COMPONENT_TRANSFORM],
                        "pos");
        QUERY->add_arg(QUERY, "vec2", "pos");
        QUERY->doc_func(QUERY, "Set object XY position in local space. Z is preserved");

        MFUN(ggen_set_pos2_no_vec2, SG_CKNames[SG_COMPONENT_TRANSFORM], "pos");
        ARG("float", "x");
        ARG("float", "y");
        DOC_FUNC("Set object XY position in local space. Z is preserved");

        // vec3 posWorld()
        QUERY->add_mfun(QUERY, ggen_get_pos_world, "vec3", "posWorld");
        QUERY->doc_func(QUERY, "Get object position in world space");

        // vec3 posWorld( float )
        QUERY->add_mfun(QUERY, ggen_set_pos_world, SG_CKNames[SG_COMPONENT_TRANSFORM],
                        "posWorld");
        QUERY->add_arg(QUERY, "vec3", "pos");
        QUERY->doc_func(QUERY, "Set object position in world space");

        QUERY->add_mfun(QUERY, ggen_set_pos_world_vec2,
                        SG_CKNames[SG_COMPONENT_TRANSFORM], "posWorld");
        QUERY->add_arg(QUERY, "vec2", "pos");
        QUERY->doc_func(QUERY, "Set object XY position in world space. Z is preserved");

        // GGen translate( vec3 )
        QUERY->add_mfun(QUERY, ggen_translate, "GGen", "translate");
        QUERY->add_arg(QUERY, "vec3", "translation");
        QUERY->doc_func(QUERY, "Translate this GGen by the given vector");

        // GGen translateX( float )
        QUERY->add_mfun(QUERY, ggen_translate_x, "GGen", "translateX");
        QUERY->add_arg(QUERY, "float", "amt");
        QUERY->doc_func(
          QUERY, "Translate this GGen by given amount on the X axis in local space");

        // GGen translateY( float )
        QUERY->add_mfun(QUERY, ggen_translate_y, "GGen", "translateY");
        QUERY->add_arg(QUERY, "float", "amt");
        QUERY->doc_func(
          QUERY, "Translate this GGen by given amount on the Y axis in local space");

        // GGen translateZ( float )
        QUERY->add_mfun(QUERY, ggen_translate_z, "GGen", "translateZ");
        QUERY->add_arg(QUERY, "float", "amt");
        QUERY->doc_func(
          QUERY, "Translate this GGen by given amount on the Z axis in local space");

        // Rotation
        // ===============================================================

        // float rotX()
        QUERY->add_mfun(QUERY, ggen_get_rot_x, "float", "rotX");
        QUERY->doc_func(QUERY,
                        "Get the rotation of this GGen on the X axis in local space");

        // float rotX( float )
        QUERY->add_mfun(QUERY, ggen_set_rot_x, "GGen", "rotX");
        QUERY->add_arg(QUERY, "float", "radians");
        QUERY->doc_func(QUERY,
                        "Set the rotation of this GGen on the X axis in local "
                        "space to the given radians");

        // float rotY()
        QUERY->add_mfun(QUERY, ggen_get_rot_y, "float", "rotY");
        QUERY->doc_func(QUERY,
                        "Get the rotation of this GGen on the Y axis in local space");

        // float rotY( float )
        QUERY->add_mfun(QUERY, ggen_set_rot_y, "GGen", "rotY");
        QUERY->add_arg(QUERY, "float", "radians");
        QUERY->doc_func(QUERY,
                        "Set the rotation of this GGen on the Y axis in local "
                        "space to the given radians");

        // float rotZ()
        QUERY->add_mfun(QUERY, ggen_get_rot_z, "float", "rotZ");
        QUERY->doc_func(QUERY,
                        "Get the rotation of this GGen on the Z axis in local space");

        // float rotZ( float )
        QUERY->add_mfun(QUERY, ggen_set_rot_z, "GGen", "rotZ");
        QUERY->add_arg(QUERY, "float", "radians");
        QUERY->doc_func(QUERY,
                        "Set the rotation of this GGen on the Z axis in local "
                        "space to the given radians");

        // vec3 rot()
        QUERY->add_mfun(QUERY, ggen_get_rot, "vec3", "rot");
        QUERY->doc_func(
          QUERY, "Get object rotation in local space as euler angles in radians");

        // vec3 rot( vec3 )
        QUERY->add_mfun(QUERY, ggen_set_rot, "vec3", "rot");
        QUERY->add_arg(QUERY, "vec3", "eulers");
        QUERY->doc_func(QUERY,
                        "Set rotation of this GGen in local space as euler angles");

        // GGen rotate( vec3 )
        QUERY->add_mfun(QUERY, ggen_rotate, "GGen", "rotate");
        QUERY->add_arg(QUERY, "vec3", "eulers");
        QUERY->doc_func(QUERY,
                        "Rotate this GGen by the given euler angles in local space");

        // GGen rotateX( float )
        QUERY->add_mfun(QUERY, ggen_rotate_x, "GGen", "rotateX");
        QUERY->add_arg(QUERY, "float", "radians");
        QUERY->doc_func(
          QUERY, "Rotate this GGen by the given radians on the X axis in local space");

        // GGen rotateY( float )
        QUERY->add_mfun(QUERY, ggen_rotate_y, "GGen", "rotateY");
        QUERY->add_arg(QUERY, "float", "radians");
        QUERY->doc_func(
          QUERY, "Rotate this GGen by the given radians on the Y axis in local space");

        // GGen rotateZ( float )
        QUERY->add_mfun(QUERY, ggen_rotate_z, "GGen", "rotateZ");
        QUERY->add_arg(QUERY, "float", "radians");
        QUERY->doc_func(
          QUERY, "Rotate this GGen by the given radians on the Z axis in local space");

        // GGen rotateOnLocalAxis( vec3, float )
        QUERY->add_mfun(QUERY, ggen_rot_on_local_axis, "GGen", "rotateOnLocalAxis");
        QUERY->add_arg(QUERY, "vec3", "axis");
        QUERY->add_arg(QUERY, "float", "radians");
        QUERY->doc_func(QUERY,
                        "Rotate this GGen by the given radians on the given "
                        "axis in local space");

        // GGen rotateOnWorldAxis( vec3, float )
        QUERY->add_mfun(QUERY, ggen_rot_on_world_axis, "GGen", "rotateOnWorldAxis");
        QUERY->add_arg(QUERY, "vec3", "axis");
        QUERY->add_arg(QUERY, "float", "radians");
        QUERY->doc_func(QUERY,
                        "Rotate this GGen by the given radians on the given "
                        "axis in world space");

        // GGen lookAt( vec3 )
        QUERY->add_mfun(QUERY, ggen_lookat_vec3, "GGen", "lookAt");
        QUERY->add_arg(QUERY, "vec3", "pos");
        QUERY->doc_func(
          QUERY,
          "Rotate this GGen to look at the given position in world space. Uses a "
          "default UP vector of @(0,1,0). Be careful that the forward vector "
          "(GGen.posWorld() - pos) and this up vector are not collinear");

        MFUN(ggen_lookat_vec3_with_up, "GGen", "lookAt");
        ARG("vec3", "pos");
        ARG("vec3", "up");
        DOC_FUNC(
          "Rotate this GGen to look at the given position in world space."
          "Be careful that the forward vector (GGen.posWorld() - pos) and this up "
          "vector are not collinear");

        // Scale ===============================================================

        // float scaX()
        QUERY->add_mfun(QUERY, ggen_get_scale_x, "float", "scaX");
        QUERY->doc_func(QUERY, "Get X scale of this GGen in local space");

        // float scaX( float )
        QUERY->add_mfun(QUERY, ggen_set_scale_x, "GGen", "scaX");
        QUERY->add_arg(QUERY, "float", "scale");
        QUERY->doc_func(QUERY, "Set X scale of this GGen in local space");

        // float scaY()
        QUERY->add_mfun(QUERY, ggen_get_scale_y, "float", "scaY");
        QUERY->doc_func(QUERY, "Get Y scale of this GGen in local space");

        // float scaY( float )
        QUERY->add_mfun(QUERY, ggen_set_scale_y, "GGen", "scaY");
        QUERY->add_arg(QUERY, "float", "scale");
        QUERY->doc_func(QUERY, "Set Y scale of this GGen in local space");

        // float scaZ()
        QUERY->add_mfun(QUERY, ggen_get_scale_z, "float", "scaZ");
        QUERY->doc_func(QUERY, "Get Z scale of this GGen in local space");

        // float scaZ( float )
        QUERY->add_mfun(QUERY, ggen_set_scale_z, "GGen", "scaZ");
        QUERY->add_arg(QUERY, "float", "scale");
        QUERY->doc_func(QUERY, "Set Z scale of this GGen in local space");

        // vec3 sca()
        QUERY->add_mfun(QUERY, ggen_get_scale, "vec3", "sca");
        QUERY->doc_func(QUERY, "Get object scale in local space");

        // vec3 sca( vec3 )
        QUERY->add_mfun(QUERY, ggen_set_scale, "GGen", "sca");
        QUERY->add_arg(QUERY, "vec3", "scale");
        QUERY->doc_func(QUERY, "Set object scale in local space");

        // vec3 sca( float )
        QUERY->add_mfun(QUERY, ggen_set_scale_uniform, "GGen", "sca");
        QUERY->add_arg(QUERY, "float", "scale");
        QUERY->doc_func(QUERY,
                        "Set object scale in local space uniformly across all axes");

        // vec3 scaWorld()
        QUERY->add_mfun(QUERY, ggen_get_scale_world, "vec3", "scaWorld");
        QUERY->doc_func(QUERY, "Get object scale in world space");

        // vec3 scaWorld( vec3 )
        QUERY->add_mfun(QUERY, ggen_set_scale_world, "GGen", "scaWorld");
        QUERY->add_arg(QUERY, "vec3", "scale");
        QUERY->doc_func(QUERY, "Set object scale in world space");

        QUERY->add_mfun(QUERY, ggen_set_scale_world_uniform, "GGen", "scaWorld");
        QUERY->add_arg(QUERY, "float", "scale");
        QUERY->doc_func(QUERY,
                        "Set object scale in world space uniformly across all axes");

        MFUN(ggen_set_scale_2d, "GGen", "sca");
        ARG("vec2", "scale_xy");
        DOC_FUNC("Sets object x and y scale (useful for 2d)");

        // Matrix transform API
        // ===============================================================
        QUERY->add_mfun(QUERY, ggen_local_pos_to_world_pos, "vec3", "posLocalToWorld");
        QUERY->add_arg(QUERY, "vec3", "localPos");
        QUERY->doc_func(QUERY, "Transform a position in local space to world space");

        // scenegraph relationship methods
        // =======================================
        QUERY->add_mfun(QUERY, ggen_get_parent, "GGen", "parent");
        QUERY->doc_func(QUERY, "Get the parent of this GGen");

        QUERY->add_mfun(QUERY, ggen_get_child, "GGen", "child");
        QUERY->add_arg(QUERY, "int", "n");
        QUERY->doc_func(QUERY, "Get the n'th child of this GGen");

        MFUN(ggen_get_scene, "GGen", "scene");
        DOC_FUNC(
          "Get the scene this GGen is attached to. Returns null if not attached");

        QUERY->add_mfun(QUERY, ggen_get_child_default, "GGen", "child");
        QUERY->doc_func(QUERY, "Get the 0th child of this GGen");

        QUERY->add_mfun(QUERY, ggen_get_num_children, "int", "numChildren");
        QUERY->doc_func(QUERY, "Get the number of children for this GGen");

        // overload GGen --> GGen
        QUERY->add_op_overload_binary(QUERY, ggen_op_gruck, "GGen", "-->", "GGen",
                                      "lhs", "GGen", "rhs");

        // overload GGen --< GGen
        QUERY->add_op_overload_binary(QUERY, ggen_op_ungruck, "GGen", "--<", "GGen",
                                      "lhs", "GGen", "rhs");

        MFUN(ggen_detach_parent, "void", "detachParent");
        DOC_FUNC(
          "Detach this GGen from its parent. Equivalent to ggen --< ggen.parent()");

        MFUN(ggen_detach_children, "void", "detachChildren");
        DOC_FUNC(
          "Detach all children from this GGen. Equivalent to ggen --< ggen.child(i) "
          "for all children. children are NOT reparented");

        MFUN(ggen_detach_all, "void", "detach");
        DOC_FUNC(
          "Detach this GGen from its parent and all children. children are NOT "
          "reparented");

        MFUN(ggen_set_shadowed_all_children, "void", "shadowed");
        ARG("int", "receive_shadows");
        ARG("int", "apply_to_subgraph");
        DOC_FUNC(
          "Set whether this GGen and all children can receive shadows. See "
          "GMesh.shadowed(int) for more details on how this flag works");

        QUERY->end_class(QUERY); // GGen
    }

    // set update vt offset
    ggen_update_vt_offset
      = chugin_setVTableOffset(SG_CKNames[SG_COMPONENT_TRANSFORM], "update");
}

// CGLObject DLL ==============================================
CK_DLL_CTOR(ggen_ctor)
{
    Chuck_DL_Api::Type ggenType
      = API->type->lookup(VM, SG_CKNames[SG_COMPONENT_TRANSFORM]);
    Chuck_DL_Api::Type thisType = API->object->get_type(SELF);
    // only create if:
    if (API->type->is_equal(
          thisType,
          ggenType) // this type is GGen (subclasses are handled on their own)
        || API->type->origin_hint(thisType)
             == ckte_origin_USERDEFINED // this type is defined .ck file
        || API->type->origin_hint(thisType)
             == ckte_origin_IMPORT // .ck file included in search path
    ) {
        ulib_ggen_create(SELF, SHRED);
    }

    // always register to shred map
    chugin_setOriginShred(SELF, SHRED);
}

CK_DLL_DTOR(ggen_dtor)
{
    // unregister from Shred2GGen map
    // (so we don't geta null ptr reference when the host SHRED exits and tries
    // to detach all GGens)
    // CGL::UnregisterGGenFromShred(SHRED, SELF);

    // push command to destroy this object on render thread as well
    // // CGL::PushCommand(new DestroySceneGraphNodeCommand(
    //   SELF, CGL::GetGGenDataOffset(), API, &CGL::mainScene));
}

CK_DLL_MFUN(ggen_update)
{
}

CK_DLL_MFUN(ggen_get_right)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    glm::vec3 right     = SG_Transform::right(xform);
    RETURN->v_vec3      = { right.x, right.y, right.z };
}

CK_DLL_MFUN(ggen_get_forward)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    glm::vec3 vec       = SG_Transform::forward(xform);
    RETURN->v_vec3      = { vec.x, vec.y, vec.z };
}

CK_DLL_MFUN(ggen_get_up)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    glm::vec3 vec       = SG_Transform::up(xform);
    RETURN->v_vec3      = { vec.x, vec.y, vec.z };
}

// Position Impl ===============================================================

CK_DLL_MFUN(ggen_get_pos_x)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    RETURN->v_float     = xform->pos.x;
}

CK_DLL_MFUN(ggen_set_pos_x)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKFLOAT posX      = GET_NEXT_FLOAT(ARGS);
    xform->pos.x        = posX;
    RETURN->v_object    = SELF;

    CQ_PushCommand_SetPosition(xform);
}

CK_DLL_MFUN(ggen_get_pos_y)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    RETURN->v_float     = xform->pos.y;
}

CK_DLL_MFUN(ggen_set_pos_y)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKFLOAT posY      = GET_NEXT_FLOAT(ARGS);
    xform->pos.y        = posY;
    RETURN->v_object    = SELF;

    CQ_PushCommand_SetPosition(xform);
}

CK_DLL_MFUN(ggen_get_pos_z)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    RETURN->v_float     = xform->pos.z;
}

CK_DLL_MFUN(ggen_set_pos_z)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKFLOAT posZ      = GET_NEXT_FLOAT(ARGS);
    xform->pos.z        = posZ;
    RETURN->v_object    = SELF;

    CQ_PushCommand_SetPosition(xform);
}

CK_DLL_MFUN(ggen_get_pos)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    RETURN->v_vec3      = { xform->pos.x, xform->pos.y, xform->pos.z };
}

CK_DLL_MFUN(ggen_set_pos)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKVEC3 vec        = GET_NEXT_VEC3(ARGS);
    xform->pos.x        = vec.x;
    xform->pos.y        = vec.y;
    xform->pos.z        = vec.z;
    RETURN->v_object    = SELF;

    CQ_PushCommand_SetPosition(xform);
}

CK_DLL_MFUN(ggen_set_pos3_no_vec)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    xform->pos.x        = GET_NEXT_FLOAT(ARGS);
    xform->pos.y        = GET_NEXT_FLOAT(ARGS);
    xform->pos.z        = GET_NEXT_FLOAT(ARGS);
    RETURN->v_object    = SELF;

    CQ_PushCommand_SetPosition(xform);
}

CK_DLL_MFUN(ggen_set_pos_vec2)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKVEC2 vec        = GET_NEXT_VEC2(ARGS);
    xform->pos.x        = vec.x;
    xform->pos.y        = vec.y;
    RETURN->v_object    = SELF;

    CQ_PushCommand_SetPosition(xform);
}

CK_DLL_MFUN(ggen_set_pos2_no_vec2)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    xform->pos.x        = GET_NEXT_FLOAT(ARGS);
    xform->pos.y        = GET_NEXT_FLOAT(ARGS);
    RETURN->v_object    = SELF;

    CQ_PushCommand_SetPosition(xform);
}

CK_DLL_MFUN(ggen_get_pos_world)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    glm::vec3 vec       = SG_Transform::worldPosition(xform);
    RETURN->v_vec3      = { vec.x, vec.y, vec.z };
}

CK_DLL_MFUN(ggen_set_pos_world)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKVEC3 vec        = GET_NEXT_VEC3(ARGS);
    SG_Transform::worldPosition(xform, glm::vec3(vec.x, vec.y, vec.z));

    CQ_PushCommand_SetPosition(xform);

    RETURN->v_object = SELF;
}

CK_DLL_MFUN(ggen_set_pos_world_vec2)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKVEC2 vec        = GET_NEXT_VEC2(ARGS);
    glm::vec3 world_pos = SG_Transform::worldPosition(xform);
    SG_Transform::worldPosition(xform, glm::vec3(vec.x, vec.y, world_pos.z));

    CQ_PushCommand_SetPosition(xform);

    RETURN->v_object = SELF;
}

CK_DLL_MFUN(ggen_translate)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKVEC3 trans      = GET_NEXT_VEC3(ARGS);
    SG_Transform::translate(xform, glm::vec3(trans.x, trans.y, trans.z));

    CQ_PushCommand_SetPosition(xform);

    RETURN->v_object = SELF;
}

CK_DLL_MFUN(ggen_translate_x)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKFLOAT amt       = GET_NEXT_FLOAT(ARGS);
    SG_Transform::translate(xform, { amt, 0, 0 });

    CQ_PushCommand_SetPosition(xform);

    RETURN->v_object = SELF;
}

CK_DLL_MFUN(ggen_translate_y)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKFLOAT amt       = GET_NEXT_FLOAT(ARGS);
    SG_Transform::translate(xform, { 0, amt, 0 });

    RETURN->v_object = SELF;

    CQ_PushCommand_SetPosition(xform);
}

CK_DLL_MFUN(ggen_translate_z)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKFLOAT amt       = GET_NEXT_FLOAT(ARGS);
    SG_Transform::translate(xform, { 0, 0, amt });

    RETURN->v_object = SELF;

    CQ_PushCommand_SetPosition(xform);
}

// Rotation Impl ===============================================================

CK_DLL_MFUN(ggen_get_rot_x)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    RETURN->v_float     = SG_Transform::eulerRotationRadians(xform).x;
}

CK_DLL_MFUN(ggen_set_rot_x)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKFLOAT rad       = GET_NEXT_FLOAT(ARGS); // set in radians
    glm::vec3 eulers    = SG_Transform::eulerRotationRadians(xform);
    eulers.x            = rad;
    xform->rot          = glm::quat(eulers);

    RETURN->v_object = SELF;

    CQ_PushCommand_SetRotation(xform);
}

CK_DLL_MFUN(ggen_get_rot_y)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    RETURN->v_float     = SG_Transform::eulerRotationRadians(xform).y;
}

CK_DLL_MFUN(ggen_set_rot_y)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKFLOAT rad       = GET_NEXT_FLOAT(ARGS); // set in radians
    // https://gamedev.stackexchange.com/questions/200292/applying-incremental-rotation-with-quaternions-flickering-or-hesitating
    // For continuous rotation, wrap rad to be in range [-PI/2, PI/2]
    // i.e. after exceeding PI/2, rad = rad - PI
    rad = glm::mod(rad + glm::half_pi<double>(), glm::pi<double>())
          - glm::half_pi<double>();
    glm::vec3 eulers = SG_Transform::eulerRotationRadians(xform);
    eulers.y         = rad;
    xform->rot       = glm::quat(eulers);

    RETURN->v_object = SELF;

    CQ_PushCommand_SetRotation(xform);
}

CK_DLL_MFUN(ggen_get_rot_z)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    RETURN->v_float     = SG_Transform::eulerRotationRadians(xform).z;
}

CK_DLL_MFUN(ggen_set_rot_z)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKFLOAT rad       = GET_NEXT_FLOAT(ARGS); // set in radians
    glm::vec3 eulers    = SG_Transform::eulerRotationRadians(xform);
    eulers.z            = rad;
    xform->rot          = glm::quat(eulers);

    RETURN->v_object = SELF;

    CQ_PushCommand_SetRotation(xform);
}

CK_DLL_MFUN(ggen_get_rot)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    glm::vec3 vec       = SG_Transform::eulerRotationRadians(xform);
    RETURN->v_vec3      = { vec.x, vec.y, vec.z };
}

CK_DLL_MFUN(ggen_set_rot)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKVEC3 vec        = GET_NEXT_VEC3(ARGS);
    xform->rot          = glm::quat(glm::vec3(vec.x, vec.y, vec.z));
    RETURN->v_vec3      = vec;

    CQ_PushCommand_SetRotation(xform);
}

CK_DLL_MFUN(ggen_rotate)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKVEC3 vec        = GET_NEXT_VEC3(ARGS);
    SG_Transform::rotate(xform, glm::vec3(vec.x, vec.y, vec.z));
    RETURN->v_object = SELF;

    CQ_PushCommand_SetRotation(xform);
}

CK_DLL_MFUN(ggen_rotate_x)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKFLOAT rad       = GET_NEXT_FLOAT(ARGS);
    SG_Transform::rotateX(xform, rad);
    RETURN->v_object = SELF;

    CQ_PushCommand_SetRotation(xform);
}

CK_DLL_MFUN(ggen_rotate_y)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKFLOAT rad       = GET_NEXT_FLOAT(ARGS);
    SG_Transform::rotateY(xform, rad);
    RETURN->v_object = SELF;
    CQ_PushCommand_SetRotation(xform);
}

CK_DLL_MFUN(ggen_rotate_z)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKFLOAT rad       = GET_NEXT_FLOAT(ARGS);
    SG_Transform::rotateZ(xform, rad);
    RETURN->v_object = SELF;
    CQ_PushCommand_SetRotation(xform);
}

CK_DLL_MFUN(ggen_rot_on_local_axis)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKVEC3 vec        = GET_NEXT_VEC3(ARGS);
    t_CKFLOAT rad       = GET_NEXT_FLOAT(ARGS);
    SG_Transform::rotateOnLocalAxis(xform, glm::vec3(vec.x, vec.y, vec.z), rad);

    RETURN->v_object = SELF;

    CQ_PushCommand_SetRotation(xform);
}

CK_DLL_MFUN(ggen_rot_on_world_axis)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKVEC3 vec        = GET_NEXT_VEC3(ARGS);
    t_CKFLOAT rad       = GET_NEXT_FLOAT(ARGS);
    SG_Transform::rotateOnWorldAxis(xform, glm::vec3(vec.x, vec.y, vec.z), rad);

    RETURN->v_object = SELF;

    CQ_PushCommand_SetRotation(xform);
}

CK_DLL_MFUN(ggen_lookat_vec3)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKVEC3 vec        = GET_NEXT_VEC3(ARGS);
    SG_Transform::lookAt(xform, glm::vec3(vec.x, vec.y, vec.z));
    RETURN->v_object = SELF;
    CQ_PushCommand_SetRotation(xform);
}

CK_DLL_MFUN(ggen_lookat_vec3_with_up)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKVEC3 vec        = GET_NEXT_VEC3(ARGS);
    t_CKVEC3 up         = GET_NEXT_VEC3(ARGS);
    SG_Transform::lookAt(xform, glm::vec3(vec.x, vec.y, vec.z),
                         glm::vec3(up.x, up.y, up.z));
    RETURN->v_object = SELF;
    CQ_PushCommand_SetRotation(xform);
}

// Scale impl ===============================================================

CK_DLL_MFUN(ggen_get_scale_x)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    RETURN->v_float     = xform->sca.x;
}

CK_DLL_MFUN(ggen_set_scale_x)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKFLOAT scaleX    = GET_NEXT_FLOAT(ARGS);
    xform->sca.x        = scaleX;
    RETURN->v_object    = SELF;
    CQ_PushCommand_SetScale(xform);
}

CK_DLL_MFUN(ggen_get_scale_y)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    RETURN->v_float     = xform->sca.y;
}

CK_DLL_MFUN(ggen_set_scale_y)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKFLOAT scaleY    = GET_NEXT_FLOAT(ARGS);
    xform->sca.y        = scaleY;
    RETURN->v_object    = SELF;
    CQ_PushCommand_SetScale(xform);
}

CK_DLL_MFUN(ggen_get_scale_z)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    RETURN->v_float     = xform->sca.z;
}

CK_DLL_MFUN(ggen_set_scale_z)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKFLOAT scaleZ    = GET_NEXT_FLOAT(ARGS);
    xform->sca.z        = scaleZ;
    RETURN->v_object    = SELF;
    CQ_PushCommand_SetScale(xform);
}

CK_DLL_MFUN(ggen_get_scale)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    RETURN->v_vec3      = { xform->sca.x, xform->sca.y, xform->sca.z };
}

CK_DLL_MFUN(ggen_set_scale)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKVEC3 vec        = GET_NEXT_VEC3(ARGS);
    xform->sca          = glm::vec3(vec.x, vec.y, vec.z);
    RETURN->v_object    = SELF;
    CQ_PushCommand_SetScale(xform);
}

CK_DLL_MFUN(ggen_set_scale_uniform)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKFLOAT s         = GET_NEXT_FLOAT(ARGS);
    xform->sca.x        = s;
    xform->sca.y        = s;
    xform->sca.z        = s;
    RETURN->v_object    = SELF;
    CQ_PushCommand_SetScale(xform);
}

CK_DLL_MFUN(ggen_get_scale_world)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    glm::vec3 vec       = SG_Transform::worldScale(xform);
    RETURN->v_vec3      = { vec.x, vec.y, vec.z };
}

CK_DLL_MFUN(ggen_set_scale_world)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKVEC3 vec        = GET_NEXT_VEC3(ARGS);
    SG_Transform::worldScale(xform, glm::vec3(vec.x, vec.y, vec.z));
    RETURN->v_object = SELF;
    CQ_PushCommand_SetScale(xform);
}

CK_DLL_MFUN(ggen_set_scale_world_uniform)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKFLOAT sca       = GET_NEXT_FLOAT(ARGS);
    SG_Transform::worldScale(xform, glm::vec3(sca, sca, sca));
    RETURN->v_object = SELF;
    CQ_PushCommand_SetScale(xform);
}

CK_DLL_MFUN(ggen_set_scale_2d)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKVEC2 vec        = GET_NEXT_VEC2(ARGS);
    xform->sca.x        = vec.x;
    xform->sca.y        = vec.y;
    RETURN->v_object    = SELF;
    CQ_PushCommand_SetScale(xform);
}

// Transformation API
// ===============================================================

CK_DLL_MFUN(ggen_local_pos_to_world_pos)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKVEC3 vec        = GET_NEXT_VEC3(ARGS);
    glm::vec3 worldPos
      = SG_Transform::worldMatrix(xform) * glm::vec4(vec.x, vec.y, vec.z, 1.0f);
    RETURN->v_vec3 = { worldPos.x, worldPos.y, worldPos.z };
}

// Scenegraph Relationship Impl
// ===============================================================
CK_DLL_GFUN(ggen_op_gruck)
{
    // get the arguments
    Chuck_Object* lhs = GET_NEXT_OBJECT(ARGS);
    Chuck_Object* rhs = GET_NEXT_OBJECT(ARGS);

    if (!lhs || !rhs) {
        std::string errMsg = std::string("in gruck operator: ")
                             + (lhs ? "LHS" : "[null]") + " --> "
                             + (rhs ? "RHS" : "[null]");
        // nullptr exception
        API->vm->throw_exception("NullPointerException", errMsg.c_str(), SHRED);
        return;
    }

    // get internal representation
    SG_Transform* lhs_xform
      = SG_GetTransform(OBJ_MEMBER_UINT(lhs, component_offset_id));
    SG_Transform* rhs_xform
      = SG_GetTransform(OBJ_MEMBER_UINT(rhs, component_offset_id));

    // command
    CQ_PushCommand_AddChild(rhs_xform, lhs_xform);

    // return RHS
    RETURN->v_object = rhs;
}

CK_DLL_GFUN(ggen_op_ungruck)
{
    // get the arguments
    Chuck_Object* lhs = GET_NEXT_OBJECT(ARGS);
    Chuck_Object* rhs = GET_NEXT_OBJECT(ARGS);

    // get internal
    SG_Transform* lhs_xform
      = SG_GetTransform(OBJ_MEMBER_UINT(lhs, component_offset_id));
    SG_Transform* rhs_xform
      = SG_GetTransform(OBJ_MEMBER_UINT(rhs, component_offset_id));

    // command
    CQ_PushCommand_RemoveChild(rhs_xform, lhs_xform);

    // return RHS
    RETURN->v_object = rhs;
}

CK_DLL_MFUN(ggen_detach_parent)
{
    SG_Transform* child  = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    SG_Transform* parent = SG_GetTransform(child->parentID);
    if (!parent) return;

    CQ_PushCommand_RemoveChild(parent, child);
}

CK_DLL_MFUN(ggen_detach_children)
{
    SG_Transform* parent = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    SG_Transform::removeAllChildren(parent);

    CQ_PushCommand_RemoveAllChildren(parent);
}

CK_DLL_MFUN(ggen_detach_all)
{
    SG_Transform* child  = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    SG_Transform* parent = SG_GetTransform(child->parentID);

    CQ_PushCommand_RemoveChild(parent, child);
    CQ_PushCommand_RemoveAllChildren(child);
}

CK_DLL_MFUN(ggen_get_parent)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));

    SG_Component* parent = SG_GetComponent(xform->parentID);

    RETURN->v_object = parent ? parent->ckobj : NULL;
}

CK_DLL_MFUN(ggen_get_child_default)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    SG_Transform* child = SG_Transform::child(xform, 0);
    RETURN->v_object    = child ? child->ckobj : NULL;
}

CK_DLL_MFUN(ggen_get_child)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    SG_Transform* child = SG_Transform::child(xform, GET_NEXT_INT(ARGS));
    RETURN->v_object    = child ? child->ckobj : NULL;

    // index warning
    // API->vm->em_log(1, "Warning: GGen::child() index out of bounds!\n");
}

CK_DLL_MFUN(ggen_get_num_children)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    RETURN->v_int       = SG_Transform::numChildren(xform);
}

CK_DLL_MFUN(ggen_get_scene)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    SG_Scene* scene     = SG_GetScene(xform->scene_id);
    RETURN->v_object    = scene ? scene->ckobj : NULL;
}

// ===============================================================
// GMesh and friends
// ===============================================================

CK_DLL_CTOR(gmesh_ctor);
CK_DLL_CTOR(gmesh_ctor_params);

CK_DLL_MFUN(gmesh_get_mat);
CK_DLL_MFUN(gmesh_set_mat);
CK_DLL_MFUN(gmesh_get_geo);
CK_DLL_MFUN(gmesh_set_geo);
CK_DLL_MFUN(gmesh_set_mat_and_geo);

CK_DLL_CTOR(glines2d_ctor);
CK_DLL_MFUN(glines2d_set_line_positions);
CK_DLL_MFUN(glines2d_set_line_positions_with_length);
CK_DLL_MFUN(glines2d_set_line_colors);
CK_DLL_MFUN(glines2d_set_width);
CK_DLL_MFUN(glines2d_set_color);
CK_DLL_MFUN(glines2d_get_width);
CK_DLL_MFUN(glines2d_get_color);
// CK_DLL_MFUN(glines2d_get_extrusion);
// CK_DLL_MFUN(glines2d_get_loop);
// CK_DLL_MFUN(glines2d_set_extrusion);
// CK_DLL_MFUN(glines2d_set_loop);

/*
basic:

globals
- color
- size
- texture

per-point
- color,
- pointsize,

*/
CK_DLL_CTOR(gpoints_ctor);

// points geo (per point attribs)
CK_DLL_MFUN(gpoints_geo_set_positions);
CK_DLL_MFUN(gpoints_geo_set_colors);
CK_DLL_MFUN(gpoints_geo_set_sizes);

// points mat
CK_DLL_MFUN(gpoints_mat_set_color);
CK_DLL_MFUN(gpoints_mat_set_size);
CK_DLL_MFUN(gpoints_mat_set_sampler);
CK_DLL_MFUN(gpoints_mat_set_texture);
CK_DLL_MFUN(gpoints_mat_get_color);
CK_DLL_MFUN(gpoints_mat_get_size);
CK_DLL_MFUN(gpoints_mat_get_sampler);
CK_DLL_MFUN(gpoints_mat_get_texture);
CK_DLL_MFUN(gpoints_mat_get_billboard);
CK_DLL_MFUN(gpoints_mat_set_billboard);

CK_DLL_CTOR(gplane_ctor);
CK_DLL_MFUN(gplane_get_geo);
CK_DLL_CTOR(gcircle_ctor);
CK_DLL_CTOR(gcube_ctor);
CK_DLL_CTOR(gsphere_ctor);
CK_DLL_CTOR(gtorus_ctor);
CK_DLL_CTOR(gcylinder_ctor);
CK_DLL_CTOR(gknot_ctor);
CK_DLL_CTOR(gsuzanne_ctor);
// GShape phong material fns
CK_DLL_MFUN(gshape_phong_material_get_specular_color);
CK_DLL_MFUN(gshape_phong_material_set_specular_color);
CK_DLL_MFUN(gshape_phong_material_get_diffuse_color);
CK_DLL_MFUN(gshape_phong_material_set_diffuse_color);
CK_DLL_MFUN(gshape_phong_material_get_alpha);
CK_DLL_MFUN(gshape_phong_material_set_alpha);
CK_DLL_MFUN(gshape_phong_material_get_log_shininess);
CK_DLL_MFUN(gshape_phong_material_set_log_shininess);
CK_DLL_MFUN(gshape_phong_material_get_emission_color);
CK_DLL_MFUN(gshape_phong_material_set_emission_color);
CK_DLL_MFUN(gshape_phong_material_get_normal_factor);
CK_DLL_MFUN(gshape_phong_material_set_normal_factor);
CK_DLL_MFUN(gshape_phong_material_get_ao_factor);
CK_DLL_MFUN(gshape_phong_material_set_ao_factor);
CK_DLL_MFUN(gshape_phong_material_get_albedo_tex);
CK_DLL_MFUN(gshape_phong_material_set_albedo_tex);
CK_DLL_MFUN(gshape_phong_material_get_specular_tex);
CK_DLL_MFUN(gshape_phong_material_set_specular_tex);
CK_DLL_MFUN(gshape_phong_material_get_ao_tex);
CK_DLL_MFUN(gshape_phong_material_set_ao_tex);
CK_DLL_MFUN(gshape_phong_material_get_emissive_tex);
CK_DLL_MFUN(gshape_phong_material_set_emissive_tex);
CK_DLL_MFUN(gshape_phong_material_get_normal_tex);
CK_DLL_MFUN(gshape_phong_material_set_normal_tex);

// phong envmap
CK_DLL_MFUN(gshape_phong_material_get_envmap_method);
CK_DLL_MFUN(gshape_phong_material_set_envmap_method);
CK_DLL_MFUN(gshape_phong_material_get_envmap_refraction_ratio);
CK_DLL_MFUN(gshape_phong_material_set_envmap_refraction_ratio);
CK_DLL_MFUN(gshape_phong_material_get_envmap_blend_mode);
CK_DLL_MFUN(gshape_phong_material_set_envmap_blend_mode);
CK_DLL_MFUN(gshape_phong_material_get_envmap_intensity);
CK_DLL_MFUN(gshape_phong_material_set_envmap_intensity);

static void ulib_mesh_query(Chuck_DL_Query* QUERY)
{
    // GMesh -----------------------------------------------------
    QUERY->begin_class(QUERY, SG_CKNames[SG_COMPONENT_MESH],
                       SG_CKNames[SG_COMPONENT_TRANSFORM]);
    DOC_CLASS(
      "Mesh class for renderables. Can be assigned Geometry and Material components");

    QUERY->add_ctor(QUERY, gmesh_ctor);
    QUERY->add_ctor(QUERY, gmesh_ctor_params);
    QUERY->add_arg(QUERY, SG_CKNames[SG_COMPONENT_GEOMETRY], "geo");
    QUERY->add_arg(QUERY, SG_CKNames[SG_COMPONENT_MATERIAL], "mat");

    MFUN(gmesh_get_mat, SG_CKNames[SG_COMPONENT_MATERIAL], "material");
    DOC_FUNC("Get the material of this GMesh");

    MFUN(gmesh_set_mat, "GMesh", "material");
    ARG(SG_CKNames[SG_COMPONENT_MATERIAL], "material");
    DOC_FUNC("Set the material of this GMesh");

    MFUN(gmesh_get_mat, SG_CKNames[SG_COMPONENT_MATERIAL], "mat");
    DOC_FUNC("Get the material of this GMesh (shorthand for backwards compatibility)");

    MFUN(gmesh_set_mat, "GMesh", "mat");
    ARG(SG_CKNames[SG_COMPONENT_MATERIAL], "material");
    DOC_FUNC("Set the material of this GMesh (shorthand for backwards compatibility)");

    MFUN(gmesh_get_geo, SG_CKNames[SG_COMPONENT_GEOMETRY], "geometry");
    DOC_FUNC("Get the geometry of this GMesh");

    MFUN(gmesh_set_geo, "GMesh", "geometry");
    ARG(SG_CKNames[SG_COMPONENT_GEOMETRY], "geometry");
    DOC_FUNC("Set the geometry of this GMesh");

    MFUN(gmesh_get_geo, SG_CKNames[SG_COMPONENT_GEOMETRY], "geo");
    DOC_FUNC("Get the geometry of this GMesh (shorthand for backwards compatibility)");

    MFUN(gmesh_set_geo, "GMesh", "geo");
    ARG(SG_CKNames[SG_COMPONENT_GEOMETRY], "geometry");
    DOC_FUNC("Set the geometry of this GMesh (shorthand for backwards compatibility)");

    MFUN(gmesh_set_mat_and_geo, "void", "mesh");
    ARG(SG_CKNames[SG_COMPONENT_GEOMETRY], "geometry");
    ARG(SG_CKNames[SG_COMPONENT_MATERIAL], "material");
    DOC_FUNC("Set the geometry and material of this GMesh");

    // shadow API
    MFUN(gmesh_get_shadowed, "int", "shadowed");
    DOC_FUNC("Return whether this mesh can receive shadows.");

    MFUN(gmesh_set_shadowed, "void", "shadowed");
    ARG("int", "receives_shadow");
    DOC_FUNC(
      "Set whether this mesh can receive shadows. Note: setting this flag alone "
      "will not cause the mesh to be shadowed. This will only pass a flag to the "
      "shader of mesh.material(), and it is up to the shader to apply shadowing "
      "effects. To set an entire subgraph, see GGen.shadowed(int, int)");

    // abstract class, no destructor or constructor
    QUERY->end_class(QUERY);

    // GLines2D -----------------------------------------------------
    {
        BEGIN_CLASS("GLines", SG_CKNames[SG_COMPONENT_MESH]);
        DOC_CLASS("Class for creating 2D lines");
        ADD_EX("basic/circles.ck");
        ADD_EX("basic/glines.ck");
        ADD_EX("basic/lissajous.ck");
        ADD_EX("deep/sndpeek.ck");
        ADD_EX("deep/sndpeek_minimal.ck");
        ADD_EX("deep/soundbulb.ck");
        ADD_EX("deep/fish.ck");

        CTOR(glines2d_ctor);

        MFUN(glines2d_set_line_positions, "void", "positions");
        ARG("vec2[]", "points");
        DOC_FUNC(
          "Set the line positions. Z values are fixed to 0.0. Equivalent to "
          "LinesGeometry.linePoints()");

        MFUN(glines2d_set_line_positions_with_length, "void", "positions");
        ARG("vec2[]", "points");
        ARG("int", "length");
        DOC_FUNC(
          "Set line positions from the first `length` values in the `points` array.");

        MFUN(glines2d_set_line_colors, "void", "colors");
        ARG("vec3[]", "colors");
        DOC_FUNC(
          "Set the line colors. If the array is shorter than the number of lines, "
          "the last color is used for the remaining lines. Equivalent to "
          "LinesGeometry.lineColors()");

        MFUN(glines2d_set_width, "void", "width");
        ARG("float", "width");
        DOC_FUNC("Set the thickness of the lines in the material.");

        MFUN(glines2d_get_width, "float", "width");
        DOC_FUNC("Get the thickness of the lines in the material.");

        MFUN(glines2d_set_color, "void", "color");
        ARG("vec3", "color");
        DOC_FUNC("Set the line color");

        MFUN(glines2d_get_color, "vec3", "color");
        DOC_FUNC("Get the line color");

        // MFUN(glines2d_set_extrusion, "void", "extrusion");
        // ARG("float", "extrusion");
        // DOC_FUNC(
        //   "Set the miter extrusion ratio of the line. Varies from 0.0 to 1.0. A value
        //   " "of " "0.5 means the line width is split evenly on each side of each line
        //   segment " "position.");

        // MFUN(glines2d_get_extrusion, "float", "extrusion");
        // DOC_FUNC("Get the miter extrusion ratio of the line.");

        // MFUN(glines2d_set_loop, "void", "loop");
        // ARG("int", "loop");
        // DOC_FUNC(
        //   "Set whether the line segments form a closed loop. Set via
        //   GLines.loop(true) " "or GLines.loop(false)");

        // MFUN(glines2d_get_loop, "int", "loop");
        // DOC_FUNC("Get whether the line segments form a closed loop.");

        END_CLASS();
    } // GLines

    { // Gpoints -------------------------------------------------
        BEGIN_CLASS("GPoints", SG_CKNames[SG_COMPONENT_MESH]);
        DOC_CLASS("Class for creating instanced points in 3D space");
        ADD_EX("basic/gpoints.ck");
        ADD_EX("basic/points.ck");

        CTOR(gpoints_ctor);

        MFUN(gpoints_geo_set_positions, "void", "positions");
        ARG("vec3[]", "positions");
        DOC_FUNC("Set positions of the points");

        MFUN(gpoints_geo_set_colors, "void", "colors");
        ARG("vec3[]", "colors");
        DOC_FUNC(
          "Set per-point colors. If there are more points than colors, extra points "
          "cycle through the colors. The final color is multiplied by the global point "
          "color, GPoints.color()");

        MFUN(gpoints_geo_set_sizes, "void", "sizes");
        ARG("float[]", "sizes");
        DOC_FUNC(
          "Set per-point sizes. If there are more points than sizes, extra points "
          "cycle through the sizes. The final size is multiplied by the global point "
          "size, GPoints.size()");

        MFUN(gpoints_mat_set_color, "void", "color");
        ARG("vec3", "color");
        DOC_FUNC("Set the global color of the points");

        MFUN(gpoints_mat_get_color, "vec3", "color");
        DOC_FUNC("Get the global color of the points");

        MFUN(gpoints_mat_set_size, "void", "size");
        ARG("float", "size");
        DOC_FUNC(
          "Set the global size of the points (in world-space units, a size of 1 means "
          "each point is 1 unit wide)");

        MFUN(gpoints_mat_get_size, "float", "size");
        DOC_FUNC("Get the global size of the points");

        MFUN(gpoints_mat_set_sampler, "void", "sampler");
        ARG("TextureSampler", "sampler");
        DOC_FUNC("Set the sampler used for the point texture");

        MFUN(gpoints_mat_get_sampler, "TextureSampler", "sampler");
        DOC_FUNC("Get the sampler used for the point texture");

        MFUN(gpoints_mat_set_texture, "void", "texture");
        ARG("Texture", "texture");
        DOC_FUNC("Set the texture applied to each point");

        MFUN(gpoints_mat_get_texture, "Texture", "texture");
        DOC_FUNC("Get the texture applied to each point ");

        MFUN(gpoints_mat_set_billboard, "void", "billboard");
        ARG("int", "billboard");
        DOC_FUNC(
          "Set whether the points should always face the camera (billboard mode). 0 "
          "for no, 1 for yes");

        MFUN(gpoints_mat_get_billboard, "int", "billboard");
        DOC_FUNC("Get whether the points should always face the camera");

        END_CLASS();
    }

    { // GPlane -----------------------------------------------------
        BEGIN_CLASS("GPlane", SG_CKNames[SG_COMPONENT_MESH]);
        DOC_CLASS(
          "Convenience class for creating a GMesh(PlaneGeometry, PhongMaterial)");
        ADD_EX("basic/ggen_primitives.ck");
        ADD_EX("sequencers/drum_machine.ck");
        ADD_EX("sequencers/music_for_airports.ck");

        CTOR(gplane_ctor);

        PHONG_MATERIAL_METHODS(gshape_phong);

        END_CLASS();
    }

    { // GCircle -----------------------------------------------------
        BEGIN_CLASS("GCircle", SG_CKNames[SG_COMPONENT_MESH]);
        DOC_CLASS(
          "Convenience class for creating a GMesh(CircleGeometry, PhongMaterial)");
        ADD_EX("basic/ggen_primitives.ck");

        CTOR(gcircle_ctor);

        PHONG_MATERIAL_METHODS(gshape_phong);

        END_CLASS();
    }

    { // GSphere -----------------------------------------------------
        BEGIN_CLASS("GSphere", SG_CKNames[SG_COMPONENT_MESH]);
        DOC_CLASS(
          "Convenience class for creating a GMesh(SphereGeometry, PhongMaterial)");
        ADD_EX("basic/ggen_primitives.ck");
        ADD_EX("basic/orbits.ck");

        CTOR(gsphere_ctor);

        PHONG_MATERIAL_METHODS(gshape_phong);

        END_CLASS();
    }

    {
        BEGIN_CLASS("GTorus", SG_CKNames[SG_COMPONENT_MESH]);
        DOC_CLASS(
          "Convenience class for creating a GMesh(TorusGeometry, PhongMaterial)");
        ADD_EX("basic/ggen_primitives.ck");

        CTOR(gtorus_ctor);

        PHONG_MATERIAL_METHODS(gshape_phong);

        END_CLASS();
    }

    {
        BEGIN_CLASS("GCylinder", SG_CKNames[SG_COMPONENT_MESH]);
        DOC_CLASS(
          "Convenience class for creating a GMesh(CylinderGeometry, PhongMaterial)");
        ADD_EX("basic/ggen_primitives.ck");

        CTOR(gcylinder_ctor);

        PHONG_MATERIAL_METHODS(gshape_phong);

        END_CLASS();
    }

    {
        BEGIN_CLASS("GKnot", SG_CKNames[SG_COMPONENT_MESH]);
        DOC_CLASS(
          "Convenience class for creating a GMesh(KnotGeometry, PhongMaterial)");
        ADD_EX("basic/ggen_primitives.ck");

        CTOR(gknot_ctor);

        PHONG_MATERIAL_METHODS(gshape_phong);

        END_CLASS();
    }

    {
        BEGIN_CLASS("GSuzanne", SG_CKNames[SG_COMPONENT_MESH]);
        DOC_CLASS(
          "Convenience class for creating a GMesh(SuzanneGeometry, PhongMaterial)");
        ADD_EX("basic/ggen_primitives.ck");

        CTOR(gsuzanne_ctor);

        PHONG_MATERIAL_METHODS(gshape_phong);

        END_CLASS();
    }

    {
        BEGIN_CLASS("GCube", SG_CKNames[SG_COMPONENT_MESH]);
        DOC_CLASS(
          "Convenience class for creating a GMesh(CubeGeometry, PhongMaterial)");
        ADD_EX("basic/ggen_primitives.ck");
        ADD_EX("basic/jello.ck");

        CTOR(gcube_ctor);

        PHONG_MATERIAL_METHODS(gshape_phong);

        END_CLASS();
    }
}

SG_Transform* ulib_ggen_create(Chuck_Object* ckobj, Chuck_VM_Shred* shred)
{
    CK_DL_API API = g_chuglAPI;

    if (ckobj == NULL) {
        ckobj = chugin_createCkObj(SG_CKNames[SG_COMPONENT_TRANSFORM], false, shred);
    }

    // execute change on audio thread side
    SG_Transform* xform = SG_CreateTransform(ckobj);
    // save SG_ID
    OBJ_MEMBER_UINT(ckobj, component_offset_id) = xform->id;

    CQ_PushCommand_CreateTransform(xform);

    return xform;
}

SG_Mesh* ulib_mesh_create(Chuck_Object* mesh_ckobj, SG_Geometry* geo, SG_Material* mat,
                          Chuck_VM_Shred* shred)
{
    CK_DL_API API = g_chuglAPI;

    if (mesh_ckobj == NULL) {
        mesh_ckobj = chugin_createCkObj(SG_CKNames[SG_COMPONENT_MESH], false, shred);
    }

    SG_Mesh* mesh = SG_CreateMesh(mesh_ckobj, geo, mat);
    ASSERT(mesh->type == SG_COMPONENT_MESH);

    OBJ_MEMBER_UINT(mesh_ckobj, component_offset_id) = mesh->id;

    CQ_PushCommand_MeshUpdate(mesh);
    return mesh;
}

CK_DLL_CTOR(gmesh_ctor)
{
    ulib_mesh_create(SELF, NULL, NULL, SHRED);
}

CK_DLL_CTOR(gmesh_ctor_params)
{
    Chuck_Object* ck_geo = GET_NEXT_OBJECT(ARGS);
    Chuck_Object* ck_mat = GET_NEXT_OBJECT(ARGS);

    SG_Geometry* sg_geo = SG_GetGeometry(OBJ_MEMBER_UINT(ck_geo, component_offset_id));
    SG_Material* sg_mat = SG_GetMaterial(OBJ_MEMBER_UINT(ck_mat, component_offset_id));

    ulib_mesh_create(SELF, sg_geo, sg_mat, SHRED);
}

CK_DLL_MFUN(gmesh_get_mat)
{
    SG_Mesh* mesh    = GET_MESH(SELF);
    SG_Material* mat = SG_GetMaterial(mesh->_mat_id);
    RETURN->v_object = mat ? mat->ckobj : NULL;
}

CK_DLL_MFUN(gmesh_set_mat)
{
    Chuck_Object* ck_mat = GET_NEXT_OBJECT(ARGS);

    SG_Mesh* mesh = GET_MESH(SELF);
    SG_Material* mat
      = ck_mat ? SG_GetMaterial(OBJ_MEMBER_UINT(ck_mat, component_offset_id)) : NULL;
    SG_Mesh::setMaterial(mesh, mat);
    CQ_PushCommand_MeshUpdate(mesh);
}

CK_DLL_MFUN(gmesh_get_geo)
{
    SG_Mesh* mesh    = GET_MESH(SELF);
    SG_Geometry* geo = SG_GetGeometry(mesh->_geo_id);
    RETURN->v_object = geo ? geo->ckobj : NULL;
}

CK_DLL_MFUN(gmesh_set_geo)
{
    Chuck_Object* ck_geo = GET_NEXT_OBJECT(ARGS);

    SG_Mesh* mesh = GET_MESH(SELF);
    SG_Geometry* geo
      = ck_geo ? SG_GetGeometry(OBJ_MEMBER_UINT(ck_geo, component_offset_id)) : NULL;
    SG_Mesh::setGeometry(mesh, geo);
    CQ_PushCommand_MeshUpdate(mesh);
}

CK_DLL_MFUN(gmesh_set_mat_and_geo)
{
    Chuck_Object* ck_geo = GET_NEXT_OBJECT(ARGS);
    Chuck_Object* ck_mat = GET_NEXT_OBJECT(ARGS);

    SG_Mesh* mesh = GET_MESH(SELF);
    SG_Geometry* geo
      = ck_geo ? SG_GetGeometry(OBJ_MEMBER_UINT(ck_geo, component_offset_id)) : NULL;
    SG_Material* mat
      = ck_mat ? SG_GetMaterial(OBJ_MEMBER_UINT(ck_mat, component_offset_id)) : NULL;
    SG_Mesh::setGeometry(mesh, geo);
    SG_Mesh::setMaterial(mesh, mat);
    CQ_PushCommand_MeshUpdate(mesh);
}

CK_DLL_MFUN(gmesh_get_shadowed)
{
    SG_Mesh* mesh = GET_MESH(SELF);
    RETURN->v_int = mesh->receives_shadows;
}

CK_DLL_MFUN(gmesh_set_shadowed)
{
    SG_Mesh* mesh = GET_MESH(SELF);
    CQ_PushCommand_MeshSetShadowed(mesh, GET_NEXT_OBJECT(ARGS) ? 1 : 0);
}

CK_DLL_MFUN(ggen_set_shadowed_all_children)
{
    SG_Transform* xform = SG_GetTransform(OBJ_MEMBER_UINT(SELF, component_offset_id));
    b32 shadowed        = GET_NEXT_INT(ARGS) ? 1 : 0;
    b32 add_children    = GET_NEXT_INT(ARGS) ? 1 : 0;

    if (xform->type == SG_COMPONENT_MESH) {
        CQ_PushCommand_MeshSetShadowed(xform, shadowed);
    }

    if (add_children) { // BFS add all children
        u64 curr = audio_frame_arena.curr;
        memcpy(Arena::push(&audio_frame_arena, xform->childrenIDs.curr),
               xform->childrenIDs.base, xform->childrenIDs.curr);

        while (curr != audio_frame_arena.curr) {
            xform = SG_GetTransform(*(SG_ID*)Arena::get(&audio_frame_arena, curr));
            ASSERT(xform);
            CQ_PushCommand_MeshSetShadowed(xform, shadowed);
            curr += sizeof(SG_ID);

            memcpy(Arena::push(&audio_frame_arena, xform->childrenIDs.curr),
                   xform->childrenIDs.base, xform->childrenIDs.curr);
        }
    }
}

// GLines2D ===============================================================

static void ulib_mesh_create_gshape(Chuck_Object* ckobj, SG_GeometryType geo_type,
                                    SG_MaterialType mat_type, Chuck_VM_Shred* shred)
{
    CK_DL_API API    = g_chuglAPI;
    SG_Mesh* mesh    = GET_MESH(ckobj);
    SG_Geometry* geo = ulib_geometry_create(geo_type, shred);
    SG_Material* mat = ulib_material_create(mat_type, shred);
    SG_Mesh::setGeometry(mesh, geo);
    SG_Mesh::setMaterial(mesh, mat);
    CQ_PushCommand_MeshUpdate(mesh);
}

CK_DLL_CTOR(glines2d_ctor)
{
    ulib_mesh_create_gshape(SELF, SG_GEOMETRY_LINES2D, SG_MATERIAL_LINES2D, SHRED);
}

CK_DLL_MFUN(glines2d_set_line_positions)
{
    SG_Mesh* mesh        = GET_MESH(SELF);
    Chuck_Object* ck_arr = GET_NEXT_OBJECT(ARGS);
    SG_Geometry* geo     = SG_GetGeometry(mesh->_geo_id);
    ulib_geo_lines2d_set_lines_points(geo, ck_arr);
}

CK_DLL_MFUN(glines2d_set_line_positions_with_length)
{
    SG_Mesh* mesh        = GET_MESH(SELF);
    Chuck_Object* ck_arr = GET_NEXT_OBJECT(ARGS);
    t_CKINT length       = GET_NEXT_INT(ARGS);
    SG_Geometry* geo     = SG_GetGeometry(mesh->_geo_id);
    ulib_geo_lines2d_set_lines_points(geo, ck_arr, length);
}

CK_DLL_MFUN(glines2d_set_line_colors)
{
    SG_Mesh* mesh        = GET_MESH(SELF);
    Chuck_Object* ck_arr = GET_NEXT_OBJECT(ARGS);
    SG_Geometry* geo     = SG_GetGeometry(mesh->_geo_id);
    ulib_geo_lines2d_set_line_colors(geo, ck_arr);
}

CK_DLL_MFUN(glines2d_set_width)
{
    SG_Mesh* mesh         = GET_MESH(SELF);
    SG_Material* material = SG_GetMaterial(mesh->_mat_id); // get the material
    t_CKFLOAT width       = GET_NEXT_FLOAT(ARGS);

    // TODO find better way to connect this with ulib_material impl
    SG_Material::uniformFloat(material, 0, (f32)width);
    CQ_PushCommand_MaterialSetUniform(material, 0);
}

CK_DLL_MFUN(glines2d_set_extrusion)
{
    SG_Mesh* mesh         = GET_MESH(SELF);
    SG_Material* material = SG_GetMaterial(mesh->_mat_id); // get the material
    t_CKFLOAT extrusion   = GET_NEXT_FLOAT(ARGS);

    SG_Material::uniformFloat(material, 3, (f32)extrusion);
    CQ_PushCommand_MaterialSetUniform(material, 3);
}

CK_DLL_MFUN(glines2d_set_loop)
{
    SG_Mesh* mesh         = GET_MESH(SELF);
    SG_Material* material = SG_GetMaterial(mesh->_mat_id); // get the material
    t_CKINT loop          = GET_NEXT_INT(ARGS);

    SG_Material::uniformInt(material, 2, loop ? 1 : 0);
    CQ_PushCommand_MaterialSetUniform(material, 2);
}

CK_DLL_MFUN(glines2d_set_color)
{
    SG_Mesh* mesh         = GET_MESH(SELF);
    SG_Material* material = SG_GetMaterial(mesh->_mat_id); // get the material
    t_CKVEC3 color        = GET_NEXT_VEC3(ARGS);

    SG_Material::uniformVec3f(material, 1, glm::vec3(color.x, color.y, color.z));
    CQ_PushCommand_MaterialSetUniform(material, 1);
}

CK_DLL_MFUN(glines2d_get_width)
{
    SG_Material* material = GET_MESH_MATERIAL(SELF);
    RETURN->v_float       = material ? material->uniforms[0].as.f : 0.0f;
}

CK_DLL_MFUN(glines2d_get_extrusion)
{
    SG_Material* material = GET_MESH_MATERIAL(SELF);
    RETURN->v_float       = material ? material->uniforms[3].as.f : 0.0f;
}

CK_DLL_MFUN(glines2d_get_loop)
{
    SG_Material* material = GET_MESH_MATERIAL(SELF);
    RETURN->v_int         = material ? material->uniforms[2].as.i : 0;
}

CK_DLL_MFUN(glines2d_get_color)
{
    SG_Material* material = GET_MESH_MATERIAL(SELF);
    RETURN->v_vec3        = {};
    if (material) {
        glm::vec3 color = material->uniforms[1].as.vec3f;
        RETURN->v_vec3  = { color.x, color.y, color.z };
    }
}

// GPoints ===============================================================

CK_DLL_CTOR(gpoints_ctor)
{
    ulib_mesh_create_gshape(SELF, SG_GEOMETRY, SG_MATERIAL_CUSTOM, SHRED);

    // initialize geo and mat manually here (not exposing to chugl API for now)
    SG_Mesh* mesh = GET_MESH(SELF);
    ASSERT(mesh);
    SG_Material* material = SG_GetMaterial(mesh->_mat_id);
    SG_Geometry* geo      = SG_GetGeometry(mesh->_geo_id);

    { // init geometry data
        CQ_PushCommand_GeometrySetVertexCount(geo, 0);

        static f32 plane_vertex_data[] = {
            // x,   y,    z,     uvx, uvy
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
            0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
            0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right

            0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
            -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, // top left
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        };

        ulib_geo_set_pulled_vertex_attribute_data(geo, 0, plane_vertex_data,
                                                  ARRAY_LENGTH(plane_vertex_data));

        f32 plane_init_color[] = { 1.0f, 1.0f, 1.0f };
        ulib_geo_set_pulled_vertex_attribute_data(geo, 1, plane_init_color,
                                                  ARRAY_LENGTH(plane_init_color));

        f32 plane_init_sizes[] = { 1.0f };
        ulib_geo_set_pulled_vertex_attribute_data(geo, 2, plane_init_sizes,
                                                  ARRAY_LENGTH(plane_init_sizes));

        f32 plane_init_pos[] = { 0.0f, 0.0f, 0.0f };
        ulib_geo_set_pulled_vertex_attribute_data(geo, 3, plane_init_pos,
                                                  ARRAY_LENGTH(plane_init_pos));
    }

    { // init material data

        SG_Shader* shader = SG_GetShader(g_material_builtin_shaders.points_shader_id);
        ulib_material_set_shader(material, shader);

        SG_Material::uniformVec4f(material, 0, glm::vec4(1.0f)); // global point color
        SG_Material::uniformFloat(material, 1, .05f);            // global point size
        SG_Material::setSampler(material, 2,
                                SG_SAMPLER_DEFAULT); // point sampler
        SG_Material::setTexture(
          material, 3,
          SG_GetTexture(g_builtin_textures.white_pixel_id)); // point texture
        SG_Material::uniformInt(material, 4, true);          // billboard mode

        ulib_material_cq_update_all_uniforms(material);
    }
}

CK_DLL_MFUN(gpoints_geo_set_positions)
{
    Chuck_ArrayVec3* ckarr = GET_NEXT_VEC3_ARRAY(ARGS);
    int len                = API->object->array_vec3_size(ckarr);
    SG_Geometry* geo       = GET_MESH_GEOMETRY(SELF);

    geoSetPulledVertexAttribute(geo, 3, (Chuck_Object*)ckarr, 3, false);

    CQ_PushCommand_GeometrySetVertexCount(geo, len * 6);
}

CK_DLL_MFUN(gpoints_geo_set_colors)
{
    geoSetPulledVertexAttribute(GET_MESH_GEOMETRY(SELF), 1, GET_NEXT_OBJECT(ARGS), 3,
                                false);
}

CK_DLL_MFUN(gpoints_geo_set_sizes)
{
    geoSetPulledVertexAttribute(GET_MESH_GEOMETRY(SELF), 2, GET_NEXT_OBJECT(ARGS), 1,
                                false);
}

CK_DLL_MFUN(gpoints_mat_set_color)
{
    SG_Material* material = GET_MESH_MATERIAL(SELF);
    t_CKVEC3 color        = GET_NEXT_VEC3(ARGS);
    SG_Material::uniformVec4f(material, 0, glm::vec4(color.x, color.y, color.z, 1.0f));
    CQ_PushCommand_MaterialSetUniform(material, 0);
}

CK_DLL_MFUN(gpoints_mat_set_size)
{
    SG_Material* material = GET_MESH_MATERIAL(SELF);
    SG_Material::uniformFloat(material, 1, GET_NEXT_FLOAT(ARGS));
    CQ_PushCommand_MaterialSetUniform(material, 1);
}

CK_DLL_MFUN(gpoints_mat_set_sampler)
{
    SG_Material* material = GET_MESH_MATERIAL(SELF);
    SG_Material::setSampler(material, 2, SG_Sampler::fromCkObj(GET_NEXT_OBJECT(ARGS)));
    CQ_PushCommand_MaterialSetUniform(material, 2);
}

CK_DLL_MFUN(gpoints_mat_set_texture)
{
    SG_Material* material = GET_MESH_MATERIAL(SELF);
    Chuck_Object* ckobj   = GET_NEXT_OBJECT(ARGS);
    SG_Texture* tex       = ckobj ?
                              SG_GetTexture(OBJ_MEMBER_UINT(ckobj, component_offset_id)) :
                              SG_GetTexture(g_builtin_textures.white_pixel_id);
    SG_Material::setTexture(material, 3, tex);
    CQ_PushCommand_MaterialSetUniform(material, 3);
}

CK_DLL_MFUN(gpoints_mat_get_color)
{
    glm::vec3 color = GET_MESH_MATERIAL(SELF)->uniforms[0].as.vec4f;
    RETURN->v_vec3  = { color.r, color.g, color.b };
}

CK_DLL_MFUN(gpoints_mat_get_size)
{
    RETURN->v_float = GET_MESH_MATERIAL(SELF)->uniforms[1].as.f;
}

CK_DLL_MFUN(gpoints_mat_get_sampler)
{
    RETURN->v_object = ulib_texture_ckobj_from_sampler(
      GET_MESH_MATERIAL(SELF)->uniforms[2].as.sampler, false, SHRED);
}

CK_DLL_MFUN(gpoints_mat_get_texture)
{
    SG_Texture* tex = SG_GetTexture(GET_MESH_MATERIAL(SELF)->uniforms[3].as.texture_id);
    RETURN->v_object = tex ? tex->ckobj : NULL;
}

CK_DLL_MFUN(gpoints_mat_get_billboard)
{
    RETURN->v_int = GET_MESH_MATERIAL(SELF)->uniforms[4].as.i;
}

CK_DLL_MFUN(gpoints_mat_set_billboard)
{
    SG_Material* material = GET_MESH_MATERIAL(SELF);
    SG_Material::uniformInt(material, 4, GET_NEXT_INT(ARGS) ? 1 : 0);
    CQ_PushCommand_MaterialSetUniform(material, 4);
}

// GShapes ===============================================================

CK_DLL_CTOR(gplane_ctor)
{
    ulib_mesh_create_gshape(SELF, SG_GEOMETRY_PLANE, SG_MATERIAL_PHONG, SHRED);
}

CK_DLL_MFUN(gplane_get_geo)
{
    SG_Mesh* mesh    = GET_MESH(SELF);
    SG_Geometry* geo = SG_GetGeometry(mesh->_geo_id);
    RETURN->v_object = geo ? geo->ckobj : NULL;
}

CK_DLL_CTOR(gcircle_ctor)
{
    ulib_mesh_create_gshape(SELF, SG_GEOMETRY_CIRCLE, SG_MATERIAL_PHONG, SHRED);
}
CK_DLL_CTOR(gcube_ctor)
{
    ulib_mesh_create_gshape(SELF, SG_GEOMETRY_CUBE, SG_MATERIAL_PHONG, SHRED);
}
CK_DLL_CTOR(gsphere_ctor)
{
    ulib_mesh_create_gshape(SELF, SG_GEOMETRY_SPHERE, SG_MATERIAL_PHONG, SHRED);
}
CK_DLL_CTOR(gtorus_ctor)
{
    ulib_mesh_create_gshape(SELF, SG_GEOMETRY_TORUS, SG_MATERIAL_PHONG, SHRED);
}
CK_DLL_CTOR(gcylinder_ctor)
{
    ulib_mesh_create_gshape(SELF, SG_GEOMETRY_CYLINDER, SG_MATERIAL_PHONG, SHRED);
}
CK_DLL_CTOR(gknot_ctor)
{
    ulib_mesh_create_gshape(SELF, SG_GEOMETRY_KNOT, SG_MATERIAL_PHONG, SHRED);
}
CK_DLL_CTOR(gsuzanne_ctor)
{
    ulib_mesh_create_gshape(SELF, SG_GEOMETRY_SUZANNE, SG_MATERIAL_PHONG, SHRED);
}

// GShape phong material fns ============================================

CK_DLL_MFUN(gshape_phong_material_get_specular_color)
{
    glm::vec3 color = *PhongParams::specular(GET_MESH_MATERIAL(SELF));
    RETURN->v_vec3  = { color.r, color.g, color.b };
}

CK_DLL_MFUN(gshape_phong_material_set_specular_color)
{
    t_CKVEC3 color = GET_NEXT_VEC3(ARGS);
    PhongParams::specular(GET_MESH_MATERIAL(SELF),
                          glm::vec3(color.x, color.y, color.z));
}

CK_DLL_MFUN(gshape_phong_material_get_diffuse_color)
{
    glm::vec4 color = *PhongParams::diffuse(GET_MESH_MATERIAL(SELF));
    RETURN->v_vec3  = { color.r, color.g, color.b };
}

CK_DLL_MFUN(gshape_phong_material_set_diffuse_color)
{
    t_CKVEC3 color = GET_NEXT_VEC3(ARGS);
    PhongParams::diffuse(GET_MESH_MATERIAL(SELF), glm::vec3(color.x, color.y, color.z));
}

CK_DLL_MFUN(gshape_phong_material_get_alpha)
{
    RETURN->v_float = PhongParams::diffuse(GET_MESH_MATERIAL(SELF))->a;
}

CK_DLL_MFUN(gshape_phong_material_set_alpha)
{
    PhongParams::alpha(GET_MESH_MATERIAL(SELF), GET_NEXT_FLOAT(ARGS));
}

CK_DLL_MFUN(gshape_phong_material_get_log_shininess)
{
    RETURN->v_float = glm::log2(*PhongParams::shininess(GET_MESH_MATERIAL(SELF)));
}

CK_DLL_MFUN(gshape_phong_material_set_log_shininess)
{
    t_CKFLOAT shininess = GET_NEXT_FLOAT(ARGS);
    PhongParams::shininess(GET_MESH_MATERIAL(SELF), glm::pow(2.0f, (f32)shininess));
}

CK_DLL_MFUN(gshape_phong_material_get_emission_color)
{
    glm::vec3 color = *PhongParams::emission(GET_MESH_MATERIAL(SELF));
    RETURN->v_vec3  = { color.r, color.g, color.b };
}

CK_DLL_MFUN(gshape_phong_material_set_emission_color)
{
    t_CKVEC3 color = GET_NEXT_VEC3(ARGS);
    PhongParams::emission(GET_MESH_MATERIAL(SELF),
                          glm::vec3(color.x, color.y, color.z));
}

CK_DLL_MFUN(gshape_phong_material_get_normal_factor)
{
    RETURN->v_float = *PhongParams::normalFactor(GET_MESH_MATERIAL(SELF));
}

CK_DLL_MFUN(gshape_phong_material_set_normal_factor)
{
    PhongParams::normalFactor(GET_MESH_MATERIAL(SELF), GET_NEXT_FLOAT(ARGS));
}

CK_DLL_MFUN(gshape_phong_material_get_ao_factor)
{
    RETURN->v_float = *PhongParams::aoFactor(GET_MESH_MATERIAL(SELF));
}

CK_DLL_MFUN(gshape_phong_material_set_ao_factor)
{
    PhongParams::aoFactor(GET_MESH_MATERIAL(SELF), GET_NEXT_FLOAT(ARGS));
}

CK_DLL_MFUN(gshape_phong_material_get_albedo_tex)
{
    SG_Texture* tex  = PhongParams::albedoTex(GET_MESH_MATERIAL(SELF));
    RETURN->v_object = tex ? tex->ckobj : NULL;
}

CK_DLL_MFUN(gshape_phong_material_set_albedo_tex)
{
    Chuck_Object* ckobj = GET_NEXT_OBJECT(ARGS);
    SG_Texture* tex
      = ckobj ? SG_GetTexture(OBJ_MEMBER_UINT(ckobj, component_offset_id)) : NULL;
    PhongParams::albedoTex(GET_MESH_MATERIAL(SELF), tex);
}

CK_DLL_MFUN(gshape_phong_material_get_specular_tex)
{
    SG_Texture* tex  = PhongParams::specularTex(GET_MESH_MATERIAL(SELF));
    RETURN->v_object = tex ? tex->ckobj : NULL;
}

CK_DLL_MFUN(gshape_phong_material_set_specular_tex)
{
    Chuck_Object* ckobj = GET_NEXT_OBJECT(ARGS);
    SG_Texture* tex
      = ckobj ? SG_GetTexture(OBJ_MEMBER_UINT(ckobj, component_offset_id)) : NULL;
    PhongParams::specularTex(GET_MESH_MATERIAL(SELF), tex);
}

CK_DLL_MFUN(gshape_phong_material_get_ao_tex)
{
    SG_Texture* tex  = PhongParams::aoTex(GET_MESH_MATERIAL(SELF));
    RETURN->v_object = tex ? tex->ckobj : NULL;
}

CK_DLL_MFUN(gshape_phong_material_set_ao_tex)
{
    Chuck_Object* ckobj = GET_NEXT_OBJECT(ARGS);
    SG_Texture* tex
      = ckobj ? SG_GetTexture(OBJ_MEMBER_UINT(ckobj, component_offset_id)) : NULL;
    PhongParams::aoTex(GET_MESH_MATERIAL(SELF), tex);
}

CK_DLL_MFUN(gshape_phong_material_get_emissive_tex)
{
    SG_Texture* tex  = PhongParams::emissiveTex(GET_MESH_MATERIAL(SELF));
    RETURN->v_object = tex ? tex->ckobj : NULL;
}

CK_DLL_MFUN(gshape_phong_material_set_emissive_tex)
{
    Chuck_Object* ckobj = GET_NEXT_OBJECT(ARGS);
    SG_Texture* tex
      = ckobj ? SG_GetTexture(OBJ_MEMBER_UINT(ckobj, component_offset_id)) : NULL;
    PhongParams::emissiveTex(GET_MESH_MATERIAL(SELF), tex);
}

CK_DLL_MFUN(gshape_phong_material_get_normal_tex)
{
    SG_Texture* tex  = PhongParams::normalTex(GET_MESH_MATERIAL(SELF));
    RETURN->v_object = tex ? tex->ckobj : NULL;
}

CK_DLL_MFUN(gshape_phong_material_set_normal_tex)
{
    Chuck_Object* ckobj = GET_NEXT_OBJECT(ARGS);
    SG_Texture* tex
      = ckobj ? SG_GetTexture(OBJ_MEMBER_UINT(ckobj, component_offset_id)) : NULL;
    PhongParams::normalTex(GET_MESH_MATERIAL(SELF), tex);
}

CK_DLL_MFUN(gshape_phong_material_get_envmap_method)
{
    RETURN->v_int = *PhongParams::envmapMethod(GET_MESH_MATERIAL(SELF));
}

CK_DLL_MFUN(gshape_phong_material_set_envmap_method)
{
    PhongParams::envmapMethod(GET_MESH_MATERIAL(SELF),
                              (SG_EnvmapSampleMode)GET_NEXT_INT(ARGS));
}

CK_DLL_MFUN(gshape_phong_material_get_envmap_refraction_ratio)
{
    RETURN->v_float = *PhongParams::envmapRefractionRatio(GET_MESH_MATERIAL(SELF));
}

CK_DLL_MFUN(gshape_phong_material_set_envmap_refraction_ratio)
{
    PhongParams::envmapRefractionRatio(GET_MESH_MATERIAL(SELF), GET_NEXT_FLOAT(ARGS));
}

CK_DLL_MFUN(gshape_phong_material_get_envmap_blend_mode)
{
    RETURN->v_int = *PhongParams::envmapBlendMode(GET_MESH_MATERIAL(SELF));
}

CK_DLL_MFUN(gshape_phong_material_set_envmap_blend_mode)
{
    PhongParams::envmapBlendMode(GET_MESH_MATERIAL(SELF),
                                 (SG_EnvmapBlendMode)GET_NEXT_INT(ARGS));
}

CK_DLL_MFUN(gshape_phong_material_get_envmap_intensity)
{
    RETURN->v_float = *PhongParams::envmapIntensity(GET_MESH_MATERIAL(SELF));
}

CK_DLL_MFUN(gshape_phong_material_set_envmap_intensity)
{
    PhongParams::envmapIntensity(GET_MESH_MATERIAL(SELF), GET_NEXT_FLOAT(ARGS));
}
