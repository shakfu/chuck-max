// CLAP Bypass Example
// Demonstrates toggling CLAP bypass to compare processed vs unprocessed audio

PulseOsc osc => CLAP clap => Gain g => dac;
220 => osc.freq;
0.3 => osc.gain;
0.3 => g.gain;

<<< "=== CLAP Bypass Example ===" >>>;

// Replace with a valid CLAP plugin path on your system
"/Library/Audio/Plug-Ins/CLAP/TAL-Reverb-4.clap" => string pluginPath;

if (clap.load(pluginPath)) {
    <<< "Successfully loaded CLAP plugin!" >>>;

    // Process audio normally
    <<< "Processing audio (effect enabled)..." >>>;
    3::second => now;

    // Bypass the effect - audio passes through unchanged
    <<< "Bypassing effect..." >>>;
    clap.bypass(1);
    3::second => now;

    // Re-enable the effect
    <<< "Re-enabling effect..." >>>;
    clap.bypass(0);
    3::second => now;

    // Toggle a few more times for comparison
    <<< "Toggling bypass on/off..." >>>;
    for (0 => int i; i < 3; i++) {
        <<< "Bypass ON" >>>;
        clap.bypass(1);
        1::second => now;

        <<< "Bypass OFF" >>>;
        clap.bypass(0);
        1::second => now;
    }

    clap.close();
} else {
    <<< "Failed to load CLAP plugin at:", pluginPath >>>;
    <<< "Run examples/00-list-clap.ck to see available plugins" >>>;
}

<<< "=== Example complete ===" >>>;
