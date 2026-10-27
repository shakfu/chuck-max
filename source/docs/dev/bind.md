# Host-defined ChucK classes via `ChucK::bind()`

Status:

- `Host.msg()`: implemented in `chuck_tilde.cpp`. Passes the manual Max test
  (Max 9.2.0, macOS arm64, 2026-10-23).

- `HostClock`: implemented in `chuck_tilde/maxclock.cpp`. Passes `bind_test`.
  Passes the manual Max test in `test_max_clock.maxpat` (Max 9.2.0, macOS
  arm64, 2026-10-23): beat clicks, tempo change, stop/start, seek to 0.

- `HostIn` / `HostMsg`: implemented in `chuck_tilde/maxin.cpp`. Passes
  `bind_test` and the manual Max test in `test_max_in.maxpat` (Max 9.2.0,
  macOS arm64, 2026-10-23).

- `HostBuffer`: implemented in `chuck_tilde/maxbuffer.cpp`. Passes
  `bind_test` and the manual Max test in `test_max_buffer.maxpat` (Max 9.2.0,
  macOS arm64, 48 kHz, 2026-10-23).

- `HostBufPlay` UGen: implemented in `chuck_tilde/maxbuffer.cpp`. Passes
  `bind_test` and the manual Max test in `test_max_buffer.maxpat` (Max 9.2.0,
  macOS arm64, 2026-10-23).

Naming: the classes use host-neutral names (`Host`, `HostIn`, `HostMsg`,
`HostBuffer`, `HostBufPlay`, `HostClock`), shared with numchuck and pd-chuck,
so one `.ck` file can run in each. They were `Max`, `MaxIn`, and so on until
2026-10-24, and `Host.send(name, value)` became `Host.msg` on 2026-10-27. The
manual Max tests in `patchers/tests/` were rerun after both renames and pass
(2026-10-27).

## Purpose

`ChucK::bind()` registers native classes from the host into a ChucK VM.
`chuck~` does not call it. This document proposes a `Host` class.
`Host.msg()` lets a shred push a message out of the `chuck~` reply outlet.

Today, pushing a value from ChucK to Max takes four steps:

1. Declare a `global float`.
2. Declare a `global Event`.
3. Send `listen` to the event.
4. Answer each event with a `get` round trip.

`Host.msg()` does this in one call.

The same mechanism supports four bindings:

| Direction | Binding | Delivery |
|-|-|-|
| ChucK to Max | `Host.msg()` | queued, next main-thread tick |
| Max to ChucK | `HostIn` / `HostMsg` | queued, next audio block |
| ChucK reads Max | `HostBuffer` | immediate, audio thread |
| Transport sync | `HostClock` | snapshot per block, extrapolated per sample |

`Host.msg()` is specified in full below. The others are outlines.

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
    Host.msg("freq", f);     // reply outlet: msg freq 437.2
    250::ms => now;
}
```

Max: `[chuck~]` rightmost outlet -> `[route msg]` -> `[route freq]`.

`Host.msg` is overloaded by argument type:

```chuck
Host.msg("done");                 // msg done
Host.msg("freq", 440.0);          // msg freq 440.
Host.msg("file", "kick.wav");     // msg file kick.wav
Host.msg("pair", [440.0, 0.5]);   // msg pair 440. 0.5   (at most 63 values)
```

An empty list sends `msg <name>`, as `sendList` sends bang. An int argument
promotes to the float overload.

`Host.msg` goes to the reply outlet; `Host.send*` goes to `[receive]`. The
name is the outlet selector, and mirrors `HostIn`, which receives
`msg <name> <args...>`; `send*` keeps the `[send]`/`[receive]` idiom. The
call was `Host.send(name, value)`, float only, until 2026-10-27.

## Implementation

In `chuck_tilde.cpp`:

```cpp
static t_symbol* ps_msg = NULL;      // msg <name> <value>      -- Host.msg()

// find the chuck~ that owns this VM; audio-thread safe (atomic slot reads)
static t_ck* ck_instance_from_vm(Chuck_VM* vm)
{
    for (long i = 0; i < CK_MAX_INSTANCES; i++) {
        t_ck* x = CK_INSTANCE_SLOTS[i].load(std::memory_order_acquire);
        if (x && x->chuck && x->chuck->vm() == vm) return x;
    }
    return NULL;
}

// Host.msg(string name, float value); runs on the audio thread. the bang,
// string and float[] overloads are three more sfuns under the same name
CK_DLL_SFUN(max_msg_float)
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
    QUERY->begin_class(QUERY, "Host", "Object");
    QUERY->doc_class(QUERY, "messages from ChucK to the chuck~ reply outlet");
    QUERY->add_sfun(QUERY, max_msg_float, "void", "msg");
    QUERY->add_arg(QUERY, "string", "name");
    QUERY->add_arg(QUERY, "float", "value");
    QUERY->end_class(QUERY);
    return TRUE;
}
```

Registration:

- `ext_main`, beside `ps_val = gensym("val")`: `ps_msg = gensym("msg");`
- `ck_new`, after `x->chuck->init()`: `x->chuck->bind(ck_max_query, "Host");`

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
  (`chuck_vm.cpp`). `Host` lives in the global namespace, so it survives.
  `bind_test` confirms this.

## Caveats

- **`gensym` on the audio thread.** `gensym()` takes a lock.
  `cb_get_all_global_vars` already does this. To avoid it, copy the name into
  the queue entry and call `gensym` in `ck_reply_drain`.

- **Timing.** The message is timed in ChucK logical time. Max delivers it on the
  next main-thread tick. Fine for control data. Not sample-accurate.

- **Overflow.** `ck_reply_push` drops the message when the queue is full. It
  does not report the drop. A shred that sends every sample will fill the queue.

## `Host.sendBang` / `sendFloat` / `sendSymbol` / `sendList`

Status: implemented in `chuck_tilde/maxsend.cpp`, ported from pd-chuck.
Passes `bind_test` and the manual Max test in `test_max_send_receive.maxpat`
(Max 9.2.0, macOS arm64, 2026-10-27).

```chuck
Host.sendBang("go");                  // every [receive go] gets bang
Host.sendFloat("freq", 440.0);        // ... float 440.
Host.sendSymbol("file", "kick.wav");  // ... symbol kick.wav
Host.sendList("xy", [0.2, 0.7]);      // ... list 0.2 0.7
```

Example: `examples/max/max_send_receive.ck`. Unlike `Host.msg`, these need
no patch cord to `chuck~` and no `[route msg]`.

- The four calls share one queue per instance. It holds 256 calls between
  drains; further calls are dropped, and the drain posts a warning with the
  count.
- `sendList` sends at most 32 elements; the drain warns about each cut list.
  An empty list sends bang, as Pd delivers it.
- `sendSymbol` sends the `symbol` selector, so `[receive]` outputs
  `symbol kick.wav`, as in Pd. Sending the symbol as the selector would be
  more usual in Max, but a word such as `set` or `clear` would then act as a
  message.
- A name with no `[receive]` at delivery is skipped, as `[send]` skips it.
- Receive names are shared by the whole Max process. Two `chuck~` objects, or
  two copies of an abstraction, that call `sendFloat("freq", ...)` reach the
  same `[receive freq]`. `Host.msg` goes to the sending object's outlet
  only. Give each copy its own receiver name when they must not collide.
- Delivery runs on the main thread, from the reply qelem, as for `Host.msg`.
  Latency is the main-thread jitter, not the scheduler's. Use these calls for
  display and state, not as a clock: `sendBang("tick")` on each beat arrives
  late by a variable amount.
- Lists hold floats only. ChucK arrays are homogeneous, so a mixed list such
  as `["note", 60]` has no ChucK type, and the queue entry is a fixed
  `double[32]` so the audio thread allocates nothing. Send a string with
  `sendSymbol`. A `string[]` overload of `sendList` is the next step if one
  host needs it.

### Implementation

`maxsend.cpp` has no Max dependency. The shred pushes onto a single-producer,
single-consumer queue on the audio thread. The reply qelem drains it on the
main thread, after the `Host.msg` replies, and `ck_send_deliver` calls
`typedmess` on the receiver name's `s_thing`.

- Delivery uses the main thread, as `Host.msg` does. Calling `typedmess` from
  the audio thread would break Max's threading rules. pd-chuck instead
  delivers after the current DSP tick, from a 0 ms clock.
- `s_thing` is the binding a message box's `;` syntax sends to. The SDK
  headers describe it only as a "possible binding to a t_object"
  (`ext_mess.h`). That `[receive]` binds there is inferred from Max
  externals practice, not documented. `NOGOOD` guards a stale pointer.
- Another object bound to the name, such as a `buffer~`, receives the
  message too, as it would from `[send]`.
- `gensym` runs on the audio thread for the receiver and symbol names, as for
  `Host.msg` (see Caveats).
- Ordering is kept within the four calls, not between them and `Host.msg`,
  which uses the reply queue.

## Extensions

The same pattern covers:

- `Host.post(string)` to the Max console.

## Signals (audio and CV)

`Host.msg()` does not suit audio or CV. It delivers at main-thread tick rate
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

For CV work, transport sync matters more. See `HostClock` below.

## Max to ChucK: `HostIn` / `HostMsg`

Max-to-ChucK without `HostIn` goes through `ck_anything`:

- `<name> <value>` sets a ChucK global.

- `<name> <a b c>` sets a global array. The first atom's type sets the array
  type.

- `sig` and `broadcast` fire a `global Event`.

Gaps:

- Sending a value and waking a shred takes two messages: `freq 440`, then
  `sig freqChanged`.

- Mixed-type lists such as `note 60 0.8 legato` are not supported.

- Every name must be a declared `global`.

`HostIn` and `HostMsg` follow ChucK's `OscIn` and `OscMsg`
(`examples/max/max_in.ck`):

```chuck
HostIn min;
min.addAddress("note");
HostMsg msg;
while (true) {
    min => now;                     // HostIn extends Event
    while (min.recv(msg)) {
        msg.getInt(0) => int pitch;
        msg.getFloat(1) => float vel;
        msg.getString(2) => string art;
    }
}
```

Max: `[msg note 60 0.8 legato(` -> `[chuck~]`.

API:

- `HostIn.addAddress(string)`: receive messages with this name. Callable more
  than once.
- `HostIn.recv(HostMsg)`: move the oldest message into the `HostMsg`; 0 if none.
- `HostMsg.name()`, `numArgs()`, `getInt(i)`, `getFloat(i)`, `getString(i)`.

`getInt` and `getFloat` convert between int and float, since Max sends `60`
and `60.` interchangeably. A missing argument or a type mismatch with a string
reads as 0 or `""`.

`msg` is an inlet selector. It matches the `msg` reply selector of
`Host.msg()`. A ChucK global named `msg` can no longer be set by name; use
`set`.

### Implementation

`chuck_tilde/maxin.cpp`, no Max dependency. `chuck_tilde.cpp` only converts
atoms (`ck_msg`), drains, and owns the state.

1. `ck_msg` runs on the main or scheduler thread. It copies the atoms into a
   per-instance ring of 128 messages. A mutex serializes the two producer
   threads; the audio thread never takes it.

2. `ck_perform64` calls `maxin_drain` before `chuck->run()`. Each message is
   copied to every `HostIn` listening for its name, which then broadcasts.

3. The shred wakes inside the same `run()`, at the block's first sample.

Draining in `ck_perform64` is preferred over `API->vm->queue_event()`
(`chuck_dl.h`), which `OscIn` and MIDI use. With `queue_event`, each `HostIn`
needs a mutex, and `recv()` takes it on the audio thread. Either way, delivery
is at the next block boundary.

String arguments are stored as `t_symbol` name pointers. Max never frees
symbols, so no copy is made on the audio thread.

Limits:

- 32 arguments per message. `ck_msg` rejects longer messages with an error.
- 128 messages queued between blocks. `ck_msg` warns when the ring is full,
  e.g. with DSP off.
- 64 unread messages per `HostIn`. Further messages to it are dropped.

### Lifecycle

- A `HostIn` unregisters in its destructor, on the VM thread. This covers an
  object freed while its shred still runs.

- A removed shred never releases its locals. `Chuck_VM_Shred::shutdown()`
  frees the stacks without releasing the objects on them. Those objects leak,
  and their destructors never run. `bind_test` confirms this for `reset`,
  `remove <id>` and `removeall`. A `REMOVE` shred watcher (`maxin_watcher`)
  therefore unregisters every `HostIn` created by the removed shred.

- The watcher cannot tell whether an object outlived its shred, e.g. through
  a `global`. Such an object stops receiving when its creating shred ends.

- An unregistered `HostIn` is never freed by the watcher. Its object may still
  be referenced, so `recv()` stays safe and returns 0.

## ChucK reads Max: `HostBuffer`

`HostBuffer` reads and writes the samples of a named `buffer~`
(`examples/max/max_buffer.ck`, `max_buffer_play.ck`):

```chuck
HostBuffer b;
b.name("ckbuf");
while (!b.ready()) 1::ms => now;    // reference made on the main thread
b.peek(0, 1) => float x;            // frame 0, channel 1
b.poke(100, 0.5);                   // frame 100, channel 0
```

API:

- `name(string)`, `name()`: the `buffer~` name.
- `ready()`: 1 if the `buffer~` exists.
- `frames()`, `channels()`, `sampleRate()`: 0 if not ready.
- `peek(frame)`, `peek(frame, chan)`: 0 if out of range or not ready.
- `poke(frame, value)`, `poke(frame, chan, value)`: ignored if out of range.

Channels count from 0, as ChucK arrays do. `peek~` counts from 1.

### Implementation

`chuck_tilde/maxbuffer.cpp`, no Max dependency. `chuck_tilde.cpp` supplies
the Max calls through `MaxBufferHost`.

- `name()` claims a slot in a per-instance table of 64 names. The main thread
  then calls `buffer_ref_new`. `ready()` stays 0 until that runs, a few ms
  later. The SDK documents no thread rules for `buffer_ref_new`, and it
  registers with the `buffer~`, so it is kept off the audio thread.

- Slots outlive shreds and `reset`. A name used before is ready at once.

- Each `peek`, `poke` and query locks the buffer and unlocks it again:
  `buffer_ref_getobject`, `buffer_locksamples`, `buffer_unlocksamples`. This
  follows the `index~` example of the full Max SDK. That example is not in
  `max-sdk-base`; the citation is from memory.

- `poke` sets a flag. The main thread calls `buffer_setdirty`, once per
  service pass, so `buffer~` views redraw. `ext_buffer.h` does not say
  whether `buffer_setdirty` is safe on the audio thread.

- `chuck~` gained a `notify` method that forwards to `buffer_ref_notify`.
  `ext_buffer.h` requires this forwarding (`buffer_ref_notify`). Without it,
  a reference does not follow a `buffer~` created or freed after `name()`.

- The main-thread work runs in the reply qelem, after the reply drain.

Cost: about three calls into Max per `peek`, plus a shred step per sample if
a shred reads every sample. For playback, use `HostBufPlay`.

### `HostBufPlay` UGen

Plays one channel of a `buffer~` (`examples/max/max_buffer_play.ck`):

```chuck
HostBufPlay p => dac;
p.name("ckbuf");        // restarts from frame 0, as SndBuf.read does
1 => p.loop;
-0.5 => p.rate;         // half speed, backward
```

- `name`, `ready()`, `frames()`: as `HostBuffer`. Both classes share the
  per-instance name slots.
- `pos` (frames), `rate`, `loop` (default 0), `interp` (1 linear, the
  default; 0 none), `channel` (from 0).
- Without `loop`, playback stops at either end and outputs 0.
- A `buffer~` at another sample rate plays at its own pitch: the step is
  `rate * buffer_sr / vm_sr`, as `SndBuf` does for files.
- Silent while the `buffer~` is missing. The position holds, so playback
  resumes where it was.

The tick locks and unlocks the buffer each sample, as `peek` does. MSP
perform routines such as the SDK's `index~` lock once per signal vector (same
caveat as above). A ChucK UGen ticks one sample at a time, so a
per-block lock would need a host hook after `run()`; not built. Mono only;
stereo takes two instances.

## Transport sync: `HostClock`

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

### Implementation

`maxclock.cpp` has no Max dependency. Each block, before `chuck->run()`,
`ck_clock_update` fills a `MaxClockSnapshot` from the global ITM
(`itm_getticks`, `itm_getstate`, `itm_tickstoms`, `itm_gettimesignature`)
and passes it to `maxclock_update`. `bind_test` passes a simulated transport
instead.

Between snapshots, positions are extrapolated from the block start at the
snapshot tempo. They use the shred's own `now`, not `vm->now()`. ChucK keeps
sub-sample logical time: after `500.3::samp => now`, `now` is 500.3, while
the VM clock reads 500.

ChucK:

```chuck
HostClock clk;
while (true) {
    clk.toNextBeat() => now;
    <<< clk.bar(), clk.beatInBar() >>>;
}
```

| Member | Returns |
|-|-|
| `ticks()` | transport position at `now` |
| `tempo()` | BPM |
| `running()` | 1 if running |
| `beat()` | `dur` of one beat |
| `toNextBeat()` | `dur` to the next beat boundary |
| `toNext(float ticks)` | `dur` to the next multiple of `ticks` |
| `bar()`, `beatInBar()` | bar and beat at `now`, from 1 |
| `unit()` | ticks into the current beat |

Semantics:

- `toNext` never returns zero. On a boundary it returns a full period, so a
  `toNextBeat() => now` loop cannot stall. A shred that starts exactly on a
  beat must play that beat itself; `examples/max/max_clock.ck` checks
  `unit()`.

- A stopped transport answers `toNext` as if it started now. `ticks()` stays
  frozen.

- A beat is `resolution * 4 / denom` ticks, so 6/8 counts eighth notes.

`HostClock` extends `Event`. `maxclock_update` broadcasts on:

- start or stop

- tempo change

- time signature change

- a jump of more than 0.5 samples from the extrapolated position (seek, loop,
  audio restart)

A wait computed before a tempo change finishes at the old tempo. The idiom in
`examples/max/max_clock.ck` restarts its beat loop on each broadcast.

Lifecycle: a removed shred leaks its `HostClock` (see `HostIn` above), so the
object stays in `MaxClockState::clocks`. Broadcasting it is safe.
`Chuck_VM_Shreduler::remove_blocked` takes the removed shred off the event's
wait queue, so the broadcast wakes nothing. `HostClock` therefore needs no shred
watcher, and a `global HostClock` keeps working after its creating shred ends.

ChucK as tempo master: `Host.msg("tempo", 128)` -> `[route msg]` ->
`[transport]`. `ext_itm.h` has no tempo setter.

### Open questions

- **Thread safety.** The SDK headers do not say whether `itm_*` is safe on the
  audio thread. `clockprobe` calls `itm_getticks`, `itm_getstate` and
  `itm_tickstoms` every block without fault (see Results). This is evidence,
  not a guarantee.

- **Scheduler in Audio Interrupt.** Answered: no effect. The transport
  advanced exactly with the audio frame count in all four runs.

- **Which ITM.** `itm_getglobal()` is the global transport. Named transports
  (`[transport @name foo]`) need `itm_getnamed()`, e.g. `HostClock clk("foo")`.

- **Jumps.** Switching SIAI made the transport jump 334.67 samples in one
  block. Tempo changes, seeks and loops should jump too. `HostClock` should
  broadcast when a block's tick advance differs from its frame count by more
  than 0.5 samples.

- **Inside the block.** `examples/max/max_clock_check.ck` reads
  `[phasor~ 4n @lock 1]` at each beat a shred wakes on. The phasor wraps at
  the beat, so its value is the timing error. Run with
  `patchers/tests/test_max_clock.maxpat`.

### Prototype: `clockprobe`

`chuck~` has a `clockprobe [ticks]` message. It answers the first two open
questions before `HostClock` is built. `clockprobe 0` stops it.

Each block, before `chuck->run()`, `ck_clockprobe_measure` reads
`itm_getticks` and `itm_getstate` on the global ITM. It compares the transport
phase with frame 0 of inlet 1, which should carry `[phasor~ <ticks> @lock 1]`.
Once per second of audio, the reply outlet sends:

```
clockprobe <blocks> <mean> <min> <max> <tempo> <running> <ticks> <jmin> <jmax>
```

`mean`, `min` and `max` are the signed error in samples. Positive means the
transport reads ahead of `phasor~`. `phasor~ @lock 1` reads the same
transport, so these cannot expose transport jitter.

`jmin` and `jmax` compare each block's tick advance, in samples, with the
frames that block rendered. Non-zero values are transport jitter or a jump.

Tempo comes from `itm_tickstoms`, not `itm_gettempo`. `itm_gettempo` is
declared in `ext_itm.h` but missing from the SDK's `MaxAPI` stub. Max 9.2.0
exports it; older versions may not, and a missing import stops `chuck~` from
loading.

Test patcher: `patchers/tests/test_clockprobe.maxpat`.

### Results

Max 9.2.0, macOS arm64, 48 kHz, 120 BPM, 2026-10-23:

| SIAI | Vector size | mean, min, max | jmin, jmax |
|-|-|-|-|
| on | 64 | 0 | 0 |
| off | 64 | 0 | 0 |
| on | 512 | 0 | 0 |
| off | 512 | 0 | 0 |

0 means below print precision. One outlier: switching SIAI off to on at
vector size 512 gave `jmax` 334.67 for one report. The next report was 0.

Conclusions:

- `[phasor~ @lock 1]` phase equals `fmod(ticks, period) / period`.

- The transport advances exactly with audio frames, with SIAI on or off. A
  block-start snapshot is exact.

- Audio reconfiguration causes a one-off jump, not drift.

## Testing

`source/projects/tests/bind_test.cpp` tests the engine side without Max.
`make test` runs it. It binds the real `HostClock`, `HostIn`, `HostMsg` and
`HostBuffer` (against a fake `buffer~` host), and
stand-ins for `Host` (`Probe`) and an early `HostIn` (`ProbeIn`). It checks:

| Case | Result |
|-|-|
| `Probe.send()` reaches the host with name and value | pass |
| The `VM` argument equals `chuck->vm()` | pass |
| Calls run at the shred's logical time (`now` 0 and 100) | pass |
| Two instances: each call names its own VM | pass |
| `Probe` still compiles after `CK_MSG_CLEARVM` | pass |
| An instance without `bind()` rejects `Probe` | pass |
| `deliver()` before `run()` wakes the shred in that `run()`, at the block's first sample | pass |
| Normal shred exit runs the `ProbeIn` dtor | pass |
| An object freed while its shred runs leaves the registry | pass |
| `CK_MSG_CLEARVM`, `CK_MSG_REMOVE <id>`, `CK_MSG_REMOVEALL` on a blocked shred: dtor does not run; the watcher clears the entry | pass |
| `HostClock.ticks()` extrapolates within a block at the snapshot tempo | pass |
| `toNextBeat() => now` lands on the boundary, including at a fractional 500.3 samples | pass |
| `toNextBeat()` on a boundary returns a full beat, not zero | pass |
| Stopped: `ticks()` frozen; `toNextBeat()` as if started now | pass |
| Broadcast on tempo change (at the block's first sample), seek, stop, time signature | pass |
| No broadcast for 0.2 samples of drift | pass |
| `bar()`, `beatInBar()`, `unit()` in 4/4 and 6/8 | pass |
| A jump in one instance does not wake the other | pass |
| A released `HostClock` leaves `clocks`; broadcasting a leaked one wakes nothing | pass |
| `HostIn`: `note 60 0.8 legato` arrives with name, count and typed arguments, in order, at the block's first sample | pass |
| `HostIn`: int/float conversion; missing or mismatched arguments read 0 or `""` | pass |
| `HostIn`: two addresses on one object; two objects on one address; other names ignored | pass |
| `HostIn`: a message with no listener is dropped, not held for a later `HostIn` | pass |
| `HostIn`: ring holds 127; 64 unread per object; arguments cut at 32 | pass |
| `HostIn`: normal exit and release while the shred runs unregister | pass |
| `HostIn`: `CK_MSG_CLEARVM`, `CK_MSG_REMOVE <id>`, `CK_MSG_REMOVEALL`: the watcher unregisters; a new receiver gets one copy | pass |
| `HostIn`: a `global HostIn` stops receiving when its creating shred ends (limitation) | pass |
| `HostIn`: a message to one instance does not reach the other | pass |
| `HostBuffer`: not ready until the main-thread service; then frames, channels, sample rate | pass |
| `HostBuffer`: interleaved `peek`/`poke`; out-of-range reads 0 and writes nothing, checked with guard samples | pass |
| `HostBuffer`: one dirty notice per service pass after writes; one request per new name and per first write | pass |
| `HostBuffer`: a `buffer~` created after `name()` becomes ready; a deleted one reads 0 | pass |
| `HostBuffer`: slots reused by name, across `reset`; the 65th name is refused | pass |
| `HostBuffer`: no state (host tearing down): every call is a no-op | pass |
| `HostBuffer`: every lock is unlocked; instances do not share slots | pass |
| `HostBufPlay`: exact output sequences for one-shot, loop, half rate with and without interpolation, backward | pass |
| `HostBufPlay`: interpolation wraps at the loop point and holds the last frame without loop | pass |
| `HostBufPlay`: channel select; out-of-range channel is silent; 22050 Hz buffer plays at half speed | pass |
| `HostBufPlay`: silent while missing, position held; resumes when the buffer~ appears; silent when deleted | pass |
| `HostBufPlay`: `name()` restarts from frame 0; shares slots with `HostBuffer` | pass |
| Every `examples/max/max_*.ck` compiles with the host classes bound | pass |

The Max side is a manual run of `patchers/tests/test_max_send.maxpat`,
which uses `examples/max/max_send.ck` and `examples/max/max_send_once.ck`.
All three checks passed on Max 9.2.0, macOS arm64, 2026-10-23:

- `msg freq <value>` arrives four times per second.

- `Host.msg` still works after `reset`.

- Two `chuck~` instances keep their messages apart.

`HostIn` has a manual test in `patchers/tests/test_max_in.maxpat`, which uses
`examples/max/max_in.ck`. All five steps passed on Max 9.2.0, macOS arm64,
2026-10-23: notes and `got` echo, legato without retrigger, `off`, no
delivery after `removeall`, one copy per note after `reset`.

`HostBuffer` has a manual test in `patchers/tests/test_max_buffer.maxpat`, which
uses `examples/max/max_buffer.ck` and `max_buffer_play.ck`. All steps passed
on Max 9.2.0, macOS arm64, 48 kHz, 2026-10-23: write (48000 frames), loop
playback, `replace` with a sound file, delete the `buffer~` (silence), undo
and rewrite (playback resumes). A second run, with `max_buffer_play.ck` using
`HostBufPlay`, also passed, including `msg rate 0.5`, `1` and `-1`.

`Host.send*` has a manual test in `patchers/tests/test_max_send_receive.maxpat`,
which uses `examples/max/max_send_receive.ck`: four `[r]` receivers with no
cord from `chuck~`, a second instance reaching the same `[r freq]`, and
silence after `removeall`. All three passed on Max 9.2.0, macOS arm64,
2026-10-27.

The `chuck` CLI cannot compile `examples/max/max_*.ck`, because it binds no
host classes. `bind_test` compiles them instead.
