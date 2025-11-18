// tempo_control.ck
// Demonstrates tempo setting and monitoring with AbletonLink
// Shows how to set tempo from ChucK and respond to tempo changes from other apps

// Create Link instance
AbletonLink link => blackhole;

// Configure Link
1 => link.enable;
4 => link.quantum;
2 => link.resolution;  // Eighth note resolution

<<< "AbletonLink Tempo Control Demo" >>>;
<<< "This example changes tempo every 8 beats" >>>;
<<< "Watch how other Link apps follow!" >>>;

// Tempo sequence to cycle through
[100.0, 120.0, 140.0, 160.0, 180.0] @=> float tempos[];
0 => int tempoIndex;

0 => float lastTempo;
-1 => float lastStep;
0 => int beatCount;

while(true) {
    // Monitor current tempo
    link.tempo() => float currentTempo;

    // Report tempo changes from external sources
    if (currentTempo != lastTempo) {
        <<< "Tempo changed to:", currentTempo, "BPM" >>>;
        currentTempo => lastTempo;
    }

    // Get current step
    link.last() => float currentStep;

    // Count beats and change tempo every 8 beats
    if (currentStep != lastStep && currentStep >= 0) {
        if (currentStep % 2 == 0) {  // On each beat (eighth note resolution)
            beatCount++;
            <<< "Beat:", beatCount, "Step:", currentStep, "Tempo:", currentTempo >>>;

            // Change tempo every 8 beats
            if (beatCount % 8 == 0) {
                tempos[tempoIndex] => link.tempo;
                <<< "*** Setting tempo to:", tempos[tempoIndex], "BPM ***" >>>;
                (tempoIndex + 1) % tempos.cap() => tempoIndex;
            }
        }

        currentStep => lastStep;
    }

    10::ms => now;
}