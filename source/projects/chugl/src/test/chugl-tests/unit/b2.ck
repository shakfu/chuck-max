b2WorldDef world_def;
// world_def.help();
T.assert(world_def.enableSleep == true, "enableSleep default value");
T.assert(world_def.enableContinuous == true, "enableContinuous default value");
T.assert(T.feq(world_def.contactHertz, 30.0), "contactHertz default value");
false => world_def.enableSleep;
T.assert(world_def.enableSleep == false, "enableSleep set to false");

b2.createWorld(world_def) => int world_id;
T.assert(b2World.isValid(world_id), "world created");
T.assert(!b2World.isValid(0), "invalid world");
b2.destroyWorld(world_id);
T.assert(!b2World.isValid(world_id), "destroyed world");

b2Filter filter;
T.assert(filter.categoryBits == 0x0001, "categoryBits default value");
T.assert(filter.maskBits & 0xFFFFFFFF == 0xFFFFFFFF, "maskBits default value");
T.assert(filter.groupIndex == 0, "groupIndex default value");

b2QueryFilter query_filter;
T.assert(query_filter.categoryBits == 0x0001, "query filter categoryBits default value");
T.assert(query_filter.maskBits & 0xFFFFFFFF == 0xFFFFFFFF, "query filter maskBits default value");

b2ShapeDef shape_def;
T.assert(T.feq(shape_def.density, 1.0), "density default value");
T.assert(shape_def.filter.categoryBits == 0x0001, "filter.categoryBits default value");
T.assert(shape_def.filter.maskBits & 0xFFFFFFFF == 0xFFFFFFFF, "filter.maskBits default value");
T.assert(shape_def.filter.groupIndex == 0, "filter.groupIndex default value");

b2BodyDef body_def;
@(1337, 2.3) => body_def.position;
T.assert(body_def.type == b2BodyType.staticBody, "type default value");
T.assert(T.feq(body_def.gravityScale, 1.0), "gravityScale default value");
T.assert(body_def.isAwake, "body is awake by default");

b2.createWorld(world_def) => world_id;
b2.createBody(world_id, body_def) => int body_id;
T.assert(b2Body.isValid(body_id), "body created");
T.assert(T.feq(1337, b2Body.position(body_id).x), "position.x set");

b2.makeBox(.5, .5) @=> b2Polygon@ box_poly;
b2.createPolygonShape(body_id, shape_def, box_poly) => int shape_id;

b2Shape.filter(shape_id) @=> b2Filter circle_shape_filter;
T.assert(circle_shape_filter.categoryBits == 0x0001, "filter.categoryBits default value");

b2Circle circle(@(1,2), 3);
b2.createCircleShape(body_id, shape_def, circle) => int circle_shape_id;

b2Segment segment(@(1,2), @(3,4));
b2.createSegmentShape(body_id, shape_def, segment) => int segment_shape_id;

b2Capsule capsule(@(1,2), @(3,4), 5);
b2.createCapsuleShape(body_id, shape_def, capsule) => int capsule_shape_id;

int shape_ids[0];
b2Body.shapes(body_id, shape_ids);
T.assert(shape_ids.size() == 4, "shape added to body");
for (auto shape_id : shape_ids)
    T.assert(b2Shape.isValid(shape_id), "shape is valid");

b2.destroyBody(body_id);
T.assert(!b2Body.isValid(body_id), "destroy body");
T.assert(!b2Shape.isValid(shape_ids[0]), "destroy shape");

// polygon creation tests

// b2.makeSquare(1.0) @=> b2Polygon square;
// T.println("Square vertices:");
// T.printArray(square.vertices());
// T.println("Square normals:");
// T.printArray(square.normals());
// T.println("Square centroid:");
// <<< square.centroid() >>>;
// T.println("Square radius:");
// <<< square.radius() >>>;


// CK_DLL_SFUN(b2_MakePolygon);
// CK_DLL_SFUN(b2_MakeOffsetPolygon);
// CK_DLL_SFUN(b2_MakeSquare);
// CK_DLL_SFUN(b2_MakeBox);
// CK_DLL_SFUN(b2_MakeRoundedBox);
// CK_DLL_SFUN(b2_MakeOffsetBox);

