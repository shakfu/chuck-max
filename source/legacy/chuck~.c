// chuck~ v 1.5, Brad Garton (1/2009) OSX 10.5, max5
// max/msp external for the ChucK music language, version 1.2.1.2 (dracula)
// see http://chuck.cs.princeton.edu/ for more info
//
// 6/2007 -- univetsal binary
//
// 5/2008 -- temporary path fix for max5
//
// 1/2009 -- changed the dyn-loading scheme to use NSmodules for OSX 10.5
//	apple somehow changed the CFBundle stuff so that the data space isn't unique
//	no limit on # of chuck~'s now!
//	also updated to ChucK 1.2.1.2
//
// 1/2011
//	-- updated to chuck v 1.2.1.3
//	-- still using NSLoad stuff instead of dlopen(), see rtcmix~ source for reasons why

#define VERSION "1.6"
#define CHUCK_VERSION "1.2.1.3"

#include "ext.h"
#include "z_dsp.h"
#include "string.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ext_obex.h"

// for the NSmodule stuff
#include <mach-o/dyld.h>


#define MAX_INPUTS 10 	// arbitrary
#define MAX_OUTPUTS 10	// also arbitrary
#define MAX_SCRIPTS 20	//how many scripts can we store internally


/******* ChucK stuff *******/
typedef int (*chuckmainFunctionPtr)(int vecsize, int srate);
typedef void (*pull_cb2FunctionPtr)(float *maxmsp_outbuf, int vecsize);
typedef int (*parse_scoreFunctionPtr)(char *buf);
typedef int (*check_bangFunctionPtr)();
typedef int (*check_valsFunctionPtr)(float *thevals);
typedef int (*inlet_setFunctionPtr)(int inlet, float pval, int ninlets);


void *chuck_class;

typedef struct _chuck
{
	// header
	t_pxobject x_obj;

	// variables specific to this object (see not above)
	float srate;				  	// sample rate var for the heck of it
	long num_inputs, num_outputs; 	// number of inputs and outputs
	long num_vinlets;				// number of inlets for dynamic chuck var control
	short in_connected[MAX_INPUTS]; // we'll store whether there are 0 or more signal connections
 
	void *outpointer;

	/******* ChucK stuff *******/
	chuckmainFunctionPtr chuckmain;
	pull_cb2FunctionPtr pull_cb2;
	parse_scoreFunctionPtr parse_score;
	check_bangFunctionPtr check_bang;
	check_valsFunctionPtr check_vals;
	inlet_setFunctionPtr inlet_set;

	NSObjectFileImage objectFileImage;
	NSModule module;
	NSSymbol symbol;

	int outbuf_size;  // chuck uses the same buffer for both input and output
	// space for these malloc'd in chuck_dsp()
	float *maxmsp_outbuf;

	// for the chuck_var() and chuck_varlist() $n variable scheme
	#define NVARS 9	
	float var_array[NVARS];
	Boolean var_set[NVARS];

	// stuff for check_vals -- "1000" is arbitrary (and set in chuck_main.cpp also)
	float thevals[1000];
	t_atom valslist[1000];

	// script buffer pointer for large binbuf restores
	char *restore_buf_ptr;

	// editor stuff
	t_object m_obj;
	t_object *m_editor;
	char *chuck_script[MAX_SCRIPTS], s_name[MAX_SCRIPTS][256];
	long chuck_script_len[MAX_SCRIPTS];
	short current_script, path[MAX_SCRIPTS];
} t_chuck;


// for where the chuck-dylibs folder is located
char *mpathptr;
char mpathname[1000];


/****PROTOTYPES****/

//	max/msp setup funcs; this probably won't change, unless you decide to change the number of
//	args that the user can input, in which case chuck_new will have to change
void *chuck_new(long num_inoutputs, long num_additional);
void chuck_dsp(t_chuck *x, t_signal **sp, short *count); 
t_int *chuck_perform(t_int *w);
void chuck_assist(t_chuck *x, void *b, long m, long a, char *s);
void chuck_free(t_chuck *x);

//for getting floats, ints or bangs at inputs
void chuck_float(t_chuck *x, double f);
void chuck_int(t_chuck *x, int i);
void chuck_bang(t_chuck *x);
void chuck_dobangout(t_chuck *x, Symbol *s, short argc, t_atom *argv); // for the defer

//for custom messages
void chuck_version(t_chuck *x);
void chuck_text(t_chuck *x, Symbol *s, short argc, Atom *argv);
void chuck_dotext(t_chuck *x, Symbol *s, short argc, Atom *argv); // for the defer
void chuck_chuck(t_chuck *x, Symbol *s, short argc, Atom *argv);
void chuck_var(t_chuck *x, Symbol *s, short argc, Atom *argv);
void chuck_varlist(t_chuck *x, Symbol *s, short argc, Atom *argv);

//for the text editor
void chuck_edclose (t_chuck *x, char **text, long size);
void chuck_dblclick(t_chuck *x);
void chuck_goscript(t_chuck *x, Symbol *s, short argc, Atom *argv);
void chuck_dogoscript(t_chuck *x, Symbol *s, short argc, Atom *argv); // for the defer
void chuck_openscript(t_chuck *x, Symbol *s, short argc, Atom *argv);
void chuck_setscript(t_chuck *x, Symbol *s, short argc, Atom *argv);
void chuck_read(t_chuck *x, Symbol *s, short argc, Atom *argv);
void chuck_doread(t_chuck *x, Symbol *s, short argc, t_atom *argv);
void chuck_write(t_chuck *x, Symbol *s, short argc, Atom *argv);
void chuck_writeas(t_chuck *x, Symbol *s, short argc, Atom *argv);
void chuck_dowrite(t_chuck *x, Symbol *s, short argc, t_atom *argv);
void chuck_okclose (t_chuck *x, char *prompt, short *result);

//for binbuf storage of scripts
void chuck_save(t_chuck *x, void *w);
void chuck_restore(t_chuck *x, Symbol *s, short argc, Atom *argv);


/****FUNCTIONS****/

//primary MSP funcs -- this is called the first time you instantiate an object of this type
int main(void)
{
	int i;
	short path, rval;

	//the A_DEFLONG arguments give us the object arguments for the user to set number of ins/outs, etc. (not used in chuck~)
	setup((struct messlist **)&chuck_class, (method)chuck_new, (method)chuck_free, (short)sizeof(t_chuck), 0L, A_DEFLONG, A_DEFLONG, 0);

	// max/msp standard messages; don't change these  
	addmess((method)chuck_dsp, "dsp", A_CANT, 0);
	addmess((method)chuck_assist,"assist", A_CANT,0);
	
	//our own messages
	addmess((method)chuck_version, "version", 0);
	addmess((method)chuck_text, "text", A_GIMME, 0);
	addmess((method)chuck_chuck, "chuck", A_GIMME, 0);
	addmess((method)chuck_var, "var", A_GIMME, 0);
	addmess((method)chuck_varlist, "varlist", A_GIMME, 0);

	// chuck_float will be called for any floats coming in any inlet
	addfloat((method)chuck_float);

	// chuck_int will be called for any ints coming in any inlet
	addint((method)chuck_int);

	// trigger scripts
	addbang((method)chuck_bang);

	//for the text editor and scripts
	addmess ((method)chuck_edclose, "edclose", A_CANT, 0); 
	addmess((method)chuck_dblclick,	"dblclick",	A_CANT, 0);
	addmess((method)chuck_goscript, "goscript", A_GIMME, 0);
	addmess((method)chuck_openscript, "openscript", A_GIMME, 0);
	addmess((method)chuck_setscript, "setscript", A_GIMME, 0);
	addmess((method)chuck_read, "read", A_GIMME, 0);
	addmess ((method)chuck_okclose, "okclose", A_CANT, 0);  
	addmess((method)chuck_write, "savescript", A_GIMME, 0);
	addmess((method)chuck_writeas, "savescriptas", A_GIMME, 0);

	// binbuf storage
	addmess((method)chuck_save, "save", A_CANT, 0);
	addmess((method)chuck_restore, "restore", A_GIMME, 0);

	// msp initialization (gotta have this)
	dsp_initclass();

	// find the chuck-dylibs folder location
	nameinpath("chuck-dylibs", &path);
	rval = path_topathname(path, "", mpathname);
	if (rval != 0) error("couldn't find the chuck-dylibs folder!");
	else {
		for (i = 0; i < 1000; i++)
			if (mpathname[i] == '/') break;
		mpathptr = mpathname+i;
	}

    // ho ho!
    post("chuck~ -- ChucK music language, v. %s (ChucK v. %s)", VERSION, CHUCK_VERSION);
	return(1);
 }


// this gets called when the object is created and every time the user types in new args
void *chuck_new(long num_inoutputs, long num_additional)
{
	int i;
	t_chuck *x;

	// for the full path to the chuckdylib.so file
	char pathname[1000]; // probably should be malloc'd


	// leave this; creates our object
	x = (t_chuck *)newobject(chuck_class);

	// zero out the struct, to be careful (takk to jkclayton)
	if (x) { 
		for (i = sizeof(t_pxobject); i < sizeof(t_chuck); i++)  
			((char *)x)[i] = 0;
	}

	// binbuf storage
	// this sends the 'restore' message (chuck_restore() below)
	gensym("#X")->s_thing = (struct object*)x;
	
	// these are the entry function pointers in to the chuck.so lib
	x->chuckmain = NULL;
	x->pull_cb2 = NULL;
	x->parse_score = NULL;
	x->check_bang = NULL;
	x->check_vals = NULL;
	x->inlet_set = NULL;

	// constrain number of inputs and outputs -- note: chuck~ is set up only for stereo i/o
	if (num_inoutputs < 1) num_inoutputs = 2; // no args, use default of 2 channel in/out
	if ((num_inoutputs + num_additional) > MAX_INPUTS) {
		error("sorry, only %d total inlets are allowed!", MAX_INPUTS);
		return(NULL);
	}

	// and we set the internal t_chuck structure variables
	if (num_inoutputs != 2) {
		num_inoutputs = 2;
		error("sorry, chuck~ only does stereo i/o right now.");
	}
	x->num_inputs = num_inoutputs;
	x->num_outputs = num_inoutputs;
	x->num_vinlets = num_additional;

	// now setup audio inputs and outputs
	// inputs
	dsp_setup((t_pxobject *)x, x->num_inputs + x->num_vinlets);


	// here we set the 'outpointer' variable to reference the right-most outlet.  We will use this in our
	// chuck_float(), chuck_int(), chuck_bang() functions below to send out non-signal information
	// this will be the right-most outlet because we are creating it before the signal outlets.  max/msp
	// creates oject outlets right-to-left in the order they are set up.
	x->outpointer = outlet_new((t_object *)x, 0);
   
	// signal outputs
	for (i = 0; i < x->num_outputs; i++) {
		outlet_new((t_object *)x, "signal");
	}

	//initialize input variables; important to do this!
	for (i = 0; i < (x->num_inputs + x->num_vinlets); i++) {
		x->in_connected[i] = 0;
	}

	//occasionally this line is necessary if you are doing weird asynchronous things with the in/out vectors
	//x->x_obj.z_misc = Z_NO_INPLACE;

	// ChucK stuff
	// full path to the chuckdylib.so file
	sprintf(pathname, "%s/chuckdylib.so", mpathptr);

	// load the dylib
	NSCreateObjectFileImageFromFile(pathname, &(x->objectFileImage));
	x->module = NSLinkModule(x->objectFileImage, pathname, NSLINKMODULE_OPTION_PRIVATE | NSLINKMODULE_OPTION_BINDNOW);
	NSDestroyObjectFileImage(x->objectFileImage);

	// find the main entry to be sure we're cool...
	x->symbol = NSLookupSymbolInModule(x->module, "_chuckmain");
	if (x->symbol == NULL)
		error("cannot find chuckmain, looked here: %s", pathname);
	else {
		x->chuckmain = NSAddressOfSymbol(x->symbol);
			if (!(x->chuckmain))
				error("chuck~ could not find chuckmain()");
	}

	x->symbol = NSLookupSymbolInModule(x->module, "_pull_cb2");
	if (x->symbol == NULL)
		error("cannot find pull_cb2, looked here: %s", pathname);
	else {
		x->pull_cb2 = NSAddressOfSymbol(x->symbol);
		if (!(x->pull_cb2))
			error("chuck~ could not find pull_cb2()");
	}

	x->symbol = NSLookupSymbolInModule(x->module, "_parse_score");
	if (x->symbol == NULL)
		error("cannot find parse_score, looked here: %s", pathname);
	else {
		x->parse_score = NSAddressOfSymbol(x->symbol);
		if (!(x->parse_score))
			error("chuck~ could not find parse_score()");
	}

	x->symbol = NSLookupSymbolInModule(x->module, "_check_bang");
	if (x->symbol == NULL)
		error("cannot find check_bang, looked here: %s", pathname);
	else {
		x->check_bang = NSAddressOfSymbol(x->symbol);
		if (!(x->check_bang))
			error("chuck~ could not find check_bang()");
	}

	x->symbol = NSLookupSymbolInModule(x->module, "_check_vals");
	if (x->symbol == NULL)
		error("cannot find check_vals, looked here: %s", pathname);
	else {
		x->check_vals = NSAddressOfSymbol(x->symbol);
		if (!(x->check_vals))
			error("chuck~ could not find check_vals()");
	}

	x->symbol = NSLookupSymbolInModule(x->module, "_inlet_set");
	if (x->symbol == NULL)
		error("cannot find inlet_set, looked here: %s", pathname);
	else {
		x->inlet_set = NSAddressOfSymbol(x->symbol);
		if (!(x->inlet_set))
			error("chuck~ could not find inlet_set()");
	}


	// set up for the variable-substitution scheme	
	for(i = 0; i < NVARS; i++) {
		x->var_set[i] = false;
		x->var_array[i] = 0.0;
	}

	//the text editor
	x->m_editor = NULL;
	x->current_script = 0;
	for (i = 0; i < MAX_SCRIPTS; i++) {
		x->s_name[i][0] = 0;
	}

	return (x);
}


// this gets called everytime audio is started; even when audio is running, if the user changes anything
// (like deletes a patch cord), audio will be turned off and then on again, calling this func.
// this adds the "perform" method to the DSP chain, and also tells us
// where the audio vectors are and how big they are
void chuck_dsp(t_chuck *x, t_signal **sp, short *count)
{
	void *dsp_add_args[MAX_INPUTS + MAX_OUTPUTS + 2];
	int i;
	// BGG mmx -- this is for my "keep the shreduler alive" hack below
	char keepalivebuf[] = "9999.0::second => now;";

	// ChucK vars
	// for the full path to the chuckdylib.so file
	char pathname[1000]; // probably should be malloc'd
	
	// these are the entry function pointers in to the chuck.so lib
	x->chuckmain = NULL;
	x->pull_cb2 = NULL;
	x->parse_score = NULL;
	x->check_bang = NULL;
	x->check_vals = NULL;
	x->inlet_set = NULL;

	// set sample rate var, just for fun
	x->srate = sp[0]->s_sr;
	x->outbuf_size = sp[0]->s_n;

	// check to see if there are signals connected to the various inputs
	// count[] contains the number of audio signals connected to each inlet
	for (i = 0; i < (x->num_inputs + x->num_vinlets); i++)
		x->in_connected[i] = count[i];

	// construct the array of vectors and stuff for max/msp to knoww
	dsp_add_args[0] = x; // the object itself
	for (i = 0; i < (x->num_inputs + x->num_vinlets + x->num_outputs); i++) { // pointers to the input and output vectors
		dsp_add_args[i+1] = sp[i]->s_vec;
	}
	dsp_add_args[x->num_inputs + + x->num_vinlets + x->num_outputs + 1] = (void *)sp[0]->s_n; // pointer to the vector size
	dsp_addv(chuck_perform, (x->num_inputs + + x->num_vinlets + x->num_outputs + 2), dsp_add_args); // add them to the signal chain

	// ChucK stuff
	// full path to the chuckdylib.so file
	sprintf(pathname, "%s/chuckdylib.so", mpathptr);

	// load the dylib
	NSCreateObjectFileImageFromFile(pathname, &(x->objectFileImage));
	x->module = NSLinkModule(x->objectFileImage, pathname, NSLINKMODULE_OPTION_PRIVATE | NSLINKMODULE_OPTION_BINDNOW);
	NSDestroyObjectFileImage(x->objectFileImage);

	// find the main entry to be sure we're cool...
	x->symbol = NSLookupSymbolInModule(x->module, "_chuckmain");
	if (x->symbol == NULL)
		error("cannot find chuckmain, looked here: %s", pathname);
	else {
		x->chuckmain = NSAddressOfSymbol(x->symbol);
			if (!(x->chuckmain))
				error("chuck~ could not call chuckmain()");
	}

	x->symbol = NSLookupSymbolInModule(x->module, "_pull_cb2");
	if (x->symbol == NULL)
		error("cannot find pull_cb2, looked here: %s", pathname);
	else {
		x->pull_cb2 = NSAddressOfSymbol(x->symbol);
		if (!(x->pull_cb2))
			error("chuck~ could not findpull_cb2()");
	}

	x->symbol = NSLookupSymbolInModule(x->module, "_parse_score");
	if (x->symbol == NULL)
		error("cannot find parse_score, looked here: %s", pathname);
	else {
		x->parse_score = NSAddressOfSymbol(x->symbol);
		if (!(x->parse_score))
			error("chuck~ could not find parse_score()");
	}

	x->symbol = NSLookupSymbolInModule(x->module, "_check_bang");
	if (x->symbol == NULL)
		error("cannot find check_bang, looked here: %s", pathname);
	else {
		x->check_bang = NSAddressOfSymbol(x->symbol);
		if (!(x->check_bang))
			error("chuck~ could not find check_bang()");
	}

	x->symbol = NSLookupSymbolInModule(x->module, "_check_vals");
	if (x->symbol == NULL)
		error("cannot find check_vals, looked here: %s", pathname);
	else {
		x->check_vals = NSAddressOfSymbol(x->symbol);
		if (!(x->check_vals))
			error("chuck~ could not find check_vals()");
	}

	x->symbol = NSLookupSymbolInModule(x->module, "_inlet_set");
	if (x->symbol == NULL)
		error("cannot find inlet_set, looked here: %s", pathname);
	else {
		x->inlet_set = NSAddressOfSymbol(x->symbol);
		if (!(x->inlet_set))
			error("chuck~ could not find inlet_set()");
	}

	if (x->chuckmain)
		x->chuckmain(x->outbuf_size, x->srate);

	// BGG mmx -- REAL HACK
	// this sends 'keepalivebuf' ("999.0::second => now;") to chuck to keep the shreduler alive
	// there is surely a better way to do this, but I don't know how the shreduler maintains its time
	if (x->parse_score(keepalivebuf) != 0) error("problem parsing ChucK 'keepalive' script");

	// allocate the ChucK i/o transfer buffers (chuck uses the same buffer for both input and output)
	x->maxmsp_outbuf = malloc(sizeof(float) * x->outbuf_size * x->num_outputs);
}


// this is where the action is
// we get vectors of samples (n samples per vector), process them and send them out
// the "w" array contains a lot of stuff:
//	w[0] -- pointer to the chuck_perform function being called
//	w[1] -- pointer to the t_chuck object structure data
//	w[2] ... w[numinputs+1] -- pointers to arrays of samples, one vectors'-worth for each input
//	w[numinputs+2] ... w[numinputs+numoutputs+1] -- pointers to arrays to be filled with samples (output arrays)
//	w[numinputs+numoutputs+2] -- number of samples/vector (i.e. the number of samples in each input or output array)
t_int *chuck_perform(t_int *w)
{
	t_chuck *x = (t_chuck *)(w[1]);

	float *in[MAX_INPUTS]; 		// pointers to the input vectors
	float *out[MAX_OUTPUTS];	// pointers to the output vectors
	// note the the above are pointers -- each input/output is represented as an array
	// of samples (i.e. non-interleaved).  These are set to point to the w[] arrays.

	long n;	// number of samples per vector

	// local vars
	int i, j;

	// for check_vals()
	int valflag;


	// check to see if we should skip this routine if the patcher is "muted"
	// all audio objects should have this, even though the 'goto' is supremely elegant.
	if (x->x_obj.z_disabled) goto out;

	// check to see if we have a signal connected to each input and assign the pointer accordingly
	for (i = 0; i < (x->num_inputs + x->num_vinlets); i++) {
		if (x->in_connected[i]) in[i] = (float *)(w[i+2]);
	}
	
	// assign the output vector pointers
	for (i = 0 ;i < x->num_outputs; i++) {
		out[i] = (float *)(w[x->num_inputs+x->num_vinlets+i+2]);
	}
	// after these two loops above, we can get samples using *in[inlet]+sampval and *out[oulet]+sampval to
	// get and set sample values for each inlet and outlet vector.


	/* ------- this is the input sample computing loop for each vector ------- */
	// we're going to count down by the total # of samples, and process each of the current input vectors.
	// Alternatively we could have counted through each vector separately, but we didn't
	n = w[x->num_inputs + x->num_vinlets + x->num_outputs + 2];	// number of samples per vector
	j = 0;
	while (n--) {	//this is where the input action happens.....
		for(i = 0; i < x->num_inputs; i++)
			if (x->in_connected[i])
				(x->maxmsp_outbuf)[j++] = *in[i]++;
			else
				(x->maxmsp_outbuf)[j++] = 0.0;
	}

	// ChucK stuff
	// this drives the sample-computing input and output in chuck
	x->pull_cb2(x->maxmsp_outbuf, x->outbuf_size);

	/* ------- this is the output sample computing loop for each vector ------- */
	// we're going to count down by the total # of samples, and process each of the current output vectors.
	// Alternatively we could have counted through each vector separately, but we didn't
	n = w[x->num_inputs + x->num_vinlets + x->num_outputs + 2];	// number of samples per vector
	j = 0;
	while (n--) { // n is the number of samples for the vector
		// the "++" on the in[] (code above) and out[] sample vector pointers move to the next sample in the vector
		// note: for some reason signal is *required* in max/msp at the left-most inlet for processing to take place
		for (i = 0; i < x->num_outputs; i++) {
			if (i < x->num_outputs)
				*out[i]++ = (x->maxmsp_outbuf)[j++];
		}
	}
	/* ------- end of the sample computing loop ------- */

	// look for a pending bang from maxbang.go in ChucK
	if (x->check_bang() == 1) // I don't think I really need this defer, but what the heck.
		defer_low(x, (method)chuck_dobangout, (Symbol *)NULL, 0, (Atom *)NULL);

	// look for pending vals from MAXMESSAGE()	
	valflag = x->check_vals(x->thevals);

	// BGG -- I should probably defer this one.  So far not a problem...
	if (valflag > 0) {
		for (i = 0; i < valflag; i++)
			SETFLOAT((x->valslist)+i, x->thevals[i]);			
		outlet_list(x->outpointer, 0L, valflag, x->valslist);
	}

	// return a pointer to the next object in the signal chain.
out:
	return w + x->num_inputs + x->num_vinlets + x->num_outputs + 3;
}


// the deferred bang output
void chuck_dobangout(t_chuck *x, Symbol *s, short argc, Atom *argv)
{
	outlet_bang(x->outpointer);
}


// tells the user about the inputs/outputs when mousing over them
// "m" is inlet (1) or outlet (2); "a" is which one the mouse is over
void chuck_assist(t_chuck *x, void *b, long m, long a, char *s)
{
	// this is the message that gets shown in the window frame
	if (m == 1) {
		if (a == 0) sprintf(s, "signal/text (score commands) in");
		else sprintf(s, "signal/inletvals in");
	}
	if (m == 2) {
		if (a < x->num_outputs) sprintf(s, "signal out");
		else sprintf(s, "info out");
	}
}


// here's my free function
void chuck_free(t_chuck *x)
{
	// close any open editor windows
	if (x->m_editor)
		freeobject((t_object *)x->m_editor);
	x->m_editor = NULL;
	dsp_free((t_pxobject *)x);
}


// this gets called whenever a float is received at *any* input
// used for setting inlet vars in chuck
void chuck_float(t_chuck *x, double f)
{
	int i;

	// check to see which input the float came in -- isn't this fun?
	for (i = 0; i < x->num_inputs+ x->num_vinlets; i++) {
		if (i == x->x_obj.z_in) {
			if (i < x->num_inputs)
				post("chuck~: received float number %f at inlet %d, but not using it", f, i);
			else
				x->inlet_set(i - (x->num_inputs-1), f, x->num_vinlets);
		}
	}
}


// this gets called whenever an int is received at *any* input
// used for setting inlet vars in chuck
void chuck_int(t_chuck *x, int ival)
{
	int i;

	// check to see which input the int came in
	for (i = 0; i < x->num_inputs+ x->num_vinlets; i++) {
		if (i == x->x_obj.z_in) {
			if (i < x->num_inputs)
				post("chuck~: received int number %d at inlet %d, but not using it", ival, i);
			else
				x->inlet_set(i - (x->num_inputs-1), (float)ival, x->num_vinlets);
		}
	}
}


// bang triggers the current working script
void chuck_bang(t_chuck *x)
{
	Atom a[1];

	a[0].a_w.w_long = x->current_script;
	a[0].a_type = A_LONG;
	defer_low(x, (method)chuck_dogoscript, NULL, 1, a);
}


// print out the chuck~ version
void chuck_version(t_chuck *x)
{
	post("chuck~, v. %s (ChucK v. %s) by Brad Garton", VERSION, CHUCK_VERSION);
}


// see the note for chuck_dotext() below
void chuck_text(t_chuck *x, Symbol *s, short argc, Atom *argv)
{
	defer_low(x, (method)chuck_dotext, s, argc, argv); // always defer this message
}


// what to do when we get the message "text" or "chuck"
// chuck~ scores can come from the [textedit] object or message boxes this way, for example
void chuck_dotext(t_chuck *x, Symbol *s, short argc, Atom *argv)
{
	short i, varnum;
	char thebuf[8192]; // should #define these probably
	char xfer[8192];
	char *bptr;
	int nchars;
	int top;

	if (sys_getdspstate() == 0) return; // audio not running

	bptr = thebuf;
	nchars = 0;
	top = 0;

	for (i=0; i < argc; i++) {
		switch (argv[i].a_type) {
			case A_LONG:
				sprintf(xfer, " %ld", argv[i].a_w.w_long);
				break;
			case A_FLOAT:
				sprintf(xfer, " %lf", argv[i].a_w.w_float);
				break;
			case A_DOLLAR:
				varnum = argv[i].a_w.w_long;
				if ( !(x->var_set[varnum-1]) ) {
					error("variable $%d has not been set yet!   Not running...",varnum);
					return;
				}
				sprintf(xfer, " %lf", x->var_array[varnum-1]);		
				break;
			case A_SYM:
				if (top == 0) { sprintf(xfer, "%s", argv[i].a_w.w_sym->s_name); top = 1;}
				else sprintf(xfer, " %s", argv[i].a_w.w_sym->s_name);
				break;
			case A_SEMI:
				sprintf(xfer, ";");
				break;
			case A_COMMA:
				sprintf(xfer, ",");
		}
		strcpy(bptr, xfer);
		nchars = strlen(xfer);
		bptr += nchars;
	}

	if (x->parse_score(thebuf) != 0) error("problem parsing ChucK script");
}


// see the note for chuck_dotext() above
void chuck_chuck(t_chuck *x, Symbol *s, short argc, Atom *argv)
{
	defer_low(x, (method)chuck_dotext, s, argc, argv); // always defer this message
}


// the "var" message allows us to set $n variables imbedded in a scorefile with varnum value messages
void chuck_var(t_chuck *x, Symbol *s, short argc, Atom *argv)
{
	short i, varnum;

	for (i = 0; i < argc; i += 2) {
		varnum = argv[i].a_w.w_long;
		if ( (varnum < 1) || (varnum > NVARS) ) {
			error("only vars $1 - $9 are allowed");
			return;
		}
		x->var_set[varnum-1] = true;
		switch (argv[i+1].a_type) {
			case A_LONG:
				x->var_array[varnum-1] = (float)(argv[i+1].a_w.w_long);
				break;
			case A_FLOAT:
				x->var_array[varnum-1] = argv[i+1].a_w.w_float;
		}
	}
}


// the "varlist" message allows us to set $n variables imbedded in a scorefile with a list of positional vars
void chuck_varlist(t_chuck *x, Symbol *s, short argc, Atom *argv)
{
	short i;

	if (argc > NVARS) {
			error("asking for too many variables, only setting the first 9 ($1-$9)");
			argc = NVARS;
	}
	
	for (i = 0; i < argc; i++) {
		x->var_set[i] = true;
		switch (argv[i].a_type) {
			case A_LONG:
				x->var_array[i] = (float)(argv[i].a_w.w_long);
				break;
			case A_FLOAT:
				x->var_array[i] = argv[i].a_w.w_float;
		}
	}
}


// here is the text-editor buffer stuff, go dan trueman go!
// used for chuck~ internal buffers
void chuck_edclose (t_chuck *x, char **text, long size) 
{
	if (x->chuck_script[x->current_script]) {
		sysmem_freeptr((void *)x->chuck_script[x->current_script]);
		x->chuck_script[x->current_script] = 0;
	}
	x->chuck_script_len[x->current_script] = size;
    x->chuck_script[x->current_script] = (char *)sysmem_newptr((size+1) * sizeof(char)); // size+1 so we can add '\0' at end
	strncpy(x->chuck_script[x->current_script], *text, size);
	x->chuck_script[x->current_script][size] = '\0'; // add the terminating '\0'
	x->m_editor = NULL;
}


void chuck_okclose (t_chuck *x, char *prompt, short *result)
{
	*result = 3; //don't put up dialog box
	return;
}


// open up an ed window on the current buffer
void chuck_dblclick(t_chuck *x) 
{
	char title[80];
	
	if (x->m_editor) {
		if(x->chuck_script[x->current_script]) 
			object_method(x->m_editor, gensym("settext"), x->chuck_script[x->current_script], gensym("utf-8"));
	} else {
		x->m_editor = object_new(CLASS_NOBOX, gensym("jed"), (t_object *)x, 0);
		sprintf(title,"script_%d", x->current_script);
		object_attr_setsym(x->m_editor, gensym("title"), gensym(title));
		if(x->chuck_script[x->current_script])
			object_method(x->m_editor, gensym("settext"), x->chuck_script[x->current_script], gensym("utf-8"));
	}

	object_attr_setchar(x->m_editor, gensym("visible"), 1);
}


// see the note for chuck_goscript() below
void chuck_goscript(t_chuck *x, Symbol *s, short argc, Atom *argv)
{
	defer_low(x, (method)chuck_dogoscript, s, argc, argv); // always defer this message
}  


// the [goscript N] message will cause buffer N to be sent to the chuck parser
void chuck_dogoscript(t_chuck *x, Symbol *s, short argc, Atom *argv)
{
	short i,j,temp = 0;
	int tval;
	int buflen;
	#define MAXSCRIPTSIZE 16384
	char thebuf[MAXSCRIPTSIZE]; // should probably by dyn-alloced, or at least set to coordinate with chuck (if necessary)

	if (argc == 0) {
		error("chuck~: goscript needs a buffer number [0-19]");
		return;
	}

	if (sys_getdspstate() == 0) return; // audio not running
		
	for (i = 0; i < argc; i++) {
		switch (argv[i].a_type) {
			case A_LONG:
				temp = (short)argv[i].a_w.w_long;
				break;
			case A_FLOAT:
				temp = (short)argv[i].a_w.w_float;
		}
	}

	if (temp > MAX_SCRIPTS) {
		error("chuck~: only %d scripts available, setting to script number %d", MAX_SCRIPTS, MAX_SCRIPTS-1);
		temp = MAX_SCRIPTS-1;
	}
	if (temp < 0) {
		error("chuck~: the script number should be > 0!  Resetting to script number 0");
		temp = 0;
	}
	x->current_script = temp;

	buflen = x->chuck_script_len[x->current_script];
	if (x->chuck_script_len[x->current_script] == 0)
		post("chuck~: you are triggering a 0-length script!");

	if (buflen > MAXSCRIPTSIZE) {
		error("chuck~: script %d is too large!", x->current_script);
		return;
	}

	// probably don't need to transfer to a new buffer, but I want to be sure there's room for the \0,
	// plus the substitution of \n for those annoying ^M thingies	
	for (i = 0, j = 0; i < buflen; i++) {
		thebuf[j] = *(x->chuck_script[x->current_script]+i);
		if ((int)thebuf[j] == 13) thebuf[j] = '\n'; // chuck wants newlines, not <cr>'s
		
		// ok, here's where we substitute the $vars
		if (thebuf[j] == '$') {
			sscanf(x->chuck_script[x->current_script]+i+1, "%d", &tval);
			if ( !(x->var_set[tval-1]) ) {
				error("variable $%d has not been set yet!  Not running...", tval);
				return;
			}
			sprintf(thebuf+j, "%f", x->var_array[tval-1]);
			j = strlen(thebuf)-1;
			i++; // skip over the var number in input
		}
		j++;
	}
	thebuf[j] = '\0';

	if (buflen > 0) {
		if (x->parse_score(thebuf) != 0) error("problem parsing ChucK script");
	}
}


// [openscript N] will open a buffer N
void chuck_openscript(t_chuck *x, Symbol *s, short argc, Atom *argv)
{
	short i,temp = 0;

	if (argc == 0) {
		error("chuck~: openscript needs a buffer number [0-19]");
		return;
	}

	for (i = 0; i < argc; i++) {
		switch (argv[i].a_type) {
			case A_LONG:
				temp = (short)argv[i].a_w.w_long;
				break;
			case A_FLOAT:
				temp = (short)argv[i].a_w.w_float;
		}
	}

	if (temp > MAX_SCRIPTS) {
		error("chuck~: only %d scripts available, setting to script number %d", MAX_SCRIPTS, MAX_SCRIPTS-1);
		temp = MAX_SCRIPTS-1;
	}
	if (temp < 0) {
		error("chuck~: the script number should be > 0!  Resetting to script number 0");
		temp = 0;
	}
	
	x->current_script = temp;
	chuck_dblclick(x);
}


// [setscript N] will set the currently active script to N
void chuck_setscript(t_chuck *x, Symbol *s, short argc, Atom *argv)
{
	short i,temp = 0;

	if (argc == 0) {
		error("chuck~: setscript needs a buffer number [0-19]");
		return;
	}
			
	for (i = 0; i < argc; i++) {
		switch (argv[i].a_type) {
			case A_LONG:
				temp = (short)argv[i].a_w.w_long;
				break;
			case A_FLOAT:
				temp = (short)argv[i].a_w.w_float;
		}
	}
	
	if (temp > MAX_SCRIPTS) {
		error("chuck~: only %d scripts available, setting to script number %d", MAX_SCRIPTS, MAX_SCRIPTS-1);
		temp = MAX_SCRIPTS-1;
	}
	if (temp < 0) {
		error("chuck~: the script number should be > 0!  Resetting to script number 0");
		temp = 0;
	}

	x->current_script = temp;
}


// the [savescript] message triggers this
void chuck_write(t_chuck *x, Symbol *s, short argc, Atom *argv)  
{    
	short i, temp = 0;

	for (i = 0; i < argc; i++) {
		switch (argv[i].a_type) {
			case A_LONG:
				temp = (short)argv[i].a_w.w_long;
				break;
			case A_FLOAT:
				temp = (short)argv[i].a_w.w_float;
		}
	}

	if (temp > MAX_SCRIPTS) {
		error("chuck~: only %d scripts available, setting to script number %d", MAX_SCRIPTS, MAX_SCRIPTS-1);
		temp = MAX_SCRIPTS-1;
	}
	if (temp < 0) {
		error("chuck~: the script number should be > 0!  Resetting to script number 0");
		temp = 0;
	}

	x->current_script = temp;
	post("chuck: current script is %d", temp);
	
	defer(x, (method)chuck_dowrite, s, argc, argv); // always defer this message  
}


// the [savescriptas] message triggers this
void chuck_writeas(t_chuck *x, Symbol *s, short argc, Atom *argv)  
{   
	short i, temp = 0;
	
	for (i=0; i < argc; i++) {
		switch (argv[i].a_type) {
			case A_LONG:
				temp = (short)argv[i].a_w.w_long;
				if (temp > MAX_SCRIPTS) {
					error("chuck~: only %d scripts available, setting to script number %d", MAX_SCRIPTS, MAX_SCRIPTS-1);
					temp = MAX_SCRIPTS-1;
				}
				if (temp < 0) {
					error("chuck~: the script number should be > 0!  Resetting to script number 0");
					temp = 0;
				}
				x->current_script = temp;
				x->s_name[x->current_script][0] = 0;
				break;
			case A_FLOAT:
				temp = (short)argv[i].a_w.w_float;
				if (temp > MAX_SCRIPTS) {
					error("chuck~: only %d scripts available, setting to script number %d", MAX_SCRIPTS, MAX_SCRIPTS-1);
					temp = MAX_SCRIPTS-1;
				}
				if (temp < 0) {
					error("chuck~: the script number should be > 0!  Resetting to script number 0");
					temp = 0;
				}
				x->current_script = temp;
				x->s_name[x->current_script][0] = 0;
				break;
			case A_SYM://this doesn't work yet
				strcpy(x->s_name[x->current_script], argv[i].a_w.w_sym->s_name);
				post("chuck~: writing file %s",x->s_name[x->current_script]);
		}
	}
	post("chuck: current script is %d", temp);
		
	defer(x, (method)chuck_dowrite, s, argc, argv); // always defer this message  
}


// deferred from the [save*] messages
void chuck_dowrite(t_chuck *x, Symbol *s, short argc, t_atom *argv)
{
	char filename[256];
	t_handle script_handle;
	short err;
	long type_chosen, thistype = 'TEXT';
	t_filehandle fh;
	
	if(!x->s_name[x->current_script][0]) {
		//if (saveas_dialog(&x->s_name[0][x->current_script], &x->path[x->current_script], &type)) 
		  if (saveasdialog_extended(x->s_name[x->current_script], &x->path[x->current_script], &type_chosen, &thistype, 1))
			return; //user cancelled
	} 
	strcpy(filename, x->s_name[x->current_script]);
	
	err = path_createsysfile(filename, x->path[x->current_script], thistype, &fh);  
	if (err) {       
		fh = 0;      
		error("chuck~: error %d creating file", err); 
		return;
	}
	
	script_handle = sysmem_newhandle(0);
	sysmem_ptrandhand (x->chuck_script[x->current_script], script_handle, x->chuck_script_len[x->current_script]); 

	err = sysfile_writetextfile(fh, script_handle, TEXT_LB_UNIX); 
	if (err) {       
		fh = 0;      
		error("chuck~: error %d writing file", err); 
		return;
	}

	// BGG for some reason mach-o doesn't like this one... the memory hit should be small
//	sysmem_freehandle(script_handle);
	sysfile_seteof(fh, x->chuck_script_len[x->current_script]); 
	sysfile_close(fh);

	return;	

}

// the [read ...] message triggers this
void chuck_read(t_chuck *x, Symbol *s, short argc, Atom *argv)  
{    
	defer(x, (method)chuck_doread, s, argc, argv); // always defer this message  
}

// the deferred read
void chuck_doread(t_chuck *x, Symbol *s, short argc, t_atom *argv) 
{
	char filename[256];
	short err, i, temp = 0;
	long type = 'TEXT'; 
	long size;
	long outtype;
	t_filehandle fh;
	t_handle script_handle;
	
	for (i = 0; i < argc; i++) {
		switch (argv[i].a_type) {
			case A_LONG:
				temp = (short)argv[i].a_w.w_long;
				if (temp > MAX_SCRIPTS) {
					error("chuck~: only %d scripts available, setting to script number %d", MAX_SCRIPTS, MAX_SCRIPTS-1);
					temp = MAX_SCRIPTS-1;
				}
				if (temp < 0) {
					error("chuck~: the script number should be > 0!  Resetting to script number 0");
					temp = 0;
				}
				x->current_script = temp;
				x->s_name[x->current_script][0] = 0;
				break;
			case A_FLOAT:
				if (temp > MAX_SCRIPTS) {
					error("chuck~: only %d scripts available, setting to script number %d", MAX_SCRIPTS, MAX_SCRIPTS-1);
					temp = MAX_SCRIPTS-1;
				}
				if (temp < 0) {
					error("chuck~: the script number should be > 0!  Resetting to script number 0");
					temp = 0;
				}
				x->current_script = temp;
				temp = (short)argv[i].a_w.w_float;
				x->s_name[x->current_script][0] = 0;
				break;
			case A_SYM:
				strcpy(filename, argv[i].a_w.w_sym->s_name);
				strcpy(x->s_name[x->current_script], filename);
		}
	}

	
	if(!x->s_name[x->current_script][0]) {
//		if (open_dialog(filename, &path, &outtype, &type, 1))
		if (open_dialog(filename,  &x->path[x->current_script], &outtype, 0L, 0)) // allow all types of files

			return; //user cancelled
	} else {
		if (locatefile_extended(filename, &x->path[x->current_script], &outtype, &type, 1)) {
			error("chuck~: error opening file: can't find file"); 
			x->s_name[x->current_script][0] = 0;
			return; //not found
		}
	}
	
	//we should have a valid filename at this point
	err = path_opensysfile(filename, x->path[x->current_script], &fh, READ_PERM);  
	if (err) {       
		fh = 0;      
		error("error %d opening file", err); 
		return;
	}
	
	strcpy(x->s_name[x->current_script], filename);
	
	sysfile_geteof(fh, &size);
	if (x->chuck_script[x->current_script]) {
		sysmem_freeptr((void *)x->chuck_script[x->current_script]);
		x->chuck_script[x->current_script] = 0;
	}
	// BGG size+1 in max5 to add the terminating '\0'
	if (!(x->chuck_script[x->current_script] = (char *)sysmem_newptr(size+1)) || !(script_handle = sysmem_newhandle(size+1))) {
		error("chuck~: %s too big to read", filename);
		return;
	} else {
		x->chuck_script_len[x->current_script] = size;
		sysfile_readtextfile(fh, script_handle, size, TEXT_LB_NATIVE);
		strcpy(x->chuck_script[x->current_script], *script_handle);
	}
	x->chuck_script[x->current_script][size] = '\0'; // the max5 text editor apparently needs this
	// BGG for some reason mach-o doesn't like this one... the memory hit should be small
//	sysmem_freehandle(script_handle);
	sysfile_close(fh);

	return;	
}

// this converts the current script to a binbuf format and sets it up to be saved and then restored
// via the chuck_restore() method below
void chuck_save(t_chuck *x, void *w)
{
	char *fptr, *tptr;
	char tbuf[5000]; // max 5's limit on symbol size is 32k, this is totally arbitrary on my part
	int i,j,k;

	// insert the command to recreate the chuck~ object, with any additional vars
	binbuf_vinsert(w, "ssll", gensym("#N"), gensym("chuck~"), x->num_outputs, x->num_vinlets);

	for (i = 0; i < MAX_SCRIPTS; i++) {
		if (x->chuck_script[i] && (x->chuck_script_len[i] > 0)) { // there is a script...	
			// the reason I do this 'chunking' of restore messages is because of the 32k limit
			// I still wish Cycling had a generic, *untouched* buffer type.
			fptr = x->chuck_script[i];
			tptr = tbuf;
			k = 0;
			for (j = 0; j < x->chuck_script_len[i]; j++) {
				*tptr++ = *fptr++;
				if (++k >= 5000) { // 'serialize' the script
					// the 'restore' message contains script #, current buffer length, final buffer length, symbol with buffer contents
					*tptr = '\0';
					binbuf_vinsert(w, "ssllls", gensym("#X"), gensym("restore"), i, k, x->chuck_script_len[i], gensym(tbuf));
					tptr = tbuf;
					k = 0;
				}
			}
			// do the final one (or the only one in cases where scripts < 5000)
			*tptr = '\0';
			binbuf_vinsert(w, "ssllls", gensym("#X"), gensym("restore"), i, k, x->chuck_script_len[i], gensym(tbuf));
		}
	}
}


// and this gets the message set up by chuck_save()
void chuck_restore(t_chuck *x, Symbol *s, short argc, Atom *argv)
{
	int i;
	int bsize, fsize;
	char *fptr; // restore buf pointer is in the struct for repeated calls necessary for larger scripts (gensym limit)

	// script #, current buffer size, final script size, script data
	x->current_script = argv[0].a_w.w_long;
	bsize = argv[1].a_w.w_long;
	fsize = argv[2].a_w.w_long;
	fptr = argv[3].a_w.w_sym->s_name;

	if (!x->chuck_script[x->current_script]) {  // if the script isn't being restored already
		if (!(x->chuck_script[x->current_script] = (char *)sysmem_newptr(fsize+1))) { // fsize+1 for the '\0
			error("chuck~: problem allocating memory for restored script");
			return;
		}
		x->chuck_script_len[x->current_script] = fsize;
		x->restore_buf_ptr = x->chuck_script[x->current_script];
	}

	// this happy little for-loop is for older (max 4.x) chuck scripts.  The older version of max had some
	// serious parsing issues for saved text.  Now it all seems fixed in 5 -- yay!
	// convert the xCHUCK_XXx tokens to their real equivalents
	for (i = 0; i < bsize; i++) {
		switch (*fptr) {
			case 'x':
				if (strncmp(fptr, "xCHUCK_CRx", 10) == 0) {
					sprintf(x->restore_buf_ptr, "\r");
					fptr += 10;
					x->restore_buf_ptr++;
					break;
				}
				else if (strncmp(fptr, "xCHUCK_DQx", 10) == 0) {
					sprintf(x->restore_buf_ptr, "\"");
					fptr += 10;
					x->restore_buf_ptr++;
					break;
				} else {
					*x->restore_buf_ptr++ = *fptr++;
					break;
				}
			default:
				*x->restore_buf_ptr++ = *fptr++;
		}
	}

	x->chuck_script[x->current_script][fsize] = '\0';  // the final '\0'

	x->current_script = 0; // do this to set script 0 as default
}