#include <cgltf/cgltf.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "core/log.h"
#include "core/memory.h"
#include "test_base.h"

#include "entity.h"
#include "geometry.h"
#include "r_component.h"
#include "shaders.h"

#include <iostream>
#include <unordered_map>
#include <vector>

static std::unordered_map<cgltf_texture*, SG_ID> rTextureMap;
static std::unordered_map<cgltf_material*, SG_ID> rMaterialMap;

static GraphicsContext* gctx = NULL;

static SG_ID* sceneIDs;
static cgltf_size numScenes;
// static SG_ID* geoIDs;

// TODO add frameArena to app or graphics context
static Arena frameArena = {};

// TODO remove these, refactor into renderer
static R_Geometry* geo        = NULL;
static R_Transform* testXform = NULL;

static u8 whitePixel[4]  = { 255, 255, 255, 255 };
static u8 blackPixel[4]  = { 0, 0, 0, 0 };
static u8 normalPixel[4] = { 128, 128, 255, 255 };

static Texture opaqueWhitePixel      = {};
static Texture transparentBlackPixel = {};
static Texture defaultNormalPixel    = {};

// static u8 gltf_sizeOfAccessor(cgltf_accessor* accessor)
// {
//     u8 componentSize = cgltf_component_size(accessor->component_type);
//     u8 numComponents = cgltf_num_components(accessor->type);
//     return componentSize * numComponents;
// }

// TODO cache gltf geometry, material ptrs etc so we don't process same data
static void gltf_ProcessNode(R_Transform* parent, cgltf_node* node)
{
    R_Transform* sg_node = Component_CreateTransform();
    sg_node->name        = node->name;
    log_trace("processing node: %s", sg_node->name.c_str());

    // xform data
    if (node->has_translation) {
        R_Transform::pos(sg_node, glm::make_vec3(node->translation));
    }
    if (node->has_rotation) {
        R_Transform::rot(sg_node, glm::make_quat(node->rotation));
    }
    if (node->has_scale) {
        R_Transform::sca(sg_node, glm::make_vec3(node->scale));
    }
    if (node->has_matrix) {
        sg_node->local = glm::make_mat4(node->matrix);
    } else {
        sg_node->local = R_Transform::localMatrix(sg_node);
    }
    sg_node->world = parent->world * sg_node->local;

    // set xform data if gltf only provided a matrix
    if (!node->has_rotation || !node->has_scale || !node->has_translation)
        R_Transform::setXformFromMatrix(sg_node, sg_node->local);

    // log_trace("sg_node pos: %s", glm::to_string(sg_node->pos).c_str());
    // log_trace("sg_node rot: %s", glm::to_string(sg_node->rot).c_str());
    // log_trace("sg_node sca: %s", glm::to_string(sg_node->sca).c_str());

    // parent-child relationship
    R_Transform::addChild(parent, sg_node);
    R_Transform::addChild(parent, testXform);

    // mesh
    if (node->mesh) {
        // TODO: all meshes should be processed linearly at start
        // (like how we do with textures)

        // gltf_ProcessMesh(node->mesh);
        cgltf_mesh* gltf_mesh = node->mesh;
        log_trace("processing mesh: %s", gltf_mesh->name);

        for (cgltf_size i = 0; i < gltf_mesh->primitives_count; ++i) {
            cgltf_primitive* primitive = gltf_mesh->primitives + i;
            log_trace("processing primitive %d", i);

            // TODO: cache map from primitive --> R_Geometry
            // this is only possible if a single primitive can be shared by
            // multiple gltf meshes
            // a primitive has exactly 1 material, and index/vertex data
            // technically R_Geometry can be reused if primitives share same
            // indices accessor and attributes
            // caching at primitive level, rather than accessor level, means
            // gltf meshes with same geometry but different material will
            // generate duplicate geometry

            // TODO: simplify this code with cgltf_accessor_unpack_float
            // and cgltf_accessor_unpack_indices

            Vertices vertices = {};
            Vertices::init(&vertices, primitive->attributes->data->count,
                           primitive->indices->count);

            // write indices to buffer
            ASSERT(sizeof(vertices.indices[0] == 4));
            cgltf_accessor_unpack_indices(primitive->indices, vertices.indices,
                                          sizeof(vertices.indices[0]),
                                          vertices.indicesCount);
            // print indices
            // log_trace("indices (%d):", indexCount);
            // for (u32 j = 0; j < indexCount; ++j) {
            //     printf("%d ", indices[j]);
            // }
            // printf("\n");

            // vertex attributes
            for (u8 attribIndex = 0; attribIndex < primitive->attributes_count;
                 ++attribIndex) {
                cgltf_attribute* attribute = primitive->attributes + attribIndex;
                log_trace("processing attribute %d [%d]: %s", attribIndex,
                          attribute->index, attribute->name);

                cgltf_accessor* accessor = attribute->data;
                ASSERT(accessor->count == vertices.vertexCount);

                cgltf_size numFloats
                  = accessor->count * cgltf_num_components(accessor->type);

                // get the write destination
                switch (attribute->type) {
                    case cgltf_attribute_type_position: {
                        ASSERT(accessor->type == cgltf_type_vec3);
                        ASSERT(numFloats == vertices.vertexCount * 3);
                        cgltf_accessor_unpack_floats(
                          accessor, Vertices::positions(&vertices), numFloats);
                        break;
                    }
                    case cgltf_attribute_type_normal: {
                        ASSERT(accessor->type == cgltf_type_vec3);
                        ASSERT(numFloats == vertices.vertexCount * 3);
                        cgltf_accessor_unpack_floats(
                          accessor, Vertices::normals(&vertices), numFloats);
                        break;
                    }
                    case cgltf_attribute_type_texcoord: {
                        ASSERT(accessor->type == cgltf_type_vec2);
                        ASSERT(numFloats == vertices.vertexCount * 2);
                        cgltf_accessor_unpack_floats(
                          accessor, Vertices::texcoords(&vertices), numFloats);
                        // TODO: handle texcoord indices
                        // e.g. TEXCOORD_0, TEXCOORD_1, etc.
                        break;
                    }
                    case cgltf_attribute_type_tangent: {
                        ASSERT(accessor->type == cgltf_type_vec4);
                        ASSERT(numFloats == vertices.vertexCount * 4);
                        cgltf_accessor_unpack_floats(
                          accessor, Vertices::tangents(&vertices), numFloats);
                        break;
                    }
                    default: {
                        log_error("unsupported attribute type %d", attribute->type);
                        break;
                    }
                }
                // cgltf_attribute_type_position,
                // cgltf_attribute_type_normal,
                // cgltf_attribute_type_tangent,
                // cgltf_attribute_type_texcoord,
                // cgltf_attribute_type_color,
            }

            // write to gpu buffer
            // TODO: store geo ids instead
            geo = Component_CreateGeometry();
            // PlaneParams planeParams = { 1.0f, 1.0f, 1u, 1u };
            // Vertices planeVertices  = createPlane(&planeParams);
            R_Geometry::buildFromVertices(gctx, geo, &vertices);
            // R_Geometry::buildFromVertices(gctx, geo, &planeVertices);
            // associate transform with geometry

            // free vertex data (TODO should R_Geo store cpu-side vertex data?)
            Vertices::free(&vertices);

            // print vertices
            // Vertices::print(&vertices);

            { // primitive material
                ASSERT(primitive->material != NULL);
                ASSERT(rMaterialMap.find(primitive->material) != rMaterialMap.end());
                // get our sg material
                R_Material* sgMaterial
                  = Component_GetMaterial(rMaterialMap[primitive->material]);
                // for rendering, materials need to know what geometries
                // are used by them

                // TODO: default normal texture
                /*
                const opaqueWhiteTexture = createSolidColorTexture(1, 1, 1, 1);
const transparentBlackTexture = createSolidColorTexture(0, 0, 0, 0);
const defaultNormalTexture = createSolidColorTexture(0.5, 0.5, 1, 1);
                */

                // add primitives to material
                R_Material::addPrimitive(sgMaterial, geo, testXform);
                R_Material::addPrimitive(sgMaterial, geo, sg_node);
            } // primitive material

            break; // TODO: remove. forces only 1 primitive
        } // foreach primitive
    } // if (mesh)

    // process children
    for (cgltf_size i = 0; i < node->children_count; ++i) {
        cgltf_node* child = node->children[i];
        gltf_ProcessNode(sg_node, child);
    }
}

static WGPUAddressMode _addressMode_GL_to_WGPU(cgltf_int glWrapMode)
{
    // GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_MIRRORED_REPEAT, GL_REPEAT, or
    // GL_MIRROR_CLAMP_TO_EDGE
#define _GLTF_CLAMP_TO_EDGE 33071
#define _GLTF_CLAMP_TO_BORDER 33069
#define _GLTF_MIRRORED_REPEAT 33648
#define _GLTF_REPEAT 10497
#define _GLTF_MIRROR_CLAMP_TO_EDGE 34627
    switch (glWrapMode) {
        // edge
        case _GLTF_CLAMP_TO_BORDER:
        case _GLTF_CLAMP_TO_EDGE: return WGPUAddressMode_ClampToEdge;
        // mirrored repeat
        case _GLTF_MIRROR_CLAMP_TO_EDGE: // weird edge case, repeats only once?
                                         // treating as mirrored repeat for now
        case _GLTF_MIRRORED_REPEAT: return WGPUAddressMode_MirrorRepeat;
        // repeat
        case _GLTF_REPEAT: return WGPUAddressMode_Repeat;
        default: {
            log_error("unsupported wrap mode %d", glWrapMode);
            ASSERT(false);
            return WGPUAddressMode_Repeat;
        }
    }
#undef _GLTF_CLAMP_TO_EDGE
#undef _GLTF_CLAMP_TO_BORDER
#undef _GLTF_MIRRORED_REPEAT
#undef _GLTF_REPEAT
#undef _GLTF_MIRROR_CLAMP_TO_EDGE
}

static WGPUFilterMode _magFilter_GL_to_WGPU(cgltf_int glMagFilter)
{
#define _GLTF_MAG_NEAREST 9728
#define _GLTF_MAG_LINEAR 9729
    switch (glMagFilter) {
        case _GLTF_MAG_NEAREST: return WGPUFilterMode_Nearest;
        case _GLTF_MAG_LINEAR: return WGPUFilterMode_Linear;
        default: {
            // log_error("unsupported mag filter %d", glMagFilter);
            return WGPUFilterMode_Linear;
        }
    }
#undef _GLTF_MAG_NEAREST
#undef _GLTF_MAG_LINEAR
}

static WGPUFilterMode _minFilter_GL_to_WGPU(cgltf_int glMinFilter)
{
#define _GLTF_MAG_NEAREST 9728
#define _GLTF_MAG_LINEAR 9729
#define _GLTF_NEAREST_MIPMAP_NEAREST 9984
#define _GLTF_LINEAR_MIPMAP_NEAREST 9985
#define _GLTF_NEAREST_MIPMAP_LINEAR 9986
#define _GLTF_LINEAR_MIPMAP_LINEAR 9987
    switch (glMinFilter) {

        case _GLTF_MAG_NEAREST:
        case _GLTF_NEAREST_MIPMAP_NEAREST:
        case _GLTF_NEAREST_MIPMAP_LINEAR: return WGPUFilterMode_Nearest;
        case _GLTF_MAG_LINEAR:
        case _GLTF_LINEAR_MIPMAP_NEAREST:
        case _GLTF_LINEAR_MIPMAP_LINEAR: return WGPUFilterMode_Linear;
        default: {
            log_error("unsupported min filter %d", glMinFilter);
            return WGPUFilterMode_Linear;
        }
    }
#undef _GLTF_MAG_NEAREST
#undef _GLTF_MAG_LINEAR
#undef _GLTF_NEAREST_MIPMAP_NEAREST
#undef _GLTF_LINEAR_MIPMAP_NEAREST
#undef _GLTF_NEAREST_MIPMAP_LINEAR
#undef _GLTF_LINEAR_MIPMAP_LINEAR
}

static WGPUMipmapFilterMode _mipmapFilter_GL_to_WGPU(cgltf_int glFilter)
{
#define _GLTF_MAG_NEAREST 9728
#define _GLTF_MAG_LINEAR 9729
#define _GLTF_NEAREST_MIPMAP_NEAREST 9984
#define _GLTF_LINEAR_MIPMAP_NEAREST 9985
#define _GLTF_NEAREST_MIPMAP_LINEAR 9986
#define _GLTF_LINEAR_MIPMAP_LINEAR 9987
    switch (glFilter) {
        // nearest
        case _GLTF_MAG_NEAREST:
        case _GLTF_NEAREST_MIPMAP_NEAREST:
        case _GLTF_LINEAR_MIPMAP_NEAREST: return WGPUMipmapFilterMode_Nearest;
        // linear
        case _GLTF_MAG_LINEAR:
        case _GLTF_NEAREST_MIPMAP_LINEAR:
        case _GLTF_LINEAR_MIPMAP_LINEAR: return WGPUMipmapFilterMode_Linear;
        default: {
            // log_error("unsupported mipmap filter %d", glFilter);
            return WGPUMipmapFilterMode_Linear;
        }
    }
#undef _GLTF_MAG_NEAREST
#undef _GLTF_MAG_LINEAR
#undef _GLTF_NEAREST_MIPMAP_NEAREST
#undef _GLTF_LINEAR_MIPMAP_NEAREST
#undef _GLTF_NEAREST_MIPMAP_LINEAR
#undef _GLTF_LINEAR_MIPMAP_LINEAR
}

static void
_gltf_texture_view_to_MaterialTextureView(cgltf_texture_view* gltf_texture_view,
                                          MaterialTextureView* materialTextureView)
{
    MaterialTextureView::init(materialTextureView);

    materialTextureView->texcoord     = gltf_texture_view->texcoord;
    materialTextureView->strength     = gltf_texture_view->scale;
    materialTextureView->hasTransform = gltf_texture_view->has_transform;
    if (gltf_texture_view->has_transform) {
        cgltf_texture_transform* transform = &gltf_texture_view->transform;
        materialTextureView->offset[0]     = transform->offset[0];
        materialTextureView->offset[1]     = transform->offset[1];
        materialTextureView->rotation      = transform->rotation;
        materialTextureView->scale[0]      = transform->scale[0];
        materialTextureView->scale[1]      = transform->scale[1];
    }
}

static SamplerConfig _gltf_sampler_to_sampler_config(cgltf_sampler* sampler)
{
    // convert into wgpu sampler
    WGPUSamplerDescriptor desc = {};
    desc.addressModeU          = _addressMode_GL_to_WGPU(sampler->wrap_s);
    desc.addressModeV          = _addressMode_GL_to_WGPU(sampler->wrap_t);
    // not provided by default gltf spec. defaulting to clamp
    desc.addressModeW = WGPUAddressMode_ClampToEdge;

    desc.minFilter    = _minFilter_GL_to_WGPU(sampler->min_filter);
    desc.magFilter    = _magFilter_GL_to_WGPU(sampler->mag_filter);
    desc.mipmapFilter = _mipmapFilter_GL_to_WGPU(sampler->min_filter);

    desc.lodMinClamp   = 0.0f;
    desc.lodMaxClamp   = 32.0f; // webgpu spec default
    desc.maxAnisotropy = 1;     // default

    return Graphics_SamplerConfigFromDesciptor(&desc);
}

static void _gltf_image_to_texture(cgltf_image* image, Texture* texture)
{
    log_trace("processing image %s", image->name);
    if (image->uri) {
        log_trace("\turi: %s", image->uri);
        Texture::initFromFile(gctx, texture, image->uri, true);
    } else {
        const u8* data = cgltf_buffer_view_data(image->buffer_view);
        if (data == NULL) {
            log_error("image has no uri or data");
            return;
        }
        Texture::initFromBuff(gctx, texture, data, image->buffer_view->size);
    }
}

// appends scene ids to vector
static void gltf_ProcessData(cgltf_data* data)
{

    { // gltf data stats
        log_trace("gltf data stats:");
        log_trace("file: %s", data->file_type == cgltf_file_type_invalid ? "invalid" :
                              data->file_type == cgltf_file_type_gltf    ? "gltf" :
                                                                           "glb");
        log_trace("buffers: %d", data->buffers_count);
        log_trace("buffer views: %d", data->buffer_views_count);
        log_trace("accessors: %d", data->accessors_count);
        log_trace("images: %d", data->images_count);
        log_trace("samplers: %d", data->samplers_count);
        log_trace("textures: %d", data->textures_count);
        log_trace("materials: %d", data->materials_count);
        log_trace("meshes: %d", data->meshes_count);
        log_trace("nodes: %d", data->nodes_count);
        log_trace("scenes: %d", data->scenes_count);
        log_trace("lights: %d", data->lights_count);
        log_trace("cameras: %d", data->cameras_count);
    }

    // Textures
    for (u32 i = 0; i < data->textures_count; ++i) {
        cgltf_texture* gltf_texture = &data->textures[i];
        log_trace("processing texture %d %s", i, gltf_texture->name);

        // create new texture
        R_Texture* rTexture = Component_CreateTexture();

        // assign sampler config
        rTexture->samplerConfig
          = _gltf_sampler_to_sampler_config(gltf_texture->sampler);

        // assign image data
        _gltf_image_to_texture(gltf_texture->image, &rTexture->gpuTexture);

        // add to map
        ASSERT(rTextureMap.find(gltf_texture) == rTextureMap.end());
        rTextureMap[gltf_texture] = rTexture->id;
    }

    { // material
#define GET_TEXTURE(gltf_texture) ((gltf_texture) ? rTextureMap[(gltf_texture)] : 0)
        for (u32 i = 0; i < data->materials_count; ++i) {
            cgltf_material* material = &data->materials[i];
            // only support pbr metallic roughness for now
            ASSERT(material->has_pbr_metallic_roughness);
            cgltf_pbr_metallic_roughness* mr_config = &material->pbr_metallic_roughness;

            R_MaterialConfig materialConfig = {};
            materialConfig.material_type    = SG_MATERIAL_PBR;

            R_Material* sgMat = Component_CreateMaterial(gctx, &materialConfig);

            // set mr properties
            {
                struct MaterialUniforms matUniforms = {};
                matUniforms.baseColor = glm::make_vec4(mr_config->base_color_factor);
                matUniforms.emissiveFactor = glm::make_vec3(material->emissive_factor);
                matUniforms.metallic       = mr_config->metallic_factor;
                matUniforms.roughness      = mr_config->roughness_factor;
                matUniforms.normalFactor
                  = (material->normal_texture.texture ? material->normal_texture.scale :
                                                        1.0f);
                matUniforms.aoFactor = (material->occlusion_texture.texture ?
                                          material->occlusion_texture.scale :
                                          1.0f);

                R_Material::setBinding(sgMat, 0, R_BIND_UNIFORM, &matUniforms,
                                       sizeof(matUniforms));

                // bind textures and samplers
                // TODO: how does renderer know what defeault texture to use if
                // texture is not provided?
                // TODO: how to include texture view data?

                R_Material::setTextureAndSamplerBinding(
                  sgMat, 1, GET_TEXTURE(mr_config->base_color_texture.texture),
                  opaqueWhitePixel.view);

                R_Material::setTextureAndSamplerBinding(
                  sgMat, 3, GET_TEXTURE(material->normal_texture.texture),
                  defaultNormalPixel.view);

                R_Material::setTextureAndSamplerBinding(
                  sgMat, 5, GET_TEXTURE(material->occlusion_texture.texture),
                  opaqueWhitePixel.view);

                R_Material::setTextureAndSamplerBinding(
                  sgMat, 7, GET_TEXTURE(mr_config->metallic_roughness_texture.texture),
                  opaqueWhitePixel.view);

                R_Material::setTextureAndSamplerBinding(
                  sgMat, 9, GET_TEXTURE(material->emissive_texture.texture),
                  transparentBlackPixel.view);
            }

            // TODO: how to handle texture views
            UNUSED_FUNCTION(_gltf_texture_view_to_MaterialTextureView);

            // assign textures
            // sgMat->baseColorTexture
            //   = GET_TEXTURE(mr_config->base_color_texture.texture);
            // sgMat->metallicRoughnessTexture
            //   = GET_TEXTURE(mr_config->metallic_roughness_texture.texture);
            // sgMat->normalTexture
            //   = GET_TEXTURE(material->normal_texture.texture);

            // assign texture views
            // _gltf_texture_view_to_MaterialTextureView(
            //   &mr_config->base_color_texture, &sgMat->baseColorTextureView);
            // _gltf_texture_view_to_MaterialTextureView(
            //   &mr_config->metallic_roughness_texture,
            //   &sgMat->metallicRoughnessTextureView);
            // _gltf_texture_view_to_MaterialTextureView(
            //   &material->normal_texture, &sgMat->normalTextureView);

            // finished initializing material, add to map
            ASSERT(rMaterialMap.find(material) == rMaterialMap.end());
            rMaterialMap[material] = sgMat->id;
        }
#undef GET_TEXTURE
    }

    // process scene -- node transform hierarchy
    for (cgltf_size i = 0; i < data->scenes_count; ++i) {
        cgltf_scene* gltf_scene = &data->scenes[i];

        R_Transform* sg_root = Component_CreateTransform();
        sg_root->name        = gltf_scene->name;
        sceneIDs[i]          = sg_root->id;

        log_trace("processing scene: %s with %d nodes", gltf_scene->name,
                  gltf_scene->nodes_count);

        for (cgltf_size j = 0; j < gltf_scene->nodes_count; ++j) {
            cgltf_node* node = gltf_scene->nodes[j];

            // process node
            gltf_ProcessNode(sg_root, node);
        }
    }
}

static void _Test_Gltf_OnInit(GraphicsContext* ctx, GLFWwindow* window)
{
    gctx = ctx;
    log_trace("gltf test init");

    Arena::init(&frameArena, MEGABYTE);

    // TODO: initialize these default pixel textures in graphics context init
    Texture::initSinglePixel(gctx, &opaqueWhitePixel, whitePixel);
    Texture::initSinglePixel(gctx, &transparentBlackPixel, blackPixel);
    Texture::initSinglePixel(gctx, &defaultNormalPixel, normalPixel);

    // TODO remove tmp ----------------
    testXform = Component_CreateTransform();
    R_Transform::pos(testXform, 3.0f * VEC_RIGHT);

    // --------------------------------

    cgltf_options options = {};
    // Use our own memory allocation functions
    options.memory.alloc_func
      = [](void* user, cgltf_size size) { return ALLOCATE_BYTES(void, size); };
    options.memory.free_func = [](void* user, void* ptr) { FREE(ptr); };

    cgltf_data* data     = NULL;
    const char* filename = "./assets/DamagedHelmet.glb";
    cgltf_result result  = cgltf_parse_file(&options, filename, &data);
    if (result == cgltf_result_success) {
        log_trace("gltf parsed successfully");
        cgltf_result buffers_result = cgltf_load_buffers(&options, data, filename);
        if (buffers_result != cgltf_result_success) {
            log_error("failed to load buffers");
            return;
        }
        log_trace("gltf buffer data loaded");

        // allocate memory to store scene IDs
        numScenes = data->scenes_count;
        sceneIDs  = ALLOCATE_COUNT(SG_ID, numScenes);

        gltf_ProcessData(data);

        for (u32 i = 0; i < data->scenes_count; ++i) {
            printf("------scene %d------\n", i);
            R_Transform::print(Component_GetXform(sceneIDs[i]));
        }

        cgltf_free(data);
    }
}

static void OnUpdate(f32 dt)
{
    // rotate the test transform
    R_Transform::rotateOnWorldAxis(testXform, VEC_UP, dt * 0.4f);
    // rotate the root
    R_Transform::rotateOnLocalAxis(Component_GetXform(sceneIDs[0]), VEC_UP, -dt * 0.1f);
}

static void OnRender(glm::mat4 proj, glm::mat4 view, glm::vec3 camPos)
{

    // Update all transforms
    R_Transform::rebuildMatrices(Component_GetXform(sceneIDs[0]), &frameArena);

    R_Transform::print(Component_GetXform(sceneIDs[0]), 0);

    GraphicsContext::prepareFrame(gctx);
    WGPURenderPassEncoder renderPass
      = wgpuCommandEncoderBeginRenderPass(gctx->commandEncoder, &gctx->renderPassDesc);

    // write per-frame uniforms
    f32 time                    = (f32)glfwGetTime();
    FrameUniforms frameUniforms = {};
    frameUniforms.projectionMat = proj;
    frameUniforms.viewMat       = view;
    frameUniforms.projViewMat   = frameUniforms.projectionMat * frameUniforms.viewMat;
    frameUniforms.camPos        = camPos;
    frameUniforms.dirLight      = VEC_FORWARD;
    frameUniforms.time          = time;

    // log_debug("geo num instances: %d", R_Geometry::numInstances(geo));
    // Test render loop
    // use bool Component_RenderPipelineIter(size_t* i, R_RenderPipeline**
    // renderPipeline);
    // TODO move into renderer.cpp
    R_RenderPipeline* renderPipeline = NULL;
    size_t rpIndex                   = 0;
    while (Component_RenderPipelineIter(&rpIndex, &renderPipeline)) {
        // log_trace("drawing materials for render pipeline: %d",
        //           renderPipeline->rid);

        WGPURenderPipeline gpuPipeline = renderPipeline->pipeline.pipeline;
        // TODO: cache the bindGroupLayout in the pipeline after creation (it
        // will never change)
        WGPUBindGroupLayout perMaterialLayout
          = wgpuRenderPipelineGetBindGroupLayout(gpuPipeline, PER_MATERIAL_GROUP);
        WGPUBindGroupLayout perDrawLayout
          = wgpuRenderPipelineGetBindGroupLayout(gpuPipeline, PER_DRAW_GROUP);

        // set shader
        wgpuRenderPassEncoderSetPipeline(renderPass, gpuPipeline);

        // set frame bind group (needs to be set per renderpipeline as long as
        // we use implicit layout:auto)
        wgpuQueueWriteBuffer(gctx->queue, renderPipeline->pipeline.frameUniformBuffer,
                             0, &frameUniforms, sizeof(frameUniforms));
        wgpuRenderPassEncoderSetBindGroup(renderPass, PER_FRAME_GROUP,
                                          renderPipeline->pipeline.frameGroup, 0, NULL);

        // per-material render loop
        size_t materialIdx    = 0;
        R_Material* rMaterial = NULL;

        while (
          R_RenderPipeline::materialIter(renderPipeline, &materialIdx, &rMaterial)) {
            // get material
            // log_trace("drawing material: %d", rMaterial->id);

            ASSERT(rMaterial && rMaterial->pipelineID == renderPipeline->rid);

            // TODO: figure out textures / texture views...

            // set per_material bind group
            R_Material::rebuildBindGroup(rMaterial, gctx, perMaterialLayout);
            wgpuRenderPassEncoderSetBindGroup(renderPass, PER_MATERIAL_GROUP,
                                              rMaterial->bindGroup, 0, NULL);

            // iterate over material primitives
            size_t primitiveIdx           = 0;
            Material_Primitive* primitive = NULL;
            while (R_Material::primitiveIter(rMaterial, &primitiveIdx, &primitive)) {
                u32 numInstances = Material_Primitive::numInstances(primitive);
                if (numInstances == 0) continue;
                Material_Primitive::rebuildBindGroup(gctx, primitive, perDrawLayout,
                                                     &frameArena);

                // set model bind group
                wgpuRenderPassEncoderSetBindGroup(renderPass, PER_DRAW_GROUP,
                                                  primitive->bindGroup, 0, NULL);

                R_Geometry* geo = Component_GetGeometry(primitive->geoID);
                ASSERT(geo);

                // set vertex attributes
                // TODO: move into renderer, and consider separate buffer for
                // each attribute to handle case where some vertex attributes
                // are missing (tangent, color, etc)
                // TODO: what happens if a vertex attribute is not set in for
                // the shader?
                wgpuRenderPassEncoderSetVertexBuffer(
                  renderPass, 0, geo->gpuVertexBuffer, 0,
                  sizeof(f32) * geo->numVertices * 3);

                auto normalsOffset = sizeof(f32) * geo->numVertices * 3;

                wgpuRenderPassEncoderSetVertexBuffer(
                  renderPass, 1, geo->gpuVertexBuffer, normalsOffset,
                  sizeof(f32) * geo->numVertices * 3);

                auto texcoordsOffset = sizeof(f32) * geo->numVertices * 6;

                wgpuRenderPassEncoderSetVertexBuffer(
                  renderPass, 2, geo->gpuVertexBuffer, texcoordsOffset,
                  sizeof(f32) * geo->numVertices * 2);

                size_t tangentOffset = sizeof(f32) * geo->numVertices * 8;

                wgpuRenderPassEncoderSetVertexBuffer(
                  renderPass, 3, geo->gpuVertexBuffer, tangentOffset,
                  sizeof(f32) * geo->numVertices * 4);

                // populate index buffer
                if (geo->numIndices > 0)
                    wgpuRenderPassEncoderSetIndexBuffer(renderPass, geo->gpuIndexBuffer,
                                                        WGPUIndexFormat_Uint32, 0,
                                                        geo->indexBufferDesc.size);

                // draw call (indexed)
                if (geo->numIndices > 0) {
                    wgpuRenderPassEncoderDrawIndexed(renderPass, geo->numIndices,
                                                     numInstances, 0, 0, 0);
                } else {
                    // draw call (nonindexed)
                    wgpuRenderPassEncoderDraw(renderPass, geo->numVertices,
                                              numInstances, 0, 0);
                }
            }

            // material uniforms TODO switch to pbr
            // MaterialUniforms materialUniforms = {};
            // materialUniforms.color            =
            // glm::vec4(1.0, 1.0, 1.0, 1.0);
            // // TODO: only need to write if it's stale/changed
            // wgpuQueueWriteBuffer(gctx->queue, //
            //                      material.uniformBuffer, // 0, //
            //                      &materialUniforms, sizeof(materialUniforms)
            //                      //
            // );
        }
    }

    GraphicsContext::presentFrame(gctx);

    // end of frame, clear arena
    Arena::clear(&frameArena);
}

static void OnExit()
{
    log_trace("gltf test exit");

    FREE_ARRAY(SG_ID, sceneIDs, numScenes);

    Arena::free(&frameArena);

    Texture::release(&opaqueWhitePixel);
    Texture::release(&transparentBlackPixel);
    Texture::release(&defaultNormalPixel);
}

void Test_Gltf(TestCallbacks* callbacks)
{
    *callbacks          = {};
    callbacks->onInit   = _Test_Gltf_OnInit;
    callbacks->onRender = OnRender;
    callbacks->onExit   = OnExit;
    callbacks->onUpdate = OnUpdate;
}
