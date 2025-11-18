// More comprehensive MIDI test with TAL-NoiseMaker
// Plays a melody with varying velocities

// Create CLAP host
CLAP synth => dac;

// Load the TAL-NoiseMaker plugin
if (!synth.load("/Library/Audio/Plug-Ins/CLAP/TAL-NoiseMaker.clap"))
{
    <<< "Failed to load TAL-NoiseMaker plugin" >>>;
    me.exit();
}

<<< "Loaded TAL-NoiseMaker" >>>;

// Activate the plugin
if (!synth.activatePlugin(0))
{
    <<< "Failed to activate plugin" >>>;
    me.exit();
}

<<< "Plugin activated - playing melody" >>>;

// Define a melody (C major scale)
[60, 62, 64, 65, 67, 69, 71, 72] @=> int melody[];
[80, 90, 100, 110, 100, 90, 80, 100] @=> int velocities[];

// Play the melody
for (int i; i < melody.size(); i++)
{
    synth.noteOn(melody[i], velocities[i]);
    300::ms => now;
    synth.noteOff(melody[i]);
    50::ms => now;
}

<<< "Testing control change - setting filter cutoff" >>>;

// Play a note while sweeping filter cutoff
synth.noteOn(60, 100);

// Sweep control change (CC 74 is often filter cutoff in many synths)
for (int cc; cc < 128; cc++)
{
    synth.controlChange(74, cc);
    20::ms => now;
}

synth.noteOff(60);

<<< "Testing chord" >>>;

// Play a C major chord
synth.noteOn(60, 80);
synth.noteOn(64, 80);
synth.noteOn(67, 80);

1::second => now;

synth.noteOff(60);
synth.noteOff(64);
synth.noteOff(67);

<<< "MIDI test complete!" >>>;
