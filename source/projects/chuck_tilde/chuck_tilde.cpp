/**
    @file
    chuck~: chuck for Max
*/

#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

#include <libgen.h>
#include <unistd.h>

#include "chuck.h"
#include "chuck_globals.h"

#define DEBUG 0

int MX_CK_COUNT = 0;


typedef struct _ck {
    t_pxobject ob;           // the object itself (t_pxobject in MSP)

    // chuck-related
    ChucK* chuck;            // chuck instance
    int oid;                 // object id
    long channels;           // n of input/output channels
    t_bool debug;            // flag to switch per-object debug state
    t_symbol* filename;      // name of chuck file in Max search path
    const char* working_dir; // chuck working directory
    float* in_chuck_buffer;  // intermediate chuck input buffer
    float* out_chuck_buffer; // intermediate chuck output buffer
} t_ck;


// method prototypes
void* ck_new(t_symbol* s, long argc, t_atom* argv);
void ck_free(t_ck* x);
void ck_assist(t_ck* x, void* b, long m, long a, char* s);

// general message handlers
t_max_err ck_bang(t_ck* x); // (re)load chuck file
t_max_err ck_anything(t_ck* x, t_symbol* s, long argc,
                      t_atom* argv); // set global params by name, value

// special message handlers
t_max_err ck_run(t_ck* x, t_symbol* s);       // run chuck file
t_max_err ck_info(t_ck* x);                   // get info about running shreds
t_max_err ck_reset(t_ck* x);                  // remove all shreds and clean vm
t_max_err ck_signal(t_ck* x, t_symbol* s);    // signal global event
t_max_err ck_broadcast(t_ck* x, t_symbol* s); // broadcast global event
t_max_err ck_remove(t_ck* x, t_symbol* s, long argc,
                    t_atom* argv); // remove shreds (all, last or by #)

// helpers
void ck_stdout_print(const char* msg);
void ck_stderr_print(const char* msg);
void ck_run_file(t_ck* x);
void ck_compile_file(t_ck* x, const char* filename);
t_max_err ck_send_chuck_vm_msg(t_ck* x, Chuck_Msg_Type msg_type);
t_string* ck_get_path_from_external(t_class* c, char* subpath);
t_string* ck_get_path_from_package(t_class* c, char* subpath);

// audio processing
void ck_dsp64(t_ck* x, t_object* dsp64, short* count, double samplerate,
              long maxvectorsize, long flags);
void ck_perform64(t_ck* x, t_object* dsp64, double** ins, long numins,
                  double** outs, long numouts, long sampleframes, long flags,
                  void* userparam);


// global class pointer variable
static t_class* ck_class = NULL;


//-----------------------------------------------------------------------------------------------
// core

void ext_main(void* r)
{
    t_class* c = class_new("chuck~", (method)ck_new, (method)ck_free,
                           (long)sizeof(t_ck), 0L, A_GIMME, 0);

    class_addmethod(c, (method)ck_run,          "run", A_SYM, 0);
    class_addmethod(c, (method)ck_info,         "info", 0);
    class_addmethod(c, (method)ck_reset,        "reset", 0);

    // can't be called signal which is a Max global message
    class_addmethod(c, (method)ck_signal,       "sig", A_SYM, 0);
    class_addmethod(c, (method)ck_broadcast,    "broadcast", A_SYM, 0);
    class_addmethod(c, (method)ck_remove,       "remove", A_GIMME, 0);

    class_addmethod(c, (method)ck_bang,         "bang", 0);
    class_addmethod(c, (method)ck_anything,     "anything", A_GIMME, 0);

    class_addmethod(c, (method)ck_dsp64,        "dsp64", A_CANT, 0);
    class_addmethod(c, (method)ck_assist,       "assist", A_CANT, 0);

    CLASS_ATTR_LONG(c,      "debug", 0,  t_ck, debug);
    CLASS_ATTR_STYLE(c,     "debug", 0, "onoff");
    CLASS_ATTR_DEFAULT(c,   "debug", 0,     "0");
    CLASS_ATTR_BASIC(c,     "debug", 0);
    CLASS_ATTR_SAVE(c,      "debug", 0);

    class_dspinit(c);
    class_register(CLASS_BOX, c);
    ck_class = c;
}


void* ck_new(t_symbol* s, long argc, t_atom* argv)
{
    t_ck* x = (t_ck*)object_alloc(ck_class);

    if (x) {
        // set default attributes
        x->channels = 1;
        x->debug = DEBUG;

        if (argc == 0) {
            x->filename = gensym("");
        }
        else if (argc == 1) {
            x->filename = atom_getsymarg(0, argc, argv);    // 1st arg of object
        }
        else if (argc >= 2) {
            atom_arg_getlong(&x->channels, 0, argc, argv);  // 1st arg of object
            x->filename = atom_getsymarg(1, argc, argv);    // 2st arg of object
        }

        dsp_setup((t_pxobject*)x,
                  x->channels); // MSP inlets: arg is # of inlets and is
                                // REQUIRED! use 0 if you don't need inlets

        for (int i = 0; i < x->channels; i++) {
            // post("created: outlet %d", i);
            outlet_new(x, "signal"); // signal outlet (note "signal" rather than NULL)
        }

        // chuck-related
        x->working_dir = string_getptr(
            ck_get_path_from_package(ck_class, (char*)"/examples"));
        x->in_chuck_buffer = NULL;
        x->out_chuck_buffer = NULL;

        x->chuck = new ChucK();
        x->chuck->setParam(CHUCK_PARAM_SAMPLE_RATE, (t_CKINT)sys_getsr());
        x->chuck->setParam(CHUCK_PARAM_INPUT_CHANNELS, (t_CKINT)x->channels);
        x->chuck->setParam(CHUCK_PARAM_OUTPUT_CHANNELS, (t_CKINT)x->channels);
        x->chuck->setParam(CHUCK_PARAM_VM_HALT, (t_CKINT)0);
        x->chuck->setParam(CHUCK_PARAM_DUMP_INSTRUCTIONS, (t_CKINT)0);
        // directory for compiled code
        std::string global_dir = std::string(x->working_dir);
        x->chuck->setParam(CHUCK_PARAM_WORKING_DIRECTORY, global_dir);
        std::list<std::string> chugin_search;
        chugin_search.push_back(global_dir + "/chugins");
        x->chuck->setParam(CHUCK_PARAM_USER_CHUGIN_DIRECTORIES, chugin_search);
        x->chuck->setStdoutCallback(ck_stdout_print);
        x->chuck->setStderrCallback(ck_stderr_print);
        x->oid = ++MX_CK_COUNT;

        // init chuck
        x->chuck->init();
        x->chuck->start();

        post("ChucK %s", x->chuck->version());
        post("inputs: %d  outputs: %d ", x->chuck->vm()->m_num_adc_channels,
             x->chuck->vm()->m_num_dac_channels);

        post("file: %s", x->filename->s_name);
        // post("working dir: %s", x->working_dir);
        // post("chugins dir: %s/chugins", x->working_dir);
    }
    return (x);
}


void ck_free(t_ck* x)
{
    delete[] x->in_chuck_buffer;
    delete[] x->out_chuck_buffer;
    delete x->chuck;
    dsp_free((t_pxobject*)x);
}


void ck_assist(t_ck* x, void* b, long m, long a, char* s)
{
    if (m == ASSIST_INLET) { // inlet
        snprintf_zero(s, 2, "I am inlet %ld", a);
    } else { // outlet
        snprintf_zero(s, 2, "I am outlet %ld", a);
    }
}


//-----------------------------------------------------------------------------------------------
// helpers


void ck_stdout_print(const char* msg) { post("ck_stdout -> %s", msg); }

void ck_stderr_print(const char* msg) { post("ck_stderr -> %s", msg); }


t_string* ck_get_path_from_external(t_class* c, char* subpath)
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
    snprintf_zero(external_name, MAX_PATH_CHARS, ext_filename,
                  c->c_sym->s_name);
    path_toabsolutesystempath(path_id, external_name, external_path);
    path_nameconform(external_path, conform_path, PATH_STYLE_MAX,
                     PATH_TYPE_BOOT);
    result = string_new(external_path);
    if (subpath != NULL) {
        string_append(result, subpath);
    }
    return result;
}


t_string* ck_get_path_from_package(t_class* c, char* subpath)
{
    t_string* result;
    t_string* external_path = ck_get_path_from_external(c, NULL);

    const char* ext_path_c = string_getptr(external_path);

    result = string_new(dirname(dirname((char*)ext_path_c)));

    if (subpath != NULL) {
        string_append(result, subpath);
    }
    return result;
}


void ck_compile_file(t_ck* x, const char* filename)
{
    if (x->chuck->compileFile(std::string(filename), "", 1)) {
        post("compilation successful: %s", filename);
    } else {
        error("compilation error! : %s", filename);
    }
}


void ck_run_file(t_ck* x)
{
    if (x->filename != gensym("")) {
        char norm_path[MAX_PATH_CHARS];
        path_nameconform(x->filename->s_name, norm_path, PATH_STYLE_MAX,
                         PATH_TYPE_BOOT);
        if (access(norm_path, F_OK) == 0) { // file exists in path
            ck_compile_file(x, norm_path);
        } else { // try in the example folder
            char ck_file[MAX_PATH_CHARS];
            snprintf_zero(ck_file, MAX_PATH_CHARS, "%s/%s",
                x->working_dir, x->filename->s_name);
            ck_compile_file(x, ck_file);
        }
    }
}


t_max_err ck_send_chuck_vm_msg(t_ck* x, Chuck_Msg_Type msg_type)
{
    Chuck_Msg* msg = new Chuck_Msg;
    msg->type = msg_type;

    // null reply so that VM will delete for us when it's done
    msg->reply_cb = (ck_msg_func)NULL;

    if (x->chuck->vm()->globals_manager()->execute_chuck_msg_with_globals(msg)) {
        return MAX_ERR_NONE;
    } else {
        return MAX_ERR_GENERIC;
    }
}


void ck_debug(t_ck* x, char* fmt, ...)
{
    if (x->debug) {
        char msg[MAX_PATH_CHARS];

        va_list va;
        va_start(va, fmt);
        vsnprintf(msg, MAX_PATH_CHARS, fmt, va);
        va_end(va);

        object_post((t_object*)x, "[DEBUG] (%d) %s", x->oid, msg);
    }
}


//-----------------------------------------------------------------------------------------------
// message handlers


t_max_err ck_bang(t_ck* x)
{
    ck_run_file(x);
    return MAX_ERR_NONE;
}


t_max_err ck_run(t_ck* x, t_symbol* s)
{
    if (s != gensym("")) {
        post("filename: %s", s->s_name);
        x->filename = s;
        ck_run_file(x);
        return MAX_ERR_NONE;
    }
    return MAX_ERR_GENERIC;
}


t_max_err ck_anything(t_ck* x, t_symbol* s, long argc, t_atom* argv)
{
    // TODO:
    //  - should check set op (true if succeed)
    //  - handle case of 2 length array (maybe)

    if (s == NULL || argc == 0) {
        goto error;
    }

    if (argc == 1) {            // <param-name> <value>
        switch (argv->a_type) { // really argv[0]
        case A_FLOAT: {
            float p_float = atom_getfloat(argv);
            ck_debug(x, (char*)"param %s: %f", s->s_name, p_float);            
            x->chuck->vm()->globals_manager()->setGlobalFloat(s->s_name,
                                                              p_float);
            break;
        }
        case A_LONG: {
            long p_long = atom_getlong(argv);
            ck_debug(x, (char*)"param %s: %d", s->s_name, p_long);
            x->chuck->vm()->globals_manager()->setGlobalInt(s->s_name, p_long);
            break;
        }
        case A_SYM: {
            t_symbol* p_sym = atom_getsym(argv);
            if (p_sym == NULL) {
                goto error;
            }
            ck_debug(x, (char*)"param %s: %s", s->s_name, p_sym->s_name);
            x->chuck->vm()->globals_manager()->setGlobalString(s->s_name,
                                                               p_sym->s_name);
            break;
        }
        default:
            goto error;
            break;
        }

    } else { // type is a list

        if (argv->a_type == A_LONG) { // list of longs
            long* long_array = (long*)sysmem_newptr(sizeof(long*) * argc);
            for (int i = 0; i < argc; i++) {
                long_array[i] = atom_getlong(argv + i);
            }
            x->chuck->vm()->globals_manager()->setGlobalIntArray(
                s->s_name, long_array, argc);
            sysmem_freeptr(long_array);
        }

        else if (argv->a_type == A_FLOAT) { // list of doubles
            double* float_array = (double*)sysmem_newptr(sizeof(double*)
                                                         * argc);
            for (int i = 0; i < argc; i++) {
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
                    ->setGlobalAssociativeIntArrayValue(s->s_name, key->s_name,
                                                        p_long);
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
    error("[ck_anything] cannot set chuck global param");
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

    } else {
        // default to last
        msg->type = CK_MSG_REMOVE;
        msg->param = 0xffffffff;
    }

    msg->reply_cb = (ck_msg_func)0;
    x->chuck->vm()->queue_msg(msg, 1);
    return MAX_ERR_NONE;
}


t_max_err ck_signal(t_ck* x, t_symbol* s)
{
    post("signal: %s", s->s_name);
    if (x->chuck->vm()->globals_manager()->signalGlobalEvent(s->s_name)) {
        return MAX_ERR_NONE;
    } else {
        error("[ck_signal] signal global event '%s' failed", s->s_name);
        return MAX_ERR_GENERIC;
    }
}


t_max_err ck_broadcast(t_ck* x, t_symbol* s)
{
    post("broadcast: %s", s->s_name);
    if (x->chuck->vm()->globals_manager()->broadcastGlobalEvent(s->s_name)) {
        return MAX_ERR_NONE;
    } else {
        error("[ck_broadcast] broadcast global event '%s' failed", s->s_name);
        return MAX_ERR_GENERIC;
    }
}


t_max_err ck_reset(t_ck* x)
{
    t_max_err err = MAX_ERR_NONE;
    // post("# of vms: %d", x->chuck->numVMs());

    // create a msg asking to clear the globals
    err = ck_send_chuck_vm_msg(x, CK_MSG_CLEARGLOBALS);

    // create a msg asking to clear the VM
    err = ck_send_chuck_vm_msg(x, CK_MSG_CLEARVM);

    if (err == MAX_ERR_GENERIC) {
        error("[ck_reset] failed");
    }
    return err;
}


t_max_err ck_info(t_ck* x)
{
    Chuck_VM_Shreduler* shreduler = x->chuck->vm()->shreduler();
    std::vector<Chuck_VM_Shred*> shreds;
    shreduler->get_all_shreds(shreds);
    post("\nRUNNING SHREDS:");
    for (const Chuck_VM_Shred* i : shreds) {
        post("%d-%d: %s", x->oid, i->get_id(), i->name.c_str());
    }
    return MAX_ERR_NONE;
}


//-----------------------------------------------------------------------------------------------
// audio processing


void ck_dsp64(t_ck* x, t_object* dsp64, short* count, double samplerate,
              long maxvectorsize, long flags)
{
    // post("sample rate: %f", samplerate);
    // post("maxvectorsize: %d", maxvectorsize);

    delete[] x->in_chuck_buffer;
    delete[] x->out_chuck_buffer;

    x->in_chuck_buffer = new float[maxvectorsize * x->channels];
    x->out_chuck_buffer = new float[maxvectorsize * x->channels];

    memset(x->in_chuck_buffer, 0.f,
           sizeof(float) * maxvectorsize * x->channels);
    memset(x->out_chuck_buffer, 0.f,
           sizeof(float) * maxvectorsize * x->channels);

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

    for (int i = 0; i < n; i++) {
        for (int chan = 0; chan < numouts; chan++) {
            outs[chan][i] = *out_ptr++;
        }
    }
}
