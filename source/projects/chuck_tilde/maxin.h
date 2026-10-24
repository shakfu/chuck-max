// HostIn / HostMsg: Max messages into ChucK (see source/docs/dev/bind.md).
// No Max dependency: the host pushes messages from its message threads and
// drains them on the VM thread, so source/projects/tests/bind_test.cpp can
// drive it without Max.

#pragma once

#include "chuck_dl.h"

#include <atomic>
#include <deque>
#include <mutex>
#include <string>
#include <vector>

// max atoms carried by one message after its name
#define MAXIN_MAX_ATOMS 32
// depth of the host -> VM inbox, per instance
#define MAXIN_QUEUE_SIZE 128
// messages a HostIn holds before recv(); further messages are dropped
#define MAXIN_MAX_PENDING 64

enum MaxInType { MAXIN_INT, MAXIN_FLOAT, MAXIN_STRING };

// strings must stay valid for the life of the process, as Max symbols do
struct MaxInAtom {
    MaxInType type;
    union {
        t_CKINT i;
        t_CKFLOAT f;
        const char* s;
    };
};

struct MaxInMessage {
    const char* name;
    long argc;
    MaxInAtom argv[MAXIN_MAX_ATOMS];
};

struct MaxInState;

// one per HostIn object; owned by the object, listed in its state while live
struct MaxInEntry {
    MaxInState* state;
    Chuck_Object* obj;
    Chuck_VM_Shred* shred;              // creating shred, for the watcher
    bool registered;
    std::vector<std::string> names;     // from addAddress()
    std::deque<MaxInMessage> pending;
};

// per-VM state; the host owns one per ChucK instance and frees it after the VM
struct MaxInState {
    std::vector<MaxInEntry*> entries;   // VM thread only
    std::vector<MaxInMessage> inbox = std::vector<MaxInMessage>(MAXIN_QUEUE_SIZE);
    std::atomic<long> head{0};          // read cursor, VM thread
    std::atomic<long> tail{0};          // write cursor, host threads
    std::mutex push_lock;               // Max can send from main and scheduler threads
};

// maps a VM to its state; NULL once the host is tearing down
typedef MaxInState* (*f_maxin_lookup)(Chuck_VM* vm);

// set once, before any VM binds HostIn
void maxin_set_lookup(f_maxin_lookup lookup);

// ChucK::bind() query that declares HostIn and HostMsg
t_CKBOOL CK_DLL_CALL maxin_query(Chuck_DL_Query* QUERY);

// shreds watcher (ckvm_shreds_watch_REMOVE, userdata = the MaxInState).
// a removed shred leaks its locals, so their dtors never unregister them
void CK_DLL_CALL maxin_watcher(Chuck_VM_Shred* shred, t_CKINT code,
                               t_CKINT param, Chuck_VM* vm, void* userdata);

// host thread: queue a message; argc above MAXIN_MAX_ATOMS is truncated.
// returns false if the inbox is full
bool maxin_push(MaxInState* state, const char* name, long argc,
                const MaxInAtom* argv);

// VM thread, before ChucK::run(): hand queued messages to matching HostIn
// objects and broadcast them
void maxin_drain(MaxInState* state);
