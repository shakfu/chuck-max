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

#include "core/file.h"
#include "core/hashmap.h"
#include "core/log.h"

#include "geometry.h"

#include <rapidobj/rapidobj.hpp>

CK_DLL_SFUN(assloader_load_obj);
CK_DLL_SFUN(assloader_load_obj_flip_y);

#define RAPID_FLOAT3_TO_GLM_VEC3(f3) glm::vec3(f3[0], f3[1], f3[2])

static void logRapidobjError(const rapidobj::Error& error, const char* filepath)
{
    log_warn("Could not load OBJ model \"%s\": %s", filepath,
             error.code.message().c_str());
    if (!error.line.empty()) {
        log_warn("On line %d: \"%s\"", error.line_num, error.line.c_str());
    }
}

// used to track geometries per material id during OBJ loading
// currently unused
static hashmap* ulib_assloader_mat2geo_map = NULL;
struct AssloaderMat2GeoItem {
    SG_ID mat_id; // key
    SG_ID geo_id; // value

    static int compare(const void* a, const void* b, void* udata)
    {
        UNUSED_VAR(udata);
        AssloaderMat2GeoItem* item_a = (AssloaderMat2GeoItem*)a;
        AssloaderMat2GeoItem* item_b = (AssloaderMat2GeoItem*)b;
        return item_a->mat_id - item_b->mat_id;
    }

    static uint64_t hash(const void* item, uint64_t seed0, uint64_t seed1)
    {
        AssloaderMat2GeoItem* key = (AssloaderMat2GeoItem*)item;
        return hashmap_xxhash3(&key->mat_id, sizeof(key->mat_id), seed0, seed1);
    }

    static SG_Geometry* get(SG_ID material_id)
    {
        AssloaderMat2GeoItem* item = (AssloaderMat2GeoItem*)hashmap_get(
          ulib_assloader_mat2geo_map, &material_id);

        return item ? SG_GetGeometry(item->geo_id) : NULL;
    }

    // creates a new geometry and sets
    static SG_Geometry* set(SG_ID material_id, SG_Geometry* geo)
    {
        AssloaderMat2GeoItem item = {};
        item.mat_id               = material_id;
        item.geo_id               = geo->id;

        const void* exists = hashmap_set(ulib_assloader_mat2geo_map, &item);
        ASSERT(!exists);
        UNUSED_VAR(exists);

        return geo;
    }
};

void ulib_assloader_query(Chuck_DL_Query* QUERY)
{
    // AssLoader --------------------------------------------------------------
    {
        BEGIN_CLASS("AssLoader", "Object");
        DOC_CLASS("Utility for asset loading; supports .obj files");
        ADD_EX("basic/asset_loading.ck");

        SFUN(assloader_load_obj, "GGen", "loadObj");
        ARG("string", "filepath");
        DOC_FUNC("Load an .obj file from the given filepath");

        SFUN(assloader_load_obj_flip_y, "GGen", "loadObj");
        ARG("string", "filepath");
        ARG("int", "flip_y");
        DOC_FUNC(
          "Load an .obj file from the given filepath. If flip_y is true, the y-axis is "
          "flipped (default is false)");

        END_CLASS();
    }

    // init resources
    ulib_assloader_mat2geo_map
      = hashmap_new(sizeof(AssloaderMat2GeoItem), 0, 0, 0, AssloaderMat2GeoItem::hash,
                    AssloaderMat2GeoItem::compare, NULL, NULL);
}

// impl ============================================================================

static SG_Transform* ulib_assloader_load_obj(const char* filepath,
                                             Chuck_VM_Shred* SHRED, bool flip_y)
{
    // for simplicity, does not support lines or points
    // renders all models with Phong lighting (for pbr use gltf loader instead)
    rapidobj::Result result = rapidobj::ParseFile(filepath);

    if (result.error) {
        logRapidobjError(result.error, filepath);
        return ulib_ggen_create(NULL, SHRED); // on error return empty ggen
    }

    rapidobj::Triangulate(result);

    if (result.error) {
        logRapidobjError(result.error, filepath);
        return ulib_ggen_create(NULL, SHRED); // on error return empty ggen
    }

    // first create all unique materials
    size_t num_materials = result.materials.size();
    SG_ID* material_ids
      = ARENA_PUSH_ZERO_COUNT(&audio_frame_arena, SG_ID, (num_materials + 1) * 2);
    material_ids += 1; // so that material_ids[-1] is the default material

    // then make a geometry for each material (for optimization purposes, we group all
    // shapes vertices with the same material idx under the same material) this reduces
    // a model like backpack.obj from 79 geometries and 1 material --> 1 geo and 1
    // material (1 draw call!)
    SG_ID* geo_ids = material_ids + num_materials + 1;

    // we need #meshes = #materials. if >1 mesh, create a parent ggen to contain all
    SG_Transform* obj_shape_root = NULL;
    // if multiple shapes, return under parent root
    if (num_materials > 1) {
        obj_shape_root = ulib_ggen_create(NULL, SHRED);
    }

    for (size_t i = 0; i < num_materials; i++) {
        // create geometry for this material
        SG_Geometry* geo = ulib_geometry_create(SG_GEOMETRY, SHRED);
        geo_ids[i]       = geo->id;

        const rapidobj::Material& obj_material = result.materials[i];

        // assumes material is phong (currently NOT supporting pbr extension)
        SG_Material* phong_material = ulib_material_create(SG_MATERIAL_PHONG, SHRED);

        // add to material id array
        material_ids[i] = phong_material->id;

        // set name
        ulib_component_set_name(phong_material, obj_material.name.c_str());

        // set uniforms
        // adding ambient + diffuse color (PhongMaterial doesn't differentiate between
        // diffuse color and ambient color)
        PhongParams::diffuse(phong_material,
                             RAPID_FLOAT3_TO_GLM_VEC3(obj_material.diffuse)
                               + RAPID_FLOAT3_TO_GLM_VEC3(obj_material.ambient));
        PhongParams::specular(phong_material,
                              RAPID_FLOAT3_TO_GLM_VEC3(obj_material.specular));
        PhongParams::shininess(phong_material, obj_material.shininess);
        PhongParams::emission(phong_material,
                              RAPID_FLOAT3_TO_GLM_VEC3(obj_material.emission));

        // TODO set textures
        SG_TextureLoadDesc load_desc = {};
        load_desc.flip_y             = flip_y;
        load_desc.gen_mips           = true;
        std::string directory        = File_dirname(filepath);

        if (obj_material.diffuse_texname.size()) {
            std::string tex_path = directory + obj_material.diffuse_texname;
            SG_Texture* tex = ulib_texture_load(tex_path.c_str(), &load_desc, SHRED);
            PhongParams::albedoTex(phong_material, tex);
        }

        if (obj_material.specular_texname.size()) {
            std::string tex_path = directory + obj_material.specular_texname;
            SG_Texture* tex = ulib_texture_load(tex_path.c_str(), &load_desc, SHRED);
            PhongParams::specularTex(phong_material, tex);
        }

        if (obj_material.bump_texname.size()) {
            std::string tex_path = directory + obj_material.bump_texname;
            SG_Texture* tex = ulib_texture_load(tex_path.c_str(), &load_desc, SHRED);
            PhongParams::normalTex(phong_material, tex);
        }

        if (obj_material.ambient_texname.size()) {
            std::string tex_path = directory + obj_material.ambient_texname;
            SG_Texture* tex = ulib_texture_load(tex_path.c_str(), &load_desc, SHRED);
            PhongParams::aoTex(phong_material, tex);
        }

        if (obj_material.emissive_texname.size()) {
            std::string tex_path = directory + obj_material.ambient_texname;
            SG_Texture* tex = ulib_texture_load(tex_path.c_str(), &load_desc, SHRED);
            PhongParams::emissiveTex(phong_material, tex);
        }
    }

    // TODO set names
    for (const rapidobj::Shape& shape : result.shapes) {
        bool missing_normals  = false;
        bool missing_uvs      = false;
        int prev_material_idx = -100;
        SG_Geometry* face_geo = NULL;
        size_t num_vertices   = shape.mesh.indices.size();

        // reset the mat --> geo map for each shape/mesh
        ASSERT(hashmap_count(ulib_assloader_mat2geo_map) == 0);
        defer(hashmap_clear(ulib_assloader_mat2geo_map, false));

        ASSERT(num_vertices % 3 == 0);
        ASSERT(shape.mesh.indices.size() / 3 == shape.mesh.material_ids.size());

        if (!shape.lines.indices.empty()) {
            log_warn("Obj Shape \"%s\" has polylines; unsupported; skipping",
                     shape.name.c_str());
        }
        if (!shape.points.indices.empty()) {
            log_warn("Obj Shape \"%s\" has points; unsupported; skipping",
                     shape.name.c_str());
        }

        for (size_t i = 0; i < num_vertices; i++) {

            // every face update material and geometry
            if (i % 3 == 0) {
                size_t face_idx  = i / 3; // 3 vertices per face
                i32 material_idx = shape.mesh.material_ids[face_idx];
                if (material_idx != prev_material_idx) {
                    prev_material_idx = material_idx;
                    if (material_ids[material_idx] == 0) {
                        ASSERT(material_idx == -1)
                        ASSERT(geo_ids[material_idx] == 0);
                        // create default material
                        material_ids[material_idx]
                          = ulib_material_create(SG_MATERIAL_PHONG, SHRED)->id;
                        geo_ids[material_idx]
                          = ulib_geometry_create(SG_GEOMETRY, SHRED)->id;
                    }

                    face_geo = SG_GetGeometry(geo_ids[material_idx]);
                }
            }

            rapidobj::Index index = shape.mesh.indices[i];

            // get geometry buffers and allocate memory
            glm::vec3* positions = ARENA_PUSH_ZERO_TYPE(
              &face_geo->vertex_attribute_data[SG_GEOMETRY_POSITION_ATTRIBUTE_LOCATION],
              glm::vec3);

            glm::vec3* normals = ARENA_PUSH_ZERO_TYPE(
              &face_geo->vertex_attribute_data[SG_GEOMETRY_NORMAL_ATTRIBUTE_LOCATION],
              glm::vec3);

            glm::vec2* texcoords = ARENA_PUSH_ZERO_TYPE(
              &face_geo->vertex_attribute_data[SG_GEOMETRY_UV_ATTRIBUTE_LOCATION],
              glm::vec2);

            float* pos   = &result.attributes.positions[index.position_index * 3];
            positions->x = pos[0];
            positions->y = pos[1];
            positions->z = pos[2];

            // copy normals
            if (index.normal_index < 0) {
                missing_normals = true;
            } else {
                float* norm = &result.attributes.normals[index.normal_index * 3];
                normals->x  = norm[0];
                normals->y  = norm[1];
                normals->z  = norm[2];
            }

            // copy uvs
            if (index.texcoord_index < 0) {
                missing_uvs = true;
            } else {
                float* uvs   = &result.attributes.texcoords[index.texcoord_index * 2];
                texcoords->x = uvs[0];
                texcoords->y = uvs[1];
            }
        }

        if (missing_normals) {
            log_error(
              "Warning, OBJ mesh %s is missing normal data. Defaulting to (0,0,0)",
              shape.name.c_str());
        }
        if (missing_uvs) {
            log_error("Warning, OBJ mesh %s is missing uv data. Defaulting to (0,0)",
                      shape.name.c_str());
        }

        // TODO eventually conslidate with `ulib_geometry_build()`
    }

    // start from -1 to include default material/geo
    for (int i = -1; i < (i32)num_materials; i++) {
        // check for default mat/geo
        if (!material_ids[i]) {
            ASSERT(!geo_ids[i])
            continue;
        }

        SG_Geometry* geo = SG_GetGeometry(geo_ids[i]);

        // update
        CQ_UpdateAllVertexAttributes(geo);

        // create mesh
        SG_Mesh* mesh
          = ulib_mesh_create(NULL, geo, SG_GetMaterial(material_ids[i]), SHRED);

        // assign to parent
        if (obj_shape_root) {
            CQ_PushCommand_AddChild(obj_shape_root, mesh);
        } else {
            obj_shape_root = mesh;
        }
    }

    return obj_shape_root;
}

CK_DLL_SFUN(assloader_load_obj)
{
    RETURN->v_object = NULL;

    SG_Transform* obj_root
      = ulib_assloader_load_obj(API->object->str(GET_NEXT_STRING(ARGS)), SHRED, false);
    RETURN->v_object = obj_root ? obj_root->ckobj : NULL;
}

CK_DLL_SFUN(assloader_load_obj_flip_y)
{
    RETURN->v_object     = NULL;
    const char* filepath = API->object->str(GET_NEXT_STRING(ARGS));
    bool flip_y          = (bool)GET_NEXT_INT(ARGS);

    SG_Transform* obj_root = ulib_assloader_load_obj(filepath, SHRED, flip_y);
    RETURN->v_object       = obj_root ? obj_root->ckobj : NULL;
}
