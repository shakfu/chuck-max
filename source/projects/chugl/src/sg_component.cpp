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
#include "sg_component.h"
#include "core/hashmap.h"
#include "geometry.h"
#include "sg_command.h"

#include "core/log.h"
#include "ulib_helper.h"

#include <glm/gtx/quaternion.hpp>
#include <sr_webcam/include/sr_webcam.h>

// ============================================================================
// SG_Transform definitions
// ============================================================================

void SG_Transform::_init(SG_Transform* t, Chuck_Object* ckobj)
{
    ASSERT(ckobj);

    t->ckobj    = ckobj;
    t->pos      = glm::vec3(0.0f);
    t->rot      = QUAT_IDENTITY;
    t->sca      = glm::vec3(1.0f);
    t->parentID = 0;
    // initialize children array for 8 children
    Arena::init(&t->childrenIDs, sizeof(SG_ID) * 8);
}

void SG_Transform::translate(SG_Transform* t, glm::vec3 delta)
{
    t->pos += delta;
}

void SG_Transform::rotate(SG_Transform* t, glm::quat q)
{
    t->rot = q * t->rot;
}

void SG_Transform::rotate(SG_Transform* t, glm::vec3 eulers)
{
    t->rot = glm::quat(eulers) * t->rot;
}

void SG_Transform::scale(SG_Transform* t, glm::vec3 s)
{
    t->sca *= s;
}

glm::vec3 SG_Transform::eulerRotationRadians(SG_Transform* t)
{
    return glm::eulerAngles(t->rot);
}

glm::mat4 SG_Transform::modelMatrix(SG_Transform* t)
{
    return glm::translate(glm::mat4(1.0f), t->pos) * glm::toMat4(t->rot)
           * glm::scale(glm::mat4(1.0f), t->sca);
}

// TODO: can optimize by caching Model and World matrices
glm::mat4 SG_Transform::worldMatrix(SG_Transform* t)
{
    SG_Transform* parent = SG_GetTransform(t->parentID);
    if (!parent) return SG_Transform::modelMatrix(t);

    glm::mat4 worldMat = SG_Transform::modelMatrix(t);
    while (parent) {
        worldMat = SG_Transform::modelMatrix(parent) * worldMat;
        parent   = SG_GetTransform(parent->parentID);
    }
    return worldMat;
}

// walks scene graph, gets world quaternion rotation
glm::quat SG_Transform::worldRotation(SG_Transform* t)
{
    // TODO: is this bugged? does scale affect rotation??
    SG_Transform* parent = SG_GetTransform(t->parentID);
    if (!parent) return t->rot;

    glm::quat worldRot = t->rot;
    while (parent) {
        worldRot = parent->rot * worldRot;
        parent   = SG_GetTransform(parent->parentID);
    }
    return worldRot;
}

glm::vec3 SG_Transform::worldPosition(SG_Transform* t)
{
    SG_Transform* parent = SG_GetTransform(t->parentID);
    if (!parent) return t->pos;
    // multiply by parent's world matrix to get world position
    // DON'T multiply by this world matrix, because it double counts our own
    // transform
    return SG_Transform::worldMatrix(parent) * glm::vec4(t->pos, 1.0);
}

glm::vec3 SG_Transform::worldScale(SG_Transform* t)
{
    glm::vec3 scale      = t->sca;
    SG_Transform* parent = SG_GetTransform(t->parentID);
    while (parent) {
        scale *= parent->sca;
        parent = SG_GetTransform(parent->parentID);
    }
    return scale;
}

void SG_Transform::worldPosition(SG_Transform* t, glm::vec3 pos)
{
    SG_Transform* parent = SG_GetTransform(t->parentID);
    if (!parent)
        t->pos = pos;
    else
        // inverse matrix maps from world space --> local space
        t->pos = glm::inverse(SG_Transform::worldMatrix(parent)) * glm::vec4(pos, 1.0);
}

// doesn't set this object's position, only converts a local position to world
glm::vec3 SG_Transform::localToWorldPosition(SG_Transform* t, glm::vec3 pos)
{
    return SG_Transform::worldMatrix(t) * glm::vec4(pos, 1.0);
}

void SG_Transform::worldScale(SG_Transform* t, glm::vec3 scale)
{
    SG_Transform* parent = SG_GetTransform(t->parentID);
    if (!parent)
        t->sca = scale;
    else
        t->sca = scale / SG_Transform::worldScale(parent);
}

#define _SG_XFORM_DIRECTION(t, dir)                                                    \
    (glm::normalize(glm::rotate(SG_Transform::worldRotation(t), (dir))))

// get the forward direction in world space
glm::vec3 SG_Transform::forward(SG_Transform* t)
{
    return _SG_XFORM_DIRECTION(t, VEC_FORWARD);
}

// get the right direction in world space
glm::vec3 SG_Transform::right(SG_Transform* t)
{
    return _SG_XFORM_DIRECTION(t, VEC_RIGHT);
}

// get the up direction in world space
glm::vec3 SG_Transform::up(SG_Transform* t)
{
    return _SG_XFORM_DIRECTION(t, VEC_UP);
}
#undef _SG_XFORM_DIRECTION

void SG_Transform::rotateOnLocalAxis(SG_Transform* t, glm::vec3 axis, float rad)
{
    // just flip the order of multiplication to go from local <--> world. so
    // elegant...
    t->rot = t->rot * glm::angleAxis(rad, glm::normalize(axis));
}

void SG_Transform::rotateOnWorldAxis(SG_Transform* t, glm::vec3 axis, float rad)
{
    t->rot = glm::angleAxis(rad, glm::normalize(axis)) * t->rot;
}

void SG_Transform::rotateX(SG_Transform* t, float deg)
{
    SG_Transform::rotateOnLocalAxis(t, VEC_RIGHT, deg);
}

void SG_Transform::rotateY(SG_Transform* t, float deg)
{
    SG_Transform::rotateOnLocalAxis(t, VEC_UP, deg);
}

void SG_Transform::rotateZ(SG_Transform* t, float deg)
{
    SG_Transform::rotateOnLocalAxis(t, VEC_FORWARD, deg);
}

// rotates object to point towards position, updates
void SG_Transform::lookAt(SG_Transform* t, glm::vec3 pos, glm::vec3 up = VEC_UP)
{
    // first check colinearity
    glm::vec3 eye     = SG_Transform::worldPosition(t);
    glm::vec3 forward = glm::normalize(pos - eye);
    up                = glm::normalize(up);
    b32 collinear     = (glm::length2(glm::cross(up, forward)) < .00001);
    if (collinear) {
        log_warn(
          "GGen.lookAt(@(%f, %f, %f), @(%f, %f, %f)) failed because the forward and up "
          "vectors are collinear. Try GGen.lookAt(vec3 pos, vec3 up) with a different "
          "up vector",
          pos.x, pos.y, pos.z, up.x, up.y, up.z);
        return;
    }

    SG_Transform* parent = SG_GetTransform(t->parentID);

    glm::quat abs_rotation = glm::conjugate(
      glm::toQuat(glm::lookAt(SG_Transform::worldPosition(t), pos, up)));
    glm::quat local_rotation = abs_rotation;

    // convert into relative local rotation based on parent transforms
    if (parent)
        local_rotation
          = glm::inverse(SG_Transform::worldRotation(parent)) * abs_rotation;

    t->rot = local_rotation;
}

void SG_Transform::addChild(SG_Transform* parent, SG_Transform* child)
{
    // Object cannot be added as child of itself
    if (parent == child) {
        std::cerr << "GGen cannot be added as child of itself" << std::endl;
        return;
    }

    if (parent == NULL || child == NULL) {
        std::cerr << "cannot add NULL parent or child GGen" << std::endl;
        return;
    }

    if (child->type == SG_COMPONENT_SCENE) { // necessary to prevent cycles
        std::cerr << "cannot add make GScene a child of another GGen" << std::endl;
        return;
    }

    if (SG_Transform::isAncestor(child, parent)) {
        // std::cerr
        //   << "No cycles in scenegraph; cannot add parent as child of
        //   descendent"
        //   << std::endl;
        return;
    }

    // we are already the parent, do nothing
    if (child->parentID == parent->id) return;

    // remove child from old parent
    if (child->parentID) {
        SG_Transform* prevParent = SG_GetTransform(child->parentID);
        SG_Transform::removeChild(prevParent, child);
    }

    // assign to new parent
    child->parentID = parent->id;

    // reference count
    SG_AddRef(parent);

    // add to list of children
    SG_ID* xformID = ARENA_PUSH_TYPE(&parent->childrenIDs, SG_ID);
    *xformID       = child->id;

    // add ref to kid
    SG_AddRef(child);

    // loop over child subgraph, add any lights to scene
    static Arena sg_id_arena{};
    ASSERT(sg_id_arena.curr == 0);
    defer(Arena::clear(&sg_id_arena));
    if (parent->scene_id != 0) {
        SG_Scene* scene                       = SG_GetScene(parent->scene_id);
        *ARENA_PUSH_TYPE(&sg_id_arena, SG_ID) = child->id;
        while (ARENA_LENGTH(&sg_id_arena, SG_ID) > 0) {
            SG_ID* sg_id = ARENA_GET_LAST_TYPE(&sg_id_arena, SG_ID);
            ARENA_POP_TYPE(&sg_id_arena, SG_ID);
            SG_Transform* sg = SG_GetTransform(*sg_id);

            sg->scene_id = parent->scene_id;
            if (sg->type == SG_COMPONENT_LIGHT) {
                SG_Scene::addLight(scene, sg->id);
            }

            // add children to queue
            size_t num_children = SG_Transform::numChildren(sg);
            for (size_t i = 0; i < num_children; ++i) {
                *ARENA_PUSH_TYPE(&sg_id_arena, SG_ID)
                  = *ARENA_GET_TYPE(&sg->childrenIDs, SG_ID, i);
            }
        }
    }
}

static void SG_Transform_removeChildSubgraph(SG_Transform* parent, SG_Transform* child)
{
    static Arena sg_id_arena{};
    ASSERT(sg_id_arena.curr == 0);
    defer(Arena::clear(&sg_id_arena));
    if (parent->scene_id != 0) {
        SG_Scene* scene                       = SG_GetScene(parent->scene_id);
        *ARENA_PUSH_TYPE(&sg_id_arena, SG_ID) = child->id;
        while (sg_id_arena.curr > 0) {
            SG_ID* sg_id = ARENA_GET_LAST_TYPE(&sg_id_arena, SG_ID);
            ARENA_POP_TYPE(&sg_id_arena, SG_ID);
            SG_Transform* sg = SG_GetTransform(*sg_id);

            // unset scene id
            ASSERT(sg->scene_id == parent->scene_id);
            sg->scene_id = 0;

            // remove from light list
            if (sg->type == SG_COMPONENT_LIGHT) {
                SG_Scene::removeLight(scene, sg->id);
            }

            // add children to queue
            size_t num_children = SG_Transform::numChildren(sg);
            for (size_t i = 0; i < num_children; ++i) {
                *ARENA_PUSH_TYPE(&sg_id_arena, SG_ID)
                  = *ARENA_GET_TYPE(&sg->childrenIDs, SG_ID, i);
            }
        }
    }
}

void SG_Transform::removeChild(SG_Transform* parent, SG_Transform* child)
{
    if (child->parentID != parent->id) return;

    size_t numChildren = ARENA_LENGTH(&parent->childrenIDs, SG_ID);
    SG_ID* children    = (SG_ID*)parent->childrenIDs.base;

    child->parentID = 0;

    // ==optimize== flat_map instead of linear search
    for (size_t i = 0; i < numChildren; ++i) {
        if (children[i] == child->id) {
            // swap with last element
            children[i] = children[numChildren - 1];
            // pop last element
            Arena::pop(&parent->childrenIDs, sizeof(SG_ID));

            // release ref count on child's chuck object; one less reference to
            // it from us (parent)
            SG_DecrementRef(child->id);

            // release ref count on our (parent's) chuck object; one less
            // reference to it from child
            SG_DecrementRef(parent->id);
            break;
        }
    }

    SG_Transform_removeChildSubgraph(parent, child);
}

void SG_Transform::removeAllChildren(SG_Transform* parent)
{
    size_t numChildren = ARENA_LENGTH(&parent->childrenIDs, SG_ID);
    SG_ID* children    = (SG_ID*)parent->childrenIDs.base;

    for (size_t i = 0; i < numChildren; ++i) {
        // release ref count on child's chuck object; one less reference to
        // it from us (parent)
        SG_DecrementRef(children[i]);

        // release ref count on our (parent's) chuck object; one less
        // reference to it from child
        SG_DecrementRef(parent->id);

        // remove child from parent
        SG_Transform* child = SG_GetTransform(children[i]);
        SG_Transform_removeChildSubgraph(parent, child);
    }
    Arena::clear(&parent->childrenIDs);
}

bool SG_Transform::isAncestor(SG_Transform* ancestor, SG_Transform* descendent)
{
    while (descendent != NULL) {
        if (descendent == ancestor) return true;
        descendent = SG_GetTransform(descendent->parentID);
    }
    return false;
}

size_t SG_Transform::numChildren(SG_Transform* t)
{
    return ARENA_LENGTH(&t->childrenIDs, SG_ID);
}

SG_Transform* SG_Transform::child(SG_Transform* t, size_t index)
{
    if (index >= numChildren(t)) return NULL;
    return SG_GetTransform(*ARENA_GET_TYPE(&t->childrenIDs, SG_ID, index));
}

// ============================================================================
// SG_Sampler Definitions
// ============================================================================

SG_Sampler SG_Sampler::fromCkObj(Chuck_Object* ckobj)
{
    if (!ckobj) return SG_SAMPLER_DEFAULT;

    CK_DL_API API      = g_chuglAPI;
    SG_Sampler sampler = SG_SAMPLER_DEFAULT;

    sampler.wrapU = (SG_Sampler_WrapMode)OBJ_MEMBER_INT(ckobj, sampler_offset_wrapU);
    sampler.wrapV = (SG_Sampler_WrapMode)OBJ_MEMBER_INT(ckobj, sampler_offset_wrapV);
    sampler.wrapW = (SG_Sampler_WrapMode)OBJ_MEMBER_INT(ckobj, sampler_offset_wrapW);
    sampler.filterMin
      = (SG_Sampler_FilterMode)OBJ_MEMBER_INT(ckobj, sampler_offset_filterMin);
    sampler.filterMag
      = (SG_Sampler_FilterMode)OBJ_MEMBER_INT(ckobj, sampler_offset_filterMag);
    sampler.filterMip
      = (SG_Sampler_FilterMode)OBJ_MEMBER_INT(ckobj, sampler_offset_filterMip);
    return sampler;
}

// ============================================================================
// SG_Geometry Definitions
// ============================================================================

// TODO delete function
// void SG_Geometry::_init(SG_Geometry* g, SG_GeometryType geo_type, void* params)
// {
//     g->geo_type = geo_type;
//     switch (geo_type) {
//         case SG_GEOMETRY_PLANE: {
//             PlaneParams* p  = (PlaneParams*)params;
//             g->params.plane = *p;
//         } break;
//         case SG_GEOMETRY_SPHERE: {
//             SphereParams* p  = (SphereParams*)params;
//             g->params.sphere = *p;
//         } break;
//         case SG_GEOMETRY: {
//             // custom geometry
//         } break;
//         default: ASSERT(false);
//     }
// }

void SG_Geometry::initGABandNumComponents(GeometryArenaBuilder* b, SG_Geometry* g,
                                          bool clear = true)
{
    // set arena pointers
    b->pos_arena  = &g->vertex_attribute_data[SG_GEOMETRY_POSITION_ATTRIBUTE_LOCATION];
    b->norm_arena = &g->vertex_attribute_data[SG_GEOMETRY_NORMAL_ATTRIBUTE_LOCATION];
    b->uv_arena   = &g->vertex_attribute_data[SG_GEOMETRY_UV_ATTRIBUTE_LOCATION];
    b->indices_arena = &g->indices;

    // clear arenas
    if (clear) {
        Arena::clear(b->pos_arena);
        Arena::clear(b->norm_arena);
        Arena::clear(b->uv_arena);
        Arena::clear(b->indices_arena);
    }

    // set num components
    ZERO_ARRAY(g->vertex_attribute_num_components);
    g->vertex_attribute_num_components[SG_GEOMETRY_POSITION_ATTRIBUTE_LOCATION] = 3;
    g->vertex_attribute_num_components[SG_GEOMETRY_NORMAL_ATTRIBUTE_LOCATION]   = 3;
    g->vertex_attribute_num_components[SG_GEOMETRY_UV_ATTRIBUTE_LOCATION]       = 2;
}

void SG_Geometry::buildPlane(SG_Geometry* g, PlaneParams* p)
{
    ASSERT(g->geo_type == SG_GEOMETRY_PLANE);
    g->params.plane = *p;

    GeometryArenaBuilder gab;
    SG_Geometry::initGABandNumComponents(&gab, g);
    Geometry_buildPlane(&gab, p);
}

void SG_Geometry::buildSphere(SG_Geometry* g, SphereParams* p)
{
    ASSERT(g->geo_type == SG_GEOMETRY_SPHERE);
    g->params.sphere = *p;

    GeometryArenaBuilder gab;
    SG_Geometry::initGABandNumComponents(&gab, g);
    Geometry_buildSphere(&gab, p);
}

void SG_Geometry::buildSuzanne(SG_Geometry* g)
{
    ASSERT(g->geo_type == SG_GEOMETRY_SUZANNE);
    GeometryArenaBuilder gab;
    SG_Geometry::initGABandNumComponents(&gab, g);
    Geometry_buildSuzanne(&gab);
}

void SG_Geometry::buildKnot(SG_Geometry* g, KnotParams* p)
{
    ASSERT(g->geo_type == SG_GEOMETRY_KNOT);
    g->params.knot = *p;

    GeometryArenaBuilder gab;
    SG_Geometry::initGABandNumComponents(&gab, g);
    Geometry_buildKnot(&gab, p);
}

void SG_Geometry::buildPolygon(SG_Geometry* g, PolygonParams* p)
{
    ASSERT(g->geo_type == SG_GEOMETRY_POLYGON);
    g->params.polygon = *p;

    GeometryArenaBuilder gab;
    SG_Geometry::initGABandNumComponents(&gab, g);
    Geometry_buildPolygon(&gab, p);
}

void SG_Geometry::buildBox(SG_Geometry* g, BoxParams* p)
{
    ASSERT(g->geo_type == SG_GEOMETRY_CUBE);
    g->params.box = *p;

    GeometryArenaBuilder gab;
    SG_Geometry::initGABandNumComponents(&gab, g);
    Geometry_buildBox(&gab, p);
}

void SG_Geometry::buildCircle(SG_Geometry* g, CircleParams* p)
{
    ASSERT(g->geo_type == SG_GEOMETRY_CIRCLE);
    g->params.circle = *p;

    GeometryArenaBuilder gab;
    SG_Geometry::initGABandNumComponents(&gab, g);
    Geometry_buildCircle(&gab, p);
}

void SG_Geometry::buildTorus(SG_Geometry* g, TorusParams* p)
{
    ASSERT(g->geo_type == SG_GEOMETRY_TORUS);
    g->params.torus = *p;

    GeometryArenaBuilder gab;
    SG_Geometry::initGABandNumComponents(&gab, g);
    Geometry_buildTorus(&gab, p);
}

void SG_Geometry::buildCylinder(SG_Geometry* g, CylinderParams* p)
{
    ASSERT(g->geo_type == SG_GEOMETRY_CYLINDER);
    g->params.cylinder = *p;

    GeometryArenaBuilder gab;
    SG_Geometry::initGABandNumComponents(&gab, g);
    Geometry_buildCylinder(&gab, p);
}

u32 SG_Geometry::vertexCount(SG_Geometry* geo)
{
    if (geo->vertex_attribute_num_components[0] == 0) return 0;
    return ARENA_LENGTH(&geo->vertex_attribute_data[0], f32)
           / geo->vertex_attribute_num_components[0];
}

u32 SG_Geometry::indexCount(SG_Geometry* geo)
{
    return ARENA_LENGTH(&geo->indices, u32);
}

Arena* SG_Geometry::setAttribute(SG_Geometry* geo, int location, int num_components,
                                 CK_DL_API api, Chuck_Object* ck_array,
                                 int ck_array_num_components, bool is_int)
{
    ASSERT(location < SG_GEOMETRY_MAX_VERTEX_ATTRIBUTES && location >= 0);
    ASSERT(num_components >= 0);

    Arena* arena = &geo->vertex_attribute_data[location];
    Arena::clear(arena);
    geo->vertex_attribute_num_components[location] = num_components;

    if (is_int) {
        ASSERT(ck_array_num_components == 1);
        // write ck_array data to arena
        int ck_arr_len  = api->object->array_int_size((Chuck_ArrayInt*)ck_array);
        i32* arena_data = ARENA_PUSH_COUNT(arena, i32, ck_arr_len);
        for (int i = 0; i < ck_arr_len; i++)
            arena_data[i]
              = (i32)api->object->array_int_get_idx((Chuck_ArrayInt*)ck_array, i);
        ASSERT(ARENA_LENGTH(arena, i32) == ck_arr_len);
    } else {
        switch (ck_array_num_components) {
            case 1: {
                int ck_arr_len
                  = api->object->array_float_size((Chuck_ArrayFloat*)ck_array);
                f32* arena_data = ARENA_PUSH_COUNT(arena, f32, ck_arr_len);
                for (int i = 0; i < ck_arr_len; i++)
                    arena_data[i] = (f32)api->object->array_float_get_idx(
                      (Chuck_ArrayFloat*)ck_array, i);
                ASSERT(ARENA_LENGTH(arena, f32) == ck_arr_len);
            } break;
            case 2: {
                int ck_arr_len
                  = api->object->array_vec2_size((Chuck_ArrayVec2*)ck_array);
                glm::vec2* arena_data = ARENA_PUSH_COUNT(arena, glm::vec2, ck_arr_len);
                for (int i = 0; i < ck_arr_len; i++) {
                    t_CKVEC2 v
                      = api->object->array_vec2_get_idx((Chuck_ArrayVec2*)ck_array, i);
                    arena_data[i] = { v.x, v.y };
                }
                ASSERT(ARENA_LENGTH(arena, glm::vec2) == ck_arr_len);
            } break;
            case 3: {
                int ck_arr_len
                  = api->object->array_vec3_size((Chuck_ArrayVec3*)ck_array);
                glm::vec3* arena_data = ARENA_PUSH_COUNT(arena, glm::vec3, ck_arr_len);
                for (int i = 0; i < ck_arr_len; i++) {
                    t_CKVEC3 v
                      = api->object->array_vec3_get_idx((Chuck_ArrayVec3*)ck_array, i);
                    arena_data[i] = { v.x, v.y, v.z };
                }
                ASSERT(ARENA_LENGTH(arena, glm::vec3) == ck_arr_len);
            } break;
            case 4: {
                int ck_arr_len
                  = api->object->array_vec4_size((Chuck_ArrayVec4*)ck_array);
                glm::vec4* arena_data = ARENA_PUSH_COUNT(arena, glm::vec4, ck_arr_len);
                for (int i = 0; i < ck_arr_len; i++) {
                    t_CKVEC4 v
                      = api->object->array_vec4_get_idx((Chuck_ArrayVec4*)ck_array, i);
                    arena_data[i] = { v.x, v.y, v.z, v.w };
                }
                ASSERT(ARENA_LENGTH(arena, glm::vec4) == ck_arr_len);
            } break;
            default: {
                ASSERT(false);
            }
        }
    }
    return arena;
}

u32* SG_Geometry::setIndices(SG_Geometry* geo, CK_DL_API API, Chuck_ArrayInt* indices,
                             int index_count)
{
    Arena::clear(&geo->indices);

    u32* arena_data = ARENA_PUSH_COUNT(&geo->indices, u32, index_count);

    for (int i = 0; i < index_count; i++)
        arena_data[i] = (u32)API->object->array_int_get_idx(indices, i);

    return arena_data;
}

u32* SG_Geometry::getIndices(SG_Geometry* geo)
{
    return (u32*)geo->indices.base;
}

f32* SG_Geometry::getAttributeData(SG_Geometry* geo, int location)
{
    return (f32*)geo->vertex_attribute_data[location].base;
}

// ============================================================================
// SG_Mesh
// ============================================================================

void SG_Mesh::setGeometry(SG_Mesh* mesh, SG_Geometry* geo)
{
    SG_ID oldGeoID = mesh->_geo_id;

    // bump ref on new geometry
    SG_AddRef(geo);

    // assign new geometry
    mesh->_geo_id = geo ? geo->id : 0;

    // decrement ref on old geometry
    SG_DecrementRef(oldGeoID);
}

void SG_Mesh::setMaterial(SG_Mesh* mesh, SG_Material* mat)
{
    SG_ID oldMatID = mesh->_mat_id;

    // bump ref on new material
    SG_AddRef(mat);

    // assign new material
    mesh->_mat_id = mat ? mat->id : 0;

    // decrement ref on old material
    SG_DecrementRef(oldMatID);
}

// ============================================================================
// SG Component Manager Definitions
// ============================================================================

// chugin API pointers
static const Chuck_DL_Api* _ck_api = NULL;

// GC arenas (TODO move into struct)
static Arena _gc_queue_a;
static Arena _gc_queue_b;
static Arena* _gc_queue_read  = &_gc_queue_a;
static Arena* _gc_queue_write = &_gc_queue_b;

// storage arenas
static Arena SG_XformArena;
static Arena SG_SceneArena;
static Arena SG_GeoArena;
static Arena SG_ShaderArena;
static Arena SG_MaterialArena;
static Arena SG_MeshArena;
static Arena SG_TextureArena;
static Arena SG_CameraArena;
static Arena SG_TextArena;
static Arena SG_PassArena;
static Arena SG_BufferArena;
static Arena SG_LightArena;
static Arena SG_VideoArena;
static Arena SG_WebcamArena;

// locators (TODO switch to table)
static hashmap* locator = NULL;

// hashmap item for lookup
struct SG_Location {
    SG_ID id;      // key
    size_t offset; // value (byte offset into arena)
    Arena* arena;  // where to find
};

static int _SG_CompareLocation(const void* a, const void* b, void* udata)
{
    SG_Location* locA = (SG_Location*)a;
    SG_Location* locB = (SG_Location*)b;
    return locA->id - locB->id;
}

static u64 _SG_HashLocation(const void* item, uint64_t seed0, uint64_t seed1)
{
    // tested xxhash3 is best
    SG_Location* key = (SG_Location*)item;
    return hashmap_xxhash3(&key->id, sizeof(SG_ID), seed0, seed1);
    // return hashmap_sip(item, sizeof(int), seed0, seed1);
    // return hashmap_murmur(item, sizeof(int), seed0, seed1);
}

// state
static SG_ID SG_NextComponentID = 1; // 0 is reserved for NULL

static SG_ID SG_GetNewComponentID()
{
    return SG_NextComponentID++;
}

int SG_Texture_numComponentsPerTexel(WGPUTextureFormat format)
{
    switch (format) {
        case WGPUTextureFormat_RGBA8Unorm:
        case WGPUTextureFormat_RGBA16Float:
        case WGPUTextureFormat_RGBA32Float: {
            return 4;
        } break;
        case WGPUTextureFormat_R32Float: {
            return 1;
        } break;
        default: ASSERT(false);
    }
    return 0;
}

int SG_Texture_byteSizePerTexel(WGPUTextureFormat format)
{
    switch (format) {
        case WGPUTextureFormat_RGBA8Unorm: return 4;
        case WGPUTextureFormat_RGBA16Float: return 8;
        case WGPUTextureFormat_RGBA32Float: return 16;
        case WGPUTextureFormat_R32Float: return 4;
        default: ASSERT(false);
    }
    return 0;
}

void SG_Init(const Chuck_DL_Api* api)
{
    _ck_api = api;

    int seed = time(NULL);
    srand(seed);
    locator = hashmap_new(sizeof(SG_Location), 0, seed, seed, _SG_HashLocation,
                          _SG_CompareLocation, NULL, NULL);

    Arena::init(&SG_XformArena, sizeof(SG_Transform) * 64);
    Arena::init(&SG_SceneArena, sizeof(SG_Scene) * 64);
    Arena::init(&SG_GeoArena, sizeof(SG_Geometry) * 32);
    Arena::init(&SG_MaterialArena, sizeof(SG_Material) * 32);
    Arena::init(&SG_MeshArena, sizeof(SG_Mesh) * 64);
    Arena::init(&SG_TextureArena, sizeof(SG_Texture) * 32);
    Arena::init(&SG_CameraArena, sizeof(SG_Camera) * 4);
    Arena::init(&SG_TextArena, sizeof(SG_Text) * 32);
    Arena::init(&SG_PassArena, sizeof(SG_Pass) * 32);
    Arena::init(&SG_BufferArena, sizeof(SG_Pass) * 64);
    Arena::init(&SG_LightArena, sizeof(SG_Light) * 32);
    Arena::init(&SG_VideoArena, sizeof(SG_Video) * 16);
    Arena::init(&SG_WebcamArena, sizeof(SG_Webcam) * 8);

    // init gc state
    Arena::init(&_gc_queue_a, sizeof(SG_ID) * 64);
    Arena::init(&_gc_queue_b, sizeof(SG_ID) * 64);
}

void SG_Free()
{
    _ck_api = NULL;

    // TODO call free() on the components themselves
    Arena::free(&SG_XformArena);
    Arena::free(&SG_SceneArena);
    Arena::free(&SG_GeoArena);
    Arena::free(&SG_MaterialArena);
    Arena::free(&SG_MeshArena);
    Arena::free(&SG_TextureArena);
    Arena::free(&SG_CameraArena);
    Arena::free(&SG_TextArena);

    hashmap_free(locator);
    locator = NULL;

    // free gc state
    Arena::free(&_gc_queue_a);
    Arena::free(&_gc_queue_b);
}

SG_Transform* SG_CreateTransform(Chuck_Object* ckobj)
{
    size_t offset       = SG_XformArena.curr;
    SG_Transform* xform = ARENA_PUSH_TYPE(&SG_XformArena, SG_Transform);
    *xform              = {};
    SG_Transform::_init(xform, ckobj);

    xform->id   = SG_GetNewComponentID();
    xform->type = SG_COMPONENT_TRANSFORM;

    // store in map
    SG_Location loc = { xform->id, offset, &SG_XformArena };
    hashmap_set(locator, &loc);

    return xform;
}

SG_Scene* SG_CreateScene(Chuck_Object* ckobj)
{
    Arena* arena    = &SG_SceneArena;
    size_t offset   = arena->curr;
    SG_Scene* scene = ARENA_PUSH_TYPE(arena, SG_Scene);
    *scene          = {};

    // transform init
    SG_Transform::_init(scene, ckobj);

    // sg_component init
    scene->id       = SG_GetNewComponentID();
    scene->type     = SG_COMPONENT_SCENE;
    scene->scene_id = scene->id; // a scene always belongs to itself

    // scene init
    Arena::init(&scene->light_ids, sizeof(SG_ID) * 8);

    // store in map
    SG_Location loc = { scene->id, offset, arena };
    hashmap_set(locator, &loc);

    return scene;
}

SG_Geometry* SG_CreateGeometry(Chuck_Object* ckobj)
{
    Arena* arena     = &SG_GeoArena;
    size_t offset    = arena->curr;
    SG_Geometry* geo = ARENA_PUSH_ZERO_TYPE(arena, SG_Geometry);

    geo->id    = SG_GetNewComponentID();
    geo->type  = SG_COMPONENT_GEOMETRY;
    geo->ckobj = ckobj;

    // store in map
    SG_Location loc = { geo->id, offset, arena };
    hashmap_set(locator, &loc);

    return geo;
}

SG_Texture* SG_CreateTexture(SG_TextureDesc* desc, Chuck_Object* ckobj,
                             Chuck_VM_Shred* shred, bool add_ref, const char* name)
{
    CK_DL_API API = g_chuglAPI;

    // create ckobj
    // NOTE: reference counting here depends on context; e.g., add_ref would false if
    // the texture created is to be returned (without retain)
    Chuck_Object* texture_obj
      = ckobj ? ckobj :
                chugin_createCkObj(SG_CKNames[SG_COMPONENT_TEXTURE], add_ref, shred);

    // create chuck event
    Chuck_Event* texture_read_event
      = (Chuck_Event*)chugin_createCkObj("Event", true, shred);

    // create default empty texture data
    Chuck_ArrayFloat* texture_read_data = chugin_createCkFloatArray(NULL, 0, true);

    // create chugl obj
    Arena* arena    = &SG_TextureArena;
    size_t offset   = arena->curr;
    SG_Texture* tex = ARENA_PUSH_TYPE(arena, SG_Texture);
    *tex            = {};
    tex->desc       = *desc;

    tex->texture_read_event = texture_read_event;
    tex->texture_data       = texture_read_data;

    { // validate texture desc
        // clamp size values
        tex->desc.width  = MAX(tex->desc.width, 1);
        tex->desc.height = MAX(tex->desc.height, 1);
        tex->desc.depth  = MAX(tex->desc.depth, 1);

        // enforce usage_flags are a subset of ALL
        tex->desc.usage &= WGPUTextureUsage_All;

        // if usage_flags is 0, default to ALL
        if (tex->desc.usage == 0) {
            tex->desc.usage = WGPUTextureUsage_All;
        }

        tex->desc.gen_mips = desc->gen_mips ? true : false;
    }

    // init SG_Component base class
    tex->id                                          = SG_GetNewComponentID();
    tex->type                                        = SG_COMPONENT_TEXTURE;
    tex->ckobj                                       = texture_obj;
    OBJ_MEMBER_UINT(tex->ckobj, component_offset_id) = tex->id;
    if (name) COPY_STRING(tex->name, name);

    // store in map
    SG_Location loc = { tex->id, offset, arena };
    hashmap_set(locator, &loc);

    // create
    CQ_PushCommand_TextureCreate(tex);

    return tex;
}

SG_Camera* SG_CreateCamera(Chuck_Object* ckobj, SG_CameraParams cam_params)
{
    Arena* arena   = &SG_CameraArena;
    size_t offset  = arena->curr;
    SG_Camera* cam = ARENA_PUSH_ZERO_TYPE(arena, SG_Camera);
    *cam           = {};
    SG_Transform::_init(cam, ckobj);

    // copy camera params
    cam->params = cam_params;

    // init SG_Component base class
    cam->id    = SG_GetNewComponentID();
    cam->type  = SG_COMPONENT_CAMERA;
    cam->ckobj = ckobj;

    // store in map
    SG_Location loc = { cam->id, offset, arena };
    hashmap_set(locator, &loc);

    return cam;
}

SG_Text* SG_CreateText(Chuck_Object* ckobj)
{
    Arena* arena  = &SG_TextArena;
    size_t offset = arena->curr;
    SG_Text* text = ARENA_PUSH_ZERO_TYPE(arena, SG_Text);
    *text         = {};
    SG_Transform::_init(text, ckobj);

    // init SG_Component base class
    text->id    = SG_GetNewComponentID();
    text->type  = SG_COMPONENT_MESH;
    text->ckobj = ckobj;

    // store in map
    SG_Location loc = { text->id, offset, arena };
    hashmap_set(locator, &loc);

    return text;
}

SG_Pass* SG_CreatePass(Chuck_Object* ckobj, SG_PassType pass_type)
{
    Arena* arena  = &SG_PassArena;
    size_t offset = arena->curr;
    SG_Pass* pass = ARENA_PUSH_TYPE(arena, SG_Pass);
    *pass         = {};

    // init SG_Component base class
    pass->id        = SG_GetNewComponentID();
    pass->type      = SG_COMPONENT_PASS;
    pass->ckobj     = ckobj;
    pass->pass_type = pass_type;

    // store in map
    SG_Location loc = { pass->id, offset, arena };
    hashmap_set(locator, &loc);

    return pass;
}

SG_Buffer* SG_CreateBuffer(Chuck_Object* ckobj)
{
    Arena* arena      = &SG_BufferArena;
    size_t offset     = arena->curr;
    SG_Buffer* buffer = ARENA_PUSH_TYPE(arena, SG_Buffer);
    *buffer           = {};

    // init SG_Component base class
    buffer->id    = SG_GetNewComponentID();
    buffer->type  = SG_COMPONENT_BUFFER;
    buffer->ckobj = ckobj;

    // store in map
    SG_Location loc = { buffer->id, offset, arena };
    hashmap_set(locator, &loc);

    return buffer;
}

SG_Shader* SG_CreateShader(Chuck_Object* ckobj, const char* vertex_string,
                           const char* fragment_string, const char* vertex_filepath,
                           const char* fragment_filepath,
                           WGPUVertexFormat* vertex_layout, int vertex_layout_len,
                           const char* compute_string, const char* compute_filepath,
                           SG_ShaderIncludes includes)
{
#define NULL_TO_EMPTY(s) (s ? s : "")
    vertex_string     = NULL_TO_EMPTY(vertex_string);
    fragment_string   = NULL_TO_EMPTY(fragment_string);
    vertex_filepath   = NULL_TO_EMPTY(vertex_filepath);
    fragment_filepath = NULL_TO_EMPTY(fragment_filepath);
    compute_string    = NULL_TO_EMPTY(compute_string);
    compute_filepath  = NULL_TO_EMPTY(compute_filepath);

    Arena* arena      = &SG_ShaderArena;
    size_t offset     = arena->curr;
    SG_Shader* shader = ARENA_PUSH_TYPE(arena, SG_Shader);
    *shader           = {};

    // set base component values
    shader->id    = SG_GetNewComponentID();
    shader->type  = SG_COMPONENT_SHADER;
    shader->ckobj = ckobj;

    // set shader values
    shader->vertex_string_owned     = strdup(vertex_string);
    shader->fragment_string_owned   = strdup(fragment_string);
    shader->vertex_filepath_owned   = strdup(vertex_filepath);
    shader->fragment_filepath_owned = strdup(fragment_filepath);

    vertex_layout_len = MIN(vertex_layout_len, ARRAY_LENGTH(shader->vertex_layout));
    memcpy(shader->vertex_layout, vertex_layout,
           sizeof(shader->vertex_layout[0]) * vertex_layout_len);

    shader->compute_string_owned   = strdup(compute_string);
    shader->compute_filepath_owned = strdup(compute_filepath);

    shader->includes = includes;

    // store in map
    SG_Location loc = { shader->id, offset, arena };
    hashmap_set(locator, &loc);

    return shader;

#undef NULL_TO_EMPTY
}

SG_Material* SG_CreateMaterial(Chuck_Object* ckobj, SG_MaterialType material_type)
{
    Arena* arena     = &SG_MaterialArena;
    size_t offset    = arena->curr;
    SG_Material* mat = ARENA_PUSH_ZERO_TYPE(arena, SG_Material);

    mat->ckobj         = ckobj;
    mat->id            = SG_GetNewComponentID();
    mat->type          = SG_COMPONENT_MATERIAL;
    mat->material_type = material_type;
    mat->pso           = {};

    // switch (material_type) {
    //     case SG_MATERIAL_PBR: {
    //         if (params == NULL) {
    //             // copy default values
    //             SG_Material_PBR_Params p = {};
    //             COPY_STRUCT(&mat->params.pbr, &p, SG_Material_PBR_Params);
    //         } else {
    //             COPY_STRUCT(&mat->params.pbr, (SG_Material_PBR_Params*)params,
    //                         SG_Material_PBR_Params);
    //         }
    //     } break;
    //     default: ASSERT(false);
    // }

    // store in map
    SG_Location loc = { mat->id, offset, arena };
    hashmap_set(locator, &loc);

    return mat;
}

SG_Mesh* SG_CreateMesh(Chuck_Object* ckobj, SG_Geometry* sg_geo, SG_Material* sg_mat)
{
    Arena* arena  = &SG_MeshArena;
    size_t offset = arena->curr;
    SG_Mesh* mesh = ARENA_PUSH_TYPE(arena, SG_Mesh);
    *mesh         = {};

    // init transform
    SG_Transform::_init(mesh, ckobj);

    // init component
    mesh->ckobj = ckobj;
    mesh->id    = SG_GetNewComponentID();
    mesh->type  = SG_COMPONENT_MESH;

    // init mesh
    SG_Mesh::setGeometry(mesh, sg_geo);
    SG_Mesh::setMaterial(mesh, sg_mat);
    mesh->receives_shadows = 0;

    // store in map
    SG_Location loc = { mesh->id, offset, arena };
    hashmap_set(locator, &loc);

    return mesh;
}

SG_Light* SG_CreateLight(Chuck_Object* ckobj)
{
    Arena* arena    = &SG_LightArena;
    size_t offset   = arena->curr;
    SG_Light* light = ARENA_PUSH_TYPE(arena, SG_Light);
    *light          = {};

    // init base component
    light->ckobj = ckobj;
    light->id    = SG_GetNewComponentID();
    light->type  = SG_COMPONENT_LIGHT;

    // init transform data
    SG_Transform::_init(light, ckobj);

    // store in map
    SG_Location loc = { light->id, offset, arena };
    hashmap_set(locator, &loc);

    return light;
}

SG_Video* SG_CreateVideo(Chuck_Object* ckobj, t_CKUINT id_offset)
{
    CK_DL_API API = g_chuglAPI;

    Arena* arena    = &SG_VideoArena;
    size_t offset   = arena->curr;
    SG_Video* video = ARENA_PUSH_ZERO_TYPE(arena, SG_Video);
    *video          = {};

    // init base component
    video->ckobj = ckobj;
    video->id    = SG_GetNewComponentID();
    video->type  = SG_COMPONENT_VIDEO;

    // set ckobj pointer (using id_offset because Video extends UGen and therefore
    // doesn't share same `component_offset_id` as other scenegraph objects)
    OBJ_MEMBER_UINT(ckobj, id_offset) = video->id;

    // store in map
    SG_Location loc = { video->id, offset, arena };
    hashmap_set(locator, &loc);

    return video;
}

SG_Webcam* SG_CreateWebcam(Chuck_Object* ckobj, Chuck_VM_Shred* shred, int device_id,
                           int width, int height, int fps)
{
    CK_DL_API API = g_chuglAPI;

    Arena* arena      = &SG_WebcamArena;
    size_t offset     = arena->curr;
    SG_Webcam* webcam = ARENA_PUSH_TYPE(arena, SG_Webcam);
    *webcam           = {};

    // init base component
    webcam->ckobj = ckobj;
    webcam->id    = SG_GetNewComponentID();
    webcam->type  = SG_COMPONENT_WEBCAM;

    // set ckobj pointer
    OBJ_MEMBER_UINT(ckobj, component_offset_id) = webcam->id;

    // store in map
    SG_Location loc = { webcam->id, offset, arena };
    hashmap_set(locator, &loc);

    { // webcam init
        SG_Texture* webcam_texture = SG_GetTexture(g_builtin_textures.magenta_pixel_id);
        webcam->texture_id         = webcam_texture->id;
        webcam->device_id          = device_id;

        // bounds check (hardcoded to 8 webcams)
        if (device_id >= 8) {
            log_warn(
              "Only webcam device ids 0-7 are supported. Defaulting to magenta "
              "texture");
            return webcam;
        }

        sr_webcam_device* device;
        sr_webcam_create(&device, device_id);
        sr_webcam_set_format(device, width, height, fps);
        int webcam_open_success = sr_webcam_open(device) == 0;

        /*
        on macos, sr_webcam_delete causes segfault.
        furthermore, openning the same webcam multiple times, even if closing the
        earlier opens, causes webcam framerate to half from 30 -> 15. Probably an
        implementation bug in sr_webcam. workaround: given a webcam id, only ever open
        it once on the audio thread, and pass it to the graphics thread over the command
        queue.
        */

        if (!webcam_open_success) {
            log_warn("Could not open webcam device %d. Defaulting to magenta texture",
                     device_id);
        } else { // successful
            // Get back video parameters.
            int vidW, vidH, vidFps;
            sr_webcam_get_dimensions(device, &vidW, &vidH);
            sr_webcam_get_framerate(device, &vidFps);
            webcam->fps = vidFps;

            // if successful, create webcam texture
            SG_TextureDesc desc = {};
            desc.width          = vidW;
            desc.height         = vidH;
            desc.dimension      = WGPUTextureDimension_2D;
            desc.format         = WGPUTextureFormat_RGBA8Unorm;
            desc.usage          = WGPUTextureUsage_All;
            desc.gen_mips       = false; // no mipmaps for video

            webcam_texture     = SG_CreateTexture(&desc, NULL, shred, true);
            webcam->texture_id = webcam_texture->id;
            strncpy(webcam->device_name, sr_webcam_get_user_friendly_name(device),
                    sizeof(webcam->device_name));

            // if successful, create graphics component
            CQ_PushCommand_WebcamCreate(webcam, device);
        }
    }

    return webcam;
}

SG_Component* SG_GetComponent(SG_ID id)
{
    SG_Location key     = {};
    key.id              = id;
    SG_Location* result = (SG_Location*)hashmap_get(locator, &key);
    SG_Component* component
      = result ? (SG_Component*)Arena::get(result->arena, result->offset) : NULL;
    if (component) {
        ASSERT(result->id == id);
        ASSERT(component->id == id);
    }
    return component;
}

SG_Transform* SG_GetTransform(SG_ID id)
{
    SG_Component* component = SG_GetComponent(id);
    ASSERT(component == NULL || component->type == SG_COMPONENT_TRANSFORM
           || component->type == SG_COMPONENT_SCENE
           || component->type == SG_COMPONENT_MESH
           || component->type == SG_COMPONENT_CAMERA
           || component->type == SG_COMPONENT_LIGHT);
    return (SG_Transform*)component;
}

SG_Scene* SG_GetScene(SG_ID id)
{
    SG_Component* component = SG_GetComponent(id);
    ASSERT(component == NULL || component->type == SG_COMPONENT_SCENE);
    return (SG_Scene*)component;
}

SG_Geometry* SG_GetGeometry(SG_ID id)
{
    SG_Component* component = SG_GetComponent(id);
    ASSERT(component == NULL || component->type == SG_COMPONENT_GEOMETRY);
    return (SG_Geometry*)component;
}

SG_Shader* SG_GetShader(SG_ID id)
{
    SG_Component* component = SG_GetComponent(id);
    if (component) ASSERT(component->type == SG_COMPONENT_SHADER);
    return (SG_Shader*)component;
}

SG_Material* SG_GetMaterial(SG_ID id)
{
    SG_Component* component = SG_GetComponent(id);
    ASSERT(component == NULL || component->type == SG_COMPONENT_MATERIAL);
    return (SG_Material*)component;
}

SG_Mesh* SG_GetMesh(SG_ID id)
{
    SG_Component* component = SG_GetComponent(id);
    ASSERT(component == NULL || component->type == SG_COMPONENT_MESH);
    return (SG_Mesh*)component;
}

SG_Texture* SG_GetTexture(SG_ID id)
{
    SG_Component* component = SG_GetComponent(id);
    ASSERT(!component || component->type == SG_COMPONENT_TEXTURE);
    return (SG_Texture*)component;
}

SG_Camera* SG_GetCamera(SG_ID id)
{
    SG_Component* component = SG_GetComponent(id);
    ASSERT(component == NULL || component->type == SG_COMPONENT_CAMERA);
    return (SG_Camera*)component;
}

SG_Text* SG_GetText(SG_ID id)
{
    SG_Component* component = SG_GetComponent(id);
    ASSERT(component == NULL || component->type == SG_COMPONENT_MESH);
    return (SG_Text*)component;
}

SG_Pass* SG_GetPass(SG_ID id)
{
    SG_Component* component = SG_GetComponent(id);
    ASSERT(component == NULL || component->type == SG_COMPONENT_PASS);
    return (SG_Pass*)component;
}

SG_Buffer* SG_GetBuffer(SG_ID id)
{
    SG_Component* component = SG_GetComponent(id);
    ASSERT(component == NULL || component->type == SG_COMPONENT_BUFFER);
    return (SG_Buffer*)component;
}

SG_Light* SG_GetLight(SG_ID id)
{
    SG_Component* component = SG_GetComponent(id);
    ASSERT(component == NULL || component->type == SG_COMPONENT_LIGHT);
    return (SG_Light*)component;
}

SG_Video* SG_GetVideo(SG_ID id)
{
    SG_Component* component = SG_GetComponent(id);
    ASSERT(component == NULL || component->type == SG_COMPONENT_VIDEO);
    return (SG_Video*)component;
}

SG_Webcam* SG_GetWebcam(SG_ID id)
{
    SG_Component* component = SG_GetComponent(id);
    ASSERT(component == NULL || component->type == SG_COMPONENT_WEBCAM);
    return (SG_Webcam*)component;
}

// ============================================================================
// SG Garbage Collector
// ============================================================================

static void _SG_SwapGCQueues()
{
    Arena* temp     = _gc_queue_read;
    _gc_queue_read  = _gc_queue_write;
    _gc_queue_write = temp;
}

void SG_DecrementRef(SG_ID id)
{
    if (id == 0) return; // NULL component
    *ARENA_PUSH_TYPE(_gc_queue_write, SG_ID) = id;
}

void SG_AddRef(SG_Component* comp)
{
    if (comp == NULL) return;
    ASSERT(comp->ckobj);
    _ck_api->object->add_ref(comp->ckobj);
}

void SG_GC()
{
    // swap queues
    _SG_SwapGCQueues();

    // release all objects in read queue
    size_t count = ARENA_LENGTH(_gc_queue_read, SG_ID);
    for (size_t i = 0; i < count; i++) {
        SG_ID comp_id      = *ARENA_GET_TYPE(_gc_queue_read, SG_ID, i);
        SG_Component* comp = SG_GetComponent(comp_id);
        if (!comp) continue; // already deleted
        _ck_api->object->release(comp->ckobj);
    }

    // clear read queue
    Arena::clear(_gc_queue_read);
}

// frees resources within the locator hashmap and SG_Component arenas
// sectioned off as a separate function to prevent any memory errors
// `component_size` is size in bytes
static void _SG_ComponentManagerFree(SG_ID id, int component_size)
{
    // remove from shader arena (via swap-delete with last element)
    SG_Location* result = (SG_Location*)hashmap_get(locator, &id);
    ASSERT(result);
    ASSERT(result->offset % component_size
           == 0); // offset should be a multiple of struct size
    ASSERT(result->arena->curr % component_size
           == 0); // arena size should be multiple of component size
    ASSERT(result->arena->curr >= component_size); // nonempty
    ASSERT(result->id == id);

    // int component_idx = result->offset / component_size;

    memcpy(Arena::get(result->arena, result->offset),
           Arena::get(result->arena, result->arena->curr - component_size),
           component_size);
    Arena::pop(result->arena, component_size);

    // IMPORTANT: change offset of the newly swapped arena item
    SG_ID swapped_component_id
      = ((SG_Component*)Arena::get(result->arena, result->offset))->id;
    SG_Location* swapped_component_location
      = (SG_Location*)hashmap_get(locator, &swapped_component_id);
    ASSERT(swapped_component_location);
    ASSERT(swapped_component_location->id == swapped_component_id);
    ASSERT(swapped_component_location->arena == result->arena);
    swapped_component_location->offset = result->offset;

    // delete old item from locator
    const void* delete_result = hashmap_delete(locator, &id);
    ASSERT(delete_result);
    UNUSED_VAR(delete_result);
}

void SG_ComponentFree(SG_Component* comp)
{
    // chugl v0.2.3 10/30/24 azaday: only implementing gc for shader class
    switch (comp->type) {
        case SG_COMPONENT_SHADER: {
            // push free command
            CQ_PushCommand_ComponentFree(comp);

            log_trace("freeing shader %d", comp->id);
            // free shader strings
            SG_Shader::free((SG_Shader*)comp);
            _SG_ComponentManagerFree(comp->id, sizeof(SG_Shader));
        } break;
        default: break; // TODO impl other types
    }
}

// ============================================================================
// SG_Material
// ============================================================================

void SG_Material::removeUniform(SG_Material* mat, int location)
{
    mat->uniforms[location].type = SG_MATERIAL_UNIFORM_NONE;
    // zero out
    memset(&mat->uniforms[location].as, 0, sizeof(mat->uniforms[location].as));
}

void SG_Material::setUniform(SG_Material* mat, int location, void* uniform,
                             SG_MaterialUniformType type)
{
    mat->uniforms[location].type = type;
    switch (type) {
        case SG_MATERIAL_UNIFORM_FLOAT:
            mat->uniforms[location].as.f = *(f32*)uniform;
            break;
        case SG_MATERIAL_UNIFORM_VEC2F:
            mat->uniforms[location].as.vec2f = *(glm::vec2*)uniform;
            break;
        case SG_MATERIAL_UNIFORM_VEC3F:
            mat->uniforms[location].as.vec3f = *(glm::vec3*)uniform;
            break;
        case SG_MATERIAL_UNIFORM_VEC4F:
            mat->uniforms[location].as.vec4f = *(glm::vec4*)uniform;
            break;
        case SG_MATERIAL_UNIFORM_INT:
            mat->uniforms[location].as.i = *(i32*)uniform;
            break;
        case SG_MATERIAL_UNIFORM_IVEC2:
            mat->uniforms[location].as.ivec2 = *(glm::ivec2*)uniform;
            break;
        case SG_MATERIAL_UNIFORM_IVEC3:
            mat->uniforms[location].as.ivec3 = *(glm::ivec3*)uniform;
            break;
        case SG_MATERIAL_UNIFORM_IVEC4:
            mat->uniforms[location].as.ivec4 = *(glm::ivec4*)uniform;
            break;
        default: ASSERT(false);
    }
}

void SG_Material::setTexture(SG_Material* mat, int location, SG_Texture* tex)
{
    if (mat->uniforms[location].type == SG_MATERIAL_UNIFORM_TEXTURE
        && mat->uniforms[location].as.texture_id == tex->id) {
        return; // no change
    }

    mat->uniforms[location].type = SG_MATERIAL_UNIFORM_TEXTURE;

    // refcount incoming texture
    SG_AddRef(tex);

    // decrement refcount of previous texture
    SG_DecrementRef(mat->uniforms[location].as.texture_id);

    mat->uniforms[location].as.texture_id = tex->id;
}

void SG_Material::setStorageTexture(SG_Material* mat, int location, SG_Texture* tex)
{
    mat->uniforms[location].type = SG_MATERIAL_STORAGE_TEXTURE;

    // refcount incoming texture
    SG_AddRef(tex);

    // decrement refcount of previous texture
    SG_DecrementRef(mat->uniforms[location].as.texture_id);

    mat->uniforms[location].as.texture_id = tex->id;
}

void SG_Material::shader(SG_Material* mat, SG_Shader* shader)
{
    // refcount incoming shader
    SG_AddRef(shader);

    // deref old shader
    if (mat->pso.sg_shader_id) SG_DecrementRef(mat->pso.sg_shader_id);

    // set new shader
    mat->pso.sg_shader_id = shader ? shader->id : 0;
}

bool SG_Pass::isConnected(SG_Pass* pass_a, SG_Pass* pass_b)
{
    while (pass_a && pass_b) {
        if (pass_a == pass_b) return true;
        pass_a = SG_GetPass(pass_a->next_pass_id);
    }

    return false;
}

bool SG_Pass::connect(SG_Pass* this_pass, SG_Pass* next_pass)
{
    if (!this_pass) return false;
    if (isConnected(next_pass, this_pass)) return false;

    SG_AddRef(next_pass);
    SG_DecrementRef(this_pass->next_pass_id);
    this_pass->next_pass_id = next_pass ? next_pass->id : 0;

    return true;
}

void SG_Pass::disconnect(SG_Pass* this_pass, SG_Pass* next_pass)
{
    if (next_pass == NULL || next_pass->id != this_pass->next_pass_id) return;

    this_pass->next_pass_id = 0;

    SG_DecrementRef(next_pass->next_pass_id);
}

void SG_Pass::scene(SG_Pass* pass, SG_Scene* scene)
{
    SG_AddRef(scene);
    SG_DecrementRef(pass->scene_id);
    pass->scene_id = scene ? scene->id : 0;
}

void SG_Pass::camera(SG_Pass* pass, SG_Camera* cam)
{
    SG_AddRef(cam);
    SG_DecrementRef(pass->camera_id);
    pass->camera_id = cam ? cam->id : 0;
}

void SG_Pass::screenMaterial(SG_Pass* pass, SG_Material* mat)
{
    ASSERT(pass->pass_type == SG_PassType_Screen);
    SG_AddRef(mat);
    SG_DecrementRef(pass->screen_material_id);
    pass->screen_material_id = mat->id;
}

void SG_Pass::colorTarget(SG_Pass* pass, SG_Texture* tex)
{
    SG_AddRef(tex);
    SG_DecrementRef(pass->color_target_id);
    pass->color_target_id = tex ? tex->id : 0;
}

void SG_Pass::bloomOutputRenderTexture(SG_Pass* pass, SG_Texture* tex)
{
    SG_AddRef(tex);
    SG_DecrementRef(pass->bloom_output_render_texture_id);
    pass->bloom_output_render_texture_id = tex ? tex->id : 0;
}

void SG_Pass::bloomInputRenderTexture(SG_Pass* pass, SG_Texture* tex)
{
    SG_AddRef(tex);
    SG_DecrementRef(pass->bloom_input_render_texture_id);
    pass->bloom_input_render_texture_id = tex ? tex->id : 0;
}

void SG_Scene::setMainCamera(SG_Scene* scene, SG_Camera* camera)
{
    // refcount new camera
    SG_AddRef(camera);

    // deref old camera
    SG_DecrementRef(scene->desc.main_camera_id);

    // update scene
    scene->desc.main_camera_id = camera ? camera->id : 0;
}

void SG_Scene::addLight(SG_Scene* scene, SG_ID light_id)
{
    ASSERT(!ARENA_CONTAINS(&scene->light_ids, light_id));
    *ARENA_PUSH_TYPE(&scene->light_ids, SG_ID) = light_id;
}

void SG_Scene::removeLight(SG_Scene* scene, SG_ID light_id)
{
    size_t numLights = ARENA_LENGTH(&scene->light_ids, SG_ID);
    SG_ID* lights    = (SG_ID*)scene->light_ids.base;

    for (size_t i = 0; i < numLights; ++i) {
        if (lights[i] == light_id) {
            ARENA_SWAP_DELETE(&scene->light_ids, SG_ID, i);
            return;
        }
    }
    ASSERT(false); // light not found
}

SG_Light* SG_Scene::getLight(SG_Scene* scene, u32 idx)
{
    size_t numLights = ARENA_LENGTH(&scene->light_ids, SG_ID);
    if (idx >= numLights) return 0;
    return SG_GetLight(*ARENA_GET_TYPE(&scene->light_ids, SG_ID, idx));
}

void SG_Scene::setEnvMap(SG_Scene* scene, SG_Texture* env_map)
{
    ASSERT(env_map && env_map->desc.depth == 6);

    // refcount new env map
    SG_AddRef(env_map);

    // deref old env map
    SG_DecrementRef(scene->desc.env_map_id);

    // update scene
    scene->desc.env_map_id = env_map ? env_map->id : 0;
}

void SG_Scene::setEnvMapSampler(SG_Scene* scene, SG_Sampler sampler)
{
    scene->desc.env_map_sampler = sampler;
}

void SG_Scene::setSkyboxMaterial(SG_Scene* scene, SG_Material* skybox_material)
{
    // refcount new skybox material
    SG_AddRef(skybox_material);

    // deref old skybox material
    SG_DecrementRef(scene->desc.skybox_material_id);

    // update scene
    scene->desc.skybox_material_id = skybox_material ? skybox_material->id : 0;
}

void SG_Texture::updateTextureData(SG_Texture* texture, void* data, int data_size_bytes)
{
    // byte size must be multiple of texel size
    ASSERT(data_size_bytes % SG_Texture_byteSizePerTexel(texture->desc.format) == 0);

    int num_components = SG_Texture_numComponentsPerTexel(texture->desc.format);
    int num_texels
      = data_size_bytes / SG_Texture_byteSizePerTexel(texture->desc.format);

    Chuck_ArrayFloat* ck_arr = (Chuck_ArrayFloat*)chugin_createCkObj("float[]", true);

    switch (texture->desc.format) {
        case WGPUTextureFormat_RGBA8Unorm: {
            u8* arr = (u8*)data;
            for (int i = 0; i < num_texels * num_components; i++) {
                g_chuglAPI->object->array_float_push_back(ck_arr, arr[i] / 255.0f);
            }
        } break;
        case WGPUTextureFormat_RGBA32Float:
        case WGPUTextureFormat_R32Float: {
            f32* arr = (f32*)data;
            for (int i = 0; i < num_texels * num_components; i++) {
                g_chuglAPI->object->array_float_push_back(ck_arr, arr[i]);
            }
        } break;
        default: {
            // texture format not supported
            ASSERT(false);
        }
    }

    // release old texture data
    g_chuglAPI->object->release((Chuck_Object*)texture->texture_data);

    // assign new texture data
    texture->texture_data = ck_arr;

    // broadcast TextureReadEvent
    Event_Broadcast(texture->texture_read_event);
}
