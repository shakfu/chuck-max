#include "maxin.h"

#include "chuck_oo.h"
#include "chuck_vm.h"

#include <algorithm>

static f_maxin_lookup g_lookup = NULL;

// member offsets, the same for every VM that binds the classes
static t_CKUINT maxin_offset_entry = 0;
static t_CKUINT maxmsg_offset_data = 0;

void maxin_set_lookup(f_maxin_lookup lookup)
{
    g_lookup = lookup;
}

bool maxin_push(MaxInState* state, const char* name, long argc,
                const MaxInAtom* argv)
{
    std::lock_guard<std::mutex> lock(state->push_lock);
    long tail = state->tail.load(std::memory_order_relaxed);
    long next = (tail + 1) % MAXIN_QUEUE_SIZE;
    if (next == state->head.load(std::memory_order_acquire)) {
        return false;
    }
    MaxInMessage& m = state->inbox[tail];
    m.name = name;
    m.argc = std::max(0L, std::min(argc, (long)MAXIN_MAX_ATOMS));
    std::copy(argv, argv + m.argc, m.argv);
    state->tail.store(next, std::memory_order_release);
    return true;
}

static void unregister(MaxInEntry* e)
{
    if (!e->registered) {
        return;
    }
    std::vector<MaxInEntry*>& v = e->state->entries;
    v.erase(std::remove(v.begin(), v.end(), e), v.end());
    e->registered = false;
    e->pending.clear();
}

static bool listens(const MaxInEntry* e, const char* name)
{
    for (const std::string& n : e->names) {
        if (n == name) {
            return true;
        }
    }
    return false;
}

void maxin_drain(MaxInState* state)
{
    long head = state->head.load(std::memory_order_relaxed);
    while (head != state->tail.load(std::memory_order_acquire)) {
        const MaxInMessage& m = state->inbox[head];
        // broadcast_local only moves waiting shreds to the shreduler; it runs
        // no ChucK code, so entries cannot change underneath this loop
        for (MaxInEntry* e : state->entries) {
            if (listens(e, m.name) && e->pending.size() < MAXIN_MAX_PENDING) {
                e->pending.push_back(m);
                ((Chuck_Event*)e->obj)->broadcast_local();
            }
        }
        head = (head + 1) % MAXIN_QUEUE_SIZE;
        state->head.store(head, std::memory_order_release);
    }
}

void CK_DLL_CALL maxin_watcher(Chuck_VM_Shred* shred, t_CKINT code,
                               t_CKINT param, Chuck_VM* vm, void* userdata)
{
    MaxInState* state = (MaxInState*)userdata;
    if (state == NULL) {
        return;
    }
    for (size_t i = 0; i < state->entries.size();) {
        MaxInEntry* e = state->entries[i];
        if (e->shred == shred) {
            unregister(e);
        } else {
            i++;
        }
    }
}


//-----------------------------------------------------------------------------
// ChucK classes

static MaxInEntry* entry_of(Chuck_Object* self)
{
    return (MaxInEntry*)OBJ_MEMBER_INT(self, maxin_offset_entry);
}

static MaxInMessage* message_of(Chuck_Object* self)
{
    return (MaxInMessage*)OBJ_MEMBER_INT(self, maxmsg_offset_data);
}

CK_DLL_CTOR(maxin_ctor)
{
    MaxInEntry* e = new MaxInEntry;
    e->state = g_lookup ? g_lookup(VM) : NULL;
    e->obj = SELF;
    e->shred = SHRED;
    e->registered = e->state != NULL;
    if (e->registered) {
        e->state->entries.push_back(e);
    }
    OBJ_MEMBER_INT(SELF, maxin_offset_entry) = (t_CKINT)e;
}

// the host frees the state after the VM, so e->state is valid here
CK_DLL_DTOR(maxin_dtor)
{
    MaxInEntry* e = entry_of(SELF);
    if (e) {
        unregister(e);
        delete e;
        OBJ_MEMBER_INT(SELF, maxin_offset_entry) = 0;
    }
}

CK_DLL_MFUN(maxin_add_address)
{
    MaxInEntry* e = entry_of(SELF);
    Chuck_String* name = GET_NEXT_STRING(ARGS);
    if (e && name) {
        e->names.push_back(name->str());
    }
}

CK_DLL_MFUN(maxin_recv)
{
    MaxInEntry* e = entry_of(SELF);
    Chuck_Object* msg = GET_NEXT_OBJECT(ARGS);
    RETURN->v_int = 0;
    if (e == NULL || msg == NULL || e->pending.empty()) {
        return;
    }
    *message_of(msg) = e->pending.front();
    e->pending.pop_front();
    RETURN->v_int = 1;
}

CK_DLL_CTOR(maxmsg_ctor)
{
    MaxInMessage* m = new MaxInMessage;
    m->name = "";
    m->argc = 0;
    OBJ_MEMBER_INT(SELF, maxmsg_offset_data) = (t_CKINT)m;
}

CK_DLL_DTOR(maxmsg_dtor)
{
    delete message_of(SELF);
    OBJ_MEMBER_INT(SELF, maxmsg_offset_data) = 0;
}

static const MaxInAtom* arg(Chuck_Object* self, t_CKINT i)
{
    MaxInMessage* m = message_of(self);
    return (m && i >= 0 && i < m->argc) ? &m->argv[i] : NULL;
}

CK_DLL_MFUN(maxmsg_name)
{
    MaxInMessage* m = message_of(SELF);
    RETURN->v_string = (Chuck_String*)API->object->create_string(
        VM, m ? m->name : "", FALSE);
}

CK_DLL_MFUN(maxmsg_num_args)
{
    MaxInMessage* m = message_of(SELF);
    RETURN->v_int = m ? m->argc : 0;
}

// Max sends 60 and 60. interchangeably, so numbers convert both ways
CK_DLL_MFUN(maxmsg_get_int)
{
    const MaxInAtom* a = arg(SELF, GET_NEXT_INT(ARGS));
    RETURN->v_int = !a ? 0
                  : a->type == MAXIN_INT ? a->i
                  : a->type == MAXIN_FLOAT ? (t_CKINT)a->f : 0;
}

CK_DLL_MFUN(maxmsg_get_float)
{
    const MaxInAtom* a = arg(SELF, GET_NEXT_INT(ARGS));
    RETURN->v_float = !a ? 0
                    : a->type == MAXIN_FLOAT ? a->f
                    : a->type == MAXIN_INT ? (t_CKFLOAT)a->i : 0;
}

CK_DLL_MFUN(maxmsg_get_string)
{
    const MaxInAtom* a = arg(SELF, GET_NEXT_INT(ARGS));
    RETURN->v_string = (Chuck_String*)API->object->create_string(
        VM, (a && a->type == MAXIN_STRING) ? a->s : "", FALSE);
}

t_CKBOOL CK_DLL_CALL maxin_query(Chuck_DL_Query* QUERY)
{
    // MaxMsg first: MaxIn.recv() takes one
    QUERY->begin_class(QUERY, "MaxMsg", "Object");
    QUERY->doc_class(QUERY, "one message received by a MaxIn");
    QUERY->add_ctor(QUERY, maxmsg_ctor);
    QUERY->add_dtor(QUERY, maxmsg_dtor);
    maxmsg_offset_data = QUERY->add_mvar(QUERY, "int", "@data", FALSE);

    QUERY->add_mfun(QUERY, maxmsg_name, "string", "name");
    QUERY->doc_func(QUERY, "message name: 'note' in 'msg note 60'");
    QUERY->add_mfun(QUERY, maxmsg_num_args, "int", "numArgs");
    QUERY->doc_func(QUERY, "number of arguments after the name");
    QUERY->add_mfun(QUERY, maxmsg_get_int, "int", "getInt");
    QUERY->add_arg(QUERY, "int", "index");
    QUERY->doc_func(QUERY, "argument as int; floats truncate, strings give 0");
    QUERY->add_mfun(QUERY, maxmsg_get_float, "float", "getFloat");
    QUERY->add_arg(QUERY, "int", "index");
    QUERY->doc_func(QUERY, "argument as float; strings give 0");
    QUERY->add_mfun(QUERY, maxmsg_get_string, "string", "getString");
    QUERY->add_arg(QUERY, "int", "index");
    QUERY->doc_func(QUERY, "argument as string; numbers give \"\"");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "MaxIn", "Event");
    QUERY->doc_class(QUERY, "receives 'msg <name> ...' sent to chuck~; "
                            "broadcasts once per message");
    QUERY->add_ctor(QUERY, maxin_ctor);
    QUERY->add_dtor(QUERY, maxin_dtor);
    maxin_offset_entry = QUERY->add_mvar(QUERY, "int", "@entry", FALSE);

    QUERY->add_mfun(QUERY, maxin_add_address, "void", "addAddress");
    QUERY->add_arg(QUERY, "string", "name");
    QUERY->doc_func(QUERY, "receive messages with this name");
    QUERY->add_mfun(QUERY, maxin_recv, "int", "recv");
    QUERY->add_arg(QUERY, "MaxMsg", "msg");
    QUERY->doc_func(QUERY, "move the oldest message into msg; 0 if none");
    QUERY->end_class(QUERY);
    return TRUE;
}
