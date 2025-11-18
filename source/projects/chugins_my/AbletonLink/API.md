# AbletonLink Chugin API Reference

The AbletonLink chugin enables tempo synchronization between ChucK and other applications using Ableton Link technology. Link is a protocol that synchronizes musical beat, tempo, phase, and start/stop commands across multiple applications on a local network.

## Table of Contents

- [Overview](#overview)
- [Core Concepts](#core-concepts)
- [Configuration](#configuration)
- [Tempo Control](#tempo-control)
- [Output Signal](#output-signal)
- [Synchronization](#synchronization)
- [Network Discovery](#network-discovery)
- [Usage Patterns](#usage-patterns)

---

## Overview

AbletonLink is a **UGen** (unit generator) that outputs step values based on synchronized timing. It connects ChucK to the Link network, allowing synchronization with:
- Ableton Live
- Other DAWs with Link support
- Mobile apps (iOS/Android)
- Other ChucK instances
- Any Link-enabled application

**Key Features:**
- Automatic network peer discovery
- Tempo synchronization across applications
- Quantum-based measure alignment
- Configurable beat resolution
- Latency compensation

---

## Core Concepts

### Tempo
Musical tempo in beats per minute (BPM). The Link network maintains a shared tempo that can be set by any peer.

### Quantum
Number of beats that define a measure or synchronization boundary. Common values:
- `1` - Synchronize on every beat
- `2` - Half-note measures (2/4 time)
- `4` - Standard measures (4/4 time)
- `8` - Two-measure phrases

### Resolution
Number of steps per beat (subdivisions):
- `1` - Quarter notes (4 steps per measure in 4/4)
- `2` - Eighth notes (8 steps per measure in 4/4)
- `4` - Sixteenth notes (16 steps per measure in 4/4)

### Step
The current position in the synchronized timeline, incremented based on resolution. Steps are integers that increase with time.

---

## Configuration

### `enable(int enabled) -> int`

Enable or disable Link networking.

**Parameters:**
- `enabled` - `1` to enable Link, `0` to disable

**Returns:** The value set

**Notes:**
- Must be enabled to sync with other Link applications
- When disabled, tempo and quantum can still be set locally
- Network discovery only happens when enabled

**Example:**
```chuck
AbletonLink link => blackhole;

// Enable Link networking
1 => link.enable;

// Disable Link networking
0 => link.enable;
```

### `tempo(float bpm) -> float`

Set the tempo in beats per minute.

**Parameters:**
- `bpm` - Tempo in BPM (typically 40.0 to 240.0)

**Returns:** The tempo value set

**Notes:**
- Setting tempo proposes it to the Link network
- Other peers can override the tempo
- Use `tempo()` getter to read current network tempo

**Example:**
```chuck
AbletonLink link => blackhole;

// Set tempo to 120 BPM
120.0 => link.tempo;

// Set tempo to 140 BPM
140.0 => link.tempo;
```

### `quantum(int beats) -> int`

Set the quantum (measure length in beats).

**Parameters:**
- `beats` - Number of beats per quantum (typically 1, 2, 4, or 8)

**Returns:** The quantum value set

**Notes:**
- Defines synchronization boundaries for measure alignment
- Affects when other Link apps align to the same measure
- Does not change the output directly, but affects phase calculations

**Example:**
```chuck
AbletonLink link => blackhole;

// 4/4 time signature
4 => link.quantum;

// 3/4 time signature (waltz)
3 => link.quantum;

// 8-beat phrase
8 => link.quantum;
```

### `resolution(int steps_per_beat) -> int`

Set the resolution (subdivisions per beat).

**Parameters:**
- `steps_per_beat` - Number of steps per beat

**Returns:** The resolution value set

**Common Values:**
- `1` - Quarter note resolution
- `2` - Eighth note resolution
- `4` - Sixteenth note resolution
- `8` - Thirty-second note resolution

**Notes:**
- Higher resolution = more frequent step updates
- Output value increments faster with higher resolution
- Affects timing precision of generated events

**Example:**
```chuck
AbletonLink link => blackhole;

// Quarter note resolution (4 steps per 4/4 measure)
1 => link.resolution;

// Eighth note resolution (8 steps per 4/4 measure)
2 => link.resolution;

// Sixteenth note resolution (16 steps per 4/4 measure)
4 => link.resolution;
```

### `offset(float offset_ms) -> float`

Set timing offset in milliseconds for latency compensation.

**Parameters:**
- `offset_ms` - Offset in milliseconds (can be positive or negative)

**Returns:** The offset value set

**Notes:**
- Use to compensate for audio hardware latency
- Positive values delay the Link timeline
- Negative values advance the Link timeline
- Useful for tight synchronization in performance scenarios

**Example:**
```chuck
AbletonLink link => blackhole;

// Compensate for 10ms output latency
10.0 => link.offset;

// Compensate for 5ms advance timing
-5.0 => link.offset;
```

### `reset(int trigger) -> int`

Reset the beat timeline to align with the current quantum.

**Parameters:**
- `trigger` - Any non-zero value triggers reset

**Returns:** The value passed

**Notes:**
- Forces immediate alignment to quantum boundaries
- Useful for re-synchronizing after drift
- Affects all Link peers on the network

**Example:**
```chuck
AbletonLink link => blackhole;

// Reset timeline
1 => link.reset;
```

---

## Tempo Control

### `tempo() -> float`

Get the current tempo from the Link network.

**Returns:** Current tempo in BPM

**Notes:**
- Returns the synchronized tempo from the Link network
- May differ from locally-set tempo if another peer changed it
- Poll regularly to detect tempo changes from other apps

**Example:**
```chuck
AbletonLink link => blackhole;

// Get current tempo
link.tempo() => float currentTempo;
<<< "Current tempo:", currentTempo, "BPM" >>>;

// Monitor tempo changes
0 => float lastTempo;
while (true) {
    link.tempo() => float tempo;
    if (tempo != lastTempo) {
        <<< "Tempo changed to:", tempo, "BPM" >>>;
        tempo => lastTempo;
    }
    100::ms => now;
}
```

### `quantum() -> int`

Get the current quantum setting.

**Returns:** Current quantum (beats per measure)

**Example:**
```chuck
AbletonLink link => blackhole;
4 => link.quantum;

link.quantum() => int q;
<<< "Quantum is:", q, "beats" >>>;
```

### `resolution() -> int`

Get the current resolution setting.

**Returns:** Current resolution (steps per beat)

**Example:**
```chuck
AbletonLink link => blackhole;
2 => link.resolution;

link.resolution() => int res;
<<< "Resolution is:", res, "steps per beat" >>>;
```

---

## Output Signal

AbletonLink extends **UGen** and outputs a signal representing the current step number.

### `.last()` (UGen output)

The UGen continuously outputs the current step value.

**Output Value:**
- Integer step number (0, 1, 2, 3, ...)
- Increments based on tempo and resolution
- Resets at measure boundaries when quantum changes

**Usage Pattern:**
```chuck
AbletonLink link => blackhole;

// Read current step value
link.last() => float currentStep;
```

**Notes:**
- Must connect to `blackhole` or `dac` for processing
- Output updates sample-by-sample
- Step changes occur at beat boundaries
- Use in sample-rate loop to detect step changes

---

## Synchronization

### Basic Synchronization Pattern

```chuck
AbletonLink link => blackhole;

// Configure
1 => link.enable;
120.0 => link.tempo;
4 => link.quantum;
1 => link.resolution;

-1 => float lastStep;

while (true) {
    link.last() => float currentStep;

    // Detect step changes
    if (currentStep != lastStep && currentStep >= 0) {
        <<< "New step:", currentStep >>>;

        // Trigger events on step changes
        // Your code here

        currentStep => lastStep;
    }

    1::samp => now;
}
```

### Measure Detection

```chuck
AbletonLink link => blackhole;

1 => link.enable;
120.0 => link.tempo;
4 => link.quantum;
1 => link.resolution;

-1 => float lastStep;

while (true) {
    link.last() => float currentStep;

    if (currentStep != lastStep && currentStep >= 0) {
        currentStep $ int => int step;

        // Detect start of measure (downbeat)
        if (step % link.quantum() == 0) {
            <<< "*** DOWNBEAT - Measure start ***" >>>;
        } else {
            <<< "Beat", (step % link.quantum()) + 1, "of", link.quantum() >>>;
        }

        currentStep => lastStep;
    }

    1::samp => now;
}
```

### Multi-Resolution Triggering

```chuck
AbletonLink link => blackhole;

1 => link.enable;
120.0 => link.tempo;
4 => link.quantum;
4 => link.resolution;  // Sixteenth note resolution

-1 => float lastStep;

while (true) {
    link.last() => float currentStep;

    if (currentStep != lastStep && currentStep >= 0) {
        currentStep $ int => int step;

        // Trigger different sounds on different subdivisions
        if (step % 16 == 0) {
            <<< "MEASURE START" >>>;
            // Kick drum
        } else if (step % 4 == 0) {
            <<< "BEAT" >>>;
            // Snare
        } else if (step % 2 == 0) {
            <<< "eighth" >>>;
            // Hi-hat closed
        } else {
            <<< "sixteenth" >>>;
            // Hi-hat open
        }

        currentStep => lastStep;
    }

    1::samp => now;
}
```

---

## Network Discovery

### Automatic Peer Discovery

When `enable(1)` is called, AbletonLink automatically:
1. Broadcasts presence on local network (UDP multicast)
2. Discovers other Link-enabled applications
3. Synchronizes tempo and phase with peers
4. Maintains synchronization continuously

**Requirements:**
- Same local network (WiFi or Ethernet)
- Network allows UDP multicast (port 20808)
- Firewall permits Link traffic

### No Manual Configuration

Link uses **zero-configuration networking**:
- No IP addresses to configure
- No server/client setup
- Peers automatically discover each other
- Works across different applications and platforms

---

## Usage Patterns

### Basic Metronome

```chuck
AbletonLink link => blackhole;
SinOsc click => ADSR env => dac;

env.set(1::ms, 1::ms, 0.0, 1::ms);

// Configure Link
1 => link.enable;
120.0 => link.tempo;
4 => link.quantum;
1 => link.resolution;

-1 => float lastStep;

while (true) {
    link.last() => float currentStep;

    if (currentStep != lastStep && currentStep >= 0) {
        // Downbeat on beat 1
        if (currentStep % 4 == 0) {
            800 => click.freq;
        } else {
            400 => click.freq;
        }

        env.keyOn();
        10::ms => now;
        env.keyOff();

        currentStep => lastStep;
    }

    1::samp => now;
}
```

### Drum Machine

```chuck
AbletonLink link => blackhole;

// Drum sounds
SndBuf kick => dac;
SndBuf snare => dac;
SndBuf hat => dac;

// Load samples
"kick.wav" => kick.read;
"snare.wav" => snare.read;
"hat.wav" => hat.read;

// Configure Link
1 => link.enable;
120.0 => link.tempo;
4 => link.quantum;
4 => link.resolution;  // Sixteenth notes

-1 => float lastStep;

while (true) {
    link.last() => float currentStep;

    if (currentStep != lastStep && currentStep >= 0) {
        currentStep $ int => int step;

        // Kick on beats 1 and 3
        if (step % 16 == 0 || step % 16 == 8) {
            0 => kick.pos;
        }

        // Snare on beats 2 and 4
        if (step % 16 == 4 || step % 16 == 12) {
            0 => snare.pos;
        }

        // Hi-hat on eighth notes
        if (step % 2 == 0) {
            0 => hat.pos;
        }

        currentStep => lastStep;
    }

    1::samp => now;
}
```

### Tempo Following

```chuck
AbletonLink link => blackhole;

1 => link.enable;
4 => link.quantum;
1 => link.resolution;

// Don't set tempo - let other apps control it
0 => float lastTempo;

while (true) {
    link.tempo() => float currentTempo;

    // React to tempo changes from other Link apps
    if (currentTempo != lastTempo) {
        <<< "Tempo changed to:", currentTempo, "BPM" >>>;

        // Adjust musical parameters based on tempo
        if (currentTempo > 140.0) {
            <<< "Fast tempo - use shorter notes" >>>;
        } else if (currentTempo < 100.0) {
            <<< "Slow tempo - use longer notes" >>>;
        }

        currentTempo => lastTempo;
    }

    100::ms => now;
}
```

### Multi-Peer Synchronization

```chuck
// Instance 1: Bassline
AbletonLink link => blackhole;

1 => link.enable;
120.0 => link.tempo;  // Propose initial tempo
4 => link.quantum;
1 => link.resolution;

SinOsc bass => dac;
0.5 => bass.gain;

[48, 48, 55, 55] @=> int bassline[];
-1 => float lastStep;

while (true) {
    link.last() => float currentStep;

    if (currentStep != lastStep && currentStep >= 0) {
        currentStep $ int => int step;
        Std.mtof(bassline[step % 4]) => bass.freq;
        currentStep => lastStep;
    }

    1::samp => now;
}
```

```chuck
// Instance 2: Drums (separate ChucK instance)
AbletonLink link => blackhole;

1 => link.enable;
// Don't set tempo - sync with Instance 1
4 => link.quantum;
4 => link.resolution;  // Subdivide beats

// Drums will automatically sync to Instance 1's tempo
// Add drum pattern here...
```

---

## Platform-Specific Notes

### Network Requirements

**macOS/Linux:**
- Requires UDP multicast support
- Default port: 20808
- Firewall must allow Link protocol

**Firewall Configuration:**
- Allow incoming/outgoing UDP on port 20808
- Allow multicast traffic (239.255.76.67)

### Link Library

AbletonLink uses the official **Ableton Link SDK**:
- Cross-platform C++ library
- Sample-accurate synchronization
- Peer-to-peer architecture (no server required)

### Performance

- Minimal CPU overhead
- Sample-accurate timing
- Sub-millisecond network latency (typical)
- Automatic network jitter compensation

---

## Timing Precision

### Sample-Accurate Events

AbletonLink provides **sample-accurate** step changes:
- Step values update at exact sample boundaries
- No timing jitter from network delays
- Compensates for network latency automatically

### Quantum Alignment

When quantum is set:
- Measures align across all Link peers
- Start/stop commands respect quantum boundaries
- Phase-locked synchronization

### Latency Compensation

Use `offset()` to compensate for:
- Audio interface latency
- Processing delays
- Network latency (if needed)

---

## Common Patterns

### Step Change Detection

```chuck
-1 => float lastStep;

while (true) {
    link.last() => float currentStep;

    if (currentStep != lastStep && currentStep >= 0) {
        // Step changed - trigger event
        currentStep => lastStep;
    }

    1::samp => now;
}
```

### Beat vs. Subdivision

```chuck
4 => link.quantum;
4 => link.resolution;  // 16 steps per measure

currentStep $ int => int step;

if (step % 4 == 0) {
    // On beat
} else {
    // On subdivision
}
```

### Measure Counting

```chuck
0 => int measureCount;

if (step % link.quantum() == 0) {
    measureCount++;
    <<< "Measure", measureCount >>>;
}
```

---

## Troubleshooting

### Link Not Syncing

**Problem:** ChucK doesn't sync with other Link apps

**Solutions:**
1. Verify `enable(1)` is called
2. Check network connectivity (same WiFi/LAN)
3. Disable firewall or allow port 20808
4. Verify other app has Link enabled
5. Try restarting both applications

### Tempo Drift

**Problem:** Tempo gradually drifts over time

**Solutions:**
1. Use `offset()` to compensate for latency
2. Call `reset()` to re-sync
3. Ensure sample rate is accurate
4. Check system clock accuracy

### Step Changes Not Detected

**Problem:** Step changes aren't triggering events

**Solutions:**
1. Verify resolution is set correctly
2. Check tempo is not zero
3. Ensure `link => blackhole` connection
4. Use `1::samp => now` in loop (not longer intervals)

---

## Best Practices

### Always Connect to blackhole

```chuck
// Required for Link to process
AbletonLink link => blackhole;
```

### Use Sample-Rate Polling

```chuck
// Efficient step detection
while (true) {
    link.last() => float currentStep;
    // Process step changes
    1::samp => now;
}
```

### Initialize Before Use

```chuck
AbletonLink link => blackhole;

// Configure before starting
1 => link.enable;
120.0 => link.tempo;
4 => link.quantum;
1 => link.resolution;

// Then start your musical logic
```

### Monitor Tempo Changes

```chuck
// Track tempo changes from other peers
0 => float lastTempo;
while (true) {
    link.tempo() => float tempo;
    if (tempo != lastTempo) {
        // Adapt to new tempo
        tempo => lastTempo;
    }
    100::ms => now;
}
```

---

## Complete Example: Synchronized Sequencer

```chuck
// Synchronized 4-step sequencer using AbletonLink
AbletonLink link => blackhole;

// Sound
SqrOsc osc => ADSR env => dac;
env.set(1::ms, 100::ms, 0.3, 50::ms);

// Configure Link
1 => link.enable;
120.0 => link.tempo;
4 => link.quantum;
4 => link.resolution;  // Sixteenth notes

// Sequencer pattern (MIDI notes)
[60, 64, 67, 72] @=> int sequence[];

<<< "AbletonLink Sequencer - Synced to:", link.tempo(), "BPM" >>>;

-1 => float lastStep;
0 => float lastTempo;

while (true) {
    // Monitor tempo changes
    link.tempo() => float currentTempo;
    if (currentTempo != lastTempo) {
        <<< "Tempo changed to:", currentTempo, "BPM" >>>;
        currentTempo => lastTempo;
    }

    // Get current step
    link.last() => float currentStep;

    // Trigger on step changes
    if (currentStep != lastStep && currentStep >= 0) {
        currentStep $ int => int step;

        // Play note every 4 steps (quarter notes)
        if (step % 4 == 0) {
            sequence[(step / 4) % sequence.cap()] => int note;
            Std.mtof(note) => osc.freq;
            env.keyOn();

            <<< "Step:", step, "Note:", note, "Measure:", (step / 16) >>>;
        }

        currentStep => lastStep;
    }

    1::samp => now;
}
```

---

## Return Values

### Setter Methods (Property Assignment)

Methods that set values return the value assigned:
- `enable(int)` → Returns `int`
- `tempo(float)` → Returns `float`
- `quantum(int)` → Returns `int`
- `resolution(int)` → Returns `int`
- `offset(float)` → Returns `float`
- `reset(int)` → Returns `int`

### Getter Methods

Methods that retrieve values:
- `tempo()` → Returns current tempo as `float`
- `quantum()` → Returns current quantum as `int`
- `resolution()` → Returns current resolution as `int`

### UGen Output

- `.last()` → Returns current step as `float` (integer values)
