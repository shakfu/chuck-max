// latency_test.ck
// Tests and demonstrates latency compensation with AbletonLink
// Shows how to use offset adjustment for tight synchronization

// Create Link instance
AbletonLink link => blackhole;

// Sound for timing reference
SinOsc beep => ADSR env => Gain g => dac;

0.8 => g.gain;
env.set(1::ms, 10::ms, 0.0, 1::ms);

// Configure Link
1 => link.enable;
120.0 => link.tempo;
4 => link.quantum;
4 => link.resolution;  // Sixteenth notes for precise timing

<<< "AbletonLink Latency Compensation Test" >>>;
<<< "This example demonstrates offset adjustment" >>>;
<<< "Listen for timing relative to other Link apps" >>>;

// Test different offset values
[0.0, 5.0, 10.0, -5.0, -10.0] @=> float offsets[];
0 => int offsetIndex;

-1 => float lastStep;
0 => int measureCount;

// Start with no offset
0.0 => link.offset;


while(true) {
    link.last() => float currentStep;

    if (currentStep != lastStep && currentStep >= 0) {
        currentStep $ int => int step;

        // Beep on every fourth sixteenth note (quarter notes)
        if (step % 4 == 0) {
            if (step % 16 == 0) {
                // Downbeat - higher pitch
                800 => beep.freq;
                measureCount++;
                //<<< "=== MEASURE", measureCount, "=== Offset:", link.offset(), "ms" >>>;

                // Change offset every 4 measures
                if (measureCount % 4 == 0) {
                    offsets[offsetIndex] => link.offset;
                    <<< ">>> Changing offset to:", offsets[offsetIndex], "ms" >>>;
                    (offsetIndex + 1) % offsets.cap() => offsetIndex;
                }
            } else {
                // Other beats - lower pitch
                400 => beep.freq;
            }

            env.keyOn();
            //<<< "Beat - Step:", step, "Offset:", link.offset() >>>;
        }

        currentStep => lastStep;
    }

    1::samp => now;
}