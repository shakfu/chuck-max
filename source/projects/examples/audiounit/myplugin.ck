// AudioUnit MIDI - Simple Working Example
// Demonstrates MIDI with actual audio output

@import "AudioUnit";

<<< "=== Simple MIDI Test ===" >>>;

// Create AudioUnit instrument and connect to dac
AudioUnit synth => dac;

// IMPORTANT: Set gain to hear output
0.5 => synth.gain;

// Load DLSMusicDevice
if (!synth.load("Diva")) {
    <<< "Failed to load" >>>;
    me.exit();
}

// Discover presets
<<< "Preset count:", synth.presetCount() >>>;

// List all available presets
<<< "Available presets:" >>>;
for (0 => int i; i < synth.presetCount(); i++) {
    <<< "  [" + i + "]", synth.presetName(i) >>>;
}

// Get current preset
<<< "Current preset index:", synth.getPreset() >>>;

<<< "Loaded:", synth.getMIDIDeviceName() >>>;
<<< "Playing melody..." >>>;

// Simple melody
[60, 64, 67, 72, 67, 64] @=> int notes[];

for (0 => int i; i < notes.size(); i++) {
    <<< "Note:", notes[i] >>>;

    // Start note
    synth.noteOn(notes[i], 100);

    // Let it play
    500::ms => now;

    // Stop note
    synth.noteOff(notes[i]);

    // Gap
    100::ms => now;
}

<<< "Done!" >>>;
synth.close();
