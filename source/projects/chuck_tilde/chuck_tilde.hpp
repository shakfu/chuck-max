/* chuck_tilde.hpp */

#pragma once

#ifndef CHUCK_TILDE_H
#define CHUCK_TILDE_H

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
#define CHANNELS 1
#define EMBEDDED_CHUGINS 0


// typedefs
typedef void (*ck_callback)(void);
typedef std::unordered_map<std::string, ck_callback> callback_map;

// data structures
typedef struct t_ck t_ck;

// method prototypes
void* ck_new(t_symbol* s, long argc, t_atom* argv);
void ck_free(t_ck* x);
void ck_assist(t_ck* x, void* b, long m, long a, char* s);

// attribute set/get
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
t_max_err ck_globals(t_ck* x);                  // dump global variabls
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

// callback demo
t_max_err ck_demo(t_ck* x);

// callbacks
void cb_demo(void);
void cb_global_int1(t_CKINT val);
void cb_global_int2(const char* name, t_CKINT val);
void cb_global_int3(t_CKINT cb_id, t_CKINT val);
void cb_global_float1(t_CKFLOAT val);
void cb_global_float2(const char* name, t_CKFLOAT val);
void cb_global_float3(t_CKINT cb_id, t_CKFLOAT val);
void cb_global_string1(const char* val);
void cb_global_string2(const char* name, const char* val);
void cb_global_string3(t_CKINT cb_id, const char* val);



#endif // CHUCK_TILDE_H
