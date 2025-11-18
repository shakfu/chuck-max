// Test Volume Parameter
// Tests if ANY parameter changes work by sweeping volume

SinOsc osc => CLAP clap => Gain g => dac;
440 => osc.freq;
0.3 => osc.gain;
0.5 => g.gain;

"/Library/Audio/Plug-Ins/CLAP/TAL-Filter-2.clap" => string pluginPath;

if (clap.load(pluginPath)) {
    <<< "Successfully loaded CLAP plugin!" >>>;

    // Find Volume Out parameter (should be parameter 3)
    for (0 => int i; i < clap.paramCount(); i++) {
        if (clap.paramName(i).find("Volume Out") >= 0) {
            <<< "Found Volume Out at index", i >>>;
            <<< "Current value:", clap.getParam(i) >>>;

            <<< "\nSweeping Volume Out from 1.0 down to 0.0 over 5 seconds" >>>;
            <<< "You should hear the volume decrease...\n" >>>;

            now => time start;
            while (now - start < 5::second) {
                // Sweep from 1.0 to 0.0
                1.0 - ((now - start) / 5::second) => float volume;
                clap.setParam(i, volume);

                if (((now - start) / 500::ms) $ int != (((now - start) - 10::ms) / 500::ms) $ int)) {
                    <<< "Volume:", volume >>>;
                }

                10::ms => now;
            }

            <<< "\nNow sweeping back up from 0.0 to 1.0 over 5 seconds\n" >>>;

            now => start;
            while (now - start < 5::second) {
                (now - start) / 5::second => float volume;
                clap.setParam(i, volume);

                if (((now - start) / 500::ms) $ int != (((now - start) - 10::ms) / 500::ms) $ int)) {
                    <<< "Volume:", volume >>>;
                }

                10::ms => now;
            }

            break;
        }
    }

    clap.close();
} else {
    <<< "Failed to load plugin" >>>;
}
