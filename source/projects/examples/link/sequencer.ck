// sequencer.ck
// Musical sequencer synchronized with AbletonLink
// Demonstrates melodic patterns with tempo synchronization

// Create Link instance
AbletonLink link => blackhole;

// Musical setup
SinOsc osc => ADSR env => LPF filter => Gain g => dac;

.6 => g.gain;
env.set(10::ms, 100::ms, 0.7, 200::ms);
filter.freq(2000);
filter.Q(2);

// Configure Link for eighth note sequences
1 => link.enable;
120.0 => link.tempo;
4 => link.quantum;
2 => link.resolution;  // Eighth note resolution

<<< "AbletonLink Musical Sequencer" >>>;
<<< "Playing melodic patterns synchronized with Link" >>>;
<<< "8 steps per 4/4 measure (eighth notes)" >>>;

// Musical sequence (8 eighth notes per measure)
// MIDI note numbers for C major scale pattern
[60, 62, 64, 65, 67, 65, 64, 62] @=> int sequence1[];  // C D E F G F E D
[67, 69, 71, 72, 71, 69, 67, 65] @=> int sequence2[];  // G A B C B A G F
[72, 71, 69, 67, 65, 67, 69, 71] @=> int sequence3[];  // C B A G F G A B

// Array of sequences to cycle through
[sequence1, sequence2, sequence3] @=> int sequences[][];
0 => int currentSeq;

-1 => float lastStep;
0 => int patternCount;

while(true) {
    link.last() => float currentStep;

    if (currentStep != lastStep && currentStep >= 0) {
        currentStep $ int => int step;
        step % 8 => int seqStep;  // 8 steps per pattern

        // Get current sequence
        sequences[currentSeq] @=> int currentSequence[];

        // Convert MIDI note to frequency and play
        currentSequence[seqStep] => int midiNote;
        Std.mtof(midiNote) => osc.freq;

        // Trigger note
        env.keyOn();

        <<< "Step:", seqStep, "Note:", midiNote, "Freq:", osc.freq(), "Tempo:", link.tempo() >>>;

        // Change sequence every 2 measures (16 eighth notes)
        if (step > 0 && step % 16 == 0) {
            patternCount++;
            (currentSeq + 1) % sequences.cap() => currentSeq;
            <<< "*** Switching to sequence", currentSeq + 1, "***" >>>;
        }

        currentStep => lastStep;
    }

    // Release note after a short time
    if (env.state() == 1) {  // If attack phase
        80::ms => now;
        env.keyOff();
    } else {
        1::samp => now;
    }
}