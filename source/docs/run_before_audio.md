# Issue #11: Loading Large Chuck Files Before Audio Causes Crash

> Investigation of https://github.com/shakfu/chuck-max/issues/11

## Summary

This document analyzes the crash that occurs when loading ChucK files containing global custom events **before** turning on audio in Max/MSP.

## Crash Mechanism

The crash occurs in `Chuck_Globals_Manager::get_global_event()` at `chuck_globals.cpp:1320`:

```cpp
Chuck_Event * Chuck_Globals_Manager::get_global_event( const std::string & name )
{
    return m_global_events[name]->val;
}
```

**Problem**: If the key doesn't exist in the map, `std::map::operator[]` creates a default-initialized entry (NULL pointer), and `->val` dereferences NULL, causing a crash at address 0x0.

### Crash Trace
```
Thread 60 Crashed:: com.apple.audio.IOThread.client
0   chuck~    Chuck_Globals_Manager::get_global_event(...) + 44 (chuck_globals.cpp:1320)
1   chuck~    Chuck_Instr_Alloc_Word_Global::execute(...) + 476 (chuck_instr.cpp:4162)
```

## Minimal Reproduction Case

### ChucK Code (`crash.ck`)
```chuck
class CustomEvent extends Event {}
global CustomEvent e;
while (true) { 1::second => now; }
```

### Crash Sequence
1. Send `clear vm` or `reset` message
2. Send `run crash.ck` message
3. Turn on audio
4. **CRASH**

## Root Cause: Queue Ordering Issue

The crash is caused by a **FIFO queue ordering problem** in the globals manager's request processing.

### Detailed Flow

1. **T1**: `clear vm` is called
   - `CK_MSG_CLEARVM` is queued via `execute_chuck_msg_with_globals()`

2. **T2**: `run crash.ck` is called
   - File is compiled (emit phase)
   - `init_global_event("e", CustomEvent)` is called - event added to `m_global_events`
   - `spork_shred_request` is queued

3. **T3**: Audio is turned on
   - `ck_perform64()` calls `chuck->run()`
   - `compute()` calls `handle_global_queue_messages()`
   - Messages processed in FIFO order:
     - **First**: `CK_MSG_CLEARVM` processed - `cleanup_global_variables()` **removes the event**
     - **Second**: `spork_shred_request` processed - shred is sporked

4. **T3**: Shred executes
   - `Chuck_Instr_Alloc_Word_Global::execute()` is called
   - `get_global_event("e")` is called
   - Event not found in map - **CRASH**

### Key Insight

The `CK_MSG_CLEARVM` was queued **before** the file was compiled, but it's processed **after** the event was added to the map. This removes the freshly-added event before the shred can access it.

## Original Issue (Without Explicit Clear)

The original bug report describes crashes when loading files before audio **without** an explicit `clear vm`:

> "When I load the chuck program files first, then start the audio, Max crashes."

### Possible Causes

1. **Implicit VM state**: Some initialization may occur during the first audio callback that's required for globals to function properly

2. **Threading/Memory Visibility**: The main thread (emit) writes to `m_global_events`, the audio thread reads from it. Without proper memory barriers, the audio thread might not see the writes.

3. **Multiple Instances**: The original report mentions "2 separate instances of chuck~" which could introduce interactions

4. **VM Initialization State**: The globals manager may require "priming" through normal audio processing before globals work correctly

## Current Workaround

The `run_needs_audio` attribute (default: `1`) prevents the crash by refusing to run/add shreds when audio is off:

```cpp
// In ck_run() and ck_add()
if (x->run_needs_audio && !sys_getdspstate()) {
    ck_error(x, (char*)"can only run/add shred when audio is on");
    return MAX_ERR_GENERIC;
}
```

This forces the safe workflow: audio on first, then load files.

## Proposed Fixes

### Option 1: Defensive Null Checks in Accessors (Recommended)

Add existence checks before accessing map entries:

```cpp
Chuck_Event * Chuck_Globals_Manager::get_global_event( const std::string & name )
{
    auto it = m_global_events.find(name);
    if (it == m_global_events.end() || it->second == nullptr) {
        return nullptr;  // Caller must handle NULL
    }
    return it->second->val;
}
```

Then update callers to handle NULL returns gracefully.

### Option 2: Process Destructive Messages Immediately

Make `CK_MSG_CLEARVM` and `CK_MSG_CLEARGLOBALS` execute immediately instead of being queued:

```cpp
t_CKBOOL Chuck_Globals_Manager::execute_chuck_msg_with_globals( Chuck_Msg * msg )
{
    // Process destructive messages immediately
    if (msg->type == CK_MSG_CLEARVM || msg->type == CK_MSG_CLEARGLOBALS) {
        m_vm->process_msg(msg);
        return TRUE;
    }
    // Queue other messages
    // ...
}
```

**Risk**: Could introduce other ordering issues.

### Option 3: Add Existence Check in Instruction

Validate event existence before accessing:

```cpp
case te_globalEvent:
    if (m_global_events.count(m_name) == 0) {
        EM_error2(0, "global event '%s' not initialized", m_name.c_str());
        goto error;
    }
    addr = (t_CKUINT) vm->globals_manager()->get_global_event( m_name );
    break;
```

### Option 4: Upstream ChucK Fix

The ideal fix would be in the ChucK VM itself:
- Ensure `Chuck_Globals_Manager` is fully initialized after `chuck->init()` and `chuck->start()`
- Allow global event allocation before first audio processing
- Add thread-safety to globals manager operations

## Related Code Locations

| File | Line | Function | Description |
|------|------|----------|-------------|
| `chuck_globals.cpp` | 1319-1322 | `get_global_event()` | Crash location |
| `chuck_globals.cpp` | 1286-1310 | `init_global_event()` | Event initialization |
| `chuck_globals.cpp` | 2768-2833 | `cleanup_global_variables()` | Clears all globals |
| `chuck_globals.cpp` | 2842-3200 | `handle_global_queue_messages()` | Processes request queue |
| `chuck_instr.cpp` | 4322-4326 | `Chuck_Instr_Alloc_Word_Global::execute()` | Calls get_global_event |
| `chuck_emit.cpp` | 5499-5511 | emit code | Calls init_global_event during compile |
| `chuck_vm.cpp` | 970-987 | `process_msg()` | Handles CK_MSG_CLEARVM |
| `chuck_tilde.cpp` | 285 | `ck_new()` | Sets run_needs_audio default |
| `chuck_tilde.cpp` | 989, 1072 | `ck_run()`, `ck_add()` | Enforces run_needs_audio |

## Testing

Test cases are located in `/patchers/tests/critical/reset-before-run/`:
- `crash.*.maxpat` - Patches that reproduce the crash
- `nocrash.*.maxpat` - Patches that work correctly

## Conclusion

The crash is caused by a combination of:
1. Unsafe map access in `get_global_event()` (no existence check)
2. Queue ordering issues when `clear vm` is called before `run`
3. Possible VM initialization requirements not met when loading before audio

The `run_needs_audio` workaround effectively prevents the crash by enforcing the correct workflow. A proper fix would require either defensive coding in the accessors or changes to the ChucK VM's globals initialization logic.
