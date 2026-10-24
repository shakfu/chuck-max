// Host.sendBang / sendFloat / sendSymbol / sendList: ChucK to every [receive]
// with a name (see source/docs/dev/bind.md). No Max dependency: the host
// interns names and delivers on its own thread, so
// source/projects/tests/bind_test.cpp can drive it without Max.

#pragma once

#include "chuck_dl.h"

#include <atomic>

// calls queued between drains, per instance; further calls are dropped
#define MAXSEND_QUEUE_SIZE 256
// elements per sendList; longer lists are cut
#define MAXSEND_MAX_ATOMS 32

enum MaxSendType { MAXSEND_BANG, MAXSEND_FLOAT, MAXSEND_SYMBOL, MAXSEND_LIST };

struct MaxSendMessage {
    MaxSendType type;
    void* receiver;     // interned by the host
    void* symbol;       // interned by the host; MAXSEND_SYMBOL only
    long argc;          // 1 for MAXSEND_FLOAT, 1..MAXSEND_MAX_ATOMS for MAXSEND_LIST
    double argv[MAXSEND_MAX_ATOMS];
};

// host calls; ctx is the host's per-instance pointer
struct MaxSendHost {
    void* ctx;
    // VM thread: a handle for name that stays valid for the process, as a
    // Max symbol does
    void* (*intern)(void* ctx, const char* name);
    // VM thread: have the host call maxsend_drain soon
    void (*request)(void* ctx);
    // host thread, from maxsend_drain
    void (*deliver)(void* ctx, const MaxSendMessage* m);
};

// single producer (VM thread), single consumer (host thread); the cursors
// only grow, so the queue holds MAXSEND_QUEUE_SIZE messages
struct MaxSendState {
    MaxSendHost host;
    MaxSendMessage queue[MAXSEND_QUEUE_SIZE];
    std::atomic<long> head{0};          // read cursor, host thread
    std::atomic<long> tail{0};          // write cursor, VM thread
    std::atomic<long> dropped{0};       // calls refused on a full queue
    std::atomic<long> truncated{0};     // lists cut to MAXSEND_MAX_ATOMS
};

// maps a VM to its state; NULL once the host is tearing down
typedef MaxSendState* (*f_maxsend_lookup)(Chuck_VM* vm);

// set once, before any VM binds a class with these methods
void maxsend_set_lookup(f_maxsend_lookup lookup);

// declare sendBang, sendFloat, sendSymbol and sendList in the open class
void maxsend_add_methods(Chuck_DL_Query* QUERY);

// host thread: deliver queued messages in order, then report and reset the
// dropped and truncated counts
void maxsend_drain(MaxSendState* state, long* dropped, long* truncated);
