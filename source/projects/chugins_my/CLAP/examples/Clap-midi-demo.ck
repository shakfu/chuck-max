// CLAP MIDI Demo - Using a CLAP synth plugin with MIDI from ChucK
//
// This example demonstrates how to drive a CLAP synthesizer plugin
// with MIDI note events from ChucK. This is particularly useful for
// using virtual instruments like TAL-NoiseMaker, Surge XT, or other
// CLAP-format synth plugins.

// Create CLAP host
CLAP synth => dac;

// Load a CLAP synthesizer plugin
// macOS bundle paths are automatically resolved
if (!synth.load("/Library/Audio/Plug-Ins/CLAP/TAL-NoiseMaker.clap"))
{
    <<< "Failed to load plugin - check the path" >>>;
    me.exit();
}

<<< "Plugin loaded successfully" >>>;

// List available plugins in the loaded library
<<< "Available plugins in library:" >>>;
synth.listPlugins();

// Activate the first plugin (index 0)
if (!synth.activatePlugin(0))
{
    <<< "Failed to activate plugin" >>>;
    me.exit();
}

<<< "Plugin activated - ready to play MIDI" >>>;

// ===== MIDI Methods =====
//
// synth.noteOn(pitch, velocity)   - Send note on (pitch: 0-127, velocity: 0-127)
// synth.noteOff(pitch)             - Send note off (pitch: 0-127)
// synth.sendMIDI(0xB0, cc, value)   - Send control change (cc: 0-127, value: 0-127)
// synth.sendMIDI(status, data1, data2) - Send raw MIDI message

// Example 1: Simple note sequence
<<< "=== Example 1: Simple melody ===" >>>;
[60, 64, 67, 72, 67, 64, 60] @=> int notes[];

for (int i; i < notes.size(); i++)
{
    synth.noteOn(notes[i], 100);
    300::ms => now;
    synth.noteOff(notes[i]);
    50::ms => now;
}

// Example 2: Velocity sensitivity
<<< "=== Example 2: Velocity dynamics ===" >>>;
20 => int velStep;
for (int vel; vel < 127; vel + velStep => vel)
{
    synth.noteOn(60, vel);
    <<< "Velocity:", vel >>>;
    200::ms => now;
    synth.noteOff(60);
    100::ms => now;
}

// Example 3: Chord progression
<<< "=== Example 3: Chords ===" >>>;
// C major
synth.noteOn(60, 80);
synth.noteOn(64, 80);
synth.noteOn(67, 80);
500::ms => now;
synth.noteOff(60);
synth.noteOff(64);
synth.noteOff(67);
100::ms => now;

// F major
synth.noteOn(65, 80);
synth.noteOn(69, 80);
synth.noteOn(72, 80);
500::ms => now;
synth.noteOff(65);
synth.noteOff(69);
synth.noteOff(72);
100::ms => now;

// Example 4: Control changes
<<< "=== Example 4: MIDI CC sweep ===" >>>;
synth.noteOn(60, 100);

// Sweep a control change parameter
// Note: CC numbers vary by plugin - check plugin documentation
for (int cc; cc < 128; cc++)
{
    synth.controlChange(1, cc);  // CC 1 is modulation wheel
    10::ms => now;
}

synth.noteOff(60);

<<< "Demo complete!" >>>;
