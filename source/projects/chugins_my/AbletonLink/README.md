# AbletonLink Chugin

A ChucK chugin that provides tempo synchronization capabilities using Ableton Link technology. This chugin allows ChucK programs to synchronize tempo and beat timing with other Link-enabled applications like Ableton Live, Max/MSP, and other music software.

## Overview

Ableton Link is a technology that synchronizes beat, tempo, and phase across multiple applications running on devices connected to a local network. This chugin embeds the [Ableton Link library](https://github.com/Ableton/link) into ChucK, enabling seamless integration with the Link ecosystem.

## Features

- Real-time tempo synchronization with other Link-enabled applications
- Beat grid synchronization with configurable quantum
- Network-based peer discovery and synchronization
- Low-latency audio thread integration
- Customizable resolution for sub-beat timing
- Offset compensation for latency adjustment

## Usage

### Basic Setup

```chuck
// Create an AbletonLink instance
AbletonLink link => dac;

// Enable Link networking
1 => link.enable;

// Set initial tempo (optional)
120.0 => link.tempo;

// Set quantum (measures in beats)
4 => link.quantum;
```

### Getting Synchronized Timing

```chuck
// The AbletonLink UGen outputs step values based on the current beat position
while(true) {
    // Read the current step value
    float step => link => blackhole;

    // Trigger events based on step changes
    if (step != previousStep) {
        // Play sound on each step
        triggerSound();
        step => previousStep;
    }

    1::samp => now;
}
```

## Methods and Properties

### Enable/Disable Link

```chuck
// Enable Link networking
1 => link.enable;

// Disable Link networking
0 => link.enable;
```

### Tempo Control

```chuck
// Set tempo (BPM)
140.0 => link.tempo;

// Get current tempo
link.tempo() => float currentTempo;
```

### Quantum Configuration

The quantum defines the beat grid measurement (typically 4 beats = 1 bar):

```chuck
// Set quantum to 4 beats (1 bar)
4 => link.quantum;

// Get current quantum
link.quantum() => int currentQuantum;
```

### Step Resolution

Control the subdivision resolution within each beat:

```chuck
// Set resolution (steps per beat)
// 1 = quarter notes, 2 = eighth notes, 4 = sixteenth notes
4 => link.resolution;

// Get current resolution
link.resolution() => int currentResolution;
```

### Latency Compensation

```chuck
// Set offset in milliseconds for latency compensation
5.0 => link.offset;
```

### Transport Reset

```chuck
// Reset the beat timeline
1 => link.reset;
```

## Output Behavior

The AbletonLink UGen outputs step values that increment based on the current beat position and resolution setting:

- With resolution = 1: outputs 0, 1, 2, 3 (quarter notes in 4/4 time)
- With resolution = 2: outputs 0, 1, 2, 3, 4, 5, 6, 7 (eighth notes in 4/4 time)
- With resolution = 4: outputs 0-15 (sixteenth notes in 4/4 time)

Step values reset to 0 at the beginning of each quantum period.

## Example: Simple Metronome

```chuck
// Create Link instance
AbletonLink link => dac;

// Configure Link
1 => link.enable;
120.0 => link.tempo;
4 => link.quantum;
1 => link.resolution;  // Quarter note resolution

// Sound for metronome
SinOsc click => ADSR env => dac;
env.set(1::ms, 1::ms, 0.0, 1::ms);

float lastStep = -1;

while(true) {
    // Get current step
    link.last() => float currentStep;

    // Trigger click on step changes
    if (currentStep != lastStep && currentStep >= 0) {
        // Higher pitch on beat 1
        if (currentStep % 4 == 0) {
            800 => click.freq;
        } else {
            400 => click.freq;
        }

        // Trigger envelope
        env.keyOn();
        10::ms => now;
        env.keyOff();

        currentStep => lastStep;
    }

    1::samp => now;
}
```

## Network Synchronization

AbletonLink automatically discovers other Link-enabled applications on the local network. When multiple applications are running Link:

1. They automatically synchronize to a shared tempo
2. Beat grids align based on quantum settings
3. Tempo changes in any application propagate to all others
4. The most recent tempo change takes precedence

## Performance Considerations

- The AbletonLink UGen is designed for real-time audio thread usage
- Network synchronization adds minimal latency
- Use appropriate resolution settings to balance precision and performance
- The chugin handles thread safety internally

## Compatibility

This chugin is compatible with:
- Ableton Live (with Link enabled)
- Max/MSP (with link~ object)
- Other ChucK instances running AbletonLink
- Any application supporting Ableton Link protocol

## Technical Details

- Built with Ableton Link SDK (latest version)
- Cross-platform support (macOS, Linux, Windows)
- Thread-safe implementation
- Low-latency audio thread integration
- Network discovery via UDP multicast

## Building

The AbletonLink chugin is built as part of the chuck-max build system. It requires:
- Ableton Link library (included in the link/ subdirectory)
- ASIO standalone library (for network functionality on Winoows)
- Platform-specific frameworks (CoreFoundation, CoreServices on macOS)

See the main chuck-max documentation for build instructions.
