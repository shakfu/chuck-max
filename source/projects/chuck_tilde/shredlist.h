// Shred helpers for chuck~: the `shreds dump` snapshot, named code shreds
// (`addcode`, `replacecode`) and replace detection for the shred watcher.
// No Max dependency, so source/projects/tests/bind_test.cpp can drive it.
//
// The shreduler may only be read on the thread that runs compute(). The host
// thread requests a snapshot, the VM thread fills it between compute() calls,
// and the host thread reads and releases it. Each phase has one owner, so the
// entries need no lock.

#pragma once

#include "chuck_def.h"

#include <atomic>
#include <string>
#include <vector>

class ChucK;
class Chuck_VM;
class Chuck_VM_Shred;

// entries per snapshot; further shreds are counted in `total` only
#define SHREDLIST_MAX 256
// bytes per name, including the terminator
#define SHREDLIST_NAME 256

enum { SHREDLIST_IDLE, SHREDLIST_REQUESTED, SHREDLIST_FILLING, SHREDLIST_READY };

struct ShredListEntry {
    t_CKUINT id;
    double elapsed;             // seconds of VM time since the shred started
    char name[SHREDLIST_NAME];  // truncated on a UTF-8 boundary
};

struct ShredListState {
    std::atomic<int> phase{SHREDLIST_IDLE};
    long count = 0;  // entries filled
    long total = 0;  // shreds in the VM, may exceed SHREDLIST_MAX
    ShredListEntry entries[SHREDLIST_MAX];
    std::vector<Chuck_VM_Shred*> scratch;  // reserved, so filling does not allocate

    ShredListState() { scratch.reserve(SHREDLIST_MAX); }
};

// host thread: ask for a snapshot; a pending request absorbs this one. false
// while a snapshot is being filled or waits to be read
bool shredlist_request(ShredListState* st);

// VM thread, outside compute(): fill a requested snapshot. true if this call
// filled it, so the host should be told to read it. Two concurrent callers
// cannot both fill: the first to claim the request wins
bool shredlist_service(ShredListState* st, Chuck_VM* vm);

// host thread: true if a snapshot is ready to read
bool shredlist_ready(ShredListState* st);

// host thread: done reading; the next request may proceed
void shredlist_release(ShredListState* st);

// copy `src` into `dst` (size `n`), cutting at a UTF-8 code point boundary
void shredlist_copy_name(char* dst, size_t n, const std::string& src);

// host thread: compile `code` and spork one shred named `name`, shreduled in
// the next compute(). Returns its id, or 0 if compilation failed
t_CKUINT shredlist_add_code(ChucK* ck, const std::string& code, const std::string& name);

// host thread: compile `code` and queue a replace of shred `id`, applied in
// the next compute(). The new shred keeps `id` and takes `name`. false if
// compilation failed
bool shredlist_replace_code(ChucK* ck, t_CKUINT id, const std::string& code,
                            const std::string& name);

// VM thread, from a REMOVE watcher: true if `shred` left because a replace
// gave its id to a new shred. A replace notifies SPORK for the new shred
// before REMOVE for the old one, so reporting this remove would drop a live id
bool shredlist_replaced(Chuck_VM* vm, Chuck_VM_Shred* shred);
