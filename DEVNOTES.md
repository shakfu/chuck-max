# Dev Notes

## Converting chugins to cmake builds

This has been pretty straightforward, especially as the project is currently only targeting one platform (macOS). 

However, static complition and integration of chugins is not working although this is indicated as possible in the makefiles: chugins were not recognized when invoked via test scripts.


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

This was changed to this initially:

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

and finally, it was simplified to the following:

```c++
void ck_perform64(t_ck *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
    float * in_ptr = x->in_chuck_buffer;
    float * out_ptr = x->out_chuck_buffer;
    int n = sampleframes; // n = 64

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

### Issues

#### How to set default values?

As a general rule, if the chuck code will not run in chuck, then it will not run in chuck-max.

So global parameters should be initialized in the chuck script as normal and chuck-max will change the value of initialization on opening. It practice, this is a non-issue.


### From "Chunity: Integrated Audiovisual Programming in Unity":

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

> So far, the `global` keyword is enabled for three types of variables;

> 1. The first type of global variable is primitives: `ints`, `floats`, and `strings`. The embedding host can get and set their values. The get operation requires the use of a *callback* because the embedding host often runs on a different thread than the audio thread.

> 2. The second type of global variable is `Events`. ChucK Events are used to pause execution in a ChucK script until the Event signals that it has occurred. The embedding host can *signal* or *broadcast* a global Event (i.e. trigger *one* or *all* ChucK scripts waiting on the event). The embedding host can also register a callback to be called every time a global Event is broadcast, as (B) in the code example above. This callback to user code occurs on the audio thread and thus is timed with sample-level accuracy; a tighter integration of timing between audio and visuals is not achievable.

> 3. The third type of global variable is `UGens` (unit generators). ChucK UGens are signal processing elements that generate streams of audio. The embedding host can fetch a global UGen’s most recent samples.

> ...

> The desire to embed ChucK in Unity motivated the wider libChucK rearchitecture project, which enables ChucK to act as an embeddable component in any C++ project.

> The ChucK source was separated into core and host code- bases. The core comprises the language parser, which compiles code, and virtual machine (VM), which translates audio inputs to outputs. One embeds ChucK in a new project by simply writing a new host that calls these functions.

> The rearchitecture allowed multiple VMs to exist in the same address space (useful for contexts where the number of channels is limited and multiple outputs are desired, such as in a digital audio plugin or Unity’s spatial audio system). **It also enabled the redirection of all ChucK error messages to an optional callback** (e.g. the Unity debug console).

