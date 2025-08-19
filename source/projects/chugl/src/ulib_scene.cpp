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

CK_DLL_CTOR(gscene_ctor);
CK_DLL_DTOR(gscene_dtor);

CK_DLL_MFUN(gscene_set_background_color);
CK_DLL_MFUN(gscene_get_background_color);

CK_DLL_MFUN(gscene_set_main_camera);
CK_DLL_MFUN(gscene_get_main_camera);

CK_DLL_MFUN(gscene_set_ambient_light);
CK_DLL_MFUN(gscene_get_ambient_light);

CK_DLL_MFUN(gscene_get_default_light);
CK_DLL_MFUN(gscene_get_lights);

CK_DLL_MFUN(gscene_set_environment_map);
CK_DLL_MFUN(gscene_get_environment_map);

CK_DLL_MFUN(gscene_set_skybox_material);
CK_DLL_MFUN(gscene_get_skybox_material);

SG_Scene* ulib_scene_create(Chuck_Object* ckobj, bool skybox, bool orthographic)
{
    CK_DL_API API = g_chuglAPI;

    // execute change on audio thread side
    SG_Scene* scene = SG_CreateScene(ckobj);
    // save SG_ID
    OBJ_MEMBER_UINT(ckobj, component_offset_id) = scene->id;

    // tell graphics thread to create the scene
    CQ_PushCommand_SceneUpdate(scene);

    // default skybox and envmap
    if (skybox) {
        SG_Material* skybox_material = ulib_material_create(SG_MATERIAL_SKYBOX, NULL);
        SG_Scene::setSkyboxMaterial(scene, skybox_material);
    }
    // SG_Scene::setEnvMapSampler(scene, SG_SAMPLER_DEFAULT);
    SG_Scene::setEnvMap(scene, SG_GetTexture(g_builtin_textures.default_cubemap_id));

    // default directional light
    Chuck_Object* dir_light_ckobj = chugin_createCkObj("GDirLight", false);
    SG_Light* dir_light = ulib_light_create(dir_light_ckobj, SG_LightType_Directional);
    CQ_PushCommand_AddChild(scene, dir_light);
    // angle light down slightly
    SG_Transform::lookAt(dir_light, glm::vec3(0.0f, -1.0f, -1.0f));
    CQ_PushCommand_SetRotation(dir_light);

    // default camera
    SG_Camera* default_camera
      = ulib_camera_create(chugin_createCkObj(SG_CKNames[SG_COMPONENT_CAMERA], false));
    CQ_PushCommand_AddChild(scene, default_camera);
    SG_Scene::setMainCamera(scene, default_camera);
    gg_config.mainCamera = default_camera->id;
    default_camera->pos  = glm::vec3(0.0f, 0.0f, 5.0f);
    CQ_PushCommand_SetPosition(default_camera);
    if (orthographic) {
        default_camera->params.camera_type = SG_CameraType_ORTHOGRAPHIC;
        CQ_PushCommand_CameraSetParams(default_camera);
    }

    // push rest of scene updates
    CQ_PushCommand_SceneUpdate(scene);

    return scene;
}

static void ulib_gscene_query(Chuck_DL_Query* QUERY)
{
    // EM_log(CK_LOG_INFO, "ChuGL scene");
    // CGL scene
    QUERY->begin_class(QUERY, SG_CKNames[SG_COMPONENT_SCENE],
                       SG_CKNames[SG_COMPONENT_TRANSFORM]);
    DOC_CLASS(
      "Scene class. Static--all instances point to the same underlying ChuGL main "
      "scene. GGens must be added to a scene to be rendered.");
    ADD_EX("basic/skybox.ck");

    QUERY->add_ctor(QUERY, gscene_ctor);

    // background color
    QUERY->add_mfun(QUERY, gscene_set_background_color, "void", "backgroundColor");
    QUERY->add_arg(QUERY, "vec3", "color");
    QUERY->doc_func(QUERY, "Set the background color of the scene");

    QUERY->add_mfun(QUERY, gscene_get_background_color, "vec3", "backgroundColor");
    QUERY->doc_func(QUERY, "Get the background color of the scene");

    // main camera
    MFUN(gscene_set_main_camera, "GCamera", "camera");
    ARG("GCamera", "camera");
    DOC_FUNC("Set the main camera of the scene");

    MFUN(gscene_get_main_camera, "GCamera", "camera");
    DOC_FUNC("Get the main camera of the scene");

    // ambient light
    MFUN(gscene_set_ambient_light, "void", "ambient");
    ARG("vec3", "color");
    DOC_FUNC(
      "Set the ambient lighting of the scene. Sets material visibility even when no "
      "light is present");

    MFUN(gscene_get_ambient_light, "vec3", "ambient");
    DOC_FUNC("Get the ambient lighting value of the scene");

    MFUN(gscene_get_default_light, SG_CKNames[SG_COMPONENT_LIGHT], "light");
    DOC_FUNC(
      "Get the first light of the scene. On the initial "
      "default scene, this returns the default directional light");

    MFUN(gscene_get_lights, "GLight[]", "lights");
    DOC_FUNC("Get array of all lights in the scene");

    MFUN(gscene_set_environment_map, "void", "envMap");
    ARG(SG_CKNames[SG_COMPONENT_TEXTURE], "envMap");
    DOC_FUNC(
      "Set the environment map of the scene. By default this used as a skybox and for "
      "environment lighting calculations on lit materials (PhongMaterial supported, "
      "PBRMaterial not yet supported). The environment color will be multiplied by the "
      "background color set in GScene.backgroundColor(vec3), remember to set the "
      "background color to Color.WHITE to view the original image colors");

    MFUN(gscene_get_environment_map, SG_CKNames[SG_COMPONENT_TEXTURE], "envMap");
    DOC_FUNC("Get the environment map of the scene");

    MFUN(gscene_set_skybox_material, "void", "skybox");
    ARG(SG_CKNames[SG_COMPONENT_MATERIAL], "skybox");
    DOC_FUNC(
      "Set the skybox material of the scene. Will be used in the skybox pass and drawn "
      "with NO bound vertex attribute buffers and a vertexCount of 3. The "
      "default scene already includes an instance of SkyboxMaterial here");

    MFUN(gscene_get_skybox_material, SG_CKNames[SG_COMPONENT_MATERIAL], "skybox");
    DOC_FUNC("Get the skybox material of the scene");

    // end class -----------------------------------------------------
    QUERY->end_class(QUERY);
}

CK_DLL_CTOR(gscene_ctor)
{
    ulib_scene_create(SELF, true, false);
}

CK_DLL_DTOR(gscene_dtor)
{
    // TODO
}

CK_DLL_MFUN(gscene_get_background_color)
{
    SG_Scene* scene = SG_GetScene(OBJ_MEMBER_UINT(SELF, component_offset_id));
    glm::vec4 color = scene->desc.bg_color;
    RETURN->v_vec3  = { color.r, color.g, color.b };
}

CK_DLL_MFUN(gscene_set_background_color)
{
    SG_Scene* scene = SG_GetScene(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKVEC3 color  = GET_NEXT_VEC3(ARGS);

    scene->desc.bg_color = { color.x, color.y, color.z, 1.0f };

    CQ_PushCommand_SceneUpdate(scene);
}

CK_DLL_MFUN(gscene_set_main_camera)
{
    SG_Scene* scene      = SG_GetScene(OBJ_MEMBER_UINT(SELF, component_offset_id));
    Chuck_Object* ck_cam = GET_NEXT_OBJECT(ARGS);

    SG_Camera* cam
      = ck_cam ? SG_GetCamera(OBJ_MEMBER_UINT(ck_cam, component_offset_id)) : NULL;

    if (cam && cam->id == scene->desc.main_camera_id) {
        RETURN->v_object = ck_cam;
        return;
    }

    // check if camera is connected to scene
    if (cam && !SG_Transform::isAncestor(scene, cam)) {
        CK_THROW("DisconnctedCamera",
                 "A camera must be connected (grucked) to scene before it can be set "
                 "as the main camera",
                 SHRED);
    }

    SG_Scene::setMainCamera(scene, cam);

    // update gfx thread
    CQ_PushCommand_SceneUpdate(scene);
}

CK_DLL_MFUN(gscene_get_main_camera)
{
    SG_Scene* scene = SG_GetScene(OBJ_MEMBER_UINT(SELF, component_offset_id));
    SG_Camera* cam  = SG_GetCamera(scene->desc.main_camera_id);

    RETURN->v_object = cam ? cam->ckobj : NULL;
}

CK_DLL_MFUN(gscene_set_ambient_light)
{
    SG_Scene* scene  = SG_GetScene(OBJ_MEMBER_UINT(SELF, component_offset_id));
    t_CKVEC3 ambient = GET_NEXT_VEC3(ARGS);

    scene->desc.ambient_light = { ambient.x, ambient.y, ambient.z };

    CQ_PushCommand_SceneUpdate(scene);
}

CK_DLL_MFUN(gscene_get_ambient_light)
{
    SG_Scene* scene   = SG_GetScene(OBJ_MEMBER_UINT(SELF, component_offset_id));
    glm::vec3 ambient = scene->desc.ambient_light;
    RETURN->v_vec3    = { ambient.r, ambient.g, ambient.b };
}

CK_DLL_MFUN(gscene_get_default_light)
{
    SG_Scene* scene  = SG_GetScene(OBJ_MEMBER_UINT(SELF, component_offset_id));
    SG_Light* light  = SG_Scene::getLight(scene, 0);
    RETURN->v_object = light ? light->ckobj : NULL;
}

CK_DLL_MFUN(gscene_get_lights)
{
    SG_Scene* scene = SG_GetScene(OBJ_MEMBER_UINT(SELF, component_offset_id));
    int num_lights  = ARENA_LENGTH(&scene->light_ids, SG_ID);

    Chuck_ArrayInt* light_ck_array
      = (Chuck_ArrayInt*)chugin_createCkObj("GLight[]", false, SHRED);

    for (int i = 0; i < num_lights; i++) {
        Chuck_Object* ck_light
          = SG_GetLight(*ARENA_GET_TYPE(&scene->light_ids, SG_ID, i))->ckobj;
        API->object->array_int_push_back(light_ck_array, (t_CKUINT)ck_light);
    }

    RETURN->v_object = (Chuck_Object*)light_ck_array;
}

CK_DLL_MFUN(gscene_set_environment_map)
{
    SG_Scene* scene      = SG_GetScene(OBJ_MEMBER_UINT(SELF, component_offset_id));
    Chuck_Object* ck_tex = GET_NEXT_OBJECT(
      ARGS); // SG_GetTexture(OBJ_MEMBER_UINT(ck_tex, component_offset_id));

    SG_Texture* tex = ck_tex ?
                        SG_GetTexture(OBJ_MEMBER_UINT(ck_tex, component_offset_id)) :
                        SG_GetTexture(g_builtin_textures.default_cubemap_id);

    SG_Scene::setEnvMap(scene, tex);
    CQ_PushCommand_SceneUpdate(scene);
}

CK_DLL_MFUN(gscene_get_environment_map)
{
    SG_Scene* scene  = SG_GetScene(OBJ_MEMBER_UINT(SELF, component_offset_id));
    SG_Texture* tex  = SG_GetTexture(scene->desc.env_map_id);
    RETURN->v_object = tex ? tex->ckobj : NULL;
}

CK_DLL_MFUN(gscene_set_skybox_material)
{
    SG_Scene* scene      = SG_GetScene(OBJ_MEMBER_UINT(SELF, component_offset_id));
    Chuck_Object* ck_mat = GET_NEXT_OBJECT(ARGS);
    SG_Material* mat
      = ck_mat ? SG_GetMaterial(OBJ_MEMBER_UINT(ck_mat, component_offset_id)) : NULL;
    SG_Scene::setSkyboxMaterial(scene, mat);
    CQ_PushCommand_SceneUpdate(scene);
}

CK_DLL_MFUN(gscene_get_skybox_material)
{
    SG_Scene* scene  = SG_GetScene(OBJ_MEMBER_UINT(SELF, component_offset_id));
    SG_Material* mat = SG_GetMaterial(scene->desc.skybox_material_id);
    RETURN->v_object = mat ? mat->ckobj : NULL;
}
