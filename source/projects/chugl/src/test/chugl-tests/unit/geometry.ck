// PlaneGeometry B(3.0, 4.0, 5, 6);

// Custom geometry test
T.assert(Geometry.AttributeLocation_Count == 8, "MAX_VERTEX_ATTRIBUTES");
T.assert(Geometry.AttributeLocation_Position == 0, "POSITION_ATTRIBUTE_LOCATION");
T.assert(Geometry.AttributeLocation_Normal == 1, "NORMAL_ATTRIBUTE_LOCATION");
T.assert(Geometry.AttributeLocation_UV == 2, "UV_ATTRIBUTE_LOCATION");

Geometry custom_geo;

T.assert(custom_geo.vertexAttributeNumComponents()[0] == 0, "default vertexAttributeNumComponents");
T.assert(custom_geo.vertexAttributeData(0).size() == 0, "default vertexAttributeData");

// CK_DLL_MFUN(geo_set_vertex_attribute);
[1.0, 2.0, 3.0] @=> float vertex_attrib_data[];
custom_geo.vertexAttribute(0, 3, vertex_attrib_data);

// CK_DLL_MFUN(geo_get_vertex_attribute_num_components);
T.assert(custom_geo.vertexAttributeNumComponents()[0] == 3, "vertexAttributeNumComponents");
// CK_DLL_MFUN(geo_get_vertex_attribute_data);
custom_geo.vertexAttributeData(0) @=> float copied_vertex_attrib_data[];
T.assert(vertex_attrib_data.size() == copied_vertex_attrib_data.size(), "vertexAttributeData size");
for (0 => int i; i < vertex_attrib_data.size(); ++i) {
    T.assert(T.feq(vertex_attrib_data[i], copied_vertex_attrib_data[i]), "vertexAttributeData equality");
}

// CK_DLL_MFUN(geo_set_indices);
[0, 1, 2] @=> int indices[];
custom_geo.indices(indices);

// CK_DLL_MFUN(geo_get_indices);
custom_geo.indices() @=> int copied_indices[];
T.assert(indices.size() == copied_indices.size(), "indices size");
for (0 => int i; i < indices.size(); ++i) {
    T.assert(indices[i] == copied_indices[i], "indices equality");
}

// setting integer attribute data
// CK_DLL_MFUN(geo_set_vertex_attribute_int);
// CK_DLL_MFUN(geo_get_vertex_attribute_data_int);
[6, 7, 8, 9] @=> int vertex_attrib_data_int[];
custom_geo.vertexAttribute(0, 4, vertex_attrib_data_int);
T.assert(custom_geo.vertexAttributeNumComponents()[0] == 4, "int vertexAttributeNumComponents");
// custom_geo.vertexAttributeData(0) @=> float copied_vertex_attrib_data[];
T.assert(T.arrayEquals(vertex_attrib_data_int, custom_geo.vertexAttributeDataInt(0)), "get vertexAttribInt");

[@(1,2,3)] @=> vec3 positions[];
[@(4,5,6)] @=> vec3 normals[];
[@(7,8)] @=> vec2 uvs[];

custom_geo.positions(positions);
custom_geo.normals(normals);
custom_geo.uvs(uvs);

T.assert(T.arrayEquals(positions, custom_geo.positions()), "positions");
T.assert(T.arrayEquals(normals, custom_geo.normals()), "normals");
T.assert(T.arrayEquals(uvs, custom_geo.uvs()), "uvs");
T.assert(T.arrayEquals(custom_geo.vertexAttributeNumComponents(), [3, 3, 2, 0, 0, 0, 0, 0]), "vertexAttributeNumComponents ALL");

// plane geo
PlaneGeometry A;
[0,2,1,2,3,1] @=> int truth_plane_indices[];
A.indices() @=> int plane_indices[];
for (0 => int i; i < truth_plane_indices.size(); ++i) {
    T.assert(truth_plane_indices[i] == plane_indices[i], "plane indices equality");
}
T.assert(T.arrayEquals(A.vertexAttributeNumComponents(), [3, 3, 2, 0, 0, 0, 0, 0]), "plane vertexAttributeNumComponents ALL");

// programmable vertex pulling
Geometry pulled_geo;
[1.1, 2.0, 3.0, 4.0, 5.0, 6.0] @=> float pulled_vertex_data[];
pulled_geo.pulledVertexAttribute(0, pulled_vertex_data);
pulled_geo.pulledVertexAttribute(0) @=> float pulled_vertex_data_copy[];
T.assert(pulled_vertex_data.size() == pulled_vertex_data_copy.size(), "pulledVertexAttribute size");
T.assert(T.arrayEquals(pulled_vertex_data, pulled_vertex_data_copy), "pulledVertexAttribute equality");

pulled_geo.pulledVertexAttribute(1, indices);
T.assert(T.arrayEquals(indices,pulled_geo.pulledVertexAttributeInt(1)), "pulledVertexAttributeInt equality");
