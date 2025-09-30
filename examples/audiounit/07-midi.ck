// AudioUnit with MIDI Control
// Demonstrates using MIDI to control an AudioUnit instrument

@import "AudioUnit";

// Note: This is a conceptual example showing how MIDI would be used
// with AudioUnit instruments. The actual implementation depends on
// the AudioUnit's MIDI handling capabilities.

<<< "=== AudioUnit MIDI Control Example ===" >>>;

<<< "Note: This is a conceptual example." >>>;

<<< "AudioUnit instruments typically accept MIDI messages through:" >>>;
<<< "  1. Direct MIDI input (requires MIDI interface)" >>>;
<<< "  2. Programmatic MIDI events" >>>;

<<< "Currently, the AudioUnit chugin processes audio only." >>>;
<<< "MIDI functionality would require:" >>>;
<<< "  - Extension to send MIDI events to the AudioUnit" >>>;
<<< "  - Methods like: au.noteOn(pitch, velocity)" >>>;
<<< "  - Methods like: au.noteOff(pitch)" >>>;
<<< "  - Methods like: au.controlChange(cc, value)" >>>;


// Conceptual usage (not yet implemented):
/*
AudioUnit synth => dac;

if (synth.load("DLSMusicDevice")) {
    // Future MIDI methods (not yet implemented):
    // synth.noteOn(60, 100);    // Middle C, velocity 100
    // 1::second => now;
    // synth.noteOff(60);

    synth.close();
}
*/

<<< "=== Workaround: Use ChucK's native MIDI ===" >>>;

<<< "For now, to use MIDI with audio effects:" >>>;
<<< "  1. Use ChucK's built-in instruments/oscillators" >>>;
<<< "  2. Control them with MidiIn" >>>;
<<< "  3. Process through AudioUnit effects" >>>;


// Practical workaround example:
SinOsc osc => AudioUnit au => dac;
0.3 => osc.gain;

if (au.load("AUDistortion")) {
    <<< "Example: MIDI-controlled oscillator through AudioUnit effect" >>>;


    // Simulate MIDI note events
    [60, 64, 67, 72] @=> int notes[];

    for (0 => int i; i < notes.size(); i++) {
        Std.mtof(notes[i]) => osc.freq;
        <<< "Note:", notes[i], "Freq:", osc.freq() >>>;
        500::ms => now;
    }

    au.close();
}


<<< "=== Example complete ===" >>>;