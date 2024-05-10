# Dev Notes

## Alternative to std::system

Use `ext_sysprocess`

```diff
#include "ext_sysprocess.h"

@@ -789,11 +789,11 @@ t_max_err ck_edit(t_ck* x, t_symbol* s)
     if (s != gensym("")) {
         x->edit_file = ck_check_file(x, s);
         if (x->edit_file != gensym("")) {
-            std::string cmd;
-            // post("edit: %s", x->edit_file->s_name);
-
-            cmd = std::string(x->editor->s_name) + " " + std::string(x->edit_fi
le->s_name);
-            std::system(cmd.c_str());
+            if (sysprocess_launch(x->editor->s_name, x->edit_file->s_name) == 0
) {
+                error("could not open %s with editor %s",
+                    x->edit_file->s_name, x->editor->s_name);
+                return MAX_ERR_GENERIC;
+            }
             return MAX_ERR_NONE;
         }
```



## Registering and Binding Different Callbacks

Sidestepping limitations of callbacks (see [here](https://isocpp.org/wiki/faq/pointers-to-members) and [here](https://stackoverflow.com/questions/25848690/should-i-use-stdfunction-or-a-function-pointer-in-c)) especially due to lack of capture of context variables, their use is limited in Max as they don't have access to the struct pointer unless it is stored in a global pointer..

The following provides for more than one family of callbacks through the use of maps of function pointers. Probably abandon it due to above limitation.

To have more than 1 callback instance which can be retrieved by name from a map:

```c++

// typedefs
typedef void (*t_cb_event)(const char*);
typedef void (*t_cb_int)(const char*, t_CKINT);
typedef void (*t_cb_float)(const char*, t_CKFLOAT);
typedef void (*t_cb_string)(const char*, const char*);
typedef void (*t_cb_int_array)(const char*, t_CKINT[], t_CKUINT);
typedef void (*t_cb_float_array)(const char*, t_CKFLOAT[], t_CKUINT);

typedef std::unordered_map<std::string, t_cb_event> t_map_cb_event;
typedef std::unordered_map<std::string, t_cb_int> t_map_cb_int;
typedef std::unordered_map<std::string, t_cb_float> t_map_cb_float;
typedef std::unordered_map<std::string, t_cb_string> t_map_cb_string;
typedef std::unordered_map<std::string, t_cb_int_array> t_map_cb_int_array;
typedef std::unordered_map<std::string, t_cb_float_array> t_map_cb_float_array;


x->default_cb = gensym("default");

// init callback maps
x->map_cb_event = t_map_cb_event();
x->map_cb_int = t_map_cb_int();
x->map_cb_float = t_map_cb_float();
x->map_cb_string = t_map_cb_string();
x->map_cb_int_array = t_map_cb_int_array();
x->map_cb_float_array = t_map_cb_float_array();
// ? add others

// register callbacks
x->map_cb_event.emplace("default", &cb_event);
x->map_cb_int.emplace("default", &cb_get_int);
x->map_cb_float.emplace("default", &cb_get_float);
x->map_cb_string.emplace("default", &cb_get_string);
x->map_cb_int_array.emplace("default", &cb_get_int_array);
x->map_cb_float_array.emplace("default", &cb_get_float_array);
// ? add others

// then getting or using a fpointer is a matter of lookup on 'default' or other.

t_max_err ck_register(t_ck* x, t_symbol* s, long listen_forever)
{
    if (!x->map_cb_event.count(s->s_name)) {
        error("event/callback not found: %s", s->s_name);
        return MAX_ERR_GENERIC;
    }
    std::string key = std::string(s->s_name);
    t_cb_event cb = x->map_cb_event[key];
    // false: for a one off call, strue: called everytime it is called
    if (x->chuck->vm()->globals_manager()->listenForGlobalEvent(s->s_name, cb, (bool)listen_forever)) {
        post("%s event/callback registered", s->s_name);
        return MAX_ERR_NONE;
    };
    return MAX_ERR_GENERIC;
}

t_max_err ck_unregister(t_ck* x, t_symbol* s)
{
    if (!x->map_cb_event.count(s->s_name)) {
        error("event/callback not found: %s", s->s_name);
        return MAX_ERR_GENERIC;
    }
    std::string key = std::string(s->s_name);
    t_cb_event cb = x->map_cb_event[key];
    if (x->chuck->vm()->globals_manager()->stopListeningForGlobalEvent(s->s_name, cb)) {
        post("%s event/callback unregistered", s->s_name);
        return MAX_ERR_NONE;
    };
    return MAX_ERR_GENERIC;
}

```



## Using Max code editor


```c++

// struct
t_object* code_editor;  // code editor object
char** code;            // handle to code buffer for code editor
long code_size;         // length of code buffer
t_fourcc code_filetype; // filetype four char code of 'TEXT'
t_fourcc code_outtype;  // savetype four char code of 'TEXT'
char code_filename[MAX_PATH_CHARS]; // file name field
char code_pathname[MAX_PATH_CHARS]; // file path field
short code_path;   // short code for max file system
long run_on_save;  // evaluate/run code in editor on save
long run_on_close; // evaluate/run code in editor on close



void ck_dblclick(t_ck* x)
{
    if ((x->run_file != gensym("")) && (x->editor != gensym(""))) {
        ck_edit(x, x->run_file);
    } else if (x->code_editor) {
        object_attr_setchar(x->code_editor, gensym("visible"), 1);
    } else {
        x->code_editor = (t_object*)object_new(CLASS_NOBOX, gensym("jed"), x, 0);
        object_method(x->code_editor, gensym("settext"), *x->code, gensym("utf-8"));
        object_attr_setchar(x->code_editor, gensym("scratch"), 1);
        object_attr_setsym(x->code_editor, gensym("title"), gensym("ck-editor"));
    }
}

// etc.

```


## How to Sync Chuck <=> Max

- `sync~`
- `rate~` also see [this article](https://cycling74.com/tutorials/my-favorite-object-rate~#reply-591c77941db19e747db0ca8f)
- [sync with gen~](https://cycling74.com/forums/question-about-a-patch-in-gen-book-generating-sound-and-organizing-time#reply-6615705ccb02500013795117)
- [Counting, and a World without bang Messages](https://cycling74.com/tutorials/gen~-for-beginners-part-3-counting-and-a-world-without-bang-messages)

Possible to use alternative (active transport) notion? 

```text
1nd - Dotted whole note - 2880 ticks
1n - Whole note - 1920 ticks
1nt - Whole note triplet - 1280 ticks
2nd - Dotted half note - 1440 ticks
2n - Half note - 960 ticks
2nt - Half note triplet - 640 ticks
4nd - Dotted quarter note - 720 ticks
4n - Quarter note - 480 ticks
4nt - Quarter note triplet - 320 ticks
8nd - Dotted eighth note - 360 ticks
8n - Eighth note - 240 ticks
8nt - Eighth note triplet - 160 ticks
16nd - Dotted sixteenth note - 180 ticks
16n - Sixteenth note - 120 ticks
16nt - Sixteenth note triplet - 80 ticks
32nd - Dotted thirty-second note - 90 ticks
32n - thirty-second note - 60 ticks
32nt - thirty-second-note triplet - 40 ticks
64nd - Dotted sixty-fourth note - 45 ticks
64n - Sixty-fourth note - 30 ticks
128n - One-hundred-twenty-eighth note - 15 ticks


umenu:
1nd, 1n, 1nt, 2nd, 2n, 2nt, 4nd, 4n, 4nt, 8nd, 8n, 8nt, 16nd, 16n, 16nt, 32nd, 32n, 32nt, 64nd, 64n, 128n 
```



## Sending Messages to Maxobject

```c++
t_max_err ck_send_max_msg(t_ck* x, t_symbol* s, const char* parsestr)
{
    t_max_err err;

    t_object *maxobj = (t_object*)object_new(CLASS_NOBOX, gensym("max"));
    if (maxobj == NULL) {
        error("could not get max object");
    }
    err = object_method_parse(maxobj, s, parsestr, NULL);
    if (err != MAX_ERR_NONE) {
        error("could not send msg: ;max %s %s", s->s_name, parsestr);
        return err;
    }
}
```

## Getting a default external editor

It seemed to be a bit difficult to retrieve the path of the default external editor.
So if  `x->editor` is also an attribute then it should be possible set as follows:

```c++
if (const char* editor = std::getenv("EDITOR")) {                   // 1
    post("editor: %s", editor);
    x->editor = gensym(editor);
} else if (x->editor = preferences_getsym("externaleditor")) {      // 2
    post("editor: %s", x->editor->s_name);
} else {
    x->editor = gensym("");
}
```

(1) didn't initially work, but works after a restart or intermittently? and (2) retrieves only the stem of the path of the configured executable in which `locatefile_extended` doesn't work.

```c++
t_max_err ck_edit(t_ck* x, t_symbol* s)
{
    if (x->editor == gensym("")) {
        error("editor attribute or EDITOR env var not set");
        return MAX_ERR_GENERIC;
    }

    if (s != gensym("")) {
        char conform_path[MAX_PATH_CHARS];
        std::string cmd;

        path_nameconform(s->s_name, conform_path, PATH_STYLE_MAX, PATH_TYPE_BOOT);
        post("edit: %s", conform_path);
        if (x->editor_from_prefs) {
            post("open editor: %s", x->editor->s_name);
            cmd = std::string("/usr/bin/open -a '") + std::string(x->editor->s_name) + "' " + std::string(conform_path);
        } else {
            post("exec editor: %s", x->editor->s_name);
            cmd = std::string(x->editor->s_name) + " " + std::string(conform_path);
            std::system(cmd.c_str());
        }
        return MAX_ERR_NONE;
    }
    error("ck_edit: reguires a filename");
    return MAX_ERR_GENERIC;
}
```

also see: [get-path-of-executable](https://stackoverflow.com/questions/1528298/get-path-of-executable)

## Using chuck api more efficiently

The chuck shell in `core/chuck_shell.h` has a very nice interface but is a repl (`chuck --shell` from the commandline). The `core/lib_machine.h` api is also quite nice, but is meant to be called from chuck code. Chuck shell uses the `core/chuck_otf.h` functionality. Maybe easier to hook into one or more of these apis instead or re-implementing the wheel.

see:

- chuck shell[paper](https://quod.lib.umich.edu/cgi/p/pod/dod-idx/miniaudicle-and-chuck-shell-new-interfaces-for-chuck.pdf?c=icmc;idno=bbp2372.2006.016;format=pdf)

- chuck floss [manual](https://archive.flossmanuals.net/chuck/ch018_on-the-fly-commands.html)

- chuck [versions changelog](https://chuck.stanford.edu/release/VERSIONS)

```text
    implemented Chuck_Msg_Type(s)

    [x] CK_MSG_ADD
    [x] CK_MSG_REMOVE
    [x] CK_MSG_REMOVEALL
    [x] CK_MSG_REPLACE
    [x] CK_MSG_STATUS
    [-] CK_MSG_PAUSE            OTF only
    [-] CK_MSG_EXIT             Not applicable
    [x] CK_MSG_TIME
    [x] CK_MSG_RESET_ID
    [-] CK_MSG_DONE             OTF only
    [-] CK_MSG_ABORT            OTF only
    [-] CK_MSG_ERROR            Not implemented
    [x] CK_MSG_CLEARVM
    [x] CK_MSG_CLEARGLOBALS


    TODO:

    [x] add <path> <args>
    [x] add <code>

    [x] replace <shredID> <path> <args>
    [ ] replace <shredID> <withShredID>
    [ ] replace <shredID> <code>
```

## Fauck and WarpBuf chugins

Fauck chugin can be deployed in two ways for macOS:

1. Default is via a macos bundle with the  faust `stdlib` included inside the bundle

2. As a non-bundled module with `chugins` and faust `stdlib` in the `examples` folder

Preference in this case is for (2) since it is useful to have direct access to chugins and stdlib (and related examples). The package format is more natural for Max/MSP.

Converting (1) to (2) was a major pain because the faust dependency needs to be git cloned as branch `2.69.3`, if current main was cloned it would. segfault inexplicably at the `compute` function.

## Converting chugins to cmake builds

This has been pretty straightforward, especially as the project is currently only targeting one platform (macOS).

However, static compilation and integration of chugins is not working although this is indicated as possible in the makefiles: static chugins were not recognized when invoked via test scripts.

## Why isn't chuck a git submodule?

The directories `chuck/src/core`, `chuck/src/host`, `chuck/src/host-embed` are directly included for the sake of practicality in the `chuck-max/source/projects/chuck~` folder. Typically the `chuck` repo would have been included as an external dependency and downloaded and built when required.

The prior reason was that the `chuck` repo until recently, even at `--depth=1`, required more than 1 GB of storage and could take ages to download.

With this problem solved, a script (`source/scripts/update.sh`) is preferred to including chuck and chugins directories as git submodules.

## Audio Processing Block Evolution

The initial audio processing algorithm was pretty much taken from David Braun's [ChucKDesigner](https://github.com/DBraun/ChucKDesigner) `processBlock` function in [Plugin_ChucK.cpp](https://github.com/DBraun/ChucKDesigner/blob/main/src/Plugin_ChucK.cpp)

```c++
CHUCKDESIGNERSHARED_API bool processBlock(unsigned int chuckID, const float** inBuffer, int inBufferNumChannels, int inBufferNumSamples, float* inChucKBuffer, float* outChucKBuffer, float** outBuffer, int numOutSamples, int numOutChannels)
{

    if (chuck_instances.count(chuckID) == 0) {
        return false;
    }
    ChucK* chuck = chuck_instances[chuckID];

    int numOutChans = chuck->vm()->m_num_dac_channels;
    if (numOutChans != numOutChannels) {
        return false;
    }

    int numSamples;
    int numInChannels = std::min<int>(inBufferNumChannels, (int)chuck->vm()->m_num_adc_channels);

    for (int i = 0; i < numOutSamples; i += CHUCKDESIGNERCHOP_BUFFER_SIZE) {

        // chuck->run(inbuffer, *output->channels, output->numSamples); // this doesn't work because of interleaved samples.
        // Chuck returns LRLRLRLR but for touchdesigner we want LLLLRRRR.
        // Therefore we must use an intermediate buffer
        float* inPtr = inChucKBuffer;

        numSamples = min(CHUCKDESIGNERCHOP_BUFFER_SIZE, numOutSamples - i);

        if (inBuffer) {
            for (int samp = i; samp < std::min<int>(inBufferNumSamples, i + CHUCKDESIGNERCHOP_BUFFER_SIZE); samp++) {
                for (int chan = 0; chan < numInChannels; chan++) {
                    *(inPtr++) = inBuffer[chan][samp];
                }
            }
        }
        float* outPtr = outChucKBuffer;

        chuck->run(inChucKBuffer, outChucKBuffer, numSamples);

        for (int samp = 0; samp < numSamples; samp++) {
            for (int chan = 0; chan < numOutChans; chan++) {
                outBuffer[chan][i + samp] = *outPtr++;
            }
        }

    }

    return true;
}
```

This was initially changed to this:

```c++
void ck_perform64(t_ck *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
    int n = sampleframes; // n = 64
    int numSamples;

    int numOutSamples = n;
    int inBufferNumSamples = n;

    for (int i = 0; i < numOutSamples; i += BUFFER_SIZE) {

        float* inPtr = x->in_chuck_buffer;

        numSamples = min(BUFFER_SIZE, numOutSamples - i);

        if (ins) {
            for (int samp = i; samp < std::min<int>(inBufferNumSamples, i + BUFFER_SIZE); samp++) {
                for (int chan = 0; chan < N_IN_CHANNELS; chan++) {
                    *(inPtr++) = ins[chan][samp];
                }
            }
        }
        float* outPtr = x->out_chuck_buffer;

        x->chuck->run(x->in_chuck_buffer, x->out_chuck_buffer, numSamples);

        for (int samp = 0; samp < numSamples; samp++) {
            for (int chan = 0; chan < N_OUT_CHANNELS; chan++) {
                outs[chan][i + samp] = *outPtr++;
            }
        }

    }
}
```

and finally, it was simplified to the following with the help of Professor GE Wang

```c++
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
```

## Parameters via the `global` keyword

### How to set default global values?

As a general rule, if the chuck code will not run in chuck, then it will not run in chuck-max.

So global parameters should be initialized in the chuck script as normal and chuck-max will change the value of initialization on opening. It practice, this is a non-issue.

### From "Chunity: Integrated Audiovisual Programming in Unity"

> We have added the new global keyword to enable integrated communication between ChucK code and the outside environment that ChucK is embedded in (the embedding host).
> The `global` keyword is used when declaring the type of a variable, such as (A) below

```c++
public class EventResponder : MonoBehaviour {
 private ChuckSubInstance myChuck;
 void Start() {
  myChuck = GetComponent<ChuckSubInstance>();
     // broadcast "notifier" every 250 ms
    
     myChuck.RunCode( @"
       global Event notifier; // <- Here (A)
       while( true ) {
         notifier.broadcast();
         250::ms => now;
  } ");

     // create a ChuckEventListener
     ChuckEventListener listener = gameObject
       .AddComponent<ChuckEventListener>();
     
     // call MyCallback() during Update()
     //   after every broadcast from "notifier"
     listener.ListenForEvent( myChuck, "notifier", // <- Here (B)
       MyCallback );
 }

 void MyCallback() {
     // react to event (rotate my object)
     transform.Rotate( new Vector3( 5, 10, 15 ) );
 }
}
```

> The main guiding principle in the design of this keyword is that it is not necessary for ChucK to know anything about the embedding host, or whether it is embedded at all. Instead, `global` variables appear like normal variables within their own ChucK script, but can be inspected, edited, or listened to by other ChucK scripts or by the embedding host.
> 
> So far, the `global` keyword is enabled for three types of variables;
>
> 1. The first type of global variable is primitives: `ints`, `floats`, and `strings`. The embedding host can get and set their values. The get operation requires the use of a *callback* because the embedding host often runs on a different thread than the audio thread.
>
> 2. The second type of global variable is `Events`. ChucK Events are used to pause execution in a ChucK script until the Event signals that it has occurred. The embedding host can *signal* or *broadcast* a global Event (i.e. trigger *one* or *all* ChucK scripts waiting on the event). The embedding host can also register a callback to be called every time a global Event is broadcast, as (B) in the code example above. This callback to user code occurs on the audio thread and thus is timed with sample-level accuracy; a tighter integration of timing between audio and visuals is not achievable.
>
> 3. The third type of global variable is `UGens` (unit generators). ChucK UGens are signal processing elements that generate streams of audio. The embedding host can fetch a global UGen’s most recent samples.
>
> ...
> The desire to embed ChucK in Unity motivated the wider libChucK rearchitecture project, which enables ChucK to act as an embeddable component in any C++ project.
>
> The ChucK source was separated into core and host code- bases. The core comprises the language parser, which compiles code, and virtual machine (VM), which translates audio inputs to outputs. One embeds ChucK in a new project by simply writing a new host that calls these functions.
> 
> The rearchitecture allowed multiple VMs to exist in the same address space (useful for contexts where the number of channels is limited and multiple outputs are desired, such as in a digital audio plugin or Unity’s spatial audio system). **It also enabled the redirection of all ChucK error messages to an optional callback** (e.g. the Unity debug console).

## Chuck Events

See above section on events.. "ChucK Events are used to pause execution in a ChucK script until the Event signals that it has occurred."

Documented [here](https://chuck.cs.princeton.edu/doc/language/event.html)

Relevant interface is provided in `chuck/core/chuck_oo.h` and `chuck/core/chuck_globals.h`

There are local events and `global` events, see `test-local-event.ck` and `test-global-event.ck` for examples of both. Trigger a single named event is via `ck_sigal` , and all events of the same name via `ck_broadcast` which implement:

```c++
t_CKBOOL Chuck_Globals_Manager::signalGlobalEvent( const char * name );
t_CKBOOL Chuck_Globals_Manager::broadcastGlobalEvent( const char * name );
```



## Data and Event flow between chuck~ and Max

### Max messages change chuck global variables

The following `set` functions are used in the `ck_anything()` message handler

```c++
t_CKBOOL setGlobalInt( const char * name, t_CKINT val );
t_CKBOOL setGlobalFloat( const char * name, t_CKFLOAT val );
t_CKBOOL setGlobalString( const char * name, const char * val );
t_CKBOOL setGlobalIntArray( const char * name, t_CKINT arrayValues[], t_CKUINT numValues );
t_CKBOOL setGlobalIntArrayValue( const char * name, t_CKUINT index, t_CKINT value );
t_CKBOOL setGlobalAssociativeIntArrayValue( const char * name, const char * key, t_CKINT value );
t_CKBOOL setGlobalFloatArray( const char * name, t_CKFLOAT arrayValues[], t_CKUINT numValues );
t_CKBOOL setGlobalFloatArrayValue( const char * name, t_CKUINT index, t_CKFLOAT value );
t_CKBOOL setGlobalAssociativeFloatArrayValue( const char * name, const char * key, t_CKFLOAT value );
```


### Clobal Variable Callbacks

```c++
// A. primitives
// ----------------------------------------------------------------

// fn(T value)
void cb_global_int1(t_CKINT val);
void cb_global_float1(t_CKFLOAT val);
void cb_global_string1(const char* val);
// used by
t_CKBOOL getGlobalInt( const char * name, void (*callback)(t_CKINT) );
t_CKBOOL getGlobalFloat( const char * name, void (*callback)(t_CKFLOAT) );
t_CKBOOL getGlobalString( const char * name, void (*callback)(const char*) );

// fn(string name, T value)
void cb_global_int2(const char* name, t_CKINT val);
void cb_global_float2(const char* name, t_CKFLOAT val);
void cb_global_string2(const char* name, const char* val);
// used by
t_CKBOOL getGlobalInt( const char * name, void (*callback)(const char*, t_CKINT) );
t_CKBOOL getGlobalFloat( const char * name, void (*callback)(const char*, t_CKFLOAT) );
t_CKBOOL getGlobalString( const char * name, void (*callback)(const char*, const char*) );

// fn(int id, T value)
void cb_global_int3(t_CKINT cb_id, t_CKINT val);
void cb_global_float3(t_CKINT cb_id, t_CKFLOAT val);
void cb_global_string3(t_CKINT cb_id, const char* val);
// used by
t_CKBOOL getGlobalInt( const char * name, t_CKINT callbackID, void (*callback)(t_CKINT, t_CKINT) );
t_CKBOOL getGlobalFloat( const char * name, t_CKINT callbackID, void (*callback)(t_CKINT, t_CKFLOAT) );
t_CKBOOL getGlobalString( const char * name, t_CKINT callbackID, void (*callback)(t_CKINT, const char*) );

// B. arrays
// ----------------------------------------------------------------

// fn(T array[], int size)
void cb_global_int_array1(t_CKINT array[], t_CKUINT n);
void cb_global_float_array1(t_CKFLOAT array[], t_CKUINT n);
// used by
t_CKBOOL getGlobalIntArray( const char * name, void (*callback)(t_CKINT[], t_CKUINT) );
t_CKBOOL getGlobalFloatArray( const char * name, void (*callback)(t_CKFLOAT[], t_CKUINT) );

// fn(string name, T array[], int size)
void cb_global_int_array2(const char* name, t_CKINT array[], t_CKUINT n);
void cb_global_float_array2(const char* name, t_CKFLOAT array[], t_CKUINT n);
// used by
t_CKBOOL getGlobalIntArray( const char * name, void (*callback)(const char*, t_CKINT[], t_CKUINT) );
t_CKBOOL getGlobalFloatArray( const char * name, void (*callback)(const char*, t_CKFLOAT[], t_CKUINT) );

// fn(int cb_id, T array[], int size)
void cb_global_int_array3(t_CKINT cb_id, t_CKINT array[], t_CKUINT n);
void cb_global_float_array3(t_CKINT cb_id, t_CKFLOAT array[], t_CKUINT n);
// used by
t_CKBOOL getGlobalIntArray( const char * name, t_CKINT callbackID, void (*callback)(t_CKINT, t_CKINT[], t_CKUINT) );
t_CKBOOL getGlobalFloatArray( const char * name, t_CKINT callbackID, void (*callback)(t_CKINT, t_CKFLOAT[], t_CKUINT) );


// C. array values
// ----------------------------------------------------------------

// fn(T value)
void cb_global_int_array_value1(t_CKINT value);
void cb_global_float_array_value1(t_CKFLOAT value);
// used by
t_CKBOOL getGlobalIntArrayValue( const char * name, t_CKUINT index, void (*callback)(t_CKINT) );
t_CKBOOL getGlobalFloatArrayValue( const char * name, t_CKUINT index, void (*callback)(t_CKFLOAT) );

// fn(string name, T value)
void cb_global_int_array_value2(const char* name, t_CKINT value);
void cb_global_float_array_value2(const char* name, t_CKFLOAT value);
// use by
t_CKBOOL getGlobalIntArrayValue( const char * name, t_CKUINT index, void (*callback)(const char*, t_CKINT) );
t_CKBOOL getGlobalFloatArrayValue( const char * name, t_CKUINT index, void (*callback)(const char*, t_CKFLOAT) );

// fn(int cb_id, T value)
void cb_global_int_array_value3(t_CKINT cb_id, t_CKINT value);
void cb_global_float_array_value3(t_CKINT cb_id, t_CKFLOAT value);
// used by
t_CKBOOL getGlobalIntArrayValue( const char * name, t_CKINT callbackID, t_CKUINT index, void (*callback)(t_CKINT, t_CKINT) );
t_CKBOOL getGlobalFloatArrayValue( const char * name, t_CKINT callbackID, t_CKUINT index, void (*callback)(t_CKINT, t_CKFLOAT) );

// D. array values of associated arrays
// ----------------------------------------------------------------

// fn(T value)
void cb_global_assoc_int_array_value1(t_CKINT val);
void cb_global_assoc_float_array_value1(t_CKFLOAT val);
// used by
t_CKBOOL getGlobalAssociativeIntArrayValue( const char * name, const char * key, void (*callback)(t_CKINT) );
t_CKBOOL getGlobalAssociativeFloatArrayValue( const char * name, const char * key, void (*callback)(t_CKFLOAT) );


// fn(string name, T value)
void cb_global_assoc_int_array_value2(const char* name, t_CKINT val);
void cb_global_assoc_float_array_value2(const char* name, t_CKFLOAT val);
// used by
t_CKBOOL getGlobalAssociativeIntArrayValue( const char * name, const char * key, void (*callback)(const char*, t_CKINT) );
t_CKBOOL getGlobalAssociativeFloatArrayValue( const char * name, const char * key, void (*callback)(const char*, t_CKFLOAT) );


// fn(int cb_id, T value)
void cb_global_assoc_int_array_value3(t_CKINT cb_id, t_CKINT val);
void cb_global_assoc_float_array_value3(t_CKINT cb_id, t_CKFLOAT val);
// used by
t_CKBOOL getGlobalAssociativeIntArrayValue( const char * name, t_CKINT callbackID, const char * key, void (*callback)(t_CKINT, t_CKINT) );
t_CKBOOL getGlobalAssociativeFloatArrayValue( const char * name, t_CKINT callbackID, const char * key, void (*callback)(t_CKINT, t_CKFLOAT) );
```


### Global Event Callbacks

Can be used for chuck code to trigger callback functions in the external. Prelim example provided.

Better examples needed. How to use the following functions creatively

```c++
t_CKBOOL signalGlobalEvent( const char * name );
t_CKBOOL broadcastGlobalEvent( const char * name );
t_CKBOOL listenForGlobalEvent( const char * name, void (*callback)(void), t_CKBOOL listen_forever );
t_CKBOOL listenForGlobalEvent( const char * name, void (*callback)(const char*), t_CKBOOL listen_forever );
t_CKBOOL listenForGlobalEvent( const char * name, t_CKINT callbackID, void (*callback)(t_CKINT), t_CKBOOL listen_forever );
t_CKBOOL stopListeningForGlobalEvent( const char * name, void (*callback)(void) );
t_CKBOOL stopListeningForGlobalEvent( const char * name, void (*callback)(const char*) );
t_CKBOOL stopListeningForGlobalEvent( const char * name, t_CKINT callbackID, void (*callback)(t_CKINT) );
```

### Global Sample Data


```c++
t_CKBOOL getGlobalUGenSamples( const char * name, SAMPLE* buffer, int numFrames );

```


## Windows Support

```cmake
$<$<AND:$<BOOL:${WIN32}>,$<BOOL:${ENABLE_WARNINGS}>>:/W4>

```

Console level flags

```text
defs
    __WINDOWS_DS__
    __WINDOWS_ASIO__
    __WINDOWS_WASAPI__
    WIN32
    NDEBUG for debug, _DEBUG for release
    _CONSOLE

console
    dsound.lib
    dinput8.lib
    dxguid.lib
    wsock32.lib
    ws2_32.lib
    iphlpapi.lib
    winmm.lib


..\core\lo\address.c
..\core\lo\blob.c
..\core\lo\bundle.c
..\core\lo\message.c
..\core\lo\method.c
..\core\lo\pattern_match.c
..\core\lo\server.c
..\core\lo\send.c
..\core\lo\timetag.c
    defs
        HAVE_CONFIG_H
        __PLATFORM_WIN32__
        __WINDOWS_DS__
        WIN32
        NDEBUG
        _CONSOLE
    additional_includes:
        lo
```

vcproject:

```xml
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" ToolsVersion="15.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <ItemGroup>
    <ClInclude Include="..\core\chuck.h" />
    <ClInclude Include="..\core\chuck_absyn.h" />
    <ClInclude Include="..\core\chuck_carrier.h" />
    <ClInclude Include="..\core\chuck_compile.h" />
    <ClInclude Include="..\core\chuck_globals.h" />
    <ClInclude Include="..\core\util_platforms.h" />
    <ClInclude Include="..\host\chuck_audio.h" />
    <ClInclude Include="..\host\chuck_console.h" />
    <ClInclude Include="..\core\chuck_def.h" />
    <ClInclude Include="..\core\chuck_dl.h" />
    <ClInclude Include="..\core\chuck_emit.h" />
    <ClInclude Include="..\core\chuck_errmsg.h" />
    <ClInclude Include="..\core\chuck_frame.h" />
    <ClInclude Include="..\core\chuck_instr.h" />
    <ClInclude Include="..\core\chuck_io.h" />
    <ClInclude Include="..\core\chuck_lang.h" />
    <ClInclude Include="..\core\chuck_oo.h" />
    <ClInclude Include="..\core\chuck_otf.h" />
    <ClInclude Include="..\core\chuck_parse.h" />
    <ClInclude Include="..\core\chuck_scan.h" />
    <ClInclude Include="..\core\chuck_shell.h" />
    <ClInclude Include="..\core\chuck_stats.h" />
    <ClInclude Include="..\core\chuck_symbol.h" />
    <ClInclude Include="..\core\chuck_table.h" />
    <ClInclude Include="..\core\chuck_type.h" />
    <ClInclude Include="..\core\chuck_ugen.h" />
    <ClInclude Include="..\core\chuck_vm.h" />
    <ClInclude Include="..\core\chuck_yacc.h" />
    <ClInclude Include="..\core\dirent_win32.h" />
    <ClInclude Include="..\core\hidio_sdl.h" />
    <ClInclude Include="..\core\lo\config.h" />
    <ClInclude Include="..\core\lo\lo.h" />
    <ClInclude Include="..\core\lo\lo_endian.h" />
    <ClInclude Include="..\core\lo\lo_errors.h" />
    <ClInclude Include="..\core\lo\lo_internal.h" />
    <ClInclude Include="..\core\lo\lo_lowlevel.h" />
    <ClInclude Include="..\core\lo\lo_macros.h" />
    <ClInclude Include="..\core\lo\lo_osc_types.h" />
    <ClInclude Include="..\core\lo\lo_throw.h" />
    <ClInclude Include="..\core\lo\lo_types.h" />
    <ClInclude Include="..\core\lo\lo_types_internal.h" />
    <ClInclude Include="..\core\midiio_rtmidi.h" />
    <ClInclude Include="..\core\rtmidi.h" />
    <ClInclude Include="..\core\uana_extract.h" />
    <ClInclude Include="..\core\uana_xform.h" />
    <ClInclude Include="..\core\ugen_filter.h" />
    <ClInclude Include="..\core\ugen_osc.h" />
    <ClInclude Include="..\core\ugen_stk.h" />
    <ClInclude Include="..\core\ugen_xxx.h" />
    <ClInclude Include="..\core\ulib_ai.h" />
    <ClInclude Include="..\core\ulib_doc.h" />
    <ClInclude Include="..\core\ulib_machine.h" />
    <ClInclude Include="..\core\ulib_math.h" />
    <ClInclude Include="..\core\ulib_opsc.h" />
    <ClInclude Include="..\core\ulib_std.h" />
    <ClInclude Include="..\core\util_buffers.h" />
    <ClInclude Include="..\core\util_console.h" />
    <ClInclude Include="..\core\util_hid.h" />
    <ClInclude Include="..\core\util_math.h" />
    <ClInclude Include="..\core\util_network.h" />
    <ClInclude Include="..\core\util_opsc.h" />
    <ClInclude Include="..\core\util_raw.h" />
    <ClInclude Include="..\core\util_serial.h" />
    <ClInclude Include="..\core\util_sndfile.h" />
    <ClInclude Include="..\core\util_string.h" />
    <ClInclude Include="..\core\util_thread.h" />
    <ClInclude Include="..\core\util_xforms.h" />
    <ClInclude Include="..\host\RtAudio\include\asio.h" />
    <ClInclude Include="..\host\RtAudio\include\asiodrivers.h" />
    <ClInclude Include="..\host\RtAudio\include\asiodrvr.h" />
    <ClInclude Include="..\host\RtAudio\include\asiolist.h" />
    <ClInclude Include="..\host\RtAudio\include\asiosys.h" />
    <ClInclude Include="..\host\RtAudio\include\dsound.h" />
    <ClInclude Include="..\host\RtAudio\include\functiondiscoverykeys_devpkey.h" />
    <ClInclude Include="..\host\RtAudio\include\ginclude.h" />
    <ClInclude Include="..\host\RtAudio\include\iasiodrv.h" />
    <ClInclude Include="..\host\RtAudio\include\iasiothiscallresolver.h" />
    <ClInclude Include="..\host\RtAudio\include\soundcard.h" />
    <ClInclude Include="..\host\RtAudio\RtAudio.h" />
  </ItemGroup>
  <ItemGroup>
    <ClCompile Include="..\core\chuck.cpp" />
    <ClCompile Include="..\core\chuck_absyn.cpp" />
    <ClCompile Include="..\core\chuck_carrier.cpp" />
    <ClCompile Include="..\core\chuck_compile.cpp" />
    <ClCompile Include="..\core\chuck_globals.cpp" />
    <ClCompile Include="..\core\util_platforms.cpp" />
    <ClCompile Include="..\host\chuck_audio.cpp">
      <AdditionalIncludeDirectories Condition="'$(Configuration)|$(Platform)'=='Release|x64'">..\core;.;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
    </ClCompile>
    <ClCompile Include="..\host\chuck_console.cpp">
      <AdditionalIncludeDirectories Condition="'$(Configuration)|$(Platform)'=='Release|x64'">..\core;.;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
    </ClCompile>
    <ClCompile Include="..\core\chuck_dl.cpp" />
    <ClCompile Include="..\core\chuck_emit.cpp" />
    <ClCompile Include="..\core\chuck_errmsg.cpp" />
    <ClCompile Include="..\core\chuck_frame.cpp" />
    <ClCompile Include="..\core\chuck_instr.cpp" />
    <ClCompile Include="..\core\chuck_io.cpp" />
    <ClCompile Include="..\core\chuck_lang.cpp" />
    <ClCompile Include="..\host\chuck_main.cpp">
      <AdditionalIncludeDirectories Condition="'$(Configuration)|$(Platform)'=='Release|x64'">..\core;.;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
    </ClCompile>
    <ClCompile Include="..\core\chuck_oo.cpp" />
    <ClCompile Include="..\core\chuck_otf.cpp" />
    <ClCompile Include="..\core\chuck_parse.cpp" />
    <ClCompile Include="..\core\chuck_scan.cpp" />
    <ClCompile Include="..\core\chuck_shell.cpp" />
    <ClCompile Include="..\core\chuck_stats.cpp" />
    <ClCompile Include="..\core\chuck_symbol.cpp" />
    <ClCompile Include="..\core\chuck_table.cpp" />
    <ClCompile Include="..\core\chuck_type.cpp">
      <AdditionalIncludeDirectories Condition="'$(Configuration)|$(Platform)'=='Release|x64'">..\core</AdditionalIncludeDirectories>
    </ClCompile>
    <ClCompile Include="..\core\chuck_ugen.cpp" />
    <ClCompile Include="..\core\chuck_vm.cpp" />
    <ClCompile Include="..\core\chuck_yacc.c" />
    <ClCompile Include="..\core\hidio_sdl.cpp" />
    <ClCompile Include="..\core\lo\address.c">
      <PreprocessorDefinitions Condition="'$(Configuration)|$(Platform)'=='Release|x64'">HAVE_CONFIG_H;__PLATFORM_WIN32__;__WINDOWS_DS__;WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <AdditionalIncludeDirectories Condition="'$(Configuration)|$(Platform)'=='Release|x64'">lo;.</AdditionalIncludeDirectories>
    </ClCompile>
    <ClCompile Include="..\core\lo\blob.c">
      <PreprocessorDefinitions Condition="'$(Configuration)|$(Platform)'=='Release|x64'">HAVE_CONFIG_H;__PLATFORM_WIN32__;__WINDOWS_DS__;WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <AdditionalIncludeDirectories Condition="'$(Configuration)|$(Platform)'=='Release|x64'">lo;.</AdditionalIncludeDirectories>
    </ClCompile>
    <ClCompile Include="..\core\lo\bundle.c">
      <PreprocessorDefinitions Condition="'$(Configuration)|$(Platform)'=='Release|x64'">HAVE_CONFIG_H;__PLATFORM_WIN32__;__WINDOWS_DS__;WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <AdditionalIncludeDirectories Condition="'$(Configuration)|$(Platform)'=='Release|x64'">lo;.</AdditionalIncludeDirectories>
    </ClCompile>
    <ClCompile Include="..\core\lo\message.c">
      <PreprocessorDefinitions Condition="'$(Configuration)|$(Platform)'=='Release|x64'">HAVE_CONFIG_H;__PLATFORM_WIN32__;__WINDOWS_DS__;WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <AdditionalIncludeDirectories Condition="'$(Configuration)|$(Platform)'=='Release|x64'">lo;.</AdditionalIncludeDirectories>
    </ClCompile>
    <ClCompile Include="..\core\lo\method.c">
      <PreprocessorDefinitions Condition="'$(Configuration)|$(Platform)'=='Release|x64'">HAVE_CONFIG_H;__PLATFORM_WIN32__;__WINDOWS_DS__;WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <AdditionalIncludeDirectories Condition="'$(Configuration)|$(Platform)'=='Release|x64'">lo;.</AdditionalIncludeDirectories>
    </ClCompile>
    <ClCompile Include="..\core\lo\pattern_match.c">
      <PreprocessorDefinitions Condition="'$(Configuration)|$(Platform)'=='Release|x64'">HAVE_CONFIG_H;__PLATFORM_WIN32__;__WINDOWS_DS__;WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <AdditionalIncludeDirectories Condition="'$(Configuration)|$(Platform)'=='Release|x64'">lo;.</AdditionalIncludeDirectories>
    </ClCompile>
    <ClCompile Include="..\core\lo\send.c">
      <PreprocessorDefinitions Condition="'$(Configuration)|$(Platform)'=='Release|x64'">HAVE_CONFIG_H;__PLATFORM_WIN32__;__WINDOWS_DS__;WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <AdditionalIncludeDirectories Condition="'$(Configuration)|$(Platform)'=='Release|x64'">lo;.</AdditionalIncludeDirectories>
    </ClCompile>
    <ClCompile Include="..\core\lo\server.c">
      <PreprocessorDefinitions Condition="'$(Configuration)|$(Platform)'=='Release|x64'">HAVE_CONFIG_H;__PLATFORM_WIN32__;__WINDOWS_DS__;WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <AdditionalIncludeDirectories Condition="'$(Configuration)|$(Platform)'=='Release|x64'">lo;.</AdditionalIncludeDirectories>
    </ClCompile>
    <ClCompile Include="..\core\lo\timetag.c">
      <PreprocessorDefinitions Condition="'$(Configuration)|$(Platform)'=='Release|x64'">HAVE_CONFIG_H;__PLATFORM_WIN32__;__WINDOWS_DS__;WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <AdditionalIncludeDirectories Condition="'$(Configuration)|$(Platform)'=='Release|x64'">lo;.</AdditionalIncludeDirectories>
    </ClCompile>
    <ClCompile Include="..\core\midiio_rtmidi.cpp" />
    <ClCompile Include="..\core\rtmidi.cpp" />
    <ClCompile Include="..\core\uana_extract.cpp" />
    <ClCompile Include="..\core\uana_xform.cpp" />
    <ClCompile Include="..\core\ugen_filter.cpp" />
    <ClCompile Include="..\core\ugen_osc.cpp" />
    <ClCompile Include="..\core\ugen_stk.cpp" />
    <ClCompile Include="..\core\ugen_xxx.cpp" />
    <ClCompile Include="..\core\ulib_ai.cpp" />
    <ClCompile Include="..\core\ulib_doc.cpp" />
    <ClCompile Include="..\core\ulib_machine.cpp" />
    <ClCompile Include="..\core\ulib_math.cpp" />
    <ClCompile Include="..\core\ulib_opsc.cpp" />
    <ClCompile Include="..\core\ulib_std.cpp" />
    <ClCompile Include="..\core\util_buffers.cpp" />
    <ClCompile Include="..\core\util_console.cpp" />
    <ClCompile Include="..\core\util_hid.cpp" />
    <ClCompile Include="..\core\util_math.cpp" />
    <ClCompile Include="..\core\util_network.c" />
    <ClCompile Include="..\core\util_opsc.cpp" />
    <ClCompile Include="..\core\util_raw.c" />
    <ClCompile Include="..\core\util_serial.cpp" />
    <ClCompile Include="..\core\util_sndfile.c" />
    <ClCompile Include="..\core\util_string.cpp" />
    <ClCompile Include="..\core\util_thread.cpp" />
    <ClCompile Include="..\core\util_xforms.c" />
    <ClCompile Include="..\host\RtAudio\include\asio.cpp" />
    <ClCompile Include="..\host\RtAudio\include\asiodrivers.cpp" />
    <ClCompile Include="..\host\RtAudio\include\asiolist.cpp" />
    <ClCompile Include="..\host\RtAudio\include\iasiothiscallresolver.cpp" />
    <ClCompile Include="..\host\RtAudio\RtAudio.cpp" />
  </ItemGroup>
  <ItemGroup>
    <None Include="..\core\chuck.lex" />
    <None Include="..\core\chuck.y" />
  </ItemGroup>
  <ItemGroup>
    <Text Include="..\host\RtAudio\include\asioinfo.txt" />
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <ProjectGuid>{C83D6D51-D477-4BAB-B2C2-F0698B9D3FB1}</ProjectGuid>
    <Keyword>Win32Proj</Keyword>
    <RootNamespace>chuck_win32</RootNamespace>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>NotSet</CharacterSet>
    <PlatformToolset>v142</PlatformToolset>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <ImportGroup Label="ExtensionSettings">
  </ImportGroup>
  <ImportGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'" Label="PropertySheets">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <PropertyGroup Label="UserMacros" />
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <LinkIncremental>false</LinkIncremental>
    <TargetName>chuck</TargetName>
  </PropertyGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <PrecompiledHeader>
      </PrecompiledHeader>
      <Optimization>MaxSpeed</Optimization>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <PreprocessorDefinitions>__WINDOWS_DS__;__WINDOWS_ASIO__;__WINDOWS_WASAPI__;WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <AdditionalIncludeDirectories>..\host\RtAudio\include;.</AdditionalIncludeDirectories>
      <RuntimeLibrary>MultiThreaded</RuntimeLibrary>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <EnableCOMDATFolding>true</EnableCOMDATFolding>
      <OptimizeReferences>true</OptimizeReferences>
      <AdditionalDependencies>dsound.lib;dinput8.lib;dxguid.lib;wsock32.lib;ws2_32.lib;iphlpapi.lib;winmm.lib;%(AdditionalDependencies)</AdditionalDependencies>
    </Link>
  </ItemDefinitionGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
  <ImportGroup Label="ExtensionTargets">
  </ImportGroup>
</Project>
```
