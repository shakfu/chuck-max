# Upstream ChucK Issue: Crash in get_global_event() when accessing non-existent global

> Issue text for posting to upstream ChucK project

---

## Title: Crash in get_global_event() when accessing non-existent global (NULL dereference)

### Description

`Chuck_Globals_Manager::get_global_event()` (and similar functions `get_global_ugen()`, `get_global_object()`) can crash with a NULL pointer dereference when the requested global doesn't exist in the map.

### Root Cause

The accessor functions use `std::map::operator[]` without checking if the key exists:

```cpp
Chuck_Event * Chuck_Globals_Manager::get_global_event( const std::string & name )
{
    return m_global_events[name]->val;
}
```

If `name` doesn't exist in the map, `operator[]` creates a default-initialized entry (NULL pointer), then `->val` dereferences NULL, causing a crash.

### Reproduction Scenario

This can occur when `CK_MSG_CLEARVM` is processed after a file is compiled but before its shred runs:

1. Queue `CK_MSG_CLEARVM` via `execute_chuck_msg_with_globals()`
2. Compile a file with `global CustomEvent e;` - this calls `init_global_event()`, adding the event to the map
3. When audio processing starts, `handle_global_queue_messages()` processes in FIFO order:
   - `CK_MSG_CLEARVM` runs first, calling `cleanup_global_variables()` which clears the map
   - The shred is then sporked and runs
4. Shred executes `Chuck_Instr_Alloc_Word_Global` which calls `get_global_event()` - key not found, crash

### Minimal Test Case

```chuck
class CustomEvent extends Event {}
global CustomEvent e;
while (true) { 1::second => now; }
```

The crash occurs when this file is loaded after a VM clear but before audio processing begins.

### Crash Trace

```
Thread Crashed:: com.apple.audio.IOThread.client
0   Chuck_Globals_Manager::get_global_event(...) + 44 (chuck_globals.cpp:1320)
1   Chuck_Instr_Alloc_Word_Global::execute(...) + 476 (chuck_instr.cpp:4162)
```

### Proposed Fix

Add defensive null checks in the accessor functions:

**chuck_globals.cpp:**
```cpp
Chuck_Event * Chuck_Globals_Manager::get_global_event( const std::string & name )
{
    auto it = m_global_events.find(name);
    if( it == m_global_events.end() || it->second == nullptr )
        return nullptr;
    return it->second->val;
}
```

(Same pattern for `get_global_ugen()` and `get_global_object()`)

**chuck_instr.cpp** (in `Chuck_Instr_Alloc_Word_Global::execute()`):
```cpp
case te_globalEvent:
    addr = (t_CKUINT) vm->globals_manager()->get_global_event( m_name );
    if( addr == 0 ) {
        EM_error2( 0, "global event '%s' not found", m_name.c_str() );
        goto error;
    }
    break;
```

This converts crashes into recoverable errors with helpful messages.

### Environment

- ChucK version: 1.5.5.x
- Discovered in chuck-max (Max/MSP external embedding ChucK)
- Issue tracking: https://github.com/shakfu/chuck-max/issues/11
