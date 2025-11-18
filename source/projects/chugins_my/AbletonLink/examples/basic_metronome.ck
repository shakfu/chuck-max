// basic_metronome.ck
// Basic metronome using AbletonLink for tempo synchronization
// Demonstrates fundamental Link usage with click generation

// Create Link instance
AbletonLink link => blackhole;

// Sound generation for metronome clicks
SinOsc click => ADSR env => dac;
env.set(1::ms, 1::ms, 0.0, 1::ms);

// Configure Link
1 => link.enable;           // Enable Link networking
120.0 => link.tempo;        // Set initial tempo to 120 BPM
4 => link.quantum;          // 4 beats per measure
1 => link.resolution;       // Quarter note resolution

<<< "Basic Metronome with AbletonLink" >>>;
<<< "Tempo: 120 BPM, Quantum: 4 beats" >>>;
<<< "Connect other Link apps to sync!" >>>;

-1 => float lastStep;

while(true) {
    // Get current step from Link
    link.last() => float currentStep;

    // Check for step changes
    if (currentStep != lastStep && currentStep >= 0) {
        // Different pitch for downbeat vs other beats
        if (currentStep % 4 == 0) {
            800 => click.freq;  // Higher pitch on beat 1
            <<< "BEAT 1 - Step:", currentStep >>>;
        } else {
            400 => click.freq;  // Lower pitch on other beats
            <<< "beat", (currentStep % 4) + 1, "- Step:", currentStep >>>;
        }

        // Trigger click
        env.keyOn();
        10::ms => now;
        env.keyOff();

        currentStep => lastStep;
    }

    1::samp => now;
}