// MaxBuffer: ChucK reads and writes buffer~ samples (see source/docs/dev/bind.md).
// No Max dependency: the host supplies the buffer calls through MaxBufferHost,
// so source/projects/tests/bind_test.cpp can drive it with fake buffers.

#pragma once

#include "chuck_dl.h"

#include <atomic>
#include <string>

// distinct buffer~ names per instance; slots are never freed before the host
#define MAXBUFFER_MAX_SLOTS 64

// one locked buffer; samples are interleaved by channel
struct MaxBufferView {
    void* obj;          // host's buffer object, passed back to unlock
    float* samples;
    long frames;
    long chans;
    double sr;
};

// host calls; ctx is the host's per-instance pointer
struct MaxBufferHost {
    void* ctx;
    // VM thread: have the main thread call maxbuffer_service soon
    void (*request)(void* ctx);
    // VM thread: lock the buffer behind handle; false if it does not exist
    bool (*lock)(void* ctx, void* handle, MaxBufferView* view);
    void (*unlock)(void* ctx, MaxBufferView* view);
    // main thread: a reference to the buffer named 'name', which may not
    // exist yet; never NULL
    void* (*create)(void* ctx, const char* name);
    // main thread: tell Max the buffer changed
    void (*set_dirty)(void* ctx, void* handle);
};

// name is written by the VM thread before 'count' publishes the slot; handle
// is written once by the main thread
struct MaxBufferSlot {
    std::string name;
    std::atomic<void*> handle{nullptr};
    std::atomic<bool> dirty{false};
};

// per-instance state; the host owns one per ChucK instance, frees it after
// the VM, and frees every non-NULL slot handle
struct MaxBufferState {
    MaxBufferHost host;
    MaxBufferSlot slots[MAXBUFFER_MAX_SLOTS];
    std::atomic<int> count{0};
};

// maps a VM to its state; NULL once the host is tearing down
typedef MaxBufferState* (*f_maxbuffer_lookup)(Chuck_VM* vm);

// set once, before any VM binds MaxBuffer
void maxbuffer_set_lookup(f_maxbuffer_lookup lookup);

// ChucK::bind() query that declares MaxBuffer and the MaxBufPlay UGen
t_CKBOOL CK_DLL_CALL maxbuffer_query(Chuck_DL_Query* QUERY);

// main thread: create references for new names; pass dirty flags to Max
void maxbuffer_service(MaxBufferState* state);
