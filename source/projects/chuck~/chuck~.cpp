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

#define GAIN 0.5
#define SAMPLE float
#define MY_SRATE 44100
#define N_IN_CHANNELS 1
#define N_OUT_CHANNELS 1
#define MAX_FILENAME 128
// #define BUFFER_SIZE 256
#define BUFFER_SIZE 64

// struct to represent the object's state
typedef struct _ck {
    t_pxobject ob;      // the object itself (t_pxobject in MSP instead of t_object)
    t_symbol* filename; // filename of lua file in Max search path
    t_symbol* funcname; // name of lua dsp function to use
    double param1;      // the value of a property of our object
    double v1;          // historical value;
    const char *working_dir;
    ChucK *chuck;
    float *in_chuck_buffer;
    float *out_chuck_buffer;
} t_ck;


// method prototypes
void *ck_new(t_symbol *s, long argc, t_atom *argv);
void ck_free(t_ck *x);
void ck_assist(t_ck *x, void *b, long m, long a, char *s);
void ck_bang(t_ck *x);
void ck_anything(t_ck* x, t_symbol* s, long argc, t_atom* argv);
void ck_float(t_ck *x, double f);
void ck_dsp64(t_ck *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void ck_perform64(t_ck *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);

t_string* get_path_from_package(t_class* c, char* subpath);

// global class pointer variable
static t_class *ck_class = NULL;


//-----------------------------------------------------------------------------------------------


t_string* get_path_from_external(t_class* c, char* subpath)
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
    result = string_new(external_path);
    if (subpath != NULL) {
        string_append(result, subpath);
    }
    return result;
}


t_string* get_path_from_package(t_class* c, char* subpath)
{
    t_string* result;
    t_string* external_path = get_path_from_external(c, NULL);

    const char* ext_path_c = string_getptr(external_path);

    result = string_new(dirname(dirname((char*)ext_path_c)));

    if (subpath != NULL) {
        string_append(result, subpath);
    }

    return result;
}


//-----------------------------------------------------------------------------------------------

void ext_main(void *r)
{
    // object initialization, note the use of dsp_free for the freemethod, which is required
    // unless you need to free allocated memory, in which case you should call dsp_free from
    // your custom free function.

    t_class *c = class_new("chuck~", (method)ck_new, (method)ck_free, (long)sizeof(t_ck), 0L, A_GIMME, 0);

    class_addmethod(c, (method)ck_float,    "float",    A_FLOAT, 0);
    class_addmethod(c, (method)ck_anything, "anything", A_GIMME, 0);
    class_addmethod(c, (method)ck_bang,     "bang",              0);
    class_addmethod(c, (method)ck_dsp64,    "dsp64",    A_CANT,  0);
    class_addmethod(c, (method)ck_assist,   "assist",   A_CANT,  0);

    class_dspinit(c);
    class_register(CLASS_BOX, c);
    ck_class = c;
}




void ck_compile_file(t_ck *x, const char *filename)
{
    if (x->chuck->compileFile(std::string(filename), "", 1)) {
        post("compilation successful: %s", filename);
    } else {
        error("compilation error! : %s", filename);
    }

}


void ck_run_file(t_ck *x)
{
    if (x->filename != gensym("")) {
        char norm_path[MAX_PATH_CHARS];
        path_nameconform(x->filename->s_name, norm_path, 
            PATH_STYLE_MAX, PATH_TYPE_BOOT);
        if (access(norm_path, F_OK) == 0) { // file exists in path
            ck_compile_file(x, norm_path);
        } else { // try in the example folder
            t_string* path = get_path_from_package(ck_class, "/examples/");
            string_append(path, x->filename->s_name);
            const char* ck_file = string_getptr(path);
            ck_compile_file(x, ck_file);
        }
    }
}



void *ck_new(t_symbol *s, long argc, t_atom *argv)
{
    t_ck *x = (t_ck *)object_alloc(ck_class);

    if (x) {
        dsp_setup((t_pxobject *)x, N_IN_CHANNELS);  // MSP inlets: arg is # of inlets and is REQUIRED!
        // use 0 if you don't need inlets

        for (int i=0; i < N_OUT_CHANNELS; i++) {
            // post("created: outlet %d", i);
            outlet_new(x, "signal");        // signal outlet (note "signal" rather than NULL)            
        }
        x->param1 = 0.0;
        x->v1 = 0.0;
        x->filename = atom_getsymarg(0, argc, argv); // 1st arg of object
        x->funcname = gensym("base");
        x->working_dir = string_getptr(get_path_from_package(ck_class, "/examples"));
        x->in_chuck_buffer = NULL;
        x->out_chuck_buffer = NULL;


        x->chuck = new ChucK();
        x->chuck->setParam( CHUCK_PARAM_SAMPLE_RATE, (t_CKINT) MY_SRATE );
        x->chuck->setParam( CHUCK_PARAM_INPUT_CHANNELS, (t_CKINT) N_IN_CHANNELS );
        x->chuck->setParam( CHUCK_PARAM_OUTPUT_CHANNELS, (t_CKINT) N_OUT_CHANNELS );
        x->chuck->setParam( CHUCK_PARAM_VM_HALT, (t_CKINT) 0 );
        x->chuck->setParam( CHUCK_PARAM_DUMP_INSTRUCTIONS, (t_CKINT) 0 );
        // directory for compiled code
        std::string global_dir = std::string(x->working_dir);
        x->chuck->setParam( CHUCK_PARAM_WORKING_DIRECTORY, global_dir );
        std::list< std::string > chugin_search;
        chugin_search.push_back(global_dir + "/Chugins" );
        chugin_search.push_back(global_dir + "/ChuGins" );
        chugin_search.push_back(global_dir + "/chugins" );
        x->chuck->setParam( CHUCK_PARAM_USER_CHUGIN_DIRECTORIES, chugin_search );

        // init chuck
        x->chuck->init();
        x->chuck->start();

        post("ChucK %s", x->chuck->version());
        post("inputs: %d  outputs: %d ", 
            x->chuck->vm()->m_num_adc_channels, 
            x->chuck->vm()->m_num_dac_channels);

        post("file: %s", x->filename->s_name);
        // post("working dir: %s", x->working_dir);
        // post("chugins dir: %s/chugins", x->working_dir);
    }
    return (x);
}


void ck_free(t_ck *x)
{
    delete[] x->in_chuck_buffer;
    delete[] x->out_chuck_buffer;
    delete x->chuck;
    dsp_free((t_pxobject *)x);
}


void ck_assist(t_ck *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) { //inlet
        sprintf(s, "I am inlet %ld", a);
    }
    else {  // outlet
        sprintf(s, "I am outlet %ld", a);
    }
}

void ck_bang(t_ck *x)
{
    ck_run_file(x);
}

void ck_anything(t_ck* x, t_symbol* s, long argc, t_atom* argv)
{
    if (s != gensym("")) {
        post("funcname: %s", s->s_name);
        x->funcname = s;
    }
}


void ck_float(t_ck *x, double f)
{
    x->param1 = f;
    post("param1: %f", x->param1);
}


void ck_dsp64(t_ck *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    // post("sample rate: %f", samplerate);
    // post("maxvectorsize: %d", maxvectorsize);

    delete[] x->in_chuck_buffer;
    delete[] x->out_chuck_buffer;

    x->in_chuck_buffer = new float[BUFFER_SIZE * N_IN_CHANNELS];
    x->out_chuck_buffer = new float[BUFFER_SIZE * N_OUT_CHANNELS];

    memset(x->in_chuck_buffer, 0.f, sizeof(float) * BUFFER_SIZE * N_IN_CHANNELS);
    memset(x->out_chuck_buffer, 0.f, sizeof(float) * BUFFER_SIZE * N_OUT_CHANNELS);

    object_method(dsp64, gensym("dsp_add64"), x, ck_perform64, 0, NULL);
}


void ck_perform64(t_ck *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
    SAMPLE * inL = (SAMPLE *)ins[0];     // we get audio for each inlet of the object from the **ins argument
    SAMPLE * outL = (SAMPLE *)outs[0];   // we get audio for each outlet of the object from the **outs argument
    int n = sampleframes;                // n = 64
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
