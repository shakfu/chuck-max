// bind_test: engine-level tests for ChucK::bind(), no Max required.
// checks the assumptions behind the host classes in
// source/docs/dev/bind.md. exits non-zero if any check fails.

#include "chuck.h"
#include "chuck_dl.h"
#include "chuck_globals.h"
#include "chuck_vm.h"
#include "maxclock.h"
#include "maxin.h"
#include "maxbuffer.h"
#include "maxsend.h"

#include <cmath>
#include <cstdio>
#include <deque>
#include <filesystem>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <vector>

static int g_checks = 0;
static int g_failures = 0;

#define CHECK(cond)                                                     \
    do {                                                                \
        g_checks++;                                                     \
        if (!(cond)) {                                                  \
            g_failures++;                                               \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        }                                                               \
    } while (0)


//-----------------------------------------------------------------------------
// ChucK to host: Probe.send(string name, float value)

struct Record {
    std::string name;
    double value;
    Chuck_VM* vm;
    t_CKTIME now;
    std::string kind;   // which Host.msg overload ran; "" for Probe.send
};

static std::vector<Record> g_records;

CK_DLL_SFUN(probe_send)
{
    std::string name = GET_NEXT_STRING_SAFE(ARGS);
    t_CKFLOAT value = GET_NEXT_FLOAT(ARGS);
    g_records.push_back({name, value, VM, SHRED ? SHRED->now : VM->now()});
}


//-----------------------------------------------------------------------------
// host to ChucK: ProbeIn extends Event; pin.listen(name), pin.recv(), pin.value()

struct Inbox {
    Chuck_VM* vm;
    Chuck_VM_Shred* shred;  // the shred that called listen()
    std::string name;
    std::deque<double> queue;
    double current;
};

static std::map<Chuck_Object*, Inbox> g_inboxes;

static int g_dtor_calls = 0;

CK_DLL_DTOR(probein_dtor)
{
    g_dtor_calls++;
    g_inboxes.erase(SELF);
}

CK_DLL_MFUN(probein_listen)
{
    g_inboxes[SELF] = {VM, SHRED, GET_NEXT_STRING_SAFE(ARGS), {}, 0};
}

// a removed shred never releases its locals (Chuck_VM_Shred::shutdown frees
// the stacks without releasing objects), so the dtor above never runs for
// them; drop their entries when the shred leaves the VM
static void CK_DLL_CALL inbox_watcher(Chuck_VM_Shred* shred, t_CKINT code,
                                      t_CKINT param, Chuck_VM* vm, void* bindle)
{
    for (auto it = g_inboxes.begin(); it != g_inboxes.end();) {
        it = (it->second.shred == shred) ? g_inboxes.erase(it) : std::next(it);
    }
}

CK_DLL_MFUN(probein_recv)
{
    Inbox& in = g_inboxes[SELF];
    RETURN->v_int = !in.queue.empty();
    if (!in.queue.empty()) {
        in.current = in.queue.front();
        in.queue.pop_front();
    }
}

CK_DLL_MFUN(probein_value)
{
    RETURN->v_float = g_inboxes[SELF].current;
}

// the drain step proposed for ck_perform64: runs on the VM thread, before run()
static void deliver(Chuck_VM* vm, const std::string& name, double value)
{
    for (auto& kv : g_inboxes) {
        if (kv.second.vm == vm && kv.second.name == name) {
            kv.second.queue.push_back(value);
            ((Chuck_Event*)kv.first)->broadcast_local();
        }
    }
}


static t_CKBOOL CK_DLL_CALL probe_query(Chuck_DL_Query* QUERY)
{
    QUERY->begin_class(QUERY, "Probe", "Object");
    QUERY->add_sfun(QUERY, probe_send, "void", "send");
    QUERY->add_arg(QUERY, "string", "name");
    QUERY->add_arg(QUERY, "float", "value");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "ProbeIn", "Event");
    QUERY->add_dtor(QUERY, probein_dtor);
    QUERY->add_mfun(QUERY, probein_listen, "void", "listen");
    QUERY->add_arg(QUERY, "string", "name");
    QUERY->add_mfun(QUERY, probein_recv, "int", "recv");
    QUERY->add_mfun(QUERY, probein_value, "float", "value");
    QUERY->end_class(QUERY);
    return TRUE;
}


//-----------------------------------------------------------------------------
// helpers

static const t_CKINT FRAMES = 64;

static ChucK* make_chuck(bool bind)
{
    ChucK* ck = new ChucK();
    ck->setParam(CHUCK_PARAM_SAMPLE_RATE, (t_CKINT)44100);
    ck->setParam(CHUCK_PARAM_INPUT_CHANNELS, (t_CKINT)1);
    ck->setParam(CHUCK_PARAM_OUTPUT_CHANNELS, (t_CKINT)1);
    ck->setParam(CHUCK_PARAM_VM_HALT, (t_CKINT)0);
    ck->setParam(CHUCK_PARAM_CHUGIN_ENABLE, (t_CKINT)0);
    ck->setParam(CHUCK_PARAM_IS_REALTIME_AUDIO_HINT, (t_CKINT)0);
    ck->init();
    ck->start();
    if (bind) {
        ck->bind(probe_query, "Probe");
        ck->vm()->subscribe_watcher(inbox_watcher, ckvm_shreds_watch_REMOVE);
    }
    return ck;
}

static void run_block(ChucK* ck)
{
    SAMPLE in[FRAMES] = {0};
    SAMPLE out[FRAMES] = {0};
    ck->run(in, out, FRAMES);
}

static void send_vm_msg(ChucK* ck, Chuck_Msg_Type type)
{
    Chuck_Msg* msg = new Chuck_Msg;
    msg->type = type;
    msg->reply_cb = NULL;
    ck->vm()->globals_manager()->execute_chuck_msg_with_globals(msg);
}

// as ck_remove / ck_removeall send them
static void queue_vm_msg(ChucK* ck, Chuck_Msg_Type type, t_CKUINT param = 0)
{
    Chuck_Msg* msg = new Chuck_Msg;
    msg->type = type;
    msg->param = param;
    msg->reply_cb = NULL;
    ck->vm()->queue_msg(msg, 1);
}


//-----------------------------------------------------------------------------
// tests

static void test_send_and_vm()
{
    g_records.clear();
    ChucK* ck = make_chuck(true);
    CHECK(ck->compileCode("Probe.send(\"freq\", 440.0);"));
    run_block(ck);
    CHECK(g_records.size() == 1);
    if (g_records.size() == 1) {
        CHECK(g_records[0].name == "freq");
        CHECK(g_records[0].value == 440.0);
        // ck_instance_from_vm relies on this
        CHECK(g_records[0].vm == ck->vm());
    }
    delete ck;
}

static void test_timing()
{
    g_records.clear();
    ChucK* ck = make_chuck(true);
    CHECK(ck->compileCode(
        "Probe.send(\"t0\", 0); 100::samp => now; Probe.send(\"t1\", 1);"));
    run_block(ck);  // samples 0..63
    CHECK(g_records.size() == 1);
    run_block(ck);  // samples 64..127
    CHECK(g_records.size() == 2);
    if (g_records.size() == 2) {
        CHECK(g_records[0].now == 0);
        CHECK(g_records[1].now == 100);
    }
    delete ck;
}

static void test_two_instances()
{
    g_records.clear();
    ChucK* a = make_chuck(true);
    ChucK* b = make_chuck(true);
    CHECK(a->compileCode("Probe.send(\"a\", 1);"));
    CHECK(b->compileCode("Probe.send(\"b\", 2);"));
    run_block(a);
    run_block(b);
    CHECK(g_records.size() == 2);
    for (const Record& r : g_records) {
        CHECK(r.vm == (r.name == "a" ? a->vm() : b->vm()));
    }
    delete a;
    delete b;
}

static void test_survives_clearvm()
{
    g_records.clear();
    ChucK* ck = make_chuck(true);
    send_vm_msg(ck, CK_MSG_CLEARVM);
    run_block(ck);  // the VM processes queued messages inside run()
    CHECK(ck->compileCode("Probe.send(\"after\", 3);"));
    run_block(ck);
    CHECK(g_records.size() == 1);
    delete ck;
}

static void test_unbound_fails()
{
    ChucK* ck = make_chuck(false);
    CHECK(!ck->compileCode("Probe.send(\"x\", 0);"));
    delete ck;
}

static void test_host_to_chuck()
{
    g_records.clear();
    ChucK* ck = make_chuck(true);
    CHECK(ck->compileCode(
        "ProbeIn pin; pin.listen(\"note\");"
        "while (true) { pin => now;"
        "  while (pin.recv()) Probe.send(\"got\", pin.value()); }"));
    run_block(ck);  // shred starts and waits on pin
    CHECK(g_records.empty());
    deliver(ck->vm(), "note", 60);
    deliver(ck->vm(), "note", 62);
    deliver(ck->vm(), "other", 99);
    run_block(ck);  // shred should wake inside this run()
    CHECK(g_records.size() == 2);
    if (g_records.size() == 2) {
        CHECK(g_records[0].value == 60);
        CHECK(g_records[1].value == 62);
        CHECK(g_records[0].now == FRAMES);  // first sample of the block
    }
    delete ck;
}

static void test_inbox_unregisters()
{
    g_inboxes.clear();
    g_dtor_calls = 0;
    ChucK* ck = make_chuck(true);
    // the shred exits at once, so pin is released and its dtor runs
    CHECK(ck->compileCode("ProbeIn pin; pin.listen(\"x\");"));
    run_block(ck);
    run_block(ck);
    CHECK(g_dtor_calls == 1);
    CHECK(g_inboxes.empty());
    delete ck;
}

static void test_release_while_shred_lives()
{
    g_inboxes.clear();
    g_dtor_calls = 0;
    ChucK* ck = make_chuck(true);
    // pin is freed but its shred keeps running, so the watcher cannot help;
    // a stale entry here would make deliver() broadcast on freed memory
    CHECK(ck->compileCode(
        "ProbeIn pin; pin.listen(\"x\"); null @=> pin; 1::second => now;"));
    run_block(ck);
    CHECK(g_dtor_calls == 1);
    CHECK(g_inboxes.empty());
    deliver(ck->vm(), "x", 1);
    delete ck;
}

// a shred blocked on pin, ended by 'remove' (how). ChucK leaks a removed
// shred's locals, so pin's dtor never runs and only inbox_watcher clears the
// entry. if the dtor check starts failing, ChucK now releases them and the
// watcher may be redundant
enum RemoveHow { BY_CLEARVM, BY_ID, BY_REMOVEALL };

static void check_removed_shred_unregisters(RemoveHow how)
{
    g_inboxes.clear();
    g_dtor_calls = 0;
    ChucK* ck = make_chuck(true);
    std::vector<t_CKUINT> ids;
    CHECK(ck->compileCode("ProbeIn pin; pin.listen(\"x\"); pin => now;",
                          "", 1, FALSE, &ids));
    run_block(ck);
    CHECK(g_inboxes.size() == 1);
    CHECK(ids.size() == 1);
    switch (how) {
    case BY_CLEARVM:   send_vm_msg(ck, CK_MSG_CLEARVM); break;
    case BY_ID:        queue_vm_msg(ck, CK_MSG_REMOVE, ids.empty() ? 0 : ids[0]); break;
    case BY_REMOVEALL: queue_vm_msg(ck, CK_MSG_REMOVEALL); break;
    }
    run_block(ck);
    run_block(ck);
    CHECK(ck->vm()->shreduler()->lookup(ids.empty() ? 0 : ids[0]) == NULL);
    CHECK(g_dtor_calls == 0);
    CHECK(g_inboxes.empty());
    delete ck;
}

//-----------------------------------------------------------------------------
// HostClock and HostIn, as chuck_tilde wires them; a simulated transport
// stands in for Max's ITM

static std::map<Chuck_VM*, MaxClockState*> g_clock_states;
static std::map<Chuck_VM*, MaxInState*> g_in_states;

static MaxClockState* clock_lookup(Chuck_VM* vm)
{
    auto it = g_clock_states.find(vm);
    return it == g_clock_states.end() ? NULL : it->second;
}

static MaxInState* in_lookup(Chuck_VM* vm)
{
    auto it = g_in_states.find(vm);
    return it == g_in_states.end() ? NULL : it->second;
}

// fake buffer~ host: buffers exist by name, as Max's do, and a reference
// made before its buffer exists resolves once it does
struct FakeBuffer {
    std::vector<float> samples;  // interleaved
    long chans;
    double sr;
    long frames = 0;             // 0: all of samples; less leaves guard samples
};

struct FakeHost {
    std::map<std::string, FakeBuffer> buffers;
    std::vector<std::string*> refs;  // handles: the referenced name
    int requests = 0;
    int locks = 0;
    int unlocks = 0;
    std::map<std::string, int> dirtied;

    ~FakeHost()
    {
        for (std::string* r : refs) delete r;
    }
};

static MaxBufferHost fake_buffer_host(FakeHost* h)
{
    MaxBufferHost host;
    host.ctx = h;
    host.request = [](void* ctx) { ((FakeHost*)ctx)->requests++; };
    host.lock = [](void* ctx, void* handle, MaxBufferView* v) {
        FakeHost* h = (FakeHost*)ctx;
        auto it = h->buffers.find(*(std::string*)handle);
        if (it == h->buffers.end()) return false;
        h->locks++;
        v->obj = &it->second;
        v->samples = it->second.samples.data();
        v->chans = it->second.chans;
        v->frames = it->second.frames ? it->second.frames
                  : (long)it->second.samples.size() / it->second.chans;
        v->sr = it->second.sr;
        return true;
    };
    host.unlock = [](void* ctx, MaxBufferView*) { ((FakeHost*)ctx)->unlocks++; };
    host.create = [](void* ctx, const char* name) -> void* {
        std::string* r = new std::string(name);
        ((FakeHost*)ctx)->refs.push_back(r);
        return r;
    };
    host.set_dirty = [](void* ctx, void* handle) {
        ((FakeHost*)ctx)->dirtied[*(std::string*)handle]++;
    };
    return host;
}

static std::map<Chuck_VM*, MaxBufferState*> g_buffer_states;

static MaxBufferState* buffer_lookup(Chuck_VM* vm)
{
    auto it = g_buffer_states.find(vm);
    return it == g_buffer_states.end() ? NULL : it->second;
}

// 120 BPM at 480 ticks per quarter: 0.02 ticks per sample, 24000-sample beat
struct Rig {
    ChucK* ck;
    MaxClockState state;
    MaxInState in;
    FakeHost fake;
    MaxBufferState buffers;
    MaxClockSnapshot tr = {0, 0, 0.02, 120, 480, 4, 4, true};
    int broadcasts = 0;

    Rig() : ck(make_chuck(true))
    {
        ck->bind(maxclock_query, "HostClock");
        ck->bind(maxin_query, "HostIn");
        ck->bind(maxbuffer_query, "HostBuffer");
        buffers.host = fake_buffer_host(&fake);
        g_buffer_states[ck->vm()] = &buffers;
        ck->vm()->subscribe_watcher(maxin_watcher, ckvm_shreds_watch_REMOVE, &in);
        g_clock_states[ck->vm()] = &state;
        g_in_states[ck->vm()] = &in;
    }
    ~Rig()
    {
        Chuck_VM* vm = ck->vm();
        delete ck;  // HostClock and HostIn dtors still find their state here
        g_clock_states.erase(vm);
        g_in_states.erase(vm);
        g_buffer_states.erase(vm);
    }
    // what ck_perform64 does each block: drain, snapshot, update, run, then
    // advance
    void block()
    {
        maxin_drain(&in);
        tr.now = ck->now();
        broadcasts += maxclock_update(&state, tr);
        run_block(ck);
        if (tr.running) {
            tr.ticks += FRAMES * tr.tps;
        }
    }
    void blocks(int n)
    {
        for (int i = 0; i < n; i++) block();
    }
};

static bool near(double a, double b, double tol = 1e-6)
{
    return std::fabs(a - b) <= tol;
}

static const Record* find_record(const std::string& name)
{
    for (const Record& r : g_records) {
        if (r.name == name) return &r;
    }
    return NULL;
}

static void test_clock_extrapolates()
{
    g_records.clear();
    Rig rig;
    rig.tr.ticks = 1000;
    CHECK(rig.ck->compileCode(
        "HostClock clk; Probe.send(\"t0\", clk.ticks());"
        "100::samp => now; Probe.send(\"t1\", clk.ticks());"));
    rig.blocks(3);
    const Record* t0 = find_record("t0");
    const Record* t1 = find_record("t1");
    CHECK(t0 && near(t0->value, 1000));
    // block 1 starts at sample 64; sample 100 is 36 samples into it
    CHECK(t1 && t1->now == 100 && near(t1->value, 1000 + 100 * 0.02));
}

static void test_clock_to_next_beat()
{
    g_records.clear();
    Rig rig;
    rig.tr.ticks = 470;  // 10 ticks = 500 samples before beat 2
    CHECK(rig.ck->compileCode(
        "HostClock clk; clk.toNextBeat() => now; Probe.send(\"on\", clk.ticks());"
        "Probe.send(\"next\", clk.toNextBeat() / samp);"));
    rig.blocks(10);
    const Record* on = find_record("on");
    const Record* next = find_record("next");
    CHECK(on && on->now == 500 && near(on->value, 480, 0.5 * 0.02));
    // on a boundary, the next boundary is a full beat away, not zero
    CHECK(next && near(next->value, 24000));

    // 500.3 samples away: ChucK keeps sub-sample time, so the shred wakes at
    // exactly 500.3 and reads the boundary
    g_records.clear();
    Rig frac;
    frac.tr.ticks = 480 - 500.3 * 0.02;
    CHECK(frac.ck->compileCode(
        "HostClock clk; clk.toNextBeat() => now; Probe.send(\"on\", clk.ticks());"));
    frac.blocks(10);
    on = find_record("on");
    CHECK(on && near(on->now, 500.3) && near(on->value, 480));
}

static void test_clock_stopped()
{
    g_records.clear();
    Rig rig;
    rig.tr.running = false;
    rig.tr.ticks = 100;
    CHECK(rig.ck->compileCode(
        "HostClock clk; Probe.send(\"t0\", clk.ticks()); Probe.send(\"run\", clk.running());"
        "1000::samp => now; Probe.send(\"t1\", clk.ticks());"
        "Probe.send(\"next\", clk.toNextBeat() / samp);"));
    rig.blocks(20);
    const Record* t1 = find_record("t1");
    const Record* next = find_record("next");
    CHECK(find_record("run") && find_record("run")->value == 0);
    CHECK(t1 && near(t1->value, 100));
    // as if started now: 380 ticks to beat 2
    CHECK(next && near(next->value, 380 / 0.02));
}

// a shred that reports each broadcast with the tempo and running state
static const char* WATCHER =
    "HostClock clk;"
    "while (true) { clk => now; Probe.send(\"chg\", clk.tempo() * 10 + clk.running()); }";

static void test_clock_broadcasts()
{
    g_records.clear();
    Rig rig;
    CHECK(rig.ck->compileCode(WATCHER));
    rig.blocks(3);
    CHECK(g_records.empty());

    // drift under half a sample is noise, not a jump
    rig.tr.ticks += 0.2 * rig.tr.tps;
    rig.blocks(2);
    CHECK(g_records.empty());

    rig.tr.tps = 0.015, rig.tr.tempo = 90;  // tempo change
    t_CKTIME at = rig.ck->now();
    rig.block();
    CHECK(g_records.size() == 1);
    if (g_records.size() == 1) {
        CHECK(g_records[0].value == 901);
        CHECK(g_records[0].now == at);  // first sample of that block
    }

    rig.tr.ticks += 1000;  // seek
    rig.block();
    CHECK(g_records.size() == 2);

    rig.tr.running = false;  // stop
    rig.block();
    CHECK(g_records.size() == 3 && g_records.back().value == 900);

    rig.tr.denom = 8;  // time signature
    rig.block();
    CHECK(g_records.size() == 4);
    CHECK(rig.broadcasts == 4);
}

static void test_clock_bbu()
{
    g_records.clear();
    Rig rig;
    rig.tr.running = false;
    const char* code =
        "HostClock clk; Probe.send(\"bar\", clk.bar());"
        "Probe.send(\"beat\", clk.beatInBar()); Probe.send(\"unit\", clk.unit());";
    rig.tr.ticks = 2 * 1920 + 480 + 7;  // 4/4: bar 3, beat 2, 7 ticks in
    CHECK(rig.ck->compileCode(code));
    rig.blocks(1);
    CHECK(find_record("bar") && find_record("bar")->value == 3);
    CHECK(find_record("beat") && find_record("beat")->value == 2);
    CHECK(find_record("unit") && near(find_record("unit")->value, 7));

    g_records.clear();
    rig.tr.num = 6, rig.tr.denom = 8;  // 240-tick beats, 1440-tick bars
    rig.tr.ticks = 1440 + 2 * 240 + 5;  // bar 2, beat 3, 5 ticks in
    CHECK(rig.ck->compileCode(code));
    rig.blocks(1);
    CHECK(find_record("bar") && find_record("bar")->value == 2);
    CHECK(find_record("beat") && find_record("beat")->value == 3);
    CHECK(find_record("unit") && near(find_record("unit")->value, 5));

    // within half a sample of a boundary is on it. at 120 BPM and 44.1 kHz,
    // 'toNextBeat() => now' lands 1e-14 ticks short of the beat
    g_records.clear();
    rig.tr.num = 4, rig.tr.denom = 4;
    rig.tr.tps = 960.0 / 44100;
    rig.tr.ticks = std::nextafter(1920.0, 0.0);  // just short of bar 2
    CHECK(rig.ck->compileCode(code));
    rig.blocks(1);
    CHECK(find_record("bar") && find_record("bar")->value == 2);
    CHECK(find_record("beat") && find_record("beat")->value == 1);
    CHECK(find_record("unit") && near(find_record("unit")->value, 0));

    g_records.clear();
    rig.tr.ticks = 1920 - rig.tr.tps;  // a whole sample short: still bar 1
    CHECK(rig.ck->compileCode(code));
    rig.blocks(1);
    CHECK(find_record("bar") && find_record("bar")->value == 1);
    CHECK(find_record("beat") && find_record("beat")->value == 4);

    // the same from a running transport: wake on the beat, read beat 2
    g_records.clear();
    Rig live;
    live.tr.tps = 960.0 / 44100;
    CHECK(live.ck->compileCode(
        "HostClock clk; clk.toNextBeat() => now; Probe.send(\"on\", clk.ticks());"
        "Probe.send(\"beat\", clk.beatInBar());"));
    live.blocks(22050 / FRAMES + 2);
    CHECK(find_record("on") && near(find_record("on")->value, 480, 0.5 * live.tr.tps));
    CHECK(find_record("beat") && find_record("beat")->value == 2);
}

static void test_clock_instances_isolated()
{
    g_records.clear();
    Rig a, b;
    CHECK(a.ck->compileCode(WATCHER));
    CHECK(b.ck->compileCode(WATCHER));
    a.blocks(1);
    b.blocks(1);
    b.tr.ticks += 1000;  // jump only b
    a.blocks(1);
    b.blocks(1);
    CHECK(g_records.size() == 1);
    if (g_records.size() == 1) {
        CHECK(g_records[0].vm == b.ck->vm());
    }
}

static void test_clock_lifecycle()
{
    g_records.clear();
    Rig rig;
    CHECK(rig.ck->compileCode("HostClock clk;"));  // exits: clk is released
    rig.blocks(2);
    CHECK(rig.state.clocks.empty());

    // a removed shred leaks clk (no dtor), but remove_blocked() takes the
    // shred off clk's wait queue, so a later broadcast must wake nothing
    CHECK(rig.ck->compileCode(WATCHER));
    rig.blocks(1);
    CHECK(rig.state.clocks.size() == 1);
    queue_vm_msg(rig.ck, CK_MSG_REMOVEALL);
    rig.blocks(1);
    CHECK(rig.state.clocks.size() == 1);
    rig.tr.ticks += 1000;
    rig.blocks(2);
    CHECK(g_records.empty());
}

//-----------------------------------------------------------------------------
// HostIn / HostMsg, fed through maxin_push as ck_msg does

static MaxInAtom I(t_CKINT v) { MaxInAtom a; a.type = MAXIN_INT; a.i = v; return a; }
static MaxInAtom F(t_CKFLOAT v) { MaxInAtom a; a.type = MAXIN_FLOAT; a.f = v; return a; }
static MaxInAtom S(const char* v) { MaxInAtom a; a.type = MAXIN_STRING; a.s = v; return a; }

static bool push(Rig& rig, const char* name, std::vector<MaxInAtom> args = {})
{
    return maxin_push(&rig.in, name, (long)args.size(), args.data());
}

// reports each 'note' message: name and count, int 0, float 1, string 2
static const char* RECEIVER =
    "HostIn min; min.addAddress(\"note\"); HostMsg m;"
    "while (true) { min => now; while (min.recv(m)) {"
    "  Probe.send(m.name(), m.numArgs()); Probe.send(\"i\", m.getInt(0));"
    "  Probe.send(\"f\", m.getFloat(1)); Probe.send(\"s:\" + m.getString(2), 0); } }";

static void test_maxin_delivers()
{
    g_records.clear();
    Rig rig;
    CHECK(rig.ck->compileCode(RECEIVER));
    rig.blocks(1);
    CHECK(push(rig, "note", {I(60), F(0.8), S("legato")}));
    CHECK(push(rig, "other", {I(99)}));
    CHECK(push(rig, "note", {I(62)}));
    t_CKTIME at = rig.ck->now();
    rig.block();
    CHECK(g_records.size() == 8);
    if (g_records.size() == 8) {
        CHECK(g_records[0].name == "note" && g_records[0].value == 3);
        CHECK(g_records[0].now == at);  // first sample of the block
        CHECK(g_records[1].value == 60);
        CHECK(near(g_records[2].value, 0.8));
        CHECK(g_records[3].name == "s:legato");
        // second message, in order; missing arguments read as 0 and ""
        CHECK(g_records[4].name == "note" && g_records[4].value == 1);
        CHECK(g_records[5].value == 62);
        CHECK(g_records[6].value == 0);
        CHECK(g_records[7].name == "s:");
    }
}

static void test_maxin_coerces()
{
    g_records.clear();
    Rig rig;
    CHECK(rig.ck->compileCode(
        "HostIn min; min.addAddress(\"x\"); HostMsg m; min => now; min.recv(m);"
        "Probe.send(\"i0\", m.getInt(0)); Probe.send(\"f1\", m.getFloat(1));"
        "Probe.send(\"i2\", m.getInt(2)); Probe.send(\"f2\", m.getFloat(2));"
        "Probe.send(\"s:\" + m.getString(0), 0); Probe.send(\"neg\", m.getInt(-1));"));
    rig.blocks(1);
    CHECK(push(rig, "x", {F(7.9), I(5), S("a")}));
    rig.blocks(1);
    CHECK(find_record("i0") && find_record("i0")->value == 7);
    CHECK(find_record("f1") && find_record("f1")->value == 5);
    CHECK(find_record("i2") && find_record("i2")->value == 0);
    CHECK(find_record("f2") && find_record("f2")->value == 0);
    CHECK(find_record("s:") != NULL);
    CHECK(find_record("neg") && find_record("neg")->value == 0);
}

static void test_maxin_addresses()
{
    g_records.clear();
    Rig rig;
    CHECK(rig.ck->compileCode(
        "fun void rd(HostIn x, string tag) { HostMsg m;"
        "  while (true) { x => now; while (x.recv(m)) Probe.send(tag + m.name(), 0); } }"
        "HostIn p; p.addAddress(\"a\"); p.addAddress(\"b\");"
        "HostIn q; q.addAddress(\"a\");"
        "spork ~ rd(p, \"p\"); spork ~ rd(q, \"q\"); 1::day => now;"));
    rig.blocks(1);
    CHECK(push(rig, "a"));
    CHECK(push(rig, "b"));
    rig.blocks(1);
    CHECK(g_records.size() == 3);
    CHECK(find_record("pa") && find_record("pb") && find_record("qa"));
    CHECK(find_record("qb") == NULL);
}

static void test_maxin_no_listener()
{
    g_records.clear();
    Rig rig;
    CHECK(push(rig, "note", {I(1)}));
    rig.blocks(1);  // drained with no HostIn: dropped
    CHECK(rig.ck->compileCode(RECEIVER));
    rig.blocks(2);
    CHECK(g_records.empty());
    CHECK(push(rig, "note", {I(2)}));
    rig.blocks(1);
    CHECK(find_record("i") && find_record("i")->value == 2);
}

static void test_maxin_limits()
{
    Rig rig;
    // the ring holds MAXIN_QUEUE_SIZE - 1 until the VM drains it
    int pushed = 0;
    while (push(rig, "x") && pushed <= MAXIN_QUEUE_SIZE) pushed++;
    CHECK(pushed == MAXIN_QUEUE_SIZE - 1);
    rig.blocks(1);
    CHECK(push(rig, "x"));

    // a HostIn that never calls recv() keeps at most MAXIN_MAX_PENDING
    CHECK(rig.ck->compileCode("HostIn min; min.addAddress(\"x\"); 1::day => now;"));
    rig.blocks(1);
    for (int i = 0; i < MAXIN_MAX_PENDING + 10; i++) push(rig, "x");
    rig.blocks(1);
    CHECK(rig.in.entries.size() == 1);
    if (rig.in.entries.size() == 1) {
        CHECK(rig.in.entries[0]->pending.size() == MAXIN_MAX_PENDING);
    }

    // arguments past MAXIN_MAX_ATOMS are cut
    g_records.clear();
    CHECK(rig.ck->compileCode(RECEIVER));
    rig.blocks(1);
    std::vector<MaxInAtom> many(MAXIN_MAX_ATOMS + 8, I(1));
    CHECK(push(rig, "note", many));
    rig.blocks(1);
    CHECK(find_record("note") && find_record("note")->value == MAXIN_MAX_ATOMS);
}

static void test_maxin_unregisters()
{
    Rig rig;
    // the shred exits at once, so min is released and its dtor unregisters
    CHECK(rig.ck->compileCode("HostIn min; min.addAddress(\"x\");"));
    rig.blocks(2);
    CHECK(rig.in.entries.empty());

    // freed while its shred runs: the watcher cannot help, the dtor must
    CHECK(rig.ck->compileCode(
        "HostIn min; min.addAddress(\"x\"); null @=> min; 1::second => now;"));
    rig.blocks(1);
    CHECK(rig.in.entries.empty());
    push(rig, "x");
    rig.blocks(1);  // must not touch the freed object
}

// a receiver ended by 'remove' (how) leaks min, so only maxin_watcher can
// drop its entry; a fresh receiver must then get exactly one copy
static void check_maxin_removed(RemoveHow how)
{
    g_records.clear();
    Rig rig;
    std::vector<t_CKUINT> ids;
    CHECK(rig.ck->compileCode(RECEIVER, "", 1, FALSE, &ids));
    rig.blocks(1);
    CHECK(rig.in.entries.size() == 1);
    switch (how) {
    case BY_CLEARVM:   send_vm_msg(rig.ck, CK_MSG_CLEARVM); break;
    case BY_ID:        queue_vm_msg(rig.ck, CK_MSG_REMOVE, ids.empty() ? 0 : ids[0]); break;
    case BY_REMOVEALL: queue_vm_msg(rig.ck, CK_MSG_REMOVEALL); break;
    }
    rig.blocks(2);
    CHECK(rig.in.entries.empty());

    // HostIn is in the global namespace, so it compiles after CLEARVM too
    CHECK(rig.ck->compileCode(RECEIVER));
    rig.blocks(1);
    CHECK(rig.in.entries.size() == 1);
    push(rig, "note", {I(7)});
    rig.blocks(1);
    CHECK(g_records.size() == 4 && find_record("i") && find_record("i")->value == 7);
}

// documents a limitation: the watcher drops a HostIn when its creating shred
// ends, even if a global keeps it alive for another shred
static void test_maxin_global_outlives_creator()
{
    g_records.clear();
    Rig rig;
    CHECK(rig.ck->compileCode("global HostIn gin; gin.addAddress(\"x\");"));
    rig.blocks(2);  // creator exits; gin lives on as a global
    CHECK(rig.ck->compileCode(
        "global HostIn gin; HostMsg m; gin => now; Probe.send(\"woke\", gin.recv(m));"));
    rig.blocks(1);
    push(rig, "x");
    rig.blocks(2);
    CHECK(rig.in.entries.empty());
    CHECK(g_records.empty());
}

static void test_maxin_instances_isolated()
{
    g_records.clear();
    Rig a, b;
    CHECK(a.ck->compileCode(RECEIVER));
    CHECK(b.ck->compileCode(RECEIVER));
    a.blocks(1);
    b.blocks(1);
    push(a, "note", {I(1)});
    a.blocks(1);
    b.blocks(1);
    CHECK(g_records.size() == 4);
    for (const Record& r : g_records) {
        CHECK(r.vm == a.ck->vm());
    }
}

//-----------------------------------------------------------------------------
// HostBuffer, against FakeHost; service() stands in for the main-thread qelem

static void test_buffer_resolves()
{
    g_records.clear();
    Rig rig;
    rig.fake.buffers["drums"] = {std::vector<float>(2 * 100, 0), 2, 48000};
    CHECK(rig.ck->compileCode(
        "HostBuffer b; b.name(\"drums\") => string n; Probe.send(\"n:\" + n, b.ready());"
        "while (!b.ready()) 1::samp => now;"
        "Probe.send(\"frames\", b.frames()); Probe.send(\"chans\", b.channels());"
        "Probe.send(\"sr\", b.sampleRate()); Probe.send(\"get:\" + b.name(), 0);"));
    rig.blocks(2);
    // no reference until the main thread makes one
    CHECK(find_record("n:drums") && find_record("n:drums")->value == 0);
    CHECK(find_record("frames") == NULL);
    CHECK(rig.fake.requests == 1);
    maxbuffer_service(&rig.buffers);
    rig.blocks(1);
    CHECK(find_record("frames") && find_record("frames")->value == 100);
    CHECK(find_record("chans") && find_record("chans")->value == 2);
    CHECK(find_record("sr") && find_record("sr")->value == 48000);
    CHECK(find_record("get:drums") != NULL);
    CHECK(rig.fake.locks == rig.fake.unlocks);
}

static void test_buffer_peek_poke()
{
    g_records.clear();
    Rig rig;
    // 3 stereo frames, then one guard frame that must never be read or written
    FakeBuffer& fb = rig.fake.buffers["st"] =
        {{0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 9.f, 9.f}, 2, 44100, 3};
    CHECK(rig.ck->compileCode("HostBuffer b; b.name(\"st\");"));
    rig.blocks(1);
    maxbuffer_service(&rig.buffers);
    CHECK(rig.fake.dirtied.empty());
    CHECK(rig.ck->compileCode(
        "HostBuffer b; b.name(\"st\");"
        "Probe.send(\"p0\", b.peek(1)); Probe.send(\"p1\", b.peek(2, 1));"
        "Probe.send(\"oob\", b.peek(3) + b.peek(-1) + b.peek(0, 2) + b.peek(0, -1));"
        "b.poke(0, 1, 0.9); b.poke(2, -0.5); b.poke(3, 0, 1); b.poke(0, 2, 1);"));
    rig.blocks(1);
    // interleaved: frame 1 channel 0 is index 2, frame 2 channel 1 is index 5
    CHECK(find_record("p0") && near(find_record("p0")->value, 0.3));
    CHECK(find_record("p1") && near(find_record("p1")->value, 0.6));
    CHECK(find_record("oob") && find_record("oob")->value == 0);
    CHECK(near(fb.samples[1], 0.9) && near(fb.samples[4], -0.5));
    CHECK(near(fb.samples[0], 0.1) && near(fb.samples[5], 0.6));  // untouched
    CHECK(fb.samples[6] == 9.f && fb.samples[7] == 9.f);
    // Max hears of the writes once, on the main thread
    int before = rig.fake.requests;
    maxbuffer_service(&rig.buffers);
    CHECK(rig.fake.dirtied["st"] == 1);
    maxbuffer_service(&rig.buffers);
    CHECK(rig.fake.dirtied["st"] == 1);
    // one for the new name, one for the first write; later writes add none
    CHECK(before == 2);
    CHECK(rig.fake.locks == rig.fake.unlocks);
}

static void test_buffer_appears_and_goes()
{
    g_records.clear();
    Rig rig;
    CHECK(rig.ck->compileCode(
        "HostBuffer b; b.name(\"late\");"
        "while (true) { Probe.send(\"r\", b.ready()); Probe.send(\"v\", b.peek(0)); 64::samp => now; }"));
    rig.blocks(1);
    maxbuffer_service(&rig.buffers);
    rig.blocks(1);
    CHECK(g_records.size() == 4 && g_records[2].value == 0 && g_records[3].value == 0);
    rig.fake.buffers["late"] = {{0.7f}, 1, 44100};  // buffer~ created later
    rig.blocks(1);
    CHECK(g_records.size() == 6 && g_records[4].value == 1 && near(g_records[5].value, 0.7));
    rig.fake.buffers.erase("late");  // buffer~ deleted
    rig.blocks(1);
    CHECK(g_records.size() == 8 && g_records[6].value == 0 && g_records[7].value == 0);
}

static void test_buffer_slots()
{
    g_records.clear();
    Rig rig;
    rig.fake.buffers["a"] = {{0.25f}, 1, 44100};
    CHECK(rig.ck->compileCode("HostBuffer b; b.name(\"a\");"));
    rig.blocks(1);
    maxbuffer_service(&rig.buffers);
    // a name already referenced is ready at once, even after a reset
    send_vm_msg(rig.ck, CK_MSG_CLEARVM);
    rig.blocks(1);
    CHECK(rig.ck->compileCode(
        "HostBuffer c; c.name(\"a\"); Probe.send(\"again\", c.ready());"));
    rig.blocks(1);
    CHECK(find_record("again") && find_record("again")->value == 1);
    CHECK(rig.buffers.count == 1 && rig.fake.refs.size() == 1);

    // names beyond MAXBUFFER_MAX_SLOTS are refused
    CHECK(rig.ck->compileCode(
        "HostBuffer m; for (1 => int i; i < 70; i++) m.name(\"x\" + i) => string s;"
        "HostBuffer z; z.name(\"over\") => string s; Probe.send(\"over:\" + s, z.ready());"
        "Probe.send(\"zname:\" + z.name(), 0);"));
    rig.blocks(1);
    CHECK(rig.buffers.count == MAXBUFFER_MAX_SLOTS);
    CHECK(find_record("over:") && find_record("over:")->value == 0);
    CHECK(find_record("zname:") != NULL);
}

static void test_buffer_unbound_state()
{
    // no state for this VM (host tearing down): every call is a safe no-op
    g_records.clear();
    Rig rig;
    g_buffer_states.erase(rig.ck->vm());
    CHECK(rig.ck->compileCode(
        "HostBuffer b; b.name(\"a\") => string s; b.poke(0, 1);"
        "Probe.send(\"r\", b.ready() + b.frames() + b.peek(0));"));
    rig.blocks(1);
    CHECK(find_record("r") && find_record("r")->value == 0);
    CHECK(rig.fake.requests == 0);
}

static void test_buffer_instances_isolated()
{
    Rig a, b;
    CHECK(a.ck->compileCode("HostBuffer m; m.name(\"s\");"));
    a.blocks(1);
    b.blocks(1);
    CHECK(a.buffers.count == 1 && b.buffers.count == 0);
    CHECK(a.fake.requests == 1 && b.fake.requests == 0);
}

//-----------------------------------------------------------------------------
// HostBufPlay UGen, against FakeHost

// outputs of the shred's n samples after 'setup', read with last(): the
// value the UGen computed on the previous sample
static std::vector<double> play(Rig& rig, const std::string& setup, int n)
{
    g_records.clear();
    CHECK(rig.ck->compileCode(
        "HostBufPlay p => blackhole; p.name(\"a\");"
        "while (!p.ready()) 1::samp => now;" + setup +
        "repeat (" + std::to_string(n) + ") { 1::samp => now; Probe.send(\"o\", p.last()); }"));
    rig.blocks(1);
    maxbuffer_service(&rig.buffers);
    rig.blocks(2);
    std::vector<double> out;
    for (const Record& r : g_records) out.push_back(r.value);
    return out;
}

static bool seq_near(const std::vector<double>& got, const std::vector<double>& want)
{
    if (got.size() != want.size()) return false;
    for (size_t i = 0; i < got.size(); i++) {
        if (!near(got[i], want[i], 1e-6)) return false;
    }
    return true;
}

static const std::vector<float> RAMP = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};

static void test_bufplay_plays()
{
    Rig rig;
    rig.fake.buffers["a"] = {RAMP, 1, 44100};
    // one-shot: stops after the last frame
    CHECK(seq_near(play(rig, "0 => p.pos;", 7), {.1, .2, .3, .4, .5, 0, 0}));
    // name() restarts from frame 0, as SndBuf.read does
    CHECK(seq_near(play(rig, "3 => p.pos; p.name(\"a\");", 2), {.1, .2}));
    CHECK(rig.fake.locks == rig.fake.unlocks);
}

static void test_bufplay_loop_rate_interp()
{
    Rig rig;
    rig.fake.buffers["a"] = {RAMP, 1, 44100};
    CHECK(seq_near(play(rig, "1 => p.loop;", 7), {.1, .2, .3, .4, .5, .1, .2}));
    CHECK(seq_near(play(rig, "0.5 => p.rate;", 4), {.1, .15, .2, .25}));
    CHECK(seq_near(play(rig, "0.5 => p.rate; 0 => p.interp;", 4), {.1, .1, .2, .2}));
    // backward from the last frame, stopping before frame 0
    CHECK(seq_near(play(rig, "-1 => p.rate; 4 => p.pos;", 7), {.5, .4, .3, .2, .1, 0, 0}));
    // interpolation across the loop point uses frame 0
    CHECK(seq_near(play(rig, "1 => p.loop; 0.5 => p.rate; 4 => p.pos;", 3), {.5, .3, .1}));
    // without looping, the last frame holds rather than reading past the end
    CHECK(seq_near(play(rig, "0.5 => p.rate; 4 => p.pos;", 3), {.5, .5, 0}));
}

static void test_bufplay_channel_and_sr()
{
    Rig rig;
    rig.fake.buffers["a"] = {{0.1f, -0.1f, 0.2f, -0.2f, 0.3f, -0.3f}, 2, 44100};
    CHECK(seq_near(play(rig, "1 => p.channel;", 3), {-.1, -.2, -.3}));
    CHECK(seq_near(play(rig, "2 => p.channel;", 2), {0, 0}));

    // a 22050 Hz buffer~ plays at half speed on a 44100 Hz VM
    Rig slow;
    slow.fake.buffers["a"] = {RAMP, 1, 22050};
    CHECK(seq_near(play(slow, "", 4), {.1, .15, .2, .25}));
}

static void test_bufplay_missing_buffer()
{
    g_records.clear();
    Rig rig;
    CHECK(rig.ck->compileCode(
        "HostBufPlay p => blackhole; p.name(\"a\");"
        "while (true) { 1::samp => now; Probe.send(\"o\", p.last()); }"));
    rig.blocks(1);
    maxbuffer_service(&rig.buffers);
    rig.blocks(1);
    bool silent = true;
    for (const Record& r : g_records) silent = silent && r.value == 0;
    // the shred starts at sample 0 and first records at sample 1
    CHECK(silent && g_records.size() == 2 * FRAMES - 1);
    // created later: plays from frame 0, since pos held while missing
    rig.fake.buffers["a"] = {RAMP, 1, 44100};
    g_records.clear();
    rig.blocks(1);
    CHECK(g_records.size() >= 3 && near(g_records[1].value, .1) && near(g_records[2].value, .2));
    // deleted mid-play: silence
    rig.fake.buffers.erase("a");
    g_records.clear();
    rig.blocks(1);
    CHECK(g_records.size() >= 2 && g_records[1].value == 0);
    CHECK(rig.fake.locks == rig.fake.unlocks);
}

static void test_bufplay_shares_slots()
{
    Rig rig;
    rig.fake.buffers["a"] = {RAMP, 1, 44100};
    CHECK(rig.ck->compileCode(
        "HostBuffer b; b.name(\"a\"); HostBufPlay p => blackhole; p.name(\"a\");"
        "Probe.send(\"n:\" + p.name(), 0);"));
    rig.blocks(1);
    CHECK(rig.buffers.count == 1 && rig.fake.requests == 1);
    CHECK(find_record("n:a") != NULL);
}

// stand-in for chuck_tilde's Host class, same signatures, recording to
// g_records with the overload that ran in Record::kind
CK_DLL_SFUN(stub_msg_bang)
{
    std::string name = GET_NEXT_STRING_SAFE(ARGS);
    g_records.push_back({name, 0, VM, SHRED ? SHRED->now : VM->now(), "bang"});
}

CK_DLL_SFUN(stub_msg_float)
{
    std::string name = GET_NEXT_STRING_SAFE(ARGS);
    t_CKFLOAT value = GET_NEXT_FLOAT(ARGS);
    g_records.push_back({name, value, VM, SHRED ? SHRED->now : VM->now(), "float"});
}

CK_DLL_SFUN(stub_msg_string)
{
    std::string name = GET_NEXT_STRING_SAFE(ARGS);
    std::string value = GET_NEXT_STRING_SAFE(ARGS);
    g_records.push_back({name, (double)value.size(), VM,
                         SHRED ? SHRED->now : VM->now(), "string"});
}

CK_DLL_SFUN(stub_msg_list)
{
    std::string name = GET_NEXT_STRING_SAFE(ARGS);
    Chuck_ArrayFloat* values = (Chuck_ArrayFloat*)GET_NEXT_OBJECT(ARGS);
    g_records.push_back({name, (double)(values ? values->size() : -1), VM,
                         SHRED ? SHRED->now : VM->now(), "list"});
}

static t_CKBOOL CK_DLL_CALL max_stub_query(Chuck_DL_Query* QUERY)
{
    QUERY->begin_class(QUERY, "Host", "Object");
    QUERY->add_sfun(QUERY, stub_msg_bang, "void", "msg");
    QUERY->add_arg(QUERY, "string", "name");
    QUERY->add_sfun(QUERY, stub_msg_float, "void", "msg");
    QUERY->add_arg(QUERY, "string", "name");
    QUERY->add_arg(QUERY, "float", "value");
    QUERY->add_sfun(QUERY, stub_msg_string, "void", "msg");
    QUERY->add_arg(QUERY, "string", "name");
    QUERY->add_arg(QUERY, "string", "value");
    QUERY->add_sfun(QUERY, stub_msg_list, "void", "msg");
    QUERY->add_arg(QUERY, "string", "name");
    QUERY->add_arg(QUERY, "float[]", "values");
    maxsend_add_methods(QUERY);
    QUERY->end_class(QUERY);
    return TRUE;
}


//-----------------------------------------------------------------------------
// Host.send*: ChucK to [receive], through maxsend.cpp with a fake host

struct Delivery {
    MaxSendType type;
    std::string receiver;
    std::string symbol;
    std::vector<double> values;
};

struct FakeSendHost {
    std::set<std::string> names;  // interned: set nodes never move
    std::vector<Delivery> got;
    int requests = 0;
};

static void* fake_intern(void* ctx, const char* name)
{
    return (void*)&*((FakeSendHost*)ctx)->names.insert(name).first;
}

static void fake_send_request(void* ctx)
{
    ((FakeSendHost*)ctx)->requests++;
}

static void fake_deliver(void* ctx, const MaxSendMessage* m)
{
    ((FakeSendHost*)ctx)->got.push_back({m->type, *(const std::string*)m->receiver,
        m->symbol ? *(const std::string*)m->symbol : std::string(),
        std::vector<double>(m->argv, m->argv + m->argc)});
}

static std::map<Chuck_VM*, MaxSendState*> g_send_states;

static MaxSendState* send_lookup(Chuck_VM* vm)
{
    auto it = g_send_states.find(vm);
    return it == g_send_states.end() ? NULL : it->second;
}

struct SendRig {
    ChucK* ck;
    FakeSendHost fake;
    MaxSendState state;
    long dropped = -1;
    long truncated = -1;

    // with_state false: the host has no state for this VM (tearing down)
    explicit SendRig(bool with_state = true) : ck(make_chuck(false))
    {
        state.host = {&fake, fake_intern, fake_send_request, fake_deliver};
        if (with_state) {
            g_send_states[ck->vm()] = &state;
        }
        ck->bind(max_stub_query, "Host");
    }
    ~SendRig()
    {
        Chuck_VM* vm = ck->vm();
        delete ck;
        g_send_states.erase(vm);
    }
    void drain() { maxsend_drain(&state, &dropped, &truncated); }
};

static void test_send_types()
{
    SendRig rig;
    CHECK(rig.ck->compileCode(
        "Host.sendBang(\"b\");"
        "Host.sendFloat(\"f\", 2.5);"
        "Host.sendSymbol(\"s\", \"kick.wav\");"
        "Host.sendList(\"l\", [1.0, -2.0, 3.5]);"
        "float e[0]; Host.sendList(\"e\", e);"));
    run_block(rig.ck);
    // queued on the VM thread; nothing reaches a receiver before the drain
    CHECK(rig.fake.got.empty() && rig.fake.requests == 5);
    rig.drain();
    const std::vector<Delivery>& g = rig.fake.got;
    CHECK(g.size() == 5);
    if (g.size() == 5) {
        CHECK(g[0].type == MAXSEND_BANG && g[0].receiver == "b" && g[0].values.empty());
        CHECK(g[1].type == MAXSEND_FLOAT && g[1].receiver == "f"
              && g[1].values == std::vector<double>{2.5});
        CHECK(g[2].type == MAXSEND_SYMBOL && g[2].receiver == "s" && g[2].symbol == "kick.wav");
        CHECK(g[3].type == MAXSEND_LIST && g[3].receiver == "l"
              && g[3].values == (std::vector<double>{1.0, -2.0, 3.5}));
        // an empty list arrives as bang, as in Pd
        CHECK(g[4].type == MAXSEND_BANG && g[4].receiver == "e");
    }
    CHECK(rig.dropped == 0 && rig.truncated == 0);
}

static void test_send_list_truncated()
{
    SendRig rig;
    CHECK(rig.ck->compileCode(
        "float v[40]; for (0 => int i; i < 40; i++) i => v[i];"
        "Host.sendList(\"l\", v);"));
    run_block(rig.ck);
    rig.drain();
    CHECK(rig.fake.got.size() == 1);
    if (rig.fake.got.size() == 1) {
        const std::vector<double>& v = rig.fake.got[0].values;
        CHECK(v.size() == MAXSEND_MAX_ATOMS && v.front() == 0 && v.back() == 31);
    }
    CHECK(rig.truncated == 1);
    rig.drain();
    CHECK(rig.truncated == 0);  // reset by the drain that reported it
}

static void test_send_queue_full()
{
    SendRig rig;
    CHECK(rig.ck->compileCode(
        "for (0 => int i; i < 300; i++) Host.sendFloat(\"n\", i);"
        "1::samp => now;"
        "for (0 => int i; i < 100; i++) Host.sendFloat(\"m\", i);"));
    run_block(rig.ck);
    // one block: 256 queued, 44 refused, the 100 'm' refused too
    rig.drain();
    CHECK(rig.fake.got.size() == MAXSEND_QUEUE_SIZE && rig.dropped == 144);
    bool in_order = true;
    for (size_t i = 0; i < rig.fake.got.size(); i++) {
        in_order = in_order && rig.fake.got[i].values == std::vector<double>{(double)i};
    }
    CHECK(in_order);
    // the cursors keep growing past the queue size
    rig.fake.got.clear();
    CHECK(rig.ck->compileCode(
        "for (0 => int i; i < 200; i++) Host.sendFloat(\"w\", i);"));
    run_block(rig.ck);
    rig.drain();
    CHECK(rig.fake.got.size() == 200 && rig.dropped == 0);
    CHECK(!rig.fake.got.empty() && rig.fake.got.back().values == std::vector<double>{199});
}

static void test_send_no_state()
{
    SendRig rig(false);
    CHECK(rig.ck->compileCode(
        "Host.sendBang(\"b\"); Host.sendList(\"l\", [1.0]);"));
    run_block(rig.ck);
    rig.drain();
    CHECK(rig.fake.got.empty() && rig.fake.requests == 0);
}

static void test_send_instances_isolated()
{
    SendRig a, b;
    CHECK(a.ck->compileCode("Host.sendFloat(\"x\", 1);"));
    CHECK(b.ck->compileCode("Host.sendFloat(\"x\", 2);"));
    run_block(a.ck);
    run_block(b.ck);
    a.drain();
    b.drain();
    CHECK(a.fake.got.size() == 1 && a.fake.got[0].values == std::vector<double>{1});
    CHECK(b.fake.got.size() == 1 && b.fake.got[0].values == std::vector<double>{2});
}

// Host.msg resolves by argument type: bang, float (an int literal promotes),
// string, float[]; chuck_tilde.cpp registers four sfuns under one name
static void test_host_msg_overloads()
{
    SendRig rig;
    g_records.clear();
    CHECK(rig.ck->compileCode(
        "Host.msg(\"a\"); Host.msg(\"b\", 1); Host.msg(\"c\", 2.5);"
        "Host.msg(\"d\", \"sym\"); Host.msg(\"e\", [1.0, 2.0, 3.0]);"
        "float empty[0]; Host.msg(\"f\", empty);"));
    run_block(rig.ck);
    CHECK(g_records.size() == 6);
    if (g_records.size() == 6) {
        CHECK(g_records[0].kind == "bang" && g_records[0].name == "a");
        CHECK(g_records[1].kind == "float" && g_records[1].value == 1);
        CHECK(g_records[2].kind == "float" && g_records[2].value == 2.5);
        CHECK(g_records[3].kind == "string" && g_records[3].value == 3);
        CHECK(g_records[4].kind == "list" && g_records[4].value == 3);
        CHECK(g_records[5].kind == "list" && g_records[5].value == 0);
    }
    // a string[] has no overload
    CHECK(!rig.ck->compileCode("string s[1]; Host.msg(\"g\", s);"));
}

// the chuck CLI in 'make test' cannot compile these: it binds no host classes
static void test_max_examples_compile()
{
    int n = 0;
    for (const auto& f : std::filesystem::directory_iterator(BIND_EXAMPLES_DIR)) {
        std::string name = f.path().filename().string();
        if (name.rfind("max_", 0) != 0 || f.path().extension() != ".ck") {
            continue;
        }
        Rig rig;
        rig.ck->bind(max_stub_query, "Host");
        bool ok = rig.ck->compileFile(f.path().string());
        if (!ok) {
            fprintf(stderr, "FAIL compile %s\n", name.c_str());
        }
        CHECK(ok);
        n++;
    }
    CHECK(n >= 5);
}


int main()
{
    ChucK::setLogLevel(CK_LOG_NONE);
    // silence VM chatter such as "(VM) removing all shreds"
    ChucK::setStdoutCallback([](const char*) {});
    ChucK::setStderrCallback([](const char*) {});

    test_send_and_vm();
    test_timing();
    test_two_instances();
    test_survives_clearvm();
    test_unbound_fails();
    test_host_to_chuck();
    test_inbox_unregisters();
    test_release_while_shred_lives();
    check_removed_shred_unregisters(BY_CLEARVM);
    check_removed_shred_unregisters(BY_ID);
    check_removed_shred_unregisters(BY_REMOVEALL);

    maxclock_set_lookup(clock_lookup);
    test_clock_extrapolates();
    test_clock_to_next_beat();
    test_clock_stopped();
    test_clock_broadcasts();
    test_clock_bbu();
    test_clock_instances_isolated();
    test_clock_lifecycle();

    maxin_set_lookup(in_lookup);
    test_maxin_delivers();
    test_maxin_coerces();
    test_maxin_addresses();
    test_maxin_no_listener();
    test_maxin_limits();
    test_maxin_unregisters();
    check_maxin_removed(BY_CLEARVM);
    check_maxin_removed(BY_ID);
    check_maxin_removed(BY_REMOVEALL);
    test_maxin_global_outlives_creator();
    test_maxin_instances_isolated();

    maxbuffer_set_lookup(buffer_lookup);
    test_buffer_resolves();
    test_buffer_peek_poke();
    test_buffer_appears_and_goes();
    test_buffer_slots();
    test_buffer_unbound_state();
    test_buffer_instances_isolated();
    test_bufplay_plays();
    test_bufplay_loop_rate_interp();
    test_bufplay_channel_and_sr();
    test_bufplay_missing_buffer();
    test_bufplay_shares_slots();

    maxsend_set_lookup(send_lookup);
    test_send_types();
    test_send_list_truncated();
    test_send_queue_full();
    test_send_no_state();
    test_send_instances_isolated();
    test_host_msg_overloads();

    test_max_examples_compile();

    ChucK::globalCleanup();
    printf("bind_test: %d checks, %d failed\n", g_checks, g_failures);
    return g_failures ? 1 : 0;
}
