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

CK_DLL_CTOR(gcamera_ctor);

CK_DLL_MFUN(gcamera_set_mode_persp);
CK_DLL_MFUN(gcamera_set_mode_ortho);
CK_DLL_MFUN(gcamera_get_mode);

CK_DLL_MFUN(gcamera_set_clip);
CK_DLL_MFUN(gcamera_get_clip_near);
CK_DLL_MFUN(gcamera_get_clip_far);

// perspective camera params
// (no aspect, that's set automatically by renderer window resize callback)
CK_DLL_MFUN(gcamera_set_pers_fov);
CK_DLL_MFUN(gcamera_get_pers_fov);

// ortho camera params
CK_DLL_MFUN(gcamera_set_ortho_size); // view volume size (preserves screen aspect ratio)
CK_DLL_MFUN(gcamera_get_ortho_size);

CK_DLL_MFUN(gcamera_screen_coord_to_world_pos);
CK_DLL_MFUN(gcamera_world_pos_to_screen_coord);
CK_DLL_MFUN(gcamera_ndc_to_world_pos);
CK_DLL_MFUN(gcamera_world_pos_to_ndc);

// TODO overridable update(dt) (actualy don't we already get this from GGen?)
// add mouse click state to GWindow
// impl arcball Camera

CK_DLL_CTOR(orbit_camera_ctor);
CK_DLL_MFUN(orbit_camera_update);
CK_DLL_MFUN(orbit_camera_set_drag_speed);
CK_DLL_MFUN(orbit_camera_get_drag_speed);
CK_DLL_MFUN(orbit_camera_set_zoom_speed);
CK_DLL_MFUN(orbit_camera_get_zoom_speed);
CK_DLL_MFUN(orbit_camera_set_target);
CK_DLL_MFUN(orbit_camera_get_target);

CK_DLL_CTOR(fly_camera_ctor);
CK_DLL_MFUN(fly_camera_update);

CK_DLL_MFUN(fly_camera_set_speed);
CK_DLL_MFUN(fly_camera_get_speed);
CK_DLL_MFUN(fly_camera_set_sensitivity);
CK_DLL_MFUN(fly_camera_get_sensitivity);

static void ulib_camera_query(Chuck_DL_Query* QUERY)
{
    BEGIN_CLASS(SG_CKNames[SG_COMPONENT_CAMERA], SG_CKNames[SG_COMPONENT_TRANSFORM]);
    DOC_CLASS(
      "ChuGL Camera class. Can be used to create perspective or orthographic "
      "cameras. Also comes with builtin OrbitCamera and FlyCamera controllers.");
    ADD_EX("basic/mousepick.ck");

    static t_CKINT camera_mode_persp = (t_CKINT)SG_CameraType_PERPSECTIVE;
    static t_CKINT camera_mode_ortho = (t_CKINT)SG_CameraType_ORTHOGRAPHIC;
    SVAR("int", "PERSPECTIVE", &camera_mode_persp);
    SVAR("int", "ORTHOGRAPHIC", &camera_mode_ortho);

    CTOR(gcamera_ctor);

    // camera mode
    MFUN(gcamera_set_mode_persp, "void", "perspective");
    DOC_FUNC("Set camera mode to perspective.");
    MFUN(gcamera_set_mode_ortho, "void", "orthographic");
    DOC_FUNC("Set camera mode to orthographic.");
    MFUN(gcamera_get_mode, "int", "mode");
    DOC_FUNC(
      "Get camera mode. Returns either GCamera.PERSPECTIVE or GCamera.ORTHOGRAPHIC.");

    // clip planes
    MFUN(gcamera_set_clip, "void", "clip");
    ARG("float", "near");
    ARG("float", "far");
    DOC_FUNC("Set camera clip planes.");

    MFUN(gcamera_get_clip_near, "float", "clipNear");
    DOC_FUNC("Get camera near clip plane.");

    MFUN(gcamera_get_clip_far, "float", "clipFar");
    DOC_FUNC("Get camera far clip plane.");

    // perspective camera params
    MFUN(gcamera_set_pers_fov, "void", "fov");
    ARG("float", "fov_radians");
    DOC_FUNC("Set camera field of view in radians.");

    MFUN(gcamera_get_pers_fov, "float", "fov");
    DOC_FUNC("Get camera field of view in radians.");

    // ortho camera params
    MFUN(gcamera_set_ortho_size, "void", "viewSize");
    ARG("float", "size");
    DOC_FUNC(
      "(orthographic mode) set the height of the view volume in world space units. "
      "Width is automatically calculated based on aspect ratio.");

    MFUN(gcamera_get_ortho_size, "float", "viewSize");
    DOC_FUNC(
      "(orthographic mode) get the height of the view volume in world space units.");

    // raycast
    MFUN(gcamera_screen_coord_to_world_pos, "vec3", "screenCoordToWorldPos");
    ARG("vec2", "screen_pos");
    ARG("float", "distance");
    DOC_FUNC(
      "Returns the world position of a point in screen space at a given distance from "
      "the camera. "
      "Useful in combination with GWindow.mousePos() for mouse picking.");

    MFUN(gcamera_world_pos_to_screen_coord, "vec2", "worldPosToScreenCoord");
    ARG("vec3", "world_pos");
    DOC_FUNC(
      "Get a screen coordinate from a world position by casting a ray from worldPos "
      "back to the camera and finding the intersection with the near clipping plane"
      "world_pos is a vec3 representing a world position."
      "Returns a vec2 screen coordinate."
      "Remember, screen coordinates have origin at the top-left corner of the window");

    MFUN(gcamera_ndc_to_world_pos, "vec3", "NDCToWorldPos");
    ARG("vec3", "clip_pos");
    DOC_FUNC(
      "Convert a point in clip space to world space. Clip space x and y should be in "
      "range [-1, 1], and z in the range [0, 1]. For x and y, 0 is the center of the "
      "screen. For z, 0 is the near clip plane and 1 is the far clip plane.");

    MFUN(gcamera_world_pos_to_ndc, "vec3", "worldPosToNDC");
    ARG("vec3", "world_pos");
    DOC_FUNC("Convert a point in world space to this camera's clip space.");

    END_CLASS();

    // orbit camera
    {
        BEGIN_CLASS("GOrbitCamera", SG_CKNames[SG_COMPONENT_CAMERA]);
        DOC_CLASS(
          "Built-in orbit camera controller. Click and drag to rotate the camera. "
          "Scroll to zoom.");

        CTOR(orbit_camera_ctor);

        MFUN(orbit_camera_update, "void", "update");
        ARG("float", "dt");
        DOC_FUNC(
          "Overrides the GGen.update(float dt) method. Called automatically every "
          "frame.");

        MFUN(orbit_camera_set_drag_speed, "void", "dragSpeed");
        ARG("vec2", "speed");
        DOC_FUNC(
          "Set the horizontal and vertical speed of the camera's rotation when "
          "dragging with the mouse.");

        MFUN(orbit_camera_get_drag_speed, "vec2", "dragSpeed");
        DOC_FUNC(
          "Get the speed of the camera's rotation when dragging with the mouse.");

        MFUN(orbit_camera_set_zoom_speed, "void", "zoomSpeed");
        ARG("float", "speed");
        DOC_FUNC(
          "Set the speed of the camera's zoom when scrolling with the mouse wheel.");

        MFUN(orbit_camera_get_zoom_speed, "float", "zoomSpeed");
        DOC_FUNC(
          "Get the speed of the camera's zoom when scrolling with the mouse wheel.");

        MFUN(orbit_camera_set_target, "void", "target");
        ARG("vec3", "target");
        DOC_FUNC("Set the point in world space that the camera orbits around.");

        MFUN(orbit_camera_get_target, "vec3", "target");
        DOC_FUNC("Get the point in world space that the camera orbits around.");

        END_CLASS();
    }

    // fly camera
    {
        BEGIN_CLASS("GFlyCamera", SG_CKNames[SG_COMPONENT_CAMERA]);
        DOC_CLASS(
          "Built-in fly camera controller. Use WASD to move and mouse to look. Use QE "
          "to move up and down.");

        CTOR(fly_camera_ctor);

        MFUN(fly_camera_update, "void", "update");
        ARG("float", "dt");
        DOC_FUNC(
          "Overrides the GGen.update(dt) method. Called automatically every frame.");

        MFUN(fly_camera_set_speed, "void", "speed");
        ARG("float", "speed");
        DOC_FUNC("Set the move speed of the camera");

        MFUN(fly_camera_get_speed, "float", "speed");
        DOC_FUNC("Get the move speed of the camera");

        MFUN(fly_camera_set_sensitivity, "void", "sensitivity");
        ARG("float", "sensitivity");
        DOC_FUNC("Set the mouse look sensitivity of the camera");

        MFUN(fly_camera_get_sensitivity, "float", "sensitivity");
        DOC_FUNC("Get the mouse look sensitivity of the camera");

        END_CLASS();
    }
}

SG_Camera* ulib_camera_create(Chuck_Object* ckobj)
{
    CK_DL_API API = g_chuglAPI;
    // create camera component
    SG_CameraParams default_cam_params = {}; // passing direclty for now rather than
                                             // creating separate CameraParams struct
    SG_Camera* cam = SG_CreateCamera(ckobj, default_cam_params);
    OBJ_MEMBER_UINT(ckobj, component_offset_id) = cam->id;
    CQ_PushCommand_CameraCreate(cam);

    return cam;
}

CK_DLL_CTOR(gcamera_ctor)
{
    ulib_camera_create(SELF);
}

CK_DLL_MFUN(gcamera_set_mode_persp)
{
    SG_Camera* cam          = GET_CAMERA(SELF);
    cam->params.camera_type = SG_CameraType_PERPSECTIVE;

    CQ_PushCommand_CameraSetParams(cam);
}

CK_DLL_MFUN(gcamera_set_mode_ortho)
{
    SG_Camera* cam          = GET_CAMERA(SELF);
    cam->params.camera_type = SG_CameraType_ORTHOGRAPHIC;

    CQ_PushCommand_CameraSetParams(cam);
}

CK_DLL_MFUN(gcamera_get_mode)
{
    SG_Camera* cam = GET_CAMERA(SELF);
    RETURN->v_int  = (t_CKINT)cam->params.camera_type;
}

CK_DLL_MFUN(gcamera_set_clip)
{
    SG_Camera* cam         = GET_CAMERA(SELF);
    cam->params.near_plane = GET_NEXT_FLOAT(ARGS);
    cam->params.far_plane  = GET_NEXT_FLOAT(ARGS);

    CQ_PushCommand_CameraSetParams(cam);
}

CK_DLL_MFUN(gcamera_get_clip_near)
{
    SG_Camera* cam  = GET_CAMERA(SELF);
    RETURN->v_float = cam->params.near_plane;
}

CK_DLL_MFUN(gcamera_get_clip_far)
{
    SG_Camera* cam  = GET_CAMERA(SELF);
    RETURN->v_float = cam->params.far_plane;
}

CK_DLL_MFUN(gcamera_set_pers_fov)
{
    SG_Camera* cam          = GET_CAMERA(SELF);
    cam->params.fov_radians = GET_NEXT_FLOAT(ARGS);

    CQ_PushCommand_CameraSetParams(cam);
}

CK_DLL_MFUN(gcamera_get_pers_fov)
{
    SG_Camera* cam  = GET_CAMERA(SELF);
    RETURN->v_float = cam->params.fov_radians;
}

CK_DLL_MFUN(gcamera_set_ortho_size)
{
    SG_Camera* cam   = GET_CAMERA(SELF);
    cam->params.size = GET_NEXT_FLOAT(ARGS);

    CQ_PushCommand_CameraSetParams(cam);
}

CK_DLL_MFUN(gcamera_get_ortho_size)
{
    SG_Camera* cam  = GET_CAMERA(SELF);
    RETURN->v_float = cam->params.size;
}

CK_DLL_MFUN(gcamera_screen_coord_to_world_pos)
{
    SG_Camera* cam      = GET_CAMERA(SELF);
    t_CKVEC2 screen_pos = GET_NEXT_VEC2(ARGS);
    float distance      = GET_NEXT_FLOAT(ARGS);

    t_CKVEC2 windowSize = CHUGL_Window_WindowSize();
    int screenWidth     = windowSize.x;
    int screenHeight    = windowSize.y;
    float aspect        = (float)screenWidth / (float)screenHeight;

    if (cam->params.camera_type == SG_CameraType_ORTHOGRAPHIC) {
        // calculate camera frustrum size in world space
        float frustrum_height = cam->params.size;
        float frustrum_width  = frustrum_height * aspect;

        // convert from normalized mouse coords to view space coords
        // (we negate viewY so that 0,0 is bottom left instead of top left)
        float view_x = frustrum_width * (screen_pos.x / screenWidth - 0.5f);
        float view_y = -frustrum_height * (screen_pos.y / screenHeight - 0.5f);

        // convert from view space coords to world space coords
        glm::vec3 world_pos
          = SG_Transform::worldMatrix(cam) * glm::vec4(view_x, view_y, -distance, 1.0f);

        RETURN->v_vec3 = { world_pos.x, world_pos.y, world_pos.z };
        return;
    } else if (cam->params.camera_type
               == SG_CameraType_PERPSECTIVE) { // perspective camera
        glm::vec2 ndc = { (screen_pos.x / screenWidth) * 2.0f - 1.0f,
                          1.0f - (screen_pos.y / screenHeight) * 2.0f };

        // first convert to normalized device coordinates in range [-1, 1]
        glm::vec4 lRayEnd_NDC(ndc, 1.0, 1.0f); // The far plane maps to Z=1 in NDC

        glm::mat4 M = glm::inverse(SG_Camera::projectionMatrix(cam, aspect)
                                   * SG_Camera::viewMatrix(cam));

        glm::vec3 lRayStart_world = SG_Transform::worldPosition(cam);
        glm::vec4 lRayEnd_world   = M * lRayEnd_NDC;
        lRayEnd_world /= lRayEnd_world.w; // perspective divide

        glm::vec3 lRayDir_world
          = glm::normalize(glm::vec3(lRayEnd_world) - lRayStart_world);
        glm::vec3 world_pos = lRayStart_world + distance * lRayDir_world;
        RETURN->v_vec3      = { world_pos.x, world_pos.y, world_pos.z };
        return;
    }
    ASSERT(false); // unsupported camera type
}

CK_DLL_MFUN(gcamera_ndc_to_world_pos)
{
    SG_Camera* cam      = GET_CAMERA(SELF);
    t_CKVEC3 ndc_pos    = GET_NEXT_VEC3(ARGS);
    t_CKVEC2 windowSize = CHUGL_Window_WindowSize();
    float aspect        = (float)windowSize.x / (float)windowSize.y;

    /*
    Formula:
    clip_pos = P * V * world_pos
    world_pos = inv(P * V) * clip_pos
    */

    glm::mat4 view = SG_Camera::viewMatrix(cam);
    glm::mat4 proj = SG_Camera::projectionMatrix(cam, aspect);
    glm::vec4 world
      = glm::inverse(proj * view) * glm::vec4(ndc_pos.x, ndc_pos.y, ndc_pos.z, 1.0f);
    world /= world.w; // perspective divide

    RETURN->v_vec3 = { world.x, world.y, world.z };
}

CK_DLL_MFUN(gcamera_world_pos_to_ndc)
{
    SG_Camera* cam      = GET_CAMERA(SELF);
    t_CKVEC3 world_pos  = GET_NEXT_VEC3(ARGS);
    t_CKVEC2 windowSize = CHUGL_Window_WindowSize();
    float aspect        = (float)windowSize.x / (float)windowSize.y;

    /*
    Formula:
    clip_pos = P * V * world_pos
    */

    glm::mat4 view = SG_Camera::viewMatrix(cam);
    glm::mat4 proj = SG_Camera::projectionMatrix(cam, aspect);
    glm::vec4 clip
      = proj * view * glm::vec4(world_pos.x, world_pos.y, world_pos.z, 1.0f);
    glm::vec4 ndc = clip / clip.w;

    RETURN->v_vec3 = { ndc.x, ndc.y, ndc.z };
}

CK_DLL_MFUN(gcamera_world_pos_to_screen_coord)
{
    SG_Camera* cam    = GET_CAMERA(SELF);
    t_CKVEC3 worldPos = GET_NEXT_VEC3(ARGS);

    t_CKVEC2 windowSize = CHUGL_Window_WindowSize();
    float aspect        = windowSize.x / windowSize.y;

    // first convert to clip space
    glm::mat4 view = SG_Camera::viewMatrix(cam);

    glm::mat4 proj = SG_Camera::projectionMatrix(cam, aspect);
    glm::vec4 clipPos
      = proj * view * glm::vec4(worldPos.x, worldPos.y, worldPos.z, 1.0f);

    // convert to screen space
    float x = (clipPos.x / clipPos.w + 1.0f) / 2.0f * windowSize.x;
    // need to invert y because screen coordinates are top-left origin
    float y = (1.0f - clipPos.y / clipPos.w) / 2.0f * windowSize.y;
    // z is depth value (buggy)
    // float z        = clipPos.z / clipPos.w;
    RETURN->v_vec2 = { x, y };
}

// Orbit Camera =====================================================================

CK_DLL_CTOR(orbit_camera_ctor)
{
    SG_Camera* camera       = GET_CAMERA(SELF);
    camera->controller_type = SG_CameraControllerType_Orbit;

    // initialize orbit params
    camera->orbit = {};

    // set camera transform to initial spherical coords
    camera->pos
      = SphericalCoords::toCartesian(camera->orbit.spherical, camera->orbit.target);
    SG_Transform::lookAt(camera, VEC_ORIGIN);

    CQ_PushCommand_SetPosition(camera);
    CQ_PushCommand_SetRotation(camera);
}

CK_DLL_MFUN(orbit_camera_update)
{
    SG_Camera* camera           = GET_CAMERA(SELF);
    SG_OrbitCameraParams* orbit = &camera->orbit;

    // note: don't need to multiply by dt since it's scaled by mouse deltas (which is a
    // measure of distance, not time)

    // update spherical coords to current camera position
    // (length check to avoid NaNs)
    if (glm::length(camera->pos - orbit->target) > 0.0f)
        orbit->spherical = SphericalCoords::fromCartesian(camera->pos - orbit->target);

    // update scroll
    t_CKVEC2 scroll_deltas = CHUGL_scroll_delta();
    orbit->spherical.radius
      = MAX(0.1f, orbit->spherical.radius - orbit->zoom_speed * (scroll_deltas.y));

    if (CHUGL_Mouse_LeftButton()) {
        t_CKVEC2 mouse_deltas = CHUGL_Mouse_Delta();
        orbit->spherical.theta -= (orbit->speed.x * mouse_deltas.x);
        orbit->spherical.phi += (orbit->speed.y * mouse_deltas.y);

        // clamp phi
        orbit->spherical.phi
          = CLAMP(orbit->spherical.phi, -PI / (2.01f), PI / (2.01f + EPSILON));

        // clamp theta
        orbit->spherical.theta = fmod(orbit->spherical.theta, 2.0f * PI);
    }

    // update camera position from spherical coordinates
    camera->pos = SphericalCoords::toCartesian(orbit->spherical, orbit->target);
    SG_Transform::lookAt(camera, orbit->target);

    // log_debug("orbit camera update: radius %f, theta %f, phi %f",
    //           orbit->spherical.radius, orbit->spherical.theta, orbit->spherical.phi);
    // log_debug("orbit camera transform: pos %f %f %f", camera->pos.x, camera->pos.y,
    //           camera->pos.z);

    CQ_PushCommand_SetPosition(camera);
    CQ_PushCommand_SetRotation(camera);
}

CK_DLL_MFUN(orbit_camera_set_drag_speed)
{
    GET_CAMERA(SELF)->orbit.speed = GET_NEXT_VEC2(ARGS);
}

CK_DLL_MFUN(orbit_camera_get_drag_speed)
{
    RETURN->v_vec2 = GET_CAMERA(SELF)->orbit.speed;
}

CK_DLL_MFUN(orbit_camera_set_zoom_speed)
{
    GET_CAMERA(SELF)->orbit.zoom_speed = GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(orbit_camera_get_zoom_speed)
{
    RETURN->v_float = GET_CAMERA(SELF)->orbit.zoom_speed;
}

CK_DLL_MFUN(orbit_camera_set_target)
{
    SG_Camera* camera    = GET_CAMERA(SELF);
    t_CKVEC3 target      = GET_NEXT_VEC3(ARGS);
    camera->orbit.target = { target.x, target.y, target.z };

    // update camera position
    camera->pos
      = SphericalCoords::toCartesian(camera->orbit.spherical, camera->orbit.target);
    SG_Transform::lookAt(camera, camera->orbit.target);

    CQ_PushCommand_SetPosition(camera);
    CQ_PushCommand_SetRotation(camera);
}

CK_DLL_MFUN(orbit_camera_get_target)
{
    glm::vec3 target = GET_CAMERA(SELF)->orbit.target;
    RETURN->v_vec3   = { target.x, target.y, target.z };
}

// GFlyCamera =====================================================================

CK_DLL_CTOR(fly_camera_ctor)
{
    SG_Camera* camera = GET_CAMERA(SELF);
    // initialize orbit params
    camera->fly             = {};
    camera->controller_type = SG_CameraControllerType_Fly;
}

CK_DLL_MFUN(fly_camera_update)
{
    /*
    TODO:
    - scroll change camera fov
    */

    SG_Camera* camera = GET_CAMERA(SELF);
    f32 dt            = GET_NEXT_FLOAT(ARGS);

    if (CHUGL_Kb_key(GLFW_KEY_W).down)
        SG_Transform::translate(camera,
                                camera->fly.speed * dt * SG_Transform::forward(camera));

    if (CHUGL_Kb_key(GLFW_KEY_S).down)
        SG_Transform::translate(camera, -camera->fly.speed * dt
                                          * SG_Transform::forward(camera));

    if (CHUGL_Kb_key(GLFW_KEY_D).down)
        SG_Transform::translate(camera,
                                camera->fly.speed * dt * SG_Transform::right(camera));

    if (CHUGL_Kb_key(GLFW_KEY_A).down)
        SG_Transform::translate(camera,
                                -camera->fly.speed * dt * SG_Transform::right(camera));

    if (CHUGL_Kb_key(GLFW_KEY_E).down)
        SG_Transform::translate(camera, camera->fly.speed * dt * VEC_UP);

    if (CHUGL_Kb_key(GLFW_KEY_Q).down)
        SG_Transform::translate(camera, -camera->fly.speed * dt * VEC_UP);

    // mouse lookaround
    t_CKVEC2 mouse_deltas = CHUGL_Mouse_Delta();

    // for mouse deltaY, rotate around right axis
    SG_Transform::rotateOnLocalAxis(camera, VEC_RIGHT,
                                    camera->fly.mouse_sensitivity * -mouse_deltas.y);
    // for mouse deltaX, rotate around (0,1,0)
    SG_Transform::rotateOnWorldAxis(camera, VEC_UP,
                                    camera->fly.mouse_sensitivity * -mouse_deltas.x);

    CQ_PushCommand_SetPosition(camera);
    CQ_PushCommand_SetRotation(camera);
}

CK_DLL_MFUN(fly_camera_set_speed)
{
    GET_CAMERA(SELF)->fly.speed = GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(fly_camera_get_speed)
{
    RETURN->v_float = GET_CAMERA(SELF)->fly.speed;
}

CK_DLL_MFUN(fly_camera_set_sensitivity)
{
    GET_CAMERA(SELF)->fly.mouse_sensitivity = GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(fly_camera_get_sensitivity)
{
    RETURN->v_float = GET_CAMERA(SELF)->fly.mouse_sensitivity;
}
