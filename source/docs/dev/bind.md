# Host-defined ChucK classes via `ChucK::bind()`

Status: **proposal**. Not implemented. The code below has not been compiled.

## Purpose

`ChucK::bind()` registers native classes from the host into a ChucK VM.
`chuck~` does not call it. This document proposes a `Max` class.
`Max.send()` lets a shred push a message out of the `chuck~` reply outlet.

Today, pushing a value from ChucK to Max takes four steps:

1. Declare a `global float`.
2. Declare a `global Event`.
3. Send `listen` to the event.
4. Answer each event with a `get` round trip.

`Max.send()` does this in one call.

The same mechanism supports four bindings:

| Direction | Binding | Delivery |
|-|-|-|
| ChucK to Max | `Max.send()` | queued, next main-thread tick |
| Max to ChucK | `MaxIn` / `MaxMsg` | queued, next audio block |
| ChucK reads Max | `MaxBuffer` | immediate, audio thread |
| Transport sync | `MaxClock` | snapshot per block, extrapolated per sample |

`Max.send()` is specified in full below. The others are outlines.

## API

`ChucK::bind( f_ck_query queryFunc, const std::string & name )`
(`chuck/core/chuck.cpp`, `ChucK::bind`).

- `queryFunc` declares classes the same way a chugin's `ck_query` does.
- Types go into the `global` namespace (`Chuck_Compiler::bind`, default
  `nspc = "global"` in `chuck_compile.h`).
- It needs an initialized compiler, so call it after `ChucK::init()`.

## Usage

ChucK:

```chuck
SinOsc s => dac;
while (true) {
    Math.random2f(200, 800) => float f => s.freq;
    Max.send("freq", f);     // reply outlet: msg freq 437.2
    250::ms => now;
}
```

Max: `[chuck~]` rightmost outlet -> `[route msg]` -> `[route freq]`.

## Implementation sketch

In `chuck_tilde.cpp`:

```cpp
static t_symbol* ps_msg = NULL;      // msg <name> <value>      -- Max.send()

// find the chuck~ that owns this VM; audio-thread safe (atomic slot reads)
static t_ck* ck_instance_from_vm(Chuck_VM* vm)
{
    for (long i = 0; i < CK_MAX_INSTANCES; i++) {
        t_ck* x = CK_INSTANCE_SLOTS[i].load(std::memory_order_acquire);
        if (x && x->chuck && x->chuck->vm() == vm) return x;
    }
    return NULL;
}

// Max.send(string name, float value); runs on the audio thread
CK_DLL_SFUN(max_send)
{
    Chuck_String* name = GET_NEXT_STRING(ARGS);
    t_CKFLOAT value = GET_NEXT_FLOAT(ARGS);
    t_ck* x = ck_instance_from_vm(VM);
    if (x == NULL || name == NULL) return;

    t_atom a[2];
    atom_setsym(a, gensym(name->c_str()));
    atom_setfloat(a + 1, value);
    ck_reply_push(x, ps_msg, 2, a);
}

static t_CKBOOL CK_DLL_CALL ck_max_query(Chuck_DL_Query* QUERY)
{
    QUERY->begin_class(QUERY, "Max", "Object");
    QUERY->doc_class(QUERY, "messages from ChucK to the chuck~ reply outlet");
    QUERY->add_sfun(QUERY, max_send, "void", "send");
    QUERY->add_arg(QUERY, "string", "name");
    QUERY->add_arg(QUERY, "float", "value");
    QUERY->end_class(QUERY);
    return TRUE;
}
```

Registration:

- `ext_main`, beside `ps_val = gensym("val")`: `ps_msg = gensym("msg");`
- `ck_new`, after `x->chuck->init()`: `x->chuck->bind(ck_max_query, "Max");`

## Design decisions

- **Instance lookup.** A static function receives `Chuck_VM* VM`, but no host
  pointer. A scan of `CK_INSTANCE_SLOTS` maps the VM to its `t_ck`. A global
  `std::map<Chuck_VM*, t_ck*>` would need a lock on the audio thread.

- **Threading.** Shreds run inside `ck_perform64`, on the audio thread.
  `ck_reply_push` is the existing lock-free queue. A qelem drains it on the main
  thread. Calling `outlet_anything` from a shred would break Max's threading
  rules.

- **Selector.** The selector is always `msg`. The user's name is an argument.
  This follows the reply vocabulary (`val`, `event`, `global`). No ChucK name
  can collide with those selectors.

- **`reset`.** `CK_MSG_CLEARVM` calls `clear_user_namespace()`
  (`chuck_vm.cpp`). `Max` lives in the global namespace, so it should survive.
  This is inferred from the source, not tested.

## Caveats

- **`gensym` on the audio thread.** `gensym()` takes a lock.
  `cb_get_all_global_vars` already does this. To avoid it, copy the name into
  the queue entry and call `gensym` in `ck_reply_drain`.

- **Timing.** The message is timed in ChucK logical time. Max delivers it on the
  next main-thread tick. Fine for control data. Not sample-accurate.

- **Overflow.** `ck_reply_push` drops the message when the queue is full. It
  does not report the drop. A shred that sends every sample will fill the queue.

## Extensions

The same pattern covers:

- Overloads of `send` with `int`, `string`, or `float[]` arguments.
- `Max.post(string)` to the Max console.
- `Max.bang(string name)`.

## Signals (audio and CV)

`Max.send()` does not suit audio or CV. It delivers at main-thread tick rate
and drops messages when the queue fills.

Existing routes already carry signals:

| Need | Route |
|-|-|
| Audio out | `dac` channels map to signal outlets. `[chuck~ 8]` gives 8. |
| CV out | Same `dac` channels. CV is an ordinary Max signal. `dac` does not block DC, so `Step s => dac.chan(3)` holds a constant. Inferred, not tested. Needs a DC-coupled interface. |
| Audio/CV in | `adc` channels map to signal inlets. |
| Signals off `dac` | `@ntap N` outlets read `global` UGens with `buffered` set. `ck_perform64` reads them in the same block via `getGlobalUGenSamples[Multi]`. |

`bind()` can also register UGens, through `add_ugen_func` and
`add_ugen_funcf` (`chuck_dl.h`). The tick function runs inside
`ChucK::run()`, on the audio thread. `SELF->originVM()` (`chuck_oo.h`)
identifies the owning `chuck~`. A `MaxOut` UGen could write to a per-instance
buffer that `ck_perform64` copies to extra outlets:

```chuck
SinOsc lfo => MaxOut cv1;   // hypothetical bound UGen
"pitch" => cv1.name;        // outlet chosen by name, not dac channel
```

The only gain over `dac` and taps is outlets named from ChucK. The costs:

- A second buffer path in `ck_perform64`.
- Rules for mapping names to outlets.
- Tests for both.

Max fixes outlet count at object creation, so ChucK code cannot add outlets at
runtime. Not recommended.

For CV work, transport sync matters more. See `MaxClock` below.

## Max to ChucK: `MaxIn` / `MaxMsg`

Max-to-ChucK today goes through `ck_anything`:

- `<name> <value>` sets a ChucK global.

- `<name> <a b c>` sets a global array. The first atom's type sets the array
  type.

- `signal` and `broadcast` fire a `global Event`.

Gaps:

- Sending a value and waking a shred takes two messages: `freq 440`, then
  `signal freqChanged`.

- Mixed-type lists such as `note 60 0.8 legato` are not supported.

- Every name must be a declared `global`.

`MaxIn` and `MaxMsg` follow ChucK's `OscIn` and `OscMsg`:

```chuck
MaxIn min;
min.addAddress("note");
MaxMsg msg;
while (true) {
    min => now;                     // MaxIn extends Event
    while (min.recv(msg)) {
        msg.getInt(0) => int pitch;
        msg.getFloat(1) => float vel;
        msg.getString(2) => string art;
    }
}
```

Max: `[msg note 60 0.8 legato(` -> `[chuck~]`.

`msg` is a new inlet selector. It matches the `msg` reply selector of
`Max.send()`. Unknown selectors keep setting globals, so existing patches do
not change.

Mechanism:

1. `ck_msg` runs on the main thread. It copies the atoms into a per-instance
   lock-free inbox, the reverse of the reply queue.

2. `ck_perform64` drains the inbox before `chuck->run()`. This is the VM
   thread. Each message goes to every `MaxIn` registered for its name. That
   `MaxIn` then broadcasts.

3. The shred wakes inside the same `run()`.

Draining in `ck_perform64` is preferred over `API->vm->queue_event()`
(`chuck_dl.h`), which `OscIn` and MIDI use. With `queue_event`, each `MaxIn`
needs a mutex, and `recv()` takes it on the audio thread. Either way, delivery
is at the next block boundary.

Lifecycle:

- A `MaxIn` must unregister in its destructor, on the VM thread. Otherwise the
  drain writes into a freed object.

- `CK_MSG_CLEARVM` destroys user objects. The registry must clear on the same
  path. Inferred, not traced.

- Payloads need a cap, as `CK_REPLY_MAX_ATOMS` caps replies.

## ChucK reads Max: `MaxBuffer`

A shred can read Max state directly where Max's API supports the audio thread.
`buffer_locksamples` is meant for perform routines. A bound `MaxBuffer` class
could read and write `buffer~` samples from ChucK. Not checked against the Max
SDK.

## Transport sync: `MaxClock`

### What is already synchronized

`ck_perform64` calls `chuck->run(..., n)` once per signal vector. ChucK's `now`
advances by exactly `n` samples per call. ChucK and Max share one sample
clock, so they cannot drift.

### What is missing

ChucK has no access to Max's transport:

- tempo

- beat and bar position

- time signature

- running or stopped

- seek and loop jumps

So a shred cannot start on a Max beat, or follow a tempo change.

### Baseline without `bind()`

Feed `[phasor~ @lock 1]` into a `chuck~` signal inlet. A shred reads it
through `adc` and detects wraps. This is sample-accurate and works today. The
cost: wrap detection needs a `1::samp => now` loop or a UGen chain. It also
uses one inlet per clock signal.

### Proposal

The Max SDK exposes transport state through ITM (`ext_itm.h`):

- `itm_getticks`

- `itm_gettempo`

- `itm_getstate`

- `itm_gettimesignature`

- `itm_tickstoms`

Each block, before `chuck->run()`, `ck_perform64` stores a snapshot:

| Field | Source |
|-|-|
| `ticks` | `itm_getticks` |
| `tempo` | `itm_gettempo` |
| `running` | `itm_getstate` |
| `num`, `denom` | `itm_gettimesignature` |
| `now0` | ChucK `now` at block start |

Bound functions read the snapshot. Inside the block, they extrapolate from
`now0` at the snapshot tempo:

```
ticks(now) = ticks + (now - now0) * tempo / 60 * 480 / sr
```

480 is the default ticks per quarter note (`itm_getresolution`).

ChucK:

```chuck
MaxClock clk;

clk.toNextBeat() => now;            // dur until the next quarter note
while (clk.running()) {
    // play
    clk.beat() => dur b;            // one beat at the current tempo
    b => now;
}
```

| Member | Returns |
|-|-|
| `ticks()` | transport position at `now` |
| `tempo()` | BPM |
| `running()` | transport state |
| `beat()` | `dur` of one beat |
| `toNextBeat()` | `dur` to the next beat boundary |
| `toNext(int ticks)` | `dur` to the next multiple of `ticks` |
| `bar()`, `beatInBar()`, `unit()` | bar, beat and units at `now` (`itm_tickstobarbeatunits`) |

`MaxClock` extends `Event`. The drain step broadcasts it when the snapshot
shows a change:

- start or stop

- tempo change

- a jump that extrapolation did not predict (seek, loop)

A shred waiting on `clk => now` can then reschedule.

Durations computed from `toNextBeat()` are sample-accurate while tempo is
constant. After a tempo change, a pending wait finishes at the old tempo. The
broadcast lets the shred correct on the next beat.

ChucK as tempo master: `Max.send("tempo", 128)` -> `[route msg]` ->
`[transport]`. `ext_itm.h` has no tempo setter.

### Open questions

- **Thread safety.** The SDK headers do not say whether `itm_*` is safe on the
  audio thread. `[phasor~ @lock 1]` reads the transport in its perform routine,
  which suggests it is. Not verified.

- **Scheduler in Audio Interrupt.** With SIAI on, Max runs the scheduler on
  the audio thread before each vector. A snapshot then matches the block
  start. With SIAI off, the scheduler is a separate thread. The snapshot may
  lag by up to one scheduler interval. From memory of Max's documentation, not
  verified.

- **Which ITM.** `itm_getglobal()` is the global transport. Named transports
  (`[transport @name foo]`) need `itm_getnamed()`, e.g. `MaxClock clk("foo")`.

- **Validation.** Compare `clk.ticks()` against `[phasor~ @lock 1]` read
  through `adc`. Run with SIAI on and off, at vector sizes 64 and 512.

## Testing

- Add `examples/test/max_send.ck`. `make test` then compile-checks it. This
  only works after `Max` is bound. The `chuck` CLI does not bind `Max`, so this
  file needs an exclusion in the `test` target.

- Add a test patcher in `patchers/tests/` that routes `msg` to a number box.

- Check that `Max` still resolves after `reset`.
