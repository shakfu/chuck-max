// CLAP Instrument Example
// Demonstrates using CLAP plugins as sound generators/instruments

// Create CLAP instance and connect to dac
CLAP clap => dac;

<<< "=== CLAP Instrument Example ===" >>>;

// Replace with a valid CLAP instrument path on your system
// Example instruments: Surge XT, Vital, Dexed
"/Library/Audio/Plug-Ins/CLAP/TAL-NoiseMaker.clap" => string pluginPath;

if (clap.load(pluginPath)) {
    <<< "Successfully loaded CLAP plugin!" >>>;

    // Check if this is an instrument
    if (clap.isInstrument()) {
        <<< "This is a CLAP instrument - playing notes..." >>>;

        // Set gain to hear the output
        0.5 => clap.gain;

        // Play a simple melody
        [60, 64, 67, 72, 67, 64, 60] @=> int notes[];

        for (0 => int i; i < notes.size(); i++) {
            <<< "Playing note:", notes[i] >>>;
            clap.noteOn(notes[i], 100);  // Note on with velocity 100
            500::ms => now;
            clap.noteOff(notes[i]);      // Note off
            100::ms => now;
        }

        // Let reverb tail fade out
        1::second => now;

    } else {
        <<< "This plugin is not a CLAP instrument - it's an effect" >>>;
        <<< "Try loading an instrument plugin like Surge XT or Vital" >>>;
    }

    clap.close();
} else {
    <<< "Failed to load CLAP plugin at:", pluginPath >>>;
    <<< "Run examples/00-list-clap.ck to see available plugins" >>>;
}

<<< "=== Example complete ===" >>>;
