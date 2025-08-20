//-----------------------------------------------------------------------------
// name: custom_geo.ck
// desc: creating custom meshes via passing vertex data directly in chuck
// requires: ChuGL + chuck-1.5.3.0 or higher
//
// author: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//         Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Fall 2023
//-----------------------------------------------------------------------------

// Let's build a square out of 2 triangles!

// Construct the vertex data ==================================================
Geometry customGeometry;

// pass in 3D positions for each vertex
customGeometry.vertexAttribute(
    Geometry.AttributeLocation_Position,
    3,
    // vertex positions for a plane
    [
        -0.5, 0.5, 0,  // top left
        0.5, 0.5, 0,   // top right
        -0.5, -0.5, 0, // bottom left
        0.5, -0.5, 0,  // bottom right
    ]
);

// pass in the normals (used in lighting calculations), make sure they
// are normalized (i.e. have magnitude = 1)
customGeometry.vertexAttribute(
    // vertex normals for a plane ( all of them point out along +z axis )
    Geometry.AttributeLocation_Normal,
    3,
    [
        0.0, 0, 1, 
        0, 0, 1, 
        0, 0, 1, 
        0, 0, 1,
    ]
);

// pass in the texture coordinates (used to map textures onto the mesh)
// each field be clamped between 0 and 1
customGeometry.vertexAttribute(
    // vertex uvs for a plane
    Geometry.AttributeLocation_UV,
    2,
    [
        0.0, 1, 
        1, 1, 
        0, 0, 
        1, 0, 
    ]
);

// pass in the indices. optional
// if set, every 3 indices will be used to construct a triangle
customGeometry.indices(
    [
        0, 2, 1, // bottom left triangle
        2, 3, 1, // top right triangle
    ]
);

// Scene setup ================================================================
PBRMaterial mat;
GMesh mesh(customGeometry, mat) --> GG.scene();

// Game loop ==================================================================
while (true) {
    GG.nextFrame() => now; 
    GG.dt() => mesh.rotateY;  // rotate on Y axis
}