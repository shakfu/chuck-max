// AudioUnit MIDI - Option C: Virtual MIDI Destination
// Demonstrates using the automatically-created virtual MIDI destination
//
// This example shows how to route external MIDI or ChucK's MidiIn/MidiOut
// to an AudioUnit instrument through its virtual MIDI destination.

@import "AudioUnit";

<<< "=== AudioUnit Virtual MIDI Destination (Option C) ===" >>>;

// Create AudioUnit instrument
AudioUnit synth => dac;

// IMPORTANT: Set gain to hear output
0.5 => synth.gain;

// Load a MusicDevice
if (!synth.load("DLSMusicDevice")) {
    <<< "Failed to load DLSMusicDevice" >>>;
    me.exit();
}

if (!synth.isMusicDevice()) {
    <<< "Not a MusicDevice" >>>;
    me.exit();
}

<<< "Loaded:", synth.getMIDIDeviceName() >>>;
<<< "A virtual MIDI destination has been automatically created!" >>>;
<<< "This MIDI destination is visible to:" >>>;
<<< "  1. ChucK (via chuck --probe)" >>>;
<<< "  2. External MIDI controllers" >>>;
<<< "  3. DAWs and other music software" >>>;
<<< "  4. System MIDI utilities" >>>;

<<< "=== Usage Options ===" >>>;
<<< "Option 1: Forward from ChucK's MidiIn" >>>;
<<< "  Connect a MIDI controller, then use the manual forwarding" >>>;
<<< "  pattern shown below to route MIDI to this AudioUnit." >>>;
<<< "Option 2: Use ChucK's MidiOut" >>>;
<<< "  In another ChucK program, use MidiOut to send to this" >>>;
<<< "  virtual destination (find device number with chuck --probe)." >>>;
<<< "Option 3: External MIDI software" >>>;
<<< "  Any MIDI software can now send to:", synth.getMIDIDeviceName() >>>;

<<< "=== Example: Manual MIDI Forwarding ===" >>>;
<<< "Uncomment the code below to forward from a MIDI input device:" >>>;

/*
// Forward MIDI from an input device to the AudioUnit
MidiIn min;
MidiMsg msg;

// Open MIDI input device (device 0 - change as needed)
if (!min.open(0)) {
    <<< "Failed to open MIDI input device" >>>;
    <<< "Run 'chuck --probe' to see available devices" >>>;
    me.exit();
}

<<< "MIDI input opened:", min.name() >>>;
<<< "Now forwarding MIDI to AudioUnit..." >>>;

while (true) {
    min => now;  // Wait for MIDI event

    while (min.recv(msg)) {
        // Forward the MIDI message to the AudioUnit
        synth.sendMIDI(msg.data1, msg.data2, msg.data3);

        // Optional: print for debugging
        <<< "Forwarded:", msg.data1, msg.data2, msg.data3 >>>;
    }
}
*/

<<< "=== Example: Using MidiOut (from another ChucK program) ===" >>>;
<<< "In a separate ChucK program, you can do:" >>>;
<<< "  MidiOut mout;" >>>;
<<< "  MidiMsg msg;" >>>;
<<< "  " >>>;
<<< "  // Find device number from 'chuck --probe'" >>>;
<<< "  mout.open(DEVICE_NUMBER);" >>>;
<<< "  " >>>;
<<< "  // Send MIDI" >>>;
<<< "  0x90 => msg.data1;  // Note on" >>>;
<<< "  60 => msg.data2;     // Middle C" >>>;
<<< "  100 => msg.data3;    // Velocity" >>>;
<<< "  mout.send(msg);" >>>;

<<< "=== Keeping AudioUnit alive ===" >>>;
<<< "The virtual MIDI destination remains active while this program runs." >>>;
<<< "Press Ctrl+C to exit..." >>>;

// Keep running so the virtual MIDI destination stays available
while (true) {
    1::second => now;
}
