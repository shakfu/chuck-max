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

#include <string>
#include <unordered_map>
#include <memory>
#include <array>
#include <filesystem>
#include <sstream>
#include <vector>

#include "chuck.h"
#include "chuck_globals.h"

// globals defs
#define CK_CHANNELS 1
#define CK_EMBEDDED_CHUGINS 0

namespace fs = std::filesystem;

// global variables
int MX_CK_COUNT = 0;

// typedefs
typedef void (*ck_callback)(void);
typedef std::unordered_map<std::string, ck_callback> callback_map;

// data structures
typedef struct _ck {
    t_pxobject ob;           // the object itself (t_pxobject in MSP)

    // max-related
    t_symbol* name;          // instance unique name (used for scripting)
    t_patcher* patcher;      // to send msgs to objects
    t_box* box;              // the ui box of the chuck~ instance?
    t_symbol* patcher_dir;   // patcher directory

    // chuck-related
    ChucK* chuck;            // chuck instance
    int oid;                 // object id
    long channels;           // n of input/output channels
    t_symbol* run_file;      // path of chuck file to run
    t_symbol* working_dir;   // chuck working directory
    t_symbol* chugins_dir;   // chugins directory
    float* in_chuck_buffer;  // intermediate chuck input buffer
    float* out_chuck_buffer; // intermediate chuck output buffer
    callback_map cb_map;     // callback map<string,callback>
    long loglevel;           // chuck log level
    long current_shred_id;   // current shred id
    t_symbol* editor;        // external text editor for chuck code
    t_symbol* edit_file;     // path of file to edit by external editor
    long run_needs_audio;    // only run/add shred if dsp is on
} t_ck;


// method prototypes
void* ck_new(t_symbol* s, long argc, t_atom* argv);
void ck_free(t_ck* x);
void ck_assist(t_ck* x, void* b, long m, long a, char* s);

// attribute handkers
t_max_err ck_editor_get(t_ck *x, t_object *attr, long *argc, t_atom **argv);
t_max_err ck_editor_set(t_ck *x, t_object *attr, long argc, t_atom *argv);

// general message handlers
t_max_err ck_bang(t_ck* x);                     // (re)load chuck file
t_max_err ck_anything(t_ck* x, t_symbol* s, long argc, t_atom* argv); // set global params by name/value

// basic message handlers
t_max_err ck_run(t_ck* x, t_symbol* s);         // run chuck file
t_max_err ck_edit(t_ck* x, t_symbol* s);        // edit chuck file

// chuck vm message handlers
t_max_err ck_add(t_ck* x, t_symbol* s, long argc, t_atom* argv);     // add shred from file
t_max_err ck_remove(t_ck* x, t_symbol* s, long argc, t_atom* argv);  // remove shreds (all, last, by #)
t_max_err ck_replace(t_ck* x, t_symbol* s, long argc, t_atom* argv); // replace shreds 
t_max_err ck_clear(t_ck* x, t_symbol* s, long argc, t_atom* argv);   // clear_vm, clear_globals
t_max_err ck_reset(t_ck* x, t_symbol* s, long argc, t_atom* argv);   // clear_vm, reset_id
t_max_err ck_status(t_ck* x);                   // metadata about running shreds in the chuck vm
t_max_err ck_time(t_ck* x);                     // current time

// event/callback message handlers
t_max_err ck_signal(t_ck* x, t_symbol* s);      // signal global event
t_max_err ck_broadcast(t_ck* x, t_symbol* s);   // broadcast global event

// special message handlers
t_max_err ck_vm(t_ck* x);                       // get vm state
t_max_err ck_globals(t_ck* x);                  // dump global variables
t_max_err ck_docs(t_ck* x);                     // open chuck docs in a browser
t_max_err ck_info(t_ck* x);                     // get info about running shreds
t_max_err ck_chugins(t_ck* x);                  // probe and list available chugins
t_max_err ck_loglevel(t_ck* x, t_symbol* s, long argc, t_atom* argv);  // get and set loglevels

// helpers
bool path_exists(const char* name);
void ck_debug(t_ck* x, char* fmt, ...);
void ck_stdout_print(const char* msg);
void ck_stderr_print(const char* msg);
void ck_dblclick(t_ck* x);
t_max_err ck_send_max_msg(t_ck* x, t_symbol* s, const char* parsestr);
t_max_err ck_check_editor(t_ck* x, t_symbol* entry);
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

// callback registeration
t_max_err ck_register(t_ck* x, t_symbol* s);
t_max_err ck_unregister(t_ck* x, t_symbol* s);

// callback demo
t_max_err ck_demo(t_ck* x);


// callbacks (events)
void cb_global_event1(void);
void cb_global_event2(const char* name);
void cb_global_event3(t_CKINT cb_id);

// callbacks (variables)
void cb_global_int1(t_CKINT val);
void cb_global_int2(const char* name, t_CKINT val);
void cb_global_int3(t_CKINT cb_id, t_CKINT val);
void cb_global_float1(t_CKFLOAT val);
void cb_global_float2(const char* name, t_CKFLOAT val);
void cb_global_float3(t_CKINT cb_id, t_CKFLOAT val);
void cb_global_string1(const char* val);
void cb_global_string2(const char* name, const char* val);
void cb_global_string3(t_CKINT cb_id, const char* val);
void cb_global_int_array1(t_CKINT array[], t_CKUINT n);
void cb_global_int_array2(const char* name, t_CKINT array[], t_CKUINT n);
void cb_global_int_array3(t_CKINT cb_id, t_CKINT array[], t_CKUINT n);
void cb_global_int_array_value1(t_CKINT value);
void cb_global_int_array_value2(const char* name, t_CKINT value);
void cb_global_int_array_value3(t_CKINT cb_id, t_CKINT value);
void cb_global_float_array1(t_CKFLOAT array[], t_CKUINT n);
void cb_global_float_array2(const char* name, t_CKFLOAT array[], t_CKUINT n);
void cb_global_float_array3(t_CKINT cb_id, t_CKFLOAT array[], t_CKUINT n);
void cb_global_float_array_value1(t_CKFLOAT value);
void cb_global_float_array_value2(const char* name, t_CKFLOAT value);
void cb_global_float_array_value3(t_CKINT cb_id, t_CKFLOAT value);
void cb_global_assoc_int_array_value1(t_CKINT val);
void cb_global_assoc_int_array_value2(const char* name, t_CKINT val);
void cb_global_assoc_int_array_value3(t_CKINT cb_id, t_CKINT val);
void cb_global_assoc_float_array_value1(t_CKFLOAT val);
void cb_global_assoc_float_array_value2(const char* name, t_CKFLOAT val);
void cb_global_assoc_float_array_value3(t_CKINT cb_id, t_CKFLOAT val);

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

    class_addmethod(c, (method)ck_add,          "add",      A_GIMME, 0);
    class_addmethod(c, (method)ck_remove,       "remove",   A_GIMME, 0);
    class_addmethod(c, (method)ck_replace,      "replace",  A_GIMME, 0);
    class_addmethod(c, (method)ck_clear,        "clear",    A_GIMME, 0);
    class_addmethod(c, (method)ck_reset,        "reset",    A_GIMME, 0); // reset -> 'clear vm', 'reset id' 
    class_addmethod(c, (method)ck_status,       "status",   0);
    class_addmethod(c, (method)ck_time,         "time",     0);

    class_addmethod(c, (method)ck_demo,         "demo",     0);
 
    class_addmethod(c, (method)ck_vm,           "vm",       0);
    class_addmethod(c, (method)ck_globals,      "globals",  0);
    class_addmethod(c, (method)ck_docs,         "docs",     0);
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
        x->run_needs_audio = 0;

        t_symbol* filename;

        if (argc == 1) {
            if (argv->a_type == A_LONG) {
                atom_arg_getlong((t_atom_long*)&x->channels, 0, argc, argv);
            } else if (argv->a_type == A_SYM) {
                filename = atom_getsymarg(0, argc, argv);
                x->run_file = ck_check_file(x, filename);
            } else {
                error("invalid object arguments");
                return;
            }
        }
        else if (argc >= 2) {
            atom_arg_getlong((t_atom_long*)&x->channels, 0, argc, argv);  // is 1st arg of object
            filename = atom_getsymarg(1, argc, argv);    // is 2nd arg of object
            x->run_file = ck_check_file(x, filename);
        } 
        // else just use defaults

        // get external editor
        if (const char* editor = std::getenv("EDITOR")) {
            post("editor from env: %s", editor);
            x->editor = gensym(editor);
        } else {
            x->editor = gensym("");
        }

        // set patcher object
        object_obex_lookup(x, gensym("#P"), (t_patcher**)&x->patcher);
        if (x->patcher == NULL)
            error("patcher object not created.");

        // set box object
        object_obex_lookup(x, gensym("#B"), (t_box**)&x->box);
        if (x->box == NULL)
            error("box object not created.");

        // create scripting name
        char name[MAX_FILENAME_CHARS];
        int res = snprintf_zero(name, MAX_FILENAME_CHARS, "chuck-%d", x->oid);
        if (res >= 0 && res < MAX_FILENAME_CHARS) {
            x->name = gensym(name);
        } else {
            x->name = symbol_unique(); // fallback to unique symbol name
        }
        t_max_err err = jbox_set_varname(x->box, x->name);
        if (err != MAX_ERR_NONE) {
            error("could not set chuck~ box's scripting name");
        }

        // get patcher directory
        // t_symbol *p_name = object_attr_getsym(x->patcher, gensym("name"));
        t_symbol *p_path = object_attr_getsym(x->patcher, gensym("filepath"));
        // post("name of patcher: %s", p_name->s_name);
        // post("path of patcher: %s", p_path->s_name);
        char patcher_dir[MAX_FILENAME_CHARS];
        char patcher_file[MAX_PATH_CHARS];
        char patcher_conform_dir[MAX_PATH_CHARS];
        path_splitnames(p_path->s_name, patcher_dir, patcher_file);
        // post("dir of patcher: %s", patcher_dir);
        path_nameconform(patcher_dir, patcher_conform_dir, PATH_STYLE_MAX, PATH_TYPE_BOOT);
        // post("conform dir of patcher: %s", patcher_conform_dir);
        x->patcher_dir = gensym(patcher_conform_dir);

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
            error("critical: could not create chuck object");
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

        // object id corresponds to order of object creation
        x->oid = ++MX_CK_COUNT;

        // init cb_map and register callbacks
        x->cb_map = callback_map();
        x->cb_map.emplace("demo", &cb_global_event1);

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

std::vector<std::string> split(std::string s, char delimiter)
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

//-----------------------------------------------------------------------------------------------
// attribute set/get

t_max_err ck_editor_set(t_ck *x, t_object *attr, long argc, t_atom *argv)
{
    t_symbol* editor = atom_getsym(argv);
    post("editor_set: %s", editor->s_name);
    ck_check_editor(x, editor);
    // x->editor = editor;
    return MAX_ERR_NONE;
}

t_max_err ck_editor_get(t_ck *x, t_object *attr, long *argc, t_atom **argv)
{   
    char alloc;
    atom_alloc(argc, argv, &alloc); 
    post("editor_get: %s", x->editor->s_name);
    t_max_err err =  atom_setsym(*argv, x->editor);
    if (err != MAX_ERR_NONE) {
        error("failed: ck_editor_get");
    }
    return err;
}


//-----------------------------------------------------------------------------------------------
// helpers

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
    post("last_id: %d", id);
    return id;
}

long ck_spork_next_id(t_ck* x)
{
    long id = x->chuck->vm()->next_id();
    post("next_id: %d", id);
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
        error("could not get max object");
        return MAX_ERR_GENERIC;
    }
    t_max_err err = object_method_parse(maxobj, s, parsestr, NULL);
    if (err != MAX_ERR_NONE) {
        error("could not send msg: ;max %s %s", s->s_name, parsestr);
        return err;
    }
    return MAX_ERR_NONE;
}


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

t_max_err ck_check_editor(t_ck* x, t_symbol* entry)
{
    if (path_exists(entry->s_name)) {
        x->editor = entry;
        return MAX_ERR_NONE;
    }
    return MAX_ERR_NONE;
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
        // post("patcher_file: %s", patcher_file);
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
        error("ck_check_file: locatefile_extended failed");
        return gensym("");

    err = path_toabsolutesystempath(path, filepath, abspath);
    if (err != MAX_ERR_NONE)
        error("ck_check_file: path_toabsolutesystempath failed");
        return gensym("");

    normpath[0] = '\0'; // erase it to re-use it

    path_nameconform(abspath, normpath, PATH_STYLE_MAX, PATH_TYPE_BOOT);

    if (path_exists(normpath)) {
        return gensym(normpath);
    }

    error("ck_check_file: could not locate %s", name->s_name);
    return gensym("");
}

t_max_err ck_compile_code(t_ck* x, const char* code, const char* args)
{
    std::vector<t_CKUINT> vec;
    if (!x->chuck->compileCode(std::string(code), std::string(args), 1, FALSE, &vec)) {
        error("could not compile code: %s", code);
        return MAX_ERR_GENERIC;
    }
    return MAX_ERR_NONE;
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
    if (x->run_file != gensym("")) {
        return ck_compile_file(x, x->run_file->s_name);
    }
    error("ck_run_file: filename slot is empty");
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
        if (x->run_needs_audio && !sys_getdspstate()) {
            error("can only run/add shred when audio is on");
            return MAX_ERR_GENERIC;
        }
        x->run_file = ck_check_file(x, s);
        return ck_run_file(x);
    }
    error("ck_run: reguires a filename to edit");
    return MAX_ERR_GENERIC;
}


t_max_err ck_edit(t_ck* x, t_symbol* s)
{
    if (x->editor == gensym("")) {
        error("ck_edit: editor attribute or EDITOR env var not set to full path of editor");
        return MAX_ERR_GENERIC;
    }

    if (s != gensym("")) {
        x->edit_file = ck_check_file(x, s);
        if (x->edit_file != gensym("")) {
            std::string cmd;
            // post("edit: %s", x->edit_file->s_name);

            cmd = std::string(x->editor->s_name) + " " + std::string(x->edit_file->s_name);
            std::system(cmd.c_str());
            return MAX_ERR_NONE;
        }
    }
    error("ck_edit: reguires a valid filename");
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

    if (argc < 1) {
        error("add message needs at least one <filename> argument");
        return MAX_ERR_GENERIC;
    }

    if ((argv)->a_type != A_SYM) {
        error("first argument must be a symbol");
        return MAX_ERR_GENERIC;
    }

    if (x->run_needs_audio && !sys_getdspstate()) {
        error("can only run/add shred when audio is on");
        return MAX_ERR_GENERIC;
    };

    filename_sym = atom_getsym(argv);

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
        error("could not add file");
        return MAX_ERR_GENERIC;
    }
    
    std::string full_path = std::string(checked_file->s_name);

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

        // handle one arg case
        msg->reply_cb = (ck_msg_func)0;
        x->chuck->vm()->queue_msg(msg, 1);
        return MAX_ERR_NONE;

    } else {
        // assume message is along :-) the lines of (remove 2 4 1 [..])
        long* long_array = (long*)sysmem_newptr(sizeof(long*) * argc);
        t_max_err err = atom_getlong_array(argc, argv, argc, long_array);
        if (err != MAX_ERR_NONE) {
            error("remove msg: multiple args can only be ints");
            return err;
        }
        for (int i = 0; i < argc; i++) {
            // post("removing: long_array[%d] = %d", i, long_array[i]);
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

    t_symbol* checked_file = ck_check_file(x, gensym(filename.c_str()));

    if (checked_file == gensym("")) {
        error("could not replace file");
        return MAX_ERR_GENERIC;
    }
    
    std::string full_path = std::string(checked_file->s_name);

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
                post("[chuck]: clean up global variables without clearing the whole VM");
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
                // post("i: %d -> %d ", i, atom_getlong(argv + i));
                long_array[i] = atom_getlong(argv + i);
            }
            x->chuck->vm()->globals_manager()->setGlobalIntArray(
                s->s_name, long_array, argc);
            sysmem_freeptr(long_array);
        }

        else if (argv->a_type == A_FLOAT) { // list of doubles
            double* float_array = (double*)sysmem_newptr(sizeof(double*) * argc);
            for (int i = 0; i < argc; i++) {
                // post("i: %d -> %d ", i, atom_getfloat(argv + i));
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

t_max_err ck_docs(t_ck* x)
{
    ck_send_max_msg(x, gensym("launchbrowser"), "https://chuck.stanford.edu/doc");
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


t_max_err ck_globals(t_ck* x)
{
    if (x->chuck->vm()->globals_manager()->getAllGlobalVariables(cb_get_all_global_vars, NULL)) {
        return MAX_ERR_NONE;
    }
    error("could not dump global variable to console");
    return MAX_ERR_GENERIC;
}


t_max_err ck_vm(t_ck* x)
{
    post("VM %d status", x->oid);
    post("\tinitialized: %d", x->chuck->vm()->has_init());
    post("\trunning: %d", x->chuck->vm()->running());
    return MAX_ERR_NONE;
}


//-----------------------------------------------------------------------------------------------
// callbacks

/* nothing useful here yet just minimal demos */

void cb_global_int1(t_CKINT val)
{
    post("cb_global_int1: %d", val);
}

void cb_global_int2(const char* name, t_CKINT val)
{
     post("cb_global_int2: name: %s value: %d", name, val);
}

void cb_global_int3(t_CKINT cb_id, t_CKINT val)
{
     post("cb_global_int3: id: %d value: %d", cb_id, val);
}

void cb_global_float1(t_CKFLOAT val)
{
    post("cb_global_float1: %f", val);
}

void cb_global_float2(const char* name, t_CKFLOAT val)
{
     post("cb_global_float2: name: %s value: %f", name, val);
}

void cb_global_float3(t_CKINT cb_id, t_CKFLOAT val)
{
     post("cb_global_float3: id: %d value: %f", cb_id, val);
}

void cb_global_string1(const char* val)
{
    post("cb_global_string1: %s", val);
}

void cb_global_string2(const char* name, const char* val)
{
     post("cb_global_string2: name: %s value: %s", name, val);
}

void cb_global_string3(t_CKINT cb_id, const char* val)
{
     post("cb_global_string3: id: %d value: %s", cb_id, val);
}


void cb_global_event1(void)
{
    post("cb_global_event1");
}

void cb_global_event2(const char* name)
{
    post("cb_global_event2: %s", name);
}

void cb_global_event3(t_CKINT cb_id)
{
    post("cb_global_event3: id: %d", cb_id);
}

void cb_global_int_array1(t_CKINT array[], t_CKUINT n)
{
    post("cb_global_int_array1: %d", n);
    for (int i = 0; i < n; i++) {
        post("array[%d] = %d", i, array[i]);
    }
}

void cb_global_int_array2(const char* name, t_CKINT array[], t_CKUINT n)
{
    post("cb_global_int_array2: name: %s size: %d", name, n);
    for (int i = 0; i < n; i++) {
        post("array[%d] = %d", i, array[i]);
    }
}

void cb_global_int_array3(t_CKINT cb_id, t_CKINT array[], t_CKUINT n)
{
    post("cb_global_int_array3: cb_id: %d size: %d", cb_id, n);
    for (int i = 0; i < n; i++) {
        post("array[%d] = %d", i, array[i]);
    }
}

void cb_global_int_array_value1(t_CKINT value)
{
    post("cb_global_int_array_value1: value: %d", value);
}

void cb_global_int_array_value2(const char* name, t_CKINT value)
{
    post("cb_global_int_array_value2: name: %s value: %d", name, value);
}

void cb_global_int_array_value3(t_CKINT cb_id, t_CKINT value)
{
    post("cb_global_int_array_value3: cb_id: %d value: %d", cb_id, value);
}

void cb_global_float_array1(t_CKFLOAT array[], t_CKUINT n)
{
    post("cb_global_float_array1: %d", n);
    for (int i = 0; i < n; i++) {
        post("array[%d] = %d", i, array[i]);
    }
}

void cb_global_float_array2(const char* name, t_CKFLOAT array[], t_CKUINT n)
{
    post("cb_global_float_array2: name: %s size: %d", name, n);
    for (int i = 0; i < n; i++) {
        post("array[%d] = %d", i, array[i]);
    }
}

void cb_global_float_array3(t_CKINT cb_id, t_CKFLOAT array[], t_CKUINT n)
{
    post("cb_global_float_array3: cb_id: %d size: %d", cb_id, n);
    for (int i = 0; i < n; i++) {
        post("array[%d] = %d", i, array[i]);
    }
}

void cb_global_float_array_value1(t_CKFLOAT value)
{
    post("cb_global_float_array_value1: value: %d", value);
}

void cb_global_float_array_value2(const char* name, t_CKFLOAT value)
{
    post("cb_global_float_array_value2: name: %s value: %d", name, value);
}

void cb_global_float_array_value3(t_CKINT cb_id, t_CKFLOAT value)
{
    post("cb_global_float_array_value3: cb_id: %d value: %d", cb_id, value);
}

void cb_get_all_global_vars(const std::vector<Chuck_Globals_TypeValue> & list, void * data)
{
    post("cb_get_all_global_vars:");
    for (auto v : list) {
        post("type: %s name: %s", v.type.c_str(), v.name.c_str());
    }
}


void cb_global_assoc_int_array_value1(t_CKINT val)
{
    post("cb_global_assoc_int_array_value1: %d", val);
}

void cb_global_assoc_int_array_value2(const char* name, t_CKINT val)
{
     post("cb_global_assoc_int_array_value2: name: %s value: %d", name, val);
}

void cb_global_assoc_int_array_value3(t_CKINT cb_id, t_CKINT val)
{
     post("cb_global_assoc_int_array_value3: id: %d value: %d", cb_id, val);
}

void cb_global_assoc_float_array_value1(t_CKFLOAT val)
{
    post("cb_global_assoc_float_array_value1: %f", val);
}

void cb_global_assoc_float_array_value2(const char* name, t_CKFLOAT val)
{
     post("cb_global_assoc_float_array_value2: name: %s value: %f", name, val);
}

void cb_global_assoc_float_array_value3(t_CKINT cb_id, t_CKFLOAT val)
{
     post("cb_global_assoc_float_array_value3: id: %d value: %f", cb_id, val);
}

//-----------------------------------------------------------------------------------------------
// callback handlers


t_max_err ck_register(t_ck* x, t_symbol* s)
{
    if (!x->cb_map.count(s->s_name)) {
        error("event/callback not found: %s", s->s_name);
        return MAX_ERR_GENERIC;
    }
    std::string key = std::string(s->s_name);
    ck_callback cb = x->cb_map[key];
    // false: for a one off call, strue: called everytime it is called
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

// t_max_err ck_demo_gevents(t_ck* x)
// {
//     x->chuck->vm()->globals_manager()->listenForGlobalEvent("gevent", cb_global_event1, false);
//     x->chuck->vm()->globals_manager()->listenForGlobalEvent("gevent", cb_global_event2, false);
//     x->chuck->vm()->globals_manager()->listenForGlobalEvent("gevent", 3, cb_global_event3, false);
//     x->chuck->vm()->globals_manager()->stopListeningForGlobalEvent( const char * name, void (*callback)(void) );
//     x->chuck->vm()->globals_manager()->stopListeningForGlobalEvent( const char * name, void (*callback)(const char*) );
//     x->chuck->vm()->globals_manager()->stopListeningForGlobalEvent( const char * name, t_CKINT callbackID, void (*callback)(t_CKINT) );
// }

// t_max_err ck_demo_set_global_arrays(t_ck* x)
// {
//     x->chuck->vm()->globals_manager()->setGlobalIntArray( const char * name, t_CKINT arrayValues[], t_CKUINT numValues );
//     x->chuck->vm()->globals_manager()->setGlobalIntArrayValue( const char * name, t_CKUINT index, t_CKINT value );
//     x->chuck->vm()->globals_manager()->setGlobalAssociativeIntArrayValue( const char * name, const char * key, t_CKINT value );
//     x->chuck->vm()->globals_manager()->setGlobalFloatArray( const char * name, t_CKFLOAT arrayValues[], t_CKUINT numValues );
//     x->chuck->vm()->globals_manager()->setGlobalFloatArrayValue( const char * name, t_CKUINT index, t_CKFLOAT value );
//     x->chuck->vm()->globals_manager()->setGlobalAssociativeFloatArrayValue( const char * name, const char * key, t_CKFLOAT value );
// }


t_max_err ck_demo(t_ck* x)
{
    // ck_spork_last_id(x);
    // ck_spork_next_id(x);

    x->chuck->vm()->globals_manager()->getGlobalInt("gint", cb_global_int1);
    x->chuck->vm()->globals_manager()->getGlobalInt("gint", cb_global_int2);
    x->chuck->vm()->globals_manager()->getGlobalInt("gint", 3, cb_global_int3);

    x->chuck->vm()->globals_manager()->getGlobalFloat("gfloat", cb_global_float1);
    x->chuck->vm()->globals_manager()->getGlobalFloat("gfloat", cb_global_float2);
    x->chuck->vm()->globals_manager()->getGlobalFloat("gfloat", 3, cb_global_float3);

    x->chuck->vm()->globals_manager()->getGlobalString("gstring", cb_global_string1);
    x->chuck->vm()->globals_manager()->getGlobalString("gstring", cb_global_string2);
    x->chuck->vm()->globals_manager()->getGlobalString("gstring", 3, cb_global_string3);

    // x->chuck->vm()->globals_manager()->getGlobalUGenSamples( const char * name, SAMPLE* buffer, int numFrames );

    x->chuck->vm()->globals_manager()->getGlobalIntArray("gints", cb_global_int_array1);
    x->chuck->vm()->globals_manager()->getGlobalIntArray("gints", cb_global_int_array2);
    x->chuck->vm()->globals_manager()->getGlobalIntArray("gints", 3, cb_global_int_array3);

    x->chuck->vm()->globals_manager()->getGlobalIntArrayValue("gints", 0, cb_global_int_array_value1);
    x->chuck->vm()->globals_manager()->getGlobalIntArrayValue("gints", 0, cb_global_int_array_value2);
    x->chuck->vm()->globals_manager()->getGlobalIntArrayValue("gints", 3, 0, cb_global_int_array_value3);

    x->chuck->vm()->globals_manager()->getGlobalAssociativeIntArrayValue("imap", "a", cb_global_assoc_int_array_value1);
    x->chuck->vm()->globals_manager()->getGlobalAssociativeIntArrayValue("imap", "a", cb_global_assoc_int_array_value2);
    x->chuck->vm()->globals_manager()->getGlobalAssociativeIntArrayValue("imap", 3, "a", cb_global_assoc_int_array_value3);

    x->chuck->vm()->globals_manager()->getGlobalFloatArray("gfloats", cb_global_float_array1);
    x->chuck->vm()->globals_manager()->getGlobalFloatArray("gfloats", cb_global_float_array2);
    x->chuck->vm()->globals_manager()->getGlobalFloatArray("gfloats", 3, cb_global_float_array3);

    x->chuck->vm()->globals_manager()->getGlobalFloatArrayValue("gfloats", 0, cb_global_float_array_value1);
    x->chuck->vm()->globals_manager()->getGlobalFloatArrayValue("gfloats", 0, cb_global_float_array_value2);
    x->chuck->vm()->globals_manager()->getGlobalFloatArrayValue("gfloats", 3, 0, cb_global_float_array_value3);

    x->chuck->vm()->globals_manager()->getGlobalAssociativeFloatArrayValue("fmap", "a", cb_global_assoc_float_array_value1);
    x->chuck->vm()->globals_manager()->getGlobalAssociativeFloatArrayValue("fmap", "a", cb_global_assoc_float_array_value2);
    x->chuck->vm()->globals_manager()->getGlobalAssociativeFloatArrayValue("fmap", 3, "a", cb_global_assoc_float_array_value3);

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
