# Dev Notees


## global keyword

from "Chunity: Integrated Audiovisual Programming in Unity":

We have added the new global keyword to enable integrated communication between ChucK code and the outside environment that ChucK is embedded in (the embedding host). 

The `global` keyword is used when declaring the type of a variable, such as (A) below

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

The main guiding principle in the design of this keyword is that it is not necessary for ChucK to know anything about the embedding host, or whether it is embedded at all. Instead, `global` variables appear like normal variables within their own ChucK script, but can be inspected, edited, or listened to by other ChucK scripts or by the embedding host.

So far, the `global` keyword is enabled for three types of variables;

1. The first type of global variable is primitives: `ints`, `floats`, and `strings`. The embedding host can get and set their values. The get operation requires the use of a *callback* because the embedding host often runs on a different thread than the audio thread.

2. The second type of global variable is `Events`. ChucK Events are used to pause execution in a ChucK script until the Event signals that it has occurred. The embedding host can *signal* or *broadcast* a global Event (i.e. trigger *one* or *all* ChucK scripts waiting on the event). The embedding host can also register a callback to be called every time a global Event is broadcast, as (B) in the code example above. This callback to user code occurs on the audio thread and thus is timed with sample-level accuracy; a tighter integration of timing between audio and visuals is not achievable.

3. The third type of global variable is `UGens` (unit generators). ChucK UGens are signal processing elements that generate streams of audio. The embedding host can fetch a global UGen’s most recent samples.

...

The desire to embed ChucK in Unity motivated the wider libChucK rearchitecture project, which enables ChucK to act as an embeddable component in any C++ project.

The ChucK source was separated into core and host code- bases. The core comprises the language parser, which compiles code, and virtual machine (VM), which translates audio inputs to outputs. One embeds ChucK in a new project by simply writing a new host that calls these functions.

The rearchitecture allowed multiple VMs to exist in the same address space (useful for contexts where the number of channels is limited and multiple outputs are desired, such as in a digital audio plugin or Unity’s spatial audio system). **It also enabled the redirection of all ChucK error messages to an optional callback** (e.g. the Unity debug console).
