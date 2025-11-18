// Test Filter Sweep
// Sweeps a filter parameter to verify the effect is working

SinOsc osc => CLAP clap => Gain g => dac;
440 => osc.freq;
0.3 => osc.gain;
0.5 => g.gain;

"/Library/Audio/Plug-Ins/CLAP/TAL-Filter-2.clap" => string pluginPath;

if (clap.load(pluginPath)) {
    <<< "Successfully loaded CLAP plugin!" >>>;
    <<< "Parameter count:", clap.paramCount() >>>;

    // Print all parameters
    <<< "\nAvailable parameters:" >>>;
    for (0 => int i; i < clap.paramCount(); i++) {
        <<< "  [" + i + "]", clap.paramName(i) + ":", clap.getParam(i) >>>;
    }

    <<< "\nSweeping parameter 0 (" + clap.paramName(0) + ") from 0.0 to 1.0" >>>;
    <<< "Listen for changes in the tone...\n" >>>;

    // Sweep parameter 0 over 10 seconds
    now => time start;
    while (now - start < 10::second) {
        // Calculate sweep position (0.0 to 1.0)
        (now - start) / 10::second => float t;

        // Set parameter
        clap.setParam(0, t);

        // Print progress every 0.5 seconds
        if (((now - start) / 500::ms) $ int != (((now - start - 10::ms) / 500::ms) $ int)) {
            <<< "Time:", (now - start) / 1::second, "s  -  Param value:", t >>>;
        }

        10::ms => now;
    }

    <<< "\nNow sweeping parameter 5 (" + clap.paramName(5) + ") from 0.0 to 1.0" >>>;

    // Reset parameter 0
    clap.setParam(0, 0.5);

    // Sweep parameter 5 (Resonance) over 10 seconds
    now => start;
    while (now - start < 10::second) {
        (now - start) / 10::second => float t;
        clap.setParam(5, t);

        if (((now - start) / 500::ms) $ int != (((now - start - 10::ms) / 500::ms) $ int)) {
            <<< "Time:", (now - start) / 1::second, "s  -  Param value:", t >>>;
        }

        10::ms => now;
    }

    <<< "\nClosing plugin..." >>>;
    clap.close();
} else {
    <<< "Failed to load CLAP plugin at:", pluginPath >>>;
}
