/**
    @file
    chuck~: chuck for Max
*/

#include "ext.h"
#include "ext_obex.h"
#include "ext_preferences.h"
#include "z_dsp.h"

#include <string>
#include <unordered_map>

#include "chuck.h"
#include "chuck_globals.h"

// globals defs
#define CHANNELS 1
#define EMBEDDED_CHUGINS 0

// global variables
static int MX_CK_COUNT = 0;

// typedefs
typedef void (*ck_callback)(void);
typedef std::unordered_map<std::string, ck_callback> callback_map;

// object struct
typedef struct _ck {
    t_pxobject ob;           // the object itself (t_pxobject in MSP)

    // chuck-related
    ChucK* chuck;            // chuck instance
    int oid;                 // object id
    long channels;           // n of input/output channels
    t_symbol* filename;      // name of chuck file in Max search path
    const char* working_dir; // chuck working directory
    const char* chugins_dir; // chugins directory
    float* in_chuck_buffer;  // intermediate chuck input buffer
    float* out_chuck_buffer; // intermediate chuck output buffer
    callback_map cb_map;     // callback map<string,callback>
    long loglevel;           // chuck log level
    long current_shred_id;   // current shred id
    t_symbol* editor;        // external text editor for chuck code
} t_ck;


// method prototypes
void* ck_new(t_symbol* s, long argc, t_atom* argv);
void ck_free(t_ck* x);
void ck_assist(t_ck* x, void* b, long m, long a, char* s);

// general message handlers
t_max_err ck_bang(t_ck* x); // (re)load chuck file
t_max_err ck_anything(t_ck* x, t_symbol* s, long argc, t_atom* argv); // set global params by name/value

// basic message handlers
t_max_err ck_run(t_ck* x, t_symbol* s);                              // run chuck file
t_max_err ck_edit(t_ck* x, t_symbol* s);                             // edit chuck file

// chuck vm message handlers
t_max_err ck_add(t_ck* x, t_symbol* s, long argc, t_atom* argv);     // add shred from file
t_max_err ck_remove(t_ck* x, t_symbol* s, long argc, t_atom* argv);  // remove shreds (all, last, by #)
t_max_err ck_replace(t_ck* x, t_symbol* s, long argc, t_atom* argv); // replace shreds 
t_max_err ck_clear(t_ck* x, t_symbol* s, long argc, t_atom* argv);   // clear_vm, clear_globals
t_max_err ck_reset(t_ck* x, t_symbol* s, long argc, t_atom* argv);   // clear_vm, reset_id
t_max_err ck_status(t_ck* x);
t_max_err ck_time(t_ck* x);

// event/callback message handlers
t_max_err ck_signal(t_ck* x, t_symbol* s);    // signal global event
t_max_err ck_broadcast(t_ck* x, t_symbol* s); // broadcast global event

// special message handlers
t_max_err ck_info(t_ck* x);                   // get info about running shreds
t_max_err ck_chugins(t_ck* x);
t_max_err ck_loglevel(t_ck* x, t_symbol* s, long argc, t_atom* argv);  // get and set loglevels


// helpers
bool path_exists(const char* name);
void ck_debug(t_ck* x, char* fmt, ...);
void ck_stdout_print(const char* msg);
void ck_stderr_print(const char* msg);
void ck_dblclick(t_ck* x);
t_max_err ck_check_file(t_ck* x, t_symbol* name);
t_max_err ck_compile_file(t_ck* x, const char* filename);
t_max_err ck_run_file(t_ck* x);
t_max_err ck_send_chuck_vm_msg(t_ck* x, Chuck_Msg_Type msg_type);
t_string* ck_get_path_from_external(t_class* c, char* subpath);
t_string* ck_get_path_from_package(t_class* c, char* subpath);

// audio processing
void ck_dsp64(t_ck* x, t_object* dsp64, short* count, double samplerate,
              long maxvectorsize, long flags);
void ck_perform64(t_ck* x, t_object* dsp64, double** ins, long numins,
                  double** outs, long numouts, long sampleframes, long flags,
                  void* userparam);

// callback registeration
t_max_err ck_register(t_ck* x, t_symbol* s);
t_max_err ck_unregister(t_ck* x, t_symbol* s);


// callbacks
void cb_demo(void);


// global class pointer variable
static t_class* ck_class = NULL;


//-----------------------------------------------------------------------------------------------
// callbacks

/* nothing useful here yet */

void cb_demo(void)
{
    post("==> demo callback is called!");
}


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

    class_addmethod(c, (method)ck_add,          "add",      A_GIMME, 0);
    class_addmethod(c, (method)ck_remove,       "remove",   A_GIMME, 0);
    class_addmethod(c, (method)ck_replace,      "replace",  A_GIMME, 0);
    class_addmethod(c, (method)ck_clear,        "clear",    A_GIMME, 0);
    class_addmethod(c, (method)ck_reset,        "reset",    A_GIMME, 0); // reset -> 'clear vm', 'reset id' 
    class_addmethod(c, (method)ck_status,       "status",   0);
    class_addmethod(c, (method)ck_time,         "time",     0);

    class_addmethod(c, (method)ck_info,         "info",     0);
    class_addmethod(c, (method)ck_chugins,      "chugins",  0);
    class_addmethod(c, (method)ck_loglevel,     "loglevel", A_GIMME, 0);

    // can't be called signal which is a Max global message
    class_addmethod(c, (method)ck_signal,       "sig",      A_SYM, 0);
    class_addmethod(c, (method)ck_broadcast,    "broadcast", A_SYM, 0);

    class_addmethod(c, (method)ck_register,     "register", A_SYM, 0);
    class_addmethod(c, (method)ck_unregister,   "unregister", A_SYM, 0);

    class_addmethod(c, (method)ck_bang,         "bang",     0);
    class_addmethod(c, (method)ck_anything,     "anything", A_GIMME, 0);

    class_addmethod(c, (method)ck_dsp64,        "dsp64",    A_CANT, 0);
    class_addmethod(c, (method)ck_assist,       "assist",   A_CANT, 0);
    class_addmethod(c, (method)ck_dblclick,     "dblclick", A_CANT, 0);
    
    // class attributes
    //------------------------------------------------------------------------

    CLASS_ATTR_SYM(c,       "editor", 0,      t_ck, editor);
    CLASS_ATTR_BASIC(c,     "editor", 0);

    CLASS_ATTR_SYM(c,       "file", 0,     t_ck,  filename);
    CLASS_ATTR_STYLE(c,     "file", 0,     "file");
    CLASS_ATTR_BASIC(c,     "file", 0);
    // CLASS_ATTR_SAVE(c,      "file", 0);

    // clang-format on
    //------------------------------------------------------------------------

    class_dspinit(c);
    class_register(CLASS_BOX, c);
    ck_class = c;
}


void* ck_new(t_symbol* s, long argc, t_atom* argv)
{
    t_ck* x = (t_ck*)object_alloc(ck_class);

    if (x) {
        // set default attributes
        // defaults can be overriden by compile-time definitions
        x->channels = CHANNELS;
        x->loglevel = CK_LOG_SYSTEM;
        x->current_shred_id = 0;
        x->filename = gensym("");
        x->editor = gensym("");
        x->working_dir = string_getptr(
            ck_get_path_from_package(ck_class, (char*)"/examples"));

        t_symbol* filename;

        if (argc == 1) {
            if (argv->a_type == A_LONG) {
                atom_arg_getlong((t_atom_long*)&x->channels, 0, argc, argv);
            } else if (argv->a_type == A_SYM) {
                filename = atom_getsymarg(0, argc, argv);
                ck_check_file(x, filename);
            } else {
                error("invalid arguments");
                return;
            }
        }
        else if (argc >= 2) {
            atom_arg_getlong((t_atom_long*)&x->channels, 0, argc, argv);  // is 1st arg of object
            filename = atom_getsymarg(1, argc, argv);    // is 2nd arg of object
            ck_check_file(x, filename);
        } 
        // else just use defaults

        dsp_setup((t_pxobject*)x, x->channels);   // MSP inlets: 2nd arg is # of inlets
    
        for (int i = 0; i < x->channels; i++) {
            outlet_new((t_pxobject*)x, "signal"); // signal outlet
        }

        // chuck-related
        x->chuck = NULL;
#if EMBEDDED_CHUGINS 
        x->chugins_dir = string_getptr(
            ck_get_path_from_external(ck_class, (char*)"/Contents/Resources/chugins"));
#else
        x->chugins_dir = string_getptr(
            ck_get_path_from_package(ck_class, (char*)"/examples/chugins"));
#endif        
        x->in_chuck_buffer = NULL;
        x->out_chuck_buffer = NULL;

        x->chuck = new ChucK();
        if (x->chuck == NULL) {
            error("critical: could not create chuck object");
            return NULL;
        }
        x->chuck->setParam(CHUCK_PARAM_SAMPLE_RATE, (t_CKINT)sys_getsr());
        x->chuck->setParam(CHUCK_PARAM_INPUT_CHANNELS, (t_CKINT)x->channels);
        x->chuck->setParam(CHUCK_PARAM_OUTPUT_CHANNELS, (t_CKINT)x->channels);
        x->chuck->setParam(CHUCK_PARAM_VM_HALT, (t_CKINT)0);
        x->chuck->setParam(CHUCK_PARAM_DUMP_INSTRUCTIONS, (t_CKINT)0);

        // set default chuck examples dirs
        std::string global_dir = std::string(x->working_dir);
        x->chuck->setParam(CHUCK_PARAM_WORKING_DIRECTORY, global_dir);

        // set default chuggins dirs
        std::string chugins_dir = std::string(x->chugins_dir);
        std::list<std::string> chugin_search;
#if EMBEDDED_CHUGINS
        // add extra package-level chugins directory
        chugin_search.push_back(global_dir + "/chugins");
#endif
        chugin_search.push_back(chugins_dir);
        x->chuck->setParam(CHUCK_PARAM_USER_CHUGIN_DIRECTORIES, chugin_search);

        // redirect chuck stdout/stderr to local callbacks
        x->chuck->setStdoutCallback(ck_stdout_print);
        x->chuck->setStderrCallback(ck_stderr_print);

        // object id corresponds to order of object creation
        x->oid = ++MX_CK_COUNT;

        // init cb_map and register callbacks
        x->cb_map = callback_map();
        x->cb_map.emplace("demo", &cb_demo);

        // init chuck
        x->chuck->init();
        x->chuck->start();
        ChucK::setLogLevel(x->loglevel);

        post("ChucK %s", x->chuck->version());
        post("inputs: %d  outputs: %d ", x->chuck->vm()->m_num_adc_channels,
             x->chuck->vm()->m_num_dac_channels);
        post("file: %s", x->filename->s_name);
        post("working dir: %s", x->working_dir);
        post("chugins dir: %s", x->chugins_dir);

        if (const char* editor = std::getenv("EDITOR")) {
            post("editor: %s", editor);
            x->editor = gensym(editor);
        } else if (x->editor = preferences_getsym("externaleditor")) {
            post("editor: %s", x->editor->s_name);
        } else {
            x->editor = gensym("");
        }

    }
    return (x);
}


void ck_free(t_ck* x)
{
    delete[] x->in_chuck_buffer;
    delete[] x->out_chuck_buffer;
    ChucK::globalCleanup();
    delete x->chuck;
    dsp_free((t_pxobject*)x);
}


void ck_assist(t_ck* x, void* b, long m, long a, char* s)
{
    if (m == ASSIST_INLET) { // inlet
        snprintf_zero(s, 20, "I am inlet %ld", a);
    } else {                 // outlet
        snprintf_zero(s, 20, "I am outlet %ld", a);
    }
}


//-----------------------------------------------------------------------------------------------
// helpers

/* x-platform solution to check if a path exists
 * 
 * since ext_path.h (`path_exists`) is not available
 * and std::filesystem::exists requires macos >= 10.15
 */
bool path_exists(const char* name) {

    if (FILE *file = fopen(name, "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}


void ck_stdout_print(const char* msg) { post("%s", msg); }

void ck_stderr_print(const char* msg) { post("%s", msg); }


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
    snprintf_zero(external_name, MAX_PATH_CHARS, ext_filename, c->c_sym->s_name);
    path_toabsolutesystempath(path_id, external_name, external_path);
    path_nameconform(external_path, conform_path, PATH_STYLE_MAX, PATH_TYPE_BOOT);
    result = string_new(conform_path);
    if (result != NULL && subpath != NULL) {
        string_append(result, subpath);
    }
    return result;
}

t_string* ck_get_path_from_package(t_class* c, char* subpath)
{
    char filename[MAX_FILENAME_CHARS];
    char externals_dir[MAX_PATH_CHARS];
    char package_dir[MAX_PATH_CHARS];
    t_string* package_dir_s;

    t_string* external_path = ck_get_path_from_external(c, NULL);
    const char* ext_path_c = string_getptr(external_path);
    path_splitnames(ext_path_c, externals_dir, filename);
    path_splitnames(externals_dir, package_dir, filename);
    package_dir_s = string_new(package_dir);
    if (subpath != NULL) {
        string_append(package_dir_s, subpath);
    }
    return package_dir_s;
}


t_max_err ck_check_file(t_ck* x, t_symbol* name)
{
    char filepath[MAX_PATH_CHARS];
    char normpath[MAX_PATH_CHARS];
    strncpy_zero(filepath, name->s_name, MAX_FILENAME_CHARS);
    path_nameconform(filepath, normpath, PATH_STYLE_MAX, PATH_TYPE_BOOT);
    // post("ck_check_file.name: %s", normpath);


    // 1. check if file exists
    if (path_exists(normpath)) {
        // post("ck_check_file.normpath exists: %s", normpath);
        x->filename = gensym(normpath);
        return MAX_ERR_NONE;
    }

    // 2. check if exists with an `examples` folder prefix
    char eg_file[MAX_PATH_CHARS];
    snprintf_zero(eg_file, MAX_PATH_CHARS, "%s/%s", x->working_dir, filepath);
    if (path_exists(eg_file)) {
        // post("ck_check_file.eg_file exists: %s", eg_file);
        x->filename = gensym(eg_file);
        return MAX_ERR_NONE;
    }

    // 3. use locatefile_extended to search
    char abspath[MAX_PATH_CHARS];
    short path, res;
    t_fourcc outtype;
    t_fourcc filetypelist;
    t_max_err err;

    res = locatefile_extended(filepath, &path, &outtype, &filetypelist, 1);
    if (res != 0)
        error("ck_check_file: locatefile_extended failed");
        return MAX_ERR_GENERIC;

    err = path_toabsolutesystempath(path, filepath, abspath);
    if (err != MAX_ERR_NONE)
        error("ck_check_file: path_toabsolutesystempath failed");
        return err;

    // post("ck_check_file.abspath: %s", abspath);

    normpath[0] = '\0'; // erase it to re-use it

    path_nameconform(abspath, normpath, PATH_STYLE_MAX, PATH_TYPE_BOOT);

    if (path_exists(normpath)) {
        // post("ck_check_file.normpath exists: %s", normpath);
        x->filename = gensym(normpath);
        return MAX_ERR_NONE;
    }

    error("ck_check_file: could not locate %s", name->s_name);
    return MAX_ERR_GENERIC;
}



t_max_err ck_compile_file(t_ck* x, const char* filename)
{
    if (x->chuck->compileFile(std::string(filename), "", 1)) {
        post("compiled: %s", filename);
        return MAX_ERR_NONE;
    } else {
        error("compilation error! : %s", filename);
        return MAX_ERR_GENERIC;
    }
}


t_max_err ck_run_file(t_ck* x)
{
    if (x->filename != gensym("")) {
        return ck_compile_file(x, x->filename->s_name);
    }
    error("ck_run_file: filename slot was empty");
    return MAX_ERR_GENERIC;
}


t_max_err ck_send_chuck_vm_msg(t_ck* x, Chuck_Msg_Type msg_type)
{
    Chuck_Msg* msg = NULL;

    msg = new Chuck_Msg;
    if (msg == NULL) {
        error("ck_send_chuck_vm_msg: could not create chuck msg");
        return MAX_ERR_GENERIC;
    }
    msg->type = msg_type;

    // null reply so that VM will delete for us when it's done
    msg->reply_cb = (ck_msg_func)NULL;

    if (x->chuck->vm()->globals_manager()->execute_chuck_msg_with_globals(msg)) {
        return MAX_ERR_NONE;
    } else {
        error("ck_send_chuck_vm_msg: could not send error msg");
        return MAX_ERR_GENERIC;
    }
}


void ck_debug(t_ck* x, char* fmt, ...)
{
    if (x->loglevel >= 6) {
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
        ck_check_file(x, s);
        return ck_run_file(x);
    }
    error("ck_run: eguires a filename to edit");
    return MAX_ERR_GENERIC;
}


t_max_err ck_edit(t_ck* x, t_symbol* s)
{
    if (x->editor == gensym("")) {
        error("editor attribute or EDITOR env var not set");
        return MAX_ERR_GENERIC;
    }

    if (s != gensym("")) {
        char conform_path[MAX_PATH_CHARS];
        path_nameconform(s->s_name, conform_path, PATH_STYLE_MAX, PATH_TYPE_BOOT);
        post("edit: %s", conform_path);
        std::string cmd = std::string(x->editor->s_name) + " " + std::string(conform_path);
        // std::string cmd = std::string("/usr/bin/open -a '") + std::string(x->editor->s_name) + "' " + std::string(conform_path);
        std::system(cmd.c_str());
        return MAX_ERR_NONE;
    }
    error("ck_edit: reguires a filename");
    return MAX_ERR_GENERIC;
}

void ck_dblclick(t_ck* x)
{
    if (x->filename != gensym("")) {
        ck_edit(x, x->filename);
    }
}


t_max_err ck_add(t_ck* x, t_symbol* s, long argc, t_atom* argv)
{
    t_symbol *filename_sym = _sym_nothing;

    if (argc < 1) {
        error("add message needs at least one <filename> argument");
        return MAX_ERR_GENERIC;
    }

    if ((argv)->a_type != A_SYM) {
        error("first argument must be a symbol");
        return MAX_ERR_GENERIC;
    }
    filename_sym = atom_getsym(argv);

    // get string
    std::string path = std::string(filename_sym->s_name);
    // filename
    std::string filename;
    // arguments
    std::string args;
    // extract args FILE:arg1:arg2:arg3
    extract_args( path, filename, args );

    std::string full_path = std::string(x->working_dir) + "/" + filename; // not portable

    // compile but don't run yet (instance == 0)
    if( !x->chuck->compileFile( full_path, args, 0 ) ) {
        error("could not compile file");
        return MAX_ERR_GENERIC;
    }

    // construct chuck msg (must allocate on heap, as VM will clean up)
    Chuck_Msg * msg = new Chuck_Msg();
    // set type
    msg->type = CK_MSG_ADD;
    // set code for incoming shred
    msg->code = x->chuck->vm()->carrier()->compiler->output();
    // create args array
    msg->args = new vector<string>;
    // extract args again but this time into vector
    extract_args( path, filename, *(msg->args) );
    // process ADD message, return new shred ID
    x->current_shred_id = x->chuck->vm()->process_msg( msg );
    return MAX_ERR_NONE;
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

t_max_err ck_replace(t_ck* x, t_symbol* s, long argc, t_atom* argv)
{
    long shred_id;
    t_symbol *filename_sym = _sym_nothing;

    if (argc < 2) {
        error("replace message needs at least two arguments");
        return MAX_ERR_GENERIC;
    }
    if (argv->a_type != A_LONG) {
        error("first argument must a long");
        return MAX_ERR_GENERIC;
    }
    shred_id = atom_getlong(argv);

    if ((argv+1)->a_type != A_SYM) {
        error("second argument must be a symbol");
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

    std::string full_path = std::string(x->working_dir) + "/" + filename; // not portable

    // compile but don't run yet (instance == 0)
    if( !x->chuck->compileFile( full_path, args, 0 ) ) {
        error("could not compile file");
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
                post("=> [chuck]: clean up global variables without clearing the whole VM");
                return ck_send_chuck_vm_msg(x, CK_MSG_CLEARGLOBALS);
            } 
            if (target == gensym("vm")) {
                return ck_send_chuck_vm_msg(x, CK_MSG_CLEARVM); 
            }
        }
    }
    error("must be 'clear globals' or 'clear vm'");
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
    error("must be 'reset id' or just 'reset' for clearvm");
    return MAX_ERR_GENERIC;
}


t_max_err ck_status(t_ck* x)
{
    Chuck_VM_Shreduler* shreduler = x->chuck->vm()->shreduler();
    shreduler->status();
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
    t_symbol* name = _sym_nothing;

    if (argc == 0) {
        x->loglevel = ChucK::getLogLevel();
        name = ck_get_loglevel_name(x->loglevel);
        post("loglevel is %d (%s)", x->loglevel, name->s_name);
        return MAX_ERR_NONE;
    }
    if (argc == 1) {
        long level = 2;
        if (argv->a_type == A_LONG) {
            level = atom_getlong(argv);
            if ((level >= 0) && (level <= 10)) {
                name = ck_get_loglevel_name(level);
                x->loglevel = level;
                post("setting loglevel to %d (%s)", x->loglevel, name->s_name);
                ChucK::setLogLevel(x->loglevel);
                return MAX_ERR_NONE;
            } else {
                error("loglevel out-of-range: must between 0-10 inclusive. Defaulting to level 2");
                ChucK::setLogLevel(CK_LOG_SYSTEM);
                return MAX_ERR_GENERIC;
            }
        }
    }
    error("could not get or set loglevel");
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

    // set '^' as shorthand for ck_status method
    if (s == gensym("=")) {
        ck_status(x);
        return MAX_ERR_NONE;
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
            t_atom_long* long_array = (t_atom_long*)sysmem_newptr(sizeof(t_atom_long*) * argc);
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


t_max_err ck_chugins(t_ck* x)
{
    post("probe chugins:");
    x->chuck->probeChugins();
    return MAX_ERR_NONE;
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


t_max_err ck_register(t_ck* x, t_symbol* s)
{
    if (!x->cb_map.count(s->s_name)) {
        error("event/callback not found: %s", s->s_name);
        return MAX_ERR_GENERIC;
    }
    std::string key = std::string(s->s_name);
    ck_callback cb = x->cb_map[key];
    // false: for a one off call, true: called everytime it is called
    if (x->chuck->vm()->globals_manager()->listenForGlobalEvent(s->s_name, cb, false)) {
        post("%s event/callback registered", s->s_name);
        return MAX_ERR_NONE;
    };
    return MAX_ERR_GENERIC;
}


t_max_err ck_unregister(t_ck* x, t_symbol* s)
{
    if (!x->cb_map.count(s->s_name)) {
        error("event/callback not found: %s", s->s_name);
        return MAX_ERR_GENERIC;
    }
    std::string key = std::string(s->s_name);
    ck_callback cb = x->cb_map[key];
    if (x->chuck->vm()->globals_manager()->stopListeningForGlobalEvent(s->s_name, cb)) {
        post("%s event/callback unregistered", s->s_name);
        return MAX_ERR_NONE;
    };
    return MAX_ERR_GENERIC;
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
