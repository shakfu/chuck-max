// quantum_sync.ck
// Demonstrates quantum synchronization and measure alignment
// Shows how different quantum settings affect synchronization

// Create Link instance
AbletonLink link => blackhole;

// Sound for measure markers
TriOsc bell => ADSR env => Gain g => dac;

0.5 => g.gain;
env.set(10::ms, 200::ms, 0.3, 500::ms);

// Configure Link
1 => link.enable;
120.0 => link.tempo;
1 => link.resolution;  // Quarter note resolution

<<< "AbletonLink Quantum Synchronization Demo" >>>;
<<< "This example cycles through different quantum values" >>>;
<<< "Quantum affects how measures align across Link apps" >>>;

// Different quantum values to try
[1, 2, 4, 8] @=> int quantums[];
0 => int quantumIndex;

-1 => float lastStep;
0 => int measureCount;

// Start with quantum = 4 (standard 4/4 time)
4 => link.quantum;

while(true) {
    link.last() => float currentStep;

    if (currentStep != lastStep && currentStep >= 0) {
        currentStep $ int => int step;

        // Check if we're at the start of a new measure
        if (step % link.quantum() == 0) {
            measureCount++;

            // Different bell tones for different quantum values
            if (link.quantum() == 1) 400 => bell.freq;
            else if (link.quantum() == 2) 500 => bell.freq;
            else if (link.quantum() == 4) 600 => bell.freq;
            else if (link.quantum() == 8) 700 => bell.freq;

            env.keyOn();

            <<< "*** MEASURE", measureCount, "START ***" >>>;
            <<< "Quantum:", link.quantum(), "Step:", step, "Tempo:", link.tempo() >>>;

            // Change quantum every 4 measures
            if (measureCount % 4 == 0) {
                quantums[quantumIndex] => link.quantum;
                <<< ">>> Changing quantum to:", quantums[quantumIndex] >>>;
                (quantumIndex + 1) % quantums.cap() => quantumIndex;
            }
        } else {
            // Beat within measure
            <<< "Beat", (step % link.quantum()) + 1, "of", link.quantum(), "- Step:", step >>>;
        }

        currentStep => lastStep;
    }

    1::samp => now;
}