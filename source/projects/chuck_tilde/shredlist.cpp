#include "shredlist.h"

#include "chuck.h"
#include "chuck_compile.h"
#include "chuck_vm.h"

#include <cstring>

bool shredlist_request(ShredListState* st)
{
    int expected = SHREDLIST_IDLE;
    if (st->phase.compare_exchange_strong(expected, SHREDLIST_REQUESTED,
                                          std::memory_order_acq_rel)) {
        return true;
    }
    return expected == SHREDLIST_REQUESTED;  // coalesce with the pending one
}

bool shredlist_service(ShredListState* st, Chuck_VM* vm)
{
    int expected = SHREDLIST_REQUESTED;
    if (!st->phase.compare_exchange_strong(expected, SHREDLIST_FILLING,
                                           std::memory_order_acq_rel)) {
        return false;
    }
    Chuck_VM_Shreduler* sh = vm->shreduler();
    sh->get_all_shreds(st->scratch);
    double sr = vm->srate() ? (double)vm->srate() : 1.0;

    st->total = (long)st->scratch.size();
    st->count = 0;
    for (Chuck_VM_Shred* s : st->scratch) {
        if (st->count == SHREDLIST_MAX) {
            break;
        }
        ShredListEntry* e = &st->entries[st->count++];
        e->id = s->get_id();
        e->elapsed = (sh->now_system - s->start) / sr;
        shredlist_copy_name(e->name, sizeof(e->name), s->name);
    }
    st->phase.store(SHREDLIST_READY, std::memory_order_release);
    return true;
}

bool shredlist_ready(ShredListState* st)
{
    return st->phase.load(std::memory_order_acquire) == SHREDLIST_READY;
}

void shredlist_release(ShredListState* st)
{
    st->phase.store(SHREDLIST_IDLE, std::memory_order_release);
}

void shredlist_copy_name(char* dst, size_t n, const std::string& src)
{
    if (n == 0) {
        return;
    }
    size_t len = src.size();
    if (len > n - 1) {
        len = n - 1;
        // back off continuation bytes (10xxxxxx) to the start of a code point
        while (len > 0 && ((unsigned char)src[len] & 0xC0) == 0x80) {
            len--;
        }
    }
    memcpy(dst, src.data(), len);
    dst[len] = '\0';
}

// compile without sporking; the compiler's output is named after the literal
// signifier, so rename it before any shred copies the name
static Chuck_VM_Code* compile_named(ChucK* ck, const std::string& code, const std::string& name)
{
    if (!ck->compileCode(code, "", 0)) {
        return NULL;
    }
    Chuck_VM_Code* vm_code = ck->vm()->carrier()->compiler->output();
    if (vm_code) {
        vm_code->name = name;
    }
    return vm_code;
}

t_CKUINT shredlist_add_code(ChucK* ck, const std::string& code, const std::string& name)
{
    Chuck_VM_Code* vm_code = compile_named(ck, code, name);
    if (!vm_code) {
        return 0;
    }
    // immediate = FALSE: queued for the VM thread, as compileCode does
    Chuck_VM_Shred* shred = ck->vm()->spork(vm_code, NULL, FALSE);
    return shred ? shred->get_id() : 0;
}

bool shredlist_replace_code(ChucK* ck, t_CKUINT id, const std::string& code,
                            const std::string& name)
{
    Chuck_VM_Code* vm_code = compile_named(ck, code, name);
    if (!vm_code) {
        return false;
    }
    Chuck_Msg* msg = new Chuck_Msg;
    msg->type = CK_MSG_REPLACE;
    msg->param = id;
    msg->code = vm_code;
    msg->reply_cb = NULL;  // the VM deletes the message
    ck->vm()->queue_msg(msg, 1);
    return true;
}

bool shredlist_replaced(Chuck_VM* vm, Chuck_VM_Shred* shred)
{
    // the old shred is out of the shreduler by now; the id resolves only if
    // a replacement holds it
    Chuck_VM_Shred* holder = vm->shreduler()->lookup(shred->get_id());
    return holder != NULL && holder != shred;
}
