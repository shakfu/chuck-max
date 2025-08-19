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
#include "graphics.h"
#include "core/log.h"
#include "core/memory.h"
#include "shaders.h"

#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <stb/stb_image.h>

#ifdef WEBGPU_BACKEND_WGPU
#include <webgpu/wgpu.h>
#endif

// static void printBackend()
// {
// #if defined(WEBGPU_BACKEND_DAWN)
//     std::cout << "Backend: Dawn" << std::endl;
// #elif defined(WEBGPU_BACKEND_WGPU)
//     std::cout << "Backend: WGPU" << std::endl;
// #elif defined(WEBGPU_BACKEND_EMSCRIPTEN)
//     std::cout << "Backend: Emscripten" << std::endl;
// #else
//     std::cout << "Backend: Unknown" << std::endl;
// #endif
// }

// static void printAdapterInfo(WGPUAdapter adapter)
// {
//     WGPUAdapterProperties properties;
//     wgpuAdapterGetProperties(adapter, &properties);

//     std::cout << "Adapter name: " << properties.name << std::endl;
//     std::cout << "Adapter vendor: " << properties.vendorName << std::endl;
//     std::cout << "Adapter deviceID: " << properties.deviceID << std::endl;
//     std::cout << "Adapter backend: " << properties.backendType << std::endl;
// }

/**
 * Utility function to get a WebGPU adapter, so that
 *     WGPUAdapter adapter = requestAdapter(options);
 * is roughly equivalent to
 *     const adapter = await navigator.gpu.requestAdapter(options);
 */
static WGPUAdapter request_adapter(WGPUInstance instance,
                                   WGPURequestAdapterOptions const* options)
{
    // A simple structure holding the local information shared with the
    // onAdapterRequestEnded callback.
    struct UserData {
        WGPUAdapter adapter = nullptr;
        bool requestEnded   = false;
    };
    UserData userData;

    // Callback called by wgpuInstanceRequestAdapter when the request returns
    // This is a C++ lambda function, but could be any function defined in the
    // global scope. It must be non-capturing (the brackets [] are empty) so
    // that it behaves like a regular C function pointer, which is what
    // wgpuInstanceRequestAdapter expects (WebGPU being a C API). The workaround
    // is to convey what we want to capture through the pUserData pointer,
    // provided as the last argument of wgpuInstanceRequestAdapter and received
    // by the callback as its last argument.
    auto onAdapterRequestEnded
      = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const* message,
           void* pUserData) {
            UserData* userData = (UserData*)pUserData;

            if (status == WGPURequestAdapterStatus_Success)
                userData->adapter = adapter;
            else
                std::cout << "Could not get WebGPU adapter: " << message << std::endl;

            userData->requestEnded = true;
        };

    // Call to the WebGPU request adapter procedure
    wgpuInstanceRequestAdapter(instance /* equivalent of navigator.gpu */, options,
                               onAdapterRequestEnded, (void*)&userData);
#ifdef __EMSCRIPTEN__
    // In the Emscripten environment, the WebGPU adapter request is asynchronous
    // while (!userData.requestEnded) emscripten_sleep(10);
    while (!userData.requestEnded) {
    }
#endif
    // In theory we should wait until onAdapterReady has been called, which
    // could take some time (what the 'await' keyword does in the JavaScript
    // code). In practice, we know that when the wgpuInstanceRequestAdapter()
    // function returns its callback has been called.
    ASSERT(userData.requestEnded);

    return userData.adapter;
}

/**
 * Utility function to get a WebGPU device, so that
 *     WGPUAdapter device = requestDevice(adapter, options);
 * is roughly equivalent to
 *     const device = await adapter.requestDevice(descriptor);
 * It is very similar to requestAdapter
 */
static WGPUDevice request_device(WGPUAdapter adapter,
                                 WGPUDeviceDescriptor const* descriptor)
{
    struct UserData {
        WGPUDevice device = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device,
                                   char const* message, void* pUserData) {
        UserData& userData = *reinterpret_cast<UserData*>(pUserData);
        if (status == WGPURequestDeviceStatus_Success) {
            userData.device = device;
        } else {
            std::cout << "Could not get WebGPU device: " << message << std::endl;
        }
        userData.requestEnded = true;
    };

    wgpuAdapterRequestDevice(adapter, descriptor, onDeviceRequestEnded,
                             (void*)&userData);

#ifdef __EMSCRIPTEN__
    // while (!userData.requestEnded) emscripten_sleep(10);
    while (!userData.requestEnded) {
    }
#endif

    ASSERT(userData.requestEnded);

    return userData.device;
}

static void on_device_error(WGPUErrorType type, char const* message,
                            void* /* pUserData */)
{
    log_error("Uncaptured device error: type %d (%s)", type, message);
    exit(EXIT_FAILURE); // intentionally crash here, even in release mode, so we can see
                        // shader compilation errors
};

#ifdef WGPU_OLD_VERSION
static bool createSwapChain(GraphicsContext* context, u32 width, u32 height)
{
    // ensure previous swap chain has been released
    ASSERT(context->swapChain == NULL);

    WGPUSwapChainDescriptor swap_chain_desc = {
        NULL,                              // nextInChain
        "The default swap chain",          // label
        WGPUTextureUsage_RenderAttachment, // usage
        context->swapChainFormat,          // format
        width,                             // width
        height,                            // height
        WGPUPresentMode_Fifo,              // presentMode (vsynced)
    };
    context->swapChain
      = wgpuDeviceCreateSwapChain(context->device, context->surface, &swap_chain_desc);

    if (!context->swapChain) return false;
    return true;
}
#endif

static void logWGPULimits(WGPULimits const* limits)
{
    log_trace("Supported limits:");
    log_trace("maxBindGroups: %d", limits->maxBindGroups);
    log_trace("maxBindingsPerBindGroup: %d", limits->maxBindingsPerBindGroup);
    log_trace("minUniformBufferOffsetAlignment %d",
              limits->minUniformBufferOffsetAlignment);
    log_trace("minStorageBufferOffsetAlignment %d",
              limits->minStorageBufferOffsetAlignment);
}

static void GraphicsContext_ConfigureSurface(GraphicsContext* gctx, u32 window_width,
                                             u32 window_height)
{

    WGPUSurfaceConfiguration surface_config = {};
    surface_config.device                   = gctx->device;
    surface_config.format                   = gctx->surface_format;
    surface_config.usage                    = WGPUTextureUsage_RenderAttachment;
    surface_config.width                    = window_width;
    surface_config.height                   = window_height;
    surface_config.presentMode              = WGPUPresentMode_Fifo; // vsynced

    wgpuSurfaceConfigure(gctx->surface, &surface_config);
}

static const char* GraphicsContext_BackendTypeToString(WGPUBackendType backend)
{
    switch (backend) {
        case WGPUBackendType_Undefined: return "Undefined";
        case WGPUBackendType_Null: return "Null";
        case WGPUBackendType_WebGPU: return "WebGPU";
        case WGPUBackendType_D3D11: return "D3D11";
        case WGPUBackendType_D3D12: return "D3D12";
        case WGPUBackendType_Metal: return "Metal";
        case WGPUBackendType_Vulkan: return "Vulkan";
        case WGPUBackendType_OpenGL: return "OpenGL";
        case WGPUBackendType_OpenGLES: return "OpenGLES";
        case WGPUBackendType_Force32: return "Force32";
    }
    return "Unknown";
}

static const char* GraphicsContext_AdapterTypeToString(WGPUAdapterType type)
{
    switch (type) {
        case WGPUAdapterType_DiscreteGPU: return "Discrete GPU";
        case WGPUAdapterType_IntegratedGPU: return "Integrated GPU";
        case WGPUAdapterType_CPU: return "CPU";
        case WGPUAdapterType_Unknown: return "Unknown";
        case WGPUAdapterType_Force32: return "Force32";
    }
    return "Unknown";
}

bool GraphicsContext::init(GraphicsContext* context, GLFWwindow* window)
{
    Arena::init(&context->frame_arena,
                1); // init to 1 byte to test resize doesn't crash

    log_trace("initializing WebGPU context");
    ASSERT(context->device == NULL);

#ifdef __EMSCRIPTEN__
    // See
    // https://github.com/emscripten-core/emscripten/blob/main/system/lib/webgpu/webgpu.cpp#L22
    // instance descriptor not implemented yet (as of 4/8/2024)
    // must pass nullptr instead
    WGPUInstance instance = wgpuCreateInstance(NULL);
#else
    // WGPU
    WGPUInstanceExtras instanceExtras = {};
    instanceExtras.chain.sType        = (WGPUSType)WGPUSType_InstanceExtras;
    instanceExtras.backends
      = WGPUInstanceBackend_All; // enables all backends, allows falling back to openGL
                                 // if vulkan/d3d12/metal not available

    WGPUInstanceDescriptor instanceDescriptor = {};
    instanceDescriptor.nextInChain            = &instanceExtras.chain;

    WGPUInstance instance = wgpuCreateInstance(&instanceDescriptor);
#endif
    defer(WGPU_RELEASE_RESOURCE(Instance, instance));

    if (!instance) return false;
    log_trace("WebGPU instance created");

    context->surface = glfwCreateWindowWGPUSurface(instance, window);
    if (!context->surface) return false;
    // context->window = window;
    log_trace("WebGPU surface created");

    WGPURequestAdapterOptions adapterOpts = {};
    adapterOpts.compatibleSurface         = context->surface;
    adapterOpts.powerPreference           = WGPUPowerPreference_HighPerformance;
    WGPUAdapter adapter                   = request_adapter(instance, &adapterOpts);
    if (!adapter) return false;
    defer(WGPU_RELEASE_RESOURCE(Adapter, adapter));
    log_trace("adapter created");

    { // log adapter info
        WGPUAdapterInfo adapter_info;
        wgpuAdapterGetInfo(adapter, &adapter_info);
        log_info("Adapter vendor: %s", adapter_info.vendor);
        log_info("Adapter architecture: %s", adapter_info.architecture);
        log_info("Adapter device: %s", adapter_info.device);
        log_info("Adapter description: %s", adapter_info.description);
        log_info("Adapter backend type: %s",
                 GraphicsContext_BackendTypeToString(adapter_info.backendType));
        log_info("Adapter type: %s",
                 GraphicsContext_AdapterTypeToString(adapter_info.adapterType));
        log_info("Adapter vendor ID: %d", adapter_info.vendorID);
        log_info("Adapter device ID: %d", adapter_info.deviceID);
    }

    // set required limits to max supported
    WGPURequiredLimits requiredLimits = {};
#ifdef WEBGPU_BACKEND_WGPU
    WGPUSupportedLimits supportedLimits = {};
    bool success = wgpuAdapterGetLimits(adapter, &supportedLimits);
    UNUSED_VAR(success);
    ASSERT(success);
    // copy supported limits into context
    context->limits = supportedLimits.limits;
    logWGPULimits(&context->limits);

    requiredLimits.limits = context->limits;

    // clang-format off
    WGPUFeatureName requiredFeatures[] = {
        (WGPUFeatureName)WGPUNativeFeature_VertexWritableStorage,
        // (WGPUFeatureName) WGPUNativeFeature_TextureAdapterSpecificFormatFeatures,  // allows passing 32-bit float textures to texture_2d<f32> in shaders

        // enabling this feature still doesn't work
        WGPUFeatureName_Float32Filterable // needed to sample 32-bit float textures in shaders
    };
    // clang-format on
    const u32 requiredFeaturesCount = ARRAY_LENGTH(requiredFeatures);
    log_trace("required features: %d", ARRAY_LENGTH(requiredFeatures));
#else
    const u32 requiredFeaturesCount   = 0;
    WGPUFeatureName* requiredFeatures = NULL;
#endif

    // see your machine's supported features here: https://webgpureport.org/
    WGPUDeviceDescriptor deviceDescriptor = {};
    deviceDescriptor.label                = "ChuGL Device";
    deviceDescriptor.requiredFeatureCount = requiredFeaturesCount;
    deviceDescriptor.requiredFeatures     = requiredFeatures;
    deviceDescriptor.requiredLimits       = &requiredLimits;
    deviceDescriptor.defaultQueue         = { NULL, "ChuGL queue" };
    deviceDescriptor.deviceLostCallback
      = [](WGPUDeviceLostReason reason, char const* message, void* /* pUserData */) {
            log_error("Device lost: reason %d", reason);
            if (message) log_error(" (%s)", message);
        };
    deviceDescriptor.deviceLostUserdata          = NULL;
    deviceDescriptor.uncapturedErrorCallbackInfo = {
        NULL, on_device_error, NULL /* pUserData */
    };

    context->device = request_device(adapter, &deviceDescriptor);
    if (!context->device) return false;
    log_trace("device created");

    { // set debug callbacks
      // #if defined(WEBGPU_BACKEND_WGPU)
      //         wgpuSetLogLevel(WGPULogLevel_Warn);
      //         wgpuSetLogCallback(
      //           [](WGPULogLevel level, char const* message, void* /* userdata */) {
      //               log_error("wgpu log [%d]: %s", level, message);
      //           },
      //           NULL);

        //         log_trace("Using wgpu version %d", wgpuGetVersion());
        // #endif
    }

    context->queue = wgpuDeviceGetQueue(context->device);
    if (!context->queue) return false;

    // determine swapchain format
    // note: we try to always pick an 8unorm format
    {
        WGPUSurfaceCapabilities surface_capabilities;
        wgpuSurfaceGetCapabilities(context->surface, adapter, &surface_capabilities);
        context->surface_format = WGPUTextureFormat_Undefined;
        for (int i = 0; i < surface_capabilities.formatCount; i++) {
            WGPUTextureFormat format = surface_capabilities.formats[i];
            if (format == WGPUTextureFormat_BGRA8Unorm
                || format == WGPUTextureFormat_RGBA8Unorm) {
                context->surface_format = format;
                break;
            }
        }
        // error handle if we didn't find a unorm non-srgb format
        if (context->surface_format == WGPUTextureFormat_Undefined) {
            log_fatal("Surface does not accept any supported texture formats.");
            ASSERT(false);
            return false;
        }
    }
    log_info("Surface texture format: %s",
             G_Util::textureFormatToString(context->surface_format));

    int framebuffer_width = 1, framebuffer_height = 1;
    glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
    GraphicsContext_ConfigureSurface(context, (u32)framebuffer_width,
                                     (u32)framebuffer_height);

    // init mip map generator
    MipMapGenerator_init(context);

    { // create default resources
        WGPUSamplerDescriptor samplerDesc = {};
        samplerDesc.label                 = "Default Comparison Sampler";
        samplerDesc.addressModeU          = WGPUAddressMode_ClampToEdge;
        samplerDesc.addressModeV          = WGPUAddressMode_ClampToEdge;
        samplerDesc.addressModeW          = WGPUAddressMode_ClampToEdge;
        samplerDesc.minFilter             = WGPUFilterMode_Nearest;
        samplerDesc.magFilter             = WGPUFilterMode_Linear;
        samplerDesc.mipmapFilter          = WGPUMipmapFilterMode_Nearest;
        samplerDesc.compare               = WGPUCompareFunction_Less;
        samplerDesc.lodMinClamp           = 0.0f;
        samplerDesc.lodMaxClamp           = 1.0f;
        samplerDesc.maxAnisotropy         = 1;
        context->shadow_comparison_sampler
          = wgpuDeviceCreateSampler(context->device, &samplerDesc);

        WGPUTextureDescriptor desc = {};
        desc.label                 = "Sentinel Spotlight Depth2DArray";
        desc.size                  = { 1, 1, context->limits.maxTextureArrayLayers };
        desc.mipLevelCount         = 1;
        desc.sampleCount           = 1;
        desc.dimension             = WGPUTextureDimension_2D;
        desc.format                = WGPUTextureFormat_Depth32Float;
        desc.usage
          = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_TextureBinding;
        context->sentinel_spotlight_depth_2d_array
          = wgpuDeviceCreateTexture(context->device, &desc);

        desc.label = "Sentinel Dirlight Depth2DArray";
        context->sentinel_dirlight_depth_2d_array
          = wgpuDeviceCreateTexture(context->device, &desc);
    }

    return true;
}

static WGPUTextureView GraphicsContext_GetNextSurfaceTextureView(GraphicsContext* gctx)

{
    // Get the surface texture
    wgpuSurfaceGetCurrentTexture(gctx->surface, &gctx->surface_texture);
    if (gctx->surface_texture.status != WGPUSurfaceGetCurrentTextureStatus_Success) {
        return nullptr;
    }

    // Create a view for this surface texture
    WGPUTextureViewDescriptor viewDescriptor = {};
    viewDescriptor.label                     = "Surface texture view";
    viewDescriptor.format         = wgpuTextureGetFormat(gctx->surface_texture.texture);
    viewDescriptor.dimension      = WGPUTextureViewDimension_2D;
    viewDescriptor.baseMipLevel   = 0;
    viewDescriptor.mipLevelCount  = 1;
    viewDescriptor.baseArrayLayer = 0;
    viewDescriptor.arrayLayerCount = 1;
    viewDescriptor.aspect          = WGPUTextureAspect_All;
    WGPUTextureView targetView
      = wgpuTextureCreateView(gctx->surface_texture.texture, &viewDescriptor);

    return targetView;
}

bool GraphicsContext::prepareFrame(GraphicsContext* ctx)
{
    WGPU_RELEASE_RESOURCE(TextureView, ctx->backbufferView);
    WGPU_RELEASE_RESOURCE(CommandEncoder, ctx->commandEncoder);

    if (ctx->window_minimized) return false;

    // get target texture view
    ctx->backbufferView = GraphicsContext_GetNextSurfaceTextureView(ctx);
    if (!ctx->backbufferView) return false;

    // initialize encoder
    WGPUCommandEncoderDescriptor encoderDesc = {};
    ctx->commandEncoder = wgpuDeviceCreateCommandEncoder(ctx->device, &encoderDesc);
    return true;
}

void GraphicsContext::presentFrame(GraphicsContext* ctx)
{
    // submit
    WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
    WGPUCommandBuffer command
      = wgpuCommandEncoderFinish(ctx->commandEncoder, &cmdBufferDescriptor);

    // Finally submit the command queue
    wgpuQueueSubmit(ctx->queue, 1, &command);

    // present
#ifndef __EMSCRIPTEN__
    wgpuSurfacePresent(ctx->surface);
#endif

    // free surface texture
    // #ifndef WEBGPU_BACKEND_WGPU
    // We no longer need the texture, only its view
    // (NB: with wgpu-native, surface textures must not be manually released)
    WGPU_RELEASE_RESOURCE(Texture, ctx->surface_texture.texture);
    // #endif // WEBGPU_BACKEND_WGPU

    WGPU_RELEASE_RESOURCE(CommandBuffer, command);

    // clear frame arena
    Arena::clear(&ctx->frame_arena);

    // poll device
#if defined(WEBGPU_BACKEND_WGPU)
    wgpuDevicePoll(ctx->device, false, nullptr);
#endif
}

void GraphicsContext::resize(GraphicsContext* ctx, u32 width, u32 height)
{
    if (width == 0 || height == 0) {
        ctx->window_minimized = true;
        return;
    } else {
        ctx->window_minimized = false;
    }

    GraphicsContext_ConfigureSurface(ctx, width, height);
}

void GraphicsContext::release(GraphicsContext* ctx)
{
    // mip map gen
    MipMapGenerator_release();

    wgpuSurfaceUnconfigure(ctx->surface);
    wgpuSurfaceRelease(ctx->surface);

    wgpuQueueRelease(ctx->queue);
    wgpuDeviceRelease(ctx->device);

    *ctx = {};
}

// void VertexBufferLayout::init(VertexBufferLayout* layout, u8 attribute_count,
//                               u32* attribute_strides)
// {
//     WGPUVertexFormat format = WGPUVertexFormat_Undefined;

//     for (u8 i = 0; i < attribute_count; i++) {
//         // determine format
//         switch (attribute_strides[i]) {
//             case 0: return; // assume first 0 means end of list
//             case 1: format = WGPUVertexFormat_Float32; break;
//             case 2: format = WGPUVertexFormat_Float32x2; break;
//             case 3: format = WGPUVertexFormat_Float32x3; break;
//             case 4: format = WGPUVertexFormat_Float32x4; break;
//             default: format = WGPUVertexFormat_Undefined; break;
//         }

//         layout->attribute_count = i + 1;

//         layout->attributes[i] = {
//             format, // format
//             0,      // offset
//             i,      // shader location
//         };

//         layout->layouts[i] = {
//             sizeof(f32) * attribute_strides[i], // arrayStride
//             WGPUVertexStepMode_Vertex,          // stepMode
//             1,                                  // attribute count
//             layout->attributes + i,             // vertexAttribute
//         };
//     }
// }

static u64 wgpuVertexFormatSize(WGPUVertexFormat format)
{
    switch (format) {
        case WGPUVertexFormat_Uint8x2:
        case WGPUVertexFormat_Sint8x2:
        case WGPUVertexFormat_Unorm8x2:
        case WGPUVertexFormat_Snorm8x2: {
            return 2;
        } break;
        case WGPUVertexFormat_Uint8x4:
        case WGPUVertexFormat_Sint8x4:
        case WGPUVertexFormat_Unorm8x4:
        case WGPUVertexFormat_Snorm8x4: {
            return 4;
        } break;
        case WGPUVertexFormat_Uint16x2:
        case WGPUVertexFormat_Sint16x2:
        case WGPUVertexFormat_Float16x2:
        case WGPUVertexFormat_Unorm16x2:
        case WGPUVertexFormat_Snorm16x2: {
            return 4;
        } break;
        case WGPUVertexFormat_Uint16x4:
        case WGPUVertexFormat_Sint16x4:
        case WGPUVertexFormat_Float16x4:
        case WGPUVertexFormat_Unorm16x4:
        case WGPUVertexFormat_Snorm16x4: {
            return 8;
        } break;
        case WGPUVertexFormat_Float32:
        case WGPUVertexFormat_Uint32:
        case WGPUVertexFormat_Sint32: {
            return 4;
        } break;
        case WGPUVertexFormat_Float32x2:
        case WGPUVertexFormat_Uint32x2:
        case WGPUVertexFormat_Sint32x2: {
            return 8;
        } break;
        case WGPUVertexFormat_Float32x3:
        case WGPUVertexFormat_Uint32x3:
        case WGPUVertexFormat_Sint32x3: {
            return 12;
        } break;
        case WGPUVertexFormat_Float32x4:
        case WGPUVertexFormat_Uint32x4:
        case WGPUVertexFormat_Sint32x4: {
            return 16;
        } break;
        default: ASSERT(false); return 0;
    }
}

void VertexBufferLayout::init(VertexBufferLayout* layout, u8 format_count,
                              WGPUVertexFormat* formats)
{
    layout->attribute_count = format_count;
    for (u32 i = 0; i < format_count; i++) {
        if (formats[i] == 0) {
            layout->attribute_count = i;
            return;
        }

        layout->attributes[i] = {
            formats[i], // format
            0,          // offset
            i,          // shader location
        };

        layout->layouts[i] = {
            wgpuVertexFormatSize(formats[i]), // arrayStride
            WGPUVertexStepMode_Vertex,        // stepMode
            1,                                // attribute count
            layout->attributes + i,           // vertexAttribute
        };
    }
}

// ============================================================================
// Blend State
// ============================================================================

WGPUBlendState G_createBlendState(bool enableBlend)
{
    WGPUBlendComponent descriptor = {};
    descriptor.operation          = WGPUBlendOperation_Add;

    if (enableBlend) {
        // a*src + (1-a)*dst
        descriptor.srcFactor = WGPUBlendFactor_SrcAlpha;
        descriptor.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
    } else {
        // 1*src + 0*dst
        descriptor.srcFactor = WGPUBlendFactor_One;
        descriptor.dstFactor = WGPUBlendFactor_Zero;
    }

    return {
        descriptor, // color
        descriptor, // alpha
    };
}

// ============================================================================
// Pipeline and RenderPass State Helpers
// ============================================================================

WGPUDepthStencilState G_createDepthStencilState(WGPUTextureFormat format,
                                                bool enableDepthWrite)
{
    WGPUStencilFaceState stencil = {};
    stencil.compare              = WGPUCompareFunction_Always;
    stencil.failOp               = WGPUStencilOperation_Keep;
    stencil.depthFailOp          = WGPUStencilOperation_Keep;
    stencil.passOp               = WGPUStencilOperation_Keep;

    WGPUDepthStencilState depthStencilState = {};
    depthStencilState.depthWriteEnabled     = enableDepthWrite;
    depthStencilState.format                = format;
    // using WGPUCompareFunction_LessEqual so skybox is rendered correctly
    depthStencilState.depthCompare        = WGPUCompareFunction_LessEqual;
    depthStencilState.stencilFront        = stencil;
    depthStencilState.stencilBack         = stencil;
    depthStencilState.stencilReadMask     = 0xFFFFFFFF;
    depthStencilState.stencilWriteMask    = 0xFFFFFFFF;
    depthStencilState.depthBias           = 0;
    depthStencilState.depthBiasSlopeScale = 0.0f;
    depthStencilState.depthBiasClamp      = 0.0f;

    return depthStencilState;
}

WGPUMultisampleState G_createMultisampleState(u8 sample_count)
{
    WGPUMultisampleState ms   = {};
    ms.count                  = sample_count;
    ms.mask                   = 0xFFFFFFFF;
    ms.alphaToCoverageEnabled = false;
    return ms;
}

WGPUShaderModule G_createShaderModule(GraphicsContext* gctx, const char* code,
                                      const char* label)
{
    std::string preprocessed_code = Shaders_genSource(code);
    WGPUShaderModuleWGSLDescriptor desc
      = { { NULL, WGPUSType_ShaderModuleWGSLDescriptor }, // base class
          preprocessed_code.c_str() };

    WGPUShaderModuleDescriptor moduleDesc = {};
    moduleDesc.label                      = label;
    moduleDesc.nextInChain                = &desc.chain;

    WGPUShaderModule module = wgpuDeviceCreateShaderModule(gctx->device, &moduleDesc);

    // NOT IMPLEMENTED IN CURRENT VERSION OF WGPU
    // #ifdef CHUGL_DEBUG
    // wgpuShaderModuleGetCompilationInfo(module, G_ShaderCompilationCallback, NULL);
    // #endif

    return module;
}

// ============================================================================
// MipMapGenerator (static)
// ============================================================================

#define NUMBER_OF_TEXTURE_FORMATS WGPUTextureFormat_ASTC12x12UnormSrgb // 94

/// @brief Determines the number of mip levels needed for a full mip chain
u32 G_mipLevels(int width, int height)
{
    return (u32)(floor((float)(log2(MAX(width, height))))) + 1;
}

// calculate number of mip levels based on downscale limit
u32 G_mipLevelsLimit(u32 w, u32 h, u32 downscale_limit)
{
    if (downscale_limit >= w || downscale_limit >= h) return 1;

    u32 mip_levels = G_mipLevels(w, h) - G_mipLevels(downscale_limit, downscale_limit);

    ASSERT(mip_levels > 0);
    return mip_levels;
}

G_MipSize G_mipLevelSize(int width, int height, u32 mip_level)
{
    return { MAX(1u, (u32)(glm::floor(float(width) / glm::pow(2.0, mip_level)))),
             MAX(1u, (u32)(glm::floor(float(height) / glm::pow(2.0, mip_level)))) };
}

int G_componentsPerTexel(WGPUTextureFormat format)
{
    switch (format) {
        case WGPUTextureFormat_RGBA8Unorm:
        case WGPUTextureFormat_RGBA16Float:
        case WGPUTextureFormat_RGBA32Float: {
            return 4;
        } break;
        case WGPUTextureFormat_R32Float: {
            return 1;
        } break;
        default: ASSERT(false);
    }
    return 0;
}

int G_bytesPerTexel(WGPUTextureFormat format)
{
    switch (format) {
        case WGPUTextureFormat_RGBA8Unorm: return 4;
        case WGPUTextureFormat_RGBA16Float: return 8;
        case WGPUTextureFormat_RGBA32Float: return 16;
        case WGPUTextureFormat_R32Float: return 4;
        default: ASSERT(false);
    }
    return 0;
}

// TODO make part of GraphicsContext and cleanup
struct {
    WGPUSampler sampler;

    // Pipeline for every texture format used.
    // TODO: can layout be shared?
    WGPUBindGroupLayout pipeline_layouts[(u32)NUMBER_OF_TEXTURE_FORMATS];
    WGPURenderPipeline pipelines[(u32)NUMBER_OF_TEXTURE_FORMATS];

    // Vertex state and Fragment state are shared between all pipelines
    WGPUVertexState vertexState;
    WGPUFragmentState fragmentState;

    bool initialized = false;
} mip_map_generator = {};

void MipMapGenerator_init(GraphicsContext* ctx)
{
    if (mip_map_generator.initialized) return;
    mip_map_generator.initialized = true;

    // Create sampler
    WGPUSamplerDescriptor sampler_desc = {};
    sampler_desc.label                 = "mip map sampler";
    sampler_desc.addressModeU          = WGPUAddressMode_ClampToEdge;
    sampler_desc.addressModeV          = WGPUAddressMode_ClampToEdge;
    sampler_desc.addressModeW          = WGPUAddressMode_ClampToEdge;
    sampler_desc.minFilter             = WGPUFilterMode_Linear;
    sampler_desc.magFilter             = WGPUFilterMode_Nearest;
    sampler_desc.mipmapFilter          = WGPUMipmapFilterMode_Nearest;
    sampler_desc.lodMinClamp           = 0.0f;
    sampler_desc.lodMaxClamp           = 1.0f;
    sampler_desc.maxAnisotropy         = 1;

    mip_map_generator.sampler = wgpuDeviceCreateSampler(ctx->device, &sampler_desc);

    // Vertex state and Fragment state are shared between all pipelines, so
    // only create once.
    if (!mip_map_generator.vertexState.module
        || !mip_map_generator.fragmentState.module) {
        // vertex state
        mip_map_generator.vertexState             = {};
        mip_map_generator.vertexState.bufferCount = 0;
        mip_map_generator.vertexState.buffers     = NULL;
        mip_map_generator.vertexState.module
          = G_createShaderModule(ctx, mipMapShader, "mipmap vertex shader");
        mip_map_generator.vertexState.entryPoint = VS_ENTRY_POINT;

        // fragment state
        mip_map_generator.fragmentState = {};
        mip_map_generator.fragmentState.module
          = G_createShaderModule(ctx, mipMapShader, "mipmap fragment shader");
        mip_map_generator.fragmentState.entryPoint  = FS_ENTRY_POINT;
        mip_map_generator.fragmentState.targetCount = 1;

        // don't release shader modules here, they are released in
        // MipMapGenerator_release shader modules need to be saved for creating
        // other pipelines
    }
}

void MipMapGenerator_release()
{
    // release sampler
    WGPU_RELEASE_RESOURCE(Sampler, mip_map_generator.sampler);

    // release pipelines
    for (int i = 0; i < ARRAY_LENGTH(mip_map_generator.pipelines); ++i) {
        WGPU_RELEASE_RESOURCE(RenderPipeline, mip_map_generator.pipelines[i]);
    }

    WGPU_RELEASE_RESOURCE(ShaderModule, mip_map_generator.vertexState.module);
    WGPU_RELEASE_RESOURCE(ShaderModule, mip_map_generator.fragmentState.module);
}

static WGPURenderPipeline MipMapGenerator_getPipeline(GraphicsContext* ctx,
                                                      WGPUTextureFormat format)
{
    u32 pipeline_index = (u32)format;
    ASSERT(pipeline_index < (u32)NUMBER_OF_TEXTURE_FORMATS)
    if (mip_map_generator.pipelines[pipeline_index])
        return mip_map_generator.pipelines[pipeline_index];

    // Create pipeline if it doesn't exist

    // Primitive state
    WGPUPrimitiveState primitiveStateDesc = {};
    primitiveStateDesc.topology           = WGPUPrimitiveTopology_TriangleStrip;
    primitiveStateDesc.stripIndexFormat   = WGPUIndexFormat_Uint32;
    primitiveStateDesc.frontFace          = WGPUFrontFace_CCW;
    primitiveStateDesc.cullMode           = WGPUCullMode_None;

    // Color target state
    WGPUBlendState blend_state                   = G_createBlendState(false);
    WGPUColorTargetState color_target_state_desc = {};
    color_target_state_desc.format               = format;
    color_target_state_desc.blend                = &blend_state;
    color_target_state_desc.writeMask            = WGPUColorWriteMask_All;

    mip_map_generator.fragmentState.targets = &color_target_state_desc;

    // Multisample state
    WGPUMultisampleState multisampleState   = {};
    multisampleState.count                  = 1;
    multisampleState.mask                   = 0xFFFFFFFF;
    multisampleState.alphaToCoverageEnabled = false;

    WGPURenderPipelineDescriptor pipelineDesc = {};
    // layout: defaults to `auto`
    pipelineDesc.label       = "mipmap blit render pipeline";
    pipelineDesc.primitive   = primitiveStateDesc;
    pipelineDesc.vertex      = mip_map_generator.vertexState;
    pipelineDesc.fragment    = &mip_map_generator.fragmentState;
    pipelineDesc.multisample = multisampleState;

    // Create rendering pipeline using the specified states
    mip_map_generator.pipelines[pipeline_index]
      = wgpuDeviceCreateRenderPipeline(ctx->device, &pipelineDesc);
    ASSERT(mip_map_generator.pipelines[pipeline_index] != NULL);

    // Store the bind group layout of the created pipeline
    mip_map_generator.pipeline_layouts[pipeline_index]
      = wgpuRenderPipelineGetBindGroupLayout(
        mip_map_generator.pipelines[pipeline_index], 0);
    ASSERT(mip_map_generator.pipeline_layouts[pipeline_index] != NULL)

    return mip_map_generator.pipelines[pipeline_index];
}

void MipMapGenerator_generate(GraphicsContext* ctx, WGPUTexture texture,
                              const char* label)
{
    ASSERT(mip_map_generator.sampler && mip_map_generator.vertexState.module
           && mip_map_generator.fragmentState.module);

    const u32 mip_level_count      = wgpuTextureGetMipLevelCount(texture);
    WGPUTextureDimension dimension = wgpuTextureGetDimension(texture);
    WGPUTextureFormat format       = wgpuTextureGetFormat(texture);

    if (mip_level_count <= 1) return;

    WGPURenderPipeline pipeline = MipMapGenerator_getPipeline(ctx, format);

    log_trace("Generating %d mip levels for texture %s", mip_level_count, label);

    if (dimension == WGPUTextureDimension_3D || dimension == WGPUTextureDimension_1D) {
        log_error("Generating mipmaps for non-2d textures is currently unsupported!");
        return;
    }

    WGPUTexture mip_texture     = texture;
    const u32 array_layer_count = wgpuTextureGetDepthOrArrayLayers(texture);

    WGPUExtent3D mip_level_size = {
        (u32)ceil(wgpuTextureGetWidth(texture) / 2.0f),
        (u32)ceil(wgpuTextureGetHeight(texture) / 2.0f),
        array_layer_count,
    };

    // If the texture was created with RENDER_ATTACHMENT usage we can render
    // directly between mip levels.
    bool render_to_source
      = wgpuTextureGetUsage(texture) & WGPUTextureUsage_RenderAttachment;
    if (!render_to_source) {
        // Otherwise we have to use a separate texture to render into. It can be
        // one mip level smaller than the source texture, since we already have
        // the top level.
        WGPUTextureDescriptor mip_texture_desc = {};
        mip_texture_desc.size                  = mip_level_size;
        mip_texture_desc.format                = format;
        mip_texture_desc.usage                 = WGPUTextureUsage_CopySrc
                                 | WGPUTextureUsage_TextureBinding
                                 | WGPUTextureUsage_RenderAttachment;
        mip_texture_desc.dimension     = WGPUTextureDimension_2D;
        mip_texture_desc.mipLevelCount = mip_level_count - 1;
        mip_texture_desc.sampleCount   = 1;

        mip_texture = wgpuDeviceCreateTexture(ctx->device, &mip_texture_desc);
        ASSERT(mip_texture != NULL);
    }

    WGPUCommandEncoder cmd_encoder = wgpuDeviceCreateCommandEncoder(ctx->device, NULL);
    u32 pipeline_index             = (u32)format;
    WGPUBindGroupLayout bind_group_layout
      = mip_map_generator.pipeline_layouts[pipeline_index];

    const u32 views_count  = array_layer_count * mip_level_count;
    WGPUTextureView* views = ALLOCATE_COUNT(WGPUTextureView, views_count);

    const u32 bind_group_count = array_layer_count * (mip_level_count - 1);
    WGPUBindGroup* bind_groups = ALLOCATE_COUNT(WGPUBindGroup, bind_group_count);

    WGPUTextureViewDescriptor viewDesc = {};
    viewDesc.label                     = "src_view";
    viewDesc.aspect                    = WGPUTextureAspect_All;
    viewDesc.baseMipLevel              = 0;
    viewDesc.mipLevelCount             = 1;
    viewDesc.dimension                 = WGPUTextureViewDimension_2D;
    viewDesc.baseArrayLayer            = 0; // updated in loop
    viewDesc.arrayLayerCount           = 1;

    for (u32 array_layer = 0; array_layer < array_layer_count; ++array_layer) {
        u32 view_index = array_layer * mip_level_count;

        viewDesc.baseArrayLayer = array_layer;
        views[view_index]       = wgpuTextureCreateView(texture, &viewDesc);

        u32 dst_mip_level = render_to_source ? 1 : 0;
        for (u32 i = 1; i < mip_level_count; ++i) {
            const uint32_t target_mip = view_index + i;

            WGPUTextureViewDescriptor mipViewDesc = {};
            mipViewDesc.label                     = "dst_view";
            mipViewDesc.aspect                    = WGPUTextureAspect_All;
            mipViewDesc.baseMipLevel              = dst_mip_level++;
            mipViewDesc.mipLevelCount             = 1;
            mipViewDesc.dimension                 = WGPUTextureViewDimension_2D;
            mipViewDesc.baseArrayLayer            = array_layer;
            mipViewDesc.arrayLayerCount           = 1;

            views[target_mip] = wgpuTextureCreateView(mip_texture, &mipViewDesc);

            WGPURenderPassColorAttachment colorAttachmentDesc = {};
            colorAttachmentDesc.view                          = views[target_mip];
            colorAttachmentDesc.depthSlice    = WGPU_DEPTH_SLICE_UNDEFINED;
            colorAttachmentDesc.resolveTarget = NULL;
            colorAttachmentDesc.loadOp        = WGPULoadOp_Clear;
            colorAttachmentDesc.storeOp       = WGPUStoreOp_Store;
            colorAttachmentDesc.clearValue    = { 0.0f, 0.0f, 0.0f, 0.0f };

            WGPURenderPassDescriptor render_pass_desc = {};
            render_pass_desc.colorAttachmentCount     = 1;
            render_pass_desc.colorAttachments         = &colorAttachmentDesc;
            render_pass_desc.depthStencilAttachment   = NULL;

            WGPURenderPassEncoder pass_encoder
              = wgpuCommandEncoderBeginRenderPass(cmd_encoder, &render_pass_desc);

            // initialize bind group entries
            WGPUBindGroupEntry bg_entries[2] = {};

            // sampler bind group
            bg_entries[0]         = {};
            bg_entries[0].binding = 0;
            bg_entries[0].sampler = mip_map_generator.sampler;

            // source texture bind group
            bg_entries[1]             = {};
            bg_entries[1].binding     = 1;
            bg_entries[1].textureView = views[target_mip - 1];

            WGPUBindGroupDescriptor bg_desc = {};
            bg_desc.layout                  = bind_group_layout;
            bg_desc.entryCount              = ARRAY_LENGTH(bg_entries);
            bg_desc.entries                 = bg_entries;

            uint32_t bind_group_index = array_layer * (mip_level_count - 1) + i - 1;
            bind_groups[bind_group_index]
              = wgpuDeviceCreateBindGroup(ctx->device, &bg_desc);

            wgpuRenderPassEncoderSetPipeline(pass_encoder, pipeline);
            wgpuRenderPassEncoderSetBindGroup(pass_encoder, 0,
                                              bind_groups[bind_group_index], 0, NULL);
            wgpuRenderPassEncoderDraw(pass_encoder, 3, 1, 0, 0);
            wgpuRenderPassEncoderEnd(pass_encoder);

            WGPU_RELEASE_RESOURCE(RenderPassEncoder, pass_encoder)
        }
    }

    // If we didn't render to the source texture, finish by copying the mip
    // results from the temporary mipmap texture to the source.
    if (!render_to_source) {
        for (u32 i = 1; i < mip_level_count; ++i) {

            // log_debug("Copying to mip level %d with sizes %d, %d\n", i,
            //           mip_level_size.width, mip_level_size.height);

            WGPUImageCopyTexture mipCopySrc = {};
            mipCopySrc.texture              = mip_texture;
            mipCopySrc.mipLevel             = i - 1;

            WGPUImageCopyTexture mipCopyDst = {};
            mipCopyDst.texture              = texture;
            mipCopyDst.mipLevel             = i;

            wgpuCommandEncoderCopyTextureToTexture(cmd_encoder, &mipCopySrc,
                                                   &mipCopyDst, &mip_level_size //
            );

            // Turns out wgpu uses floor not ceil to determine mip size
            // mip_level_size.width  = ceil(mip_level_size.width / 2.0f);
            // mip_level_size.height = ceil(mip_level_size.height / 2.0f);
            mip_level_size.width  = floor(mip_level_size.width / 2.0f);
            mip_level_size.height = floor(mip_level_size.height / 2.0f);
        }
    }

    WGPUCommandBuffer command_buffer = wgpuCommandEncoderFinish(cmd_encoder, NULL);
    ASSERT(command_buffer != NULL);
    WGPU_RELEASE_RESOURCE(CommandEncoder, cmd_encoder)

    // Sumbit commmand buffer
    wgpuQueueSubmit(ctx->queue, 1, &command_buffer);

    { // cleanup
        WGPU_RELEASE_RESOURCE(CommandBuffer, command_buffer)

        if (!render_to_source) {
            WGPU_RELEASE_RESOURCE(Texture, mip_texture);
        }

        for (uint32_t i = 0; i < views_count; ++i) {
            WGPU_RELEASE_RESOURCE(TextureView, views[i]);
        }
        FREE_ARRAY(WGPUTextureView, views, views_count);

        for (uint32_t i = 0; i < bind_group_count; ++i) {
            WGPU_RELEASE_RESOURCE(BindGroup, bind_groups[i]);
        }
        FREE_ARRAY(WGPUBindGroup, bind_groups, bind_group_count);
    }
}

// ============================================================================
// Sampler
// ============================================================================
// Samplers are cached and generated on the fly.
// No need to store pointers to WGPUSamplers directly, just store a config
// and fetch the corresponding WGPUSampler only during rendering.
// SamplerConfig is a 9-bit bitfield.
// All possible SamplerConfig <-->WGPUSampler mappings are stored in a flat
// static array.

#define WEBGPU_SAMPLER_PERMUTATIONS 512 // 2^9

// make sure compiler has aligned the SamplerConfig bit-field to 2 bytes
static_assert(sizeof(SamplerConfig) == 2, "SamplerConfig size mismatch");

SamplerConfig SamplerConfig::Default()
{
    SamplerConfig config = {};
    config.wrapU         = SAMPLER_WRAP_REPEAT;
    config.wrapV         = SAMPLER_WRAP_REPEAT;
    config.wrapW         = SAMPLER_WRAP_REPEAT;
    config.filterMag     = SAMPLER_FILTER_LINEAR;
    config.filterMin     = SAMPLER_FILTER_LINEAR;
    config.filterMip     = SAMPLER_FILTER_LINEAR;
    return config;
}

static WGPUSampler _sampler_cache[WEBGPU_SAMPLER_PERMUTATIONS] = {};

WGPUSampler Graphics_GetSampler(GraphicsContext* gctx, SamplerConfig config)
{
    ASSERT(sizeof(config) == sizeof(u16));
    u16 key = *(u16*)&config;
    if (_sampler_cache[key]) return _sampler_cache[key];
    // else create a new sampler
    WGPUSamplerDescriptor samplerDesc = {};
    samplerDesc.addressModeU          = (WGPUAddressMode)config.wrapU;
    samplerDesc.addressModeV          = (WGPUAddressMode)config.wrapV;
    samplerDesc.addressModeW          = (WGPUAddressMode)config.wrapW;
    samplerDesc.magFilter             = (WGPUFilterMode)config.filterMag;
    samplerDesc.minFilter             = (WGPUFilterMode)config.filterMin;
    samplerDesc.mipmapFilter          = (WGPUMipmapFilterMode)config.filterMip;
    samplerDesc.lodMinClamp           = 0;
    samplerDesc.lodMaxClamp           = 32;
    samplerDesc.maxAnisotropy         = 1; // (16 is max but requires LINEAR filtering)
    _sampler_cache[key] = wgpuDeviceCreateSampler(gctx->device, &samplerDesc);

    ASSERT(_sampler_cache[key]);
    return _sampler_cache[key];
}

SamplerConfig Graphics_SamplerConfigFromDesciptor(WGPUSamplerDescriptor* desc)
{
    SamplerConfig config = {};
    config.wrapU         = (SamplerWrapMode)desc->addressModeU;
    config.wrapV         = (SamplerWrapMode)desc->addressModeV;
    config.wrapW         = (SamplerWrapMode)desc->addressModeW;
    config.filterMag     = (SamplerFilterMode)desc->magFilter;
    config.filterMin     = (SamplerFilterMode)desc->minFilter;
    config.filterMip     = (SamplerFilterMode)desc->mipmapFilter;
    return config;
}

// ============================================================================
// GPU_Buffer
// ============================================================================

bool GPU_Buffer::resizeNoCopy(GraphicsContext* gctx, GPU_Buffer* gpu_buffer,
                              u64 new_size, WGPUBufferUsageFlags usage_flags)

{
    if (new_size <= gpu_buffer->capacity
        && (usage_flags & gpu_buffer->usage) == usage_flags) {
        gpu_buffer->size = new_size;
        return false;
    }

    log_debug("Resizing GPU_Buffer from %llu to %llu\n", gpu_buffer->capacity,
              new_size);

    WGPUBufferDescriptor desc = {};
    desc.usage                = usage_flags | WGPUBufferUsage_CopyDst;
    u64 new_capacity          = MAX(gpu_buffer->capacity * 2, new_size);
    desc.size                 = NEXT_MULT4(new_capacity);

    // release old buffer
    WGPU_RELEASE_RESOURCE(Buffer, gpu_buffer->buf);

    // update buffer
    gpu_buffer->buf      = wgpuDeviceCreateBuffer(gctx->device, &desc);
    gpu_buffer->capacity = desc.size;
    gpu_buffer->usage    = desc.usage;
    gpu_buffer->size     = new_size;
    return true;
}

WGPUTextureFormat G_Util::textureFormatSrgbVariant(WGPUTextureFormat format)
{
    if (format == WGPUTextureFormat_BGRA8Unorm) return WGPUTextureFormat_BGRA8UnormSrgb;
    if (format == WGPUTextureFormat_RGBA8Unorm) return WGPUTextureFormat_RGBA8UnormSrgb;

    ASSERT(format == WGPUTextureFormat_BGRA8UnormSrgb
           || format == WGPUTextureFormat_RGBA8UnormSrgb);
    return format;
}

bool G_Util::isStripTopology(WGPUPrimitiveTopology topology)
{
    return (topology == WGPUPrimitiveTopology_LineStrip
            || topology == WGPUPrimitiveTopology_TriangleStrip);
}

void G_Util::printBindGroupEntry(WGPUBindGroupEntry* entry)
{
    printf(
      "WGPUBindGroupEntry {\n"
      "   binding: %d, \n"
      "   buffer : %p, \n"
      "   offset : %llu, \n"
      "   size   : %llu, \n"
      "   sampler: %p, \n"
      "   texture: %p,  \n"
      "}\n",
      entry->binding, (void*)entry->buffer, entry->offset, entry->size,
      (void*)entry->sampler, (void*)entry->textureView);
}

void G_Util::printBindGroupEntryList(WGPUBindGroupEntry* entry, int count)
{
    for (int i = 0; i < count; ++i) printBindGroupEntry(entry + i);
}

bool G_Util::isDepthTextureFormat(WGPUTextureFormat format)
{
    return (format == WGPUTextureFormat_Depth16Unorm
            || format == WGPUTextureFormat_Depth24Plus
            || format == WGPUTextureFormat_Depth24PlusStencil8
            || format == WGPUTextureFormat_Depth32Float
            || format == WGPUTextureFormat_Depth32FloatStencil8);
}

const char* G_Util::textureFormatToString(WGPUTextureFormat format)
{
    switch (format) {
        case WGPUTextureFormat_Undefined: return "Undefined";
        case WGPUTextureFormat_R8Unorm: return "R8Unorm";
        case WGPUTextureFormat_R8Snorm: return "R8Snorm";
        case WGPUTextureFormat_R8Uint: return "R8Uint";
        case WGPUTextureFormat_R8Sint: return "R8Sint";
        case WGPUTextureFormat_R16Uint: return "R16Uint";
        case WGPUTextureFormat_R16Sint: return "R16Sint";
        case WGPUTextureFormat_R16Float: return "R16Float";
        case WGPUTextureFormat_RG8Unorm: return "RG8Unorm";
        case WGPUTextureFormat_RG8Snorm: return "RG8Snorm";
        case WGPUTextureFormat_RG8Uint: return "RG8Uint";
        case WGPUTextureFormat_RG8Sint: return "RG8Sint";
        case WGPUTextureFormat_R32Float: return "R32Float";
        case WGPUTextureFormat_R32Uint: return "R32Uint";
        case WGPUTextureFormat_R32Sint: return "R32Sint";
        case WGPUTextureFormat_RG16Uint: return "RG16Uint";
        case WGPUTextureFormat_RG16Sint: return "RG16Sint";
        case WGPUTextureFormat_RG16Float: return "RG16Float";
        case WGPUTextureFormat_RGBA8Unorm: return "RGBA8Unorm";
        case WGPUTextureFormat_RGBA8UnormSrgb: return "RGBA8UnormSrgb";
        case WGPUTextureFormat_RGBA8Snorm: return "RGBA8Snorm";
        case WGPUTextureFormat_RGBA8Uint: return "RGBA8Uint";
        case WGPUTextureFormat_RGBA8Sint: return "RGBA8Sint";
        case WGPUTextureFormat_BGRA8Unorm: return "BGRA8Unorm";
        case WGPUTextureFormat_BGRA8UnormSrgb: return "BGRA8UnormSrgb";
        case WGPUTextureFormat_RGB10A2Uint: return "RGB10A2Uint";
        case WGPUTextureFormat_RGB10A2Unorm: return "RGB10A2Unorm";
        case WGPUTextureFormat_RG11B10Ufloat: return "RG11B10Ufloat";
        case WGPUTextureFormat_RGB9E5Ufloat: return "RGB9E5Ufloat";
        case WGPUTextureFormat_RG32Float: return "RG32Float";
        case WGPUTextureFormat_RG32Uint: return "RG32Uint";
        case WGPUTextureFormat_RG32Sint: return "RG32Sint";
        case WGPUTextureFormat_RGBA16Uint: return "RGBA16Uint";
        case WGPUTextureFormat_RGBA16Sint: return "RGBA16Sint";
        case WGPUTextureFormat_RGBA16Float: return "RGBA16Float";
        case WGPUTextureFormat_RGBA32Float: return "RGBA32Float";
        case WGPUTextureFormat_RGBA32Uint: return "RGBA32Uint";
        case WGPUTextureFormat_RGBA32Sint: return "RGBA32Sint";
        case WGPUTextureFormat_Stencil8: return "Stencil8";
        case WGPUTextureFormat_Depth16Unorm: return "Depth16Unorm";
        case WGPUTextureFormat_Depth24Plus: return "Depth24Plus";
        case WGPUTextureFormat_Depth24PlusStencil8: return "Depth24PlusStencil8";
        case WGPUTextureFormat_Depth32Float: return "Depth32Float";
        case WGPUTextureFormat_Depth32FloatStencil8: return "Depth32FloatStencil8";
        case WGPUTextureFormat_BC1RGBAUnorm: return "BC1RGBAUnorm";
        case WGPUTextureFormat_BC1RGBAUnormSrgb: return "BC1RGBAUnormSrgb";
        case WGPUTextureFormat_BC2RGBAUnorm: return "BC2RGBAUnorm";
        case WGPUTextureFormat_BC2RGBAUnormSrgb: return "BC2RGBAUnormSrgb";
        case WGPUTextureFormat_BC3RGBAUnorm: return "BC3RGBAUnorm";
        case WGPUTextureFormat_BC3RGBAUnormSrgb: return "BC3RGBAUnormSrgb";
        case WGPUTextureFormat_BC4RUnorm: return "BC4RUnorm";
        case WGPUTextureFormat_BC4RSnorm: return "BC4RSnorm";
        case WGPUTextureFormat_BC5RGUnorm: return "BC5RGUnorm";
        case WGPUTextureFormat_BC5RGSnorm: return "BC5RGSnorm";
        case WGPUTextureFormat_BC6HRGBUfloat: return "BC6HRGBUfloat";
        case WGPUTextureFormat_BC6HRGBFloat: return "BC6HRGBFloat";
        case WGPUTextureFormat_BC7RGBAUnorm: return "BC7RGBAUnorm";
        case WGPUTextureFormat_BC7RGBAUnormSrgb: return "BC7RGBAUnormSrgb";
        case WGPUTextureFormat_ETC2RGB8Unorm: return "ETC2RGB8Unorm";
        case WGPUTextureFormat_ETC2RGB8UnormSrgb: return "ETC2RGB8UnormSrgb";
        case WGPUTextureFormat_ETC2RGB8A1Unorm: return "ETC2RGB8A1Unorm";
        case WGPUTextureFormat_ETC2RGB8A1UnormSrgb: return "ETC2RGB8A1UnormSrgb";
        case WGPUTextureFormat_ETC2RGBA8Unorm: return "ETC2RGBA8Unorm";
        case WGPUTextureFormat_ETC2RGBA8UnormSrgb: return "ETC2RGBA8UnormSrgb";
        case WGPUTextureFormat_EACR11Unorm: return "EACR11Unorm";
        case WGPUTextureFormat_EACR11Snorm: return "EACR11Snorm";
        case WGPUTextureFormat_EACRG11Unorm: return "EACRG11Unorm";
        case WGPUTextureFormat_EACRG11Snorm: return "EACRG11Snorm";
        case WGPUTextureFormat_ASTC4x4Unorm: return "ASTC4x4Unorm";
        case WGPUTextureFormat_ASTC4x4UnormSrgb: return "ASTC4x4UnormSrgb";
        case WGPUTextureFormat_ASTC5x4Unorm: return "ASTC5x4Unorm";
        case WGPUTextureFormat_ASTC5x4UnormSrgb: return "ASTC5x4UnormSrgb";
        case WGPUTextureFormat_ASTC5x5Unorm: return "ASTC5x5Unorm";
        case WGPUTextureFormat_ASTC5x5UnormSrgb: return "ASTC5x5UnormSrgb";
        case WGPUTextureFormat_ASTC6x5Unorm: return "ASTC6x5Unorm";
        case WGPUTextureFormat_ASTC6x5UnormSrgb: return "ASTC6x5UnormSrgb";
        case WGPUTextureFormat_ASTC6x6Unorm: return "ASTC6x6Unorm";
        case WGPUTextureFormat_ASTC6x6UnormSrgb: return "ASTC6x6UnormSrgb";
        case WGPUTextureFormat_ASTC8x5Unorm: return "ASTC8x5Unorm";
        case WGPUTextureFormat_ASTC8x5UnormSrgb: return "ASTC8x5UnormSrgb";
        case WGPUTextureFormat_ASTC8x6Unorm: return "ASTC8x6Unorm";
        case WGPUTextureFormat_ASTC8x6UnormSrgb: return "ASTC8x6UnormSrgb";
        case WGPUTextureFormat_ASTC8x8Unorm: return "ASTC8x8Unorm";
        case WGPUTextureFormat_ASTC8x8UnormSrgb: return "ASTC8x8UnormSrgb";
        case WGPUTextureFormat_ASTC10x5Unorm: return "ASTC10x5Unorm";
        case WGPUTextureFormat_ASTC10x5UnormSrgb: return "ASTC10x5UnormSrgb";
        case WGPUTextureFormat_ASTC10x6Unorm: return "ASTC10x6Unorm";
        case WGPUTextureFormat_ASTC10x6UnormSrgb: return "ASTC10x6UnormSrgb";
        case WGPUTextureFormat_ASTC10x8Unorm: return "ASTC10x8Unorm";
        case WGPUTextureFormat_ASTC10x8UnormSrgb: return "ASTC10x8UnormSrgb";
        case WGPUTextureFormat_ASTC10x10Unorm: return "ASTC10x10Unorm";
        case WGPUTextureFormat_ASTC10x10UnormSrgb: return "ASTC10x10UnormSrgb";
        case WGPUTextureFormat_ASTC12x10Unorm: return "ASTC12x10Unorm";
        case WGPUTextureFormat_ASTC12x10UnormSrgb: return "ASTC12x10UnormSrgb";
        case WGPUTextureFormat_ASTC12x12Unorm: return "ASTC12x12Unorm";
        case WGPUTextureFormat_ASTC12x12UnormSrgb: return "ASTC12x12UnormSrgb";
        case WGPUTextureFormat_Force32: return "Force32";
    }
    return "INVALID_FORMAT";
}

const char* G_Util::cullModeToString(WGPUCullMode mode)
{
    switch (mode) {
        case WGPUCullMode_None: return "None";
        case WGPUCullMode_Front: return "Front";
        case WGPUCullMode_Back: return "Back";
        default: return "INVALID_CULL_MODE";
    }
}

const char* G_Util::topologyToString(WGPUPrimitiveTopology top)
{
    switch (top) {
        case WGPUPrimitiveTopology_PointList: return "PointList";
        case WGPUPrimitiveTopology_LineList: return "LineList";
        case WGPUPrimitiveTopology_LineStrip: return "LineStrip";
        case WGPUPrimitiveTopology_TriangleList: return "TriangleList";
        case WGPUPrimitiveTopology_TriangleStrip: return "TriangleStrip";
        default: return "INVALID_TOPOLOGY";
    }
}
