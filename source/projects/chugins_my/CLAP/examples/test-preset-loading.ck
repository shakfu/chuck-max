// Test Preset Loading from Files
// Loads TAL-Filter-2 presets from disk and demonstrates the filter effect

SinOsc osc => CLAP clap => Gain g => dac;
440 => osc.freq;
0.3 => osc.gain;
0.5 => g.gain;

"/Library/Audio/Plug-Ins/CLAP/TAL-Filter-2.clap" => string pluginPath;

if (clap.load(pluginPath)) {
    <<< "Successfully loaded TAL-Filter-2!" >>>;
    <<< "Parameter count:", clap.paramCount() >>>;
    <<< "" >>>;

    // Define preset paths
    "/Users/sa/Library/Application Support/ToguAudioLine/TAL-Filter-2/presets/default.talfilter2" => string defaultPreset;
    "/Users/sa/Library/Application Support/ToguAudioLine/TAL-Filter-2/presets/TAL/FILTER/LP Slow Sine.talfilter2" => string preset1;
    "/Users/sa/Library/Application Support/ToguAudioLine/TAL-Filter-2/presets/TAL/FILTER/HP Shizzle.talfilter2" => string preset2;
    "/Users/sa/Library/Application Support/ToguAudioLine/TAL-Filter-2/presets/TAL/FILTER/LP Tremolo.talfilter2" => string preset3;

    // Test 1: Play with default settings
    <<< "=== Test 1: Default settings ===" >>>;
    <<< "Playing sine wave for 3 seconds..." >>>;
    3::second => now;

    // Test 2: Load first preset
    <<< "" >>>;
    <<< "=== Test 2: LP Slow Sine preset ===" >>>;
    if (clap.loadPresetFile(preset1)) {
        <<< "Preset loaded successfully!" >>>;
        <<< "Playing for 5 seconds..." >>>;
        5::second => now;
    } else {
        <<< "Failed to load preset" >>>;
    }

    // Test 3: Load second preset
    <<< "" >>>;
    <<< "=== Test 3: HP Shizzle preset ===" >>>;
    if (clap.loadPresetFile(preset2)) {
        <<< "Preset loaded successfully!" >>>;
        <<< "Playing for 5 seconds..." >>>;
        5::second => now;
    } else {
        <<< "Failed to load preset" >>>;
    }

    // Test 4: Load third preset
    <<< "" >>>;
    <<< "=== Test 4: LP Tremolo preset ===" >>>;
    if (clap.loadPresetFile(preset3)) {
        <<< "Preset loaded successfully!" >>>;
        <<< "Playing for 5 seconds..." >>>;
        5::second => now;
    } else {
        <<< "Failed to load preset" >>>;
    }

    <<< "" >>>;
    <<< "=== Test complete ===" >>>;
    clap.close();
} else {
    <<< "Failed to load plugin" >>>;
}
