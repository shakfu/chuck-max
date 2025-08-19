//-----------------------------------------------------------------------------
// name: triangulate.ck
// desc: Simple example of how to use the PolygonGeometry class to triangulate 
// an polygon with arbitrary holes.
//
// authors: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//
// date: Fall 2024
//-----------------------------------------------------------------------------

// Polygon Geometry can perform runtime triangulation via ear clipping
// Useful for animating or rendering deformable shapes
PolygonGeometry poly_geo;

// triangulate a hexagon with holes (you can call this at frame rate!)
poly_geo.build(
    // outer
    [
        @(-1, 0),
        @(-.5, Math.sqrt(3)/2),
        @(.5, Math.sqrt(3)/2),
        @(1, 0),
        @(.5, -Math.sqrt(3)/2),
        @(-.5, -Math.sqrt(3)/2),
    ],
    [
        // first square hole
        @(-.5, 0),
        @(-.5, .5),
        @(.5, .5),
        @(.5, 0),
        // second triangle hole (below)
        @(-.5, -.5),
        @(.5, -.5),
        @(0, -.8),
    ],
    // hole lengths
    [4, 3]
);

// create a mesh with the geometry
GMesh mesh(poly_geo, new PhongMaterial) --> GG.scene();

while (true) { GG.nextFrame() => now; }
