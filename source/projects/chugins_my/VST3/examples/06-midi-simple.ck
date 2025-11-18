// VST3 Simple MIDI Example
// Demonstrates basic MIDI control of a VST3 instrument

// Create VST3 instance
VST3 vst => dac;

<<< "=== VST3 Simple MIDI Example ===" >>>;

// Replace with a valid VST3 instrument path on your system
"/Library/Audio/Plug-Ins/VST3/Serum2.vst3" => string pluginPath;

if (vst.load(pluginPath)) {
    <<< "Successfully loaded VST3 plugin!" >>>;

    if (vst.isInstrument()) {
        <<< "Playing a simple 6-note melody..." >>>;

        // Define melody
        [60, 62, 64, 65, 67, 69] @=> int notes[];

        // Play each note
        for (0 => int i; i < notes.size(); i++) {
            vst.noteOn(notes[i], 100);
            400::ms => now;
            vst.noteOff(notes[i]);
            100::ms => now;
        }

        // Let sound fade out
        500::ms => now;

    } else {
        <<< "This plugin is not an instrument" >>>;
    }

    vst.close();
} else {
    <<< "Failed to load VST3 plugin at:", pluginPath >>>;
    <<< "Run examples/00-list-vst3.ck to see available plugins" >>>;
}

<<< "=== Example complete ===" >>>;
