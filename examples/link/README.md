# AbletonLink Chugin Examples

This directory contains ChucK examples demonstrating the `AbletonLink` chugin for tempo synchronization across applications. These examples show various aspects of using Ableton Link for collaborative music making and tight timing synchronization.

## Examples Overview

### basic_metronome.ck

A simple metronome that demonstrates the fundamental concepts of `AbletonLink` integration:
- Basic Link setup and configuration
- Reading synchronized timing information
- Generating audio events based on Link timing
- Different sounds for downbeats vs other beats

**Key concepts**: Link enable, tempo setting, quantum configuration, step detection

### tempo_control.ck

Shows how to control and monitor tempo changes with `AbletonLink`:
- Setting tempo from ChucK
- Monitoring tempo changes from other applications
- Cycling through different tempos automatically
- Real-time tempo synchronization feedback

**Key concepts**: Tempo setting/getting, external tempo monitoring, bidirectional sync

### drum_machine.ck

A synchronized drum machine using sixteenth-note resolution:
- Multi-track drum patterns (kick, snare, hi-hat)
- High-resolution timing (16 steps per measure)
- Pattern-based sequencing
- Multiple simultaneous sound sources

**Key concepts**: High resolution timing, pattern programming, multiple instruments

### sequencer.ck

Musical sequencer playing melodic patterns:
- Eighth-note melodic sequences
- MIDI note to frequency conversion
- Pattern switching and cycling
- Musical timing and note articulation

**Key concepts**: Musical sequences, note triggering, pattern progression

### quantum_sync.ck

Demonstrates quantum synchronization and measure alignment:
- Different quantum values (1, 2, 4, 8 beats)
- Measure boundary detection
- Dynamic quantum changes
- Cross-application measure alignment

**Key concepts**: Quantum configuration, measure alignment, dynamic parameter changes

### latency_test.ck

Tests and demonstrates latency compensation:
- Offset adjustment for timing compensation
- Testing different offset values
- Tight synchronization techniques
- Latency measurement and correction

**Key concepts**: Latency compensation, offset adjustment, timing precision

## Getting Started

1. Ensure the `AbletonLink` chugin is properly built and available
2. Start with `basic_metronome.ck` to understand fundamental concepts
3. Run multiple examples simultaneously to see synchronization
4. Try connecting with other Link-enabled applications like Ableton Live

## Running Examples

```bash
# Basic metronome
chuck basic_metronome.ck

# Run multiple examples for ensemble playing
chuck basic_metronome.ck &
chuck drum_machine.ck &
chuck sequencer.ck
```

## Link Network Setup

- All examples automatically discover other Link applications on the local network
- Start any Link-enabled application (Ableton Live, Max/MSP with link~, etc.)
- Examples will synchronize automatically
- Tempo changes in any application affect all connected apps

## Customization Tips

### Changing Tempo

```chuck
// Set tempo in any example
140.0 => link.tempo;
```

### Adjusting Resolution

```chuck
// Quarter notes
1 => link.resolution;

// Eighth notes
2 => link.resolution;

// Sixteenth notes
4 => link.resolution;
```

### Setting Quantum

```chuck
// 4/4 time (4 beats per measure)
4 => link.quantum;

// 3/4 time (3 beats per measure)
3 => link.quantum;

// 8 beats per measure
8 => link.quantum;
```

### Latency Compensation

```chuck
// Add 5ms offset for latency compensation
5.0 => link.offset;

// Negative offset for early triggering
-3.0 => link.offset;
```

## Common Patterns

### Step Detection

```chuck
float lastStep = -1;

while(true) {
    link.last() => float currentStep;

    if (currentStep != lastStep && currentStep >= 0) {
        // New step detected - trigger events here
        currentStep => lastStep;
    }

    1::samp => now;
}
```

### Downbeat Detection

```chuck
if (step % link.quantum() == 0) {
    // This is a downbeat (start of measure)
}
```

### Pattern Indexing

```chuck
// For patterns with N steps
currentStep $ int % N => int patternIndex;
```

## Troubleshooting

- **No synchronization**: Check that Link is enabled (`1 => link.enable`)
- **Timing issues**: Try adjusting the offset value
- **Network problems**: Ensure all devices are on the same local network
- **Performance**: Use appropriate resolution settings for your needs

## Performance Notes

- Higher resolutions (more steps per beat) require more CPU
- Use `1::samp => now` for tightest timing in audio loops
- Consider using `spork` for non-critical timing tasks
- The Link chugin is optimized for real-time audio thread usage

## Further Reading

See the main ``AbletonLink`` chugin README.md for detailed API documentation and technical information.