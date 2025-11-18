// Test CLAP MIDI functionality with TAL-NoiseMaker synth
// This tests note on/off and control changes

// Create CLAP host
CLAP synth => dac;

// Load the TAL-NoiseMaker plugin
if (!synth.load("/Library/Audio/Plug-Ins/CLAP/TAL-NoiseMaker.clap"))
{
    <<< "Failed to load TAL-NoiseMaker plugin" >>>;
    me.exit();
}

// List available plugins
<<< "Available plugins:" >>>;
synth.listPlugins();

// Activate the first plugin (index 0)
if (!synth.activatePlugin(0))
{
    <<< "Failed to activate plugin" >>>;
    me.exit();
}

<<< "Plugin activated successfully" >>>;
<<< "Playing MIDI notes..." >>>;

// Test a simple melody
[60, 64, 67, 72] @=> int notes[];

for (int i; i < notes.size(); i++)
{
    <<< "Note On:", notes[i] >>>;
    synth.noteOn(notes[i], 100);
    500::ms => now;

    <<< "Note Off:", notes[i] >>>;
    synth.noteOff(notes[i]);
    100::ms => now;
}

<<< "Test complete" >>>;
