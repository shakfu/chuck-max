// VST3 Instrument Example
// Demonstrates using VST3 plugins as sound generators/instruments

// Create VST3 instance and connect to dac
VST3 vst => dac;

<<< "=== VST3 Instrument Example ===" >>>;

// Replace with a valid VST3 instrument path on your system
// Example instruments: Any VST3 synth
"/Library/Audio/Plug-Ins/VST3/Serum2.vst3" => string pluginPath;

if (vst.load(pluginPath)) {
    <<< "Successfully loaded VST3 plugin!" >>>;

    // Check if this is an instrument
    if (vst.isInstrument()) {
        <<< "This is a VST3 instrument - playing notes..." >>>;

        // Play a simple melody
        [60, 64, 67, 72, 67, 64, 60] @=> int notes[];

        for (0 => int i; i < notes.size(); i++) {
            <<< "Playing note:", notes[i] >>>;
            vst.noteOn(notes[i], 100);  // Note on with velocity 100
            500::ms => now;
            vst.noteOff(notes[i]);      // Note off
            100::ms => now;
        }

        // Let reverb tail fade out
        1::second => now;

    } else {
        <<< "This plugin is not a VST3 instrument - it's an effect" >>>;
        <<< "Try loading an instrument plugin" >>>;
    }

    vst.close();
} else {
    <<< "Failed to load VST3 plugin at:", pluginPath >>>;
    <<< "Run examples/00-list-vst3.ck to see available plugins" >>>;
}

<<< "=== Example complete ===" >>>;
