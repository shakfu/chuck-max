// AudioUnit as Generator/Instrument
// Demonstrates using an AudioUnit that generates sound
// Note: Most AudioUnit instruments require MIDI input to make sound

@import "AudioUnit";

AudioUnit au => dac;

<<< "=== AudioUnit Generator Example ===" >>>;


// Try to load an AudioUnit instrument/generator
// Note: This example attempts to load Apple's DLS Music Device
// For full instrument functionality, see the MIDI examples

if (au.load("DLS")) {
    <<< "Loaded DLS Music Device (instrument)" >>>;

    <<< "Note: Most AudioUnit instruments require MIDI input to produce sound" >>>;
    <<< "See example 07-midi.ck for full instrument usage" >>>;


    <<< "Running generator for 5 seconds..." >>>;
    <<< "(You may not hear sound without MIDI input)" >>>;

    5::second => now;

    au.close();
    <<< "Closed AudioUnit" >>>;
} else {
    <<< "Failed to load DLS Music Device" >>>;

    <<< "Note: Generator/Instrument AudioUnits typically require:" >>>;
    <<< "  1. Proper MIDI initialization" >>>;
    <<< "  2. MIDI note messages to trigger sound" >>>;
    <<< "  3. May not be directly loadable by name" >>>;

    <<< "See example 07-midi.ck for proper instrument usage" >>>;
}


<<< "=== Example complete ===" >>>;