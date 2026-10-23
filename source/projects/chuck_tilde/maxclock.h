// MaxClock: ChucK view of a host transport (see source/docs/dev/bind.md).
// No Max dependency: the host fills a MaxClockSnapshot each block, so
// source/projects/tests/bind_test.cpp can drive it with a simulated transport.

#pragma once

#include "chuck_dl.h"

#include <vector>

// transport state at the start of one audio block
struct MaxClockSnapshot {
    t_CKTIME now;       // ChucK time at block start, in samples
    double ticks;       // transport position
    double tps;         // ticks per sample while running (> 0)
    double tempo;       // BPM
    double resolution;  // ticks per quarter note
    long num;           // time signature numerator
    long denom;         // time signature denominator
    bool running;
};

// per-VM state; the host owns one per ChucK instance
struct MaxClockState {
    MaxClockSnapshot cur = {};
    bool valid = false;
    std::vector<Chuck_Object*> clocks;  // live MaxClock objects, VM thread only
};

// maps a VM to its state; NULL once the host is tearing down
typedef MaxClockState* (*f_maxclock_lookup)(Chuck_VM* vm);

// set once, before any VM binds MaxClock
void maxclock_set_lookup(f_maxclock_lookup lookup);

// ChucK::bind() query that declares the MaxClock class
t_CKBOOL CK_DLL_CALL maxclock_query(Chuck_DL_Query* QUERY);

// VM thread, before ChucK::run(): store the snapshot and broadcast every
// MaxClock on start/stop, tempo or time signature change, or a jump of more
// than half a sample. returns true if it broadcast
bool maxclock_update(MaxClockState* state, const MaxClockSnapshot& snap);

// transport position at ChucK time 'now', extrapolated from the snapshot
double maxclock_ticks_at(const MaxClockSnapshot& snap, t_CKTIME now);
