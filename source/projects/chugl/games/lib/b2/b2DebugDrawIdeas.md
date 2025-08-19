// B2 Debug Draw

```c
wgpuRenderPassEncoderDraw(
    WGPURenderPassEncoder renderPassEncoder, 
    uint32_t vertexCount, 
    uint32_t instanceCount, 
    uint32_t firstVertex, 
    uint32_t firstInstance
) WGPU_FUNCTION_ATTRIBUTE;

wgpuRenderPassEncoderDrawIndexed(
    WGPURenderPassEncoder renderPassEncoder, 
    uint32_t indexCount, 
    uint32_t instanceCount, 
    uint32_t firstIndex, 
    int32_t baseVertex, 
    uint32_t firstInstance
) WGPU_FUNCTION_ATTRIBUTE;
```

IMPLEMENTING BOX2D DEBUG DRAW

for sdfs on a quad, just store the quad in shader global memory like so:
```rust
    var<private> pos : array<vec2<f32>, 3> = array<vec2<f32>, 3>(
        vec2<f32>(-1.0, -1.0), 
        vec2<f32>(-1.0, 3.0), 
        vec2<f32>(3.0, -1.0)
    );

    // https://stackoverflow.com/questions/73379152/how-can-i-declare-and-use-a-constant-array-in-a-wgsl-vertex-shader
    // OR actually try this:
    // (var<private> is mutable in vertex shader, only visible to current invocation)
$$    const TRI_VERTICES = array(
        vec4(0., 0., 0., 1.),
        vec4(0., 1., 0., 1.),
        vec4(1., 1., 0., 1.),
    );
```

- https://wickedengine.net/2016/11/abuse-the-immediate-constant-buffer/


Core idea: b2_debugDraw doesn't have notion of transform/ggen: it gives the absolute positions
of physics geometry. So instead of keeping a pool of GMesh, collect all the necessary data of 
each geometry (circle, capsule, polygon etc), store it into a single GPU buffer, and draw
every instance at once via instanced draw call...
- reveals key difference betwee 2D and 3D rendering pipelines

GB2DebugDraw will be like GText: a special GGen that doesn't expose it's underlying material and geometry
- on audio thread side, it will be a GGen
- on graphics side, it will be a GMesh?

Impl:
- would like for the DebugDraw world to be a GGen that chugl user can position/scale, renders in 3D space
- idea: do the entire DebugDraw app.cpp critical region, before broadcasting NextFrameEvent.
    - critical region to prevent read/write race condition with graphics shreds calling b2 API
    - downside: this requires preparing vertex and storage buffers, expensive. Want to minimize duration of critical region
    - but doing it in critical region on graphics thread is still more performant than audio thread: at least other 
    non-graphics shreds can still run. 

Other approach: add some kind of procedural 2D immediate-mode drawing API to ChuGL
- starting to see a way...
    - maybe add this *after* building out the special case for DebugDraw
    - group all the geometry/parameters together under their respective shapes/materials
      - then at frame flip on audio thread, bundle and add the whole thing together at once to the end of command queue
        - only need 1 command type: DRAW_IMMEDIATE_MODE, which contains all the data for circles/capsules/squares/etc all supported shapes








