#include "maxsend.h"

#include "chuck_oo.h"
#include "chuck_vm.h"

static f_maxsend_lookup g_lookup = NULL;

void maxsend_set_lookup(f_maxsend_lookup lookup)
{
    g_lookup = lookup;
}

// VM thread: the next free entry, or NULL if the queue is full; the caller
// fills it and calls commit()
static MaxSendMessage* reserve(MaxSendState* st, MaxSendType type, Chuck_String* receiver)
{
    if (st == NULL || receiver == NULL) {
        return NULL;
    }
    long tail = st->tail.load(std::memory_order_relaxed);
    if (tail - st->head.load(std::memory_order_acquire) == MAXSEND_QUEUE_SIZE) {
        st->dropped.fetch_add(1, std::memory_order_relaxed);
        return NULL;
    }
    MaxSendMessage* m = &st->queue[tail % MAXSEND_QUEUE_SIZE];
    m->type = type;
    m->receiver = st->host.intern(st->host.ctx, receiver->str().c_str());
    m->symbol = NULL;
    m->argc = 0;
    return m;
}

static void commit(MaxSendState* st)
{
    st->tail.store(st->tail.load(std::memory_order_relaxed) + 1, std::memory_order_release);
    st->host.request(st->host.ctx);
}

static MaxSendState* state_of(Chuck_VM* vm)
{
    return g_lookup ? g_lookup(vm) : NULL;
}

// Host.sendBang(string receiver)
CK_DLL_SFUN(maxsend_bang)
{
    Chuck_String* receiver = GET_NEXT_STRING(ARGS);
    MaxSendState* st = state_of(VM);
    if (reserve(st, MAXSEND_BANG, receiver)) {
        commit(st);
    }
}

// Host.sendFloat(string receiver, float value)
CK_DLL_SFUN(maxsend_float)
{
    Chuck_String* receiver = GET_NEXT_STRING(ARGS);
    t_CKFLOAT value = GET_NEXT_FLOAT(ARGS);
    MaxSendState* st = state_of(VM);
    MaxSendMessage* m = reserve(st, MAXSEND_FLOAT, receiver);
    if (m) {
        m->argv[0] = value;
        m->argc = 1;
        commit(st);
    }
}

// Host.sendSymbol(string receiver, string symbol)
CK_DLL_SFUN(maxsend_symbol)
{
    Chuck_String* receiver = GET_NEXT_STRING(ARGS);
    Chuck_String* symbol = GET_NEXT_STRING(ARGS);
    MaxSendState* st = state_of(VM);
    MaxSendMessage* m = reserve(st, MAXSEND_SYMBOL, receiver);
    if (m) {
        m->symbol = st->host.intern(st->host.ctx, symbol ? symbol->str().c_str() : "");
        commit(st);
    }
}

// Host.sendList(string receiver, float[] values); an empty list arrives as
// bang, as it does in Pd
CK_DLL_SFUN(maxsend_list)
{
    Chuck_String* receiver = GET_NEXT_STRING(ARGS);
    Chuck_ArrayFloat* values = (Chuck_ArrayFloat*)GET_NEXT_OBJECT(ARGS);
    MaxSendState* st = state_of(VM);
    t_CKINT n = values ? values->size() : 0;
    MaxSendMessage* m = reserve(st, n > 0 ? MAXSEND_LIST : MAXSEND_BANG, receiver);
    if (m == NULL) {
        return;
    }
    if (n > MAXSEND_MAX_ATOMS) {
        st->truncated.fetch_add(1, std::memory_order_relaxed);
        n = MAXSEND_MAX_ATOMS;
    }
    for (t_CKINT i = 0; i < n; i++) {
        t_CKFLOAT v = 0;
        values->get(i, &v);
        m->argv[i] = v;
    }
    m->argc = (long)n;
    commit(st);
}

void maxsend_add_methods(Chuck_DL_Query* QUERY)
{
    QUERY->add_sfun(QUERY, maxsend_bang, "void", "sendBang");
    QUERY->add_arg(QUERY, "string", "receiver");
    QUERY->doc_func(QUERY, "send bang to every [receive <receiver>], as [send] does");
    QUERY->add_sfun(QUERY, maxsend_float, "void", "sendFloat");
    QUERY->add_arg(QUERY, "string", "receiver");
    QUERY->add_arg(QUERY, "float", "value");
    QUERY->doc_func(QUERY, "send value to every [receive <receiver>]");
    QUERY->add_sfun(QUERY, maxsend_symbol, "void", "sendSymbol");
    QUERY->add_arg(QUERY, "string", "receiver");
    QUERY->add_arg(QUERY, "string", "symbol");
    QUERY->doc_func(QUERY, "send 'symbol <symbol>' to every [receive <receiver>]");
    QUERY->add_sfun(QUERY, maxsend_list, "void", "sendList");
    QUERY->add_arg(QUERY, "string", "receiver");
    QUERY->add_arg(QUERY, "float[]", "values");
    QUERY->doc_func(QUERY, "send values as a list to every [receive <receiver>]; "
                           "at most 32 elements, and an empty list sends bang");
}

void maxsend_drain(MaxSendState* st, long* dropped, long* truncated)
{
    long head = st->head.load(std::memory_order_relaxed);
    long tail = st->tail.load(std::memory_order_acquire);
    for (; head != tail; head++) {
        st->host.deliver(st->host.ctx, &st->queue[head % MAXSEND_QUEUE_SIZE]);
        st->head.store(head + 1, std::memory_order_release);
    }
    *dropped = st->dropped.exchange(0, std::memory_order_relaxed);
    *truncated = st->truncated.exchange(0, std::memory_order_relaxed);
}
