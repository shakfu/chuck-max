// Check what CLAP extensions TAL-Filter-2 supports

CLAP clap; Gain g;
1 => g.gain;
clap => g => dac;

"/Library/Audio/Plug-Ins/CLAP/TAL-Filter-2.clap" => string pluginPath;

if (clap.load(pluginPath)) {
    <<< "Successfully loaded TAL-Filter-2!" >>>;
    <<< "" >>>;
    <<< "Plugin info:" >>>;
    <<< "  Parameters:", clap.paramCount() >>>;
    <<< "  Presets (via discovery):", clap.presetCount() >>>;
    <<< "  Is instrument:", clap.isInstrument() >>>;
    <<< "" >>>;

    clap.close();
} else {
    <<< "Failed to load plugin" >>>;
}
