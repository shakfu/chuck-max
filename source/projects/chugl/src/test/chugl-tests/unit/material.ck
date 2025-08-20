ShaderDesc shader_desc;
// test defaults
T.assert(shader_desc.vertexCode == "", "shader desc vertexCode");
T.assert(shader_desc.fragmentCode == "", "shader desc fragmentCode");
T.assert(shader_desc.vertexPath == "", "shader desc vertexPath");
T.assert(shader_desc.fragmentPath == "", "shader desc fragmentPath");
T.assert(T.arrayEquals(shader_desc.vertexLayout, [VertexFormat.Float3, VertexFormat.Float3, VertexFormat.Float2]), "shader desc default vertexLayout");
T.assert(!shader_desc.lit, "shader desc lit");
T.assert(!shader_desc.usesEnvMap, "shader desc usesEnvmap");

"vertex_string" => shader_desc.vertexCode;
"fragment_string" => shader_desc.fragmentCode;
"vertex_filepath" => shader_desc.vertexPath;
"fragment_filepath" => shader_desc.fragmentPath;
[
    VertexFormat.Float, 
    VertexFormat.Float2, 
    VertexFormat.Float3,
    VertexFormat.Float4,
    VertexFormat.Int,
    VertexFormat.Int2,
    VertexFormat.Int3,
    VertexFormat.Int4,
] @=> shader_desc.vertexLayout;
true => shader_desc.lit;
true => shader_desc.usesEnvMap;

// Shader shader; // default constructor not allowed
Shader shader(shader_desc);
T.assert(shader.fragmentCode() == shader_desc.fragmentCode, "shader fragmentCode");
T.assert(shader.vertexCode() == shader_desc.vertexCode, "shader vertexCode");
T.assert(shader.fragmentPath() == shader_desc.fragmentPath, "shader fragmentPath");
T.assert(shader.vertexPath() == shader_desc.vertexPath, "shader vertexPath");
T.assert(T.arrayEquals(shader.vertexLayout(), shader_desc.vertexLayout), "shader vertexLayout");
T.assert(shader.lit(), "shader lit");
T.assert(shader.usesEnvMap(), "shader usesEnvMap");

ShaderDesc shader_desc2;
null @=> shader_desc2.vertexLayout;
Shader shader2(shader_desc2);
T.assert(shader2.fragmentCode() == shader_desc2.fragmentCode, "shader2 fragmentCode");
T.assert(shader2.vertexCode() == shader_desc2.vertexCode, "shader2 vertexCode");
T.assert(shader2.fragmentPath() == shader_desc2.fragmentPath, "shader2 fragmentPath");
T.assert(shader2.vertexPath() == shader_desc2.vertexPath, "shader2 vertexPath");
T.assert(T.arrayEquals(shader2.vertexLayout(), [0,0,0,0,0,0,0,0]), "shader2 vertexLayout");
T.assert(!shader2.lit(), "shader2 lit");
T.assert(!shader2.usesEnvMap(), "shader2 usesEnvMap");

Material material;

T.assert(material.Cull_None == 0, "material Cull_None");
T.assert(material.Cull_Front == 1, "material Cull_Front");
T.assert(material.Cull_Back == 2, "material Cull_Back");

T.assert(material.Topology_PointList == 0, "material TOPOLOGY_POINTLIST");
T.assert(material.Topology_LineList == 1, "material TOPOLOGY_LINELIST");
T.assert(material.Topology_LineStrip == 2, "material TOPOLOGY_LINESTRIP");
T.assert(material.Topology_TriangleList == 3, "material TOPOLOGY_TRIANGLELIST");
T.assert(material.Topology_TriangleStrip == 4, "material TOPOLOGY_TRIANGLESTRIP");

T.assert(material.cullMode() == material.Cull_None, "material cullMode default"); 
material.cullMode(material.Cull_Back);
T.assert(material.cullMode() == material.Cull_Back, "material cullMode");

T.assert(material.topology() == material.Topology_TriangleList, "material topology default");
material.topology(material.Topology_PointList);
T.assert(material.topology() == material.Topology_PointList, "material topology");

T.assert(material.shader() == null, "material shader default");
shader => material.shader;
T.assert(material.shader() == shader, "material shader");

// Uniforms

material.uniformFloat(0, 1.2);
T.assert(T.feq(material.uniformFloat(0), 1.2), "material uniformFloat");
T.assert(T.arrayEquals(material.activeUniformLocations(), [0]), "material activeUniformLocations");
material.uniformFloat(1, 3.4);
T.assert(T.feq(material.uniformFloat(1), 3.4), "material uniformFloat");

material.storageBuffer(0, [1.0]);
T.assert(T.arrayEquals(material.activeUniformLocations(), [0, 1]), "material activeUniformLocations after storageBuffer");

material.uniformInt(1, 1);
T.assert(material.uniformInt(1) == 1, "material uniformInt");
material.uniformInt2(2, 1, 2);
T.assert(T.arrayEquals(material.uniformInt2(2), [1, 2]), "material uniformInt2");
material.uniformInt3(3, 1, 2, 3);
T.assert(T.arrayEquals(material.uniformInt3(3), [1, 2, 3]), "material uniformInt3");
material.uniformInt4(4, 1, 2, 3, 4);
T.assert(T.arrayEquals(material.uniformInt4(4), [1, 2, 3, 4]), "material uniformInt4");

// test setting/getting TextureSampler

TextureSampler sampler;
TextureSampler.Wrap_Clamp => sampler.wrapU;
Texture texture;

material.texture(0, texture);
material.sampler(1, sampler);


T.assert(material.texture(0) == texture, "material texture");
T.assert(
    material.sampler(1).wrapU  == sampler.wrapU &&
    material.sampler(1).wrapV  == sampler.wrapV &&
    material.sampler(1).wrapW  == sampler.wrapW &&
    material.sampler(1).filterMin  == sampler.filterMin &&
    material.sampler(1).filterMag  == sampler.filterMag &&
    material.sampler(1).filterMip  == sampler.filterMip,
    "material sampler"
);

// FlatMaterial 

FlatMaterial flat_material;

flat_material.sampler(sampler);

T.assert(
    flat_material.sampler().wrapU  == sampler.wrapU &&
    flat_material.sampler().wrapV  == sampler.wrapV &&
    flat_material.sampler().wrapW  == sampler.wrapW &&
    flat_material.sampler().filterMin  == sampler.filterMin &&
    flat_material.sampler().filterMag  == sampler.filterMag &&
    flat_material.sampler().filterMip  == sampler.filterMip,
    "material sampler"
);

// FlatMaterial default scale and offset
T.assert( T.veq(flat_material.offset(), @(0, 0)), "flat_material offset default");
T.assert( T.veq(flat_material.scale(), @(1, 1)), "flat_material scale default");