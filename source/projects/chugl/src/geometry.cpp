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
#include "geometry.h"
#include "core/memory.h"
#include "suzanne_geo.cpp"
#include <array> // ew
#include <glm/gtc/epsilon.hpp>
#include <mikktspace/mikktspace.h>
#include <vector> // ew

// #include <earcut/earcut.hpp>
#include <rapidobj/rapidobj.hpp>

#include <cfloat> // FLT_MAX

// ============================================================================
// Vertex
// ============================================================================

void Vertex::pos(Vertex* v, char c, f32 val)
{
    switch (c) {
        case 'x': v->x = val; return;
        case 'y': v->y = val; return;
        case 'z': v->z = val; return;
        default: ASSERT(false); return; // error
    }
}

void Vertex::norm(Vertex* v, char c, f32 val)
{
    switch (c) {
        case 'x': v->nx = val; return;
        case 'y': v->ny = val; return;
        case 'z': v->nz = val; return;
        default: ASSERT(false); return; // error
    }
}

// ============================================================================
// Arena builders
// ============================================================================

struct gvec3i {
    u32 x, y, z;
};

struct gvec2f {
    f32 x, y;
};

struct gvec3f {
    f32 x, y, z;

    f32* comp(char c)
    {
        switch (c) {
            case 'x': return &x;
            case 'y': return &y;
            case 'z': return &z;
            default: ASSERT(false); return NULL;
        }
    }
};

struct gvec4f {
    f32 x, y, z, w;
};

static int GAB_vertexCount(GeometryArenaBuilder* builder)
{
    return ARENA_LENGTH(builder->pos_arena, gvec3f);
}

// static int GAB_indicesCount(GeometryArenaBuilder* builder)
// {
//     return ARENA_LENGTH(builder->indices_arena, u32);
// }

void Geometry_buildPlane(GeometryArenaBuilder* builder, PlaneParams* params)
{
    const f32 width_half  = params->width * 0.5f;
    const f32 height_half = params->height * 0.5f;

    const u32 gridX = params->widthSegments;
    const u32 gridY = params->heightSegments;

    const u32 gridX1 = gridX + 1;
    const u32 gridY1 = gridY + 1;

    const f32 segment_width  = params->width / gridX;
    const f32 segment_height = params->height / gridY;

    const u32 vertex_count    = gridX1 * gridY1;
    const u32 index_tri_count = gridX * gridY * 2;

    // initialize arena memory
    gvec3f* pos_array  = ARENA_PUSH_COUNT(builder->pos_arena, gvec3f, vertex_count);
    gvec3f* norm_array = ARENA_PUSH_COUNT(builder->norm_arena, gvec3f, vertex_count);
    gvec2f* uv_array   = ARENA_PUSH_COUNT(builder->uv_arena, gvec2f, vertex_count);
    gvec3i* indices_array
      = ARENA_PUSH_COUNT(builder->indices_arena, gvec3i, index_tri_count);

    u32 index = 0;
    for (u32 iy = 0; iy < gridY1; iy++) {
        const f32 y = iy * segment_height - height_half;
        for (u32 ix = 0; ix < gridX1; ix++) {
            const f32 x       = ix * segment_width - width_half;
            pos_array[index]  = { x, -y, 0 };
            norm_array[index] = { 0, 0, 1 };
            uv_array[index]   = { (f32)ix / gridX, 1.0f - ((f32)iy / gridY) };

            ++index;
        }
    }
    ASSERT(index == vertex_count);

    index = 0;
    for (u32 iy = 0; iy < gridY; iy++) {
        for (u32 ix = 0; ix < gridX; ix++) {
            const u32 a = ix + gridX1 * iy;
            const u32 b = ix + gridX1 * (iy + 1);
            const u32 c = (ix + 1) + gridX1 * (iy + 1);
            const u32 d = (ix + 1) + gridX1 * iy;

            indices_array[index++] = { a, b, d };
            indices_array[index++] = { b, c, d };
        }
    }
    ASSERT(index == index_tri_count);
}

void Geometry_buildSphere(GeometryArenaBuilder* builder, SphereParams* params)
{

    params->widthSeg  = MAX(3, params->widthSeg);
    params->heightSeg = MAX(2, params->heightSeg);

    const f32 thetaEnd = MIN(params->thetaStart + params->thetaLength, PI);

    int num_vertices = (params->widthSeg + 1) * (params->heightSeg + 1);

    glm::vec3* positions
      = ARENA_PUSH_COUNT(builder->pos_arena, glm::vec3, num_vertices);
    glm::vec3* normals = ARENA_PUSH_COUNT(builder->norm_arena, glm::vec3, num_vertices);
    gvec2f* uvs        = ARENA_PUSH_COUNT(builder->uv_arena, gvec2f, num_vertices);

    u32 index = 0;
    std::vector<u32> grid;
    std::vector<u32> indices;

    // generate vertices, normals and uvs
    for (u32 iy = 0; iy <= params->heightSeg; iy++) {

        const f32 v = (f32)iy / (f32)params->heightSeg;

        // special case for the poles
        f32 uOffset = 0;
        if (iy == 0 && glm::epsilonEqual(params->thetaStart, 0.0f, EPSILON)) {
            uOffset = 0.5f / params->widthSeg;
        } else if (iy == params->heightSeg
                   && glm::epsilonEqual(thetaEnd, PI, EPSILON)) {
            uOffset = -0.5 / params->widthSeg;
        }

        for (u32 ix = 0; ix <= params->widthSeg; ix++) {

            const f32 u = (f32)ix / (f32)params->widthSeg;

            // vertex
            positions[index].x
              = -params->radius * glm::cos(params->phiStart + u * params->phiLength)
                * glm::sin(params->thetaStart + v * params->thetaLength);
            positions[index].y
              = params->radius * glm::cos(params->thetaStart + v * params->thetaLength);
            positions[index].z
              = params->radius * glm::sin(params->phiStart + u * params->phiLength)
                * glm::sin(params->thetaStart + v * params->thetaLength);

            // normal
            normals[index] = glm::normalize(positions[index]);

            // uv
            uvs[index].x = u + uOffset;
            uvs[index].y = 1 - v;

            grid.push_back(index++);
        }
    }

    const size_t rowSize = (size_t)params->widthSeg + 1;
    for (size_t iy = 0; iy < params->heightSeg; iy++) {
        for (size_t ix = 0; ix < params->widthSeg; ix++) {

            const u32 a = grid[(iy * rowSize) + ix + 1];
            const u32 b = grid[(iy * rowSize) + ix];
            const u32 c = grid[(rowSize * (iy + 1)) + ix];
            const u32 d = grid[rowSize * (iy + 1) + (ix + 1)];

            if (iy != 0 || params->thetaStart > EPSILON) {
                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(d);
            }
            if (iy != (size_t)params->heightSeg - 1 || thetaEnd < PI - EPSILON) {
                indices.push_back(b);
                indices.push_back(c);
                indices.push_back(d);
            }
        }
    }

    size_t index_count = indices.size();
    ASSERT(index_count % 3 == 0); // must be a multiple of 3 triangles only

    u32* indices_array = ARENA_PUSH_COUNT(builder->indices_arena, u32, index_count);

    // copy indices
    memcpy(indices_array, indices.data(), indices.size() * sizeof(*indices_array));
    ASSERT(ARENA_LENGTH(builder->indices_arena, u32) == index_count);
}

void Geometry_buildSuzanne(GeometryArenaBuilder* builder)
{
    // copy data.
    f32* positions
      = ARENA_PUSH_COUNT(builder->pos_arena, f32, ARRAY_LENGTH(suzanne_positions));
    f32* normals
      = ARENA_PUSH_COUNT(builder->norm_arena, f32, ARRAY_LENGTH(suzanne_normals));
    f32* texcoords
      = ARENA_PUSH_COUNT(builder->uv_arena, f32, ARRAY_LENGTH(suzanne_uvs));

    memcpy(positions, suzanne_positions, sizeof(suzanne_positions));
    memcpy(normals, suzanne_normals, sizeof(suzanne_normals));
    memcpy(texcoords, suzanne_uvs, sizeof(suzanne_uvs));
}

// Box ============================================================================

static void Geometry_Box_buildPlane(GeometryArenaBuilder* gab, char u, char v, char w,
                                    int udir, int vdir, float width, float height,
                                    float depth, int gridX, int gridY)
{

    const float segmentWidth  = width / (float)gridX;
    const float segmentHeight = height / (float)gridY;

    const float widthHalf  = width / 2.0f;
    const float heightHalf = height / 2.0f;
    const float depthHalf  = depth / 2.0f;

    const int gridX1 = gridX + 1;
    const int gridY1 = gridY + 1;

    // save number of vertices BEFORE adding any this round
    // used to figure out indices
    const int numberOfVertices = GAB_vertexCount(gab);
    gvec3f* pos_array  = ARENA_PUSH_COUNT(gab->pos_arena, gvec3f, gridX1 * gridY1);
    gvec3f* norm_array = ARENA_PUSH_COUNT(gab->norm_arena, gvec3f, gridX1 * gridY1);
    gvec2f* uv_array   = ARENA_PUSH_COUNT(gab->uv_arena, gvec2f, gridX1 * gridY1);
    gvec3i* indices_array
      = ARENA_PUSH_COUNT(gab->indices_arena, gvec3i, gridX * gridY * 2);

    // generate vertices, normals and uvs
    int index = 0;
    for (int iy = 0; iy < gridY1; iy++) {
        const float y = iy * segmentHeight - heightHalf;
        for (int ix = 0; ix < gridX1; ix++) {
            const float x = ix * segmentWidth - widthHalf;

            gvec3f* pos   = &pos_array[index];
            *pos->comp(u) = x * udir;
            *pos->comp(v) = y * vdir;
            *pos->comp(w) = depthHalf;

            // set normals
            gvec3f* norm   = &norm_array[index];
            *norm          = { 0, 0, 0 };
            *norm->comp(w) = depth > 0 ? 1 : -1;

            // set uvs
            uv_array[index] = { (float)ix / gridX, 1.0f - (float)iy / gridY };

            ++index;
        }
    }

    // indices

    // 1. you need three indices to draw a single face
    // 2. a single segment consists of two faces
    // 3. so we need to generate six (2*3) indices per segment

    index = 0;
    for (int iy = 0; iy < gridY; iy++) {
        for (int ix = 0; ix < gridX; ix++) {

            unsigned int a = numberOfVertices + ix + gridX1 * iy;
            unsigned int b = numberOfVertices + ix + gridX1 * (iy + 1);
            unsigned int c = numberOfVertices + (ix + 1) + gridX1 * (iy + 1);
            unsigned int d = numberOfVertices + (ix + 1) + gridX1 * iy;

            // faces
            indices_array[index++] = { a, b, d };
            indices_array[index++] = { b, c, d };
        }
    }
}

void Geometry_buildBox(GeometryArenaBuilder* gab, BoxParams* params)
{
    Geometry_Box_buildPlane(gab, 'z', 'y', 'x', -1, -1, params->depth, params->height,
                            params->width, params->depthSeg, params->heightSeg); // px
    Geometry_Box_buildPlane(gab, 'z', 'y', 'x', 1, -1, params->depth, params->height,
                            -params->width, params->depthSeg, params->heightSeg); // nx
    Geometry_Box_buildPlane(gab, 'x', 'z', 'y', 1, 1, params->width, params->depth,
                            params->height, params->widthSeg, params->depthSeg); // py
    Geometry_Box_buildPlane(gab, 'x', 'z', 'y', 1, -1, params->width, params->depth,
                            -params->height, params->widthSeg, params->depthSeg); // ny
    Geometry_Box_buildPlane(gab, 'x', 'y', 'z', 1, -1, params->width, params->height,
                            params->depth, params->widthSeg, params->heightSeg); // pz
    Geometry_Box_buildPlane(gab, 'x', 'y', 'z', -1, -1, params->width, params->height,
                            -params->depth, params->widthSeg, params->heightSeg); // nz
}

void Geometry_buildCircle(GeometryArenaBuilder* gab, CircleParams* params)
{
    const int num_vertices = params->segments + 2;
    const int num_indices  = params->segments;

    gvec3f* positions     = ARENA_PUSH_COUNT(gab->pos_arena, gvec3f, num_vertices);
    gvec3f* normals       = ARENA_PUSH_COUNT(gab->norm_arena, gvec3f, num_vertices);
    gvec2f* uvs           = ARENA_PUSH_COUNT(gab->uv_arena, gvec2f, num_vertices);
    gvec3i* indices_array = ARENA_PUSH_COUNT(gab->indices_arena, gvec3i, num_indices);

    // center vertex
    positions[0] = { 0, 0, 0 };
    normals[0]   = { 0, 0, 1.0f };
    uvs[0]       = { 0.5f, 0.5f };

    int index = 1;
    for (int s = 0, i = 3; s <= params->segments; s++, i += 3) {
        const float segment
          = params->thetaStart
            + (float)s / (float)params->segments * params->thetaLength;

        // vertex
        positions[index] = { params->radius * glm::cos(segment),
                             params->radius * glm::sin(segment), 0 };

        // normal
        normals[index] = { 0, 0, 1.0f };

        // uvs
        uvs[index] = { (positions[index].x / params->radius + 1.0f) / 2.0f,
                       (positions[index].y / params->radius + 1.0f) / 2.0f };

        index++;
    }

    // indices
    index = 0;
    for (u32 i = 1; i <= params->segments; i++) {
        indices_array[index++] = { i, i + 1, 0 };
    }
}

void Geometry_buildTorus(GeometryArenaBuilder* gab, TorusParams* params)
{
    const int num_vertices
      = (params->radialSegments + 1) * (params->tubularSegments + 1);
    const int num_indices = params->radialSegments * params->tubularSegments * 2;

    ASSERT(sizeof(glm::vec3) == sizeof(gvec3f));
    glm::vec3* positions = ARENA_PUSH_COUNT(gab->pos_arena, glm::vec3, num_vertices);
    glm::vec3* normals   = ARENA_PUSH_COUNT(gab->norm_arena, glm::vec3, num_vertices);
    gvec2f* uvs          = ARENA_PUSH_COUNT(gab->uv_arena, gvec2f, num_vertices);
    gvec3i* indices      = ARENA_PUSH_COUNT(gab->indices_arena, gvec3i, num_indices);

    int index = 0;
    for (int j = 0; j <= params->radialSegments; j++) {
        for (int i = 0; i <= params->tubularSegments; i++) {
            const float u
              = (float)i / (float)params->tubularSegments * params->arcLength;
            const float v = (float)j / (float)params->radialSegments * PI * 2.0f;

            // vertex
            positions[index]
              = { (params->radius + params->tubeRadius * glm::cos(v)) * glm::cos(u),
                  (params->radius + params->tubeRadius * glm::cos(v)) * glm::sin(u),
                  params->tubeRadius * glm::sin(v) };

            // normal
            glm::vec3 center
              = { params->radius * glm::cos(u), params->radius * glm::sin(u), 0 };
            normals[index] = glm::normalize(positions[index] - center);

            // uv
            uvs[index] = { (float)i / (float)params->tubularSegments,
                           (float)j / (float)params->radialSegments };

            index++;
        }
    }

    // generate indices
    index = 0;
    for (u32 j = 1; j <= params->radialSegments; j++) {
        for (u32 i = 1; i <= params->tubularSegments; i++) {
            // indices
            const u32 a = (params->tubularSegments + 1) * j + i - 1;
            const u32 b = (params->tubularSegments + 1) * (j - 1) + i - 1;
            const u32 c = (params->tubularSegments + 1) * (j - 1) + i;
            const u32 d = (params->tubularSegments + 1) * j + i;

            indices[index++] = { a, b, d };
            indices[index++] = { b, c, d };
        }
    }
}

// Cylinder ============================================================================
static void Geometry_Cylinder_GenerateTorso(GeometryArenaBuilder* gab,
                                            const CylinderParams& p)
{
    const float halfHeight = p.height / 2.0f;

    const int num_vertices = (p.heightSegments + 1) * (p.radialSegments + 1);
    const int num_indices  = p.heightSegments * p.radialSegments * 2;
    gvec3f* positions      = ARENA_PUSH_COUNT(gab->pos_arena, gvec3f, num_vertices);
    gvec3f* normals        = ARENA_PUSH_COUNT(gab->norm_arena, gvec3f, num_vertices);
    gvec2f* uvs            = ARENA_PUSH_COUNT(gab->uv_arena, gvec2f, num_vertices);
    gvec3i* indices        = ARENA_PUSH_COUNT(gab->indices_arena, gvec3i, num_indices);

    static Arena index_array_arena{};
    ASSERT(ARENA_LENGTH(&index_array_arena, int) == 0);
    int* indexArray = ARENA_PUSH_COUNT(&index_array_arena, int, num_vertices);
    defer(Arena::clear(&index_array_arena));

    // this will be used to calculate the normal
    const float slope = (p.radiusBottom - p.radiusTop) / p.height;

    // generate vertices, normals and uvs
    int index = 0;
    for (unsigned int y = 0; y <= p.heightSegments; y++) {

        std::vector<unsigned int> indexRow;

        const float v = (float)y / (float)p.heightSegments;

        // calculate the radius of the current row
        const float radius = v * (p.radiusBottom - p.radiusTop) + p.radiusTop;

        for (unsigned int x = 0; x <= p.radialSegments; x++) {
            const float u = (float)x / (float)p.radialSegments;

            const float theta = u * p.thetaLength + p.thetaStart;

            const float sinTheta = glm::sin(theta);
            const float cosTheta = glm::cos(theta);

            // vertex
            positions[index]
              = { radius * sinTheta, -v * p.height + halfHeight, radius * cosTheta };

            // normal
            glm::vec3 normal = glm::normalize(glm::vec3(sinTheta, slope, cosTheta));
            normals[index]   = { normal.x, normal.y, normal.z };

            // uv
            uvs[index] = { u, 1.0f - v };

            indexArray[y * (p.radialSegments + 1) + x] = index;
            index++;
        }
    }

    // generate indices
    index = 0;
    for (unsigned int x = 0; x < p.radialSegments; x++) {
        for (unsigned int y = 0; y < p.heightSegments; y++) {

            // we use the index array to access the correct indices
            const unsigned int a = indexArray[y * (p.radialSegments + 1) + x];
            const unsigned int b = indexArray[(y + 1) * (p.radialSegments + 1) + x];
            const unsigned int c = indexArray[(y + 1) * (p.radialSegments + 1) + x + 1];
            const unsigned int d = indexArray[y * (p.radialSegments + 1) + x + 1];

            // faces
            indices[index++] = { a, b, d };
            indices[index++] = { b, c, d };
        }
    }
}

static void Geometry_Cylinder_GenerateCap(GeometryArenaBuilder* gab,
                                          const CylinderParams& p, bool top)
{
    const float halfHeight = p.height / 2.0f;

    // save the index of the first center vertex
    const int centerIndexStart = GAB_vertexCount(gab);

    const int num_vertices = 2 * p.radialSegments + 1;
    const int num_indices  = p.radialSegments;

    gvec3f* positions = ARENA_PUSH_COUNT(gab->pos_arena, gvec3f, num_vertices);
    gvec3f* normals   = ARENA_PUSH_COUNT(gab->norm_arena, gvec3f, num_vertices);
    gvec2f* uvs       = ARENA_PUSH_COUNT(gab->uv_arena, gvec2f, num_vertices);
    gvec3i* indices   = ARENA_PUSH_COUNT(gab->indices_arena, gvec3i, num_indices);

    const float radius = top ? p.radiusTop : p.radiusBottom;
    const float sign   = top ? 1.0f : -1.0f;

    // first we generate the center vertex data of the cap.
    // because the geometry needs one set of uvs per face,
    // we must generate a center vertex per face/segment
    int index = 0;
    for (unsigned int x = 1; x <= p.radialSegments; x++) {
        positions[index] = { 0.0, halfHeight * sign, 0.0 };
        normals[index]   = { 0.0, sign, 0.0 };
        uvs[index]       = { 0.5, 0.5 };

        index++;
    }

    // save the index of the last center vertex
    const unsigned int centerIndexEnd = index + centerIndexStart;

    // now we generate the surrounding vertices, normals and uvs
    for (unsigned int x = 0; x <= p.radialSegments; x++) {

        const float u     = (float)x / (float)p.radialSegments;
        const float theta = u * p.thetaLength + p.thetaStart;

        const float cosTheta = glm::cos(theta);
        const float sinTheta = glm::sin(theta);

        // vertex
        positions[index] = { radius * sinTheta, halfHeight * sign, radius * cosTheta };

        // normal
        normals[index] = { 0.0, sign, 0.0 };

        // uv
        uvs[index] = { cosTheta * 0.5f + 0.5f, sinTheta * 0.5f * sign + 0.5f };

        // increase index
        index++;
    }

    // generate indices
    for (unsigned int x = 0; x < p.radialSegments; x++) {
        const unsigned int c = centerIndexStart + x;
        const unsigned int i = centerIndexEnd + x;

        if (top) {
            indices[x] = { i, i + 1, c };
        } else {
            indices[x] = { i + 1, i, c };
        }
    }
}

void Geometry_buildCylinder(GeometryArenaBuilder* gab, CylinderParams* params)
{
    // generate torso
    Geometry_Cylinder_GenerateTorso(gab, *params);

    if (!params->openEnded) {
        if (params->radiusTop > 0) Geometry_Cylinder_GenerateCap(gab, *params, true);
        if (params->radiusBottom > 0)
            Geometry_Cylinder_GenerateCap(gab, *params, false);
    }
}

// Knot ============================================================================

static void Geometry_Knot_calculatePositionOnCurve(float u, int p, int q, float radius,
                                                   glm::vec3& position)
{

    const float quOverP = (f32)q / p * u;
    const float cs      = glm::cos(quOverP);

    position.x = radius * (2 + cs) * 0.5 * glm::cos(u);
    position.y = radius * (2 + cs) * glm::sin(u) * 0.5;
    position.z = radius * glm::sin(quOverP) * 0.5;
}

void Geometry_buildKnot(GeometryArenaBuilder* gab, KnotParams* params)
{
    // buffers
    const int num_vertices
      = (params->tubularSegments + 1) * (params->radialSegments + 1);
    const int num_indices = 2 * params->tubularSegments * params->radialSegments;

    glm::vec3* positions = ARENA_PUSH_COUNT(gab->pos_arena, glm::vec3, num_vertices);
    glm::vec3* normals   = ARENA_PUSH_COUNT(gab->norm_arena, glm::vec3, num_vertices);
    gvec2f* uvs          = ARENA_PUSH_COUNT(gab->uv_arena, gvec2f, num_vertices);
    gvec3i* indices      = ARENA_PUSH_COUNT(gab->indices_arena, gvec3i, num_indices);

    glm::vec3 P1 = glm::vec3(0.0f);
    glm::vec3 P2 = glm::vec3(0.0f);

    glm::vec3 B = glm::vec3(0.0f);
    glm::vec3 T = glm::vec3(0.0f);
    glm::vec3 N = glm::vec3(0.0f);

    // generate vertices, normals and uvs
    int index = 0;
    for (int i = 0; i <= params->tubularSegments; ++i) {

        // the radian "u" is used to calculate the position on the torus curve of the
        // current tubular segment

        const float u = (f32)i / params->tubularSegments * params->p * PI * 2.0f;

        // now we calculate two points. P1 is our current position on the curve, P2 is a
        // little farther ahead. these points are used to create a special "coordinate
        // space", which is necessary to calculate the correct vertex positions

        Geometry_Knot_calculatePositionOnCurve(u, params->p, params->q, params->radius,
                                               P1);
        Geometry_Knot_calculatePositionOnCurve(u + 0.01, params->p, params->q,
                                               params->radius, P2);

        // calculate orthonormal basis
        T = P2 - P1;
        N = P2 + P1;
        B = glm::normalize(glm::cross(T, N));
        N = glm::normalize(glm::cross(B, T));

        for (int j = 0; j <= params->radialSegments; ++j) {

            // now calculate the vertices. they are nothing more than an extrusion of
            // the torus curve. because we extrude a shape in the xy-plane, there is no
            // need to calculate a z-value.

            const float v  = (f32)j / params->radialSegments * PI * 2.0f;
            const float cx = -params->tube * glm::cos(v);
            const float cy = params->tube * glm::sin(v);

            // now calculate the final vertex position.
            // first we orient the extrusion with our basis vectors, then we add it to
            // the current position on the curve

            positions[index].x = P1.x + (cx * N.x + cy * B.x);
            positions[index].y = P1.y + (cx * N.y + cy * B.y);
            positions[index].z = P1.z + (cx * N.z + cy * B.z);

            // normal (P1 is always the center/origin of the extrusion, thus we can use
            // it to calculate the normal)

            normals[index] = glm::normalize(positions[index] - P1);

            // uv
            uvs[index].x = (float)i / params->tubularSegments;
            uvs[index].y = (float)j / params->radialSegments;

            index++;
        }
    }
    ASSERT(index == num_vertices);

    // generate indices

    index = 0;
    for (int j = 1; j <= params->tubularSegments; j++) {
        for (int i = 1; i <= params->radialSegments; i++) {

            // indices
            const u32 a = (params->radialSegments + 1) * (j - 1) + (i - 1);
            const u32 b = (params->radialSegments + 1) * j + (i - 1);
            const u32 c = (params->radialSegments + 1) * j + i;
            const u32 d = (params->radialSegments + 1) * (j - 1) + i;

            // faces
            indices[index++] = { a, b, d };
            indices[index++] = { b, c, d };
        }
    }
    ASSERT(index == num_indices);
}

void Geometry_buildPolygon(GeometryArenaBuilder* gab, PolygonParams* params)
{
    if (params->main_polygon_length == 0) return;

    // The number type to use for tessellation
    using Coord = f32;

    // The index type. Defaults to uint32_t, but you can also pass uint16_t if you know
    // that your data won't have more than 65536 vertices.
    using N = u32;

    // Create array
    using Point = std::array<Coord, 2>;
    std::vector<std::vector<Point>> polygon;
    static_assert(sizeof(std::array<f32, 2>) == sizeof(f32) * 2, "Point size mismatch");

    // create std::vector from f32* array
    std::vector<Point> main_polygon((Point*)params->main_polygon,
                                    (Point*)params->main_polygon
                                      + params->main_polygon_length);
    // TODO add safety bounds clamping
    polygon.push_back(main_polygon);

    int hole_run_length_accum = 0;
    for (int i = 0; i < params->num_holes; i++) {
        int end = hole_run_length_accum + params->hole_run_lengths[i];

        if (end > params->holes_length) {
            // past end of array, break out
            break;
        }

        std::vector<Point> hole((Point*)params->holes + hole_run_length_accum,
                                (Point*)params->holes + end);
        polygon.push_back(hole);

        hole_run_length_accum = end;
    }

    // Run tessellation
    // Returns array of indices that refer to the vertices of the input polygon.
    // e.g: the index 6 would refer to {25, 75} in this example.
    // Three subsequent indices form a triangle. Output triangles are clockwise.
    std::vector<N> earcut_indices = mapbox::earcut<N>(polygon);

    // setup vertex buffers
    int num_vertices      = params->main_polygon_length + params->holes_length;
    const int num_indices = earcut_indices.size();

    // arenas should be reset
    ASSERT(gab->pos_arena->curr == 0);
    ASSERT(gab->norm_arena->curr == 0);
    ASSERT(gab->uv_arena->curr == 0);
    ASSERT(gab->indices_arena->curr == 0);

    glm::vec3* positions = ARENA_PUSH_COUNT(gab->pos_arena, glm::vec3, num_vertices);
    glm::vec3* normals   = ARENA_PUSH_COUNT(gab->norm_arena, glm::vec3, num_vertices);
    gvec2f* uvs          = ARENA_PUSH_COUNT(gab->uv_arena, gvec2f, num_vertices);
    u32* indices         = ARENA_PUSH_COUNT(gab->indices_arena, u32, num_indices);

    // set positions and get min/max vertices for setting uvs
    glm::vec2 min             = { FLT_MAX, FLT_MAX };
    glm::vec2 max             = { FLT_MIN, FLT_MIN };
    glm::vec2* polygon_coords = (glm::vec2*)params->main_polygon;
    for (int i = 0; i < params->main_polygon_length; i++) {
        // set position
        positions[i] = { polygon_coords[i].x, polygon_coords[i].y, 0 };
        // update min/max
        min = glm::min(min, polygon_coords[i]);
        max = glm::max(max, polygon_coords[i]);
        // set normal
        normals[i] = { 0, 0, 1 };
    }

    glm::vec2* hole_coords = (glm::vec2*)params->holes;
    for (int i = 0; i < params->holes_length; i++) {
        // set position
        positions[params->main_polygon_length + i]
          = { hole_coords[i].x, hole_coords[i].y, 0 };
        // update min/max
        min = glm::min(min, hole_coords[i]);
        // set normal
        normals[params->main_polygon_length + i] = { 0, 0, 1 };
    }

    // now that we have min/max, we can set uvs
    float width  = max.x - min.x;
    float height = max.y - min.y;
    for (int i = 0; i < num_vertices; i++) {
        uvs[i].x = (positions[i].x - min.x) / width;
        uvs[i].y = (positions[i].y - min.y) / height;
    }

    // copy indices
    memcpy(indices, earcut_indices.data(), earcut_indices.size() * sizeof(*indices));
}
