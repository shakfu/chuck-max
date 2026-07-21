/**
    @file
    chuck~: chuck for Max
*/

#include "ext.h"
#include "ext_obex.h"
#include "ext_preferences.h"
#include "z_dsp.h"

#ifdef __APPLE__
#include "unistd.h"
#endif

#include <array>
#include <filesystem>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "chuck.h"
#include "chuck_globals.h"
#include "chuck_dl.h"

#include <atomic>

// globals defs
#define CK_CHANNELS 1

// max simultaneous chuck~ objects that can receive routed callbacks
#define CK_MAX_INSTANCES 128
// depth of the audio-thread -> main-thread reply queue, per instance
#define CK_REPLY_QUEUE_SIZE 256
// max atoms carried by a single reply
#define CK_REPLY_MAX_ATOMS 64
// max in-flight 'get' requests whose variable name is remembered, per instance
#define CK_PENDING_SIZE 256
// max concurrent event listeners, per instance
#define CK_MAX_LISTENS 32

namespace fs = std::filesystem;

// global variables
int CK_INSTANCE_COUNT = 0;              // monotonic; source of per-object ids
int CK_INSTANCE_LIVE = 0;               // currently-alive instances; guards globalCleanup
std::vector<std::string> CK_INSTANCE_NAMES;

// forward decl so the instance slot table can refer to it
typedef struct _ck t_ck;

// chuck's globals callbacks carry a t_CKINT id and nothing else, so routing a
// reply back to the object that asked for it means encoding the destination in
// that id. slot indexes this table; the table is only mutated on the main thread
// (object creation / destruction) and only read on the audio thread, so atomic
// pointer slots are enough to make the lookup safe without locking
static std::atomic<t_ck*> CK_INSTANCE_SLOTS[CK_MAX_INSTANCES];

// a reply queued from the audio thread, drained on the main thread
typedef struct _ck_reply {
    t_symbol* selector;
    long argc;
    t_atom argv[CK_REPLY_MAX_ATOMS];
} t_ck_reply;

// pack / unpack a globals callback id: high 32 bits select the instance slot,
// low 32 bits are a per-instance ticket identifying the request
#define CK_ID_PACK(slot, ticket) \
    (((t_CKINT)(slot) << 32) | ((t_CKINT)(ticket) & 0xFFFFFFFFLL))
#define CK_ID_SLOT(id)   ((long)(((t_CKINT)(id)) >> 32))
#define CK_ID_TICKET(id) ((long)(((t_CKINT)(id)) & 0xFFFFFFFFLL))


// data structures
struct _ck {
    t_pxobject ob;                  // the object itself (t_pxobject in MSP)

    // max-related
    t_symbol* name;                 // instance unique name (used for scripting)
    t_patcher* patcher;             // to send msgs to objects
    t_box* box;                     // the ui box of the chuck~ instance?
    t_symbol* patcher_dir;          // patcher directory

    // chuck-related
    ChucK* chuck;                   // chuck instance
    int oid;                        // object id
    long channels;                  // n of input/output channels
    t_symbol* run_file;             // path of chuck file to run
    t_symbol* working_dir;          // chuck working directory
    t_symbol* chugins_dir;          // chugins directory
    float* in_chuck_buffer;         // intermediate chuck input buffer
    float* out_chuck_buffer;        // intermediate chuck output buffer
    long verbose;                   // max-side reporting verbosity (loglevel is
                                    // process-global; see ck_loglevel)
    long current_shred_id;          // current shred id
    t_symbol* editor;               // external text editor for chuck code
    t_symbol* edit_file;            // path of file to edit by external editor
    long run_needs_audio;           // only run/add shred if dsp is on
    t_symbol* code;                 // chuck code buffer

    // tap-related (for reading global UGen samples)
    long tap_channels;              // number of tap outlet channels (0 = disabled)
    t_symbol* tap_ugens[16];        // names of global UGens to tap (one per outlet, max 16)
    long tap_ugen_nchans[16];       // channel count of the UGen feeding this outlet (0 = mono)
    long tap_ugen_chan[16];         // which channel of that UGen this outlet carries
    float* tap_buffer;              // buffer for tapped samples
    long tap_buffer_frames;         // frames allocated per channel in tap_buffer

    // reply-related (routing chuck callbacks back out to the patch)
    void* reply_outlet;             // rightmost outlet: get/listen/shred replies
    void* reply_qelem;              // defers queued replies to the main thread
    long slot;                      // index into CK_INSTANCE_SLOTS (-1 if none)
    t_ck_reply reply_queue[CK_REPLY_QUEUE_SIZE];
    std::atomic<long> reply_head;   // read cursor, owned by the main thread
    std::atomic<long> reply_tail;   // write cursor, owned by the audio thread
    t_symbol* pending_names[CK_PENDING_SIZE]; // variable name per in-flight get
    long pending_ticket;            // next ticket to issue
    t_symbol* listen_names[CK_MAX_LISTENS];  // event name per active listener
};


// method prototypes
void* ck_new(t_symbol* s, long argc, t_atom* argv);
void ck_free(t_ck* x);
void ck_assist(t_ck* x, void* b, long m, long a, char* s);

// attribute handlers
t_max_err ck_editor_get(t_ck *x, t_object *attr, long *argc, t_atom **argv);
t_max_err ck_editor_set(t_ck *x, t_object *attr, long argc, t_atom *argv);

// scratch message handler (used for testing)
t_max_err ck_demo(t_ck* x, t_symbol* s, long argc, t_atom* argv);

// general message handlers
t_max_err ck_bang(t_ck* x);                     // (re)load chuck file
t_max_err ck_anything(t_ck* x, t_symbol* s, long argc, t_atom* argv); // set global params by name/value

// basic message handlers
t_max_err ck_run(t_ck* x, t_symbol* s);         // run chuck file
t_max_err ck_edit(t_ck* x, t_symbol* s);        // edit chuck file

// chuck vm message handlers
t_max_err ck_add(t_ck* x, t_symbol* s, long argc, t_atom* argv);     // add shred from file
t_max_err ck_eval(t_ck* x, t_symbol* s, long argc, t_atom* argv);    // add chuck code as shred
t_max_err ck_remove(t_ck* x, t_symbol* s, long argc, t_atom* argv);  // remove shreds (all, last, by #)
t_max_err ck_replace(t_ck* x, t_symbol* s, long argc, t_atom* argv); // replace shreds 
t_max_err ck_clear(t_ck* x, t_symbol* s, long argc, t_atom* argv);   // clear_vm, clear_globals
t_max_err ck_reset(t_ck* x, t_symbol* s, long argc, t_atom* argv);   // clear_vm, reset_id
t_max_err ck_removeall(t_ck* x);                // remove all shreds (keeps VM state)
t_max_err ck_abort(t_ck* x);                    // abort the currently-running shred
t_max_err ck_status(t_ck* x);                   // metadata about running shreds in the chuck vm
t_max_err ck_time(t_ck* x);                     // current time

// event/callback message handlers
t_max_err ck_signal(t_ck* x, t_symbol* s);      // signal global event
t_max_err ck_broadcast(t_ck* x, t_symbol* s);   // broadcast global event
t_max_err ck_listen(t_ck* x, t_symbol* s, long listen_forever);
t_max_err ck_unlisten(t_ck* x, t_symbol* s);

// special message handlers
t_max_err ck_chugins(t_ck* x);                  // probe and list available chugins
t_max_err ck_globals(t_ck* x);                  // list global variables
t_max_err ck_docs(t_ck* x);                     // open chuck docs in a browser
t_max_err ck_vm(t_ck* x);                       // get vm state
t_max_err ck_loglevel(t_ck* x, t_symbol* s, long argc, t_atom* argv);  // get and set loglevels

// tap message handler (for reading global UGen samples)
t_max_err ck_tap(t_ck* x, t_symbol* s, long argc, t_atom* argv);  // set global UGen to tap

// param message handler (for querying/setting VM parameters)
t_max_err ck_param(t_ck* x, t_symbol* s, long argc, t_atom* argv);

// shreds message handler (for shred introspection)
t_max_err ck_shreds(t_ck* x, t_symbol* s, long argc, t_atom* argv);

// adaptive message handler (for adaptive block processing control)
t_max_err ck_adaptive(t_ck* x, t_symbol* s, long argc, t_atom* argv);

// error-reporting / logging helpers
void ck_stdout_print(const char* msg);
void ck_stderr_print(const char* msg);
// let the compiler type-check the varargs against the format string
#if defined(__GNUC__) || defined(__clang__)
#define CK_PRINTF_FMT(fmt_idx, args_idx) \
    __attribute__((format(printf, fmt_idx, args_idx)))
#else
#define CK_PRINTF_FMT(fmt_idx, args_idx)
#endif

void ck_info(t_ck* x, const char* fmt, ...) CK_PRINTF_FMT(2, 3);
void ck_warn(t_ck* x, const char* fmt, ...) CK_PRINTF_FMT(2, 3);
void ck_debug(t_ck* x, const char* fmt, ...) CK_PRINTF_FMT(2, 3);
void ck_error(t_ck* x, const char* fmt, ...) CK_PRINTF_FMT(2, 3);

// helpers
void replace_character(char* str, char c1, char c2);
bool path_exists(const char* name);
bool is_safe_path(const char* path);
char* ck_atom_gettext(long ac, t_atom* av);
void ck_dblclick(t_ck* x);
t_max_err ck_send_max_msg(t_ck* x, t_symbol* s, const char* parsestr);
t_symbol* ck_check_file(t_ck* x, t_symbol* name);
t_max_err ck_compile_code(t_ck* x, const char* code, const char* args);
t_max_err ck_compile_file(t_ck* x, const char* filename);
t_max_err ck_run_file(t_ck* x);
t_max_err ck_send_chuck_vm_msg(t_ck* x, Chuck_Msg_Type msg_type);
t_symbol* ck_get_path_from_external(t_class* c, char* subpath);
t_symbol* ck_get_path_from_package(t_class* c, char* subpath);

// audio processing
void ck_dsp64(t_ck* x, t_object* dsp64, short* count, double samplerate,
              long maxvectorsize, long flags);
void ck_perform64(t_ck* x, t_object* dsp64, double** ins, long numins,
                  double** outs, long numouts, long sampleframes, long flags,
                  void* userparam);

// global variable get/set via callbacks
t_max_err ck_get(t_ck* x, t_symbol* s, long argc, t_atom* argv);
t_max_err ck_set(t_ck* x, t_symbol* s, long argc, t_atom* argv);

// reply plumbing: chuck's globals callbacks fire on the audio thread, so replies
// are queued there and flushed out the reply outlet on the main thread
void ck_reply_push(t_ck* x, t_symbol* selector, long argc, t_atom* argv);
void ck_reply_drain(t_ck* x);
t_ck* ck_instance_from_id(t_CKINT id);
t_symbol* ck_pending_name(t_ck* x, t_CKINT id);
t_CKINT ck_pending_issue(t_ck* x, t_symbol* name);

// callbacks (events); the id encodes instance slot + listener ticket
void cb_event(t_CKINT id);

// callbacks (variables); the id encodes instance slot + request ticket
void cb_get_int(t_CKINT id, t_CKINT val);
void cb_get_float(t_CKINT id, t_CKFLOAT val);
void cb_get_string(t_CKINT id, const char* val);
void cb_get_int_array(t_CKINT id, t_CKINT array[], t_CKUINT n);
void cb_get_int_array_value(t_CKINT id, t_CKINT value);
void cb_get_float_array(t_CKINT id, t_CKFLOAT array[], t_CKUINT n);
void cb_get_float_array_value(t_CKINT id, t_CKFLOAT value);
void cb_get_assoc_int_array_value(t_CKINT id, t_CKINT val);
void cb_get_assoc_float_array_value(t_CKINT id, t_CKFLOAT val);

// shred lifecycle watcher; BINDLE carries the t_ck* directly
void CK_DLL_CALL cb_shreds_watcher(Chuck_VM_Shred* shred, t_CKINT code,
                                   t_CKINT param, Chuck_VM* vm, void* bindle);

// dump all global variables
void cb_get_all_global_vars(const std::vector<Chuck_Globals_TypeValue> & list, void * data);


// global class pointer variable
static t_class* ck_class = NULL;

// cached reply selectors. every message out of the reply outlet uses one of
// these: the selector is always ours and any user-supplied name travels as an
// argument, so no ChucK global can ever collide with the reply vocabulary.
// cached rather than gensym'd at call time because these are emitted from the
// audio thread
static t_symbol* ps_val = NULL;      // val <name> <value...>   -- 'get' reply
static t_symbol* ps_event = NULL;    // event <name>            -- 'listen'
static t_symbol* ps_shred = NULL;    // shred add|remove <id>   -- vm watcher
static t_symbol* ps_global = NULL;   // global <name> <type>    -- 'globals'


//-----------------------------------------------------------------------------------------------
// core

void ext_main(void* r)
{
    t_class* c = class_new("chuck~", (method)ck_new, (method)ck_free,
                           (long)sizeof(t_ck), 0L, A_GIMME, 0);

    // object methods
    //------------------------------------------------------------------------

    class_addmethod(c, (method)ck_run,          "run",      A_SYM, 0);
    class_addmethod(c, (method)ck_edit,         "edit",     A_SYM, 0);

    class_addmethod(c, (method)ck_demo,         "demo",     A_GIMME, 0);

    class_addmethod(c, (method)ck_add,          "add",      A_GIMME, 0);
    class_addmethod(c, (method)ck_eval,         "eval",     A_GIMME, 0);    
    class_addmethod(c, (method)ck_remove,       "remove",   A_GIMME, 0);
    class_addmethod(c, (method)ck_replace,      "replace",  A_GIMME, 0);
    class_addmethod(c, (method)ck_clear,        "clear",    A_GIMME, 0);
    class_addmethod(c, (method)ck_reset,        "reset",    A_GIMME, 0); // reset -> 'clear vm', 'reset id'
    class_addmethod(c, (method)ck_removeall,    "removeall", 0);
    class_addmethod(c, (method)ck_abort,        "abort",    0);
    class_addmethod(c, (method)ck_status,       "status",   0);
    class_addmethod(c, (method)ck_time,         "time",     0);

    class_addmethod(c, (method)ck_chugins,      "chugins",  0);
    class_addmethod(c, (method)ck_globals,      "globals",  0);
    class_addmethod(c, (method)ck_vm,           "vm",       0);
    class_addmethod(c, (method)ck_docs,         "docs",     0);
    class_addmethod(c, (method)ck_loglevel,     "loglevel", A_GIMME, 0);

    // can't be called signal which is a Max global message
    class_addmethod(c, (method)ck_signal,       "sig",      A_SYM, 0);
    class_addmethod(c, (method)ck_broadcast,    "broadcast", A_SYM, 0);

    class_addmethod(c, (method)ck_get,          "get",      A_GIMME, 0);
    class_addmethod(c, (method)ck_set,          "set",      A_GIMME, 0);
    class_addmethod(c, (method)ck_listen,       "listen",   A_SYM, A_DEFLONG, 0);
    class_addmethod(c, (method)ck_unlisten,     "unlisten", A_SYM, 0);

    class_addmethod(c, (method)ck_tap,          "tap",      A_GIMME, 0);
    class_addmethod(c, (method)ck_param,        "param",    A_GIMME, 0);
    class_addmethod(c, (method)ck_shreds,       "shreds",   A_GIMME, 0);
    class_addmethod(c, (method)ck_adaptive,     "adaptive", A_GIMME, 0);

    class_addmethod(c, (method)ck_bang,         "bang",     0);
    class_addmethod(c, (method)ck_anything,     "anything", A_GIMME, 0);

    class_addmethod(c, (method)ck_dsp64,        "dsp64",    A_CANT, 0);
    class_addmethod(c, (method)ck_assist,       "assist",   A_CANT, 0);
    class_addmethod(c, (method)ck_dblclick,     "dblclick", A_CANT, 0);
    
    // class attributes
    //------------------------------------------------------------------------

    CLASS_ATTR_SYM(c,       "editor", 0,        t_ck, editor);
    CLASS_ATTR_BASIC(c,     "editor", 0);
    CLASS_ATTR_ACCESSORS(c, "editor", NULL,     ck_editor_set);
    // CLASS_ATTR_ACCESSORS(c, "editor", ck_editor_get, ck_editor_set);

    CLASS_ATTR_SYM(c,       "file", 0,          t_ck,  run_file);
    CLASS_ATTR_STYLE(c,     "file", 0,          "file");
    CLASS_ATTR_BASIC(c,     "file", 0);
    // CLASS_ATTR_SAVE(c,      "file", 0);

    // can only run / add while dsp is on
    CLASS_ATTR_LONG(c,      "run_needs_audio", 0,  t_ck, run_needs_audio);
    CLASS_ATTR_STYLE(c,     "run_needs_audio", 0, "onoff");
    CLASS_ATTR_BASIC(c,     "run_needs_audio", 0);
    // CLASS_ATTR_SAVE(c,      "run_needs_audio", 0);

    // verbose: how much this object reports, independent of 'loglevel' which
    // controls the ChucK VM's own logging. 0 quiet, 1 normal, 2 adds debug
    CLASS_ATTR_LONG(c,      "verbose", 0, t_ck, verbose);
    CLASS_ATTR_LABEL(c,     "verbose", 0, "Object Reporting Verbosity");
    CLASS_ATTR_BASIC(c,     "verbose", 0);
    CLASS_ATTR_FILTER_CLIP(c, "verbose", 0, 2);

    // ntap: number of additional outlets for tapping global UGen samples
    CLASS_ATTR_LONG(c,      "ntap", 0, t_ck, tap_channels);
    CLASS_ATTR_LABEL(c,     "ntap", 0, "Number of Tap Outlet Channels");
    CLASS_ATTR_BASIC(c,     "ntap", 0);
    CLASS_ATTR_FILTER_CLIP(c, "ntap", 0, 16);  // limit to 0-16 channels

    // clang-format on
    //------------------------------------------------------------------------

    class_dspinit(c);
    class_register(CLASS_BOX, c);
    ck_class = c;

    // cache the reply selectors once
    ps_val = gensym("val");
    ps_event = gensym("event");
    ps_shred = gensym("shred");
    ps_global = gensym("global");
}

void* ck_new(t_symbol* s, long argc, t_atom* argv)
{
    t_ck* x = (t_ck*)object_alloc(ck_class);

    if (x) {
        // set default attributes (allcaps can be overriden by compile-time defs)
        x->channels = CK_CHANNELS;
        // 0 by default, which is what the old loglevel gating amounted to in
        // practice: ck_info required loglevel >= 5 against a default of 2
        x->verbose = 0;
        x->current_shred_id = 0;
        x->run_file = gensym("");
        x->edit_file = gensym("");
        x->editor = gensym("");
        x->working_dir = ck_get_path_from_package(ck_class, (char*)"/examples");
        x->patcher = NULL;
        x->box = NULL;
        x->patcher_dir = gensym("");
        x->code = gensym("");
        x->run_needs_audio = 1; // Default to 1 to prevent issue #11 (loading files before audio)

        // tap defaults
        x->tap_channels = 0;
        for (int i = 0; i < 16; i++) {
            x->tap_ugens[i] = gensym("");
            x->tap_ugen_nchans[i] = 0;
            x->tap_ugen_chan[i] = 0;
        }
        x->tap_buffer = NULL;
        x->tap_buffer_frames = 0;

        // reply defaults
        x->reply_outlet = NULL;
        x->reply_qelem = NULL;
        x->slot = -1;
        x->reply_head.store(0);
        x->reply_tail.store(0);
        x->pending_ticket = 0;
        for (int i = 0; i < CK_PENDING_SIZE; i++) {
            x->pending_names[i] = NULL;
        }
        for (int i = 0; i < CK_MAX_LISTENS; i++) {
            x->listen_names[i] = NULL;
        }

        // get external editor
        if (const char* editor = std::getenv("EDITOR")) {
            ck_info(x, (char*)"editor from env: %s", editor);
            x->editor = gensym(editor);
        } else {
            x->editor = gensym("");
        }

        // object id corresponds to order of object creation
        x->oid = CK_INSTANCE_COUNT++;
        CK_INSTANCE_LIVE++;

        // set patcher object
        object_obex_lookup(x, gensym("#P"), (t_patcher**)&x->patcher);
        if (x->patcher == NULL)
            ck_error(x, (char*)"patcher object not created.");

        // set box object
        object_obex_lookup(x, gensym("#B"), (t_box**)&x->box);
        if (x->box == NULL)
            ck_error(x, (char*)"box object not created.");

        // create scripting name
        char name[MAX_FILENAME_CHARS];
        int res = snprintf_zero(name, MAX_FILENAME_CHARS, "chuck-%d", x->oid);
        if (res >= 0 && res < MAX_FILENAME_CHARS) {
            x->name = gensym(name);
        } else {
            x->name = symbol_unique(); // fallback to unique symbol name
        }
        // register the object in the namespace
        object_register(CLASS_BOX, x->name, x);

        // set the scripting name
        t_max_err err = jbox_set_varname(x->box, x->name);
        if (err != MAX_ERR_NONE) {
            ck_error(x, (char*)"could not set chuck~ box's scripting name");
        }

        // add to the global vector of chuck instance names
        CK_INSTANCE_NAMES.push_back(std::string(x->name->s_name));

        // get patcher directory
        t_symbol *p_path = object_attr_getsym(x->patcher, gensym("filepath"));
        char patcher_dir[MAX_FILENAME_CHARS];
        char patcher_file[MAX_PATH_CHARS];
        char patcher_conform_dir[MAX_PATH_CHARS];
        path_splitnames(p_path->s_name, patcher_dir, patcher_file);
        path_nameconform(patcher_dir, patcher_conform_dir, PATH_STYLE_MAX, PATH_TYPE_BOOT);
        x->patcher_dir = gensym(patcher_conform_dir);

        // get object instance args
        // use attr_args_offset to find where positional args end and attributes begin
        long attr_offset = attr_args_offset(argc, argv);
        t_symbol* filename;

        if (attr_offset == 1) {
            if (argv->a_type == A_LONG) {
                atom_arg_getlong((t_atom_long*)&x->channels, 0, argc, argv);
            } else if (argv->a_type == A_SYM) {
                filename = atom_getsymarg(0, argc, argv);
                x->run_file = ck_check_file(x, filename);
            } else {
                ck_error(x, (char*)"invalid object arguments");
                return NULL;
            }
        }
        else if (attr_offset >= 2) {
            atom_arg_getlong((t_atom_long*)&x->channels, 0, argc, argv);  // is 1st arg of object
            filename = atom_getsymarg(1, argc, argv);    // is 2nd arg of object
            x->run_file = ck_check_file(x, filename);
        }
        // else just use defaults (attr_offset == 0 means no positional args)

        // process attributes early to get tap_channels (@ntap) before creating outlets
        attr_args_process(x, argc, argv);

        // DEBUG: print attribute processing results
        post("chuck~: argc=%ld attr_offset=%ld channels=%ld tap_channels=%ld",
             argc, attr_offset, x->channels, x->tap_channels);

        dsp_setup((t_pxobject*)x, x->channels);   // MSP inlets: 2nd arg is # of inlets

        // outlets are created right to left: the FIRST outlet_new call produces
        // the rightmost outlet. so the reply outlet must be created before the
        // signal outlets to land on the right. creating it last would make it
        // outlet 0 and shift every audio and tap outlet one position over,
        // silently rewiring existing patches
        x->reply_outlet = outlet_new((t_object*)x, NULL);
        x->reply_qelem = qelem_new(x, (method)ck_reply_drain);

        // the signal outlets below are interchangeable at creation time; which
        // one carries audio and which carries a tap is decided by the outs[]
        // index written in ck_perform64, and outs[] runs left to right. so the
        // main outlets are outs[0..channels-1] and the taps follow them
        for (int i = 0; i < x->channels; i++) {
            outlet_new((t_pxobject*)x, "signal"); // main signal outlet
        }
        for (int i = 0; i < x->tap_channels; i++) {
            outlet_new((t_pxobject*)x, "signal"); // tap signal outlet
        }

        // claim an instance slot so chuck's callbacks can find their way back
        for (long i = 0; i < CK_MAX_INSTANCES; i++) {
            t_ck* expected = NULL;
            if (CK_INSTANCE_SLOTS[i].compare_exchange_strong(expected, x)) {
                x->slot = i;
                break;
            }
        }
        if (x->slot < 0) {
            ck_warn(x, (char*)"more than %d chuck~ objects: replies from this "
                             "instance will not be routed", CK_MAX_INSTANCES);
        }

        // chuck-related
        x->chuck = NULL;
#if defined(__APPLE__) && defined(CM_MACOS_BUNDLED_CHUGINS)
        x->chugins_dir = ck_get_path_from_external(ck_class, (char*)"/Contents/Resources/chugins");
#elif defined(CM_MULTIPLATFORM_CHUGINS)

#if defined(__APPLE__) && defined(__aarch64__)
        x->chugins_dir = ck_get_path_from_package(ck_class, (char*)"/examples/chugins/darwin-arm64");
#elif defined(__APPLE__) && defined(__x86_64__)
        x->chugins_dir = ck_get_path_from_package(ck_class, (char*)"/examples/chugins/darwin-x86_64");
#else
        x->chugins_dir = ck_get_path_from_package(ck_class, (char*)"/examples/chugins/windows-amd64");
#endif

#else
        x->chugins_dir = ck_get_path_from_package(ck_class, (char*)"/examples/chugins");
#endif        
        x->in_chuck_buffer = NULL;
        x->out_chuck_buffer = NULL;

        x->chuck = new ChucK();
        if (x->chuck == NULL) {
            ck_error(x, (char*)"critical: could not create chuck object");
            return NULL;
        }
        x->chuck->setParam(CHUCK_PARAM_SAMPLE_RATE, (t_CKINT)sys_getsr());
        x->chuck->setParam(CHUCK_PARAM_INPUT_CHANNELS, (t_CKINT)x->channels);
        x->chuck->setParam(CHUCK_PARAM_OUTPUT_CHANNELS, (t_CKINT)x->channels);
        x->chuck->setParam(CHUCK_PARAM_VM_HALT, (t_CKINT)0);
        x->chuck->setParam(CHUCK_PARAM_DUMP_INSTRUCTIONS, (t_CKINT)0);

        // chuck~ is always driven from the MSP audio callback, so tell the VM so;
        // this defaults to 0 and affects realtime-dependent behaviour in the engine
        x->chuck->setParam(CHUCK_PARAM_IS_REALTIME_AUDIO_HINT, (t_CKINT)1);

        // set default chuck examples dirs
        std::string global_dir = std::string(x->working_dir->s_name);
        x->chuck->setParam(CHUCK_PARAM_WORKING_DIRECTORY, global_dir);

        // enable chugins
        x->chuck->setParam(CHUCK_PARAM_CHUGIN_ENABLE, (t_CKINT)1);

        // set default chugins dirs
        std::string chugins_dir = std::string(x->chugins_dir->s_name);
        std::list<std::string> chugin_search;
        chugin_search.push_back(chugins_dir);
#if defined(__APPLE__) && defined(CK_EMBEDDED_CHUGINS)
        // add extra package-level chugins directory
        chugin_search.push_back(global_dir + "/chugins");
#endif
        x->chuck->setParam(CHUCK_PARAM_IMPORT_PATH_SYSTEM, chugin_search);
        // redirect chuck stdout/stderr to local callbacks.
        // these are process-wide, but they are the ONLY route for the VM's own
        // printing: 'status' output, the "(VM) ..." messages, EM_log, and so on.
        // dropping them in favour of chout/cherr alone silences all of that
        x->chuck->setStdoutCallback(ck_stdout_print);
        x->chuck->setStderrCallback(ck_stderr_print);

        // init chuck
        x->chuck->init();
        x->chuck->start();
        // loglevel is process-global (ChucK::setLogLevel is static). set the
        // package default just once, on the first instance, so creating a
        // second chuck~ does not reset a level the user changed on the first.
        // ChucK itself defaults to CK_LOG_CORE (1); we prefer CK_LOG_SYSTEM (2)
        static bool s_loglevel_defaulted = false;
        if (!s_loglevel_defaulted) {
            ChucK::setLogLevel(CK_LOG_SYSTEM);
            s_loglevel_defaulted = true;
        }

        // chout/cherr carry <<< >>> output from chuck code and are per-instance.
        // they must be set AFTER init(): setChoutCallback() bails out early
        // unless m_init is true and the carrier's chout already exists
        x->chuck->setChoutCallback(ck_stdout_print);
        x->chuck->setCherrCallback(ck_stderr_print);

        // report shred lifecycle out the reply outlet; the bindle carries the
        // instance, so this needs no id packing
        x->chuck->vm()->subscribe_watcher(
            cb_shreds_watcher,
            ckvm_shreds_watch_SPORK | ckvm_shreds_watch_REMOVE,
            x);

        post("ChucK %s", x->chuck->version());
        post("inputs: %d  outputs: %d  tap: %d",
            x->chuck->vm()->m_num_adc_channels,
            x->chuck->vm()->m_num_dac_channels,
            x->tap_channels);
        post("file: %s", x->run_file->s_name);
        post("working dir: %s", x->working_dir->s_name);
        post("chugins dir: %s", x->chugins_dir->s_name);
        post("patcher_dir: %s", x->patcher_dir->s_name);
    }
    return x;
}


void ck_free(t_ck* x)
{
    if (x->in_chuck_buffer) {
        delete[] x->in_chuck_buffer;
        x->in_chuck_buffer = NULL;
    }
    if (x->out_chuck_buffer) {
        delete[] x->out_chuck_buffer;
        x->out_chuck_buffer = NULL;
    }
    if (x->tap_buffer) {
        delete[] x->tap_buffer;
        x->tap_buffer = NULL;
    }
    // release the instance slot first so any callback still in flight on the
    // audio thread resolves to NULL and bails out instead of touching a
    // half-destroyed object
    if (x->slot >= 0) {
        CK_INSTANCE_SLOTS[x->slot].store(NULL, std::memory_order_release);
        x->slot = -1;
    }
    if (x->chuck) {
        if (x->chuck->vm()) {
            x->chuck->vm()->remove_watcher(cb_shreds_watcher);
        }
        delete x->chuck;
        x->chuck = NULL;
    }
    // no further qelem_set can occur now that the VM is gone
    if (x->reply_qelem) {
        qelem_free(x->reply_qelem);
        x->reply_qelem = NULL;
    }
    // globalCleanup() tears down process-wide chuck state, so it may only run
    // once the last chuck~ in the process is gone; calling it per-instance
    // pulled that state out from under any still-running siblings
    if (--CK_INSTANCE_LIVE <= 0) {
        CK_INSTANCE_LIVE = 0;
        ChucK::globalCleanup();
    }
    dsp_free((t_pxobject*)x);
}


void ck_assist(t_ck* x, void* b, long m, long a, char* s)
{
    if (m == ASSIST_INLET) { // inlet
        snprintf_zero(s, 512, "(signal) audio input %ld", a + 1);
    } else {                 // outlet
        if (a < x->channels) {
            snprintf_zero(s, 512, "(signal) audio output %ld", a + 1);
        } else if (a < x->channels + x->tap_channels) {
            long tap_index = a - x->channels;
            snprintf_zero(s, 512, "(signal) tap output %ld", tap_index + 1);
        } else {
            snprintf_zero(s, 512, "(list) get values, events, shred changes");
        }
    }
}


//-----------------------------------------------------------------------------------------------
// general utilities

std::vector<std::string> split(std::string s, char delimiter = ' ')
{
    std::vector<std::string> output;
    for (auto cur = std::begin(s), beg = cur;; ++cur) {
        if (cur == std::end(s) || *cur == delimiter || !*cur) {
            output.insert(output.end(), std::string(beg, cur));
            if (cur == std::end(s) || !*cur)
                break;
            else
                beg = std::next(cur);
        }
    }
    return output;
}

std::string join(std::vector<std::string> elements,
    const char* const delimiter)
{
    std::ostringstream os;
    auto b = std::begin(elements);
    auto e = std::end(elements);

    if (b != e) {
        std::copy(b, std::prev(e),
            std::ostream_iterator<std::string>(os, delimiter));
        b = std::prev(e);
    }
    if (b != e) {
        os << *b;
    }

    return os.str();
}


#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

std::string get_output(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        error("popen() failed!");
        return std::string("");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

bool is_path(const char* target)
{
    std::string s = std::string(target);
    return (s.find('/') != std::string::npos);
}


void replace_character(char* str, char c1, char c2)
{
    size_t j, n = strlen(str);
    for (size_t i = j = 0; i < n; i++) {
        if (str[i] != c1) {
            str[j++] = str[i];
        }
        else {
            str[j++] = c2;
        }
    }
    str[j] = '\0';
}


/* x-platform solution to check if a path exists
 * 
 * since ext_path.h (`path_exists`) is not available
 * and std::filesystem::exists requires macos >= 10.15
 */
#ifdef __APPLE__
bool path_exists(const char* name) {
   return access( name, 0 ) == 0;
}
#else
bool path_exists(const char* name) {

    if (FILE *file = fopen(name, "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}
#endif

bool is_safe_path(const char* path)
{
    if (!path) return false;

    std::string p(path);

    // Check for directory traversal attempts
    if (p.find("..") != std::string::npos) return false;

    // Check for absolute paths on Unix-like systems
    if (p.length() > 0 && p[0] == '/') return false;

    // Check for Windows absolute paths
    if (p.length() > 2 && p[1] == ':') return false;

    // Check for Windows UNC paths
    if (p.length() > 1 && p[0] == '\\' && p[1] == '\\') return false;

    // Check for null bytes (can be used to bypass checks)
    if (p.find('\0') != std::string::npos) return false;

    return true;
}

// repurposed from simplestring_atom_gettext
char* ck_atom_gettext(long ac, t_atom* av)
{
    if (ac && av) {
        char* text = NULL;
        long size = 0;
        atom_gettext(ac, av, &size, &text, OBEX_UTIL_ATOM_GETTEXT_SYM_NO_QUOTE | OBEX_UTIL_ATOM_GETTEXT_NOESCAPE | OBEX_UTIL_ATOM_GETTEXT_LINEBREAK_NODELIM);
        if (text && size) {
            return text;
        }
    }
    return NULL;
}

//-----------------------------------------------------------------------------------------------
// attribute set/get

t_max_err ck_editor_set(t_ck *x, t_object *attr, long argc, t_atom *argv)
{
    t_symbol* editor = atom_getsym(argv);
    ck_info(x, (char*)"editor_set: %s", editor->s_name);
    if (path_exists(editor->s_name)) {
        x->editor = editor;
        return MAX_ERR_NONE;
    }
    return MAX_ERR_NONE;
}

t_max_err ck_editor_get(t_ck *x, t_object *attr, long *argc, t_atom **argv)
{   
    char alloc;
    atom_alloc(argc, argv, &alloc); 
    ck_info(x, (char*)"editor_get: %s", x->editor->s_name);
    t_max_err err =  atom_setsym(*argv, x->editor);
    if (err != MAX_ERR_NONE) {
        ck_error(x, (char*)"failed: ck_editor_get");
    }
    return err;
}


//-----------------------------------------------------------------------------------------------
// helpers



// Reporting helpers.
//
// These are gated on x->verbose, NOT on the ChucK VM log level. loglevel means one thing
// only -- how chatty the ChucK VM is in its own internal logging, which is what
// ChucK::setLogLevel() controls. Conflating the two meant you could not make
// this object talk without also making the engine talk, and the default
// loglevel never reached these thresholds anyway, so 47 of the 118 calls were
// dead. See source/docs/logging.md.
//
// Errors and warnings are never gated: a warning nobody sees is not a warning.
// Query answers ('status', 'vm', 'param', 'get', ...) use object_post directly,
// since a question that returns nothing is indistinguishable from a broken
// object.
void ck_warn(t_ck* x, const char* fmt, ...)
{
    char msg[MAX_PATH_CHARS];

    va_list va;
    va_start(va, fmt);
    vsnprintf(msg, MAX_PATH_CHARS, fmt, va);
    va_end(va);

    object_warn((t_object*)x, "[warn] %s", msg);
}

void ck_info(t_ck* x, const char* fmt, ...)
{
    if (x->verbose >= 1) {
        char msg[MAX_PATH_CHARS];

        va_list va;
        va_start(va, fmt);
        vsnprintf(msg, MAX_PATH_CHARS, fmt, va);
        va_end(va);

        object_post((t_object*)x, "[info] %s", msg);
    }
}

void ck_debug(t_ck* x, const char* fmt, ...)
{
    if (x->verbose >= 2) {
        char msg[MAX_PATH_CHARS];

        va_list va;
        va_start(va, fmt);
        vsnprintf(msg, MAX_PATH_CHARS, fmt, va);
        va_end(va);

        object_post((t_object*)x, "[debug] %s", msg);
    }
}

void ck_error(t_ck* x, const char* fmt, ...)
{
    char msg[MAX_PATH_CHARS];

    va_list va;
    va_start(va, fmt);
    vsnprintf(msg, MAX_PATH_CHARS, fmt, va);
    va_end(va);

    object_error((t_object*)x, "[error] %s", msg);
}


void ck_stdout_print(const char* msg) { post("%s", msg); }

void ck_stderr_print(const char* msg) { post("%s", msg); }


Chuck_VM_Shred * ck_shred_lookup(t_ck* x, long id)
{
    Chuck_VM_Shreduler* shreduler = x->chuck->vm()->shreduler();
    return shreduler->lookup(id);
}

std::vector<t_CKUINT> ck_get_ready_shred_ids(t_ck* x)
{
    std::vector<t_CKUINT> shred_ids;
    Chuck_VM_Shreduler* shreduler = x->chuck->vm()->shreduler();
    shreduler->get_ready_shred_ids(shred_ids);
    return shred_ids;
}

std::vector<t_CKUINT> ck_get_blocked_shred_ids(t_ck* x)
{
    std::vector<t_CKUINT> shred_ids;
    Chuck_VM_Shreduler* shreduler = x->chuck->vm()->shreduler();
    shreduler->get_blocked_shred_ids(shred_ids);
    return shred_ids;
}

std::vector<t_CKUINT>  ck_get_all_shred_ids(t_ck* x)
{
    std::vector<t_CKUINT> shred_ids;
    Chuck_VM_Shreduler* shreduler = x->chuck->vm()->shreduler();
    shreduler->get_all_shred_ids(shred_ids);
    return shred_ids;
}

long ck_spork_highest_id(t_ck* x)
{
    Chuck_VM_Shreduler* shreduler = x->chuck->vm()->shreduler();
    return shreduler->highest();
}

long ck_spork_last_id(t_ck* x)
{
    long id = x->chuck->vm()->last_id();
    ck_info(x, "last_id: %ld", (long)id);
    return id;
}

long ck_spork_next_id(t_ck* x)
{
    long id = x->chuck->vm()->next_id();
    ck_info(x, "next_id: %ld", (long)id);
    return id;
}

/**
 * @brief      Send a message to Max object
 *
 * @param      x         object instance
 * @param      s         method name
 * @param[in]  parsestr  method args if any
 * 
 * eg. (; max launchbrowser http://www.cycling74.com)
 *
 * @return     Max error
 */
t_max_err ck_send_max_msg(t_ck* x, t_symbol* s, const char* parsestr)
{
    t_object *maxobj = (t_object*)object_new(CLASS_NOBOX, gensym("max"));
    if (maxobj == NULL) {
        ck_error(x, (char*)"could not get max object");
        return MAX_ERR_GENERIC;
    }
    t_max_err err = object_method_parse(maxobj, s, parsestr, NULL);
    if (err != MAX_ERR_NONE) {
        ck_error(x, (char*)"could not send msg: ;max %s %s", s->s_name, parsestr);
        return err;
    }
    return MAX_ERR_NONE;
}

/**
 * @brief Get external's path
 * 
 * @param c             class
 * @param subpath       optional subpath (NULL if no subpath)
 * @return t_symbol*    external path as symbol
 */
t_symbol* ck_get_path_from_external(t_class* c, char* subpath)
{
    char external_path[MAX_PATH_CHARS];
    char external_name[MAX_PATH_CHARS];
    char conform_path[MAX_PATH_CHARS];
    short path_id = class_getpath(c);
    t_string* result;

#ifdef __APPLE__
    const char* ext_filename = "%s.mxo";
#else
    const char* ext_filename = "%s.mxe64";
#endif
    snprintf_zero(external_name, MAX_PATH_CHARS, ext_filename, c->c_sym->s_name);
    path_toabsolutesystempath(path_id, external_name, external_path);
    path_nameconform(external_path, conform_path, PATH_STYLE_MAX, PATH_TYPE_BOOT);
    result = string_new(conform_path);
    if (result != NULL && subpath != NULL) {
        string_append(result, subpath);
    }
    return gensym(string_getptr(result));
}

t_symbol* ck_get_path_from_package(t_class* c, char* subpath)
{
    char filename[MAX_FILENAME_CHARS];
    char externals_dir[MAX_PATH_CHARS];
    char package_dir[MAX_PATH_CHARS];
    t_string* package_dir_s;

    t_symbol* external_path = ck_get_path_from_external(c, NULL);
    // const char* ext_path_c = string_getptr(external_path);
    path_splitnames(external_path->s_name, externals_dir, filename);
    path_splitnames(externals_dir, package_dir, filename);
    package_dir_s = string_new(package_dir);
    if (subpath != NULL) {
        string_append(package_dir_s, subpath);
    }
    return gensym(string_getptr(package_dir_s));
}


t_symbol* ck_check_file(t_ck* x, t_symbol* name)
{
    char filepath[MAX_PATH_CHARS];
    char normpath[MAX_PATH_CHARS];
    strncpy_zero(filepath, name->s_name, MAX_FILENAME_CHARS);

    // Validate path for security
    if (!is_safe_path(filepath)) {
        ck_error(x, (char*)"ck_check_file: unsafe path detected: %s", filepath);
        return gensym("");
    }

    path_nameconform(filepath, normpath, PATH_STYLE_MAX, PATH_TYPE_BOOT);


    // 1. check if file exists
    if (path_exists(normpath)) {
        return gensym(normpath);
    }

    // 2. check if exists with an `examples` folder prefix
    char eg_file[MAX_PATH_CHARS];
    snprintf_zero(eg_file, MAX_PATH_CHARS, "%s/%s", x->working_dir->s_name, filepath);
    ck_debug(x, "example file: %s", eg_file);
    if (path_exists(eg_file)) {
        return gensym(eg_file);
    }

    // 3. check if file exists in the patcher's directory
    char patcher_file[MAX_PATH_CHARS];
    snprintf_zero(patcher_file, MAX_PATH_CHARS, "%s/%s", x->patcher_dir->s_name, filepath);
    if(path_exists(patcher_file)) {
        // ck_debug(x, "patcher_file: %s", patcher_file);
        return gensym(patcher_file);
    }

    // 4. use locatefile_extended to search
    char abspath[MAX_PATH_CHARS];
    short path, res;
    t_fourcc outtype;
    t_fourcc filetypelist;
    t_max_err err;

    res = locatefile_extended(filepath, &path, &outtype, &filetypelist, 1);
    if (res != 0) {
        ck_error(x, (char*)"ck_check_file: locatefile_extended failed");
        return gensym("");
    }

    err = path_toabsolutesystempath(path, filepath, abspath);
    if (err != MAX_ERR_NONE) {
        ck_error(x, (char*)"ck_check_file: path_toabsolutesystempath failed");
        return gensym("");
    }

    normpath[0] = '\0'; // erase it to re-use it

    path_nameconform(abspath, normpath, PATH_STYLE_MAX, PATH_TYPE_BOOT);

    if (path_exists(normpath)) {
        return gensym(normpath);
    }

    ck_error(x, (char*)"ck_check_file: could not locate %s", name->s_name);
    return gensym("");
}

t_max_err ck_compile_code(t_ck* x, const char* code, const char* args)
{
    std::vector<t_CKUINT> vec;
    if (!x->chuck->compileCode(std::string(code), std::string(args), 1, FALSE, &vec)) {
        ck_error(x, (char*)"could not compile code: %s", code);
        return MAX_ERR_GENERIC;
    }
    return MAX_ERR_NONE;
}

t_max_err ck_compile_file(t_ck* x, const char* filename)
{
    if (x->chuck->compileFile(std::string(filename), "", 1)) {
        ck_info(x, (char*)"compiled: %s", filename);
        return MAX_ERR_NONE;
    } else {
        ck_error(x, (char*)"compilation error! : %s", filename);
        return MAX_ERR_GENERIC;
    }
}

t_max_err ck_run_file(t_ck* x)
{
    if (x->run_file != gensym("")) {
        return ck_compile_file(x, x->run_file->s_name);
    }
    ck_error(x, (char*)"ck_run_file: filename slot is empty");
    return MAX_ERR_GENERIC;
}

t_max_err ck_send_chuck_vm_msg(t_ck* x, Chuck_Msg_Type msg_type)
{
    Chuck_Msg* msg = NULL;

    msg = new Chuck_Msg;
    if (msg == NULL) {
        ck_error(x, (char*)"ck_send_chuck_vm_msg: could not create chuck msg");
        return MAX_ERR_GENERIC;
    }
    msg->type = msg_type;

    // null reply so that VM will delete for us when it's done
    msg->reply_cb = (ck_msg_func)NULL;

    if (x->chuck->vm()->globals_manager()->execute_chuck_msg_with_globals(msg)) {
        return MAX_ERR_NONE;
    } else {
        ck_error(x, (char*)"ck_send_chuck_vm_msg: could not send error msg");
        return MAX_ERR_GENERIC;
    }
}


//-----------------------------------------------------------------------------------------------
// message handlers


t_max_err ck_demo(t_ck* x, t_symbol* s, long argc, t_atom* argv)
{
    post("nothing here.. for now.");
    return MAX_ERR_NONE;
}


t_max_err ck_bang(t_ck* x)
{
    ck_run_file(x);
    return MAX_ERR_NONE;
}

t_max_err ck_run(t_ck* x, t_symbol* s)
{ 
    if (s != gensym("")) {
        if (x->run_needs_audio && !sys_getdspstate()) {
            ck_error(x, (char*)"can only run/add shred when audio is on");
            return MAX_ERR_GENERIC;
        }
        x->run_file = ck_check_file(x, s);
        return ck_run_file(x);
    }
    ck_error(x, (char*)"ck_run: reguires a filename to edit");
    return MAX_ERR_GENERIC;
}

t_max_err ck_edit(t_ck* x, t_symbol* s)
{
    if (x->editor == gensym("")) {
        ck_error(x, (char*)"ck_edit: editor attribute or EDITOR env var not set to full path of editor");
        return MAX_ERR_GENERIC;
    }

    // Validate editor path exists and is safe
    if (!path_exists(x->editor->s_name)) {
        ck_error(x, (char*)"ck_edit: editor path does not exist: %s", x->editor->s_name);
        return MAX_ERR_GENERIC;
    }

    if (s != gensym("")) {
        x->edit_file = ck_check_file(x, s);
        if (x->edit_file != gensym("")) {
            try {
                fs::path _file = std::string(x->edit_file->s_name);
                fs::path canonical_path = std::filesystem::canonical(_file);
                std::string file_path = canonical_path.make_preferred().string();

                // Additional validation of the canonical path
                if (!path_exists(file_path.c_str())) {
                    ck_error(x, (char*)"ck_edit: canonical file path does not exist");
                    return MAX_ERR_GENERIC;
                }

                ck_debug(x, (char*)"edit: %s", x->edit_file->s_name);

                // Build command safely with proper escaping
                std::string cmd = std::string(x->editor->s_name) + " \"" + file_path + "\"";
                ck_debug(x, (char*)"edit cmd: %s", cmd.c_str());

                // Execute command (still using system, but with validated inputs)
                int result = std::system(cmd.c_str());
                if (result != 0) {
                    ck_warn(x, (char*)"editor command returned non-zero exit code: %d", result);
                }
                return MAX_ERR_NONE;
            } catch (const std::exception& e) {
                ck_error(x, (char*)"ck_edit: filesystem error: %s", e.what());
                return MAX_ERR_GENERIC;
            }
        }
    }
    ck_error(x, (char*)"ck_edit: requires a valid filename");
    return MAX_ERR_GENERIC;
}

void ck_dblclick(t_ck* x)
{
    if (x->run_file != gensym("")) {
        ck_edit(x, x->run_file);
    }
}

t_max_err ck_add(t_ck* x, t_symbol* s, long argc, t_atom* argv)
{
    t_symbol *filename_sym = _sym_nothing;
    // std::regex volname_re("^([a-zA-Z0-9_ :]+)"); // to remove the unavoidable `Macintosh HD:` prefix
    std::regex volname_re("^(.+:)"); // to remove the unavoidable `Macintosh HD:` prefix

    if (argc < 1) {
        ck_error(x, (char*)"add message needs at least one <filename> argument");
        return MAX_ERR_GENERIC;
    }

    if ((argv)->a_type != A_SYM) {
        ck_error(x, (char*)"first argument must be a symbol");
        return MAX_ERR_GENERIC;
    }

    if (x->run_needs_audio && !sys_getdspstate()) {
        ck_error(x, (char*)"can only run/add shred when audio is on");
        return MAX_ERR_GENERIC;
    }

    t_symbol* dirty_sym = atom_getsym(argv);
    std::string cleaned = std::regex_replace(std::string(dirty_sym->s_name), volname_re, "");

    if (argc > 1) { // args provided
        // test if ':' is in the filename
        std::size_t found = cleaned.find(":");
        if (found != std::string::npos) {
            ck_error(x, (char*)"cannot use colon-separated args, use space-separated args instead");
            return MAX_ERR_GENERIC;
        }

        // convert the atom list to text
        char* text = ck_atom_gettext(argc, argv);
        replace_character(text, ' ', ':'); // convert space-separated args to colon-separated args
        filename_sym = gensym(text);
        sysmem_freeptr(text);
    } else {
        filename_sym = gensym(cleaned.c_str());
    }

    std::string path = std::string(filename_sym->s_name);
    std::string filename;
    std::string args;
    // extract args FILE:arg1:arg2:arg3
    extract_args( path, filename, args );
    
    t_symbol* checked_file = ck_check_file(x, gensym(filename.c_str()));

    if (checked_file == gensym("")) {
        ck_error(x, (char*)"could not add file");
        return MAX_ERR_GENERIC;
    }
    
    std::string full_path = std::string(checked_file->s_name);

    // compile but don't run yet (instance == 0)
    if( !x->chuck->compileFile( full_path, args, 0 ) ) {
        ck_error(x, (char*)"could not compile file");
        return MAX_ERR_GENERIC;
    }

    // construct chuck msg (must allocate on heap, as VM will clean up)
    Chuck_Msg * msg = new Chuck_Msg();
    msg->type = CK_MSG_ADD;
    msg->code = x->chuck->vm()->carrier()->compiler->output();
    msg->args = new vector<string>;
    extract_args( path, filename, *(msg->args) );
    x->current_shred_id = x->chuck->vm()->process_msg( msg );
    return MAX_ERR_NONE;    
}

t_max_err ck_eval(t_ck* x, t_symbol* s, long argc, t_atom* argv)
{
    if (argc == 0) {
        ck_error(x, (char*)"ck_eval: need at least one arg");
        return MAX_ERR_GENERIC;
    }

    if (argc == 1) {
        t_symbol* s = atom_getsym(argv);
        if (x->chuck->compileCode(std::string(s->s_name))) {
            ck_info(x, (char*)"ck_eval symbol compiled: success");
            return MAX_ERR_NONE;
        }
        return MAX_ERR_GENERIC;
    }

    char* text = ck_atom_gettext(argc, argv);

    if (text) {
        x->code = gensym(text);
        ck_info(x, (char*)"text: %s", text);
        ck_info(x, (char*)"code: %s", x->code->s_name);
        if (x->chuck->compileCode(std::string(text))) {
            ck_info(x, (char*)"ck_eval text compiled: success");            
        }        
        // t_CKBOOL compileCode( const std::string & code, const std::string & argsTogether = "",
        //                   t_CKUINT count = 1, t_CKBOOL immediate = FALSE, std::vector<t_CKUINT> * shredIDs = NULL );
        sysmem_freeptr(text);
        return MAX_ERR_NONE;
    }

    return MAX_ERR_GENERIC;
}

t_max_err ck_remove(t_ck* x, t_symbol* s, long argc, t_atom* argv)
{
    Chuck_Msg* msg = new Chuck_Msg;

    if (argc == 1) {

        if (argv->a_type == A_LONG) {
            long shred_id = atom_getlong(argv);
            msg->type = CK_MSG_REMOVE;
            msg->param = shred_id;

        } else if (argv->a_type == A_SYM) {

            t_symbol* cmd = atom_getsym(argv);

            if (cmd == gensym("all")) {
                msg->type = CK_MSG_REMOVEALL;

            } else if (cmd == gensym("last")) {
                msg->type = CK_MSG_REMOVE;
                msg->param = 0xffffffff;
            }
        }

        // handle one arg case
        msg->reply_cb = (ck_msg_func)0;
        x->chuck->vm()->queue_msg(msg, 1);
        return MAX_ERR_NONE;

    } else {
        // assume message is along :-) the lines of (remove 2 4 1 [..])
        t_atom_long* long_array = (t_atom_long*)sysmem_newptr(sizeof(t_atom_long) * argc);
        t_max_err err = atom_getlong_array(argc, argv, argc, long_array);
        if (err != MAX_ERR_NONE) {
            ck_error(x, (char*)"remove msg: multiple args can only be ints");
            return err;
        }
        for (int i = 0; i < argc; i++) {
            // ck_info(x, (char*)"removing: long_array[%d] = %d", i, long_array[i]);
            Chuck_Msg* m = new Chuck_Msg;
            m->type = CK_MSG_REMOVE;
            m->param = long_array[i]; // shred id
            m->reply_cb = (ck_msg_func)0;
            x->chuck->vm()->queue_msg(m, 1);
        }
        sysmem_freeptr(long_array);
    }
    return MAX_ERR_NONE;
}

t_max_err ck_removeall(t_ck* x)
{
    // NOTE: this removes shreds only. global UGens belong to the VM rather than
    // to any shred, so a 'global SinOsc g => dac' keeps sounding after every
    // shred is gone -- which reads as "removeall did nothing" when the patch
    // gets its sound from globals. 'reset' (CK_MSG_CLEARVM) clears the type
    // system and globals too, which is why that one goes silent.
    std::vector<t_CKUINT> shred_ids;
    x->chuck->vm()->shreduler()->get_all_shred_ids(shred_ids);

    Chuck_Msg* msg = new Chuck_Msg;
    msg->type = CK_MSG_REMOVEALL;
    msg->reply_cb = (ck_msg_func)0;
    x->chuck->vm()->queue_msg(msg, 1);

    object_post((t_object*)x,
                "removeall: removing %ld shred(s); global UGens are VM state "
                "and keep running -- use 'reset' to clear those too",
                (long)shred_ids.size());
    return MAX_ERR_NONE;
}

t_max_err ck_abort(t_ck* x)
{
    // abort the shred currently executing in the VM. unlike 'remove', this can
    // break out of a shred stuck in a loop that never advances time.
    //
    // NOTE: this only has a target while the VM is inside a compute() cycle,
    // because Chuck_VM::abort_current_shred() reads m_shreduler->m_current_shred
    // and that is only non-NULL during compute. sending 'abort' from a message
    // box on an otherwise healthy patch therefore finds nothing to abort, and
    // correctly reports so. it bites precisely when it is needed: a runaway
    // shred leaves the audio thread stuck inside compute(), and the abort
    // arriving from the main thread then does have a current shred to flag.
    //
    // reported with object_post/object_warn rather than ck_info/ck_warn, which
    // are gated on loglevel >= 5 / >= 4 and so are silent at the default
    // loglevel of CK_LOG_SYSTEM (2). a user-invoked command must always answer.
    if (x->chuck == NULL || x->chuck->vm() == NULL) {
        ck_error(x, (char*)"abort: vm not available");
        return MAX_ERR_GENERIC;
    }
    if (x->chuck->vm()->abort_current_shred()) {
        object_post((t_object*)x, "abort: aborted the running shred");
        return MAX_ERR_NONE;
    }
    object_warn((t_object*)x,
                "abort: no shred is currently executing; abort only takes "
                "effect on a shred that is stuck inside the VM");
    return MAX_ERR_NONE;
}

t_max_err ck_replace(t_ck* x, t_symbol* s, long argc, t_atom* argv)
{
    long shred_id;
    t_symbol *filename_sym = _sym_nothing;

    if (argc < 2) {
        ck_error(x, (char*)"replace message needs at least two arguments");
        return MAX_ERR_GENERIC;
    }
    if (argv->a_type != A_LONG) {
        ck_error(x, (char*)"first argument must a long");
        return MAX_ERR_GENERIC;
    }
    shred_id = atom_getlong(argv);

    if ((argv+1)->a_type != A_SYM) {
        ck_error(x, (char*)"second argument must be a symbol");
        return MAX_ERR_GENERIC;
    }
    filename_sym = atom_getsym(argv+1);

    // get string
    std::string path = std::string(filename_sym->s_name);
    // filename
    std::string filename;
    // arguments
    std::string args;
    // extract args FILE:arg1:arg2:arg3
    extract_args( path, filename, args );

    t_symbol* checked_file = ck_check_file(x, gensym(filename.c_str()));

    if (checked_file == gensym("")) {
        ck_error(x, (char*)"could not replace file");
        return MAX_ERR_GENERIC;
    }
    
    std::string full_path = std::string(checked_file->s_name);

    // compile but don't run yet (instance == 0)
    if( !x->chuck->compileFile( full_path, args, 0 ) ) {
        ck_error(x, (char*)"could not compile file");
        return MAX_ERR_GENERIC;
    }

    // construct chuck msg (must allocate on heap, as VM will clean up)
    Chuck_Msg * msg = new Chuck_Msg();
    // set type
    msg->type = CK_MSG_REPLACE;
    // set shred id to replace
    msg->param = shred_id;
    // set code for incoming shred
    msg->code = x->chuck->vm()->carrier()->compiler->output();
    // create args array
    msg->args = new vector<string>;
    // extract args again but this time into vector
    extract_args( path, filename, *(msg->args) );
    // process REPLACE message, return new shred ID
    x->current_shred_id = x->chuck->vm()->process_msg( msg );
    return MAX_ERR_NONE;
}

t_max_err ck_clear(t_ck* x, t_symbol* s, long argc, t_atom* argv)
{
    if (argc == 0) {
        return ck_send_chuck_vm_msg(x, CK_MSG_CLEARVM);
    }

    if (argc == 1) {
        if (argv->a_type == A_SYM) {
            t_symbol* target = atom_getsym(argv);
            if (target == gensym("globals")) {
                ck_info(x, (char*)"[chuck]: clean up global variables without clearing the whole VM");
                return ck_send_chuck_vm_msg(x, CK_MSG_CLEARGLOBALS);
            } 
            if (target == gensym("vm")) {
                return ck_send_chuck_vm_msg(x, CK_MSG_CLEARVM); 
            }
            if (target == gensym("console")) {
                return ck_send_max_msg(x, gensym("clearmaxwindow"),""); 
            }
        }
    }
    ck_error(x, (char*)"must be 'clear globals' or 'clear vm'");
    return MAX_ERR_GENERIC;
}

t_max_err ck_reset(t_ck* x, t_symbol* s, long argc, t_atom* argv)
{
    if (argc == 0) {
        return ck_send_chuck_vm_msg(x, CK_MSG_CLEARVM);
    }
    
    if (argc == 1) {
        if (argv->a_type == A_SYM) {
            t_symbol* target = atom_getsym(argv);
            if (target == gensym("id")) {
                return ck_send_chuck_vm_msg(x, CK_MSG_RESET_ID);
            } 
        }
    }
    ck_error(x, (char*)"must be 'reset id' or just 'reset' for clearvm");
    return MAX_ERR_GENERIC;
}

t_max_err ck_status(t_ck* x)
{
    Chuck_VM_Shreduler* shreduler = x->chuck->vm()->shreduler();
    shreduler->status();

    if (1) {
        std::vector<Chuck_VM_Shred*> shreds;
        shreduler->get_all_shreds(shreds);
        for (const Chuck_VM_Shred* i : shreds) {
            object_post((t_object*)x, "%lu:%s", (unsigned long)i->get_id(), i->name.c_str());
        }
    }

    return MAX_ERR_NONE;
}


t_max_err ck_time(t_ck* x)
{
    return ck_send_chuck_vm_msg(x, CK_MSG_TIME);
}


t_symbol* ck_get_loglevel_name(long level)
{
    t_symbol* name = _sym_nothing;

    switch (level) {
        case CK_LOG_NONE:
            name = gensym("CK_LOG_NONE");
            break;
        case CK_LOG_CORE:
            name = gensym("CK_LOG_CORE");
            break;
        case CK_LOG_SYSTEM:
            name = gensym("CK_LOG_SYSTEM");
            break;
        case CK_LOG_HERALD:
            name = gensym("CK_LOG_HERALD");
            break;
        case CK_LOG_WARNING:
            name = gensym("CK_LOG_WARNING");
            break;
        case CK_LOG_INFO:
            name = gensym("CK_LOG_INFO");
            break;
        case CK_LOG_DEBUG:
            name = gensym("CK_LOG_DEBUG");
            break;
        case CK_LOG_FINE:
            name = gensym("CK_LOG_NONE");
            break;
        case CK_LOG_FINER:
            name = gensym("CK_LOG_FINER");
            break;
        case CK_LOG_FINEST:
            name = gensym("CK_LOG_FINEST");
            break;
        case CK_LOG_ALL:
            name = gensym("CK_LOG_ALL");
            break;
        default:
            name = gensym("CK_LOG_SYSTEM");
    }
    return name;
}

t_max_err ck_loglevel(t_ck* x, t_symbol* s, long argc, t_atom* argv)
{
    // loglevel controls the ChucK VM's own logging, which is process-global:
    // ChucK::setLogLevel/getLogLevel are static, so there is one level shared by
    // every chuck~ in the process. This reads and writes that shared state
    // directly rather than caching a per-instance copy, which previously made a
    // query on one object silently overwrite a stale field and read as if the
    // level were per-object. For per-object reporting, use 'verbose' instead.
    if (argc == 0) {
        long level = (long)ChucK::getLogLevel();
        t_symbol* name = ck_get_loglevel_name(level);
        post("loglevel %ld (%s), shared by all chuck~ in this process",
             level, name->s_name);
        return MAX_ERR_NONE;
    }
    if (argc == 1 && argv->a_type == A_LONG) {
        long level = atom_getlong(argv);
        if ((level >= 0) && (level <= 10)) {
            t_symbol* name = ck_get_loglevel_name(level);
            ChucK::setLogLevel(level);
            object_post((t_object*)x, "loglevel %ld (%s), applies to all "
                        "chuck~ in this process", level, name->s_name);
            return MAX_ERR_NONE;
        }
        ck_error(x, (char*)"loglevel out of range: must be 0-10 inclusive");
        return MAX_ERR_GENERIC;
    }
    ck_error(x, (char*)"could not get or set loglevel");
    return MAX_ERR_GENERIC;
}

t_max_err ck_anything(t_ck* x, t_symbol* s, long argc, t_atom* argv)
{
    t_atom atoms[128];

    // TODO:
    //  - should check set op (true if succeed)
    //  - handle case of 2 length array (maybe)

    if (s == gensym("") || argc == 0) {
        goto error;
    }

    // set '+' as shorthand for ck_add method
    if (s == gensym("+")) {
        ck_add(x, gensym(""), argc, argv);
        return MAX_ERR_NONE;
    }

    // set '-' as shorthand for ck_remove method
    if (s == gensym("-")) {
        ck_remove(x, gensym(""), argc, argv);
        return MAX_ERR_NONE;
    }

    // FIXME: doesn't work in Max (and pd)
    // set '--' as shorthand for ck_remove (last) method
    if (s == gensym("--")) {
        atom_setsym(atoms, gensym("last"));
        ck_remove(x, gensym(""), 1, atoms);
        return MAX_ERR_NONE;
    }

    // set '=' as shorthand for ck_replace method
    if (s == gensym("=")) {
        ck_replace(x, gensym(""), argc, argv);
        return MAX_ERR_NONE;
    }

    // FIXME: doesn't work in Max (and pd)
    // set '^' as shorthand for ck_status method
    if (s == gensym("^")) {
        ck_status(x);
        return MAX_ERR_NONE;
    }

    if (argc == 1) {            // <param-name> <value>
        switch (argv->a_type) { // really argv[0]
        case A_FLOAT: {
            float p_float = atom_getfloat(argv);
            ck_debug(x, (char*)"param %s: %f", s->s_name, p_float);            
            x->chuck->vm()->globals_manager()->setGlobalFloat(s->s_name, p_float);
            break;
        }
        case A_LONG: {
            long p_long = atom_getlong(argv);
            ck_debug(x, "param %s: %ld", s->s_name, (long)p_long);
            x->chuck->vm()->globals_manager()->setGlobalInt(s->s_name, p_long);
            break;
        }
        case A_SYM: {
            t_symbol* p_sym = atom_getsym(argv);
            if (p_sym == NULL) {
                goto error;
            }
            ck_debug(x, (char*)"param %s: %s", s->s_name, p_sym->s_name);
            x->chuck->vm()->globals_manager()->setGlobalString(s->s_name, p_sym->s_name);
            break;
        }
        default:
            goto error;
            break;
        }

    } else { // type is a list

        if (argv->a_type == A_LONG) { // list of longs
            t_atom_long* long_array = (t_atom_long*)sysmem_newptr(sizeof(t_atom_long) * argc);
            for (int i = 0; i < argc; i++) {
                // ck_info(x, (char*)"i: %d -> %d ", i, atom_getlong(argv + i));
                long_array[i] = atom_getlong(argv + i);
            }
            x->chuck->vm()->globals_manager()->setGlobalIntArray(
                s->s_name, long_array, argc);
            sysmem_freeptr(long_array);
        }

        else if (argv->a_type == A_FLOAT) { // list of doubles
            double* float_array = (double*)sysmem_newptr(sizeof(double*) * argc);
            for (int i = 0; i < argc; i++) {
                // ck_info(x, (char*)"i: %d -> %d ", i, atom_getfloat(argv + i));
                float_array[i] = atom_getfloat(argv + i);
            }
            x->chuck->vm()->globals_manager()->setGlobalFloatArray(
                s->s_name, float_array, argc);
            sysmem_freeptr(float_array);
        }
    }

    if (argc == 2) {                  // <param-name> <index|key> <value
        if (argv->a_type == A_LONG) { // int index
            long index = atom_getlong(argv);

            switch ((argv + 1)->a_type) { // really argv[1]
            case A_FLOAT: {
                float p_float = atom_getfloat(argv + 1);
                x->chuck->vm()->globals_manager()->setGlobalFloatArrayValue(
                    s->s_name, index, p_float);
                break;
            }
            case A_LONG: {
                long p_long = atom_getlong(argv + 1);
                x->chuck->vm()->globals_manager()->setGlobalIntArrayValue(
                    s->s_name, index, p_long);
                break;
            }
            default:
                goto error;
                break;
            }

        } else if (argv->a_type == A_SYM) { // key/value
            t_symbol* key = atom_getsym(argv);

            switch ((argv + 1)->a_type) { // really argv[1]
            case A_FLOAT: {
                float p_float = atom_getfloat(argv + 1);
                x->chuck->vm()
                    ->globals_manager()
                    ->setGlobalAssociativeFloatArrayValue(
                        s->s_name, key->s_name, p_float);
                break;
            }
            case A_LONG: {
                long p_long = atom_getlong(argv + 1);
                x->chuck->vm()
                    ->globals_manager()
                    ->setGlobalAssociativeIntArrayValue(s->s_name, key->s_name, p_long);
                break;
            }
            default:
                goto error;
                break;
            }
        }
    }

    return MAX_ERR_NONE;

error:
    ck_error(x, (char*)"[ck_anything] cannot set chuck global param");
    return MAX_ERR_GENERIC;
}

t_max_err ck_signal(t_ck* x, t_symbol* s)
{
    ck_debug(x, (char*)"signal: %s", s->s_name);
    if (x->chuck->vm()->globals_manager()->signalGlobalEvent(s->s_name)) {
        return MAX_ERR_NONE;
    } else {
        ck_error(x, (char*)"[ck_signal] signal global event '%s' failed", s->s_name);
        return MAX_ERR_GENERIC;
    }
}

t_max_err ck_broadcast(t_ck* x, t_symbol* s)
{
    ck_debug(x, (char*)"broadcast: %s", s->s_name);
    if (x->chuck->vm()->globals_manager()->broadcastGlobalEvent(s->s_name)) {
        return MAX_ERR_NONE;
    } else {
        ck_error(x, (char*)"[ck_broadcast] broadcast global event '%s' failed", s->s_name);
        return MAX_ERR_GENERIC;
    }
}

t_max_err ck_chugins(t_ck* x)
{
    object_post((t_object*)x, "probe chugins:");
    x->chuck->probeChugins();
    return MAX_ERR_NONE;
}

t_max_err ck_docs(t_ck* x)
{
    ck_send_max_msg(x, gensym("launchbrowser"), "https://chuck.stanford.edu/doc");
    return MAX_ERR_NONE;
}

t_max_err ck_globals(t_ck* x)
{
    if (x->chuck->vm()->globals_manager()->getAllGlobalVariables(cb_get_all_global_vars, x)) {
        return MAX_ERR_NONE;
    }
    ck_error(x, (char*)"could not dump global variable to console");
    return MAX_ERR_GENERIC;
}

t_max_err ck_vm(t_ck* x)
{
    object_post((t_object*)x, "VM %d / %d status", x->oid, CK_INSTANCE_COUNT);
    object_post((t_object*)x, "\tinitialized: %lu", (unsigned long)x->chuck->vm()->has_init());
    object_post((t_object*)x, "\trunning: %lu", (unsigned long)x->chuck->vm()->running());
    return MAX_ERR_NONE;
}

// clear a single tap slot back to "unassigned, mono"
static void ck_tap_clear_slot(t_ck* x, long i)
{
    x->tap_ugens[i] = gensym("");
    x->tap_ugen_nchans[i] = 0;
    x->tap_ugen_chan[i] = 0;
}

// drop any multichannel group that is no longer internally consistent. assigning
// a group over part of an existing one can otherwise strand the leftover members,
// which the perform loop would skip forever and leave holding stale audio
static void ck_tap_normalize(t_ck* x)
{
    for (long i = 0; i < x->tap_channels; i++) {
        long nchans = x->tap_ugen_nchans[i];
        if (nchans <= 1) {
            continue;
        }
        long base = i - x->tap_ugen_chan[i];
        bool ok = (base >= 0) && (base + nchans <= x->tap_channels);
        for (long c = 0; ok && c < nchans; c++) {
            long j = base + c;
            ok = (x->tap_ugens[j] == x->tap_ugens[i])
                 && (x->tap_ugen_nchans[j] == nchans)
                 && (x->tap_ugen_chan[j] == c);
        }
        if (!ok) {
            ck_tap_clear_slot(x, i);
        }
    }
}

t_max_err ck_tap(t_ck* x, t_symbol* s, long argc, t_atom* argv)
{
    if (x->tap_channels == 0) {
        ck_error(x, (char*)"tap: no tap outlets configured (use @ntap attribute)");
        return MAX_ERR_GENERIC;
    }

    if (argc == 0) {
        // tap (no args): clear all taps
        for (int i = 0; i < x->tap_channels; i++) {
            ck_tap_clear_slot(x, i);
        }
        ck_info(x, (char*)"tap: cleared all");
    }
    else if (argc == 1) {
        if (argv[0].a_type == A_SYM) {
            // tap ugen_name: set all outlets to tap the same UGen
            t_symbol* ugen_name = atom_getsym(&argv[0]);
            for (int i = 0; i < x->tap_channels; i++) {
                ck_tap_clear_slot(x, i);
                x->tap_ugens[i] = ugen_name;
            }
            ck_info(x, (char*)"tap: all outlets set to '%s'", ugen_name->s_name);
        }
        else if (argv[0].a_type == A_LONG) {
            // tap outlet_index: clear specific outlet
            long outlet = atom_getlong(&argv[0]);
            if (outlet < 1 || outlet > x->tap_channels) {
                ck_error(x, (char*)"tap: outlet %ld out of range (1-%ld)", outlet, x->tap_channels);
                return MAX_ERR_GENERIC;
            }
            ck_tap_clear_slot(x, outlet - 1);
            ck_info(x, (char*)"tap: outlet %ld cleared", outlet);
        }
        else {
            ck_error(x, (char*)"tap: invalid argument type");
            return MAX_ERR_GENERIC;
        }
    }
    else if (argc == 2 || argc == 3) {
        // tap outlet_index ugen_name [nchannels]: set specific outlet, where a
        // multichannel UGen spans nchannels consecutive outlets
        if (argv[0].a_type != A_LONG) {
            ck_error(x, (char*)"tap: first argument must be outlet number (1-%ld)", x->tap_channels);
            return MAX_ERR_GENERIC;
        }
        long outlet = atom_getlong(&argv[0]);
        if (outlet < 1 || outlet > x->tap_channels) {
            ck_error(x, (char*)"tap: outlet %ld out of range (1-%ld)", outlet, x->tap_channels);
            return MAX_ERR_GENERIC;
        }
        if (argv[1].a_type != A_SYM) {
            ck_error(x, (char*)"tap: second argument must be UGen name");
            return MAX_ERR_GENERIC;
        }
        t_symbol* ugen_name = atom_getsym(&argv[1]);

        long nchans = 1;
        if (argc == 3) {
            if (argv[2].a_type != A_LONG) {
                ck_error(x, (char*)"tap: third argument must be a channel count");
                return MAX_ERR_GENERIC;
            }
            nchans = atom_getlong(&argv[2]);
            if (nchans < 1) {
                ck_error(x, (char*)"tap: channel count must be at least 1");
                return MAX_ERR_GENERIC;
            }
            if (outlet - 1 + nchans > x->tap_channels) {
                ck_error(x, (char*)"tap: %ld channels from outlet %ld exceeds %ld tap outlets",
                         nchans, outlet, x->tap_channels);
                return MAX_ERR_GENERIC;
            }
        }

        if (nchans > 1) {
            for (long c = 0; c < nchans; c++) {
                long j = outlet - 1 + c;
                x->tap_ugens[j] = ugen_name;
                x->tap_ugen_nchans[j] = nchans;
                x->tap_ugen_chan[j] = c;
            }
            ck_info(x, (char*)"tap: outlets %ld-%ld set to '%s' (%ld channels)",
                    outlet, outlet + nchans - 1, ugen_name->s_name, nchans);
        } else {
            ck_tap_clear_slot(x, outlet - 1);
            x->tap_ugens[outlet - 1] = ugen_name;
            ck_info(x, (char*)"tap: outlet %ld set to '%s'", outlet, ugen_name->s_name);
        }
    }
    else {
        ck_error(x, (char*)"tap: too many arguments");
        return MAX_ERR_GENERIC;
    }

    ck_tap_normalize(x);
    return MAX_ERR_NONE;
}

t_max_err ck_param(t_ck* x, t_symbol* s, long argc, t_atom* argv)
{
    // list of known int params
    const char* int_params[] = {
        CHUCK_PARAM_SAMPLE_RATE,
        CHUCK_PARAM_INPUT_CHANNELS,
        CHUCK_PARAM_OUTPUT_CHANNELS,
        CHUCK_PARAM_VM_ADAPTIVE,
        CHUCK_PARAM_VM_HALT,
        CHUCK_PARAM_OTF_ENABLE,
        CHUCK_PARAM_OTF_PORT,
        CHUCK_PARAM_OTF_PRINT_WARNINGS,
        CHUCK_PARAM_DUMP_INSTRUCTIONS,
        CHUCK_PARAM_AUTO_DEPEND,
        CHUCK_PARAM_DEPRECATE_LEVEL,
        CHUCK_PARAM_CHUGIN_ENABLE,
        CHUCK_PARAM_IS_REALTIME_AUDIO_HINT,
        CHUCK_PARAM_COMPILER_HIGHLIGHT_ON_ERROR,
        CHUCK_PARAM_TTY_COLOR,
        CHUCK_PARAM_TTY_WIDTH_HINT,
        NULL
    };

    // list of known string params
    const char* string_params[] = {
        CHUCK_PARAM_VERSION,
        CHUCK_PARAM_WORKING_DIRECTORY,
        NULL
    };

    // list of known string list params
    const char* string_list_params[] = {
        CHUCK_PARAM_USER_CHUGINS,
        CHUCK_PARAM_IMPORT_PATH_SYSTEM,
        CHUCK_PARAM_IMPORT_PATH_PACKAGES,
        CHUCK_PARAM_IMPORT_PATH_USER,
        NULL
    };

    // no args: list all available params
    if (argc == 0) {
        post("ChucK VM Parameters:");
        post("  Integer parameters:");
        for (int i = 0; int_params[i] != NULL; i++) {
            t_CKINT val = x->chuck->getParamInt(int_params[i]);
            post("    %s = %lld", int_params[i], (long long)val);
        }
        post("  String parameters:");
        for (int i = 0; string_params[i] != NULL; i++) {
            std::string val = x->chuck->getParamString(string_params[i]);
            post("    %s = %s", string_params[i], val.c_str());
        }
        post("  String list parameters:");
        for (int i = 0; string_list_params[i] != NULL; i++) {
            std::list<std::string> val = x->chuck->getParamStringList(string_list_params[i]);
            post("    %s (%d items):", string_list_params[i], (int)val.size());
            for (const auto& item : val) {
                post("      - %s", item.c_str());
            }
        }
        return MAX_ERR_NONE;
    }

    // get param name
    if (argv->a_type != A_SYM) {
        ck_error(x, (char*)"param: first argument must be parameter name");
        return MAX_ERR_GENERIC;
    }
    t_symbol* param_name = atom_getsym(argv);
    std::string name = std::string(param_name->s_name);

    // helper lambdas to check param type
    auto is_int_param = [&]() {
        for (int i = 0; int_params[i] != NULL; i++) {
            if (name == int_params[i]) return true;
        }
        return false;
    };
    auto is_string_param = [&]() {
        for (int i = 0; string_params[i] != NULL; i++) {
            if (name == string_params[i]) return true;
        }
        return false;
    };
    auto is_string_list_param = [&]() {
        for (int i = 0; string_list_params[i] != NULL; i++) {
            if (name == string_list_params[i]) return true;
        }
        return false;
    };

    // one arg: get param value
    if (argc == 1) {
        if (is_int_param()) {
            t_CKINT val = x->chuck->getParamInt(name);
            post("param %s = %lld", name.c_str(), (long long)val);
        } else if (is_string_param()) {
            std::string val = x->chuck->getParamString(name);
            post("param %s = %s", name.c_str(), val.c_str());
        } else if (is_string_list_param()) {
            std::list<std::string> val = x->chuck->getParamStringList(name);
            post("param %s (%d items):", name.c_str(), (int)val.size());
            for (const auto& item : val) {
                post("  - %s", item.c_str());
            }
        } else {
            ck_error(x, (char*)"param: unknown parameter '%s'", name.c_str());
            return MAX_ERR_GENERIC;
        }
        return MAX_ERR_NONE;
    }

    // two+ args: set param value
    if (argc >= 2) {
        if (is_int_param()) {
            if ((argv + 1)->a_type != A_LONG) {
                ck_error(x, (char*)"param: %s requires an integer value", name.c_str());
                return MAX_ERR_GENERIC;
            }
            t_CKINT val = (t_CKINT)atom_getlong(argv + 1);
            x->chuck->setParam(name, val);
            post("param %s set to %lld", name.c_str(), (long long)val);
        } else if (is_string_param()) {
            if ((argv + 1)->a_type != A_SYM) {
                ck_error(x, (char*)"param: %s requires a string value", name.c_str());
                return MAX_ERR_GENERIC;
            }
            std::string val = std::string(atom_getsym(argv + 1)->s_name);
            x->chuck->setParam(name, val);
            post("param %s set to %s", name.c_str(), val.c_str());
        } else if (is_string_list_param()) {
            // build list from remaining args
            std::list<std::string> val;
            for (int i = 1; i < argc; i++) {
                if ((argv + i)->a_type == A_SYM) {
                    val.push_back(std::string(atom_getsym(argv + i)->s_name));
                }
            }
            x->chuck->setParam(name, val);
            post("param %s set to %d items", name.c_str(), (int)val.size());
        } else {
            ck_error(x, (char*)"param: unknown parameter '%s'", name.c_str());
            return MAX_ERR_GENERIC;
        }
        return MAX_ERR_NONE;
    }

    return MAX_ERR_GENERIC;
}

t_max_err ck_shreds(t_ck* x, t_symbol* s, long argc, t_atom* argv)
{
    Chuck_VM_Shreduler* shreduler = x->chuck->vm()->shreduler();

    // no args: list all shreds
    if (argc == 0) {
        std::vector<Chuck_VM_Shred*> shreds;
        shreduler->get_all_shreds(shreds);

        if (shreds.empty()) {
            post("shreds: no shreds running");
        } else {
            post("shreds: %d running", (int)shreds.size());
            for (const Chuck_VM_Shred* shred : shreds) {
                const char* state = shred->is_running ? "running" :
                                   (shred->event ? "blocked" : "ready");
                post("  [%d] %s (%s)", shred->xid, shred->name.c_str(), state);
            }
        }
        return MAX_ERR_NONE;
    }

    // handle subcommands
    if (argv->a_type == A_SYM) {
        t_symbol* cmd = atom_getsym(argv);

        if (cmd == gensym("all")) {
            std::vector<t_CKUINT> ids = ck_get_all_shred_ids(x);
            post("shreds all: %d shreds", (int)ids.size());
            for (t_CKUINT id : ids) {
                Chuck_VM_Shred* shred = shreduler->lookup(id);
                if (shred) {
                    post("  [%d] %s", shred->xid, shred->name.c_str());
                }
            }
            return MAX_ERR_NONE;
        }

        if (cmd == gensym("ready")) {
            std::vector<t_CKUINT> ids = ck_get_ready_shred_ids(x);
            post("shreds ready: %d shreds", (int)ids.size());
            for (t_CKUINT id : ids) {
                Chuck_VM_Shred* shred = shreduler->lookup(id);
                if (shred) {
                    post("  [%d] %s", shred->xid, shred->name.c_str());
                }
            }
            return MAX_ERR_NONE;
        }

        if (cmd == gensym("blocked")) {
            std::vector<t_CKUINT> ids = ck_get_blocked_shred_ids(x);
            post("shreds blocked: %d shreds", (int)ids.size());
            for (t_CKUINT id : ids) {
                Chuck_VM_Shred* shred = shreduler->lookup(id);
                if (shred) {
                    post("  [%d] %s", shred->xid, shred->name.c_str());
                }
            }
            return MAX_ERR_NONE;
        }

        if (cmd == gensym("highest")) {
            long id = ck_spork_highest_id(x);
            post("shreds highest: %d", id);
            return MAX_ERR_NONE;
        }

        if (cmd == gensym("last")) {
            long id = x->chuck->vm()->last_id();
            post("shreds last: %d", id);
            return MAX_ERR_NONE;
        }

        if (cmd == gensym("next")) {
            long id = x->chuck->vm()->next_id();
            post("shreds next: %d", id);
            return MAX_ERR_NONE;
        }

        if (cmd == gensym("count")) {
            std::vector<t_CKUINT> ids = ck_get_all_shred_ids(x);
            post("shreds count: %d", (int)ids.size());
            return MAX_ERR_NONE;
        }

        ck_error(x, (char*)"shreds: unknown subcommand '%s'", cmd->s_name);
        return MAX_ERR_GENERIC;
    }

    // numeric arg: get info about specific shred
    if (argv->a_type == A_LONG) {
        t_CKUINT id = (t_CKUINT)atom_getlong(argv);
        Chuck_VM_Shred* shred = shreduler->lookup(id);

        if (!shred) {
            ck_error(x, (char*)"shreds: shred %d not found", (int)id);
            return MAX_ERR_GENERIC;
        }

        post("shred [%d]:", shred->xid);
        post("  name: %s", shred->name.c_str());
        post("  running: %s", shred->is_running ? "yes" : "no");
        post("  done: %s", shred->is_done ? "yes" : "no");
        post("  blocked: %s", shred->event ? "yes (waiting on event)" : "no");
        post("  wake_time: %.2f samples", shred->wake_time);
        post("  start: %.2f samples", shred->start);

        if (!shred->args.empty()) {
            post("  args:");
            for (const auto& arg : shred->args) {
                post("    - %s", arg.c_str());
            }
        }
        return MAX_ERR_NONE;
    }

    ck_error(x, (char*)"shreds: invalid argument");
    return MAX_ERR_GENERIC;
}

t_max_err ck_adaptive(t_ck* x, t_symbol* s, long argc, t_atom* argv)
{
    Chuck_VM_Shreduler* shreduler = x->chuck->vm()->shreduler();

    // no args: get current adaptive mode status
    if (argc == 0) {
        t_CKBOOL adaptive = shreduler->m_adaptive;
        t_CKUINT max_block = shreduler->m_max_block_size;

        if (adaptive) {
            post("adaptive: ON (max block size: %d samples)", (int)max_block);
        } else {
            post("adaptive: OFF");
        }
        return MAX_ERR_NONE;
    }

    // one arg: set adaptive mode
    if (argc == 1 && argv->a_type == A_LONG) {
        t_CKUINT size = (t_CKUINT)atom_getlong(argv);
        shreduler->set_adaptive(size);

        if (size > 1) {
            post("adaptive: enabled with max block size %d samples", (int)size);
        } else {
            post("adaptive: disabled");
        }
        return MAX_ERR_NONE;
    }

    ck_error(x, (char*)"adaptive: expected no args (get) or integer (set)");
    return MAX_ERR_GENERIC;
}

//-----------------------------------------------------------------------------------------------
// reply plumbing

// resolve the instance that issued a globals request from its callback id.
// returns NULL if the object was freed while the request was in flight
t_ck* ck_instance_from_id(t_CKINT id)
{
    long slot = CK_ID_SLOT(id);
    if (slot < 0 || slot >= CK_MAX_INSTANCES) {
        return NULL;
    }
    return CK_INSTANCE_SLOTS[slot].load(std::memory_order_acquire);
}

// remember the variable name for an outgoing request and return the id to hand
// to chuck. the ticket ring is only consulted when the reply arrives, which is
// within an audio block or two, so wraparound is not a practical concern
t_CKINT ck_pending_issue(t_ck* x, t_symbol* name)
{
    long ticket = x->pending_ticket++;
    x->pending_names[ticket % CK_PENDING_SIZE] = name;
    return CK_ID_PACK(x->slot, ticket);
}

// recover the variable name a reply belongs to
t_symbol* ck_pending_name(t_ck* x, t_CKINT id)
{
    long ticket = CK_ID_TICKET(id);
    t_symbol* name = x->pending_names[ticket % CK_PENDING_SIZE];
    return name ? name : gensym("?");
}

// queue a reply from the audio thread. drops the reply rather than block if the
// queue is full, which is the right trade in a realtime context
void ck_reply_push(t_ck* x, t_symbol* selector, long argc, t_atom* argv)
{
    if (x == NULL || x->reply_outlet == NULL) {
        return;
    }
    long tail = x->reply_tail.load(std::memory_order_relaxed);
    long next = (tail + 1) % CK_REPLY_QUEUE_SIZE;
    if (next == x->reply_head.load(std::memory_order_acquire)) {
        return; // full
    }
    t_ck_reply* r = &x->reply_queue[tail];
    r->selector = selector;
    r->argc = (argc > CK_REPLY_MAX_ATOMS) ? CK_REPLY_MAX_ATOMS : argc;
    for (long i = 0; i < r->argc; i++) {
        r->argv[i] = argv[i];
    }
    x->reply_tail.store(next, std::memory_order_release);
    qelem_set(x->reply_qelem);
}

// drain queued replies out the reply outlet; runs on the main thread via qelem
void ck_reply_drain(t_ck* x)
{
    long head = x->reply_head.load(std::memory_order_relaxed);
    while (head != x->reply_tail.load(std::memory_order_acquire)) {
        t_ck_reply* r = &x->reply_queue[head];
        outlet_anything(x->reply_outlet, r->selector, (short)r->argc, r->argv);
        head = (head + 1) % CK_REPLY_QUEUE_SIZE;
        x->reply_head.store(head, std::memory_order_release);
    }
}

//-----------------------------------------------------------------------------------------------
// global event callback

void cb_event(t_CKINT id)
{
    t_ck* x = ck_instance_from_id(id);
    if (x == NULL) {
        return;
    }
    long ticket = CK_ID_TICKET(id);
    if (ticket < 0 || ticket >= CK_MAX_LISTENS) {
        return;
    }
    t_symbol* name = x->listen_names[ticket];
    if (name == NULL) {
        return; // listener was cancelled
    }
    t_atom a[1];
    atom_setsym(a, name);
    ck_reply_push(x, ps_event, 1, a);
}

//-----------------------------------------------------------------------------------------------
// global variable callbacks

void cb_get_all_global_vars(const std::vector<Chuck_Globals_TypeValue> & list, void * data)
{
    t_ck* x = (t_ck*)data;
    post("cb_get_all_global_vars:");
    for (auto v : list) {
        post("type: %s name: %s", v.type.c_str(), v.name.c_str());
        if (x != NULL) {
            // name before type. once a patch strips our selector with
            // [route global], whatever follows becomes the new selector, and
            // ChucK's 'int' and 'float' type names collide with Max's typed
            // int/float methods there ("bad arguments for message int").
            // variable names cannot be ChucK keywords, so leading with the
            // name avoids that, and matches the shape of 'val <name> ...'
            t_atom a[2];
            atom_setsym(a, gensym(v.name.c_str()));
            atom_setsym(a + 1, gensym(v.type.c_str()));
            ck_reply_push(x, ps_global, 2, a);
        }
    }
}

// 'get' replies are emitted as 'val <name> <value...>'. the variable name is
// user-controlled text, so it travels as an argument rather than as the
// selector; putting it in the selector would let a ChucK global named 'shred'
// or 'event' masquerade as a control message
#define CK_VAL_MAX_VALUES (CK_REPLY_MAX_ATOMS - 1)

void cb_get_int(t_CKINT id, t_CKINT val)
{
    t_ck* x = ck_instance_from_id(id);
    if (x == NULL) return;
    t_atom a[2];
    atom_setsym(a, ck_pending_name(x, id));
    atom_setlong(a + 1, (t_atom_long)val);
    ck_reply_push(x, ps_val, 2, a);
}

void cb_get_float(t_CKINT id, t_CKFLOAT val)
{
    t_ck* x = ck_instance_from_id(id);
    if (x == NULL) return;
    t_atom a[2];
    atom_setsym(a, ck_pending_name(x, id));
    atom_setfloat(a + 1, (t_atom_float)val);
    ck_reply_push(x, ps_val, 2, a);
}

void cb_get_string(t_CKINT id, const char* val)
{
    t_ck* x = ck_instance_from_id(id);
    if (x == NULL) return;
    t_atom a[2];
    atom_setsym(a, ck_pending_name(x, id));
    atom_setsym(a + 1, gensym(val ? val : ""));
    ck_reply_push(x, ps_val, 2, a);
}

void cb_get_int_array(t_CKINT id, t_CKINT array[], t_CKUINT n)
{
    t_ck* x = ck_instance_from_id(id);
    if (x == NULL) return;
    t_atom a[CK_REPLY_MAX_ATOMS];
    long count = (n > (t_CKUINT)CK_VAL_MAX_VALUES) ? CK_VAL_MAX_VALUES : (long)n;
    atom_setsym(a, ck_pending_name(x, id));
    for (long i = 0; i < count; i++) {
        atom_setlong(a + 1 + i, (t_atom_long)array[i]);
    }
    ck_reply_push(x, ps_val, count + 1, a);
}

void cb_get_float_array(t_CKINT id, t_CKFLOAT array[], t_CKUINT n)
{
    t_ck* x = ck_instance_from_id(id);
    if (x == NULL) return;
    t_atom a[CK_REPLY_MAX_ATOMS];
    long count = (n > (t_CKUINT)CK_VAL_MAX_VALUES) ? CK_VAL_MAX_VALUES : (long)n;
    atom_setsym(a, ck_pending_name(x, id));
    for (long i = 0; i < count; i++) {
        atom_setfloat(a + 1 + i, (t_atom_float)array[i]);
    }
    ck_reply_push(x, ps_val, count + 1, a);
}

void cb_get_int_array_value(t_CKINT id, t_CKINT value)
{
    cb_get_int(id, value);
}

void cb_get_float_array_value(t_CKINT id, t_CKFLOAT value)
{
    cb_get_float(id, value);
}

void cb_get_assoc_int_array_value(t_CKINT id, t_CKINT val)
{
    cb_get_int(id, val);
}

void cb_get_assoc_float_array_value(t_CKINT id, t_CKFLOAT val)
{
    cb_get_float(id, val);
}

//-----------------------------------------------------------------------------------------------
// shred lifecycle watcher

void CK_DLL_CALL cb_shreds_watcher(Chuck_VM_Shred* shred, t_CKINT code,
                                   t_CKINT param, Chuck_VM* vm, void* bindle)
{
    t_ck* x = (t_ck*)bindle;
    if (x == NULL || shred == NULL) {
        return;
    }

    const char* what = NULL;
    switch (code) {
    case ckvm_shreds_watch_SPORK:    what = "add";      break;
    case ckvm_shreds_watch_REMOVE:   what = "remove";   break;
    case ckvm_shreds_watch_SUSPEND:  what = "suspend";  break;
    case ckvm_shreds_watch_ACTIVATE: what = "activate"; break;
    default: return;
    }

    t_atom a[2];
    atom_setsym(a, gensym(what));
    atom_setlong(a + 1, (t_atom_long)shred->get_id());
    ck_reply_push(x, ps_shred, 2, a);
}

//-----------------------------------------------------------------------------------------------
// set/get chuck global variables

t_max_err ck_set(t_ck* x, t_symbol* s, long argc, t_atom* argv)
{
    if (argc < 3) {
        ck_error(x, (char*)"ck_set: too few # of arguments");
        return MAX_ERR_GENERIC;        
    }

    if (!(argv->a_type == A_SYM && (argv+1)->a_type == A_SYM)) {
        ck_error(x, (char*)"ck_get: first two args must be symbols");
        return MAX_ERR_GENERIC;
    }

    t_symbol* type = atom_getsym(argv);
    t_symbol* name = atom_getsym(argv+1);

    if (argc == 3) {
        if (type == gensym("int") && (argv+2)->a_type == A_LONG) {
            t_atom_long value = atom_getlong(argv+2);
            if (x->chuck->vm()->globals_manager()->setGlobalInt(name->s_name, (t_CKINT)value)) {
                ck_info(x, "set %s -> %ld", name->s_name, (long)value);
                return MAX_ERR_NONE;
            }
        }
        else if (type == gensym("float") && (argv+2)->a_type == A_FLOAT) {
            t_atom_float value = atom_getfloat(argv+2);
            if (x->chuck->vm()->globals_manager()->setGlobalFloat(name->s_name, (t_CKFLOAT)value)) {
                ck_info(x, (char*)"set %s -> %f", name->s_name, value);
                return MAX_ERR_NONE;
            }
        }
        else if (type == gensym("string") && (argv+2)->a_type == A_SYM) {
            t_symbol* value = atom_getsym(argv+2);
            if (x->chuck->vm()->globals_manager()->setGlobalString(name->s_name, value->s_name)) {
                ck_info(x, (char*)"set %s -> %s", name->s_name, value->s_name);
                return MAX_ERR_NONE;
            }
        }
        return MAX_ERR_GENERIC;
    } else if (argc > 3) {
        int offset = 2;
        int length = (int)argc - offset;

        if (type == gensym("int[]")) { // list of longs
            t_atom_long* long_array = (t_atom_long*)sysmem_newptr(sizeof(t_atom_long) * length);
            for (int i = 0; i < length; i++) {
                ck_info(x, "set %s[%d] -> %ld ", name->s_name, i, (long)atom_getlong((argv+offset) + i));
                long_array[i] = atom_getlong((argv+offset) + i);
            }
            if (x->chuck->vm()->globals_manager()->setGlobalIntArray(name->s_name, long_array, length)) {
                sysmem_freeptr(long_array);
                return MAX_ERR_NONE;
            }
        }
        else if (type == gensym("float[]")) { // list of doubles
            double* float_array = (double*)sysmem_newptr(sizeof(double*) * length);
            for (int i = 0; i < length; i++) {
                ck_info(x, (char*)"set %s[%d] -> %f ", name->s_name, i, atom_getfloat((argv+offset) + i));
                float_array[i] = atom_getfloat((argv+offset) + i);
            }
            if (x->chuck->vm()->globals_manager()->setGlobalFloatArray(name->s_name, float_array, length)) {
                sysmem_freeptr(float_array);
                return MAX_ERR_NONE;                
            }
        }
        else if (type == gensym("int[i]")) {
            long index = atom_getlong((argv+2));
            long value = atom_getlong((argv+3));
            if (x->chuck->vm()->globals_manager()->setGlobalIntArrayValue(name->s_name, (t_CKUINT)index, (t_CKINT)value)) {
                ck_info(x, "set %s[%ld] -> %ld", name->s_name, (long)index, (long)value);
                return MAX_ERR_NONE;                
            }
        }
        else if (type == gensym("float[i]")) {
            long index = atom_getlong((argv+2));
            // must be t_atom_float: holding this in a long truncated every
            // fractional value on its way to the VM, so 'set float[i] a 0 0.5'
            // stored 0.0
            t_atom_float value = atom_getfloat((argv+3));
            if (x->chuck->vm()->globals_manager()->setGlobalFloatArrayValue(name->s_name, (t_CKUINT)index, (t_CKFLOAT)value)) {
                ck_info(x, "set %s[%ld] -> %f", name->s_name, index, (double)value);
                return MAX_ERR_NONE;
            }
        }
        else if (type == gensym("int[k]")) {
            t_symbol* key = atom_getsym((argv+2));
            long value = atom_getlong((argv+3));
            if (x->chuck->vm()->globals_manager()->setGlobalAssociativeIntArrayValue(name->s_name, key->s_name, (t_CKINT)value))
                return MAX_ERR_NONE;
        }
        else if (type == gensym("float[k]")) {
            t_symbol* key = atom_getsym((argv+2));
            // as above: truncating here silently discarded the fractional part
            t_atom_float value = atom_getfloat((argv+3));
            if (x->chuck->vm()->globals_manager()->setGlobalAssociativeFloatArrayValue(name->s_name, key->s_name, (t_CKFLOAT)value))
                return MAX_ERR_NONE;
        }
    }
    return MAX_ERR_GENERIC;
}

t_max_err ck_get(t_ck* x, t_symbol* s, long argc, t_atom* argv)
{
    if (argc < 2 || argc > 3) {
        ck_error(x, (char*)"ck_get: invalid # of arguments");
        return MAX_ERR_GENERIC;
    }
    
    if (!(argv->a_type == A_SYM && (argv+1)->a_type == A_SYM)) {
        ck_error(x, (char*)"ck_get: first two args must be symbols");
        return MAX_ERR_GENERIC;
    }

    t_symbol* type = atom_getsym(argv);
    t_symbol* name = atom_getsym(argv+1);

    // the callback-id overloads let the reply be routed back to this object;
    // the plain-name overloads cannot be attributed when several chuck~ objects
    // are present. the id also carries a ticket that recovers the variable name
    Chuck_Globals_Manager* gm = x->chuck->vm()->globals_manager();
    t_CKINT id = ck_pending_issue(x, name);

    if (argc == 2) {
        if (type == gensym("int")) {
            if (gm->getGlobalInt(name->s_name, id, cb_get_int))
                return MAX_ERR_NONE;
        } else if (type == gensym("float")) {
            if (gm->getGlobalFloat(name->s_name, id, cb_get_float))
                return MAX_ERR_NONE;
        } else if (type == gensym("string")) {
            if (gm->getGlobalString(name->s_name, id, cb_get_string))
                return MAX_ERR_NONE;
        } else if (type == gensym("int[]")) {
            if (gm->getGlobalIntArray(name->s_name, id, cb_get_int_array))
                return MAX_ERR_NONE;
        } else if (type == gensym("float[]")) {
            if (gm->getGlobalFloatArray(name->s_name, id, cb_get_float_array))
                return MAX_ERR_NONE;
        }
        return MAX_ERR_GENERIC;
    } else if (argc == 3) {
        if ((argv+2)->a_type == A_LONG) {
            t_atom_long index = atom_getlong(argv+2);
            if (type == gensym("int[]") || type == gensym("int[i]")) {
                if (gm->getGlobalIntArrayValue(name->s_name, id, (t_CKUINT)index, cb_get_int_array_value))
                    return MAX_ERR_NONE;
            } else if (type == gensym("float[]") || type == gensym("float[i]")) {
                if (gm->getGlobalFloatArrayValue(name->s_name, id, (t_CKUINT)index, cb_get_float_array_value))
                    return MAX_ERR_NONE;
            }
            return MAX_ERR_GENERIC;
        } else if ((argv+2)->a_type == A_SYM) {
            t_symbol* key = atom_getsym(argv+2);
            if (type == gensym("int[]") || type == gensym("int[k]")) {
                if (gm->getGlobalAssociativeIntArrayValue(name->s_name, id, key->s_name, cb_get_assoc_int_array_value))
                    return MAX_ERR_NONE;
            } else if (type == gensym("float[]") || type == gensym("float[k]")) {
                if (gm->getGlobalAssociativeFloatArrayValue(name->s_name, id, key->s_name, cb_get_assoc_float_array_value))
                    return MAX_ERR_NONE;
            }
        }
    }
    return MAX_ERR_GENERIC;
}

t_max_err ck_listen(t_ck* x, t_symbol* s, long listen_forever)
{
    // listeners are long-lived, so each gets a dedicated ticket slot rather than
    // a ring entry; the ticket is needed again to cancel the listener later
    long ticket = -1;
    for (long i = 0; i < CK_MAX_LISTENS; i++) {
        if (x->listen_names[i] == NULL) {
            ticket = i;
            break;
        }
        if (x->listen_names[i] == s) {
            ck_warn(x, (char*)"listen: already listening to event %s", s->s_name);
            return MAX_ERR_NONE;
        }
    }
    if (ticket < 0) {
        ck_error(x, (char*)"listen: too many active listeners (max %d)", CK_MAX_LISTENS);
        return MAX_ERR_GENERIC;
    }

    x->listen_names[ticket] = s;
    t_CKINT id = CK_ID_PACK(x->slot, ticket);

    if (x->chuck->vm()->globals_manager()->listenForGlobalEvent(
            s->s_name, id, cb_event, (t_CKBOOL)listen_forever)) {
        ck_info(x, (char*)"listening to event %s", s->s_name);
        return MAX_ERR_NONE;
    }
    x->listen_names[ticket] = NULL;
    return MAX_ERR_GENERIC;
}

t_max_err ck_unlisten(t_ck* x, t_symbol* s)
{
    long ticket = -1;
    for (long i = 0; i < CK_MAX_LISTENS; i++) {
        if (x->listen_names[i] == s) {
            ticket = i;
            break;
        }
    }
    if (ticket < 0) {
        ck_error(x, (char*)"unlisten: not listening to event %s", s->s_name);
        return MAX_ERR_GENERIC;
    }

    t_CKINT id = CK_ID_PACK(x->slot, ticket);
    if (x->chuck->vm()->globals_manager()->stopListeningForGlobalEvent(
            s->s_name, id, cb_event)) {
        x->listen_names[ticket] = NULL;
        ck_info(x, (char*)"stop listening to event %s", s->s_name);
        return MAX_ERR_NONE;
    }
    return MAX_ERR_GENERIC;
}

//-----------------------------------------------------------------------------------------------
// audio processing

void ck_dsp64(t_ck* x, t_object* dsp64, short* count, double samplerate,
              long maxvectorsize, long flags)
{
    // propagate the host sample rate to the VM. the rate is otherwise fixed at
    // object creation from sys_getsr(), so changing Max's sample rate afterwards
    // left chuck computing at the old rate (drifting pitch and timing).
    // setParam() forwards to Chuck_VM::update_srate() on a running VM.
    if (x->chuck != NULL && samplerate > 0) {
        t_CKINT sr = (t_CKINT)samplerate;
        if (sr != x->chuck->getParamInt(CHUCK_PARAM_SAMPLE_RATE)) {
            x->chuck->setParam(CHUCK_PARAM_SAMPLE_RATE, sr);
            ck_info(x, (char*)"sample rate updated to %d", (int)sr);
        }
    }

    delete[] x->in_chuck_buffer;
    delete[] x->out_chuck_buffer;

    x->in_chuck_buffer = new float[maxvectorsize * x->channels];
    x->out_chuck_buffer = new float[maxvectorsize * x->channels];

    memset(x->in_chuck_buffer, 0.f,
           sizeof(float) * maxvectorsize * x->channels);
    memset(x->out_chuck_buffer, 0.f,
           sizeof(float) * maxvectorsize * x->channels);

    // allocate tap buffer if tap is enabled. sized for the widest possible
    // multichannel fetch: getGlobalUGenSamplesMulti() writes numFrames samples
    // per channel, non-interleaved, for up to tap_channels channels
    if (x->tap_channels > 0) {
        delete[] x->tap_buffer;
        x->tap_buffer = new float[maxvectorsize * x->tap_channels];
        memset(x->tap_buffer, 0.f,
               sizeof(float) * maxvectorsize * x->tap_channels);
        x->tap_buffer_frames = maxvectorsize;
    }

    object_method(dsp64, gensym("dsp_add64"), x, ck_perform64, 0, NULL);
}

void ck_perform64(t_ck* x, t_object* dsp64, double** ins, long numins,
                  double** outs, long numouts, long sampleframes, long flags,
                  void* userparam)
{
    float* in_ptr = x->in_chuck_buffer;
    float* out_ptr = x->out_chuck_buffer;
    long n = sampleframes; // n = 64

    if (ins) {
        for (int i = 0; i < n; i++) {
            for (int chan = 0; chan < numins; chan++) {
                *(in_ptr++) = ins[chan][i];
            }
        }
    }

    x->chuck->run(x->in_chuck_buffer, x->out_chuck_buffer, n);

    // output main channels
    for (int i = 0; i < n; i++) {
        for (int chan = 0; chan < x->channels; chan++) {
            outs[chan][i] = *out_ptr++;
        }
    }

    // tap global UGen samples if enabled (each outlet taps independently)
    if (x->tap_channels > 0 && x->tap_buffer) {
        long tap_outlet_start = x->channels;  // tap outlets come after main outlets

        for (int chan = 0; chan < x->tap_channels; chan++) {
            t_symbol* ugen_name = x->tap_ugens[chan];

            if (ugen_name != gensym("")) {
                long nchans = x->tap_ugen_nchans[chan];
                t_CKBOOL success;

                if (nchans > 1) {
                    // multichannel UGen: fetch every channel once, on the outlet
                    // carrying channel 0, then fan the block out to the outlets
                    // that follow. chuck writes non-interleaved, channel-major.
                    if (x->tap_ugen_chan[chan] != 0) {
                        continue; // already filled by this group's channel 0
                    }
                    success = x->chuck->vm()->globals_manager()->getGlobalUGenSamplesMulti(
                        ugen_name->s_name, x->tap_buffer, (int)n, (int)nchans);

                    for (int c = 0; c < nchans; c++) {
                        long out_index = tap_outlet_start + chan + c;
                        for (int i = 0; i < n; i++) {
                            outs[out_index][i] = success
                                ? x->tap_buffer[c * n + i]
                                : 0.0;
                        }
                    }
                    continue;
                }

                // tap this outlet's UGen (mono)
                success = x->chuck->vm()->globals_manager()->getGlobalUGenSamples(
                    ugen_name->s_name, x->tap_buffer, (int)n);

                if (success) {
                    for (int i = 0; i < n; i++) {
                        outs[tap_outlet_start + chan][i] = x->tap_buffer[i];
                    }
                } else {
                    // UGen not found or not ready - output silence
                    for (int i = 0; i < n; i++) {
                        outs[tap_outlet_start + chan][i] = 0.0;
                    }
                }
            } else {
                // no UGen assigned to this outlet - output silence
                for (int i = 0; i < n; i++) {
                    outs[tap_outlet_start + chan][i] = 0.0;
                }
            }
        }
    }
}
