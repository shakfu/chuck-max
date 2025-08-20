//---------------------------------------------------------------------
// name: gen-chugl.ck
// desc: generate HTML documentation for ChuGL
//
// author: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//         Ge Wang (https://ccrma.stanford.edu/~ge/)
//   date: Fall 2023
//---------------------------------------------------------------------

// instantiate a CKDoc object
CKDoc doc; // documentation orchestra
// set the examples root
"../examples/" => doc.examplesRoot;

// add group
doc.addGroup(
    // class names
    [
        "GG", 
        "GWindow", 
        "NextFrameEvent",
        "WindowResizeEvent",
        "WindowResizeEvent",                                             
        "WindowCloseEvent",                                               
        "ContentScaleChangedEvent",
        "SG_Component",
        "Color",
        "AssLoader"
    ],
    // group name
    "Basic Classes",
    // file name
    "chugl-basic", 
    // group description
    "Basic classes for ChuGL: strongly-timed audiovisual programming in ChucK."
);

// add group
doc.addGroup(
    // class names
    [
        "GGen", 
        "GScene", 
        "GPoints", 
        "GLines", 
        "GMesh", 
        "GCircle", 
        "GPlane", 
        "GCube", 
        "GSphere", 
        "GTorus",
        "GCylinder",
        "GKnot",
        "GSuzanne",
        "GLight", 
        "GPointLight", 
        "GDirLight", 
        "GSpotLight",
        "GCamera",
        "GOrbitCamera",
        "GFlyCamera",
        "GText"
    ],
    // group name
    "Graphics Generators",
    // file name
    "chugl-ggens", 
    // group description
    "Graphics generators (GGens) that can be composed together in a scene graph."
);

// add group
doc.addGroup(
    // class names
    [
        "Geometry", 
        "CubeGeometry", 
        "SphereGeometry", 
        "CircleGeometry", 
        "PlaneGeometry", 
        "TorusGeometry", 
        "CylinderGeometry",
        "KnotGeometry",
        "SuzanneGeometry",
        "LinesGeometry",
        "PolygonGeometry",
    ],
    // group name
    "ChuGL Geometries",
    // file name
    "chugl-geo", 
    // group description
    "ChuGL geometries for assigning vertex data such as positions, normals, UV coordinates, and tangents to a GMesh."
);


// add group
doc.addGroup(
    // class names
    [ 
        // shader stuff
        "VertexFormat",
        "ShaderDesc",
        "Shader",
    ],
    // group name
    "ChuGL Shaders",
    // file name
    "chugl-shader", 
    // group description
    "ChuGL classes for creating shaders via shader source code and layout descriptions."
);

// add group
doc.addGroup(
    // class names
    [ 
        // material stuff
        "Material", 
        "NormalMaterial", 
        "WireframeMaterial", 
        "FlatMaterial", 
        "PhongMaterial", 
        "LinesMaterial", 
        "UVMaterial", 
        "PBRMaterial", 
    ],
    // group name
    "ChuGL Materials",
    // file name
    "chugl-mat", 
    // group description
    "ChuGL materials wrap and supply parameters to a ChuGL shader, describing the appearance of geometries."
);

doc.addGroup(
    // class names
    [ 
        "GPass",
        "RenderPass",
        "ScenePass",
        "ScreenPass",
        "OutputPass",
        "ComputePass",
        "BloomPass",
    ],
    // group name
    "ChuGL Passes",
    // file name
    "chugl-pass", 
    // group description
    "Render/Compute pass abstraction for creating custom render graphs"
);

doc.addGroup(
    // class names
    [ 
        "Texture",
        "TextureSampler",
        "TextureDesc",
        "TextureWriteDesc",
        "TextureLoadDesc",
    ],
    // group name
    "ChuGL Textures",
    // file name
    "chugl-tex", 
    // group description
    "Textures can be loaded from file, created dynamically from data; they are passed into materials and mapped onto 2D and 3D surfaces."
);

// add GUI group
doc.addGroup(
    // class names
    [
        "UI_Bool",
        "UI_String",
        "UI_Int",
        "UI_Int2",
        "UI_Int3",
        "UI_Int4",
        "UI_Float",
        "UI_Float2",
        "UI_Float3",
        "UI_Float4",
        "UI_Viewport",
        "UI_Style",
        "UI_WindowFlags",
        "UI_ChildFlags",
        "UI_Cond",
        "UI_Color",
        "UI_StyleVar",
        "UI_ButtonFlags",
        "UI_Direction",
        "UI_ComboFlags",
        "UI_SliderFlags",
        "UI_InputTextFlags",
        "UI_ColorEditFlags",
        "UI_TreeNodeFlags",
        "UI_SelectableFlags",
        "UI_PopupFlags",
        "UI_TableFlags",
        "UI_TableRowFlags",
        "UI_TableColumnFlags",
        "UI_TableBgTarget",
        "UI_TabBarFlags",
        "UI_MouseButton",
        "UI_Key",
        "UI_MouseCursor",
        "UI_ViewportFlags",
        "UI_FocusedFlags",
        "UI_HoveredFlags",
        "UI_DockNodeFlags",
        "UI_Callback",
        "UI_SizeCallbackData",
        "UI_SizeCallback",
        "UI_ComboCallback",
        "UI_DrawList",
        "UI",
    ],
    // group name
    "ChuGL UI",
    // file name
    "chugl-ui", 
    // group description
    "API methods for creating widgets such as buttons, sliders, dropdowns, etc.
    Use widgets to control parameter values during runtime. Great for experimentation
    and fine-tuning values. Also usable for building application user-interfaces."
);


doc.addGroup(
    // class names
    [
        "Video",
        "Webcam",
    ],
    // group name
    "ChuGL Video Objects",
    // file name
    "chugl-video", 
    // group description
    "Video and webcam access"
);

doc.addGroup(
    // class names
    [
        "StorageBuffer",
    ],
    // group name
    "GPU Buffers",
    // file name
    "chugl-buffers", 
    // group description
    "GPU Buffer wrappers for ChuGL"
);

/*
doc.addGroup(
    // class names
    [
        "b2",
        "b2World",
        "b2BodyType",
        "b2ShapeType",
        "b2BodyDef",
        "b2CastOutput",
        "b2Polygon",
        "b2Circle",
        "b2Capsule",
        "b2Segment",
        "b2Filter",
        "b2MassData",
        "b2RayCastInput",
        "b2ShapeCastInput",
        "b2SegmentDistanceResult",
        "b2QueryFilter",
        "b2RayResult",
        "b2ShapeDef",
        "b2WorldDef",
        "b2BodyMoveEvent",
        "b2ContactHitEvent",
        "b2Body",
        "b2Shape",
        "b2DebugDraw",
    ],
    // group name
    "Box2D",
    // file name
    "chugl-box2d", 
    // group description
    "Box2D physics engine"
);
*/

// sort for now until order is preserved by CKDoc
doc.sort(true);

// generate
doc.outputToDir( ".", "ChuGL [alpha] API Reference (v0.2.6)" );
