// AudioUnit MIDI - Option A: Direct Programmatic Control
// Demonstrates sending MIDI messages directly to an AudioUnit instrument

@import "AudioUnit";

<<< "=== AudioUnit Direct MIDI Control (Option A) ===" >>>;

// Create AudioUnit and connect to audio output
AudioUnit synth => dac;

// IMPORTANT: Set gain to hear output
0.5 => synth.gain;

// Load a MusicDevice (instrument) AudioUnit
if (!synth.load("DLSMusicDevice")) {
    <<< "Failed to load DLSMusicDevice" >>>;
    me.exit();
}

// Check if it's a MusicDevice
if (!synth.isMusicDevice()) {
    <<< "Not a MusicDevice, MIDI methods won't work" >>>;
    me.exit();
}

<<< "Loaded MusicDevice:", synth.getMIDIDeviceName() >>>;
<<< "This example demonstrates direct programmatic MIDI control." >>>;
<<< "MIDI messages are sent from ChucK code directly to the AudioUnit." >>>;

// Method 1: High-level convenience methods
<<< "=== Method 1: Using noteOn/noteOff ===" >>>;

// Play a simple melody using direct MIDI control
[60, 64, 67, 72] @=> int melody[];
[100, 90, 95, 110] @=> int velocities[];

for (0 => int i; i < melody.size(); i++) {
    // Send note on
    synth.noteOn(melody[i], velocities[i]);
    <<< "noteOn:", melody[i], "velocity:", velocities[i] >>>;

    // Let it play
    400::ms => now;

    // Send note off
    synth.noteOff(melody[i]);
    <<< "noteOff:", melody[i] >>>;

    // Gap between notes
    100::ms => now;
}

<<< "=== Method 2: Using Control Change ===" >>>;

// Play a note and modulate it with CC
synth.noteOn(60, 100);
<<< "Playing note 60..." >>>;

for (0 => int i; i < 128; i++) {
    synth.controlChange(1, i);  // Modulation wheel
    10::ms => now;
}

synth.noteOff(60);
<<< "CC modulation complete" >>>;

// Method 3: Using raw MIDI for full control
<<< "=== Method 3: Using sendMIDI (raw MIDI) ===" >>>;

// Note on: 0x90 (channel 0), pitch 67, velocity 120
synth.sendMIDI(0x90, 67, 120);
<<< "sendMIDI: Note ON (0x90, 67, 120)" >>>;
500::ms => now;

// Note off: 0x80 (channel 0), pitch 67
synth.sendMIDI(0x80, 67, 0);
<<< "sendMIDI: Note OFF (0x80, 67, 0)" >>>;
500::ms => now;

<<< "=== Method 4: Program Change ===" >>>;

// Try different instrument programs
for (0 => int i; i < 3; i++) {
    synth.programChange(i);
    <<< "Program:", i >>>;

    synth.noteOn(60, 100);
    300::ms => now;
    synth.noteOff(60);
    200::ms => now;
}

<<< "=== Cleaning up ===" >>>;
synth.close();
<<< "AudioUnit closed" >>>;
<<< "See 07-midi-routing.ck for virtual MIDI destination example (Option C)" >>>;
<<< "=== Example complete ===" >>>;