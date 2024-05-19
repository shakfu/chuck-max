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

// globals defs
#define CK_CHANNELS 1
#define CK_EMBEDDED_CHUGINS 0

namespace fs = std::filesystem;

// global variables
int CK_INSTANCE_COUNT = 0;
std::vector<std::string> CK_INSTANCE_NAMES;


// data structures
typedef struct _ck {
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
    long loglevel;                  // chuck log level
    long current_shred_id;          // current shred id
    t_symbol* editor;               // external text editor for chuck code
    t_symbol* edit_file;            // path of file to edit by external editor
    long run_needs_audio;           // only run/add shred if dsp is on
    t_symbol* code;                 // chuck code buffer
} t_ck;


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

// error-reporting / logging helpers
void ck_stdout_print(const char* msg);
void ck_stderr_print(const char* msg);
void ck_info(t_ck* x, char* fmt, ...);
void ck_warn(t_ck* x, char* fmt, ...);
void ck_debug(t_ck* x, char* fmt, ...);
void ck_error(t_ck* x, char* fmt, ...);

// helpers
void replace_character(char* str, char c1, char c2);
bool path_exists(const char* name);
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

// callbacks (events) -> map_cb_event
void cb_event(const char* name);

// callbacks (variables)
void cb_get_int(const char* name, t_CKINT val);
void cb_get_float(const char* name, t_CKFLOAT val);
void cb_get_string(const char* name, const char* val);
void cb_get_int_array(const char* name, t_CKINT array[], t_CKUINT n);
void cb_get_int_array_value(const char* name, t_CKINT value);
void cb_get_float_array(const char* name, t_CKFLOAT array[], t_CKUINT n);
void cb_get_float_array_value(const char* name, t_CKFLOAT value);
void cb_get_assoc_int_array_value(const char* name, t_CKINT val);
void cb_get_assoc_float_array_value(const char* name, t_CKFLOAT val);

// dump all global variables
void cb_get_all_global_vars(const std::vector<Chuck_Globals_TypeValue> & list, void * data);


// global class pointer variable
static t_class* ck_class = NULL;


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
        // set default attributes (allcaps can be overriden by compile-time defs)
        x->channels = CK_CHANNELS;
        x->loglevel = CK_LOG_SYSTEM;
        x->current_shred_id = 0;
        x->run_file = gensym("");
        x->edit_file = gensym("");
        x->editor = gensym("");
        x->working_dir = ck_get_path_from_package(ck_class, (char*)"/examples");
        x->patcher = NULL;
        x->box = NULL;
        x->patcher_dir = gensym("");
        x->code = gensym("");
        x->run_needs_audio = 0;

        // get external editor
        if (const char* editor = std::getenv("EDITOR")) {
            ck_info(x, (char*)"editor from env: %s", editor);
            x->editor = gensym(editor);
        } else {
            x->editor = gensym("");
        }

        // object id corresponds to order of object creation
        x->oid = CK_INSTANCE_COUNT++;

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
        t_symbol* filename;

        if (argc == 1) {
            if (argv->a_type == A_LONG) {
                atom_arg_getlong((t_atom_long*)&x->channels, 0, argc, argv);
            } else if (argv->a_type == A_SYM) {
                filename = atom_getsymarg(0, argc, argv);
                x->run_file = ck_check_file(x, filename);
            } else {
                ck_error(x, (char*)"invalid object arguments");
                return;
            }
        }
        else if (argc >= 2) {
            atom_arg_getlong((t_atom_long*)&x->channels, 0, argc, argv);  // is 1st arg of object
            filename = atom_getsymarg(1, argc, argv);    // is 2nd arg of object
            x->run_file = ck_check_file(x, filename);
        } 
        // else just use defaults

        dsp_setup((t_pxobject*)x, x->channels);   // MSP inlets: 2nd arg is # of inlets
    
        for (int i = 0; i < x->channels; i++) {
            outlet_new((t_pxobject*)x, "signal"); // signal outlet
        }

        // chuck-related
        x->chuck = NULL;
#if CK_EMBEDDED_CHUGINS 
        x->chugins_dir = ck_get_path_from_external(ck_class, (char*)"/Contents/Resources/chugins");
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

        // set default chuck examples dirs
        std::string global_dir = std::string(x->working_dir->s_name);
        x->chuck->setParam(CHUCK_PARAM_WORKING_DIRECTORY, global_dir);

        // set default chuggins dirs
        std::string chugins_dir = std::string(x->chugins_dir->s_name);
        std::list<std::string> chugin_search;
#if CK_EMBEDDED_CHUGINS
        // add extra package-level chugins directory
        chugin_search.push_back(global_dir + "/chugins");
#endif
        chugin_search.push_back(chugins_dir);
        x->chuck->setParam(CHUCK_PARAM_USER_CHUGIN_DIRECTORIES, chugin_search);

        // redirect chuck stdout/stderr to local callbacks
        x->chuck->setStdoutCallback(ck_stdout_print);
        x->chuck->setStderrCallback(ck_stderr_print);

        // init chuck
        x->chuck->init();
        x->chuck->start();
        ChucK::setLogLevel(x->loglevel);

        post("ChucK %s", x->chuck->version());
        post("inputs: %d  outputs: %d ",
            x->chuck->vm()->m_num_adc_channels,
            x->chuck->vm()->m_num_dac_channels);
        post("file: %s", x->run_file->s_name);
        post("working dir: %s", x->working_dir->s_name);
        post("chugins dir: %s", x->chugins_dir->s_name);
        post("patcher_dir: %s", x->patcher_dir->s_name);
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
        snprintf_zero(s, 512, "I am inlet %ld", a);
    } else {                 // outlet
        snprintf_zero(s, 512, "I am outlet %ld", a);
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
    int j, n = strlen(str);
    for (int i = j = 0; i < n; i++) {
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



void ck_warn(t_ck* x, char* fmt, ...)
{
    if (x->loglevel >= 4) {
        char msg[MAX_PATH_CHARS];

        va_list va;
        va_start(va, fmt);
        vsnprintf(msg, MAX_PATH_CHARS, fmt, va);
        va_end(va);

        object_warn((t_object*)x, "[warn] %s", msg);
    }
}

void ck_info(t_ck* x, char* fmt, ...)
{
    if (x->loglevel >= 5) {
        char msg[MAX_PATH_CHARS];

        va_list va;
        va_start(va, fmt);
        vsnprintf(msg, MAX_PATH_CHARS, fmt, va);
        va_end(va);

        object_post((t_object*)x, "[info] %s", msg);
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

        object_post((t_object*)x, "[debug] %s", msg);
    }
}

void ck_error(t_ck* x, char* fmt, ...)
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
    ck_info(x, (char*)"last_id: %d", id);
    return id;
}

long ck_spork_next_id(t_ck* x)
{
    long id = x->chuck->vm()->next_id();
    ck_info(x, (char*)"next_id: %d", id);
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
    path_nameconform(filepath, normpath, PATH_STYLE_MAX, PATH_TYPE_BOOT);


    // 1. check if file exists
    if (path_exists(normpath)) {
        return gensym(normpath);
    }

    // 2. check if exists with an `examples` folder prefix
    char eg_file[MAX_PATH_CHARS];
    snprintf_zero(eg_file, MAX_PATH_CHARS, "%s/%s", x->working_dir->s_name, filepath);
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
    if (res != 0)
        ck_error(x, (char*)"ck_check_file: locatefile_extended failed");
        return gensym("");

    err = path_toabsolutesystempath(path, filepath, abspath);
    if (err != MAX_ERR_NONE)
        ck_error(x, (char*)"ck_check_file: path_toabsolutesystempath failed");
        return gensym("");

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

    if (s != gensym("")) {
        x->edit_file = ck_check_file(x, s);
        if (x->edit_file != gensym("")) {
            std::string cmd;
            ck_debug(x, (char*)"edit: %s", x->edit_file->s_name);

            cmd = std::string(x->editor->s_name) + " " + std::string(x->edit_file->s_name);
            std::system(cmd.c_str());
            return MAX_ERR_NONE;
        }
    }
    ck_error(x, (char*)"ck_edit: reguires a valid filename");
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
        long* long_array = (long*)sysmem_newptr(sizeof(long*) * argc);
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
            ck_info(x, (char*)"%d:%s", i->get_id(), i->name.c_str());
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
                ck_info(x, (char*)"setting loglevel to %d (%s)", x->loglevel, name->s_name);
                ChucK::setLogLevel(x->loglevel);
                return MAX_ERR_NONE;
            } else {
                ck_error(x, (char*)"loglevel out-of-range: must between 0-10 inclusive. Defaulting to level 2");
                ChucK::setLogLevel(CK_LOG_SYSTEM);
                return MAX_ERR_GENERIC;
            }
        }
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
            x->chuck->vm()->globals_manager()->setGlobalString(s->s_name, p_sym->s_name);
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
    ck_info(x, (char*)"probe chugins:");
    x->chuck->probeChugins();
    return MAX_ERR_NONE;
}

t_max_err ck_docs(t_ck* x)
{
    ck_send_max_msg(x, gensym("launchbrowser"), "https://chuck.stanford.edu/doc");
}

t_max_err ck_globals(t_ck* x)
{
    if (x->chuck->vm()->globals_manager()->getAllGlobalVariables(cb_get_all_global_vars, NULL)) {
        return MAX_ERR_NONE;
    }
    ck_error(x, (char*)"could not dump global variable to console");
    return MAX_ERR_GENERIC;
}

t_max_err ck_vm(t_ck* x)
{
    ck_info(x, (char*)"VM %d / %d status", x->oid, CK_INSTANCE_COUNT);
    ck_info(x, (char*)"\tinitialized: %d", x->chuck->vm()->has_init());
    ck_info(x, (char*)"\trunning: %d", x->chuck->vm()->running());
    return MAX_ERR_NONE;
}

//-----------------------------------------------------------------------------------------------
// global event callback

void cb_event(const char* name)
{
    post("cb_event: %s", name);
}

//-----------------------------------------------------------------------------------------------
// global variable callbacks

void cb_get_all_global_vars(const std::vector<Chuck_Globals_TypeValue> & list, void * data)
{
    post("cb_get_all_global_vars:");
    for (auto v : list) {
        post("type: %s name: %s", v.type.c_str(), v.name.c_str());
    }
}

void cb_get_int(const char* name, t_CKINT val)
{
    post("cb_get_int: name: %s value: %d", name, val);
}

void cb_get_float(const char* name, t_CKFLOAT val)
{
    post("cb_get_float: name: %s value: %f", name, val);
}

void cb_get_string(const char* name, const char* val)
{
    post("cb_get_string: name: %s value: %s", name, val);
}

void cb_get_int_array(const char* name, t_CKINT array[], t_CKUINT n)
{
    post("cb_get_int_array: name: %s size: %d", name, n);
    for (int i = 0; i < n; i++) {
        post("array[%d] = %d", i, array[i]);
    }
}

void cb_get_float_array(const char* name, t_CKFLOAT array[], t_CKUINT n)
{
    post("cb_get_float_array: name: %s size: %d", name, n);
    for (int i = 0; i < n; i++) {
        post("array[%d] = %d", i, array[i]);
    }
}

void cb_get_int_array_value(const char* name, t_CKINT value)
{
    post("cb_get_int_array_value: name: %s value: %d", name, value);
}

void cb_get_float_array_value(const char* name, t_CKFLOAT value)
{
    post("cb_get_float_array_value: name: %s value: %d", name, value);
}

void cb_get_assoc_int_array_value(const char* name, t_CKINT val)
{
     post("cb_get_assoc_int_array_value: name: %s value: %d", name, val);
}

void cb_get_assoc_float_array_value(const char* name, t_CKFLOAT val)
{
     post("cb_get_assoc_float_array_value: name: %s value: %f", name, val);
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
                ck_info(x, (char*)"set %s -> %d", name->s_name, value);
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
            long* long_array = (long*)sysmem_newptr(sizeof(long*) * length);
            for (int i = 0; i < length; i++) {
                ck_info(x, (char*)"set %s[%d] -> %d ", name->s_name, i, atom_getlong((argv+offset) + i));
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
                ck_info(x, (char*)"set %s %d -> %d", name->s_name, index, value);
                return MAX_ERR_NONE;                
            }
        }
        else if (type == gensym("float[i]")) {
            long index = atom_getlong((argv+2));
            long value = atom_getfloat((argv+3));
            if (x->chuck->vm()->globals_manager()->setGlobalFloatArrayValue(name->s_name, (t_CKUINT)index, (t_CKFLOAT)value)) {                
                ck_info(x, (char*)"set %s %d -> %f", name->s_name, index, value);
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
            long value = atom_getfloat((argv+3));
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

    if (argc == 2) {
        if (type == gensym("int")) {
            if (x->chuck->vm()->globals_manager()->getGlobalInt(name->s_name, cb_get_int))
                return MAX_ERR_NONE;
        } else if (type == gensym("float")) {
            if (x->chuck->vm()->globals_manager()->getGlobalFloat(name->s_name, cb_get_float))
                return MAX_ERR_NONE;
        } else if (type == gensym("string")) {
            if (x->chuck->vm()->globals_manager()->getGlobalString(name->s_name, cb_get_string))
                return MAX_ERR_NONE;
        } else if (type == gensym("int[]")) {
            if (x->chuck->vm()->globals_manager()->getGlobalIntArray(name->s_name, cb_get_int_array))
                return MAX_ERR_NONE;
        } else if (type == gensym("float[]")) {
            if (x->chuck->vm()->globals_manager()->getGlobalFloatArray(name->s_name, cb_get_float_array))
                return MAX_ERR_NONE;
        }
        return MAX_ERR_GENERIC;
    } else if (argc == 3) {
        if ((argv+2)->a_type == A_LONG) {
            t_atom_long index = atom_getlong(argv+2);
            if (type == gensym("int[]") || type == gensym("int[i]")) {
                if (x->chuck->vm()->globals_manager()->getGlobalIntArrayValue(name->s_name, (t_CKUINT)index, cb_get_int_array_value))
                    return MAX_ERR_NONE;
            } else if (type == gensym("float[]") || type == gensym("float[i]")) {
                if (x->chuck->vm()->globals_manager()->getGlobalFloatArrayValue(name->s_name, (t_CKUINT)index, cb_get_float_array_value))
                    return MAX_ERR_NONE;
            }
            return MAX_ERR_GENERIC;
        } else if ((argv+2)->a_type == A_SYM) {
            t_symbol* key = atom_getsym(argv+2);
            if (type == gensym("int[]") || type == gensym("int[k]")) {
                if (x->chuck->vm()->globals_manager()->getGlobalAssociativeIntArrayValue(name->s_name, key->s_name, cb_get_assoc_int_array_value))
                    return MAX_ERR_NONE;;
            } else if (type == gensym("float[]") || type == gensym("float[k]")) {
                if (x->chuck->vm()->globals_manager()->getGlobalAssociativeFloatArrayValue(name->s_name, key->s_name, cb_get_assoc_float_array_value))
                    return MAX_ERR_NONE;;
            }
        }
    }
    return MAX_ERR_GENERIC;
}

t_max_err ck_listen(t_ck* x, t_symbol* s, long listen_forever)
{
    if (x->chuck->vm()->globals_manager()->listenForGlobalEvent(s->s_name, cb_event, (bool)listen_forever)) {
        ck_info(x, (char*)"listening to event %s", s->s_name);
        return MAX_ERR_NONE;
    };
    return MAX_ERR_GENERIC;
}

t_max_err ck_unlisten(t_ck* x, t_symbol* s)
{
    if (x->chuck->vm()->globals_manager()->stopListeningForGlobalEvent(s->s_name, cb_event)) {
        ck_info(x, (char*)"stop listening to event %s", s->s_name);
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
