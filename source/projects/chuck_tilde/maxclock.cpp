#include "maxclock.h"

#include "chuck_oo.h"
#include "chuck_vm.h"

#include <algorithm>
#include <cmath>

// jumps below this are float noise, not transport events
static const double MAXCLOCK_JUMP_SAMPLES = 0.5;

static f_maxclock_lookup g_lookup = NULL;

void maxclock_set_lookup(f_maxclock_lookup lookup)
{
    g_lookup = lookup;
}

double maxclock_ticks_at(const MaxClockSnapshot& snap, t_CKTIME now)
{
    return snap.running ? snap.ticks + (now - snap.now) * snap.tps : snap.ticks;
}

bool maxclock_update(MaxClockState* state, const MaxClockSnapshot& snap)
{
    if (!state->valid) {
        state->cur = snap;
        state->valid = true;
        return false;
    }
    const MaxClockSnapshot& prev = state->cur;
    bool changed = snap.running != prev.running
                || std::fabs(snap.tps - prev.tps) > 1e-9 * prev.tps
                || snap.num != prev.num || snap.denom != prev.denom;
    if (!changed && snap.tps > 0) {
        double drift = (snap.ticks - maxclock_ticks_at(prev, snap.now)) / snap.tps;
        changed = std::fabs(drift) > MAXCLOCK_JUMP_SAMPLES;
    }
    state->cur = snap;
    if (changed) {
        // only moves waiting shreds to the shreduler; runs no ChucK code, so
        // the vector cannot change underneath this loop
        for (Chuck_Object* obj : state->clocks) {
            ((Chuck_Event*)obj)->broadcast_local();
        }
    }
    return changed;
}


//-----------------------------------------------------------------------------
// ChucK class

static MaxClockState* lookup(Chuck_VM* vm)
{
    return g_lookup ? g_lookup(vm) : NULL;
}

// ChucK code sees the shred's own 'now', which keeps sub-sample precision;
// vm->now() is the whole-sample VM clock
static t_CKTIME shred_now(Chuck_VM* vm, Chuck_VM_Shred* shred)
{
    return shred ? shred->now : vm->now();
}

// dur to the next multiple of 'period' ticks, always > 0 so that
// 'clk.toNext(p) => now' in a loop cannot stall at a boundary. a stopped
// transport answers as if it started now
static t_CKDUR to_next(Chuck_VM* vm, t_CKTIME now, double period)
{
    MaxClockState* st = lookup(vm);
    if (st == NULL || !st->valid || st->cur.tps <= 0 || period <= 0) {
        return 1;  // no transport yet: poll each sample rather than spin
    }
    double t = maxclock_ticks_at(st->cur, now);
    double next = (std::floor(t / period) + 1) * period;
    double samples = (next - t) / st->cur.tps;
    if (samples < MAXCLOCK_JUMP_SAMPLES) {
        samples += period / st->cur.tps;
    }
    return samples;
}

static double beat_ticks(const MaxClockSnapshot& s)
{
    return s.resolution * 4.0 / (std::max)(s.denom, 1L);
}

CK_DLL_CTOR(maxclock_ctor)
{
    MaxClockState* st = lookup(VM);
    if (st) {
        st->clocks.push_back(SELF);
    }
}

CK_DLL_DTOR(maxclock_dtor)
{
    MaxClockState* st = lookup(VM);
    if (st) {
        st->clocks.erase(std::remove(st->clocks.begin(), st->clocks.end(), SELF),
                         st->clocks.end());
    }
}

CK_DLL_MFUN(maxclock_ticks)
{
    MaxClockState* st = lookup(VM);
    RETURN->v_float = st ? maxclock_ticks_at(st->cur, shred_now(VM, SHRED)) : 0;
}

CK_DLL_MFUN(maxclock_tempo)
{
    MaxClockState* st = lookup(VM);
    RETURN->v_float = st ? st->cur.tempo : 0;
}

CK_DLL_MFUN(maxclock_running)
{
    MaxClockState* st = lookup(VM);
    RETURN->v_int = st ? st->cur.running : 0;
}

CK_DLL_MFUN(maxclock_beat)
{
    MaxClockState* st = lookup(VM);
    RETURN->v_dur = (st && st->cur.tps > 0) ? beat_ticks(st->cur) / st->cur.tps : 0;
}

CK_DLL_MFUN(maxclock_to_next_beat)
{
    MaxClockState* st = lookup(VM);
    RETURN->v_dur = to_next(VM, shred_now(VM, SHRED), st ? beat_ticks(st->cur) : 0);
}

CK_DLL_MFUN(maxclock_to_next)
{
    RETURN->v_dur = to_next(VM, shred_now(VM, SHRED), GET_NEXT_FLOAT(ARGS));
}

// bar, beat (1-based) and units (ticks into the beat), as Max counts them
static void bbu(Chuck_VM* vm, t_CKTIME now, long* bar, long* beat, double* unit)
{
    *bar = 0, *beat = 0, *unit = 0;
    MaxClockState* st = lookup(vm);
    if (st == NULL || !st->valid) {
        return;
    }
    double t = maxclock_ticks_at(st->cur, now);
    double bt = beat_ticks(st->cur);
    double bar_ticks = bt * (std::max)(st->cur.num, 1L);
    // a position within half a sample of a boundary is on it: at 44.1 kHz
    // 'toNextBeat() => now' lands 1e-14 ticks short of the beat, and a plain
    // floor would answer the previous one
    double snap = MAXCLOCK_JUMP_SAMPLES * st->cur.tps;
    double in_bar = t - std::floor((t + snap) / bar_ticks) * bar_ticks;
    *bar = (long)std::floor((t + snap) / bar_ticks) + 1;
    *beat = (long)std::floor((in_bar + snap) / bt) + 1;
    *unit = in_bar - (*beat - 1) * bt;
}

CK_DLL_MFUN(maxclock_bar)
{
    long bar, beat; double unit;
    bbu(VM, shred_now(VM, SHRED), &bar, &beat, &unit);
    RETURN->v_int = bar;
}

CK_DLL_MFUN(maxclock_beat_in_bar)
{
    long bar, beat; double unit;
    bbu(VM, shred_now(VM, SHRED), &bar, &beat, &unit);
    RETURN->v_int = beat;
}

CK_DLL_MFUN(maxclock_unit)
{
    long bar, beat; double unit;
    bbu(VM, shred_now(VM, SHRED), &bar, &beat, &unit);
    RETURN->v_float = unit;
}

t_CKBOOL CK_DLL_CALL maxclock_query(Chuck_DL_Query* QUERY)
{
    QUERY->begin_class(QUERY, "HostClock", "Event");
    QUERY->doc_class(QUERY, "host transport; broadcasts on start/stop, "
                            "tempo or time signature change, and jumps");
    QUERY->add_ctor(QUERY, maxclock_ctor);
    QUERY->add_dtor(QUERY, maxclock_dtor);

    QUERY->add_mfun(QUERY, maxclock_ticks, "float", "ticks");
    QUERY->doc_func(QUERY, "transport position at now, in ticks");
    QUERY->add_mfun(QUERY, maxclock_tempo, "float", "tempo");
    QUERY->doc_func(QUERY, "tempo in BPM");
    QUERY->add_mfun(QUERY, maxclock_running, "int", "running");
    QUERY->doc_func(QUERY, "1 if the transport is running");
    QUERY->add_mfun(QUERY, maxclock_beat, "dur", "beat");
    QUERY->doc_func(QUERY, "duration of one beat at the current tempo");
    QUERY->add_mfun(QUERY, maxclock_to_next_beat, "dur", "toNextBeat");
    QUERY->doc_func(QUERY, "time to the next beat boundary; never zero");
    QUERY->add_mfun(QUERY, maxclock_to_next, "dur", "toNext");
    QUERY->add_arg(QUERY, "float", "ticks");
    QUERY->doc_func(QUERY, "time to the next multiple of 'ticks'; never zero");
    QUERY->add_mfun(QUERY, maxclock_bar, "int", "bar");
    QUERY->doc_func(QUERY, "bar at now, from 1");
    QUERY->add_mfun(QUERY, maxclock_beat_in_bar, "int", "beatInBar");
    QUERY->doc_func(QUERY, "beat within the bar at now, from 1");
    QUERY->add_mfun(QUERY, maxclock_unit, "float", "unit");
    QUERY->doc_func(QUERY, "ticks into the current beat");

    QUERY->end_class(QUERY);
    return TRUE;
}
