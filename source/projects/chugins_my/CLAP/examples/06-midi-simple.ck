// CLAP Simple MIDI Example
// Demonstrates basic MIDI control of a CLAP instrument

// Create CLAP instance
CLAP clap => dac;

<<< "=== CLAP Simple MIDI Example ===" >>>;

// Replace with a valid CLAP instrument path on your system
"/Library/Audio/Plug-Ins/CLAP/TAL-NoiseMaker.clap" => string pluginPath;

if (clap.load(pluginPath)) {
    <<< "Successfully loaded CLAP plugin!" >>>;

    if (clap.isInstrument()) {
        <<< "Playing a simple 6-note melody..." >>>;

        // Set gain
        0.5 => clap.gain;

        // Define melody
        [60, 62, 64, 65, 67, 69] @=> int notes[];

        // Play each note
        for (0 => int i; i < notes.size(); i++) {
            clap.noteOn(notes[i], 100);
            400::ms => now;
            clap.noteOff(notes[i]);
            100::ms => now;
        }

        // Let sound fade out
        500::ms => now;

    } else {
        <<< "This plugin is not an instrument" >>>;
    }

    clap.close();
} else {
    <<< "Failed to load CLAP plugin at:", pluginPath >>>;
    <<< "Run examples/00-list-clap.ck to see available plugins" >>>;
}

<<< "=== Example complete ===" >>>;
