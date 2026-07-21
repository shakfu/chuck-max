# Logging and Data Output in `chuck~`

Status: items 1 to 3 of the recommendation are **implemented** in both `chuck~`
(Max) and the sibling `pd-chuck`. Items 4 and 5 are not. The analysis sections
below describe the problem as it stood before that change and are kept because
the reasoning still explains why the current design is shaped as it is; the
tables have been updated to current behaviour.

## Purpose

`chuck~` reports to the outside world through several unrelated mechanisms that
have accumulated over time. The addition of a reply outlet makes the overlap
worth settling deliberately rather than by accretion. This document records what
exists today, why parts of it do not reach the user, and what to do about it.

The central question is not "how verbose should the object be" but "which
channel does each kind of output belong on". Those are different questions and
conflating them is how the present situation arose.

## The three output channels

### 1. ChucK VM output, redirected to the Max console

ChucK prints internally through `CK_FPRINTF_STDOUT` / `CK_FPRINTF_STDERR` and
the `EM_log` / `EM_print2*` family. The embedder redirects these with
`ChucK::setStdoutCallback()` and `ChucK::setStderrCallback()`, which `chuck~`
points at `ck_stdout_print` / `ck_stderr_print`. Both call Max's `post()`
unconditionally.

This is the only route for:

- `status` output, produced by `Chuck_VM_Shreduler::status()`
- the `(VM) ...` messages, such as `(VM) removing all (3) shreds...`
- `EM_log` output, gated by ChucK's own log level
- compiler diagnostics

These callbacks are **static**, that is process-wide rather than per-instance.
Every `chuck~` in every patch shares them. Because the wrapper installs the same
two functions from every instance, this currently causes no visible harm, but it
does mean VM output carries no indication of which object produced it.

### 2. ChucK language output (`<<< >>>`), redirected to the Max console

`chout` and `cherr` are the streams behind ChucK's `<<< >>>` operator. They are
redirected with `ChucK::setChoutCallback()` / `setCherrCallback()`, which are
**per-instance**.

These must be registered *after* `ChucK::init()`. Both bail out early and return
`FALSE` unless `m_init` is true and the carrier's `chout` object already exists.
Registering them before `init()` silently does nothing.

Note the naming trap: `setStdoutCallback` and `setChoutCallback` sound like
alternatives but address different streams. Replacing one with the other
silences an entire class of output. This has already happened once and cost a
debugging cycle: `status` produced nothing because the VM's own printing had
been rerouted to a channel it never uses.

### 3. Wrapper-side reporting helpers

Four varargs helpers in `chuck_tilde.cpp` report through Max's object-scoped
console functions. Two of them are gated, on `x->verbose`:

| Helper      | Max function  | Threshold       | Reachable at default? |
|-------------|---------------|-----------------|-----------------------|
| `ck_error`  | `object_error`| none            | yes                   |
| `ck_warn`   | `object_warn` | none            | yes                   |
| `ck_info`   | `object_post` | `verbose >= 1`  | no, `verbose` defaults to 0 |
| `ck_debug`  | `object_post` | `verbose >= 2`  | no                    |

Historically these were gated on `x->loglevel`, which `ck_new` initialises to
`CK_LOG_SYSTEM` (**2**) against thresholds of 4, 5 and 6. 47 of the 118 wrapper
log calls were therefore unreachable unless the user raised the log level by
hand, which also made the VM itself verbose. That is the conflation described in
the next section, now resolved: `verbose` is a separate axis, exposed as an
attribute (`@verbose 0|1|2`, settable as a message too).

`verbose` defaults to **0**, which is what the old gating amounted to in
practice, so the change is behaviour-preserving. What it buys is that the output
is now *reachable* without side effects on the engine.

All four helpers carry `__attribute__((format(printf, 2, 3)))` on GCC and Clang,
so the compiler type-checks each format string against its arguments. This is
worth keeping. When it was first enabled it found thirteen mismatches, mostly
`%d` applied to 64-bit types, and two of those were not formatting faults at all
but truncation bugs in `ck_set` that were corrupting data before it reached the
VM. Unchecked format strings in a file this size hide real defects.

### 4. The reply outlet

The rightmost outlet is a non-signal outlet carrying structured data. It exists
so that information can reach the *patch* rather than only the console.

Current vocabulary:

| Message                    | Emitted by            | Meaning                          |
|----------------------------|-----------------------|----------------------------------|
| `val <name> <value...>`    | `get`                 | value of a ChucK global          |
| `event <name>`             | active `listen`       | a global event fired             |
| `shred add <id>`           | VM shred watcher      | a shred was sporked              |
| `shred remove <id>`        | VM shred watcher      | a shred was removed              |
| `global <name> <type>`     | `globals`             | one entry per global variable    |

The governing rule, in one line: **the selector is always ours, and any
user-supplied name travels as an argument.** See "Selector namespacing" below
for why.

Mechanically, ChucK's globals callbacks and the shred watcher fire on the audio
thread. Replies are therefore written into a per-instance single-producer,
single-consumer ring buffer and flushed from a `qelem` on the main thread. The
ring holds 256 entries of at most 64 atoms. **When the ring is full, replies are
dropped silently.** That is the correct trade in a realtime context, but it is
an observability gap: a patch cannot currently distinguish "no reply" from
"reply discarded under load".

## The `loglevel` conflation (resolved)

This described the defect that motivated the split. Both the primary conflation
and the second-order problem below are now fixed; retained because the reasoning
still explains the current shape.

`x->loglevel` used to carry two unrelated meanings simultaneously:

1. It is passed to `ChucK::setLogLevel()`, controlling how verbose the **ChucK
   VM** is in its own internal logging.
2. It gates the **Max-side** reporting helpers described above.

These are different concerns with different natural defaults. ChucK's default of
`CK_LOG_SYSTEM` (2) is a sensible VM setting. Comparing it against wrapper
thresholds of 4, 5 and 6 guarantees silence. Raising the log level to make the
wrapper talk also makes the VM talk, which is rarely what the user wanted.

Item 3 of the recommendation removed the second meaning: the helpers now gate on
`x->verbose`, and `loglevel` means only what `ChucK::setLogLevel()` means.

A second-order problem used to remain here, and is now also fixed.
`ChucK::setLogLevel()` and `getLogLevel()` are static -- there is one VM log
level shared by every `chuck~` in the process -- but the code kept a per-instance
`x->loglevel` copy alongside it. That copy was the bug, not the staticness:

- Setting `loglevel` on one object changed the level for all of them while
  recording it on only that one, so the other objects' copies went stale.
- Querying `loglevel` with no argument wrote the process value back into the
  queried object's copy, so a read mutated state as a side effect, and the
  reported value looked per-object when it was not.

The per-instance copy has been removed. `ck_loglevel` now reads and writes the
static VM state directly, and reports that the level is process-wide. The
package's preferred default (`CK_LOG_SYSTEM`, since ChucK itself defaults to the
lower `CK_LOG_CORE`) is applied once, by the first instance created, rather than
re-asserted by every new object -- so creating a second `chuck~` no longer
resets a level the user changed on the first. Per-object reporting is what
`verbose` is for; `loglevel` is honestly global.

## Consequences observed in practice

- `status` printed nothing when the VM's stdout redirection was replaced by
  `chout`/`cherr`.
- `abort` appeared to do nothing. Two independent causes: its result was
  reported through `ck_info`/`ck_warn` and so was invisible, and
  `abort_current_shred()` only has a target during a `compute()` cycle, so it
  correctly finds nothing to abort when sent from a message box on a healthy
  patch.
- `tap` and `listen` confirmations have never been visible at the default log
  level, though they are written and correct.
- `removeall` looked broken. It was not: it removes shreds, but global UGens
  belong to the VM rather than to any shred and keep sounding. The absence of
  the `(VM) removing all (N) shreds...` line, suppressed by the stdout
  regression, removed the evidence that would have made this obvious.

The pattern is consistent. Output exists, is correct, and never arrives. Silent
success is indistinguishable from failure, and debugging time is spent
re-deriving behaviour that the object already knew.

## Design principles proposed

1. **Errors and warnings are never gated.** A warning nobody sees is not a
   warning. Max provides `object_error` and `object_warn` precisely so that
   these reach the user regardless of verbosity preferences.
2. **A query must always answer.** `get`, `status`, `param`, `globals`,
   `shreds`, `time`, `vm`, `chugins` are questions. A question that returns
   nothing is indistinguishable from a broken object.
3. **Success is quiet by default.** Max convention is that objects do not
   announce routine success. Chatty objects become unusable in large patches.
4. **Frequency determines the channel.** Anything that can be driven at control
   rate must never post unconditionally.
5. **The console is for humans; the outlet is for patches.** Console text cannot
   be acted upon programmatically. Data a patch might branch on belongs on the
   outlet.

## Recommendation

Not a blanket switch to unconditional reporting. `ck_set` accounts for 7 of the
35 `ck_info` sites and `ck_anything` for 2 more, and those are the
high-frequency control paths. A slider driving `set freq 440` would post
continuously. This is a usability failure rather than a realtime one:
`ck_perform64` contains no logging at all, so nothing posts from the audio
thread, and the cost is console flooding rather than dropouts.

In priority order:

### 1. Make `ck_warn` unconditional -- DONE

Three call sites. Lowest risk change in this document, and it aligns the helper
with what `object_warn` is for.

### 2. Guarantee that queries answer -- DONE

Audit the query commands so that none depend on `ck_info`. Most already bypass
it; `status` depends instead on the VM stdout redirection, which must therefore
stay registered.

### 3. Separate Max-side verbosity from ChucK's log level -- DONE

Introduce a `@verbose` attribute (off by default) that gates `ck_info` only, and
let `loglevel` mean solely what ChucK means by it. This resolves the conflation
directly and removes the need to raise VM verbosity to obtain wrapper feedback.

While doing this, make the `loglevel` query stop writing the process-wide value
back into the instance, and document that setting it affects all instances.

### 4. Leave `ck_debug` gated -- DONE (now on `verbose >= 2`)

It is doing its job.

### 5. Consider moving confirmations to the reply outlet -- NOT DONE

The larger question. Configuration commands such as `tap`, `listen` and
`unlisten` currently confirm to the console, where the confirmation is
unreachable programmatically. Emitting a structured confirmation on the reply
outlet instead would be machine-readable, would not clutter the Max window, and
would need no verbosity setting at all.

This widens the reply-outlet vocabulary, so it should be designed as a whole
rather than added piecemeal. A sketch:

| Message                      | Meaning                              |
|------------------------------|--------------------------------------|
| `tap <outlet> <ugen> <n>`    | tap assignment confirmed             |
| `listen <name> 1`            | listener registered                  |
| `listen <name> 0`            | listener removed                     |
| `srate <rate>`               | VM sample rate changed               |
| `shreds <count>`             | shred count after add/remove         |

Namespace collision, which would otherwise block this, is already resolved by
the `val` prefix. See "Selector namespacing" below. One question remains open:

- **Dropped replies.** The ring buffer discards silently when full. If the
  outlet becomes the primary reporting channel, it needs a way to report its own
  overflow, for example a `dropped <count>` message emitted once the queue
  drains.

## Selector namespacing

A single outlet carrying both ChucK data and wrapper notifications needs a rule
for telling the two apart. The rule adopted is that **the selector is always
drawn from the wrapper's vocabulary, and any user-supplied name appears as an
argument**.

Only `get` replies ever violated this. `globals` and `event` both carry a
user-chosen name (`counter`, `tick`) and both already placed it in an argument
position, keeping a fixed word in the selector. `get` alone promoted
user-controlled text into the selector, which is where the collision came from:
a ChucK file declaring `global int shred;` would produce `shred 5`, competing
with the watcher's `shred add 3`. Making `get` conform resolves it.

### The rule applies one level down as well

Namespacing the selector is necessary but not sufficient. When a patch strips the
selector with `[route val event shred global]`, **the next atom becomes the new
selector**, and Max dispatches on it. If that atom happens to name one of Max's
typed methods, the receiving object rejects the message.

This was not theoretical. `globals` originally emitted `global <type> <name>`.
After `[route global]` that left `int counter`, a message whose selector is `int`
carrying a symbol, and any object with a typed `int` method answers with
`bad arguments for message "int"`. ChucK's `int` and `float` type names failed
this way while `string`, `Event`, `SinOsc`, `Pan2` and `@array` passed, because
only the first two collide with Max method names.

The message is now `global <name> <type>`. A ChucK variable cannot be named `int`
or `float`, since those are reserved words in ChucK, so leading with the name
puts a token there that is guaranteed not to collide with Max's numeric methods.
It also matches the shape of `val <name> ...`.

Residual risk, accepted: a ChucK global named after a non-type Max method, such
as `bang`, `set` or `clear`, would still land in selector position after routing.
These are legal ChucK identifiers but unusual variable names, and the exposure is
identical for `val` and `global`, so the protocol is at least uniform. Removing
the last of the risk would mean never placing user text in the second atom
either, which costs another level of nesting for little practical gain.

Consequences of the rule:

- **No reserved words.** Not one, zero. A ChucK global may be named `shred`,
  `event`, `global` or `val` without ambiguity, because user names never reach
  the selector position.
- New control messages can be added freely later. They live in a namespace the
  user cannot enter, so extending the protocol can never invalidate existing
  ChucK code.
- Value replies cost one extra routing stage: `route val` feeding
  `route freq gain`, rather than routing on the variable name directly.

### Alternatives rejected

- **Reserving `shred`, `tap` and similar as forbidden global names.**
  Unenforceable, since the wrapper does not control the ChucK compiler and
  `eval` can introduce globals at runtime. Worse, the reserved list would grow
  with the message vocabulary, so adding a control message later would
  retroactively break patches whose ChucK code used that name.
- **Prefixing the wrapper messages instead (`ck shred add 3`), leaving `get`
  bare.** Reshapes the three messages that were already correct, leaves the
  anomalous one untouched, and still reserves one token (`ck`) out of the user's
  namespace.
- **`value` as the prefix.** `value` is an existing Max object class, so the
  word is loaded in this context.
- **A symbol prefix such as `= freq 330.`.** `=` is already bound to `replace`
  in the incoming shorthand vocabulary handled by `ck_anything`, alongside `+`
  (add), `-` (remove), `--` (remove last) and `^` (status). Reusing it outbound
  would give one token two unrelated meanings depending on direction. The rest
  of the symbol space is largely spoken for: `@` is attributes, `#` is patcher
  arguments, `$` is message-box arguments, `;` and `,` are message-box
  separators.
- **A second data outlet, values on one and notifications on the other.**
  Semantically the cleanest and needs no reserved token, but outlet count
  already varies with `channels` and `ntap`, and it pushes patching burden onto
  every user to solve what one prefix handles.
- **Including the type, as `val float freq 440.`.** Symmetric with the shape
  `globals` uses and makes replies self-describing, but lengthens the route
  expression and largely duplicates information the patch already has, since
  the `get` that requested it named the type.

## Relationship to pd-chuck

The sibling project `pd-chuck` embeds the same engine behind the same message
vocabulary, and the two externals are kept deliberately close. On the subjects
of this document they agree in design but differ in two details, both on
purpose. Recorded here so the differences read as decisions rather than drift.

### The logging split: converged, with different defaults

`pd-chuck` got this first, because it had no reporting helpers at all -- every
diagnostic was a raw, ungated `post()` or `pd_error()` -- which made it a clean
slate. `chuck~` followed. Both now separate host-side verbosity from the ChucK
VM's log level, and both leave errors and warnings ungated.

The one substantive difference is the **default**, and it differs for the same
reason in each case: preserve what that external already did.

| | chuck-max (Max) | pd-chuck (Pd) |
|---|---|---|
| Control | `@verbose` attribute, also settable as a message | `verbose` message (Pd has no attributes) |
| Default | `0` | `1` |
| Why that default | `ck_info` was gated at `loglevel >= 5` against a default of 2, so it was already silent | every message was already printed unconditionally |

So neither change altered observable behaviour on the day it landed. Both are
now a one-line edit away from the quiet-on-success default that principle 3
argues for, whenever that is decided deliberately rather than as a side effect.

Both carry `__attribute__((format(printf, 2, 3)))` on the four helpers.

### The deferral mechanism differs of necessity

Not a decision so much as a constraint, but it is the other place the two files
are structurally different and it should not be mistaken for carelessness.

The reply ring buffer is identical in both -- plain C++ atomics, no host API.
What differs is how the drain is scheduled:

- **Max** uses `qelem_set()`, which is documented safe to call from the audio
  thread.
- **Pd** has no equivalent. `outlet_anything()` from the perform routine would
  be reentrant into the DSP graph being traversed, and `clock_delay()` mutates
  the scheduler's clock list, which is only safe when DSP runs on the scheduler
  thread. Pd can run audio in callback mode, and there is no public API to ask
  which mode is active. So `pd-chuck` uses a clock on the scheduler thread that
  polls the ring every 20 ms.

The Pd clock only runs while replies are enabled, which is the other difference:
the Pd reply outlet is **opt-in** via a `reply 0|1` message, where the Max one
is always live. Making it optional there removed the objection to a permanently
running timer.

Both sites carry a comment pointing at the other.

## Non-goals

- Making `chuck~` verbose by default.
- Removing console output. It remains the right channel for errors, for ChucK
  language output, and for human-readable dumps such as `status` and `param`.
- Changing ChucK's own logging.

## Verification notes

Claims in this document that were checked rather than assumed:

- Log level constants are defined in `chuck_errmsg.h`: `CK_LOG_NONE` 0 through
  `CK_LOG_ALL` 10, with `CK_LOG_SYSTEM` 2, `CK_LOG_INFO` 5, `CK_LOG_DEBUG` 6.
- `setChoutCallback` returning `FALSE` before `init()` is visible in
  `chuck.cpp`, which checks `m_init` and the carrier's `chout` pointer.
- Call-site counts were taken by grep over `chuck_tilde.cpp`.
- The absence of logging in `ck_perform64` was confirmed by inspection.
- That `removeall` removes shreds while global UGens keep sounding was confirmed
  empirically with a standalone harness linked against `libchuck.a`, measuring
  output peak before and after: shreds went 1 to 0 while peak stayed at 0.5,
  whereas `CK_MSG_CLEARVM` took peak to 0.
