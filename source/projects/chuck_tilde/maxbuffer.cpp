#include "maxbuffer.h"

#include "chuck_oo.h"
#include "chuck_vm.h"

#include <cmath>

static f_maxbuffer_lookup g_lookup = NULL;

// member offsets, the same for every VM that binds the class
static t_CKUINT maxbuffer_offset_state = 0;
static t_CKUINT maxbuffer_offset_slot = 0;

void maxbuffer_set_lookup(f_maxbuffer_lookup lookup)
{
    g_lookup = lookup;
}

void maxbuffer_service(MaxBufferState* state)
{
    int n = state->count.load(std::memory_order_acquire);
    for (int i = 0; i < n; i++) {
        MaxBufferSlot& s = state->slots[i];
        void* handle = s.handle.load(std::memory_order_relaxed);
        if (handle == NULL) {
            handle = state->host.create(state->host.ctx, s.name.c_str());
            s.handle.store(handle, std::memory_order_release);
        }
        if (s.dirty.exchange(false, std::memory_order_acq_rel)) {
            state->host.set_dirty(state->host.ctx, handle);
        }
    }
}

// VM thread only, so the scan needs no lock; -1 when all slots are taken
static int slot_for(MaxBufferState* state, const std::string& name)
{
    int n = state->count.load(std::memory_order_relaxed);
    for (int i = 0; i < n; i++) {
        if (state->slots[i].name == name) {
            return i;
        }
    }
    if (n == MAXBUFFER_MAX_SLOTS) {
        return -1;
    }
    state->slots[n].name = name;
    state->count.store(n + 1, std::memory_order_release);
    state->host.request(state->host.ctx);
    return n;
}


//-----------------------------------------------------------------------------
// ChucK class

static MaxBufferState* state_of(Chuck_Object* self)
{
    return (MaxBufferState*)OBJ_MEMBER_INT(self, maxbuffer_offset_state);
}

// lock a slot's buffer; false until the main thread has made a reference
// and while no buffer~ has the name
static bool lock_slot(MaxBufferState* st, t_CKINT slot, MaxBufferView* view)
{
    if (st == NULL || slot < 0) {
        return false;
    }
    void* handle = st->slots[slot].handle.load(std::memory_order_acquire);
    return handle && st->host.lock(st->host.ctx, handle, view);
}

static bool lock(Chuck_Object* self, MaxBufferView* view)
{
    return lock_slot(state_of(self), OBJ_MEMBER_INT(self, maxbuffer_offset_slot), view);
}

static void unlock(Chuck_Object* self, MaxBufferView* view)
{
    MaxBufferState* st = state_of(self);
    st->host.unlock(st->host.ctx, view);
}

CK_DLL_CTOR(maxbuffer_ctor)
{
    // the host frees the state after the VM, so caching it here is safe
    OBJ_MEMBER_INT(SELF, maxbuffer_offset_state) = (t_CKINT)(g_lookup ? g_lookup(VM) : NULL);
    OBJ_MEMBER_INT(SELF, maxbuffer_offset_slot) = -1;
}

CK_DLL_MFUN(maxbuffer_set_name)
{
    MaxBufferState* st = state_of(SELF);
    Chuck_String* name = GET_NEXT_STRING(ARGS);
    t_CKINT slot = (st && name) ? slot_for(st, name->str()) : -1;
    OBJ_MEMBER_INT(SELF, maxbuffer_offset_slot) = slot;
    RETURN->v_string = (Chuck_String*)API->object->create_string(
        VM, slot >= 0 ? st->slots[slot].name.c_str() : "", FALSE);
}

CK_DLL_MFUN(maxbuffer_get_name)
{
    MaxBufferState* st = state_of(SELF);
    t_CKINT slot = OBJ_MEMBER_INT(SELF, maxbuffer_offset_slot);
    RETURN->v_string = (Chuck_String*)API->object->create_string(
        VM, (st && slot >= 0) ? st->slots[slot].name.c_str() : "", FALSE);
}

CK_DLL_MFUN(maxbuffer_ready)
{
    MaxBufferView v;
    RETURN->v_int = lock(SELF, &v);
    if (RETURN->v_int) {
        unlock(SELF, &v);
    }
}

CK_DLL_MFUN(maxbuffer_frames)
{
    MaxBufferView v;
    RETURN->v_int = 0;
    if (lock(SELF, &v)) {
        RETURN->v_int = v.frames;
        unlock(SELF, &v);
    }
}

CK_DLL_MFUN(maxbuffer_channels)
{
    MaxBufferView v;
    RETURN->v_int = 0;
    if (lock(SELF, &v)) {
        RETURN->v_int = v.chans;
        unlock(SELF, &v);
    }
}

CK_DLL_MFUN(maxbuffer_sample_rate)
{
    MaxBufferView v;
    RETURN->v_float = 0;
    if (lock(SELF, &v)) {
        RETURN->v_float = v.sr;
        unlock(SELF, &v);
    }
}

static bool in_range(const MaxBufferView& v, t_CKINT frame, t_CKINT chan)
{
    return frame >= 0 && frame < v.frames && chan >= 0 && chan < v.chans;
}

static t_CKFLOAT peek(Chuck_Object* self, t_CKINT frame, t_CKINT chan)
{
    MaxBufferView v;
    t_CKFLOAT out = 0;
    if (lock(self, &v)) {
        if (in_range(v, frame, chan)) {
            out = v.samples[frame * v.chans + chan];
        }
        unlock(self, &v);
    }
    return out;
}

static void poke(Chuck_Object* self, t_CKINT frame, t_CKINT chan, t_CKFLOAT value)
{
    MaxBufferView v;
    if (!lock(self, &v)) {
        return;
    }
    if (in_range(v, frame, chan)) {
        v.samples[frame * v.chans + chan] = (float)value;
        MaxBufferState* st = state_of(self);
        // buffer_setdirty is not documented as audio-thread safe; defer it
        if (!st->slots[OBJ_MEMBER_INT(self, maxbuffer_offset_slot)].dirty.exchange(true)) {
            st->host.request(st->host.ctx);
        }
    }
    unlock(self, &v);
}

CK_DLL_MFUN(maxbuffer_peek)
{
    t_CKINT frame = GET_NEXT_INT(ARGS);
    RETURN->v_float = peek(SELF, frame, 0);
}

CK_DLL_MFUN(maxbuffer_peek_chan)
{
    t_CKINT frame = GET_NEXT_INT(ARGS);
    t_CKINT chan = GET_NEXT_INT(ARGS);
    RETURN->v_float = peek(SELF, frame, chan);
}

CK_DLL_MFUN(maxbuffer_poke)
{
    t_CKINT frame = GET_NEXT_INT(ARGS);
    t_CKFLOAT value = GET_NEXT_FLOAT(ARGS);
    poke(SELF, frame, 0, value);
    RETURN->v_float = value;
}

CK_DLL_MFUN(maxbuffer_poke_chan)
{
    t_CKINT frame = GET_NEXT_INT(ARGS);
    t_CKINT chan = GET_NEXT_INT(ARGS);
    t_CKFLOAT value = GET_NEXT_FLOAT(ARGS);
    poke(SELF, frame, chan, value);
    RETURN->v_float = value;
}


//-----------------------------------------------------------------------------
// HostBufPlay: plays one channel of a buffer~ as a UGen

static t_CKUINT maxbufplay_offset_data = 0;

struct BufPlay {
    MaxBufferState* state;
    t_CKINT slot;
    double pos;      // in frames
    double rate;
    double vm_sr;
    t_CKINT loop;
    t_CKINT interp;  // 0 none, 1 linear
    t_CKINT chan;
};

static BufPlay* bufplay_of(Chuck_Object* self)
{
    return (BufPlay*)OBJ_MEMBER_INT(self, maxbufplay_offset_data);
}

CK_DLL_CTOR(maxbufplay_ctor)
{
    BufPlay* p = new BufPlay;
    p->state = g_lookup ? g_lookup(VM) : NULL;
    p->slot = -1;
    p->pos = 0;
    p->rate = 1;
    p->vm_sr = (double)API->vm->srate(VM);
    p->loop = 0;
    p->interp = 1;
    p->chan = 0;
    OBJ_MEMBER_INT(SELF, maxbufplay_offset_data) = (t_CKINT)p;
}

CK_DLL_DTOR(maxbufplay_dtor)
{
    delete bufplay_of(SELF);
    OBJ_MEMBER_INT(SELF, maxbufplay_offset_data) = 0;
}

static float frame_at(const MaxBufferView& v, long frame, t_CKINT chan)
{
    return v.samples[frame * v.chans + chan];
}

// one lock per sample, as in HostBuffer.peek, but without a shred step
CK_DLL_TICK(maxbufplay_tick)
{
    BufPlay* p = bufplay_of(SELF);
    MaxBufferView v;
    *out = 0;
    if (!lock_slot(p->state, p->slot, &v)) {
        return TRUE;
    }
    double n = (double)v.frames;
    if (n > 0 && p->chan >= 0 && p->chan < v.chans) {
        if (p->loop) {
            p->pos = std::fmod(p->pos, n);
            if (p->pos < 0) {
                p->pos += n;
            }
        }
        if (p->pos >= 0 && p->pos < n) {
            long i = (long)p->pos;
            double x = frame_at(v, i, p->chan);
            if (p->interp) {
                // past the last frame: wrap when looping, else hold
                long j = (i + 1 < v.frames) ? i + 1 : (p->loop ? 0 : i);
                x += (p->pos - i) * (frame_at(v, j, p->chan) - x);
            }
            *out = (SAMPLE)x;
            // buffer~ may hold a file at another rate, as SndBuf allows for
            p->pos += p->rate * (v.sr > 0 ? v.sr / p->vm_sr : 1);
        }
    }
    p->state->host.unlock(p->state->host.ctx, &v);
    return TRUE;
}

CK_DLL_MFUN(maxbufplay_set_name)
{
    BufPlay* p = bufplay_of(SELF);
    Chuck_String* name = GET_NEXT_STRING(ARGS);
    p->slot = (p->state && name) ? slot_for(p->state, name->str()) : -1;
    p->pos = 0;
    RETURN->v_string = (Chuck_String*)API->object->create_string(
        VM, p->slot >= 0 ? p->state->slots[p->slot].name.c_str() : "", FALSE);
}

CK_DLL_MFUN(maxbufplay_get_name)
{
    BufPlay* p = bufplay_of(SELF);
    RETURN->v_string = (Chuck_String*)API->object->create_string(
        VM, p->slot >= 0 ? p->state->slots[p->slot].name.c_str() : "", FALSE);
}

CK_DLL_MFUN(maxbufplay_ready)
{
    BufPlay* p = bufplay_of(SELF);
    MaxBufferView v;
    RETURN->v_int = lock_slot(p->state, p->slot, &v);
    if (RETURN->v_int) {
        p->state->host.unlock(p->state->host.ctx, &v);
    }
}

CK_DLL_MFUN(maxbufplay_frames)
{
    BufPlay* p = bufplay_of(SELF);
    MaxBufferView v;
    RETURN->v_int = 0;
    if (lock_slot(p->state, p->slot, &v)) {
        RETURN->v_int = v.frames;
        p->state->host.unlock(p->state->host.ctx, &v);
    }
}

CK_DLL_MFUN(maxbufplay_set_pos)
{
    bufplay_of(SELF)->pos = (double)GET_NEXT_INT(ARGS);
    RETURN->v_int = (t_CKINT)bufplay_of(SELF)->pos;
}

CK_DLL_MFUN(maxbufplay_get_pos)
{
    RETURN->v_int = (t_CKINT)std::floor(bufplay_of(SELF)->pos);
}

CK_DLL_MFUN(maxbufplay_set_rate)
{
    RETURN->v_float = bufplay_of(SELF)->rate = GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(maxbufplay_get_rate)
{
    RETURN->v_float = bufplay_of(SELF)->rate;
}

CK_DLL_MFUN(maxbufplay_set_loop)
{
    RETURN->v_int = bufplay_of(SELF)->loop = GET_NEXT_INT(ARGS) != 0;
}

CK_DLL_MFUN(maxbufplay_get_loop)
{
    RETURN->v_int = bufplay_of(SELF)->loop;
}

CK_DLL_MFUN(maxbufplay_set_interp)
{
    RETURN->v_int = bufplay_of(SELF)->interp = GET_NEXT_INT(ARGS) != 0;
}

CK_DLL_MFUN(maxbufplay_get_interp)
{
    RETURN->v_int = bufplay_of(SELF)->interp;
}

CK_DLL_MFUN(maxbufplay_set_channel)
{
    RETURN->v_int = bufplay_of(SELF)->chan = GET_NEXT_INT(ARGS);
}

CK_DLL_MFUN(maxbufplay_get_channel)
{
    RETURN->v_int = bufplay_of(SELF)->chan;
}

static void query_maxbufplay(Chuck_DL_Query* QUERY)
{
    QUERY->begin_class(QUERY, "HostBufPlay", "UGen");
    QUERY->doc_class(QUERY, "plays one channel of a named buffer~; "
                            "silent until the buffer~ is ready");
    QUERY->add_ctor(QUERY, maxbufplay_ctor);
    QUERY->add_dtor(QUERY, maxbufplay_dtor);
    QUERY->add_ugen_func(QUERY, maxbufplay_tick, NULL, 1, 1);
    maxbufplay_offset_data = QUERY->add_mvar(QUERY, "int", "@data", FALSE);

    QUERY->add_mfun(QUERY, maxbufplay_set_name, "string", "name");
    QUERY->add_arg(QUERY, "string", "name");
    QUERY->doc_func(QUERY, "play the buffer~ with this name, from frame 0");
    QUERY->add_mfun(QUERY, maxbufplay_get_name, "string", "name");
    QUERY->doc_func(QUERY, "the buffer~ name, or \"\"");
    QUERY->add_mfun(QUERY, maxbufplay_ready, "int", "ready");
    QUERY->doc_func(QUERY, "1 if the buffer~ exists and can be read");
    QUERY->add_mfun(QUERY, maxbufplay_frames, "int", "frames");
    QUERY->doc_func(QUERY, "length in sample frames; 0 if not ready");
    QUERY->add_mfun(QUERY, maxbufplay_set_pos, "int", "pos");
    QUERY->add_arg(QUERY, "int", "frame");
    QUERY->doc_func(QUERY, "set the play position in frames");
    QUERY->add_mfun(QUERY, maxbufplay_get_pos, "int", "pos");
    QUERY->doc_func(QUERY, "the play position in frames");
    QUERY->add_mfun(QUERY, maxbufplay_set_rate, "float", "rate");
    QUERY->add_arg(QUERY, "float", "rate");
    QUERY->doc_func(QUERY, "playback rate; 1 is normal, negative plays backward");
    QUERY->add_mfun(QUERY, maxbufplay_get_rate, "float", "rate");
    QUERY->doc_func(QUERY, "playback rate");
    QUERY->add_mfun(QUERY, maxbufplay_set_loop, "int", "loop");
    QUERY->add_arg(QUERY, "int", "loop");
    QUERY->doc_func(QUERY, "1 loops; 0 (default) stops at either end");
    QUERY->add_mfun(QUERY, maxbufplay_get_loop, "int", "loop");
    QUERY->doc_func(QUERY, "1 if looping");
    QUERY->add_mfun(QUERY, maxbufplay_set_interp, "int", "interp");
    QUERY->add_arg(QUERY, "int", "interp");
    QUERY->doc_func(QUERY, "1 (default) linear interpolation; 0 none");
    QUERY->add_mfun(QUERY, maxbufplay_get_interp, "int", "interp");
    QUERY->doc_func(QUERY, "1 if interpolating");
    QUERY->add_mfun(QUERY, maxbufplay_set_channel, "int", "channel");
    QUERY->add_arg(QUERY, "int", "chan");
    QUERY->doc_func(QUERY, "buffer~ channel to play, from 0");
    QUERY->add_mfun(QUERY, maxbufplay_get_channel, "int", "channel");
    QUERY->doc_func(QUERY, "buffer~ channel being played");
    QUERY->end_class(QUERY);
}

t_CKBOOL CK_DLL_CALL maxbuffer_query(Chuck_DL_Query* QUERY)
{
    QUERY->begin_class(QUERY, "HostBuffer", "Object");
    QUERY->doc_class(QUERY, "reads and writes the samples of a named buffer~");
    QUERY->add_ctor(QUERY, maxbuffer_ctor);
    maxbuffer_offset_state = QUERY->add_mvar(QUERY, "int", "@state", FALSE);
    maxbuffer_offset_slot = QUERY->add_mvar(QUERY, "int", "@slot", FALSE);

    QUERY->add_mfun(QUERY, maxbuffer_set_name, "string", "name");
    QUERY->add_arg(QUERY, "string", "name");
    QUERY->doc_func(QUERY, "refer to the buffer~ with this name; ready() turns "
                           "1 within a few ms if it exists. returns the name, "
                           "or \"\" if 64 names are already in use");
    QUERY->add_mfun(QUERY, maxbuffer_get_name, "string", "name");
    QUERY->doc_func(QUERY, "the buffer~ name, or \"\"");
    QUERY->add_mfun(QUERY, maxbuffer_ready, "int", "ready");
    QUERY->doc_func(QUERY, "1 if the buffer~ exists and can be read");
    QUERY->add_mfun(QUERY, maxbuffer_frames, "int", "frames");
    QUERY->doc_func(QUERY, "length in sample frames; 0 if not ready");
    QUERY->add_mfun(QUERY, maxbuffer_channels, "int", "channels");
    QUERY->doc_func(QUERY, "number of channels; 0 if not ready");
    QUERY->add_mfun(QUERY, maxbuffer_sample_rate, "float", "sampleRate");
    QUERY->doc_func(QUERY, "the buffer~ sample rate; 0 if not ready");

    QUERY->add_mfun(QUERY, maxbuffer_peek, "float", "peek");
    QUERY->add_arg(QUERY, "int", "frame");
    QUERY->doc_func(QUERY, "sample at frame, channel 0; 0 if out of range");
    QUERY->add_mfun(QUERY, maxbuffer_peek_chan, "float", "peek");
    QUERY->add_arg(QUERY, "int", "frame");
    QUERY->add_arg(QUERY, "int", "chan");
    QUERY->doc_func(QUERY, "sample at frame and channel (from 0); 0 if out of range");
    QUERY->add_mfun(QUERY, maxbuffer_poke, "float", "poke");
    QUERY->add_arg(QUERY, "int", "frame");
    QUERY->add_arg(QUERY, "float", "value");
    QUERY->doc_func(QUERY, "write value at frame, channel 0; ignored if out of range");
    QUERY->add_mfun(QUERY, maxbuffer_poke_chan, "float", "poke");
    QUERY->add_arg(QUERY, "int", "frame");
    QUERY->add_arg(QUERY, "int", "chan");
    QUERY->add_arg(QUERY, "float", "value");
    QUERY->doc_func(QUERY, "write value at frame and channel (from 0); ignored if out of range");
    QUERY->end_class(QUERY);

    query_maxbufplay(QUERY);
    return TRUE;
}
