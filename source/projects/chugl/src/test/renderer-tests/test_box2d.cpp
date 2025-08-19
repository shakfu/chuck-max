#include "test_base.h"

// extern "C" {
#include <box2d/box2d.h>
// }

static b2WorldId worldId;
static b2BodyId bodyId;
static int subStepCount = 4;

static void _Test_Box2d_onInit(GraphicsContext* ctx, GLFWwindow* w)
{
    // The world creation copies all the data it needs out of the world
    // definition, so the world definition is no longer needed.
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldId             = b2CreateWorld(&worldDef);

    // Bodies are static by default. Static bodies don't collide with other
    // static bodies and are immovable by the simulation.
    b2BodyDef groundBodyDef = b2DefaultBodyDef();
    // groundBodyDef.position  = (b2Vec2){ 0.0f, -10.0f };
    groundBodyDef.position = { 0.0f, -10.0f };
    b2BodyId groundId      = b2CreateBody(worldId, &groundBodyDef);

    // Box2D does not keep a reference to the shape data. It copies the data
    // into the internal data structures.
    b2Polygon groundBox = b2MakeBox(50.0f, 10.0f);
    b2ShapeDef groundShapeDef
      = b2DefaultShapeDef(); // defines friction, density (mass), filter
    b2CreatePolygonShape(
      groundId, &groundShapeDef,
      &groundBox); // binds actual geometry to the body transform

    // create dynamic box
    // first create body
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type      = b2_dynamicBody;
    bodyDef.position  = { 0.0f, 4.0f };
    bodyId            = b2CreateBody(worldId, &bodyDef);

    // then shape
    b2Polygon dynamicBox = b2MakeBox(1.0f, 1.0f);
    b2ShapeDef shapeDef  = b2DefaultShapeDef();
    shapeDef.density     = 1.0f;
    shapeDef.friction    = 0.3f;
    b2CreatePolygonShape(bodyId, &shapeDef, &dynamicBox);
}

static void _Test_Box2d_onUpdate(f32 dt)
{
    // TODO: detach timestap from framerate
    // https://gafferongames.com/post/fix_your_timestep/
    b2World_Step(worldId, dt, subStepCount);
    b2Vec2 position = b2Body_GetPosition(bodyId);
    float angle     = b2Body_GetAngle(bodyId);
    printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);
}

static void _Test_Box2d_onRender(glm::mat4 proj, glm::mat4 view,
                                 glm::vec3 camPos)
{
}

static void _Test_Box2d_onExit()
{
    b2DestroyWorld(worldId);
}

void Test_Box2d(TestCallbacks* callbacks)
{
    *callbacks          = {};
    callbacks->onInit   = _Test_Box2d_onInit;
    callbacks->onUpdate = _Test_Box2d_onUpdate;
    callbacks->onRender = _Test_Box2d_onRender;
    callbacks->onExit   = _Test_Box2d_onExit;
}
