// drum_machine.ck
// Synchronized drum machine using AbletonLink
// Demonstrates different resolution settings and pattern programming

// Create Link instance
AbletonLink link => blackhole;

// Drum sounds
SinOsc kick => ADSR kickEnv => dac;
Noise snare => BPF snareFilter => ADSR snareEnv => Gain gs => dac;
Noise hihat => HPF hihatFilter => ADSR hihatEnv => Gain gh => dac;

// Set gain
.6 => gs.gain;
.3 => gh.gain;

// Configure drum sounds
kickEnv.set(5::ms, 50::ms, 0.0, 5::ms);
snareEnv.set(1::ms, 100::ms, 0.0, 1::ms);
hihatEnv.set(1::ms, 50::ms, 0.0, 1::ms);

1000 => snareFilter.freq;
2.0 => snareFilter.Q;
8000 => hihatFilter.freq;

// Configure Link for sixteenth note resolution
1 => link.enable;
120.0 => link.tempo;
4 => link.quantum;
4 => link.resolution;  // Sixteenth note resolution (16 steps per bar)

<<< "AbletonLink Drum Machine" >>>;
<<< "Tempo: 120 BPM, 16 steps per bar" >>>;
<<< "Kick: steps 0, 4, 8, 12" >>>;
<<< "Snare: steps 4, 12" >>>;
<<< "Hi-hat: steps 2, 6, 10, 14" >>>;

// Drum patterns (16 steps per bar)
[1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0] @=> int kickPattern[];
[0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0] @=> int snarePattern[];
[0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0] @=> int hihatPattern[];

-1 => float lastStep;

while(true) {
    link.last() => float currentStep;

    if (currentStep != lastStep && currentStep >= 0) {
        currentStep $ int => int stepIndex;
        stepIndex % 16 => stepIndex;  // Wrap to 16 steps

        <<< "Step:", stepIndex, "Tempo:", link.tempo() >>>;

        // Trigger drum sounds based on patterns
        if (kickPattern[stepIndex]) {
            60 => kick.freq;
            kickEnv.keyOn();
            spork ~ releaseAfter(kickEnv, 10::ms);
        }

        if (snarePattern[stepIndex]) {
            snareEnv.keyOn();
            spork ~ releaseAfter(snareEnv, 10::ms);
        }

        if (hihatPattern[stepIndex]) {
            hihatEnv.keyOn();
            spork ~ releaseAfter(hihatEnv, 5::ms);
        }

        currentStep => lastStep;
    }

    1::samp => now;
}

// Helper function to release envelope after delay
fun void releaseAfter(ADSR env, dur delay) {
    delay => now;
    env.keyOff();
}