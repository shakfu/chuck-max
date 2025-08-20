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
#include <box2d/box2d.h>
#include <chuck/chugin.h>

#include "ulib_helper.h"

/*
Experiment to integrate Box2D without OOP.
This is true to the original Box2D API, and has better performance.
Box2D worlds/bodies/shapes are identified by integer ids stored as t_CKINT.
These integers are used directly, rather than wrapping in a class, like
b2World. So we avoid a cache miss from dereferencing the chuck object pointer.
This also simplifies the implementation; don't need to worry about
constructors/destructors/refcounting.

Downsides:
- slightly more verbose. world.createBody(body_def) --> B2.createBody(world_id,
body_def)
- no type safety. Everything is an int, rather than b2World, b2Body etc.
    - this will be resolved if chuck adds typedef
*/

// box2d is not threadsafe, only registered graphics shreds can call the b2 API
// OR box2d can be safely called before the first GG.nextFrame()
#define ulib_box2d_accessAllowed                                                       \
    {                                                                                  \
        static bool printed      = false;                                              \
        bool is_shred_registered = Sync_IsShredRegistered(SHRED);                      \
        bool access_denied       = g_chugl_window_initialized && !is_shred_registered; \
        if (access_denied && !printed) {                                               \
            printed = true;                                                            \
            log_warn("%s(...) called without initialization", __FUNCTION__);           \
            log_warn(" |- (hint: is the shred missing a GG.nextFrame() => now?)");     \
        }                                                                              \
        if (access_denied) return;                                                     \
    }

// make sure we can fit b2 ids within a t_CKINT
static_assert(sizeof(void*) == sizeof(t_CKUINT), "pointer size mismatch");
static_assert(sizeof(b2WorldId) <= sizeof(t_CKINT), "b2Worldsize mismatch");
static_assert(sizeof(b2BodyId) <= sizeof(t_CKINT), "b2Body size mismatch");
static_assert(sizeof(b2ShapeId) <= sizeof(t_CKINT), "b2Shape size mismatch");

// accessors
// custom accessor because size of the b2Id may be less than the corresponding ckobj
// member field (e.g. 32 byte b2WorldId is stored in the slot of a 64byte t_CKINT)
// TODO today: refactor GET_B2_ID out and replace
#define GET_NEXT_B2_ID(type, val)                                                      \
    type val = (*(type*)ARGS);                                                         \
    static_assert(sizeof(type) <= sizeof(t_CKINT), "b2 id size mismatch");             \
    GET_NEXT_INT(ARGS); // advance the pointer by amount allocate

#define GET_B2_ID(type, ptr) (*(type*)ptr)
#define RETURN_B2_ID(type, id) *((type*)&(RETURN->v_int)) = (id)

#define OBJ_MEMBER_B2_ID(type, ckobj, offset) (*(type*)OBJ_MEMBER_DATA(ckobj, offset))

#define B2_ID_TO_CKINT(id) (*(t_CKINT*)&(id))

b2BodyType ckint_to_b2BodyType(t_CKINT type)
{
    switch (type) {
        case 0: return b2_staticBody;
        case 1: return b2_kinematicBody;
        case 2: return b2_dynamicBody;
        default: return b2_staticBody;
    }
}

b2AABB vec4_to_b2AABB(t_CKVEC4 vec)
{
    b2AABB aabb{};
    aabb.lowerBound = { (f32)vec.x, (f32)vec.y };
    aabb.upperBound = { (f32)vec.z, (f32)vec.w };
    return aabb;
}

b2Vec2 vec2_to_b2Vec2(t_CKVEC2 vec)
{
    return { (f32)vec.x, (f32)vec.y };
}

t_CKVEC4 b2AABB_to_vec4(b2AABB aabb)
{
    return { aabb.lowerBound.x, aabb.lowerBound.y, aabb.upperBound.x,
             aabb.upperBound.y };
}

// ckobj data offsets --------------------------------------------
// b2WorldDef
static t_CKUINT b2WorldDef_gravity_offset                = 0;
static t_CKUINT b2WorldDef_restitutionThreshold_offset   = 0;
static t_CKUINT b2WorldDef_contactPushoutVelocity_offset = 0;
static t_CKUINT b2WorldDef_hitEventThreshold_offset      = 0;
static t_CKUINT b2WorldDef_contactHertz_offset           = 0;
static t_CKUINT b2WorldDef_contactDampingRatio_offset    = 0;
static t_CKUINT b2WorldDef_jointHertz_offset             = 0;
static t_CKUINT b2WorldDef_jointDampingRatio_offset      = 0;
static t_CKUINT b2WorldDef_maximumLinearVelocity_offset  = 0;
static t_CKUINT b2WorldDef_enableSleep_offset            = 0;
static t_CKUINT b2WorldDef_enableContinous_offset        = 0;
static t_CKUINT b2WorldDef_workerCount_offset            = 0;
CK_DLL_CTOR(b2WorldDef_ctor);

static void ckobj_to_b2WorldDef(CK_DL_API API, b2WorldDef* obj, Chuck_Object* ckobj);
// static void b2WorldDef_to_ckobj(CK_DL_API API, Chuck_Object* ckobj,
//                                 b2WorldDef* obj);

// b2BodyMoveEvent
static t_CKUINT b2BodyMoveEvent_pos_offset        = 0;
static t_CKUINT b2BodyMoveEvent_rot_offset        = 0;
static t_CKUINT b2BodyMoveEvent_bodyId_offset     = 0;
static t_CKUINT b2BodyMoveEvent_fellAsleep_offset = 0;

static Arena b2Body_move_event_pool;
static void b2BodyMoveEvent_to_ckobj(CK_DL_API API, Chuck_Object* ckobj,
                                     b2BodyMoveEvent* obj);

// b2ContactHitEvent
static t_CKUINT b2ContactHitEvent_shapeIdA_offset      = 0;
static t_CKUINT b2ContactHitEvent_shapeIdB_offset      = 0;
static t_CKUINT b2ContactHitEvent_point_offset         = 0;
static t_CKUINT b2ContactHitEvent_normal_offset        = 0;
static t_CKUINT b2ContactHitEvent_approachSpeed_offset = 0;

static Arena b2_contact_hit_event_pool; // used so we don't have to malloc a ton of
                                        // contact hit event ckobjs every frame
static void b2ContactHitEvent_to_ckobj(CK_DL_API API, Chuck_Object* ckobj,
                                       b2ContactHitEvent* obj);
// static void ckobj_to_b2ContactHitEvent(CK_DL_API  API, b2ContactHitEvent*
// obj, Chuck_Object* ckobj);

// b2RayResult
static t_CKUINT b2RayResult_shape_offset    = 0;
static t_CKUINT b2RayResult_point_offset    = 0;
static t_CKUINT b2RayResult_normal_offset   = 0;
static t_CKUINT b2RayResult_fraction_offset = 0;

static void b2RayResult_to_ckobj(Chuck_Object* ckobj, b2RayResult* obj);
// static void ckobj_to_b2RayResult(b2RayResult* obj, Chuck_Object* ckobj);

// b2QueryFilter
static t_CKUINT b2QueryFilter_categoryBits_offset = 0;
static t_CKUINT b2QueryFilter_maskBits_offset     = 0;
CK_DLL_CTOR(b2QueryFilter_ctor);

static void b2QueryFilter_to_ckobj(CK_DL_API API, Chuck_Object* ckobj,
                                   b2QueryFilter* obj);
static b2QueryFilter ckobj_to_b2QueryFilter(Chuck_Object* ckobj);

// b2Filter
static t_CKUINT b2Filter_categoryBits_offset = 0;
static t_CKUINT b2Filter_maskBits_offset     = 0;
static t_CKUINT b2Filter_groupIndex_offset   = 0;
CK_DLL_CTOR(b2Filter_ctor);

static void b2Filter_to_ckobj(CK_DL_API API, Chuck_Object* ckobj, b2Filter* obj);
static void ckobj_to_b2Filter(CK_DL_API API, b2Filter* obj, Chuck_Object* ckobj);

// b2MassData
static t_CKUINT b2MassData_mass_offset              = 0;
static t_CKUINT b2MassData_center_offset            = 0;
static t_CKUINT b2MassData_rotationalInertia_offset = 0;

static void b2MassData_to_ckobj(Chuck_Object* ckobj, b2MassData* obj);
static void ckobj_to_b2MassData(b2MassData* obj, Chuck_Object* ckobj);
static Chuck_Object* b2MassData_create(b2MassData obj, Chuck_VM_Shred* shred);

// b2RayCastInput
static t_CKUINT b2RayCastInput_origin_offset      = 0;
static t_CKUINT b2RayCastInput_translation_offset = 0;
static t_CKUINT b2RayCastInput_maxFraction_offset = 0;

// static void b2RayCastInput_to_ckobj(Chuck_Object* ckobj, b2RayCastInput* obj);
static void ckobj_to_b2RayCastInput(b2RayCastInput* obj, Chuck_Object* ckobj);

// b2ShapeCastInput
static t_CKUINT b2ShapeCastInput_points_offset      = 0;
static t_CKUINT b2ShapeCastInput_radius_offset      = 0;
static t_CKUINT b2ShapeCastInput_translation_offset = 0;
static t_CKUINT b2ShapeCastInput_maxFraction_offset = 0;
CK_DLL_CTOR(b2ShapeCastInput_ctor);

// static void b2ShapeCastInput_to_ckobj(Chuck_Object* ckobj, b2ShapeCastInput* obj);
static void ckobj_to_b2ShapeCastInput(b2ShapeCastInput* obj, Chuck_Object* ckobj);

// b2SegmentDistanceResult
static t_CKUINT b2SegmentDistanceResult_closest1_offset        = 0;
static t_CKUINT b2SegmentDistanceResult_closest2_offset        = 0;
static t_CKUINT b2SegmentDistanceResult_fraction1_offset       = 0;
static t_CKUINT b2SegmentDistanceResult_fraction2_offset       = 0;
static t_CKUINT b2SegmentDistanceResult_distanceSquared_offset = 0;
CK_DLL_CTOR(b2SegmentDistanceResult_ctor);

static void b2SegmentDistanceResult_to_ckobj(Chuck_Object* ckobj,
                                             b2SegmentDistanceResult* obj);
// static void ckobj_to_b2SegmentDistanceResult(b2SegmentDistanceResult* obj,

// b2ShapeDef
static t_CKUINT b2ShapeDef_friction_offset             = 0;
static t_CKUINT b2ShapeDef_restitution_offset          = 0;
static t_CKUINT b2ShapeDef_density_offset              = 0;
static t_CKUINT b2ShapeDef_filter_offset               = 0;
static t_CKUINT b2ShapeDef_isSensor_offset             = 0;
static t_CKUINT b2ShapeDef_enableSensorEvents_offset   = 0;
static t_CKUINT b2ShapeDef_enableContactEvents_offset  = 0;
static t_CKUINT b2ShapeDef_enableHitEvents_offset      = 0;
static t_CKUINT b2ShapeDef_enablePreSolveEvents_offset = 0;
static t_CKUINT b2ShapeDef_forceContactCreation_offset = 0;
CK_DLL_CTOR(b2ShapeDef_ctor);

static void b2ShapeDef_to_ckobj(CK_DL_API API, Chuck_Object* ckobj, b2ShapeDef* obj);
static void ckobj_to_b2ShapeDef(CK_DL_API API, b2ShapeDef* obj, Chuck_Object* ckobj);

// b2Polygon

struct b2PolygonPoolEntry {
    b2Polygon polygon;
    u32 next_empty; // index of next available slot in pool.
};

struct b2PolygonPool {
    // contiguous array of b2PolygonPoolEntry
    // first item at index 0 is reserved for bookkeeping
    Arena polygon_arena;
    u32 next_empty = 1; // index of next available entry

    static void init(b2PolygonPool* pool)
    {
        Arena::init(&pool->polygon_arena, sizeof(b2PolygonPoolEntry) * 32);
        // init first sentinal entry
        pool->next_empty = 1;
        ARENA_PUSH_ZERO_TYPE(&pool->polygon_arena, b2PolygonPoolEntry);
    }

    static b2Polygon* get(b2PolygonPool* pool, u32 index)
    {
        ASSERT(index != 0);
        ASSERT(index < b2PolygonPool::len(pool));
        return &(
          ARENA_GET_TYPE(&pool->polygon_arena, b2PolygonPoolEntry, index)->polygon);
    }

    static u32 len(b2PolygonPool* pool)
    {
        return ARENA_LENGTH(&pool->polygon_arena, b2PolygonPoolEntry);
    }

    static u32 add(b2PolygonPool* pool, b2Polygon* added_entry)
    {
        ASSERT(pool->next_empty != 0);
        b2PolygonPoolEntry* entry;
        // check if we need to realloc
        if (pool->next_empty >= b2PolygonPool::len(pool)) {
            ASSERT(pool->next_empty == b2PolygonPool::len(pool));
            entry = ARENA_PUSH_ZERO_TYPE(&pool->polygon_arena, b2PolygonPoolEntry);
        } else {
            entry = ARENA_GET_TYPE(&pool->polygon_arena, b2PolygonPoolEntry,
                                   pool->next_empty);
        }

        // copy entry
        entry->polygon = *added_entry;

        // update next empty
        u32 index = pool->next_empty;
        pool->next_empty
          = (entry->next_empty == 0) ? pool->next_empty + 1 : entry->next_empty;

        return index;
    }

    static void del(b2PolygonPool* pool, u32 index)
    {
        ASSERT(index != 0);
        ASSERT(index < b2PolygonPool::len(pool));
        ASSERT(pool->next_empty != 0);

        b2PolygonPoolEntry* deleted_entry
          = ARENA_GET_TYPE(&pool->polygon_arena, b2PolygonPoolEntry, index);
        deleted_entry->next_empty = pool->next_empty;
        pool->next_empty          = index;
    }
};

static b2PolygonPool b2polygon_pool = {};

static t_CKUINT b2Polygon_data_offset = 0;
static Chuck_Object* b2Polygon_create(Chuck_VM_Shred* shred, b2Polygon* polygon)
{
    CK_DL_API API             = g_chuglAPI;
    u32 index                 = b2PolygonPool::add(&b2polygon_pool, polygon);
    Chuck_Object* polygon_obj = chugin_createCkObj("b2Polygon", false, shred);
    OBJ_MEMBER_UINT(polygon_obj, b2Polygon_data_offset) = (t_CKUINT)index;
    return polygon_obj;
}

static b2Polygon* ckobj_to_b2Polygon(Chuck_Object* ckobj)
{
    CK_DL_API API = g_chuglAPI;
    return b2PolygonPool::get(&b2polygon_pool,
                              OBJ_MEMBER_UINT(ckobj, b2Polygon_data_offset));
}

// b2BodyDef
static t_CKUINT b2BodyDef_type_offset              = 0;
static t_CKUINT b2BodyDef_position_offset          = 0;
static t_CKUINT b2BodyDef_rotation_offset          = 0;
static t_CKUINT b2BodyDef_linearVelocity_offset    = 0;
static t_CKUINT b2BodyDef_angularVelocity_offset   = 0;
static t_CKUINT b2BodyDef_linearDamping_offset     = 0;
static t_CKUINT b2BodyDef_angularDamping_offset    = 0;
static t_CKUINT b2BodyDef_gravityScale_offset      = 0;
static t_CKUINT b2BodyDef_sleepThreshold_offset    = 0;
static t_CKUINT b2BodyDef_enableSleep_offset       = 0;
static t_CKUINT b2BodyDef_isAwake_offset           = 0;
static t_CKUINT b2BodyDef_fixedRotation_offset     = 0;
static t_CKUINT b2BodyDef_isBullet_offset          = 0;
static t_CKUINT b2BodyDef_isEnabled_offset         = 0;
static t_CKUINT b2BodyDef_automaticMass_offset     = 0;
static t_CKUINT b2BodyDef_allowFastRotation_offset = 0;
CK_DLL_CTOR(b2BodyDef_ctor);

static void b2BodyDef_to_ckobj(CK_DL_API API, Chuck_Object* ckobj, b2BodyDef* obj);
static void ckobj_to_b2BodyDef(CK_DL_API API, b2BodyDef* obj, Chuck_Object* ckobj);

// b2CastOutput
static t_CKUINT b2CastOutput_normal_offset     = 0;
static t_CKUINT b2CastOutput_point_offset      = 0;
static t_CKUINT b2CastOutput_fraction_offset   = 0;
static t_CKUINT b2CastOutput_iterations_offset = 0;
static t_CKUINT b2CastOutput_hit_offset        = 0;

static void b2CastOutput_to_ckobj(CK_DL_API API, Chuck_Object* ckobj,
                                  b2CastOutput* obj);
static Chuck_Object* b2CastOutput_create(b2CastOutput* obj, Chuck_VM_Shred* shred)
{
    Chuck_Object* ckobj = chugin_createCkObj("b2CastOutput", false, shred);
    b2CastOutput_to_ckobj(g_chuglAPI, ckobj, obj);
    return ckobj;
}
// static void ckobj_to_b2CastOutput(CK_DL_API API, b2CastOutput* obj,
// Chuck_Object* ckobj);

// b2DebugDraw
static void ckobj_to_b2DebugDraw(b2DebugDraw* obj, Chuck_Object* ckobj);
t_CKUINT b2_DebugDraw_drawingBounds_offset        = 0;
t_CKUINT b2_DebugDraw_useDrawingBounds_offset     = 0;
t_CKUINT b2_DebugDraw_drawShapes_offset           = 0;
t_CKUINT b2_DebugDraw_drawJoints_offset           = 0;
t_CKUINT b2_DebugDraw_drawJointExtras_offset      = 0;
t_CKUINT b2_DebugDraw_drawAABBs_offset            = 0;
t_CKUINT b2_DebugDraw_drawMass_offset             = 0;
t_CKUINT b2_DebugDraw_drawContacts_offset         = 0;
t_CKUINT b2_DebugDraw_drawGraphColors_offset      = 0;
t_CKUINT b2_DebugDraw_drawContactNormals_offset   = 0;
t_CKUINT b2_DebugDraw_drawContactImpulses_offset  = 0;
t_CKUINT b2_DebugDraw_drawFrictionImpulses_offset = 0;
CK_DLL_MFUN(b2_DebugDraw_DrawPolygon);
CK_DLL_MFUN(b2_DebugDraw_DrawSolidPolygon);
CK_DLL_MFUN(b2_DebugDraw_DrawCircle);
CK_DLL_MFUN(b2_DebugDraw_DrawSolidCircle);
CK_DLL_MFUN(b2_DebugDraw_DrawSolidCapsule);
CK_DLL_MFUN(b2_DebugDraw_DrawSegment);
CK_DLL_MFUN(b2_DebugDraw_DrawTransform);
CK_DLL_MFUN(b2_DebugDraw_DrawPoint);
CK_DLL_MFUN(b2_DebugDraw_DrawString);

t_CKINT b2_DebugDraw_DrawPolygon_callback_offset      = 0;
t_CKINT b2_DebugDraw_DrawSolidPolygon_callback_offset = 0;
t_CKINT b2_DebugDraw_DrawCircle_callback_offset       = 0;
t_CKINT b2_DebugDraw_DrawSolidCircle_callback_offset  = 0;
t_CKINT b2_DebugDraw_DrawSolidCapsule_callback_offset = 0;
t_CKINT b2_DebugDraw_DrawSegment_callback_offset      = 0;
t_CKINT b2_DebugDraw_DrawTransform_callback_offset    = 0;
t_CKINT b2_DebugDraw_DrawPoint_callback_offset        = 0;
t_CKINT b2_DebugDraw_DrawString_callback_offset       = 0;

CK_DLL_CTOR(b2_DebugDraw_ctor);
CK_DLL_DTOR(b2_DebugDraw_dtor);

// API ----------------------------------------------------------------

// b2
struct b2_SimulateDesc b2_sim_desc = {};
CK_DLL_SFUN(chugl_set_b2World);
CK_DLL_SFUN(b2_set_substep_count);
CK_DLL_SFUN(b2_set_simulation_rate);

CK_DLL_SFUN(b2_CreateWorld);
CK_DLL_SFUN(b2_DestroyWorld);

CK_DLL_SFUN(b2_CreateBody);
CK_DLL_SFUN(b2_DestroyBody);

// shape creation
CK_DLL_SFUN(b2_CreateCircleShape);
CK_DLL_SFUN(b2_CreateSegmentShape);
CK_DLL_SFUN(b2_CreateCapsuleShape);
CK_DLL_SFUN(b2_CreatePolygonShape);

// shape destruction
CK_DLL_SFUN(b2_DestroyShape);

// polygon creation
CK_DLL_SFUN(b2_MakePolygon);
CK_DLL_SFUN(b2_MakeOffsetPolygon);
CK_DLL_SFUN(b2_MakeSquare);
CK_DLL_SFUN(b2_MakeBox);
CK_DLL_SFUN(b2_MakeRoundedBox);
CK_DLL_SFUN(b2_MakeOffsetBox);

CK_DLL_SFUN(b2_TransformPolygon);
CK_DLL_SFUN(b2_ComputeCircleMass);
CK_DLL_SFUN(b2_ComputeCapsuleMass);
CK_DLL_SFUN(b2_ComputePolygonMass);
CK_DLL_SFUN(b2_ComputeCircleAABB);
CK_DLL_SFUN(b2_ComputeCapsuleAABB);
CK_DLL_SFUN(b2_ComputePolygonAABB);
CK_DLL_SFUN(b2_ComputeSegmentAABB);

CK_DLL_SFUN(b2_PointInCircle);
CK_DLL_SFUN(b2_PointInCapsule);
CK_DLL_SFUN(b2_PointInPolygon);

CK_DLL_SFUN(b2_RayCastSegment);
CK_DLL_SFUN(b2_RayCastPolygon);
CK_DLL_SFUN(b2_ShapeCastCircle);
CK_DLL_SFUN(b2_ShapeCastCapsule);
CK_DLL_SFUN(b2_ShapeCastSegment);
CK_DLL_SFUN(b2_ShapeCastPolygon);

CK_DLL_SFUN(b2_SegmentDistance);

// b2World ----------------------------------------------------------
CK_DLL_SFUN(b2_World_IsValid);
CK_DLL_SFUN(b2_World_Draw);
CK_DLL_SFUN(b2_World_GetBodyEvents);
CK_DLL_SFUN(b2_World_GetSensorEvents);
CK_DLL_SFUN(b2_World_GetContactEvents);

CK_DLL_SFUN(b2_World_OverlapAABB);
CK_DLL_SFUN(b2_World_OverlapCircle);
CK_DLL_SFUN(b2_World_OverlapCapsule);
CK_DLL_SFUN(b2_World_OverlapPolygon);

// TODO
// CK_DLL_SFUN(b2_World_CastRay);
// CK_DLL_SFUN(b2_World_CastCircle);
// CK_DLL_SFUN(b2_World_CastCapsule);
// CK_DLL_SFUN(b2_World_CastPolygon);

CK_DLL_SFUN(b2_World_CastRayClosest);
CK_DLL_SFUN(b2_World_CastCircleClosest);
CK_DLL_SFUN(b2_World_CastCapsuleClosest);
CK_DLL_SFUN(b2_World_CastPolygonClosest);

CK_DLL_SFUN(b2_World_EnableSleeping);
CK_DLL_SFUN(b2_World_EnableContinuous);
CK_DLL_SFUN(b2_World_SetRestitutionThreshold);
CK_DLL_SFUN(b2_World_SetHitEventThreshold);
CK_DLL_SFUN(b2_World_SetGravity);
CK_DLL_SFUN(b2_World_GetGravity);
CK_DLL_SFUN(b2_World_Explode);
CK_DLL_SFUN(b2_World_SetContactTuning);

// b2Polygon ----------------------------------------------------------
CK_DLL_DTOR(b2Polygon_dtor);

CK_DLL_MFUN(b2Polygon_get_vertices);
CK_DLL_MFUN(b2Polygon_get_normals);
CK_DLL_MFUN(b2Polygon_get_centroid);
CK_DLL_MFUN(b2Polygon_get_radius);

// b2Circle
static t_CKUINT b2Circle_position_offset = 0;
static t_CKUINT b2Circle_radius_offset   = 0;
CK_DLL_CTOR(b2Circle_ctor);
CK_DLL_CTOR(b2Circle_ctor_centered);

static void b2Circle_to_ckobj(CK_DL_API API, Chuck_Object* ckobj, b2Circle* obj);
static b2Circle ckobj_to_b2Circle(Chuck_Object* ckobj);

// b2Capsule
static t_CKUINT b2Capsule_center1_offset = 0;
static t_CKUINT b2Capsule_center2_offset = 0;
static t_CKUINT b2Capsule_radius_offset  = 0;
CK_DLL_CTOR(b2Capsule_ctor);

static void b2Capsule_to_ckobj(CK_DL_API API, Chuck_Object* ckobj, b2Capsule* obj);
static void ckobj_to_b2Capsule(CK_DL_API API, b2Capsule* obj, Chuck_Object* ckobj);

// b2Segment
static t_CKUINT b2Segment_point1_offset = 0;
static t_CKUINT b2Segment_point2_offset = 0;
CK_DLL_CTOR(b2Segment_ctor);

static void b2Segment_to_ckobj(CK_DL_API API, Chuck_Object* ckobj, b2Segment* obj);
static void ckobj_to_b2Segment(CK_DL_API API, b2Segment* obj, Chuck_Object* ckobj);

// b2Shape

// shape accessors
CK_DLL_SFUN(b2_Shape_IsValid);
CK_DLL_SFUN(b2_Shape_GetType);
CK_DLL_SFUN(b2_Shape_GetBody);
CK_DLL_SFUN(b2_Shape_IsSensor);
CK_DLL_SFUN(b2_Shape_SetDensity);
CK_DLL_SFUN(b2_Shape_GetDensity);
CK_DLL_SFUN(b2_Shape_SetFriction);
CK_DLL_SFUN(b2_Shape_GetFriction);
CK_DLL_SFUN(b2_Shape_SetRestitution);
CK_DLL_SFUN(b2_Shape_GetRestitution);
CK_DLL_SFUN(b2_Shape_GetFilter);
CK_DLL_SFUN(b2_Shape_SetFilter);
CK_DLL_SFUN(b2_Shape_EnableSensorEvents);
CK_DLL_SFUN(b2_Shape_AreSensorEventsEnabled);
CK_DLL_SFUN(b2_Shape_EnableContactEvents);
CK_DLL_SFUN(b2_Shape_AreContactEventsEnabled);
CK_DLL_SFUN(b2_Shape_EnablePreSolveEvents);
CK_DLL_SFUN(b2_Shape_ArePreSolveEventsEnabled);
CK_DLL_SFUN(b2_Shape_EnableHitEvents);
CK_DLL_SFUN(b2_Shape_AreHitEventsEnabled);
CK_DLL_SFUN(b2_Shape_TestPoint);
CK_DLL_SFUN(b2_Shape_RayCast);
CK_DLL_SFUN(b2_Shape_GetCircle);
CK_DLL_SFUN(b2_Shape_GetSegment);
// CK_DLL_SFUN(b2_Shape_GetSmoothSegment); // TODO impl after chains
CK_DLL_SFUN(b2_Shape_GetCapsule);
CK_DLL_SFUN(b2_Shape_GetPolygon);
CK_DLL_SFUN(b2_Shape_SetCircle);
CK_DLL_SFUN(b2_Shape_SetCapsule);
CK_DLL_SFUN(b2_Shape_SetSegment);
CK_DLL_SFUN(b2_Shape_SetPolygon);
// CK_DLL_SFUN(b2_Shape_GetParentChain); // TODO impl after chains

// TODO impl after contact data
// CK_DLL_SFUN(b2_Shape_GetContactCapacity);
// CK_DLL_SFUN(b2_Shape_GetContactData);

CK_DLL_SFUN(b2_Shape_GetAABB);
CK_DLL_SFUN(b2_Shape_GetClosestPoint);

// b2Body
CK_DLL_SFUN(b2_Body_is_valid);
CK_DLL_SFUN(b2_Body_get_position);
CK_DLL_SFUN(b2_Body_set_type);
CK_DLL_SFUN(b2_Body_get_type);
CK_DLL_SFUN(b2_Body_get_rotation);
CK_DLL_SFUN(b2_Body_get_angle);
CK_DLL_SFUN(b2_Body_set_transform);
CK_DLL_SFUN(b2_Body_set_position);
CK_DLL_SFUN(b2_Body_set_angle);
CK_DLL_SFUN(b2_Body_get_local_point);
CK_DLL_SFUN(b2_Body_get_world_point);
CK_DLL_SFUN(b2_Body_get_local_vector);
CK_DLL_SFUN(b2_Body_get_world_vector);
CK_DLL_SFUN(b2_Body_get_linear_velocity);
CK_DLL_SFUN(b2_Body_set_linear_velocity);
CK_DLL_SFUN(b2_Body_get_angular_velocity);
CK_DLL_SFUN(b2_Body_set_angular_velocity);
CK_DLL_SFUN(b2_Body_apply_force);
CK_DLL_SFUN(b2_Body_apply_force_to_center);
CK_DLL_SFUN(b2_Body_apply_torque);
CK_DLL_SFUN(b2_Body_apply_linear_impulse);
CK_DLL_SFUN(b2_Body_apply_linear_impulse_to_center);
CK_DLL_SFUN(b2_Body_apply_angular_impulse);
CK_DLL_SFUN(b2_Body_get_mass);
CK_DLL_SFUN(b2_Body_get_inertia);
CK_DLL_SFUN(b2_Body_get_local_center_of_mass);
CK_DLL_SFUN(b2_Body_get_world_center_of_mass);
CK_DLL_SFUN(b2_Body_get_mass_data);
CK_DLL_SFUN(b2_Body_set_mass_data);
CK_DLL_SFUN(b2_Body_apply_mass_from_shapes);
CK_DLL_SFUN(b2_Body_set_automatic_mass);
CK_DLL_SFUN(b2_Body_get_automatic_mass);
CK_DLL_SFUN(b2_Body_set_linear_damping);
CK_DLL_SFUN(b2_Body_get_linear_damping);
CK_DLL_SFUN(b2_Body_set_angular_damping);
CK_DLL_SFUN(b2_Body_get_angular_damping);
CK_DLL_SFUN(b2_Body_set_gravity_scale);
CK_DLL_SFUN(b2_Body_get_gravity_scale);
CK_DLL_SFUN(b2_Body_is_awake);
CK_DLL_SFUN(b2_Body_set_awake);
CK_DLL_SFUN(b2_Body_enable_sleep);
CK_DLL_SFUN(b2_Body_is_sleep_enabled);
CK_DLL_SFUN(b2_Body_set_sleep_threshold);
CK_DLL_SFUN(b2_Body_get_sleep_threshold);
CK_DLL_SFUN(b2_Body_is_enabled);
CK_DLL_SFUN(b2_Body_disable);
CK_DLL_SFUN(b2_Body_enable);
CK_DLL_SFUN(b2_Body_set_fixed_rotation);
CK_DLL_SFUN(b2_Body_is_fixed_rotation);
CK_DLL_SFUN(b2_Body_set_bullet);
CK_DLL_SFUN(b2_Body_is_bullet);
CK_DLL_SFUN(b2_Body_enable_hit_events);
CK_DLL_SFUN(b2_Body_get_shape_count);
CK_DLL_SFUN(b2_Body_get_shapes);
// CK_DLL_SFUN(b2_Body_get_joint_count);
// CK_DLL_SFUN(b2_Body_get_contact_capacity);
CK_DLL_SFUN(b2_Body_compute_aabb);

void ulib_box2d_query(Chuck_DL_Query* QUERY)
{
    // init resource managers
    b2PolygonPool::init(&b2polygon_pool);

    { // b2BodyType --------------------------------------
        BEGIN_CLASS("b2BodyType", "Object");
        static t_CKINT b2_static_body    = 0;
        static t_CKINT b2_kinematic_body = 1;
        static t_CKINT b2_dynamic_body   = 2;
        SVAR("int", "staticBody", &b2_static_body);
        DOC_VAR("zero mass, zero velocity, may be manually moved");
        SVAR("int", "kinematicBody", &b2_kinematic_body);
        DOC_VAR("zero mass, velocity set by user, moved by solver");
        SVAR("int", "dynamicBody", &b2_dynamic_body);
        DOC_VAR("positive mass, velocity determined by forces, moved by solver");
        END_CLASS();
    } // b2BodyType

    { // b2ShapeType --------------------------------------
        BEGIN_CLASS("b2ShapeType", "Object");
        static t_CKINT b2Circle_shape          = b2ShapeType::b2_circleShape;
        static t_CKINT b2Capsule_shape         = b2ShapeType::b2_capsuleShape;
        static t_CKINT b2Segment_shape         = b2ShapeType::b2_segmentShape;
        static t_CKINT b2Polygon_shape         = b2ShapeType::b2_polygonShape;
        static t_CKINT b2_smooth_segment_shape = b2ShapeType::b2_smoothSegmentShape;

        SVAR("int", "circleShape", &b2Circle_shape);
        DOC_VAR("A circle with an offset");

        SVAR("int", "capsuleShape", &b2Capsule_shape);
        DOC_VAR("A capsule is an extruded circle");

        SVAR("int", "segmentShape", &b2Segment_shape);
        DOC_VAR("A line segment");

        SVAR("int", "polygonShape", &b2Polygon_shape);
        DOC_VAR("A convex polygon");

        SVAR("int", "smoothSegmentShape", &b2_smooth_segment_shape);
        DOC_VAR("A smooth segment owned by a chain shape");

        END_CLASS();
    } // b2ShapeType

    { // b2BodyDef --------------------------------------
        BEGIN_CLASS("b2BodyDef", "Object");
        DOC_CLASS("https://box2d.org/documentation/group__body.html#structb2Body_def");

        CTOR(b2BodyDef_ctor);

        b2BodyDef_type_offset = MVAR("int", "type", false);
        DOC_VAR(
          "The body type: static, kinematic, or dynamic. Pass a b2BodyType enum, e.g. "
          "b2BodyType.dynamicBody");

        b2BodyDef_position_offset = MVAR("vec2", "position", false);
        DOC_VAR(
          "The initial world position of the body. Bodies should be created with "
          "the desired position. note Creating bodies at the origin and then "
          "moving them nearly doubles the cost of body creation, especially if the "
          "body is moved after shapes have been added.");

        b2BodyDef_rotation_offset = MVAR("vec2", "rotation", false);
        DOC_VAR(
          "The initial world rotation of the body. Use b2MakeRot() if you have an "
          "angle.");

        b2BodyDef_linearVelocity_offset = MVAR("vec2", "linearVelocity", false);
        DOC_VAR(
          "The initial linear velocity of the body's origin. Typically in meters "
          "per second.");

        b2BodyDef_angularVelocity_offset = MVAR("float", "angularVelocity", false);
        DOC_VAR("The initial angular velocity of the body. Radians per second");

        b2BodyDef_linearDamping_offset = MVAR("float", "linearDamping", false);
        DOC_VAR(
          "Linear damping is use to reduce the linear velocity. The damping "
          "parameter can be larger than 1 but the damping effect becomes sensitive "
          "to the time step when the damping parameter is large. Generally linear "
          "damping is undesirable because it makes objects move slowly as if they "
          "are floating.");

        b2BodyDef_angularDamping_offset = MVAR("float", "angularDamping", false);
        DOC_VAR(
          "Angular damping is use to reduce the angular velocity. The damping "
          "parameter can be larger than 1.0f but the damping effect becomes "
          "sensitive to the time step when the damping parameter is large. Angular "
          "damping can be use slow down rotating bodies.");

        b2BodyDef_gravityScale_offset = MVAR("float", "gravityScale", false);
        DOC_VAR("Scale the gravity applied to this body. Non-dimensional.");

        b2BodyDef_sleepThreshold_offset = MVAR("float", "sleepThreshold", false);
        DOC_VAR("Sleep velocity threshold, default is 0.05 meter per second");

        b2BodyDef_enableSleep_offset = MVAR("int", "enableSleep", false);
        DOC_VAR("Set this flag to false if this body should never fall asleep.");

        b2BodyDef_isAwake_offset = MVAR("int", "isAwake", false);
        DOC_VAR("Is this body initially awake or sleeping?");

        b2BodyDef_fixedRotation_offset = MVAR("int", "fixedRotation", false);
        DOC_VAR("Should this body be prevented from rotating? Useful for characters.");

        b2BodyDef_isBullet_offset = MVAR("int", "isBullet", false);
        DOC_VAR(
          "Treat this body as high speed object that performs continuous collision "
          "detection against dynamic and kinematic bodies, but not other bullet "
          "bodies. Warning Bullets should be used sparingly. They are not a "
          "solution for general dynamic-versus-dynamic continuous collision. They "
          "may interfere with joint constraints.");

        b2BodyDef_isEnabled_offset = MVAR("int", "isEnabled", false);
        DOC_VAR("Used to disable a body. A disabled body does not move or collide.");

        b2BodyDef_automaticMass_offset = MVAR("int", "automaticMass", false);
        DOC_VAR(
          "Automatically compute mass and related properties on this body from "
          "shapes. Triggers whenever a shape is add/removed/changed. Default is "
          "true.");

        b2BodyDef_allowFastRotation_offset = MVAR("int", "allowFastRotation", false);
        DOC_VAR(
          "This allows this body to bypass rotational speed limits. Should only be "
          "used "
          "for circular objects, like wheels.");

        END_CLASS();
    } // b2BodyDef

    { // b2CastOutput --------------------------------------
        BEGIN_CLASS("b2CastOutput", "Object");
        DOC_CLASS(
          "https://box2d.org/documentation/group__geometry.html#structb2_cast_output");

        b2CastOutput_normal_offset = MVAR("vec2", "normal", false);
        DOC_VAR("The surface normal at the hit point");

        b2CastOutput_point_offset = MVAR("vec2", "point", false);
        DOC_VAR("The surface hit point");

        b2CastOutput_fraction_offset = MVAR("float", "fraction", false);
        DOC_VAR("The fraction of the input translation at collision");

        b2CastOutput_iterations_offset = MVAR("int", "iterations", false);
        DOC_VAR("The number of iterations used");

        b2CastOutput_hit_offset = MVAR("int", "hit", false);
        DOC_VAR("Did the cast hit?");

        END_CLASS();
    } // b2CastOutput

    { // b2Polygon --------------------------------------
        // TODO eventually expose internals
        BEGIN_CLASS("b2Polygon", "Object");
        DOC_CLASS(
          "Don't instantiate directly. Use helpers b2.make* instead. "
          "https://box2d.org/documentation/group__geometry.html#structb2Polygon"
          "A solid convex polygon. It is assumed that the interior of the polygon"
          "is to the left of each edge."
          "Polygons have a maximum number of vertices equal to "
          "b2_maxPolygonVertices."
          "In most cases you should not need many vertices for a convex polygon."
          "@warning DO NOT fill this out manually, instead use a helper function "
          "like b2.makePolygon or b2.makeBox.");

        b2Polygon_data_offset = MVAR("int", "@b2Polygon_data", false);

        DTOR(b2Polygon_dtor);

        MFUN(b2Polygon_get_vertices, "vec2[]", "vertices");
        DOC_FUNC("Get the vertices of the polygon");

        MFUN(b2Polygon_get_normals, "vec2[]", "normals");
        DOC_FUNC("Get the outward normal vectors of the polygon sides");

        MFUN(b2Polygon_get_centroid, "vec2", "centroid");
        DOC_FUNC("Get the centroid of the polygon");

        MFUN(b2Polygon_get_radius, "float", "radius");
        DOC_FUNC("Get the external radius for rounded polygons");

        END_CLASS();
    } // b2Polygon

    { // b2Circle --------------------------------------
        BEGIN_CLASS("b2Circle", "Object");
        DOC_CLASS(
          "https://box2d.org/documentation/"
          "group__geometry.html#structb2Circle");

        CTOR(b2Circle_ctor);
        ARG("vec2", "center");
        ARG("float", "radius");

        CTOR(b2Circle_ctor_centered);
        ARG("float", "radius");
        DOC_FUNC("assumes circle is centered at @(0,0)");

        b2Circle_position_offset = MVAR("vec2", "center", false);
        DOC_VAR("The local center (relative to the body's origin)");

        b2Circle_radius_offset = MVAR("float", "radius", false);
        DOC_VAR("The radius");

        END_CLASS();
    } // b2Circle

    { // b2Capsule --------------------------------------
        BEGIN_CLASS("b2Capsule", "Object");
        DOC_CLASS(
          "https://box2d.org/documentation/"
          "group__geometry.html#structb2Capsule");

        CTOR(b2Capsule_ctor);
        ARG("vec2", "center1");
        ARG("vec2", "center2");
        ARG("float", "radius");

        b2Capsule_center1_offset = MVAR("vec2", "center1", false);
        DOC_VAR("Local center of the first semicircle");

        b2Capsule_center2_offset = MVAR("vec2", "center2", false);
        DOC_VAR("Local center of the second semicircle");

        b2Capsule_radius_offset = MVAR("float", "radius", false);
        DOC_VAR("The radius of the semicircles");

        END_CLASS();
    } // b2Capsule

    { // b2Segment --------------------------------------
        BEGIN_CLASS("b2Segment", "Object");
        DOC_CLASS(
          "https://box2d.org/documentation/"
          "group__geometry.html#structb2Segment");

        CTOR(b2Segment_ctor);
        ARG("vec2", "point1");
        ARG("vec2", "point2");

        b2Segment_point1_offset = MVAR("vec2", "point1", false);
        DOC_VAR("The first point");

        b2Segment_point2_offset = MVAR("vec2", "point2", false);
        DOC_VAR("The second point");

        END_CLASS();
    } // b2Segment

    { // b2Filter --------------------------------------
        BEGIN_CLASS("b2Filter", "Object");
        DOC_CLASS(
          "https://box2d.org/documentation/group__shape.html#structb2Filter "
          "Defaults to .categoryBits = 0x01, .maskBits = 0xFFFFFFFF, .groupIndex = 0");

        CTOR(b2Filter_ctor);

        b2Filter_categoryBits_offset = MVAR("int", "categoryBits", false);
        DOC_VAR(
          "The collision category bits. Normally you would just set one bit. The "
          "category bits should represent your application object types. E.g. player "
          "is category 0x001, enemy is category 0x002, projectile is category 0x004");

        b2Filter_maskBits_offset = MVAR("int", "maskBits", false);
        DOC_VAR(
          "The collision mask bits. This states the categories that this shape would "
          "accept for collision.  For example, you may want your player to only "
          "collide with static objects and other players. Then `maskbits = static | "
          "player`");

        b2Filter_groupIndex_offset = MVAR("int", "groupIndex", false);
        DOC_VAR(
          "	Collision groups allow a certain group of objects to never collide "
          "(negative) or always collide (positive). A group index of zero has no "
          "effect. Non-zero group filtering always wins against the mask bits.  For "
          "example, you may want ragdolls to collide with other ragdolls but you don't "
          "want ragdoll self-collision. In this case you would give each ragdoll a "
          "unique negative group index and apply that group index to all shapes on the "
          "ragdoll.");

        END_CLASS();
    } // b2Filter

    { // b2MassData --------------------------------------
        BEGIN_CLASS("b2MassData", "Object");
        // clang-format off
DOC_CLASS("Holds the mass data computed for a shape. https://box2d.org/documentation/group__geometry.html#structb2_mass_data");
        // clang-format on

        b2MassData_mass_offset = MVAR("float", "mass", false);
        DOC_VAR("The mass of the shape, usually in kilograms.");

        b2MassData_center_offset = MVAR("vec2", "center", false);
        DOC_VAR("The position of the shape's centroid relative to the shape's origin.");

        b2MassData_rotationalInertia_offset = MVAR("float", "rotationalInertia", false);
        DOC_VAR("The rotational inertia of the shape about the local origin.");

        END_CLASS();
    } // b2MassData

    { // b2RayCastInput --------------------------------------
        BEGIN_CLASS("b2RayCastInput", "Object");
        // clang-format off
DOC_CLASS("Low level ray-cast input data. https://box2d.org/documentation/group__geometry.html#structb2_ray_cast_input");
        // clang-format on

        b2RayCastInput_origin_offset = MVAR("vec2", "origin", false);
        DOC_VAR("Start point of the ray cast");

        b2RayCastInput_translation_offset = MVAR("vec2", "translation", false);
        DOC_VAR("Translation of the ray cast");

        b2RayCastInput_maxFraction_offset = MVAR("float", "maxFraction", false);
        DOC_VAR("The maximum fraction of the translation to consider, typically 1");

        END_CLASS();
    } // b2RayCastInput

    { // b2ShapeCastInput --------------------------------------
        BEGIN_CLASS("b2ShapeCastInput", "Object");
        // clang-format off
DOC_CLASS("Low level shape cast input in generic form. This allows casting an arbitrary point cloud wrap with a radius. For example, a circle is a single point with a non-zero radius.  A capsule is two points with a non-zero radius. A box is four points with a zero radius. https://box2d.org/documentation/group__geometry.html#structb2_shape_cast_input");
        // clang-format on

        CTOR(b2ShapeCastInput_ctor);

        b2ShapeCastInput_points_offset = MVAR("vec2[]", "points", false);
        DOC_VAR("A point cloud to cast. Accepts at most 8 points");

        b2ShapeCastInput_radius_offset = MVAR("float", "radius", false);
        DOC_VAR("The radius around the point cloud");

        b2ShapeCastInput_translation_offset = MVAR("vec2", "translation", false);
        DOC_VAR("The translation of the shape cast");

        b2ShapeCastInput_maxFraction_offset = MVAR("float", "maxFraction", false);
        DOC_VAR("The maximum fraction of the translation to consider, typically 1");

        END_CLASS();
    } // b2ShapeCastInput

    // Chuck_Object* ckobj);
    { // b2SegmentDistanceResult --------------------------------------
        BEGIN_CLASS("b2SegmentDistanceResult", "Object");
        // clang-format off
DOC_CLASS("Result of computing the distance between two line segments. https://box2d.org/documentation/group__distance.html#structb2_segment_distance_result");
        // clang-format on

        b2SegmentDistanceResult_closest1_offset = MVAR("vec2", "closest1", false);
        DOC_VAR("The closest point on the first segment");

        b2SegmentDistanceResult_closest2_offset = MVAR("vec2", "closest2", false);
        DOC_VAR("The closest point on the second segment");

        b2SegmentDistanceResult_fraction1_offset = MVAR("float", "fraction1", false);
        DOC_VAR("The barycentric coordinate on the first segment");

        b2SegmentDistanceResult_fraction2_offset = MVAR("float", "fraction2", false);
        DOC_VAR("The barycentric coordinate on the second segment");

        b2SegmentDistanceResult_distanceSquared_offset
          = MVAR("float", "distanceSquared", false);
        DOC_VAR("The squared distance between the closest points");

        END_CLASS();
    } // b2SegmentDistanceResult
    // b2QueryFilter --------------------------------------
    {
        BEGIN_CLASS("b2QueryFilter", "Object");
        DOC_CLASS(
          " https://box2d.org/documentation/group__shape.html#structb2_query_filter "
          "and https://box2d.org/documentation/md_simulation.html#autotoc_md112. The "
          "query filter is used to filter collisions between queries and shapes. For "
          "example, you may want a ray-cast representing a projectile to hit players "
          "and the static environment but not debris.  ");

        CTOR(b2QueryFilter_ctor);

        b2QueryFilter_categoryBits_offset = MVAR("int", "categoryBits", false);
        DOC_VAR(
          "The collision category bits of this query. Normally you would just set one "
          "bit.");

        b2QueryFilter_maskBits_offset = MVAR("int", "maskBits", false);
        DOC_VAR(
          "The collision mask bits. This states the shape categories that this query "
          "would accept for collision.");

        END_CLASS();
    } // b2QueryFilter

    { // b2RayResult --------------------------------------
        BEGIN_CLASS("b2RayResult", "Object");
        // clang-format off
        DOC_CLASS("Result from raycast queries");
        // clang-format on

        b2RayResult_shape_offset = MVAR("int", "shape", false);
        DOC_VAR("the shape id hit by the ray");

        b2RayResult_point_offset = MVAR("vec2", "point", false);
        DOC_VAR("the point of initial intersection");

        b2RayResult_normal_offset = MVAR("vec2", "normal", false);
        DOC_VAR("the normal vector at the point of intersection");

        b2RayResult_fraction_offset = MVAR("float", "fraction", false);
        DOC_VAR("the fraction along the ray at the point of intersection");

        END_CLASS();
    } // b2RayResult

    { // b2ShapeDef --------------------------------------
        BEGIN_CLASS("b2ShapeDef", "Object");
        DOC_CLASS(
          "https://box2d.org/documentation/"
          "group__shape.html#structb2Shape_def");

        CTOR(b2ShapeDef_ctor);

        b2ShapeDef_friction_offset = MVAR("float", "friction", false);
        DOC_VAR("The Coulomb (dry) friction coefficient, usually in the range [0,1]");

        b2ShapeDef_restitution_offset = MVAR("float", "restitution", false);
        DOC_VAR("The restitution (bounce) usually in the range [0,1]");

        b2ShapeDef_density_offset = MVAR("float", "density", false);
        DOC_VAR("The density, usually in kg/m^2");

        b2ShapeDef_filter_offset = MVAR("b2Filter", "filter", false);
        DOC_VAR("Collision filtering data");

        b2ShapeDef_isSensor_offset = MVAR("int", "isSensor", false);
        DOC_VAR(
          "A sensor shape generates overlap events but never generates a collision "
          "response");

        b2ShapeDef_enableSensorEvents_offset = MVAR("int", "enableSensorEvents", false);
        DOC_VAR(
          "Enable sensor events for this shape. Only applies to kinematic and "
          "dynamic bodies. Ignored for sensors");

        b2ShapeDef_enableContactEvents_offset
          = MVAR("int", "enableContactEvents", false);
        DOC_VAR(
          "Enable contact events for this shape. Only applies to kinematic and "
          "dynamic bodies. Ignored for sensors");

        b2ShapeDef_enableHitEvents_offset = MVAR("int", "enableHitEvents", false);
        DOC_VAR(
          "Enable hit events for this shape. Only applies to kinematic and dynamic "
          "bodies. Ignored for sensors");

        b2ShapeDef_enablePreSolveEvents_offset
          = MVAR("int", "enablePreSolveEvents", false);
        DOC_VAR(
          "Enable pre-solve contact events for this shape. Only applies to dynamic "
          "bodies. These are expensive and must be carefully handled due to "
          "threading. Ignored for sensors");

        b2ShapeDef_forceContactCreation_offset
          = MVAR("int", "forceContactCreation", false);
        DOC_VAR(
          "Normally shapes on static bodies don't invoke contact creation when "
          "they are added to the world. This overrides that behavior and causes "
          "contact creation. This significantly slows down static body creation "
          "which can be important when there are many static shapes");

        END_CLASS();
    } // b2ShapeDef

    { // b2WorldDef --------------------------------------
        BEGIN_CLASS("b2WorldDef", "Object");
        DOC_CLASS("World definition used to create a simulation world.");

        CTOR(b2WorldDef_ctor);

        b2WorldDef_gravity_offset = MVAR("vec2", "gravity", false);
        DOC_VAR("Gravity vector. Box2D has no up-vector defined.");

        b2WorldDef_restitutionThreshold_offset
          = MVAR("float", "restitutionThreshold", false);
        DOC_VAR(
          "Restitution velocity threshold, usually in m/s. Collisions above this "
          "speed have restitution applied (will bounce).");

        b2WorldDef_contactPushoutVelocity_offset
          = MVAR("float", "contactPushoutVelocity", false);
        DOC_VAR(
          "This parameter controls how fast overlap is resolved and has units of "
          "meters per second");

        b2WorldDef_hitEventThreshold_offset = MVAR("float", "hitEventThreshold", false);
        DOC_VAR("Threshold velocity for hit events. Usually meters per second.");

        b2WorldDef_contactHertz_offset = MVAR("float", "contactHertz", false);
        DOC_VAR("Contact stiffness. Cycles per second.");

        b2WorldDef_contactDampingRatio_offset
          = MVAR("float", "contactDampingRatio", false);
        DOC_VAR("Contact bounciness. Non-dimensional.");

        b2WorldDef_jointHertz_offset = MVAR("float", "jointHertz", false);
        DOC_VAR("Joint stiffness. Cycles per second.");

        b2WorldDef_jointDampingRatio_offset = MVAR("float", "jointDampingRatio", false);
        DOC_VAR("Joint bounciness. Non-dimensional.");

        b2WorldDef_maximumLinearVelocity_offset
          = MVAR("float", "maximumLinearVelocity", false);
        DOC_VAR("Maximum linear velocity");

        b2WorldDef_enableSleep_offset = MVAR("int", "enableSleep", false);
        DOC_VAR("Can bodies go to sleep to improve performance");

        b2WorldDef_enableContinous_offset = MVAR("int", "enableContinuous", false);
        DOC_VAR("Enable continuous collision");

        b2WorldDef_workerCount_offset = MVAR("int", "workerCount", false);
        DOC_VAR(
          "Number of workers to use with the provided task system. Box2D performs "
          "best when using only performance cores and accessing a single L2 cache. "
          "Efficiency cores and hyper-threading provide little benefit and may "
          "even harm performance.");

        END_CLASS(); // b2WorldDef
    }

    { // b2BodyMoveEvent --------------------------------------
        Arena::init(&b2Body_move_event_pool, sizeof(Chuck_Object*) * 1024);

        BEGIN_CLASS("b2BodyMoveEvent", "Object");
        // clang-format off
    DOC_CLASS(
      "https://box2d.org/documentation/group__events.html#structb2Body_move_event"
      "Body move events triggered when a body moves. Triggered when a body "
      "moves due to simulation. Not reported for bodies moved by the user. "
      "This also has a flag to indicate that the body went to sleep so the "
      "application can also sleep that actor/entity/object associated with the "
      "body. On the other hand if the flag does not indicate the body went to "
      "sleep then the application can treat the actor/entity/object associated "
      "with the body as awake. This is an efficient way for an application to "
      "update game object transforms rather than calling functions such as "
      "b2Body_GetTransform() because this data is delivered as a contiguous "
      "array and it is only populated with bodies that have moved. @note If "
      "sleeping is disabled all dynamic and kinematic bodies will trigger move "
      "events.");
        // clang-format on

        b2BodyMoveEvent_pos_offset = MVAR("vec2", "pos", false);
        DOC_VAR("2d position");

        b2BodyMoveEvent_rot_offset = MVAR("float", "rot", false);
        DOC_VAR("rotation in radians around the z-axis");

        b2BodyMoveEvent_bodyId_offset = MVAR("int", "bodyId", false);
        DOC_VAR("id of the b2body");

        b2BodyMoveEvent_fellAsleep_offset = MVAR("int", "fellAsleep", false);

        END_CLASS();
    } // b2BodyMoveEvent

    { // b2ContactHitEvent --------------------------------------
        Arena::init(&b2_contact_hit_event_pool, sizeof(Chuck_Object*) * 1024);

        BEGIN_CLASS("b2ContactHitEvent", "Object");
        // clang-format off
    DOC_CLASS(
      "https://box2d.org/documentation/group__events.html#structb2_contact_hit_event"
      "Don't instantiate directly. Use b2World.contactEvents() instead"
    );
        // clang-format on

        b2ContactHitEvent_shapeIdA_offset = MVAR("int", "shapeIdA", false);
        DOC_VAR("Id of the first shape");

        b2ContactHitEvent_shapeIdB_offset = MVAR("int", "shapeIdB", false);
        DOC_VAR("Id of the second shape");

        b2ContactHitEvent_point_offset = MVAR("vec2", "point", false);
        DOC_VAR("Point where the shapes hit");

        b2ContactHitEvent_normal_offset = MVAR("vec2", "normal", false);
        DOC_VAR("Normal vector pointing from shape A to shape B");

        b2ContactHitEvent_approachSpeed_offset = MVAR("float", "approachSpeed", false);
        DOC_VAR(
          "The speed the shapes are approaching. Always positive. Typically in "
          "meters per second.");

        END_CLASS();
    } // b2ContactHitEvent

    { // b2 --------------------------------------
        BEGIN_CLASS("b2", "Object");
        DOC_CLASS("documentation: https://box2d.org/");

        SFUN(chugl_set_b2World, "void", "world");
        ARG("int", "world");
        DOC_FUNC("Set the active physics world for simulation");

        SFUN(b2_set_substep_count, "void", "substeps");
        ARG("int", "substeps");
        DOC_FUNC(
          "Set the number of substeps for the physics simulation. Increasing the "
          "substep count can increase accuracy. Default 4.");

        SFUN(b2_set_simulation_rate, "void", "rate");
        ARG("float", "rate");
        DOC_FUNC(
          "Set the rate modifier at which the physics simulation runs. Default 1.0. "
          "E.g. A rate of 2.0 will run the simulation at twice the speed.");

        SFUN(b2_CreateWorld, "int", "createWorld");
        ARG("b2WorldDef", "def");
        DOC_FUNC(
          "Create a world for rigid body simulation. A world contains bodies, "
          "shapes, and constraints. You make create up to 128 worlds. Each world "
          "is completely independent and may be simulated in parallel.");

        SFUN(b2_DestroyWorld, "void", "destroyWorld");
        ARG("int", "world_id");
        DOC_FUNC(
          "Destroy a world and all its contents. This will free all memory "
          "associated with the world, including bodies, shapes, and joints.");

        SFUN(b2_CreateBody, "int", "createBody");
        ARG("int", "world_id");
        ARG("b2BodyDef", "def");
        DOC_FUNC(
          "Create a rigid body given a definition. No reference to the definition "
          "is retained. So you can create the definition on the stack and pass it "
          "as a pointer.");

        SFUN(b2_DestroyBody, "void", "destroyBody");
        ARG("int", "body_id");
        DOC_FUNC(
          "Destroy a rigid body given an id. This destroys all shapes and joints "
          "attached to the body. Do not keep references to the associated shapes "
          "and joints.");

        SFUN(b2_CreateCircleShape, "int", "createCircleShape");
        ARG("int", "body_id");
        ARG("b2ShapeDef", "def");
        ARG("b2Circle", "circle");
        DOC_FUNC(
          "Create a circle shape and attach it to a body. The shape definition and "
          "geometry are fully cloned. Contacts are not created until the next time "
          "step.@return the shape id for accessing the shape");

        SFUN(b2_CreateSegmentShape, "int", "createSegmentShape");
        ARG("int", "body_id");
        ARG("b2ShapeDef", "def");
        ARG("b2Segment", "segment");
        DOC_FUNC(
          "Create a line segment shape and attach it to a body. The shape "
          "definition and geometry are fully cloned. Contacts are not created "
          "until the next time step."
          "@return the shape id for accessing the shape");

        SFUN(b2_CreateCapsuleShape, "int", "createCapsuleShape");
        ARG("int", "body_id");
        ARG("b2ShapeDef", "def");
        ARG("b2Capsule", "capsule");
        DOC_FUNC(
          "Create a capsule shape and attach it to a body. The shape definition "
          "and geometry are fully cloned. Contacts are not created until the next "
          "time step. @return the shape id for accessing the shape");

        SFUN(b2_CreatePolygonShape, "int", "createPolygonShape");
        ARG("int", "body_id");
        ARG("b2ShapeDef", "def");
        ARG("b2Polygon", "polygon");
        DOC_FUNC(
          "Create a polygon shape and attach it to a body. The shape definition "
          "and geometry are fully cloned. Contacts are not created until the next "
          "time step.  @return the shape id for accessing the shape");

        SFUN(b2_DestroyShape, "void", "destroyShape");
        ARG("int", "shape_id");
        DOC_FUNC("Destroy a shape");

        // polygon creation helpers --------------------------------------
        SFUN(b2_MakeBox, "b2Polygon", "makeBox");
        ARG("float", "width");
        ARG("float", "height");
        DOC_FUNC(
          "Make a box (rectangle) polygon, bypassing the need for a convex hull.");

        SFUN(b2_MakePolygon, "b2Polygon", "makePolygon");
        ARG("vec2[]", "vertices");
        ARG("float", "radius");
        DOC_FUNC(
          "Make a convex polygon from a list of vertices. The vertices must be "
          "ordered in CCW order. The polygon must be convex. The radius is used to "
          "round the polygon's corners. You can pass at most 8 vertices.");

        SFUN(b2_MakeOffsetPolygon, "b2Polygon", "makeOffsetPolygon");
        ARG("vec2[]", "vertices");
        ARG("float", "radius");
        ARG("vec2", "position");
        ARG("float", "angle_radians");
        DOC_FUNC(
          "Make a convex polygon from a list of vertices. The vertices must be "
          "ordered in CCW order. The polygon must be convex. The radius is used to "
          "round the polygon's corners. You can pass at most 8 vertices.");

        SFUN(b2_MakeSquare, "b2Polygon", "makeSquare");
        ARG("float", "length");
        DOC_FUNC("Make a square polygon");

        SFUN(b2_MakeRoundedBox, "b2Polygon", "makeRoundedBox");
        ARG("float", "width");
        ARG("float", "height");
        ARG("float", "radius");
        DOC_FUNC("Make a rounded box");

        SFUN(b2_MakeOffsetBox, "b2Polygon", "makeOffsetBox");
        ARG("float", "width");
        ARG("float", "height");
        ARG("vec2", "center");
        ARG("float", "angle");
        DOC_FUNC("Make an offset box");

        SFUN(b2_TransformPolygon, "b2Polygon", "transformPolygon");
        ARG("vec2", "position");
        ARG("float", "angle_radians");
        ARG("b2Polygon", "polygon");
        DOC_FUNC(
          "Transform a polygon. Does not modify the given polygon, instead creating a "
          "new one at the new transform. This is useful for transferring a shape from "
          "one body to another.");

        SFUN(b2_ComputeCircleMass, "b2MassData", "computeCircleMass");
        ARG("b2Circle", "shape");
        ARG("float", "density");
        DOC_FUNC("Compute mass properties of a circle");

        SFUN(b2_ComputeCapsuleMass, "b2MassData", "computeCapsuleMass");
        ARG("b2Capsule", "shape");
        ARG("float", "density");
        DOC_FUNC("Compute mass properties of a capsule");

        SFUN(b2_ComputePolygonMass, "b2MassData", "computePolygonMass");
        ARG("b2Polygon", "shape");
        ARG("float", "density");
        DOC_FUNC("Compute mass properties of a polygon");

        SFUN(b2_ComputeCircleAABB, "vec4", "computeCircleAABB");
        ARG("b2Circle", "shape");
        ARG("vec2", "position");
        ARG("float", "angle_radians");
        DOC_FUNC(
          "Compute the bounding box of a transformed circle. Bounds are in the form "
          "@(minX, minY, maxX, maxY)");

        SFUN(b2_ComputeCapsuleAABB, "vec4", "computeCapsuleAABB");
        ARG("b2Capsule", "shape");
        ARG("vec2", "position");
        ARG("float", "angle_radians");
        DOC_FUNC(
          "Compute the bounding box of a transformed capsule. Bounds are in the form "
          "@(minX, minY, maxX, maxY)");

        SFUN(b2_ComputePolygonAABB, "vec4", "computePolygonAABB");
        ARG("b2Polygon", "shape");
        ARG("vec2", "position");
        ARG("float", "angle_radians");
        DOC_FUNC(
          "Compute the bounding box of a transformed polygon. Bounds are in the form "
          "@(minX, minY, maxX, maxY)");

        SFUN(b2_ComputeSegmentAABB, "vec4", "computeSegmentAABB");
        ARG("b2Segment", "shape");
        ARG("vec2", "position");
        ARG("float", "angle_radians");
        DOC_FUNC(
          "Compute the bounding box of a transformed line segment. Bounds are in the "
          "form @(minX, minY, maxX, maxY)");

        SFUN(b2_PointInCircle, "int", "pointInCircle");
        ARG("vec2", "point");
        ARG("b2Circle", "shape");
        DOC_FUNC("Test a point for overlap with a circle in local space");

        SFUN(b2_PointInCapsule, "int", "pointInCapsule");
        ARG("vec2", "point");
        ARG("b2Capsule", "shape");
        DOC_FUNC("Test a point for overlap with a capsule in local space");

        SFUN(b2_PointInPolygon, "int", "pointInPolygon");
        ARG("vec2", "point");
        ARG("b2Polygon", "shape");
        DOC_FUNC("Test a point for overlap with a convex polygon in local space");

        SFUN(b2_RayCastSegment, "b2CastOutput", "rayCastSegment");
        ARG("b2RayCastInput", "input");
        ARG("b2Segment", "shape");
        ARG("int", "oneSided");
        DOC_FUNC(
          "Ray cast versus segment in shape local space. Optionally treat the segment "
          "as one-sided with hits from the left side being treated as a miss.");

        SFUN(b2_RayCastPolygon, "b2CastOutput", "rayCastPolygon");
        ARG("b2RayCastInput", "input");
        ARG("b2Polygon", "shape");
        DOC_FUNC(
          "Ray cast versus polygon in shape local space. Initial overlap is treated as "
          "a miss.");

        SFUN(b2_ShapeCastCircle, "b2CastOutput", "shapeCastCircle");
        ARG("b2ShapeCastInput", "input");
        ARG("b2Circle", "shape");
        DOC_FUNC("Shape cast versus a circle. Initial overlap is treated as a miss.");

        SFUN(b2_ShapeCastCapsule, "b2CastOutput", "shapeCastCapsule");
        ARG("b2ShapeCastInput", "input");
        ARG("b2Capsule", "shape");
        DOC_FUNC("Shape cast versus a capsule. Initial overlap is treated as a miss.");

        SFUN(b2_ShapeCastSegment, "b2CastOutput", "shapeCastSegment");
        ARG("b2ShapeCastInput", "input");
        ARG("b2Segment", "shape");
        DOC_FUNC(
          "Shape cast versus a line segment. Initial overlap is treated as a miss.");

        SFUN(b2_ShapeCastPolygon, "b2CastOutput", "shapeCastPolygon");
        ARG("b2ShapeCastInput", "input");
        ARG("b2Polygon", "shape");
        DOC_FUNC(
          "Shape cast versus a convex polygon. Initial overlap is treated as a miss.");

        SFUN(b2_SegmentDistance, "b2SegmentDistanceResult", "segmentDistance");
        ARG("vec2", "p1");
        ARG("vec2", "q1");
        ARG("vec2", "p2");
        ARG("vec2", "q2");
        DOC_FUNC(
          "Compute the distance between two line segments. The closest points are "
          "returned along with the barycentric coordinates. Clamps at end points if "
          "needed");

        END_CLASS();
    } // b2

    { // b2Body ---------------------------------------
        BEGIN_CLASS("b2Body", "Object");
        DOC_CLASS(
          " Don't create bodies directly. Use b2World.createBody instead. "
          "https://box2d.org/documentation/group__body.html");

        SFUN(b2_Body_is_valid, "int", "isValid");
        ARG("int", "b2Body_id");
        DOC_FUNC("Check if a body id is valid");

        SFUN(b2_Body_get_type, "int", "type");
        ARG("int", "b2Body_id");
        DOC_FUNC("Get the body type: static, kinematic, or dynamic");

        SFUN(b2_Body_set_type, "void", "type");
        ARG("int", "b2Body_id");
        ARG("int", "b2BodyType");
        DOC_FUNC(
          " Change the body type. This is an expensive operation. This "
          "automatically updates the mass properties regardless of the automatic "
          "mass setting.");

        SFUN(b2_Body_get_position, "vec2", "position");
        ARG("int", "b2Body_id");
        DOC_FUNC(
          "Get the world position of a body. This is the location of the body "
          "origin.");

        SFUN(b2_Body_get_rotation, "complex", "rotation");
        ARG("int", "b2Body_id");
        DOC_FUNC(
          "Get the world rotation of a body as a cosine/sine pair (complex "
          "number)");

        SFUN(b2_Body_get_angle, "float", "angle");
        ARG("int", "b2Body_id");
        DOC_FUNC(" Get the body angle in radians in the range [-pi, pi]");

        SFUN(b2_Body_set_transform, "void", "transform");
        ARG("int", "b2Body_id");
        ARG("vec2", "position");
        ARG("float", "angle_radians");
        DOC_FUNC(
          "Set the world transform of a body. This acts as a teleport and is "
          "fairly expensive. "
          "@note Generally you should create a body with the intended transform. "
          "@see b2BodyDef.position. Angle is a rotation in radians and will be "
          "converted to a b2Rot rotation");

        SFUN(b2_Body_set_position, "void", "position");
        ARG("int", "b2Body_id");
        ARG("vec2", "position");
        DOC_FUNC(
          "Set the world position of a body. This acts as a teleport and is "
          "fairly expensive. @note Generally you should create a body with the "
          "intended "
          "transform. @see b2BodyDef.position. ");

        SFUN(b2_Body_set_angle, "void", "angle");
        ARG("int", "b2Body_id");
        ARG("float", "angle_radians");
        DOC_FUNC(
          "Set the world rotation of a body. This acts as a teleport and is "
          "fairly expensive.@note Generally you should create a body with the "
          "intended "
          "transform. @see b2BodyDef.position. ");

        SFUN(b2_Body_get_local_point, "vec2", "localPoint");
        ARG("int", "b2Body_id");
        ARG("vec2", "worldPoint");
        DOC_FUNC("Get a local point on a body given a world point");

        SFUN(b2_Body_get_world_point, "vec2", "worldPoint");
        ARG("int", "b2Body_id");
        ARG("vec2", "localPoint");
        DOC_FUNC("Get a world point on a body given a local point");

        SFUN(b2_Body_get_local_vector, "vec2", "localVector");
        ARG("int", "b2Body_id");
        ARG("vec2", "worldVector");
        DOC_FUNC("Get a local vector on a body given a world vector");

        SFUN(b2_Body_get_world_vector, "vec2", "worldVector");
        ARG("int", "b2Body_id");
        ARG("vec2", "localVector");
        DOC_FUNC("Get a world vector on a body given a local vector");

        SFUN(b2_Body_get_linear_velocity, "vec2", "linearVelocity");
        ARG("int", "b2Body_id");
        DOC_FUNC(
          "Get the linear velocity of a body's center of mass. Typically in "
          "meters per second.");

        SFUN(b2_Body_set_linear_velocity, "void", "linearVelocity");
        ARG("int", "b2Body_id");
        ARG("vec2", "linearVelocity");
        DOC_FUNC("Set the linear velocity of a body. Typically in meters per second.");

        SFUN(b2_Body_get_angular_velocity, "float", "angularVelocity");
        ARG("int", "b2Body_id");
        DOC_FUNC("Get the angular velocity of a body in radians per second");

        SFUN(b2_Body_set_angular_velocity, "void", "angularVelocity");
        ARG("int", "b2Body_id");
        ARG("float", "angularVelocity");
        DOC_FUNC("Set the angular velocity of a body in radians per second");

        SFUN(b2_Body_apply_force, "void", "force");
        ARG("int", "b2Body_id");
        ARG("vec2", "force");
        ARG("vec2", "point");
        ARG("int", "wake");
        DOC_FUNC(
          "Apply a force at a world point. If the force is not applied at the "
          "center of mass, it will generate a torque and affect the angular "
          "velocity. This optionally wakes up the body."
          "The force is ignored if the body is not awake.");

        SFUN(b2_Body_apply_force_to_center, "void", "force");
        ARG("int", "b2Body_id");
        ARG("vec2", "force");
        ARG("int", "wake");
        DOC_FUNC(
          "Apply a force to the center of mass. This optionally wakes up the "
          "body. The force is ignored if the body is not awake.");

        SFUN(b2_Body_apply_torque, "void", "torque");
        ARG("int", "b2Body_id");
        ARG("float", "torque");
        ARG("int", "wake");
        DOC_FUNC(
          "Apply a torque. This affects the angular velocity without affecting "
          "the linear velocity."
          "This optionally wakes the body. The torque is ignored if the body "
          "is not awake.");

        SFUN(b2_Body_apply_linear_impulse, "void", "impulse");
        ARG("int", "b2Body_id");
        ARG("vec2", "impulse");
        ARG("vec2", "point");
        ARG("int", "wake");
        DOC_FUNC(
          "Apply an impulse at a point. This immediately modifies the velocity."
          "It also modifies the angular velocity if the point of application "
          "is not at the center of mass. This optionally wakes the body."
          "The impulse is ignored if the body is not awake."
          "@warning This should be used for one-shot impulses. If you need a "
          "steady force,"
          "use a force instead, which will work better with the sub-stepping "
          "solver.");

        SFUN(b2_Body_apply_linear_impulse_to_center, "void", "impulse");
        ARG("int", "b2Body_id");
        ARG("vec2", "impulse");
        ARG("int", "wake");
        DOC_FUNC(
          "Apply an impulse to the center of mass. This immediately modifies "
          "the velocity."
          "The impulse is ignored if the body is not awake. This optionally "
          "wakes the body."
          "@warning This should be used for one-shot impulses. If you need a "
          "steady force,"
          "use a force instead, which will work better with the sub-stepping "
          "solver.");

        SFUN(b2_Body_apply_angular_impulse, "void", "impulse");
        ARG("int", "b2Body_id");
        ARG("float", "impulse");
        ARG("int", "wake");
        DOC_FUNC(
          "Apply an angular impulse. The impulse is ignored if the body is not "
          "awake. This optionally wakes the body."
          "@warning This should be used for one-shot impulses. If you need a "
          "steady"
          "force, use a force instead, which will work better with the "
          "sub-stepping solver.");

        SFUN(b2_Body_get_mass, "float", "mass");
        ARG("int", "b2Body_id");
        DOC_FUNC("Get the mass of the body, typically in kilograms");

        SFUN(b2_Body_get_inertia, "float", "inertia");
        ARG("int", "b2Body_id");
        DOC_FUNC("Get the inertia tensor of the body, typically in kg*m^2");

        SFUN(b2_Body_get_local_center_of_mass, "vec2", "localCenterOfMass");
        ARG("int", "b2Body_id");
        DOC_FUNC("Get the center of mass position of the body in local space");

        SFUN(b2_Body_get_world_center_of_mass, "vec2", "worldCenterOfMass");
        ARG("int", "b2Body_id");
        DOC_FUNC("Get the center of mass position of the body in world space");

        SFUN(b2_Body_get_mass_data, "b2MassData", "massData");
        ARG("int", "b2Body_id");
        DOC_FUNC("Get the mass data for a body");

        SFUN(b2_Body_set_mass_data, "void", "massData");
        ARG("int", "b2Body_id");
        ARG("b2MassData", "massData");
        DOC_FUNC(
          "Override the body's mass properties. Normally this is computed "
          "automatically using the shape geometry and density."
          "This information is lost if a shape is added or removed or if the "
          "body type changes.");

        SFUN(b2_Body_apply_mass_from_shapes, "void", "applyMassFromShapes");
        ARG("int", "b2Body_id");
        DOC_FUNC(
          "This update the mass properties to the sum of the mass properties "
          "of the shapes. This normally does not need to be called unless you "
          "called .massData(b2_MassData) to override the mass and you later want "
          "to reset "
          "the mass."
          "You may also use this when automatic mass computation has been "
          "disabled."
          "You should call this regardless of body type.");

        SFUN(b2_Body_set_automatic_mass, "void", "automaticMass");
        ARG("int", "automaticMass");
        DOC_FUNC(
          "Set the automatic mass setting. Normally this is set in b2BodyDef "
          "before creation.");

        SFUN(b2_Body_get_automatic_mass, "int", "automaticMass");
        DOC_FUNC("Get the automatic mass setting");

        SFUN(b2_Body_set_linear_damping, "void", "linearDamping");
        ARG("int", "b2Body_id");
        ARG("float", "linearDamping");
        DOC_FUNC(
          "Adjust the linear damping. Normally this is set in b2BodyDef before "
          "creation.");

        SFUN(b2_Body_get_linear_damping, "float", "linearDamping");
        ARG("int", "b2Body_id");
        DOC_FUNC("Get the current linear damping.");

        SFUN(b2_Body_set_angular_damping, "void", "angularDamping");
        ARG("int", "b2Body_id");
        ARG("float", "angularDamping");
        DOC_FUNC(
          "Adjust the angular damping. Normally this is set in b2BodyDef before "
          "creation.");

        SFUN(b2_Body_get_angular_damping, "float", "angularDamping");
        ARG("int", "b2Body_id");
        DOC_FUNC("Get the current angular damping.");

        SFUN(b2_Body_set_gravity_scale, "void", "gravityScale");
        ARG("int", "b2Body_id");
        ARG("float", "gravityScale");
        DOC_FUNC(
          "Adjust the gravity scale. Normally this is set in b2BodyDef before "
          "creation."
          "@see b2BodyDef::gravityScale");

        SFUN(b2_Body_get_gravity_scale, "float", "gravityScale");
        ARG("int", "b2Body_id");
        DOC_FUNC("Get the current gravity scale");

        SFUN(b2_Body_is_awake, "int", "isAwake");
        ARG("int", "b2Body_id");
        DOC_FUNC("Returns true if this body is awake");

        SFUN(b2_Body_set_awake, "void", "awake");
        ARG("int", "b2Body_id");
        ARG("int", "awake");
        DOC_FUNC(
          "Wake a body from sleep. This wakes the entire island the body is "
          "touching."
          "@warning Putting a body to sleep will put the entire island of "
          "bodies"
          "touching this body to sleep, which can be expensive and possibly "
          "unintuitive.");

        SFUN(b2_Body_enable_sleep, "void", "enableSleep");
        ARG("int", "b2Body_id");
        ARG("int", "enableSleep");
        DOC_FUNC(
          "Enable or disable sleeping for this body. If sleeping is disabled "
          "the body will wake.");

        SFUN(b2_Body_is_sleep_enabled, "int", "isSleepEnabled");
        ARG("int", "b2Body_id");
        DOC_FUNC("Returns true if sleeping is enabled for this body");

        SFUN(b2_Body_set_sleep_threshold, "void", "sleepThreshold");
        ARG("int", "b2Body_id");
        ARG("float", "sleepThreshold");
        DOC_FUNC("Set the sleep threshold, typically in meters per second");

        SFUN(b2_Body_get_sleep_threshold, "float", "sleepThreshold");
        ARG("int", "b2Body_id");
        DOC_FUNC("Get the sleep threshold, typically in meters per second.");

        SFUN(b2_Body_is_enabled, "int", "isEnabled");
        ARG("int", "b2Body_id");
        DOC_FUNC("Returns true if this body is enabled");

        SFUN(b2_Body_disable, "void", "disable");
        ARG("int", "b2Body_id");
        DOC_FUNC(
          "Disable a body by removing it completely from the simulation. This "
          "is expensive.");

        SFUN(b2_Body_enable, "void", "enable");
        ARG("int", "b2Body_id");
        DOC_FUNC("Enable a body by adding it to the simulation. This is expensive.");

        SFUN(b2_Body_set_fixed_rotation, "void", "fixedRotation");
        ARG("int", "b2Body_id");
        ARG("int", "flag");
        DOC_FUNC(
          "Set this body to have fixed rotation. This causes the mass to be "
          "reset in all cases.");

        SFUN(b2_Body_is_fixed_rotation, "int", "isFixedRotation");
        ARG("int", "b2Body_id");
        DOC_FUNC("Does this body have fixed rotation?");

        SFUN(b2_Body_set_bullet, "void", "bullet");
        ARG("int", "b2Body_id");
        ARG("int", "flag");
        DOC_FUNC(
          "Set this body to be a bullet. A bullet does continuous collision "
          "detection against dynamic bodies (but not other bullets).");

        SFUN(b2_Body_is_bullet, "int", "isBullet");
        ARG("int", "b2Body_id");
        DOC_FUNC("Is this body a bullet?");

        SFUN(b2_Body_enable_hit_events, "void", "enableHitEvents");
        ARG("int", "b2Body_id");
        ARG("int", "enableHitEvents");
        DOC_FUNC(
          "Enable/disable hit events on all shapes"
          "@see b2ShapeDef::enableHitEvents");

        SFUN(b2_Body_get_shape_count, "int", "shapeCount");
        ARG("int", "b2Body_id");
        DOC_FUNC("Get the number of shapes on this body");

        SFUN(b2_Body_get_shapes, "int[]", "shapes");
        ARG("int", "b2Body_id");
        ARG("int[]", "shape_id_array");
        DOC_FUNC("Get the shape ids for all shapes on this body");

        { // TODO add after impl joints

            // SFUN(b2_Body_get_joint_count, "int", "jointCount");
            // ARG("int", "b2Body_id");
            // DOC_FUNC("Get the number of joints on this body");

            // SFUN(b2_Body_get_joints, "b2_Joint[]", "joints");
            // DOC_FUNC("Get the joint ids for all joints on this body");
        }

        // SFUN(b2_Body_get_contact_capacity, "int", "contactCapacity");
        // ARG("int", "b2Body_id");
        // DOC_FUNC(
        //   "Get the maximum capacity required for retrieving all the touching "
        //   "contacts on a body");

        // TODO add after impl contacts
        // SFUN(b2_Body_get_contact_data, "b2_ContactData[]", "contactData");
        // ARG("int", "b2Body_id");
        // DOC_FUNC("Get the touching contact data for a body");

        SFUN(b2_Body_compute_aabb, "vec4", "computeAABB");
        // ARG("int", "b2Body_id");
        DOC_FUNC(
          "Get the current world AABB that contains all the attached shapes. Note "
          "that this may not encompass the body origin."
          "If there are no shapes attached then the returned AABB is empty and "
          "centered on the body origin."
          "The aabb is in the form of (lowerBound.x, lowerBound.y, upperBound.x, "
          "upperBound.y");

        END_CLASS();
    } // b2Body

    { // b2Shape --------------------------------------
        BEGIN_CLASS("b2Shape", "Object");
        DOC_CLASS(
          "https://box2d.org/documentation/group__shape.html"
          "Don't instantiate directly. Use b2Shape.createXXXShape() functions "
          "instead");

        // shape accessors

        SFUN(b2_Shape_IsValid, "int", "isValid");
        ARG("int", "shape_id");
        DOC_FUNC("Check if a shape id is valid");

        SFUN(b2_Shape_GetType, "int", "type");
        ARG("int", "shape_id");
        DOC_FUNC("Get the type of a shape. Returns b2ShapeType");

        SFUN(b2_Shape_GetBody, "int", "body");
        ARG("int", "shape_id");
        DOC_FUNC("Get the body id that a shape is attached to");

        SFUN(b2_Shape_IsSensor, "int", "isSensor");
        ARG("int", "shape_id");
        DOC_FUNC("Returns true If the shape is a sensor");

        SFUN(b2_Shape_SetDensity, "void", "density");
        ARG("int", "shape_id");
        ARG("float", "density");
        DOC_FUNC(
          "Set the mass density of a shape, typically in kg/m^2. This will not "
          "update the mass properties on the parent body.");

        SFUN(b2_Shape_GetDensity, "float", "density");
        ARG("int", "shape_id");
        DOC_FUNC("Get the density of a shape, typically in kg/m^2");

        SFUN(b2_Shape_SetFriction, "void", "friction");
        ARG("int", "shape_id");
        ARG("float", "friction");
        DOC_FUNC("Set the friction on a shape");

        SFUN(b2_Shape_GetFriction, "float", "friction");
        ARG("int", "shape_id");
        DOC_FUNC("Get the friction of a shape");

        SFUN(b2_Shape_SetRestitution, "void", "restitution");
        ARG("int", "shape_id");
        ARG("float", "restitution");
        DOC_FUNC("Set the shape restitution (bounciness)");

        SFUN(b2_Shape_GetRestitution, "float", "restitution");
        ARG("int", "shape_id");
        DOC_FUNC("Get the shape restitution");

        SFUN(b2_Shape_SetFilter, "void", "filter");
        ARG("int", "shape_id");
        ARG("b2Filter", "filter");
        DOC_FUNC(
          "Set the current filter. This is almost as expensive as recreating the "
          "shape. @see b2ShapeDef::filter");

        SFUN(b2_Shape_GetFilter, "b2Filter", "filter");
        ARG("int", "shape_id");
        DOC_FUNC("Get the shape filter");

        SFUN(b2_Shape_EnableSensorEvents, "void", "enableSensorEvents");
        ARG("int", "shape_id");
        ARG("int", "flag");
        DOC_FUNC(
          "Enable sensor events for this shape. Only applies to kinematic and "
          "dynamic bodies. Ignored for sensors. @see b2ShapeDef::isSensor");

        SFUN(b2_Shape_AreSensorEventsEnabled, "int", "areSensorEventsEnabled");
        ARG("int", "shape_id");
        DOC_FUNC("Returns true if sensor events are enabled");

        SFUN(b2_Shape_EnableContactEvents, "void", "enableContactEvents");
        ARG("int", "shape_id");
        ARG("int", "flag");
        DOC_FUNC(
          "Enable contact events for this shape. Only applies to kinematic and "
          "dynamic bodies. Ignored for sensors. @see "
          "b2ShapeDef::enableContactEvents");

        SFUN(b2_Shape_AreContactEventsEnabled, "int", "areContactEventsEnabled");
        ARG("int", "shape_id");
        DOC_FUNC("Returns true if contact events are enabled");

        SFUN(b2_Shape_EnablePreSolveEvents, "void", "enablePreSolveEvents");
        ARG("int", "shape_id");
        ARG("int", "flag");
        DOC_FUNC(
          "Enable pre-solve contact events for this shape. Only applies to dynamic "
          "bodies. These are expensive and must be carefully handled due to "
          "multithreading. Ignored for sensors. @see b2PreSolveFcn");

        SFUN(b2_Shape_ArePreSolveEventsEnabled, "int", "arePreSolveEventsEnabled");
        ARG("int", "shape_id");
        DOC_FUNC("Returns true if pre-solve events are enabled");

        SFUN(b2_Shape_EnableHitEvents, "void", "enableHitEvents");
        ARG("int", "shape_id");
        ARG("int", "flag");
        DOC_FUNC(
          "Enable contact hit events for this shape. Ignored for sensors. @see "
          "b2WorldDef::hitEventThreshold");

        SFUN(b2_Shape_AreHitEventsEnabled, "int", "areHitEventsEnabled");
        ARG("int", "shape_id");
        DOC_FUNC("Returns true if hit events are enabled");

        SFUN(b2_Shape_TestPoint, "int", "testPoint");
        ARG("int", "shape_id");
        ARG("vec2", "point");
        DOC_FUNC("Test a point for overlap with a shape");

        SFUN(b2_Shape_RayCast, "void", "rayCast");
        ARG("int", "shape_id");
        ARG("vec2", "origin");
        ARG("vec2", "translation");
        ARG("b2CastOutput", "output");
        DOC_FUNC(
          "Ray cast a shape directly. Copies the output to the provided "
          "output object.");

        SFUN(b2_Shape_GetCircle, "b2Circle", "circle");
        ARG("int", "shape_id");
        DOC_FUNC("Get a copy of the shape's circle. Asserts the type is correct.");

        SFUN(b2_Shape_GetSegment, "b2Segment", "segment");
        ARG("int", "shape_id");
        DOC_FUNC(
          "Get a copy of the shape's line segment. Asserts the type is correct.");

        // TODO
        // SFUN(b2_Shape_GetSmoothSegment, "b2_SmoothSegment", "smoothSegment");
        // ARG("int", "shape_id");
        // DOC_FUNC(
        //   "Get a copy of the shape's smooth line segment. Asserts the type is "
        //   "correct.");

        SFUN(b2_Shape_GetCapsule, "b2Capsule", "capsule");
        ARG("int", "shape_id");
        DOC_FUNC("Get a copy of the shape's capsule. Asserts the type is correct.");

        SFUN(b2_Shape_GetPolygon, "b2Polygon", "polygon");
        ARG("int", "shape_id");
        DOC_FUNC(
          "Get a copy of the shape's convex polygon. Asserts the type is correct.");

        SFUN(b2_Shape_SetCircle, "void", "circle");
        ARG("int", "shape_id");
        ARG("b2Circle", "circle");
        DOC_FUNC(
          "Allows you to change a shape to be a circle or update the current "
          "circle."
          "This does not modify the mass properties."
          "@see b2Body_ApplyMassFromShapes");

        SFUN(b2_Shape_SetSegment, "void", "segment");
        ARG("int", "shape_id");
        ARG("b2Segment", "segment");
        DOC_FUNC(
          "Allows you to change a shape to be a segment or update the current "
          "segment."
          "This does not modify the mass properties.");

        SFUN(b2_Shape_SetCapsule, "void", "capsule");
        ARG("int", "shape_id");
        ARG("b2Capsule", "capsule");
        DOC_FUNC(
          "Allows you to change a shape to be a capsule or update the current "
          "capsule."
          "This does not modify the mass properties."
          "@see b2Body_ApplyMassFromShapes");

        SFUN(b2_Shape_SetPolygon, "void", "polygon");
        ARG("int", "shape_id");
        ARG("b2Polygon", "polygon");
        DOC_FUNC(
          "Allows you to change a shape to be a polygon or update the current "
          "polygon."
          "This does not modify the mass properties."
          "@see b2Body_ApplyMassFromShapes");

        // SFUN(b2_Shape_GetParentChain, "int", "parentChain");
        // ARG("int", "shape_id");
        // DOC_FUNC(
        //   "Get the parent chain id if the shape type is b2_smoothSegmentShape,"
        //   "otherwise returns b2_nullChainId = 0x00.");

        // SFUN(b2_Shape_GetContactCapacity, "int", "contactCapacity");
        // ARG("int", "shape_id");
        // DOC_FUNC(
        //   "Get the maximum capacity required for retrieving all the touching "
        //   "contacts on a shape");

        // SFUN(b2_Shape_GetContactData, "void", "contactData");
        // ARG("int", "shape_id");
        // ARG("b2_ContactData[]", "contacts");
        // DOC_FUNC(
        //   "Get the touching contact data for a shape. The provided shapeId will
        //   be " "either shapeIdA or shapeIdB on the contact data. Contact data is
        //   copied " "into the given contacts array.");

        SFUN(b2_Shape_GetAABB, "vec4", "AABB");
        ARG("int", "shape_id");
        DOC_FUNC(
          "Get the current world AABB. Returns (lowerBound.x, lowerBound.y, "
          "upperBound.x, upperBound.y)");

        SFUN(b2_Shape_GetClosestPoint, "vec2", "closestPoint");
        ARG("int", "shape_id");
        ARG("vec2", "target");
        DOC_FUNC(
          "Get the closest point on a shape to a target point. Target and result "
          "are in world space.");

        END_CLASS();
    } // b2Shape

    { // b2DebugDraw

        BEGIN_CLASS("b2DebugDraw", "Object");
        DOC_CLASS(
          "This class holds callbacks you can override to draw a Box2D world. "
          "https://box2d.org/documentation/group__world.html#structb2_debug_draw");

        CTOR(b2_DebugDraw_ctor);
        DTOR(b2_DebugDraw_dtor);

        // member vars --------------------------------------
        b2_DebugDraw_drawingBounds_offset = MVAR("vec4", "drawingBounds", false);
        DOC_VAR(
          "Bounds to use if restricting drawing to a rectangular region. The AABB is "
          "in "
          "the form of @(lowerBound.x, lowerBound.y, upperBound.x, upperBound.y)");
        b2_DebugDraw_useDrawingBounds_offset = MVAR("int", "useDrawingBounds", false);
        DOC_VAR(
          "Option to restrict drawing to a rectangular region. May suffer from "
          "unstable depth sorting.");
        b2_DebugDraw_drawShapes_offset = MVAR("int", "drawShapes", false);
        DOC_VAR("Option to draw shapes");
        b2_DebugDraw_drawJoints_offset = MVAR("int", "drawJoints", false);
        DOC_VAR("Option to draw joints");
        b2_DebugDraw_drawJointExtras_offset = MVAR("int", "drawJointExtras", false);
        DOC_VAR("Option to draw additional information for joints");
        b2_DebugDraw_drawAABBs_offset = MVAR("int", "drawAABBs", false);
        DOC_VAR("Option to draw the bounding boxes for shapes");
        b2_DebugDraw_drawMass_offset = MVAR("int", "drawMass", false);
        DOC_VAR("Option to draw the mass and center of mass of dynamic bodies");
        b2_DebugDraw_drawContacts_offset = MVAR("int", "drawContacts", false);
        DOC_VAR("Option to draw contact points");
        b2_DebugDraw_drawGraphColors_offset = MVAR("int", "drawGraphColors", false);
        DOC_VAR("Option to visualize the graph coloring used for contacts and joints");
        b2_DebugDraw_drawContactNormals_offset
          = MVAR("int", "drawContactNormals", false);
        DOC_VAR("Option to draw contact normals");
        b2_DebugDraw_drawContactImpulses_offset
          = MVAR("int", "drawContactImpulses", false);
        DOC_VAR("Option to draw contact normal impulses");
        b2_DebugDraw_drawFrictionImpulses_offset
          = MVAR("int", "drawFrictionImpulses", false);
        DOC_VAR("Option to draw contact friction impulses");

        // callbacks --------------------------------------
        MFUN(b2_DebugDraw_DrawPolygon, "void", "drawPolygon");
        ARG("vec2[]", "vertices");
        ARG("vec3", "color");
        DOC_FUNC("Draw a closed polygon provided in CCW order.");

        MFUN(b2_DebugDraw_DrawSolidPolygon, "void", "drawSolidPolygon");
        ARG("vec2", "position");
        ARG("float", "rotation_radians");
        ARG("vec2[]", "vertices");
        ARG("float", "radius");
        ARG("vec3", "color");
        DOC_FUNC("Draw a solid closed polygon provided in CCW order.");

        MFUN(b2_DebugDraw_DrawCircle, "void", "drawCircle");
        ARG("vec2", "center");
        ARG("float", "radius");
        ARG("vec3", "color");
        DOC_FUNC("Draw a circle outline.");

        MFUN(b2_DebugDraw_DrawSolidCircle, "void", "drawSolidCircle");
        ARG("vec2", "center");
        ARG("float", "rotation_radians");
        ARG("float", "radius");
        ARG("vec3", "color");
        DOC_FUNC("Draw a solid circle.");

        MFUN(b2_DebugDraw_DrawSolidCapsule, "void", "drawSolidCapsule");
        ARG("vec2", "p1");
        ARG("vec2", "p2");
        ARG("float", "radius");
        ARG("vec3", "color");
        DOC_FUNC("Draw a solid capsule.");

        MFUN(b2_DebugDraw_DrawSegment, "void", "drawSegment");
        ARG("vec2", "p1");
        ARG("vec2", "p2");
        ARG("vec3", "color");
        DOC_FUNC("Draw a line segment.");

        MFUN(b2_DebugDraw_DrawTransform, "void", "drawTransform");
        ARG("vec2", "position");
        ARG("float", "rotation_radians");
        DOC_FUNC("Draw a transform. Choose your own length scale.");

        MFUN(b2_DebugDraw_DrawPoint, "void", "drawPoint");
        ARG("vec2", "position");
        ARG("float", "size");
        ARG("vec3", "color");
        DOC_FUNC("Draw a point.");

        // MFUN(b2_DebugDraw_DrawString, "void", "drawString");
        // ARG("vec2", "position");
        // ARG("string", "text");
        // DOC_FUNC("Draw a string.");

        END_CLASS();

        // store callback offsets
        b2_DebugDraw_DrawPolygon_callback_offset
          = chugin_setVTableOffset("b2DebugDraw", "drawPolygon");
        b2_DebugDraw_DrawSolidPolygon_callback_offset
          = chugin_setVTableOffset("b2DebugDraw", "drawSolidPolygon");
        b2_DebugDraw_DrawCircle_callback_offset
          = chugin_setVTableOffset("b2DebugDraw", "drawCircle");
        b2_DebugDraw_DrawSolidCircle_callback_offset
          = chugin_setVTableOffset("b2DebugDraw", "drawSolidCircle");
        b2_DebugDraw_DrawSolidCapsule_callback_offset
          = chugin_setVTableOffset("b2DebugDraw", "drawSolidCapsule");
        b2_DebugDraw_DrawSegment_callback_offset
          = chugin_setVTableOffset("b2DebugDraw", "drawSegment");
        b2_DebugDraw_DrawTransform_callback_offset
          = chugin_setVTableOffset("b2DebugDraw", "drawTransform");
        b2_DebugDraw_DrawPoint_callback_offset
          = chugin_setVTableOffset("b2DebugDraw", "drawPoint");
        // b2_DebugDraw_DrawString_callback_offset
        //   = chugin_setVTableOffset("b2DebugDraw", "drawString");

    } // b2DebugDraw

    { // b2World --------------------------------------
        BEGIN_CLASS("b2World", "Object");

        SFUN(b2_World_IsValid, "int", "isValid");
        ARG("int", "world_id");
        DOC_FUNC("World id validation. Provides validation for up to 64K allocations.");

        // TODO debug draw
        SFUN(b2_World_Draw, "void", "draw");
        ARG("int", "world_id");
        ARG("b2DebugDraw", "draw");
        DOC_FUNC(
          "Call this to have the given b2DebugDraw be called with shapes and other "
          "debug draw data for the given world. Note: you will need to extend "
          "b2DebugDraw and implement your own drawing functions");

        SFUN(b2_World_GetBodyEvents, "void", "bodyEvents");
        ARG("int", "world_id");
        ARG("b2BodyMoveEvent[]", "body_events");
        DOC_FUNC("Get the body events for the current time step.");

        SFUN(b2_World_GetSensorEvents, "void", "sensorEvents");
        ARG("int", "world_id");
        ARG("int[]", "begin_sensor_events");
        ARG("int[]", "end_sensor_events");
        DOC_FUNC(
          "Original box2D documentation: "
          "https://box2d.org/documentation/"
          "group__events.html#structb2_sensor_events "
          "This implementation is different and optimized for cache performance. "
          "The two input arrays,"
          "begin_sensor_events and end_sensor_events, are used to store the sensor "
          "events of the last frame,"
          "which are a pair of b2ShapeIds (int), stored in order. E.g. "
          "begin_sensor_events[i] is the id of the"
          "sensor shape, and begin_sensor_events[i+1] is the id of the dynamic "
          "shape that began contact with the sensor.");

        SFUN(b2_World_GetContactEvents, "void", "contactEvents");
        ARG("int", "world_id");
        ARG("int[]", "begin_contact_events_shape_ids");
        ARG("int[]", "end_contact_events_shape_ids");
        ARG("b2ContactHitEvent[]", "contact_hit_events");
        DOC_FUNC(
          "https://box2d.org/documentation/"
          "group__world.html#ga67e9e2ecf3897d4c7254196395be65ca"
          "Unlike the original box2D implementation, this function returns the "
          "ContactBeginTouchEvents and ContactEndTouchEvents in two separate flat "
          "arrays"
          "of b2ShapeIds (int), stored in order. E.g. begin_contact_events[i] and "
          "begin_contact_events[i+1] are the ids of the two shapes that began "
          "contact."
          "The contact_hit_events array is used to store the contact hit events of "
          "the "
          "last frame.");

        SFUN(b2_World_OverlapAABB, "int[]", "overlapAABB");
        ARG("int", "world_id");
        ARG("vec4", "aabb");
        ARG("b2QueryFilter", "filter");
        DOC_FUNC(
          "Returns the b2ShapeId for all shapes that overlap the provided AABB. The "
          "AABB is in the form of @(lowerBound.x, lowerBound.y, upperBound.x, "
          "upperBound.y)");

        SFUN(b2_World_OverlapCircle, "int[]", "overlapCapsule");
        ARG("int", "world_id");
        ARG("b2Circle", "circle");
        ARG("vec2", "circle_position");
        ARG("float", "circle_rotation_radians");
        ARG("b2QueryFilter", "filter");
        DOC_FUNC(
          "Returns the b2ShapeId for all shapes that overlap the provided circle");

        SFUN(b2_World_OverlapCapsule, "int[]", "overlapCapsule");
        ARG("int", "world_id");
        ARG("b2Capsule", "capsule");
        ARG("vec2", "capsule_position");
        ARG("float", "capsule_rotation_radians");
        ARG("b2QueryFilter", "filter");
        DOC_FUNC(
          "Returns the b2ShapeId for all shapes that overlap the provided capsule");

        SFUN(b2_World_OverlapPolygon, "int[]", "overlapPolygon");
        ARG("int", "world_id");
        ARG("b2Polygon", "polygon");
        ARG("vec2", "polygon_position");
        ARG("float", "polygon_rotation_radians");
        ARG("b2QueryFilter", "filter");
        DOC_FUNC(
          "Returns the b2ShapeId for all shapes that overlap the provided polygon");

        // TODO: does chuck leak with object arrays?
        SFUN(b2_World_CastRayClosest, "b2RayResult", "castRayClosest");
        ARG("int", "world_id");
        ARG("vec2", "ray_origin");
        ARG("vec2", "ray_direction");
        ARG("b2QueryFilter", "filter");
        DOC_FUNC(
          "Cast a ray and return the closest hit. The ray is defined by a start "
          "point and a direction vector.");

        SFUN(b2_World_CastCircleClosest, "b2RayResult", "castCircleClosest");
        ARG("int", "world_id");
        ARG("b2Circle", "circle");
        ARG("vec2", "circle_origin");
        ARG("vec2", "cast_direction");
        ARG("b2QueryFilter", "filter");
        DOC_FUNC(
          "Cast a circle through the world and return the closest hit shape_id. "
          "Similar to a cast ray except that a circle is cast instead of a point.");

        SFUN(b2_World_CastCapsuleClosest, "b2RayResult", "castCapsuleClosest");
        ARG("int", "world_id");
        ARG("b2Capsule", "capsule");
        ARG("vec2", "capsule_origin");
        ARG("float", "capsule_rotation_radians");
        ARG("vec2", "cast_direction");
        ARG("b2QueryFilter", "filter");
        DOC_FUNC(
          "Cast a capsule through the world and return the closest hit shape_id. "
          "Similar to a cast ray except that a capsule is cast instead of a point.");

        SFUN(b2_World_CastPolygonClosest, "b2RayResult", "castPolygonClosest");
        ARG("int", "world_id");
        ARG("b2Polygon", "polygon");
        ARG("vec2", "polygon_origin");
        ARG("float", "polygon_rotation_radians");
        ARG("vec2", "cast_direction");
        ARG("b2QueryFilter", "filter");
        DOC_FUNC(
          "Cast a polygon through the world and return the closest hit shape_id. "
          "Similar to a cast ray except that a polygon is cast instead of a point.");

        SFUN(b2_World_EnableSleeping, "void", "enableSleeping");
        ARG("int", "world_id");
        ARG("int", "flag");
        DOC_FUNC(
          "Enable/disable sleep. If your application does not need sleeping, you can "
          "gain some performance by disabling sleep completely at the world level."
          "@see b2WorldDef");

        SFUN(b2_World_EnableContinuous, "void", "enableContinuous");
        ARG("int", "world_id");
        ARG("int", "flag");
        DOC_FUNC(
          "Enable/disable continuous collision between dynamic and static bodies."
          "Generally you should keep continuous collision enabled to prevent fast"
          "moving objects from going through static objects. The performance gain from"
          "disabling continuous collision is minor."
          "@see b2WorldDef");

        SFUN(b2_World_SetRestitutionThreshold, "void", "setRestitutionThreshold");
        ARG("int", "world_id");
        ARG("float", "value");
        DOC_FUNC(
          "Adjust the restitution threshold. It is recommended not to make this value "
          "very small because it will prevent bodies from sleeping. Typically in "
          "meters "
          "per second."
          "@see b2WorldDef");

        SFUN(b2_World_SetHitEventThreshold, "void", "setHitEventThreshold");
        ARG("int", "world_id");
        ARG("float", "value");
        DOC_FUNC(
          "Adjust the hit event threshold. This controls the collision velocity needed "
          "to generate a b2ContactHitEvent. Typically in meters per second."
          "@see b2WorldDef");

        SFUN(b2_World_SetGravity, "void", "setGravity");
        ARG("int", "world_id");
        ARG("vec2", "gravity");
        DOC_FUNC(
          "Set the gravity vector for the entire world. Box2D has no concept of an up "
          "direction and this is left as a decision for the application. Typically in "
          "m/s^2."
          "@see b2WorldDef");

        SFUN(b2_World_GetGravity, "vec2", "getGravity");
        ARG("int", "world_id");
        DOC_FUNC("Get the gravity vector");

        SFUN(b2_World_Explode, "void", "explode");
        ARG("int", "world_id");
        ARG("vec2", "position");
        ARG("float", "radius");
        ARG("float", "impulse");
        DOC_FUNC("Apply a radial explosion");

        SFUN(b2_World_SetContactTuning, "void", "setContactTuning");
        ARG("int", "world_id");
        ARG("float", "hertz");
        ARG("float", "dampingRatio");
        ARG("float", "pushVelocity");
        DOC_FUNC(
          "Adjust contact tuning parameters. Advanced feature. `hertz` is contact "
          "stiffness, `dampingRatio` is contact bounciness with 1 being critical "
          "damping (non-dimensional), `pushVelocity` is the maximum contact constraint "
          "push out velocity in m/s. Advanced feature.");

        END_CLASS();
    } // b2World
}

// ============================================================================
// b2BodyMoveEvent
// ============================================================================

static void b2BodyMoveEvent_to_ckobj(CK_DL_API API, Chuck_Object* ckobj,
                                     b2BodyMoveEvent* obj)
{
    OBJ_MEMBER_VEC2(ckobj, b2BodyMoveEvent_pos_offset)
      = { obj->transform.p.x, obj->transform.p.y };
    // convert complex rot to radians
    OBJ_MEMBER_FLOAT(ckobj, b2BodyMoveEvent_rot_offset)
      = b2Rot_GetAngle(obj->transform.q);
    OBJ_MEMBER_B2_ID(b2BodyId, ckobj, b2BodyMoveEvent_bodyId_offset) = obj->bodyId;
    OBJ_MEMBER_INT(ckobj, b2BodyMoveEvent_fellAsleep_offset)         = obj->fellAsleep;
}

// ============================================================================
// b2
// ============================================================================

CK_DLL_SFUN(chugl_set_b2World)
{
    ulib_box2d_accessAllowed;

    b2WorldId world_id   = GET_B2_ID(b2WorldId, ARGS);
    b2_sim_desc.world_id = *(u32*)&world_id;
    CQ_PushCommand_b2World_Set(b2_sim_desc);
}

CK_DLL_SFUN(b2_set_substep_count)
{
    ulib_box2d_accessAllowed;
    b2_sim_desc.substeps = GET_NEXT_INT(ARGS);
    CQ_PushCommand_b2World_Set(b2_sim_desc);
}

CK_DLL_SFUN(b2_set_simulation_rate)
{
    ulib_box2d_accessAllowed;
    b2_sim_desc.rate = GET_NEXT_FLOAT(ARGS);
    CQ_PushCommand_b2World_Set(b2_sim_desc);
}

CK_DLL_SFUN(b2_CreateWorld)
{
    ulib_box2d_accessAllowed;
    b2WorldDef def = b2DefaultWorldDef();
    // TODO impl enqueueTask and finishTask callbacks
    ckobj_to_b2WorldDef(API, &def, GET_NEXT_OBJECT(ARGS));
    RETURN_B2_ID(b2WorldId, b2CreateWorld(&def));
}

CK_DLL_SFUN(b2_DestroyWorld)
{
    ulib_box2d_accessAllowed;
    b2DestroyWorld(GET_B2_ID(b2WorldId, ARGS));
}

CK_DLL_SFUN(b2_CreateBody)
{
    ulib_box2d_accessAllowed;
    b2WorldId world_id = GET_B2_ID(b2WorldId, ARGS);
    GET_NEXT_INT(ARGS); // advance to next arg
    b2BodyDef body_def = b2DefaultBodyDef();
    ckobj_to_b2BodyDef(API, &body_def, GET_NEXT_OBJECT(ARGS));
    RETURN_B2_ID(b2BodyId, b2CreateBody(world_id, &body_def));
}

CK_DLL_SFUN(b2_DestroyBody)
{
    ulib_box2d_accessAllowed;
    b2DestroyBody(GET_B2_ID(b2BodyId, ARGS));
}

CK_DLL_SFUN(b2_MakeBox)
{
    float hx          = GET_NEXT_FLOAT(ARGS) / 2.0;
    float hy          = GET_NEXT_FLOAT(ARGS) / 2.0;
    b2Polygon polygon = b2MakeBox(hx, hy);

    RETURN->v_object = b2Polygon_create(SHRED, &polygon);
}

CK_DLL_SFUN(b2_MakePolygon)
{
    Chuck_ArrayVec2* points = GET_NEXT_VEC2_ARRAY(ARGS);
    float radius            = GET_NEXT_FLOAT(ARGS);

    static b2Vec2 points_buffer[b2_maxPolygonVertices];
    int size
      = chugin_copyCkVec2Array(points, (f32*)points_buffer, b2_maxPolygonVertices);
    b2Hull hull = b2ComputeHull(points_buffer, size);

    b2Polygon polygon = b2MakePolygon(&hull, radius);
    RETURN->v_object  = b2Polygon_create(SHRED, &polygon);
}

CK_DLL_SFUN(b2_MakeOffsetPolygon)
{
    Chuck_ArrayVec2* points = GET_NEXT_VEC2_ARRAY(ARGS);
    float radius            = GET_NEXT_FLOAT(ARGS);
    b2Transform transform   = {};
    transform.p             = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    transform.q             = b2MakeRot(GET_NEXT_FLOAT(ARGS));

    static b2Vec2 points_buffer[b2_maxPolygonVertices];
    int size
      = chugin_copyCkVec2Array(points, (f32*)points_buffer, b2_maxPolygonVertices);
    b2Hull hull = b2ComputeHull(points_buffer, size);

    b2Polygon polygon = b2MakeOffsetPolygon(&hull, radius, transform);
    RETURN->v_object  = b2Polygon_create(SHRED, &polygon);
}

CK_DLL_SFUN(b2_MakeSquare)
{
    float h           = GET_NEXT_FLOAT(ARGS) / 2.0;
    b2Polygon polygon = b2MakeSquare(h);
    RETURN->v_object  = b2Polygon_create(SHRED, &polygon);
}

CK_DLL_SFUN(b2_MakeRoundedBox)
{
    float width       = GET_NEXT_FLOAT(ARGS) / 2.0;
    float height      = GET_NEXT_FLOAT(ARGS) / 2.0;
    float radius      = GET_NEXT_FLOAT(ARGS);
    b2Polygon polygon = b2MakeRoundedBox(width, height, radius);
    RETURN->v_object  = b2Polygon_create(SHRED, &polygon);
}

CK_DLL_SFUN(b2_MakeOffsetBox)
{
    float hx          = GET_NEXT_FLOAT(ARGS) / 2.0;
    float hy          = GET_NEXT_FLOAT(ARGS) / 2.0;
    b2Vec2 center     = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    float angle       = GET_NEXT_FLOAT(ARGS);
    b2Polygon polygon = b2MakeOffsetBox(hx, hy, center, angle);
    RETURN->v_object  = b2Polygon_create(SHRED, &polygon);
}

CK_DLL_SFUN(b2_TransformPolygon)
{
    b2Transform transform = {};
    transform.p           = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    transform.q           = b2MakeRot(GET_NEXT_FLOAT(ARGS));
    b2Polygon* polygon    = ckobj_to_b2Polygon(GET_NEXT_OBJECT(ARGS));

    b2Polygon transformed_polygon = b2TransformPolygon(transform, polygon);
    RETURN->v_object              = b2Polygon_create(SHRED, &transformed_polygon);
}

CK_DLL_SFUN(b2_ComputeCircleMass)
{
    b2Circle circle  = ckobj_to_b2Circle(GET_NEXT_OBJECT(ARGS));
    float density    = GET_NEXT_FLOAT(ARGS);
    b2MassData data  = b2ComputeCircleMass(&circle, density);
    RETURN->v_object = b2MassData_create(data, SHRED);
}

CK_DLL_SFUN(b2_ComputeCapsuleMass)
{
    b2Capsule capsule = {};
    ckobj_to_b2Capsule(API, &capsule, GET_NEXT_OBJECT(ARGS));
    float density    = GET_NEXT_FLOAT(ARGS);
    b2MassData data  = b2ComputeCapsuleMass(&capsule, density);
    RETURN->v_object = b2MassData_create(data, SHRED);
}

CK_DLL_SFUN(b2_ComputePolygonMass)
{
    b2Polygon* polygon = ckobj_to_b2Polygon(GET_NEXT_OBJECT(ARGS));
    float density      = GET_NEXT_FLOAT(ARGS);
    b2MassData data    = b2ComputePolygonMass(polygon, density);
    RETURN->v_object   = b2MassData_create(data, SHRED);
}

CK_DLL_SFUN(b2_ComputeCircleAABB)
{
    b2Circle circle       = ckobj_to_b2Circle(GET_NEXT_OBJECT(ARGS));
    b2Transform transform = {};
    transform.p           = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    transform.q           = b2MakeRot(GET_NEXT_FLOAT(ARGS));

    b2AABB aabb    = b2ComputeCircleAABB(&circle, transform);
    RETURN->v_vec4 = b2AABB_to_vec4(aabb);
}

CK_DLL_SFUN(b2_ComputeCapsuleAABB)
{
    b2Capsule capsule = {};
    ckobj_to_b2Capsule(API, &capsule, GET_NEXT_OBJECT(ARGS));
    b2Transform transform = {};
    transform.p           = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    transform.q           = b2MakeRot(GET_NEXT_FLOAT(ARGS));

    b2AABB aabb    = b2ComputeCapsuleAABB(&capsule, transform);
    RETURN->v_vec4 = b2AABB_to_vec4(aabb);
}

CK_DLL_SFUN(b2_ComputePolygonAABB)
{
    b2Polygon* polygon    = ckobj_to_b2Polygon(GET_NEXT_OBJECT(ARGS));
    b2Transform transform = {};
    transform.p           = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    transform.q           = b2MakeRot(GET_NEXT_FLOAT(ARGS));

    b2AABB aabb    = b2ComputePolygonAABB(polygon, transform);
    RETURN->v_vec4 = b2AABB_to_vec4(aabb);
}

CK_DLL_SFUN(b2_ComputeSegmentAABB)
{
    b2Segment segment = {};
    ckobj_to_b2Segment(API, &segment, GET_NEXT_OBJECT(ARGS));
    b2Transform transform = {};
    transform.p           = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    transform.q           = b2MakeRot(GET_NEXT_FLOAT(ARGS));

    b2AABB aabb    = b2ComputeSegmentAABB(&segment, transform);
    RETURN->v_vec4 = b2AABB_to_vec4(aabb);
}

CK_DLL_SFUN(b2_PointInCircle)
{
    b2Vec2 point    = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    b2Circle circle = ckobj_to_b2Circle(GET_NEXT_OBJECT(ARGS));
    RETURN->v_int   = b2PointInCircle(point, &circle);
}

CK_DLL_SFUN(b2_PointInCapsule)
{
    b2Vec2 point      = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    b2Capsule capsule = {};
    ckobj_to_b2Capsule(API, &capsule, GET_NEXT_OBJECT(ARGS));
    RETURN->v_int = b2PointInCapsule(point, &capsule);
}

CK_DLL_SFUN(b2_PointInPolygon)
{
    b2Vec2 point       = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    b2Polygon* polygon = ckobj_to_b2Polygon(GET_NEXT_OBJECT(ARGS));
    RETURN->v_int      = b2PointInPolygon(point, polygon);
}

CK_DLL_SFUN(b2_RayCastSegment)
{
    b2RayCastInput input = {};
    ckobj_to_b2RayCastInput(&input, GET_NEXT_OBJECT(ARGS));
    b2Segment segment = {};
    ckobj_to_b2Segment(API, &segment, GET_NEXT_OBJECT(ARGS));
    bool one_sided = GET_NEXT_INT(ARGS);

    b2CastOutput output = b2RayCastSegment(&input, &segment, one_sided);
    RETURN->v_object    = b2CastOutput_create(&output, SHRED);
}

CK_DLL_SFUN(b2_RayCastPolygon)
{
    b2RayCastInput input = {};
    ckobj_to_b2RayCastInput(&input, GET_NEXT_OBJECT(ARGS));
    b2Polygon* polygon = ckobj_to_b2Polygon(GET_NEXT_OBJECT(ARGS));

    b2CastOutput output = b2RayCastPolygon(&input, polygon);
    RETURN->v_object    = b2CastOutput_create(&output, SHRED);
}

CK_DLL_SFUN(b2_ShapeCastCircle)
{
    b2ShapeCastInput input = {};
    ckobj_to_b2ShapeCastInput(&input, GET_NEXT_OBJECT(ARGS));
    b2Circle circle = ckobj_to_b2Circle(GET_NEXT_OBJECT(ARGS));

    b2CastOutput output = b2ShapeCastCircle(&input, &circle);
    RETURN->v_object    = b2CastOutput_create(&output, SHRED);
}

CK_DLL_SFUN(b2_ShapeCastCapsule)
{
    b2ShapeCastInput input = {};
    ckobj_to_b2ShapeCastInput(&input, GET_NEXT_OBJECT(ARGS));
    b2Capsule capsule = {};
    ckobj_to_b2Capsule(API, &capsule, GET_NEXT_OBJECT(ARGS));

    b2CastOutput output = b2ShapeCastCapsule(&input, &capsule);
    RETURN->v_object    = b2CastOutput_create(&output, SHRED);
}

CK_DLL_SFUN(b2_ShapeCastSegment)
{
    b2ShapeCastInput input = {};
    ckobj_to_b2ShapeCastInput(&input, GET_NEXT_OBJECT(ARGS));
    b2Segment segment = {};
    ckobj_to_b2Segment(API, &segment, GET_NEXT_OBJECT(ARGS));

    b2CastOutput output = b2ShapeCastSegment(&input, &segment);
    RETURN->v_object    = b2CastOutput_create(&output, SHRED);
}

CK_DLL_SFUN(b2_ShapeCastPolygon)
{
    b2ShapeCastInput input = {};
    ckobj_to_b2ShapeCastInput(&input, GET_NEXT_OBJECT(ARGS));
    b2Polygon* polygon = ckobj_to_b2Polygon(GET_NEXT_OBJECT(ARGS));

    b2CastOutput output = b2ShapeCastPolygon(&input, polygon);
    RETURN->v_object    = b2CastOutput_create(&output, SHRED);
}

CK_DLL_SFUN(b2_SegmentDistance)
{
    b2Vec2 p1 = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    b2Vec2 q1 = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    b2Vec2 p2 = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    b2Vec2 q2 = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));

    b2SegmentDistanceResult result = b2SegmentDistance(p1, q1, p2, q2);

    Chuck_Object* result_obj
      = chugin_createCkObj("b2SegmentDistanceResult", false, SHRED);
    b2SegmentDistanceResult_to_ckobj(result_obj, &result);

    RETURN->v_object = result_obj;
}

// ============================================================================
// b2World
// ============================================================================

CK_DLL_SFUN(b2_World_IsValid)
{
    RETURN->v_int = b2World_IsValid(GET_B2_ID(b2WorldId, ARGS));
}

CK_DLL_SFUN(b2_World_Draw)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2WorldId, world_id);
    b2DebugDraw draw = {};
    ckobj_to_b2DebugDraw(&draw, GET_NEXT_OBJECT(ARGS));
    /// Call this to draw shapes and other debug draw data
    b2World_Draw(world_id, &draw);
}

CK_DLL_SFUN(b2_World_GetBodyEvents)
{
    ulib_box2d_accessAllowed;
    b2WorldId world_id = GET_B2_ID(b2WorldId, ARGS);
    GET_NEXT_INT(ARGS); // advance to next arg
    Chuck_ArrayInt* body_event_array = GET_NEXT_OBJECT_ARRAY(ARGS);

    b2BodyEvents body_events = b2World_GetBodyEvents(world_id);

    // first create new body events in pool
    int pool_len = ARENA_LENGTH(&b2Body_move_event_pool, Chuck_Object*);
    int diff     = body_events.moveCount - pool_len;
    if (diff > 0) {
        Chuck_Object** p
          = ARENA_PUSH_COUNT(&b2Body_move_event_pool, Chuck_Object*, diff);
        for (int i = 0; i < diff; i++)
            p[i] = chugin_createCkObj("b2BodyMoveEvent", true, SHRED);
    }

    // TODO switch to use array_int_set after updating chuck version
    // int ck_array_len         = API->object->array_int_size(body_event_array);

    // clear array
    API->object->array_int_clear(body_event_array);

    // add new body events to array
    for (int i = 0; i < body_events.moveCount; i++) {
        Chuck_Object* ckobj
          = *ARENA_GET_TYPE(&b2Body_move_event_pool, Chuck_Object*, i);
        b2BodyMoveEvent_to_ckobj(API, ckobj, &body_events.moveEvents[i]);
        API->object->array_int_push_back(body_event_array, (t_CKUINT)ckobj);
    }
}

CK_DLL_SFUN(b2_World_GetSensorEvents)
{
    ulib_box2d_accessAllowed;
    b2WorldId world_id = GET_B2_ID(b2WorldId, ARGS);
    GET_NEXT_INT(ARGS); // advance to next arg
    Chuck_ArrayInt* begin_sensor_events = GET_NEXT_OBJECT_ARRAY(ARGS);
    Chuck_ArrayInt* end_sensor_events   = GET_NEXT_OBJECT_ARRAY(ARGS);

    // TODO switch to use array_int_set after updating chuck version

    b2SensorEvents sensor_events = b2World_GetSensorEvents(world_id);

    if (begin_sensor_events) {
        API->object->array_int_clear(begin_sensor_events);

        // add new sensor events to arrays
        for (int i = 0; i < sensor_events.beginCount; i++) {
            API->object->array_int_push_back(
              begin_sensor_events,
              B2_ID_TO_CKINT(sensor_events.beginEvents[i].sensorShapeId));
            API->object->array_int_push_back(
              begin_sensor_events,
              B2_ID_TO_CKINT(sensor_events.beginEvents[i].visitorShapeId));
        }

        ASSERT(API->object->array_int_size(begin_sensor_events)
               == sensor_events.beginCount * 2);
    }

    if (end_sensor_events) {
        API->object->array_int_clear(end_sensor_events);

        for (int i = 0; i < sensor_events.endCount; i++) {
            API->object->array_int_push_back(
              end_sensor_events,
              B2_ID_TO_CKINT(sensor_events.endEvents[i].sensorShapeId));
            API->object->array_int_push_back(
              end_sensor_events,
              B2_ID_TO_CKINT(sensor_events.endEvents[i].visitorShapeId));
        }

        ASSERT(API->object->array_int_size(end_sensor_events)
               == sensor_events.endCount * 2);
    }
}

CK_DLL_SFUN(b2_World_GetContactEvents)
{
    ulib_box2d_accessAllowed;

    GET_NEXT_B2_ID(b2WorldId, world_id);
    Chuck_ArrayInt* begin_contact_events = GET_NEXT_OBJECT_ARRAY(ARGS);
    Chuck_ArrayInt* end_contact_events   = GET_NEXT_OBJECT_ARRAY(ARGS);
    Chuck_ArrayInt* hit_events           = GET_NEXT_OBJECT_ARRAY(ARGS);

    b2ContactEvents contact_events = b2World_GetContactEvents(world_id);

    // TODO switch to use array_int_set after updating chuck version

    if (begin_contact_events) {
        API->object->array_int_clear(begin_contact_events);
        for (int i = 0; i < contact_events.beginCount; i++) {
            API->object->array_int_push_back(
              begin_contact_events,
              B2_ID_TO_CKINT(contact_events.beginEvents[i].shapeIdA));
            API->object->array_int_push_back(
              begin_contact_events,
              B2_ID_TO_CKINT(contact_events.beginEvents[i].shapeIdB));
        }
    }

    if (end_contact_events) {
        API->object->array_int_clear(end_contact_events);

        // populate begin_contact_events and end_contact_events
        for (int i = 0; i < contact_events.endCount; i++) {
            API->object->array_int_push_back(
              end_contact_events, B2_ID_TO_CKINT(contact_events.endEvents[i].shapeIdA));
            API->object->array_int_push_back(
              end_contact_events, B2_ID_TO_CKINT(contact_events.endEvents[i].shapeIdB));
        }
    }

    if (hit_events) {
        API->object->array_int_clear(hit_events);
        // populate hit_events
        // first create new body events in pool
        int pool_len = ARENA_LENGTH(&b2_contact_hit_event_pool, Chuck_Object*);
        int diff     = contact_events.hitCount - pool_len;
        if (diff > 0) {
            Chuck_Object** p
              = ARENA_PUSH_COUNT(&b2_contact_hit_event_pool, Chuck_Object*, diff);
            for (int i = 0; i < diff; i++)
                p[i] = chugin_createCkObj("b2ContactHitEvent", true, SHRED);
        }

        // add new body events to array
        for (int i = 0; i < contact_events.hitCount; i++) {
            Chuck_Object* ckobj
              = *ARENA_GET_TYPE(&b2_contact_hit_event_pool, Chuck_Object*, i);
            b2ContactHitEvent_to_ckobj(API, ckobj, &contact_events.hitEvents[i]);
            API->object->array_int_push_back(hit_events, (t_CKUINT)ckobj);
        }
    }
}

static bool b2_OverlapResultFcn(b2ShapeId shapeId, void* context)
{
    Chuck_ArrayInt* overlapping_shapes = (Chuck_ArrayInt*)context;
    g_chuglAPI->object->array_int_push_back(overlapping_shapes,
                                            B2_ID_TO_CKINT(shapeId));
    return true; // return false to terminate overlap query
}

CK_DLL_SFUN(b2_World_OverlapAABB)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2WorldId, world_id);
    b2AABB aabb          = vec4_to_b2AABB(GET_NEXT_VEC4(ARGS));
    b2QueryFilter filter = ckobj_to_b2QueryFilter(GET_NEXT_OBJECT(ARGS));

    static_assert(sizeof(b2ShapeId) == sizeof(t_CKINT), "b2ShapeId size mismatch");

    Chuck_Object* overlapping_shapes
      = chugin_createCkObj(g_chuck_types.int_array, false, SHRED);

    b2World_OverlapAABB(world_id, aabb, filter, b2_OverlapResultFcn,
                        overlapping_shapes);

    RETURN->v_object = overlapping_shapes;
}

CK_DLL_SFUN(b2_World_OverlapCircle)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2WorldId, world_id);

    b2Circle circle = ckobj_to_b2Circle(GET_NEXT_OBJECT(ARGS));

    b2Transform transform = {};
    transform.p           = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    transform.q           = b2MakeRot(GET_NEXT_FLOAT(ARGS));

    b2QueryFilter filter = ckobj_to_b2QueryFilter(GET_NEXT_OBJECT(ARGS));

    Chuck_Object* overlapping_shapes
      = chugin_createCkObj(g_chuck_types.int_array, false, SHRED);

    b2World_OverlapCircle(world_id, &circle, transform, filter, b2_OverlapResultFcn,
                          overlapping_shapes);

    RETURN->v_object = overlapping_shapes;
}

CK_DLL_SFUN(b2_World_OverlapCapsule)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2WorldId, world_id);

    b2Capsule capsule = {};
    ckobj_to_b2Capsule(API, &capsule, GET_NEXT_OBJECT(ARGS));

    b2Transform transform = {};
    transform.p           = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    transform.q           = b2MakeRot(GET_NEXT_FLOAT(ARGS));

    b2QueryFilter filter = ckobj_to_b2QueryFilter(GET_NEXT_OBJECT(ARGS));

    Chuck_Object* overlapping_shapes
      = chugin_createCkObj(g_chuck_types.int_array, false, SHRED);

    b2World_OverlapCapsule(world_id, &capsule, transform, filter, b2_OverlapResultFcn,
                           overlapping_shapes);

    RETURN->v_object = overlapping_shapes;
}

CK_DLL_SFUN(b2_World_OverlapPolygon)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2WorldId, world_id);

    b2Polygon* polygon = ckobj_to_b2Polygon(GET_NEXT_OBJECT(ARGS));

    b2Transform transform = {};
    transform.p           = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    transform.q           = b2MakeRot(GET_NEXT_FLOAT(ARGS));

    b2QueryFilter filter = ckobj_to_b2QueryFilter(GET_NEXT_OBJECT(ARGS));

    Chuck_Object* overlapping_shapes
      = chugin_createCkObj(g_chuck_types.int_array, false, SHRED);

    b2World_OverlapPolygon(world_id, polygon, transform, filter, b2_OverlapResultFcn,
                           overlapping_shapes);

    RETURN->v_object = overlapping_shapes;
}

CK_DLL_SFUN(b2_World_CastRayClosest)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2WorldId, world_id);
    b2Vec2 origin        = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    b2Vec2 translation   = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    b2QueryFilter filter = ckobj_to_b2QueryFilter(GET_NEXT_OBJECT(ARGS));

    b2RayResult result = b2World_CastRayClosest(world_id, origin, translation, filter);

    Chuck_Object* ckobj = chugin_createCkObj("b2RayResult", false, SHRED);
    b2RayResult_to_ckobj(ckobj, &result);

    RETURN->v_object = ckobj;
}

// This callback finds the closest hit. This is the most common callback used in games.
static float b2_RayCastClosestFcn(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal,
                                  float fraction, void* context)
{
    b2RayResult* rayResult = (b2RayResult*)context;
    rayResult->shapeId     = shapeId;
    rayResult->point       = point;
    rayResult->normal      = normal;
    rayResult->fraction    = fraction;
    rayResult->hit         = true;
    return fraction;
}

CK_DLL_SFUN(b2_World_CastCircleClosest)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2WorldId, world_id);
    b2Circle circle      = ckobj_to_b2Circle(GET_NEXT_OBJECT(ARGS));
    b2Vec2 origin        = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    b2Vec2 direction     = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    b2QueryFilter filter = ckobj_to_b2QueryFilter(GET_NEXT_OBJECT(ARGS));

    b2Transform origin_transform = {};
    origin_transform.p           = origin;

    b2RayResult result = {};
    b2World_CastCircle(world_id, &circle, origin_transform, direction, filter,
                       b2_RayCastClosestFcn, &result);

    Chuck_Object* ckobj = chugin_createCkObj("b2RayResult", false, SHRED);
    b2RayResult_to_ckobj(ckobj, &result);

    RETURN->v_object = ckobj;
}

CK_DLL_SFUN(b2_World_CastCapsuleClosest)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2WorldId, world_id);
    b2Capsule capsule = {};
    ckobj_to_b2Capsule(API, &capsule, GET_NEXT_OBJECT(ARGS));
    b2Transform origin_transform = {};
    origin_transform.p           = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    origin_transform.q           = b2MakeRot(GET_NEXT_FLOAT(ARGS));
    b2Vec2 direction             = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    b2QueryFilter filter         = ckobj_to_b2QueryFilter(GET_NEXT_OBJECT(ARGS));

    b2RayResult result = {};
    b2World_CastCapsule(world_id, &capsule, origin_transform, direction, filter,
                        b2_RayCastClosestFcn, &result);

    Chuck_Object* ckobj = chugin_createCkObj("b2RayResult", false, SHRED);
    b2RayResult_to_ckobj(ckobj, &result);

    RETURN->v_object = ckobj;
}

CK_DLL_SFUN(b2_World_CastPolygonClosest)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2WorldId, world_id);
    b2Polygon* polygon           = ckobj_to_b2Polygon(GET_NEXT_OBJECT(ARGS));
    b2Transform origin_transform = {};
    origin_transform.p           = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    origin_transform.q           = b2MakeRot(GET_NEXT_FLOAT(ARGS));
    b2Vec2 direction             = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    b2QueryFilter filter         = ckobj_to_b2QueryFilter(GET_NEXT_OBJECT(ARGS));

    b2RayResult result = {};
    b2World_CastPolygon(world_id, polygon, origin_transform, direction, filter,
                        b2_RayCastClosestFcn, &result);

    Chuck_Object* ckobj = chugin_createCkObj("b2RayResult", false, SHRED);
    b2RayResult_to_ckobj(ckobj, &result);

    RETURN->v_object = ckobj;
}

CK_DLL_SFUN(b2_World_EnableSleeping)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2WorldId, world_id);
    b2World_EnableSleeping(world_id, GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(b2_World_EnableContinuous)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2WorldId, world_id);
    b2World_EnableContinuous(world_id, GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(b2_World_SetRestitutionThreshold)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2WorldId, world_id);
    b2World_SetRestitutionThreshold(world_id, GET_NEXT_FLOAT(ARGS));
}

CK_DLL_SFUN(b2_World_SetHitEventThreshold)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2WorldId, world_id);
    b2World_SetHitEventThreshold(world_id, GET_NEXT_FLOAT(ARGS));
}

CK_DLL_SFUN(b2_World_SetGravity)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2WorldId, world_id);
    b2World_SetGravity(world_id, vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS)));
}

CK_DLL_SFUN(b2_World_GetGravity)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2WorldId, world_id);
    b2Vec2 gravity = b2World_GetGravity(world_id);
    RETURN->v_vec2 = { gravity.x, gravity.y };
}

CK_DLL_SFUN(b2_World_Explode)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2WorldId, world_id);
    b2Vec2 position = vec2_to_b2Vec2(GET_NEXT_VEC2(ARGS));
    float radius    = GET_NEXT_FLOAT(ARGS);
    float impulse   = GET_NEXT_FLOAT(ARGS);
    b2World_Explode(world_id, position, radius, impulse);
}

CK_DLL_SFUN(b2_World_SetContactTuning)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2WorldId, world_id);
    float hertz        = GET_NEXT_FLOAT(ARGS);
    float dampingRatio = GET_NEXT_FLOAT(ARGS);
    float pushVelocity = GET_NEXT_FLOAT(ARGS);
    b2World_SetContactTuning(world_id, hertz, dampingRatio, pushVelocity);
}

// ============================================================================
// b2WorldDef
// ============================================================================
static void b2WorldDef_to_ckobj(CK_DL_API API, Chuck_Object* ckobj, b2WorldDef* obj)
{
    OBJ_MEMBER_VEC2(ckobj, b2WorldDef_gravity_offset)
      = { obj->gravity.x, obj->gravity.y };
    OBJ_MEMBER_FLOAT(ckobj, b2WorldDef_restitutionThreshold_offset)
      = obj->restitutionThreshold;
    OBJ_MEMBER_FLOAT(ckobj, b2WorldDef_contactPushoutVelocity_offset)
      = obj->contactPushoutVelocity;
    OBJ_MEMBER_FLOAT(ckobj, b2WorldDef_hitEventThreshold_offset)
      = obj->hitEventThreshold;
    OBJ_MEMBER_FLOAT(ckobj, b2WorldDef_contactHertz_offset) = obj->contactHertz;
    OBJ_MEMBER_FLOAT(ckobj, b2WorldDef_contactDampingRatio_offset)
      = obj->contactDampingRatio;
    OBJ_MEMBER_FLOAT(ckobj, b2WorldDef_jointHertz_offset) = obj->jointHertz;
    OBJ_MEMBER_FLOAT(ckobj, b2WorldDef_jointDampingRatio_offset)
      = obj->jointDampingRatio;
    OBJ_MEMBER_FLOAT(ckobj, b2WorldDef_maximumLinearVelocity_offset)
      = obj->maximumLinearVelocity;
    OBJ_MEMBER_INT(ckobj, b2WorldDef_enableSleep_offset)     = obj->enableSleep;
    OBJ_MEMBER_INT(ckobj, b2WorldDef_enableContinous_offset) = obj->enableContinous;
    OBJ_MEMBER_INT(ckobj, b2WorldDef_workerCount_offset)     = obj->workerCount;
}

static void ckobj_to_b2WorldDef(CK_DL_API API, b2WorldDef* obj, Chuck_Object* ckobj)
{
    t_CKVEC2 gravity_vec2 = OBJ_MEMBER_VEC2(ckobj, b2WorldDef_gravity_offset);
    obj->gravity          = { (float)gravity_vec2.x, (float)gravity_vec2.y };
    obj->restitutionThreshold
      = (float)OBJ_MEMBER_FLOAT(ckobj, b2WorldDef_restitutionThreshold_offset);
    obj->contactPushoutVelocity
      = (float)OBJ_MEMBER_FLOAT(ckobj, b2WorldDef_contactPushoutVelocity_offset);
    obj->hitEventThreshold
      = (float)OBJ_MEMBER_FLOAT(ckobj, b2WorldDef_hitEventThreshold_offset);
    obj->contactHertz = (float)OBJ_MEMBER_FLOAT(ckobj, b2WorldDef_contactHertz_offset);
    obj->contactDampingRatio
      = (float)OBJ_MEMBER_FLOAT(ckobj, b2WorldDef_contactDampingRatio_offset);
    obj->jointHertz = (float)OBJ_MEMBER_FLOAT(ckobj, b2WorldDef_jointHertz_offset);
    obj->jointDampingRatio
      = (float)OBJ_MEMBER_FLOAT(ckobj, b2WorldDef_jointDampingRatio_offset);
    obj->maximumLinearVelocity
      = (float)OBJ_MEMBER_FLOAT(ckobj, b2WorldDef_maximumLinearVelocity_offset);
    obj->enableSleep     = OBJ_MEMBER_INT(ckobj, b2WorldDef_enableSleep_offset);
    obj->enableContinous = OBJ_MEMBER_INT(ckobj, b2WorldDef_enableContinous_offset);
    obj->workerCount     = OBJ_MEMBER_INT(ckobj, b2WorldDef_workerCount_offset);
}

CK_DLL_CTOR(b2WorldDef_ctor)
{
    b2WorldDef default_world_def = b2DefaultWorldDef();
    b2WorldDef_to_ckobj(API, SELF, &default_world_def);
}

// ============================================================================
// b2Filter
// ============================================================================

static void b2Filter_to_ckobj(CK_DL_API API, Chuck_Object* ckobj, b2Filter* obj)
{
    OBJ_MEMBER_INT(ckobj, b2Filter_categoryBits_offset) = obj->categoryBits;
    OBJ_MEMBER_INT(ckobj, b2Filter_maskBits_offset)     = obj->maskBits;
    ASSERT(OBJ_MEMBER_INT(ckobj, b2Filter_maskBits_offset) == obj->maskBits);
    OBJ_MEMBER_INT(ckobj, b2Filter_groupIndex_offset) = obj->groupIndex;
}

static void ckobj_to_b2Filter(CK_DL_API API, b2Filter* obj, Chuck_Object* ckobj)
{
    obj->categoryBits = OBJ_MEMBER_INT(ckobj, b2Filter_categoryBits_offset);
    obj->maskBits     = OBJ_MEMBER_INT(ckobj, b2Filter_maskBits_offset);
    obj->groupIndex   = OBJ_MEMBER_INT(ckobj, b2Filter_groupIndex_offset);
}

CK_DLL_CTOR(b2Filter_ctor)
{
    b2Filter default_filter = b2DefaultFilter();
    b2Filter_to_ckobj(API, SELF, &default_filter);
}

// ============================================================================
// b2QueryFilter
// ============================================================================

static void b2QueryFilter_to_ckobj(CK_DL_API API, Chuck_Object* ckobj,
                                   b2QueryFilter* obj)
{
    OBJ_MEMBER_INT(ckobj, b2QueryFilter_categoryBits_offset) = obj->categoryBits;
    OBJ_MEMBER_INT(ckobj, b2QueryFilter_maskBits_offset)     = obj->maskBits;
}

static b2QueryFilter ckobj_to_b2QueryFilter(Chuck_Object* ckobj)
{
    CK_DL_API API     = g_chuglAPI;
    b2QueryFilter obj = {};
    obj.categoryBits  = OBJ_MEMBER_UINT(ckobj, b2QueryFilter_categoryBits_offset);
    obj.maskBits      = OBJ_MEMBER_UINT(ckobj, b2QueryFilter_maskBits_offset);
    return obj;
}

CK_DLL_CTOR(b2QueryFilter_ctor)
{
    b2QueryFilter default_query_filter = b2DefaultQueryFilter();
    b2QueryFilter_to_ckobj(API, SELF, &default_query_filter);
}

// ============================================================================
// b2SegmentDistanceResult
// ============================================================================

static void b2SegmentDistanceResult_to_ckobj(Chuck_Object* ckobj,
                                             b2SegmentDistanceResult* obj)
{
    CK_DL_API API = g_chuglAPI;
    OBJ_MEMBER_VEC2(ckobj, b2SegmentDistanceResult_closest1_offset)
      = { obj->closest1.x, obj->closest1.y };
    OBJ_MEMBER_VEC2(ckobj, b2SegmentDistanceResult_closest2_offset)
      = { obj->closest2.x, obj->closest2.y };
    OBJ_MEMBER_FLOAT(ckobj, b2SegmentDistanceResult_fraction1_offset) = obj->fraction1;
    OBJ_MEMBER_FLOAT(ckobj, b2SegmentDistanceResult_fraction2_offset) = obj->fraction2;
    OBJ_MEMBER_FLOAT(ckobj, b2SegmentDistanceResult_distanceSquared_offset)
      = obj->distanceSquared;
}

// static void ckobj_to_b2SegmentDistanceResult(b2SegmentDistanceResult* obj,
//                                              Chuck_Object* ckobj)
// {
//     CK_DL_API API = g_chuglAPI;
//     t_CKVEC2 closest1_vec2
//       = OBJ_MEMBER_VEC2(ckobj, b2SegmentDistanceResult_closest1_offset);
//     obj->closest1 = { (float)closest1_vec2.x, (float)closest1_vec2.y };
//     t_CKVEC2 closest2_vec2
//       = OBJ_MEMBER_VEC2(ckobj, b2SegmentDistanceResult_closest2_offset);
//     obj->closest2 = { (float)closest2_vec2.x, (float)closest2_vec2.y };
//     obj->fraction1
//       = (float)OBJ_MEMBER_FLOAT(ckobj, b2SegmentDistanceResult_fraction1_offset);
//     obj->fraction2
//       = (float)OBJ_MEMBER_FLOAT(ckobj, b2SegmentDistanceResult_fraction2_offset);
//     obj->distanceSquared
//       = (float)OBJ_MEMBER_FLOAT(ckobj,
//       b2SegmentDistanceResult_distanceSquared_offset);
// }

// ============================================================================
// b2MassData
// ============================================================================

static void b2MassData_to_ckobj(Chuck_Object* ckobj, b2MassData* obj)
{
    CK_DL_API API                                    = g_chuglAPI;
    OBJ_MEMBER_FLOAT(ckobj, b2MassData_mass_offset)  = obj->mass;
    OBJ_MEMBER_VEC2(ckobj, b2MassData_center_offset) = { obj->center.x, obj->center.y };
    OBJ_MEMBER_FLOAT(ckobj, b2MassData_rotationalInertia_offset)
      = obj->rotationalInertia;
}

static void ckobj_to_b2MassData(b2MassData* obj, Chuck_Object* ckobj)
{
    CK_DL_API API        = g_chuglAPI;
    obj->mass            = (float)OBJ_MEMBER_FLOAT(ckobj, b2MassData_mass_offset);
    t_CKVEC2 center_vec2 = OBJ_MEMBER_VEC2(ckobj, b2MassData_center_offset);
    obj->center          = { (float)center_vec2.x, (float)center_vec2.y };
    obj->rotationalInertia
      = (float)OBJ_MEMBER_FLOAT(ckobj, b2MassData_rotationalInertia_offset);
}

static Chuck_Object* b2MassData_create(b2MassData obj, Chuck_VM_Shred* shred)
{
    Chuck_Object* ckobj = chugin_createCkObj("b2MassData", false, shred);
    b2MassData_to_ckobj(ckobj, &obj);
    return ckobj;
}

// ============================================================================
// b2RayCastInput
// ============================================================================

// static void b2RayCastInput_to_ckobj(Chuck_Object* ckobj, b2RayCastInput* obj)
// {
//     CK_DL_API API = g_chuglAPI;
//     OBJ_MEMBER_VEC2(ckobj, b2RayCastInput_origin_offset)
//       = { obj->origin.x, obj->origin.y };
//     OBJ_MEMBER_VEC2(ckobj, b2RayCastInput_translation_offset)
//       = { obj->translation.x, obj->translation.y };
//     OBJ_MEMBER_FLOAT(ckobj, b2RayCastInput_maxFraction_offset) = obj->maxFraction;
// }

static void ckobj_to_b2RayCastInput(b2RayCastInput* obj, Chuck_Object* ckobj)
{
    CK_DL_API API        = g_chuglAPI;
    t_CKVEC2 origin_vec2 = OBJ_MEMBER_VEC2(ckobj, b2RayCastInput_origin_offset);
    obj->origin          = { (float)origin_vec2.x, (float)origin_vec2.y };
    t_CKVEC2 translation_vec2
      = OBJ_MEMBER_VEC2(ckobj, b2RayCastInput_translation_offset);
    obj->translation = { (float)translation_vec2.x, (float)translation_vec2.y };
    obj->maxFraction
      = (float)OBJ_MEMBER_FLOAT(ckobj, b2RayCastInput_maxFraction_offset);
}

// ============================================================================
// b2ShapeCastInput
// ============================================================================

// static void b2ShapeCastInput_to_ckobj(Chuck_Object* ckobj, b2ShapeCastInput* obj)
// {
//     CK_DL_API API = g_chuglAPI;
//     vec2[] _to_ckobj(API, OBJ_MEMBER_OBJECT(ckobj, b2ShapeCastInput_points_offset),
//                      &obj->points);
//     OBJ_MEMBER_FLOAT(ckobj, b2ShapeCastInput_radius_offset) = obj->radius;
//     OBJ_MEMBER_VEC2(ckobj, b2ShapeCastInput_translation_offset)
//       = { obj->translation.x, obj->translation.y };
//     OBJ_MEMBER_FLOAT(ckobj, b2ShapeCastInput_maxFraction_offset) = obj->maxFraction;
// }

static void ckobj_to_b2ShapeCastInput(b2ShapeCastInput* obj, Chuck_Object* ckobj)
{
    CK_DL_API API = g_chuglAPI;

    int len = chugin_copyCkVec2Array(
      OBJ_MEMBER_VEC2_ARRAY(ckobj, b2ShapeCastInput_points_offset), (f32*)&obj->points,
      ARRAY_LENGTH(obj->points));
    static_assert(ARRAY_LENGTH(obj->points) == b2_maxPolygonVertices,
                  "b2ShapeCastInput points size mismatch");
    obj->count  = len;
    obj->radius = (float)OBJ_MEMBER_FLOAT(ckobj, b2ShapeCastInput_radius_offset);
    t_CKVEC2 translation_vec2
      = OBJ_MEMBER_VEC2(ckobj, b2ShapeCastInput_translation_offset);
    obj->translation = { (float)translation_vec2.x, (float)translation_vec2.y };
    obj->maxFraction
      = (float)OBJ_MEMBER_FLOAT(ckobj, b2ShapeCastInput_maxFraction_offset);
}

CK_DLL_CTOR(b2ShapeCastInput_ctor)
{
}

// ============================================================================
// b2RayResult
// ============================================================================

static void b2RayResult_to_ckobj(Chuck_Object* ckobj, b2RayResult* obj)
{
    CK_DL_API API                                                = g_chuglAPI;
    OBJ_MEMBER_B2_ID(b2ShapeId, ckobj, b2RayResult_shape_offset) = obj->shapeId;
    OBJ_MEMBER_VEC2(ckobj, b2RayResult_point_offset) = { obj->point.x, obj->point.y };
    OBJ_MEMBER_VEC2(ckobj, b2RayResult_normal_offset)
      = { obj->normal.x, obj->normal.y };
    OBJ_MEMBER_FLOAT(ckobj, b2RayResult_fraction_offset) = obj->fraction;
}

// static void ckobj_to_b2RayResult(b2RayResult* obj, Chuck_Object* ckobj)
//{
//     CK_DL_API API        = g_chuglAPI;
//     obj->shapeId         = OBJ_MEMBER_B2_ID(b2ShapeId, ckobj,
//     b2RayResult_shape_offset); t_CKVEC2 point_vec2  = OBJ_MEMBER_VEC2(ckobj,
//     b2RayResult_point_offset); obj->point           = { (float)point_vec2.x,
//     (float)point_vec2.y }; t_CKVEC2 normal_vec2 = OBJ_MEMBER_VEC2(ckobj,
//     b2RayResult_normal_offset); obj->normal          = { (float)normal_vec2.x,
//     (float)normal_vec2.y }; obj->fraction        = (float)OBJ_MEMBER_FLOAT(ckobj,
//     b2RayResult_fraction_offset);
// }

// ============================================================================
// b2ShapeDef
// ============================================================================

static void b2ShapeDef_to_ckobj(CK_DL_API API, Chuck_Object* ckobj, b2ShapeDef* obj)
{
    OBJ_MEMBER_FLOAT(ckobj, b2ShapeDef_friction_offset)    = obj->friction;
    OBJ_MEMBER_FLOAT(ckobj, b2ShapeDef_restitution_offset) = obj->restitution;
    OBJ_MEMBER_FLOAT(ckobj, b2ShapeDef_density_offset)     = obj->density;
    // b2Filter_to_ckobj(API, OBJ_MEMBER_OBJECT(ckobj,
    // b2ShapeDef_filter_offset),
    //                   &obj->filter);
    b2Filter_to_ckobj(API,
                      (Chuck_Object*)OBJ_MEMBER_UINT(ckobj, b2ShapeDef_filter_offset),
                      &obj->filter);
    OBJ_MEMBER_INT(ckobj, b2ShapeDef_isSensor_offset) = obj->isSensor;
    OBJ_MEMBER_INT(ckobj, b2ShapeDef_enableSensorEvents_offset)
      = obj->enableSensorEvents;
    OBJ_MEMBER_INT(ckobj, b2ShapeDef_enableContactEvents_offset)
      = obj->enableContactEvents;
    OBJ_MEMBER_INT(ckobj, b2ShapeDef_enableHitEvents_offset) = obj->enableHitEvents;
    OBJ_MEMBER_INT(ckobj, b2ShapeDef_enablePreSolveEvents_offset)
      = obj->enablePreSolveEvents;
    OBJ_MEMBER_INT(ckobj, b2ShapeDef_forceContactCreation_offset)
      = obj->forceContactCreation;
}

static void ckobj_to_b2ShapeDef(CK_DL_API API, b2ShapeDef* obj, Chuck_Object* ckobj)
{
    obj->friction    = (float)OBJ_MEMBER_FLOAT(ckobj, b2ShapeDef_friction_offset);
    obj->restitution = (float)OBJ_MEMBER_FLOAT(ckobj, b2ShapeDef_restitution_offset);
    obj->density     = (float)OBJ_MEMBER_FLOAT(ckobj, b2ShapeDef_density_offset);
    ckobj_to_b2Filter(API, &obj->filter,
                      OBJ_MEMBER_OBJECT(ckobj, b2ShapeDef_filter_offset));
    obj->isSensor = OBJ_MEMBER_INT(ckobj, b2ShapeDef_isSensor_offset);
    obj->enableSensorEvents
      = OBJ_MEMBER_INT(ckobj, b2ShapeDef_enableSensorEvents_offset);
    obj->enableContactEvents
      = OBJ_MEMBER_INT(ckobj, b2ShapeDef_enableContactEvents_offset);
    obj->enableHitEvents = OBJ_MEMBER_INT(ckobj, b2ShapeDef_enableHitEvents_offset);
    obj->enablePreSolveEvents
      = OBJ_MEMBER_INT(ckobj, b2ShapeDef_enablePreSolveEvents_offset);
    obj->forceContactCreation
      = OBJ_MEMBER_INT(ckobj, b2ShapeDef_forceContactCreation_offset);
}

CK_DLL_CTOR(b2ShapeDef_ctor)
{
    // https://github.com/ccrma/chuck/issues/449
    // member vars which are themselves Chuck_Objects are NOT instantiated
    // instantiating manually
    OBJ_MEMBER_OBJECT(SELF, b2ShapeDef_filter_offset) = chugin_createCkObj(
      "b2Filter", true, SHRED); // adding refcount just in case gc isn't set up

    b2ShapeDef default_shape_def = b2DefaultShapeDef();
    b2ShapeDef_to_ckobj(API, SELF, &default_shape_def);
}

// ============================================================================
// b2Polygon
// ============================================================================

CK_DLL_DTOR(b2Polygon_dtor)
{
    b2PolygonPool::del(&b2polygon_pool, OBJ_MEMBER_UINT(SELF, b2Polygon_data_offset));
}

CK_DLL_MFUN(b2Polygon_get_vertices)
{
    b2Polygon* polygon = ckobj_to_b2Polygon(SELF);

    RETURN->v_object = (Chuck_Object*)chugin_createCkFloat2Array(
      (glm::vec2*)&polygon->vertices, polygon->count, false, SHRED);
}

CK_DLL_MFUN(b2Polygon_get_normals)
{
    b2Polygon* polygon = ckobj_to_b2Polygon(SELF);

    RETURN->v_object = (Chuck_Object*)chugin_createCkFloat2Array(
      (glm::vec2*)&polygon->normals, polygon->count, false, SHRED);
}

CK_DLL_MFUN(b2Polygon_get_centroid)
{
    b2Polygon* polygon = ckobj_to_b2Polygon(SELF);
    RETURN->v_vec2     = { polygon->centroid.x, polygon->centroid.y };
}

CK_DLL_MFUN(b2Polygon_get_radius)
{
    RETURN->v_float = ckobj_to_b2Polygon(SELF)->radius;
}

// ============================================================================
// b2Shape
// ============================================================================

CK_DLL_SFUN(b2_CreateCircleShape)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance to next arg

    b2ShapeDef shape_def = b2DefaultShapeDef();
    ckobj_to_b2ShapeDef(API, &shape_def, GET_NEXT_OBJECT(ARGS));

    b2Circle circle = ckobj_to_b2Circle(GET_NEXT_OBJECT(ARGS));

    RETURN_B2_ID(b2ShapeId, b2CreateCircleShape(body_id, &shape_def, &circle));
}

CK_DLL_SFUN(b2_CreateSegmentShape)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance to next arg

    b2ShapeDef shape_def = b2DefaultShapeDef();
    ckobj_to_b2ShapeDef(API, &shape_def, GET_NEXT_OBJECT(ARGS));

    Chuck_Object* segment_obj = GET_NEXT_OBJECT(ARGS);
    b2Segment segment         = {};
    ckobj_to_b2Segment(API, &segment, segment_obj);

    RETURN_B2_ID(b2ShapeId, b2CreateSegmentShape(body_id, &shape_def, &segment));
}

CK_DLL_SFUN(b2_CreateCapsuleShape)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance to next arg

    b2ShapeDef shape_def = b2DefaultShapeDef();
    ckobj_to_b2ShapeDef(API, &shape_def, GET_NEXT_OBJECT(ARGS));

    Chuck_Object* capsule_obj = GET_NEXT_OBJECT(ARGS);
    b2Capsule capsule         = {};
    ckobj_to_b2Capsule(API, &capsule, capsule_obj);

    RETURN_B2_ID(b2ShapeId, b2CreateCapsuleShape(body_id, &shape_def, &capsule));
}

CK_DLL_SFUN(b2_CreatePolygonShape)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance to next arg

    b2ShapeDef shape_def = b2DefaultShapeDef();
    ckobj_to_b2ShapeDef(API, &shape_def, GET_NEXT_OBJECT(ARGS));

    b2Polygon* polygon = ckobj_to_b2Polygon(GET_NEXT_OBJECT(ARGS));
    // b2Polygon polygon = b2MakeBox(.5f, .5f);

    RETURN_B2_ID(b2ShapeId, b2CreatePolygonShape(body_id, &shape_def, polygon));
    // RETURN_B2_ID(b2ShapeId,
    //              b2CreatePolygonShape(body_id, &shape_def, &polygon));
}

CK_DLL_SFUN(b2_DestroyShape)
{
    ulib_box2d_accessAllowed;
    b2DestroyShape(GET_B2_ID(b2ShapeId, ARGS));
}

CK_DLL_SFUN(b2_Shape_IsValid)
{
    RETURN->v_int = b2Shape_IsValid(GET_B2_ID(b2ShapeId, ARGS));
}

CK_DLL_SFUN(b2_Shape_GetType)
{
    RETURN->v_int = b2Shape_GetType(GET_B2_ID(b2ShapeId, ARGS));
}

CK_DLL_SFUN(b2_Shape_GetBody)
{
    b2ShapeId shape_id = GET_B2_ID(b2ShapeId, ARGS);
    RETURN_B2_ID(b2BodyId, b2Shape_GetBody(shape_id));
}

CK_DLL_SFUN(b2_Shape_IsSensor)
{
    RETURN->v_int = b2Shape_IsSensor(GET_B2_ID(b2ShapeId, ARGS));
}

CK_DLL_SFUN(b2_Shape_SetDensity)
{
    ulib_box2d_accessAllowed;
    b2ShapeId shape_id = GET_B2_ID(b2ShapeId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    float density = GET_NEXT_FLOAT(ARGS);
    b2Shape_SetDensity(shape_id, density);
}

CK_DLL_SFUN(b2_Shape_GetDensity)
{
    RETURN->v_float = b2Shape_GetDensity(GET_B2_ID(b2ShapeId, ARGS));
}

CK_DLL_SFUN(b2_Shape_SetFriction)
{
    ulib_box2d_accessAllowed;
    b2ShapeId shape_id = GET_B2_ID(b2ShapeId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    float friction = GET_NEXT_FLOAT(ARGS);
    b2Shape_SetFriction(shape_id, friction);
}

CK_DLL_SFUN(b2_Shape_GetFriction)
{
    RETURN->v_float = b2Shape_GetFriction(GET_B2_ID(b2ShapeId, ARGS));
}

CK_DLL_SFUN(b2_Shape_SetRestitution)
{
    ulib_box2d_accessAllowed;
    b2ShapeId shape_id = GET_B2_ID(b2ShapeId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    float f = GET_NEXT_FLOAT(ARGS);
    b2Shape_SetRestitution(shape_id, f);
}

CK_DLL_SFUN(b2_Shape_GetRestitution)
{
    RETURN->v_float = b2Shape_GetRestitution(GET_B2_ID(b2ShapeId, ARGS));
}

CK_DLL_SFUN(b2_Shape_GetFilter)
{
    b2Filter filter            = b2Shape_GetFilter(GET_B2_ID(b2ShapeId, ARGS));
    Chuck_Object* filter_ckobj = chugin_createCkObj("b2Filter", false, SHRED);
    b2Filter_to_ckobj(API, filter_ckobj, &filter);
    RETURN->v_object = filter_ckobj;
}

CK_DLL_SFUN(b2_Shape_SetFilter)
{
    ulib_box2d_accessAllowed;
    b2ShapeId shape_id = GET_B2_ID(b2ShapeId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Filter filter = b2DefaultFilter();
    ckobj_to_b2Filter(API, &filter, GET_NEXT_OBJECT(ARGS));
    b2Shape_SetFilter(shape_id, filter);
}

CK_DLL_SFUN(b2_Shape_EnableSensorEvents)
{
    ulib_box2d_accessAllowed;
    b2ShapeId shape_id = GET_B2_ID(b2ShapeId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    int flag = GET_NEXT_INT(ARGS);
    b2Shape_EnableSensorEvents(shape_id, flag);
}

CK_DLL_SFUN(b2_Shape_AreSensorEventsEnabled)
{
    RETURN->v_int = b2Shape_AreSensorEventsEnabled(GET_B2_ID(b2ShapeId, ARGS));
}

CK_DLL_SFUN(b2_Shape_EnableContactEvents)
{
    ulib_box2d_accessAllowed;
    b2ShapeId shape_id = GET_B2_ID(b2ShapeId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    int flag = GET_NEXT_INT(ARGS);
    b2Shape_EnableContactEvents(shape_id, flag);
}

CK_DLL_SFUN(b2_Shape_AreContactEventsEnabled)
{
    RETURN->v_int = b2Shape_AreContactEventsEnabled(GET_B2_ID(b2ShapeId, ARGS));
}

CK_DLL_SFUN(b2_Shape_EnablePreSolveEvents)
{
    ulib_box2d_accessAllowed;
    b2ShapeId shape_id = GET_B2_ID(b2ShapeId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    int flag = GET_NEXT_INT(ARGS);
    b2Shape_EnablePreSolveEvents(shape_id, flag);
}

CK_DLL_SFUN(b2_Shape_ArePreSolveEventsEnabled)
{
    RETURN->v_int = b2Shape_ArePreSolveEventsEnabled(GET_B2_ID(b2ShapeId, ARGS));
}

CK_DLL_SFUN(b2_Shape_EnableHitEvents)
{
    ulib_box2d_accessAllowed;
    b2ShapeId shape_id = GET_B2_ID(b2ShapeId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    int flag = GET_NEXT_INT(ARGS);
    b2Shape_EnableHitEvents(shape_id, flag);
}

CK_DLL_SFUN(b2_Shape_AreHitEventsEnabled)
{
    RETURN->v_int = b2Shape_AreHitEventsEnabled(GET_B2_ID(b2ShapeId, ARGS));
}

CK_DLL_SFUN(b2_Shape_TestPoint)
{
    ulib_box2d_accessAllowed;
    b2ShapeId shape_id = GET_B2_ID(b2ShapeId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    t_CKVEC2 point = GET_NEXT_VEC2(ARGS);
    RETURN->v_int  = b2Shape_TestPoint(shape_id, { (float)point.x, (float)point.y });
}

CK_DLL_SFUN(b2_Shape_RayCast)
{
    ulib_box2d_accessAllowed;
    b2ShapeId shape_id = GET_B2_ID(b2ShapeId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    t_CKVEC2 origin          = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 translation     = GET_NEXT_VEC2(ARGS);
    Chuck_Object* output_obj = GET_NEXT_OBJECT(ARGS);
    b2CastOutput output
      = b2Shape_RayCast(shape_id, { (float)origin.x, (float)origin.y },
                        { (float)translation.x, (float)translation.y });
    b2CastOutput_to_ckobj(API, output_obj, &output);
    RETURN->v_object = output_obj;
}

CK_DLL_SFUN(b2_Shape_GetCircle)
{
    b2Circle circle          = b2Shape_GetCircle(GET_B2_ID(b2ShapeId, ARGS));
    Chuck_Object* circle_obj = chugin_createCkObj("b2Circle", false, SHRED);
    b2Circle_to_ckobj(API, circle_obj, &circle);
    RETURN->v_object = circle_obj;
}

CK_DLL_SFUN(b2_Shape_GetSegment)
{
    b2Segment segment         = b2Shape_GetSegment(GET_B2_ID(b2ShapeId, ARGS));
    Chuck_Object* segment_obj = chugin_createCkObj("b2Segment", false, SHRED);
    b2Segment_to_ckobj(API, segment_obj, &segment);
    RETURN->v_object = segment_obj;
}

// CK_DLL_SFUN(b2_Shape_GetSmoothSegment)
// {
//     b2SmoothSegment smooth_segment
//       = b2Shape_GetSmoothSegment(GET_B2_ID(b2ShapeId, ARGS));
//     Chuck_Object* smooth_segment_obj
//       = chugin_createCkObj("b2_SmoothSegment", false, SHRED);
//     b2SmoothSegment_to_ckobj(API, smooth_segment_obj, &smooth_segment);
//     RETURN->v_object = smooth_segment_obj;
// }

CK_DLL_SFUN(b2_Shape_GetCapsule)
{
    b2Capsule capsule         = b2Shape_GetCapsule(GET_B2_ID(b2ShapeId, ARGS));
    Chuck_Object* capsule_obj = chugin_createCkObj("b2Capsule", false, SHRED);
    b2Capsule_to_ckobj(API, capsule_obj, &capsule);
    RETURN->v_object = capsule_obj;
}

CK_DLL_SFUN(b2_Shape_GetPolygon)
{
    b2Polygon polygon = b2Shape_GetPolygon(GET_B2_ID(b2ShapeId, ARGS));
    RETURN->v_object  = b2Polygon_create(SHRED, &polygon);
}

CK_DLL_SFUN(b2_Shape_SetCircle)
{
    ulib_box2d_accessAllowed;
    b2ShapeId shape_id = GET_B2_ID(b2ShapeId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Circle circle = ckobj_to_b2Circle(GET_NEXT_OBJECT(ARGS));
    b2Shape_SetCircle(shape_id, &circle);
}

CK_DLL_SFUN(b2_Shape_SetCapsule)
{
    ulib_box2d_accessAllowed;
    b2ShapeId shape_id = GET_B2_ID(b2ShapeId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Capsule capsule = {};
    ckobj_to_b2Capsule(API, &capsule, GET_NEXT_OBJECT(ARGS));
    b2Shape_SetCapsule(shape_id, &capsule);
}

CK_DLL_SFUN(b2_Shape_SetSegment)
{
    ulib_box2d_accessAllowed;
    b2ShapeId shape_id = GET_B2_ID(b2ShapeId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Segment segment = {};
    ckobj_to_b2Segment(API, &segment, GET_NEXT_OBJECT(ARGS));
    b2Shape_SetSegment(shape_id, &segment);
}

CK_DLL_SFUN(b2_Shape_SetPolygon)
{
    ulib_box2d_accessAllowed;
    b2ShapeId shape_id = GET_B2_ID(b2ShapeId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Polygon* polygon = ckobj_to_b2Polygon(GET_NEXT_OBJECT(ARGS));
    b2Shape_SetPolygon(shape_id, polygon);
}

// CK_DLL_SFUN(b2_Shape_GetParentChain)
// {
//     RETURN_B2_ID(b2ChainId, b2Shape_GetParentChain(GET_B2_ID(b2ShapeId, ARGS)));
// }

// CK_DLL_SFUN(b2_Shape_GetContactCapacity)
// {
//     RETURN->v_int = b2Shape_GetContactCapacity(GET_B2_ID(b2ShapeId, ARGS));
// }

// CK_DLL_SFUN(b2_Shape_GetContactData)
// {
//     b2ShapeId shape_id = GET_B2_ID(b2ShapeId, ARGS);
//     GET_NEXT_INT(ARGS); // advance
//     Chuck_ArrayInt* contacts = GET_NEXT_OBJECT_ARRAY(ARGS);

//     // allocate mem in frame arena for contacts
//     int contact_capacity = b2Shape_GetContactCapacity(shape_id);
//     b2ContactData* contact_data
//       = ARENA_PUSH_COUNT(&audio_frame_arena, b2ContactData,
//       contact_capacity);

//     b2Shape_GetContactData(shape_id, contact_data, contact_capacity);

//     // copy contact data back to chuck array
//     // TODO
// }

CK_DLL_SFUN(b2_Shape_GetAABB)
{
    ulib_box2d_accessAllowed;
    b2ShapeId shape_id = GET_B2_ID(b2ShapeId, ARGS);
    b2AABB aabb        = b2Shape_GetAABB(shape_id);
    RETURN->v_vec4
      = { aabb.lowerBound.x, aabb.lowerBound.y, aabb.upperBound.x, aabb.upperBound.y };
}

CK_DLL_SFUN(b2_Shape_GetClosestPoint)
{
    ulib_box2d_accessAllowed;
    b2ShapeId shape_id = GET_B2_ID(b2ShapeId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    t_CKVEC2 target = GET_NEXT_VEC2(ARGS);
    b2Vec2 closest_point
      = b2Shape_GetClosestPoint(shape_id, { (float)target.x, (float)target.y });
    RETURN->v_vec2 = { closest_point.x, closest_point.y };
}

// ============================================================================
// b2BodyDef
// ============================================================================

static void b2BodyDef_to_ckobj(CK_DL_API API, Chuck_Object* ckobj, b2BodyDef* obj)
{
    OBJ_MEMBER_INT(ckobj, b2BodyDef_type_offset) = obj->type;
    OBJ_MEMBER_VEC2(ckobj, b2BodyDef_position_offset)
      = { obj->position.x, obj->position.y };
    OBJ_MEMBER_VEC2(ckobj, b2BodyDef_rotation_offset)
      = { obj->rotation.c, obj->rotation.s };
    OBJ_MEMBER_VEC2(ckobj, b2BodyDef_linearVelocity_offset)
      = { obj->linearVelocity.x, obj->linearVelocity.y };
    OBJ_MEMBER_FLOAT(ckobj, b2BodyDef_angularVelocity_offset) = obj->angularVelocity;
    OBJ_MEMBER_FLOAT(ckobj, b2BodyDef_linearDamping_offset)   = obj->linearDamping;
    OBJ_MEMBER_FLOAT(ckobj, b2BodyDef_angularDamping_offset)  = obj->angularDamping;
    OBJ_MEMBER_FLOAT(ckobj, b2BodyDef_gravityScale_offset)    = obj->gravityScale;
    OBJ_MEMBER_FLOAT(ckobj, b2BodyDef_sleepThreshold_offset)  = obj->sleepThreshold;
    OBJ_MEMBER_INT(ckobj, b2BodyDef_enableSleep_offset)       = obj->enableSleep;
    OBJ_MEMBER_INT(ckobj, b2BodyDef_isAwake_offset)           = obj->isAwake;
    OBJ_MEMBER_INT(ckobj, b2BodyDef_fixedRotation_offset)     = obj->fixedRotation;
    OBJ_MEMBER_INT(ckobj, b2BodyDef_isBullet_offset)          = obj->isBullet;
    OBJ_MEMBER_INT(ckobj, b2BodyDef_isEnabled_offset)         = obj->isEnabled;
    OBJ_MEMBER_INT(ckobj, b2BodyDef_automaticMass_offset)     = obj->automaticMass;
    OBJ_MEMBER_INT(ckobj, b2BodyDef_allowFastRotation_offset) = obj->allowFastRotation;
}

static void ckobj_to_b2BodyDef(CK_DL_API API, b2BodyDef* obj, Chuck_Object* ckobj)
{
    obj->type              = (b2BodyType)OBJ_MEMBER_INT(ckobj, b2BodyDef_type_offset);
    t_CKVEC2 position_vec2 = OBJ_MEMBER_VEC2(ckobj, b2BodyDef_position_offset);
    obj->position          = { (float)position_vec2.x, (float)position_vec2.y };
    t_CKVEC2 rotation_vec2 = OBJ_MEMBER_VEC2(ckobj, b2BodyDef_rotation_offset);
    obj->rotation          = { (float)rotation_vec2.x, (float)rotation_vec2.y };
    t_CKVEC2 linearVelocity_vec2
      = OBJ_MEMBER_VEC2(ckobj, b2BodyDef_linearVelocity_offset);
    obj->linearVelocity
      = { (float)linearVelocity_vec2.x, (float)linearVelocity_vec2.y };
    obj->angularVelocity
      = (float)OBJ_MEMBER_FLOAT(ckobj, b2BodyDef_angularVelocity_offset);
    obj->linearDamping = (float)OBJ_MEMBER_FLOAT(ckobj, b2BodyDef_linearDamping_offset);
    obj->angularDamping
      = (float)OBJ_MEMBER_FLOAT(ckobj, b2BodyDef_angularDamping_offset);
    obj->gravityScale = (float)OBJ_MEMBER_FLOAT(ckobj, b2BodyDef_gravityScale_offset);
    obj->sleepThreshold
      = (float)OBJ_MEMBER_FLOAT(ckobj, b2BodyDef_sleepThreshold_offset);
    obj->enableSleep       = OBJ_MEMBER_INT(ckobj, b2BodyDef_enableSleep_offset);
    obj->isAwake           = OBJ_MEMBER_INT(ckobj, b2BodyDef_isAwake_offset);
    obj->fixedRotation     = OBJ_MEMBER_INT(ckobj, b2BodyDef_fixedRotation_offset);
    obj->isBullet          = OBJ_MEMBER_INT(ckobj, b2BodyDef_isBullet_offset);
    obj->isEnabled         = OBJ_MEMBER_INT(ckobj, b2BodyDef_isEnabled_offset);
    obj->automaticMass     = OBJ_MEMBER_INT(ckobj, b2BodyDef_automaticMass_offset);
    obj->allowFastRotation = OBJ_MEMBER_INT(ckobj, b2BodyDef_allowFastRotation_offset);
}

CK_DLL_CTOR(b2BodyDef_ctor)
{
    b2BodyDef default_body_def = b2DefaultBodyDef();
    b2BodyDef_to_ckobj(API, SELF, &default_body_def);
}

// ============================================================================
// b2Body
// ============================================================================

CK_DLL_SFUN(b2_Body_is_valid)
{
    RETURN->v_int = b2Body_IsValid(GET_B2_ID(b2BodyId, ARGS));
}

CK_DLL_SFUN(b2_Body_get_type)
{
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    RETURN->v_int = b2Body_GetType(body_id);
}

CK_DLL_SFUN(b2_Body_set_type)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Body_SetType(body_id, ckint_to_b2BodyType(GET_NEXT_INT(ARGS)));
}

CK_DLL_SFUN(b2_Body_get_position)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Vec2 pos     = b2Body_GetPosition(body_id);
    RETURN->v_vec2 = { pos.x, pos.y };
}

CK_DLL_SFUN(b2_Body_get_rotation)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Rot rot         = b2Body_GetRotation(body_id);
    RETURN->v_complex = { rot.c, rot.s };
}

CK_DLL_SFUN(b2_Body_get_angle)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    RETURN->v_float = b2Rot_GetAngle(b2Body_GetRotation(body_id));
}

CK_DLL_SFUN(b2_Body_set_transform)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    t_CKVEC2 pos = GET_NEXT_VEC2(ARGS);
    float angle  = GET_NEXT_FLOAT(ARGS);
    b2Body_SetTransform(body_id, { (float)pos.x, (float)pos.y }, b2MakeRot(angle));
}

CK_DLL_SFUN(b2_Body_set_position)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2BodyId, body_id);
    t_CKVEC2 pos = GET_NEXT_VEC2(ARGS);
    b2Body_SetTransform(body_id, { (float)pos.x, (float)pos.y },
                        b2Body_GetTransform(body_id).q);
}

CK_DLL_SFUN(b2_Body_set_angle)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2BodyId, body_id);
    b2Body_SetTransform(body_id, b2Body_GetTransform(body_id).p,
                        b2MakeRot(GET_NEXT_FLOAT(ARGS)));
}

CK_DLL_SFUN(b2_Body_get_local_point)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    t_CKVEC2 world_point = GET_NEXT_VEC2(ARGS);
    b2Vec2 local_point
      = b2Body_GetLocalPoint(body_id, { (float)world_point.x, (float)world_point.y });
    RETURN->v_vec2 = { local_point.x, local_point.y };
}

CK_DLL_SFUN(b2_Body_get_world_point)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    t_CKVEC2 local_point = GET_NEXT_VEC2(ARGS);
    b2Vec2 world_point
      = b2Body_GetWorldPoint(body_id, { (float)local_point.x, (float)local_point.y });
    RETURN->v_vec2 = { world_point.x, world_point.y };
}

CK_DLL_SFUN(b2_Body_get_local_vector)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    t_CKVEC2 world_vector = GET_NEXT_VEC2(ARGS);
    b2Vec2 local_vector   = b2Body_GetLocalVector(
      body_id, { (float)world_vector.x, (float)world_vector.y });
    RETURN->v_vec2 = { local_vector.x, local_vector.y };
}

CK_DLL_SFUN(b2_Body_get_world_vector)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    t_CKVEC2 local_vector = GET_NEXT_VEC2(ARGS);
    b2Vec2 world_vector   = b2Body_GetWorldVector(
      body_id, { (float)local_vector.x, (float)local_vector.y });
    RETURN->v_vec2 = { world_vector.x, world_vector.y };
}

CK_DLL_SFUN(b2_Body_get_linear_velocity)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Vec2 vel     = b2Body_GetLinearVelocity(body_id);
    RETURN->v_vec2 = { vel.x, vel.y };
}

CK_DLL_SFUN(b2_Body_set_linear_velocity)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    t_CKVEC2 vel = GET_NEXT_VEC2(ARGS);
    b2Body_SetLinearVelocity(body_id, { (float)vel.x, (float)vel.y });
}

CK_DLL_SFUN(b2_Body_get_angular_velocity)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    RETURN->v_float = b2Body_GetAngularVelocity(body_id);
}

CK_DLL_SFUN(b2_Body_set_angular_velocity)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Body_SetAngularVelocity(body_id, GET_NEXT_FLOAT(ARGS));
}

CK_DLL_SFUN(b2_Body_apply_force)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    t_CKVEC2 force = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 point = GET_NEXT_VEC2(ARGS);
    t_CKINT wake   = GET_NEXT_INT(ARGS);
    b2Body_ApplyForce(body_id, { (float)force.x, (float)force.y },
                      { (float)point.x, (float)point.y }, wake);
}

CK_DLL_SFUN(b2_Body_apply_force_to_center)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    t_CKVEC2 force = GET_NEXT_VEC2(ARGS);
    t_CKINT wake   = GET_NEXT_INT(ARGS);
    b2Body_ApplyForceToCenter(body_id, { (float)force.x, (float)force.y }, wake);
}

CK_DLL_SFUN(b2_Body_apply_torque)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    t_CKFLOAT torque = GET_NEXT_FLOAT(ARGS);
    t_CKINT wake     = GET_NEXT_INT(ARGS);
    b2Body_ApplyTorque(body_id, torque, wake);
}

CK_DLL_SFUN(b2_Body_apply_linear_impulse)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    t_CKVEC2 impulse = GET_NEXT_VEC2(ARGS);
    t_CKVEC2 point   = GET_NEXT_VEC2(ARGS);
    t_CKINT wake     = GET_NEXT_INT(ARGS);
    b2Body_ApplyLinearImpulse(body_id, { (float)impulse.x, (float)impulse.y },
                              { (float)point.x, (float)point.y }, wake);
}

CK_DLL_SFUN(b2_Body_apply_linear_impulse_to_center)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    t_CKVEC2 impulse = GET_NEXT_VEC2(ARGS);
    t_CKINT wake     = GET_NEXT_INT(ARGS);
    b2Body_ApplyLinearImpulseToCenter(body_id, { (float)impulse.x, (float)impulse.y },
                                      wake);
}

CK_DLL_SFUN(b2_Body_apply_angular_impulse)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    t_CKFLOAT impulse = GET_NEXT_FLOAT(ARGS);
    t_CKINT wake      = GET_NEXT_INT(ARGS);
    b2Body_ApplyAngularImpulse(body_id, impulse, wake);
}

CK_DLL_SFUN(b2_Body_get_mass)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    RETURN->v_float = b2Body_GetMass(body_id);
}

CK_DLL_SFUN(b2_Body_get_inertia)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    RETURN->v_float = b2Body_GetInertiaTensor(body_id);
}

CK_DLL_SFUN(b2_Body_get_local_center_of_mass)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Vec2 center  = b2Body_GetLocalCenterOfMass(body_id);
    RETURN->v_vec2 = { center.x, center.y };
}

CK_DLL_SFUN(b2_Body_get_world_center_of_mass)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Vec2 center  = b2Body_GetWorldCenterOfMass(body_id);
    RETURN->v_vec2 = { center.x, center.y };
}

CK_DLL_SFUN(b2_Body_get_mass_data)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2BodyId, body_id);
    b2MassData data         = b2Body_GetMassData(body_id);
    Chuck_Object* mass_data = chugin_createCkObj("b2MassData", false, SHRED);
    b2MassData_to_ckobj(mass_data, &data);
    RETURN->v_object = mass_data;
}

CK_DLL_SFUN(b2_Body_set_mass_data)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2BodyId, body_id);
    b2MassData mass_data = {};
    ckobj_to_b2MassData(&mass_data, GET_NEXT_OBJECT(ARGS));
    b2Body_SetMassData(body_id, mass_data);
}

CK_DLL_SFUN(b2_Body_apply_mass_from_shapes)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Body_ApplyMassFromShapes(body_id);
}

CK_DLL_SFUN(b2_Body_set_automatic_mass)
{
    ulib_box2d_accessAllowed;
    GET_NEXT_B2_ID(b2BodyId, body_id);
    b2Body_SetAutomaticMass(body_id, GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(b2_Body_get_automatic_mass)
{
    RETURN->v_int = b2Body_GetAutomaticMass(GET_B2_ID(b2BodyId, ARGS));
}

CK_DLL_SFUN(b2_Body_set_linear_damping)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Body_SetLinearDamping(body_id, GET_NEXT_FLOAT(ARGS));
}

CK_DLL_SFUN(b2_Body_get_linear_damping)
{
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    RETURN->v_float = b2Body_GetLinearDamping(body_id);
}

CK_DLL_SFUN(b2_Body_set_angular_damping)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Body_SetAngularDamping(body_id, GET_NEXT_FLOAT(ARGS));
}

CK_DLL_SFUN(b2_Body_get_angular_damping)
{
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    RETURN->v_float = b2Body_GetAngularDamping(body_id);
}

CK_DLL_SFUN(b2_Body_set_gravity_scale)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Body_SetGravityScale(body_id, GET_NEXT_FLOAT(ARGS));
}

CK_DLL_SFUN(b2_Body_get_gravity_scale)
{
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    RETURN->v_float = b2Body_GetGravityScale(body_id);
}

CK_DLL_SFUN(b2_Body_is_awake)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    RETURN->v_int = b2Body_IsAwake(body_id);
}

CK_DLL_SFUN(b2_Body_set_awake)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Body_SetAwake(body_id, GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(b2_Body_enable_sleep)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Body_EnableSleep(body_id, GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(b2_Body_is_sleep_enabled)
{
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    RETURN->v_int = b2Body_IsSleepEnabled(body_id);
}

CK_DLL_SFUN(b2_Body_set_sleep_threshold)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Body_SetSleepThreshold(body_id, GET_NEXT_FLOAT(ARGS));
}

CK_DLL_SFUN(b2_Body_get_sleep_threshold)
{
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    RETURN->v_float = b2Body_GetSleepThreshold(body_id);
}

CK_DLL_SFUN(b2_Body_is_enabled)
{
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    RETURN->v_int = b2Body_IsEnabled(body_id);
}

CK_DLL_SFUN(b2_Body_disable)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Body_Disable(body_id);
}

CK_DLL_SFUN(b2_Body_enable)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Body_Enable(body_id);
}

CK_DLL_SFUN(b2_Body_set_fixed_rotation)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Body_SetFixedRotation(body_id, GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(b2_Body_is_fixed_rotation)
{
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    RETURN->v_int = b2Body_IsFixedRotation(body_id);
}

CK_DLL_SFUN(b2_Body_set_bullet)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Body_SetBullet(body_id, GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(b2_Body_is_bullet)
{
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    RETURN->v_int = b2Body_IsBullet(body_id);
}

CK_DLL_SFUN(b2_Body_enable_hit_events)
{
    ulib_box2d_accessAllowed;
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2Body_EnableHitEvents(body_id, GET_NEXT_INT(ARGS));
}

CK_DLL_SFUN(b2_Body_get_shape_count)
{
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    RETURN->v_int = b2Body_GetShapeCount(body_id);
}

CK_DLL_SFUN(b2_Body_get_shapes)
{
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    Chuck_ArrayInt* ck_shape_array = GET_NEXT_OBJECT_ARRAY(ARGS);
    API->object->array_int_clear(ck_shape_array);

    // get b2ShapeIds
    int shape_count = b2Body_GetShapeCount(body_id);
    b2ShapeId* shape_id_array
      = ARENA_PUSH_COUNT(&audio_frame_arena, b2ShapeId, shape_count);
    b2Body_GetShapes(body_id, shape_id_array, shape_count);

    // copy into array
    for (int i = 0; i < shape_count; i++) {
        t_CKINT shape_id = *(t_CKINT*)(shape_id_array + i);
        API->object->array_int_push_back(ck_shape_array, shape_id);
    }
}

// CK_DLL_SFUN(b2_Body_get_joint_count)
// {
//     b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
//     GET_NEXT_INT(ARGS); // advance
//     RETURN->v_int = b2Body_GetJointCount(body_id);
// }

// CK_DLL_SFUN(b2_Body_get_joints)
// {
//     b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
//     GET_NEXT_INT(ARGS); // advance
//     // TODO
// }

// CK_DLL_SFUN(b2_Body_get_contact_capacity)
// {
//     b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
//     GET_NEXT_INT(ARGS); // advance
//     RETURN->v_int = b2Body_GetContactCapacity(body_id);
// }

// CK_DLL_SFUN(b2_Body_get_contact_data)
// {
//     b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
//     GET_NEXT_INT(ARGS); // advance
//     // TODO
// }

CK_DLL_SFUN(b2_Body_compute_aabb)
{
    b2BodyId body_id = GET_B2_ID(b2BodyId, ARGS);
    GET_NEXT_INT(ARGS); // advance
    b2AABB box = b2Body_ComputeAABB(body_id);
    RETURN->v_vec4
      = { box.lowerBound.x, box.lowerBound.y, box.upperBound.x, box.upperBound.y };
}

// ============================================================================
// b2Circle
// ============================================================================

static void b2Circle_to_ckobj(CK_DL_API API, Chuck_Object* ckobj, b2Circle* obj)
{
    OBJ_MEMBER_VEC2(ckobj, b2Circle_position_offset) = { obj->center.x, obj->center.y };
    OBJ_MEMBER_FLOAT(ckobj, b2Circle_radius_offset)  = obj->radius;
}

static b2Circle ckobj_to_b2Circle(Chuck_Object* ckobj)
{
    CK_DL_API API = g_chuglAPI;
    return { vec2_to_b2Vec2(OBJ_MEMBER_VEC2(ckobj, b2Circle_position_offset)),
             (f32)OBJ_MEMBER_FLOAT(ckobj, b2Circle_radius_offset) };
}

CK_DLL_CTOR(b2Circle_ctor)
{
    OBJ_MEMBER_VEC2(SELF, b2Circle_position_offset) = GET_NEXT_VEC2(ARGS);
    OBJ_MEMBER_FLOAT(SELF, b2Circle_radius_offset)  = GET_NEXT_FLOAT(ARGS);
}

CK_DLL_CTOR(b2Circle_ctor_centered)
{
    OBJ_MEMBER_VEC2(SELF, b2Circle_position_offset) = { 0, 0 };
    OBJ_MEMBER_FLOAT(SELF, b2Circle_radius_offset)  = GET_NEXT_FLOAT(ARGS);
}

// ============================================================================
// b2Capsule
// ============================================================================

static void b2Capsule_to_ckobj(CK_DL_API API, Chuck_Object* ckobj, b2Capsule* obj)
{
    OBJ_MEMBER_VEC2(ckobj, b2Capsule_center1_offset)
      = { obj->center1.x, obj->center1.y };
    OBJ_MEMBER_VEC2(ckobj, b2Capsule_center2_offset)
      = { obj->center2.x, obj->center2.y };
    OBJ_MEMBER_FLOAT(ckobj, b2Capsule_radius_offset) = obj->radius;
}

static void ckobj_to_b2Capsule(CK_DL_API API, b2Capsule* obj, Chuck_Object* ckobj)
{
    t_CKVEC2 center1_vec2 = OBJ_MEMBER_VEC2(ckobj, b2Capsule_center1_offset);
    obj->center1          = { (float)center1_vec2.x, (float)center1_vec2.y };
    t_CKVEC2 center2_vec2 = OBJ_MEMBER_VEC2(ckobj, b2Capsule_center2_offset);
    obj->center2          = { (float)center2_vec2.x, (float)center2_vec2.y };
    obj->radius           = (float)OBJ_MEMBER_FLOAT(ckobj, b2Capsule_radius_offset);
}

CK_DLL_CTOR(b2Capsule_ctor)
{
    OBJ_MEMBER_VEC2(SELF, b2Capsule_center1_offset) = GET_NEXT_VEC2(ARGS);
    OBJ_MEMBER_VEC2(SELF, b2Capsule_center2_offset) = GET_NEXT_VEC2(ARGS);
    OBJ_MEMBER_FLOAT(SELF, b2Capsule_radius_offset) = GET_NEXT_FLOAT(ARGS);
}

// ============================================================================
// b2Segment
// ============================================================================

static void b2Segment_to_ckobj(CK_DL_API API, Chuck_Object* ckobj, b2Segment* obj)
{
    OBJ_MEMBER_VEC2(ckobj, b2Segment_point1_offset) = { obj->point1.x, obj->point1.y };
    OBJ_MEMBER_VEC2(ckobj, b2Segment_point2_offset) = { obj->point2.x, obj->point2.y };
}

static void ckobj_to_b2Segment(CK_DL_API API, b2Segment* obj, Chuck_Object* ckobj)
{
    t_CKVEC2 point1_vec2 = OBJ_MEMBER_VEC2(ckobj, b2Segment_point1_offset);
    obj->point1          = { (float)point1_vec2.x, (float)point1_vec2.y };
    t_CKVEC2 point2_vec2 = OBJ_MEMBER_VEC2(ckobj, b2Segment_point2_offset);
    obj->point2          = { (float)point2_vec2.x, (float)point2_vec2.y };
}

CK_DLL_CTOR(b2Segment_ctor)
{
    OBJ_MEMBER_VEC2(SELF, b2Segment_point1_offset) = GET_NEXT_VEC2(ARGS);
    OBJ_MEMBER_VEC2(SELF, b2Segment_point2_offset) = GET_NEXT_VEC2(ARGS);
}

// ============================================================================
// b2CastOutput
// ============================================================================

static void b2CastOutput_to_ckobj(CK_DL_API API, Chuck_Object* ckobj, b2CastOutput* obj)
{
    OBJ_MEMBER_VEC2(ckobj, b2CastOutput_normal_offset)
      = { obj->normal.x, obj->normal.y };
    OBJ_MEMBER_VEC2(ckobj, b2CastOutput_point_offset) = { obj->point.x, obj->point.y };
    OBJ_MEMBER_FLOAT(ckobj, b2CastOutput_fraction_offset) = obj->fraction;
    OBJ_MEMBER_INT(ckobj, b2CastOutput_iterations_offset) = obj->iterations;
    OBJ_MEMBER_INT(ckobj, b2CastOutput_hit_offset)        = obj->hit;
}

// static void ckobj_to_b2CastOutput(CK_DL_API API, b2CastOutput* obj,
// Chuck_Object* ckobj)
// {
// t_CKVEC2 normal_vec2 = OBJ_MEMBER_VEC2(ckobj, b2CastOutput_normal_offset);
// obj->normal = { (float) normal_vec2.x, (float) normal_vec2.y };
// t_CKVEC2 point_vec2 = OBJ_MEMBER_VEC2(ckobj, b2CastOutput_point_offset);
// obj->point = { (float) point_vec2.x, (float) point_vec2.y };
// obj->fraction = (float) OBJ_MEMBER_FLOAT(ckobj,
// b2CastOutput_fraction_offset); obj->iterations = OBJ_MEMBER_INT(ckobj,
// b2CastOutput_iterations_offset); obj->hit = OBJ_MEMBER_INT(ckobj,
// b2CastOutput_hit_offset);
// }

// ============================================================================
// b2DebugDraw
// ============================================================================
static t_CKVEC3 b2_HexColorToVec3(b2HexColor c)
{
    return {
        ((c >> 16) & 0xFF) / 255.0f, //
        ((c >> 8) & 0xFF) / 255.0f,  //
        (c & 0xFF) / 255.0f,         //
    };
}

static void b2_DebugDrawPolygonCallback(const b2Vec2* vertices, int vertexCount,
                                        b2HexColor color, void* context)
{
    Chuck_Object* ckobj          = (Chuck_Object*)context;
    Chuck_VM_Shred* origin_shred = chugin_getOriginShred(ckobj);

    Chuck_DL_Arg args[2];
    // ARG("vec2[]", "vertices");
    args[0].kind           = kindof_INT;
    args[0].value.v_object = (Chuck_Object*)chugin_createCkFloat2Array(
      (glm::vec2*)vertices, vertexCount, true);
    // ARG("vec3", "color");
    args[1].kind         = kindof_VEC3;
    args[1].value.v_vec3 = b2_HexColorToVec3(color);

    g_chuglAPI->vm->invoke_mfun_immediate_mode(
      ckobj, b2_DebugDraw_DrawPolygon_callback_offset, g_chuglVM, origin_shred, args,
      ARRAY_LENGTH(args));
}

static void b2_DebugDrawSolidPolygonCallback(b2Transform transform,
                                             const b2Vec2* vertices, int vertexCount,
                                             float radius, b2HexColor color,
                                             void* context)
{
    Chuck_Object* ckobj          = (Chuck_Object*)context;
    Chuck_VM_Shred* origin_shred = chugin_getOriginShred(ckobj);

    Chuck_DL_Arg args[5];
    // ARG("vec2", "position");
    args[0].kind         = kindof_VEC2;
    args[0].value.v_vec2 = { transform.p.x, transform.p.y };
    // ARG("float", "rotation_radians");
    args[1].kind          = kindof_FLOAT;
    args[1].value.v_float = b2Rot_GetAngle(transform.q);
    // ARG("vec2[]", "vertices");
    args[2].kind           = kindof_INT;
    args[2].value.v_object = (Chuck_Object*)chugin_createCkFloat2Array(
      (glm::vec2*)vertices, vertexCount, true);
    // ARG("float", "radius");
    args[3].kind          = kindof_FLOAT;
    args[3].value.v_float = radius;
    // ARG("vec3", "color");
    args[4].kind         = kindof_VEC3;
    args[4].value.v_vec3 = b2_HexColorToVec3(color);

    g_chuglAPI->vm->invoke_mfun_immediate_mode(
      ckobj, b2_DebugDraw_DrawSolidPolygon_callback_offset, g_chuglVM, origin_shred,
      args, ARRAY_LENGTH(args));
}

static void b2_DebugDrawCircleCallback(b2Vec2 center, float radius, b2HexColor color,
                                       void* context)
{
    Chuck_Object* ckobj          = (Chuck_Object*)context;
    Chuck_VM_Shred* origin_shred = chugin_getOriginShred(ckobj);

    Chuck_DL_Arg args[3];

    // ARG("vec2", "center");
    args[0].kind         = kindof_VEC2;
    args[0].value.v_vec2 = { center.x, center.y };
    // ARG("float", "radius");
    args[1].kind          = kindof_FLOAT;
    args[1].value.v_float = radius;
    // ARG("vec3", "color");
    args[2].kind         = kindof_VEC3;
    args[2].value.v_vec3 = b2_HexColorToVec3(color);

    g_chuglAPI->vm->invoke_mfun_immediate_mode(
      ckobj, b2_DebugDraw_DrawCircle_callback_offset, g_chuglVM, origin_shred, args,
      ARRAY_LENGTH(args));
}

static void b2_DebugDrawSolidCircleCallback(b2Transform transform, float radius,
                                            b2HexColor color, void* context)
{
    Chuck_Object* ckobj          = (Chuck_Object*)context;
    Chuck_VM_Shred* origin_shred = chugin_getOriginShred(ckobj);

    Chuck_DL_Arg args[4];

    // ARG("vec2", "center");
    args[0].kind         = kindof_VEC2;
    args[0].value.v_vec2 = { transform.p.x, transform.p.y };
    // ARG("float", "rotation_radians");
    args[1].kind          = kindof_FLOAT;
    args[1].value.v_float = b2Rot_GetAngle(transform.q);
    // ARG("float", "radius");
    args[2].kind          = kindof_FLOAT;
    args[2].value.v_float = radius;
    // ARG("vec3", "color");
    args[3].kind         = kindof_VEC3;
    args[3].value.v_vec3 = b2_HexColorToVec3(color);

    g_chuglAPI->vm->invoke_mfun_immediate_mode(
      ckobj, b2_DebugDraw_DrawSolidCircle_callback_offset, g_chuglVM, origin_shred,
      args, ARRAY_LENGTH(args));
}

static void b2_DebugDrawSolidCapsuleCallback(b2Vec2 p1, b2Vec2 p2, float radius,
                                             b2HexColor color, void* context)
{
    Chuck_Object* ckobj          = (Chuck_Object*)context;
    Chuck_VM_Shred* origin_shred = chugin_getOriginShred(ckobj);

    Chuck_DL_Arg args[4];

    // ARG("vec2", "p1");
    args[0].kind         = kindof_VEC2;
    args[0].value.v_vec2 = { p1.x, p1.y };
    // ARG("vec2", "p2");
    args[1].kind         = kindof_VEC2;
    args[1].value.v_vec2 = { p2.x, p2.y };
    // ARG("float", "radius");
    args[2].kind          = kindof_FLOAT;
    args[2].value.v_float = radius;
    // ARG("vec3", "color");
    args[3].kind         = kindof_VEC3;
    args[3].value.v_vec3 = b2_HexColorToVec3(color);

    g_chuglAPI->vm->invoke_mfun_immediate_mode(
      ckobj, b2_DebugDraw_DrawSolidCapsule_callback_offset, g_chuglVM, origin_shred,
      args, ARRAY_LENGTH(args));
}

static void b2_DebugDrawSegmentCallback(b2Vec2 p1, b2Vec2 p2, b2HexColor color,
                                        void* context)
{
    Chuck_Object* ckobj          = (Chuck_Object*)context;
    Chuck_VM_Shred* origin_shred = chugin_getOriginShred(ckobj);

    Chuck_DL_Arg args[3];

    // ARG("vec2", "p1");
    args[0].kind         = kindof_VEC2;
    args[0].value.v_vec2 = { p1.x, p1.y };
    // ARG("vec2", "p2");
    args[1].kind         = kindof_VEC2;
    args[1].value.v_vec2 = { p2.x, p2.y };
    // ARG("vec3", "color");
    args[2].kind         = kindof_VEC3;
    args[2].value.v_vec3 = b2_HexColorToVec3(color);

    g_chuglAPI->vm->invoke_mfun_immediate_mode(
      ckobj, b2_DebugDraw_DrawSegment_callback_offset, g_chuglVM, origin_shred, args,
      ARRAY_LENGTH(args));
}

static void b2_DebugDrawTransformCallback(b2Transform transform, void* context)
{
    Chuck_Object* ckobj          = (Chuck_Object*)context;
    Chuck_VM_Shred* origin_shred = chugin_getOriginShred(ckobj);

    Chuck_DL_Arg args[2];

    // ARG("vec2", "position");
    args[0].kind         = kindof_VEC2;
    args[0].value.v_vec2 = { transform.p.x, transform.p.y };
    // ARG("float", "rotation_radians");
    args[1].kind          = kindof_FLOAT;
    args[1].value.v_float = b2Rot_GetAngle(transform.q);

    g_chuglAPI->vm->invoke_mfun_immediate_mode(
      ckobj, b2_DebugDraw_DrawTransform_callback_offset, g_chuglVM, origin_shred, args,
      ARRAY_LENGTH(args));
}

static void b2_DebugDrawPointCallback(b2Vec2 p, float size, b2HexColor color,
                                      void* context)
{
    Chuck_Object* ckobj          = (Chuck_Object*)context;
    Chuck_VM_Shred* origin_shred = chugin_getOriginShred(ckobj);

    Chuck_DL_Arg args[3];

    // ARG("vec2", "position");
    args[0].kind         = kindof_VEC2;
    args[0].value.v_vec2 = { p.x, p.y };
    // ARG("float", "size");
    args[1].kind          = kindof_FLOAT;
    args[1].value.v_float = size;
    // ARG("vec3", "color");
    args[2].kind         = kindof_VEC3;
    args[2].value.v_vec3 = b2_HexColorToVec3(color);

    g_chuglAPI->vm->invoke_mfun_immediate_mode(
      ckobj, b2_DebugDraw_DrawPoint_callback_offset, g_chuglVM, origin_shred, args,
      ARRAY_LENGTH(args));
}

static void b2_DebugDrawStringCallback(b2Vec2 p, const char* s, void* context)
{
    // unclear what this does... leaving out for now
    if (true) return;

    ASSERT(false);

    Chuck_Object* ckobj          = (Chuck_Object*)context;
    Chuck_VM_Shred* origin_shred = chugin_getOriginShred(ckobj);

    Chuck_DL_Arg args[2];

    // ARG("vec2", "position");
    args[0].kind         = kindof_VEC2;
    args[0].value.v_vec2 = { p.x, p.y };
    // ARG("string", "text");
    args[1].kind           = kindof_INT;
    args[1].value.v_object = (Chuck_Object*)chugin_createCkString(s, false);

    g_chuglAPI->vm->invoke_mfun_immediate_mode(
      ckobj, b2_DebugDraw_DrawString_callback_offset, g_chuglVM, origin_shred, args,
      ARRAY_LENGTH(args));
}

static void ckobj_to_b2DebugDraw(b2DebugDraw* obj, Chuck_Object* ckobj)
{
    CK_DL_API API = g_chuglAPI;
    // set flags first
    t_CKVEC4 drawing_bounds = OBJ_MEMBER_VEC4(ckobj, b2_DebugDraw_drawingBounds_offset);
    obj->drawingBounds      = vec4_to_b2AABB(drawing_bounds);
    obj->useDrawingBounds
      = (bool)OBJ_MEMBER_INT(ckobj, b2_DebugDraw_useDrawingBounds_offset);
    obj->drawShapes = (bool)OBJ_MEMBER_INT(ckobj, b2_DebugDraw_drawShapes_offset);
    obj->drawJoints = (bool)OBJ_MEMBER_INT(ckobj, b2_DebugDraw_drawJoints_offset);
    obj->drawJointExtras
      = (bool)OBJ_MEMBER_INT(ckobj, b2_DebugDraw_drawJointExtras_offset);
    obj->drawAABBs    = (bool)OBJ_MEMBER_INT(ckobj, b2_DebugDraw_drawAABBs_offset);
    obj->drawMass     = (bool)OBJ_MEMBER_INT(ckobj, b2_DebugDraw_drawMass_offset);
    obj->drawContacts = (bool)OBJ_MEMBER_INT(ckobj, b2_DebugDraw_drawContacts_offset);
    obj->drawGraphColors
      = (bool)OBJ_MEMBER_INT(ckobj, b2_DebugDraw_drawGraphColors_offset);
    obj->drawContactNormals
      = (bool)OBJ_MEMBER_INT(ckobj, b2_DebugDraw_drawContactNormals_offset);
    obj->drawContactImpulses
      = (bool)OBJ_MEMBER_INT(ckobj, b2_DebugDraw_drawContactImpulses_offset);
    obj->drawFrictionImpulses
      = (bool)OBJ_MEMBER_INT(ckobj, b2_DebugDraw_drawFrictionImpulses_offset);

    obj->context = ckobj;

    // set the callbacks
    obj->DrawPolygon      = b2_DebugDrawPolygonCallback;
    obj->DrawSolidPolygon = b2_DebugDrawSolidPolygonCallback;
    obj->DrawCircle       = b2_DebugDrawCircleCallback;
    obj->DrawSolidCircle  = b2_DebugDrawSolidCircleCallback;
    obj->DrawSolidCapsule = b2_DebugDrawSolidCapsuleCallback;
    obj->DrawSegment      = b2_DebugDrawSegmentCallback;
    obj->DrawTransform    = b2_DebugDrawTransformCallback;
    obj->DrawPoint        = b2_DebugDrawPointCallback;
    obj->DrawString       = b2_DebugDrawStringCallback;
}

CK_DLL_CTOR(b2_DebugDraw_ctor)
{
    // origin shred needed for API->vm->invoke_mfun_immediate_mode
    chugin_setOriginShred(SELF, SHRED);
}

CK_DLL_DTOR(b2_DebugDraw_dtor)
{
    chugin_removeFromOriginShredMap(SELF);
}

CK_DLL_MFUN(b2_DebugDraw_DrawPolygon)
{
}
CK_DLL_MFUN(b2_DebugDraw_DrawSolidPolygon)
{
}
CK_DLL_MFUN(b2_DebugDraw_DrawCircle)
{
}
CK_DLL_MFUN(b2_DebugDraw_DrawSolidCircle)
{
}
CK_DLL_MFUN(b2_DebugDraw_DrawSolidCapsule)
{
}
CK_DLL_MFUN(b2_DebugDraw_DrawSegment)
{
}
CK_DLL_MFUN(b2_DebugDraw_DrawTransform)
{
}
CK_DLL_MFUN(b2_DebugDraw_DrawPoint)
{
}
CK_DLL_MFUN(b2_DebugDraw_DrawString)
{
}

// ============================================================================
// b2ContactHitEvent
// ============================================================================

static void b2ContactHitEvent_to_ckobj(CK_DL_API API, Chuck_Object* ckobj,
                                       b2ContactHitEvent* obj)
{
    OBJ_MEMBER_B2_ID(b2ShapeId, ckobj, b2ContactHitEvent_shapeIdA_offset)
      = obj->shapeIdA;
    OBJ_MEMBER_B2_ID(b2ShapeId, ckobj, b2ContactHitEvent_shapeIdB_offset)
      = obj->shapeIdB;
    OBJ_MEMBER_VEC2(ckobj, b2ContactHitEvent_point_offset)
      = { obj->point.x, obj->point.y };
    OBJ_MEMBER_VEC2(ckobj, b2ContactHitEvent_normal_offset)
      = { obj->normal.x, obj->normal.y };
    OBJ_MEMBER_FLOAT(ckobj, b2ContactHitEvent_approachSpeed_offset)
      = obj->approachSpeed;
}

// static void ckobj_to_b2ContactHitEvent(CK_DL_API API, b2ContactHitEvent* obj,
// Chuck_Object* ckobj)
// {
// obj->shapeIdA = OBJ_MEMBER_INT(ckobj, b2ContactHitEvent_shapeIdA_offset);
// obj->shapeIdB = OBJ_MEMBER_INT(ckobj, b2ContactHitEvent_shapeIdB_offset);
// t_CKVEC2 point_vec2 = OBJ_MEMBER_VEC2(ckobj,
// b2ContactHitEvent_point_offset); obj->point = { (float) point_vec2.x,
// (float) point_vec2.y }; t_CKVEC2 normal_vec2 = OBJ_MEMBER_VEC2(ckobj,
// b2ContactHitEvent_normal_offset); obj->normal = { (float) normal_vec2.x,
// (float) normal_vec2.y }; obj->approachSpeed = (float) OBJ_MEMBER_FLOAT(ckobj,
// b2ContactHitEvent_approachSpeed_offset);
// }
