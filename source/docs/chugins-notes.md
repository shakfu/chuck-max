# CHUGINS

## Overview

 chugin (ChuGin) is a plug-in for the ChucK audio programming language that extends its capabilities by adding class libraries, Unit Generators (UGens), and Unit Analyzers (UAnae).

## Repository Architecture

### Directory Structure

```
chugins/
├── chuck/include/chugin.h    # Core chugin API header
├── chuginate/                # Tool for generating new chugin boilerplate
├── <ChuginName>/             # Each chugin in its own directory
│   ├── <ChuginName>.cpp      # Main implementation
│   ├── makefile              # Build orchestrator
│   ├── makefile.mac          # macOS-specific settings
│   ├── makefile.linux        # Linux-specific settings
│   ├── <ChuginName>.vcxproj  # Visual Studio project
│   └── <ChuginName>-test.ck  # ChucK test/example file
└── makefile                  # Top-level makefile for all chugins
```

### Chugin Implementation Pattern

All chugins follow this structure:

1. **Include chugin.h**: Provides ChucK types (t_CKINT, t_CKFLOAT, SAMPLE) and macros

2. **Define callback functions**: Constructor (CK_DLL_CTOR), destructor (CK_DLL_DTOR), tick function (CK_DLL_TICK), and member functions (CK_DLL_MFUN)

3. **Implement query function**: The DLL query function that registers the chugin with ChucK's type system

4. **Data offset variable**: Used to store chugin instance data in ChucK objects

Example skeleton:
```cpp
#include "chugin.h"

CK_DLL_CTOR(mychugin_ctor);
CK_DLL_DTOR(mychugin_dtor);
CK_DLL_TICK(mychugin_tick);
CK_DLL_MFUN(mychugin_setParam);

t_CKINT mychugin_data_offset = 0;

CK_DLL_QUERY(MyChugin) {
    // Register with ChucK type system
    // Set constructors, destructors, member functions
    // Store data offset
}
```

### Build Flags

- `CHUCK_STRICT=1`: Enable strict compilation (treats warnings as errors) - enabled by default in top-level makefile

- `CHUCK_DEBUG=1`: Enable debug symbols

- `CK_CHUGIN_STATIC=1`: Build as static chugin (.schug) instead of dynamic (.chug)

### Special Chugins

Some chugins have unique requirements:

- **Faust**: Requires libfaust (LLVM-based Faust compiler) installed on the system. Allows on-the-fly evaluation of Faust DSP code.

- **FluidSynth**: Not built on Windows (listed in CHUGS_NOT_ON_WIN32). Requires FluidSynth library.

- **AbletonLink**: Uses Ableton Link SDK for tempo synchronization across applications.

- **AudioUnit**: macOS-only chugin (listed in CHUGS_NOT_ON_WIN32). Loads and uses AudioUnit plugins in ChucK. Requires AudioToolbox and CoreFoundation frameworks (automatically linked on macOS).

- **WarpBuf**: Has its own CI workflow (`.github/workflows/WarpBuf.yml`) in addition to the main build workflow.

## Creating New Chugins

Use the `chuginate` tool to generate boilerplate:

```bash
cd chuginate
./chuginate <ChuginName> <destination_directory>
```

This generates:
- C++ source template
- Makefile configuration
- Visual Studio project
- Example ChucK test file
- Chuck header files

## Testing Chugins

Each chugin typically includes a `-test.ck` or `-help.ck` file demonstrating usage. Run these with:

```bash
chuck <ChuginName>-test.ck
```

Chugins must be in ChucK's search path (see README.md for default locations) or loaded explicitly:
```bash
chuck --chugin:<path/to/chugin.chug> test.ck
```

## ChucK Version Compatibility

Chugins are versioned and must match the host ChucK version. The version is defined in `chuck/include/chugin.h` as `CHUCK_VERSION_STRING` (currently "1.5.5.0 (chai)").

## File Extensions

- `.chug` - Dynamic chugin (default)
- `.schug` - Static chugin
- `.wasm` - WebChucK chugin (built with emscripten)
- `.ck` - ChucK source code
- `.dsp` - Faust source code (for Faust chugin)

## Accessing the ChucK Host API from a Chugin

### API Overview

The ChucK host API is accessed through the `Chuck_DL_Api` structure (typedef'd as `CK_DL_API`), which is automatically passed to all chugin callback functions via the `API` parameter.

### API Structure

The API provides four main sub-APIs:

1. **`API->vm`** - Virtual Machine operations (sample rate, time, events, exceptions)

2. **`API->object`** - Object management (creation, strings, arrays, member variables)

3. **`API->type`** - Type system access (type lookup, vtable offsets)

4. **`API->shred`** - Shred/thread operations

### Function Signatures

All chugin callbacks receive:
- `Chuck_VM * VM` - Handle to the ChucK VM
- `Chuck_VM_Shred * SHRED` - Current shred (thread) context
- `CK_DL_API API` - The host API handle

### Examples from AudioUnit Chugin

#### 1. Getting Sample Rate (AudioUnit.cpp:597)
```cpp
CK_DLL_CTOR(audiounit_ctor)
{
    // Access VM sample rate through API
    AudioUnitWrapper* wrapper = new AudioUnitWrapper(API->vm->srate(VM));
    OBJ_MEMBER_INT(SELF, audiounit_data_offset) = (t_CKINT)wrapper;
}
```

#### 2. Creating ChucK Strings (AudioUnit.cpp:691)
```cpp
CK_DLL_MFUN(audiounit_get_param_name)
{
    AudioUnitWrapper* wrapper = (AudioUnitWrapper*)OBJ_MEMBER_INT(SELF, audiounit_data_offset);
    t_CKINT index = GET_NEXT_INT(ARGS);

    const char* name = wrapper ? wrapper->getParameterName(index) : "";

    // Create a ChucK string and return it
    // addRef=false since we're returning without keeping a reference
    RETURN->v_string = API->object->create_string(VM, name, false);
}
```

### Common API Functions

**VM API:**
- `API->vm->srate(VM)` - Get sample rate
- `API->vm->now(VM)` - Get current time
- `API->vm->throw_exception(msg, desc, SHRED)` - Throw exception
- `API->vm->em_log(level, text)` - Log message

**Object API:**
- `API->object->create_string(VM, "text", addRef)` - Create string
- `API->object->create(SHRED, type, addRef)` - Create object
- `API->object->get_mvar_int(obj, "name", value)` - Get member variable
- `API->object->array_int_size(array)` - Array operations

**Type API:**
- `API->type->lookup(VM, "TypeName")` - Find type by name
- `API->type->get_vtable_offset(VM, type, "funcName")` - Get function offset
- `API->type->isa(type1, type2)` - Type relationship check

For complete API documentation, see `chuck/include/chugin.h` starting at line 2094 (struct Chuck_DL_Api).
