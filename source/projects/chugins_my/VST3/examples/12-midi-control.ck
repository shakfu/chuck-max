// VST3 MIDI Control Example
// Demonstrates MIDI control change and program change messages

VST3 vst => dac;

<<< "=== VST3 MIDI Control Example ===" >>>;

// Replace with a valid VST3 instrument path on your system
"/Library/Audio/Plug-Ins/VST3/Serum2.vst3" => string pluginPath;

if (vst.load(pluginPath)) {
    <<< "Successfully loaded VST3 plugin!" >>>;

    if (vst.isInstrument()) {
        <<< "This is a VST3 instrument - testing MIDI control..." >>>;

        // Play a note
        <<< "Playing note 60..." >>>;
        vst.noteOn(60, 100);
        1::second => now;

        // Send control change (CC 1 = Modulation Wheel)
        <<< "Sending CC 1 (Mod Wheel) = 127..." >>>;
        vst.controlChange(1, 127);
        1::second => now;

        // Reset modulation
        <<< "Resetting CC 1 (Mod Wheel) = 0..." >>>;
        vst.controlChange(1, 0);
        1::second => now;

        // Send CC 7 (Volume)
        <<< "Sending CC 7 (Volume) = 64..." >>>;
        vst.controlChange(7, 64);
        1::second => now;

        // Note off
        vst.noteOff(60);
        500::ms => now;

        // Test program change if presets are available
        if (vst.presetCount() > 1) {
            <<< "Testing program change..." >>>;

            vst.noteOn(64, 100);
            1::second => now;

            <<< "Program change to 1..." >>>;
            vst.programChange(1);
            500::ms => now;

            vst.noteOff(64);
            500::ms => now;
        }

    } else {
        <<< "This plugin is not an instrument - MIDI control may not work" >>>;
    }

    vst.close();
} else {
    <<< "Failed to load VST3 plugin at:", pluginPath >>>;
    <<< "Run examples/00-list-vst3.ck to see available plugins" >>>;
}

<<< "=== Example complete ===" >>>;
