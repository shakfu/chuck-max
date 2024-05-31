# FM envelopes with Max patch adding OSC control for Tidalcycles

## Files

- `fmenv-event.ck`: 
    - FM synthesis defined in a public class
    - modulator envelopes and envelope curves
    - ChucK event / eventHandler function
    - global variables for use within Max/MSP patch
    - run locally outside of Max via variable setting and initializing global values

- `fmenv-ampmod-osc.maxpat`: 
    - FM synthesis and envelope parameters
    - amplitude modulation in Max/gen~ 
    - routing logic for OSC (Open Sound Control) for external control of parameters

- `osc-target.tidal`:
    - Initialization code used with Tidalcycles to define an OSC target and initialize all of the paramters used in the Max patch
    - Includes parameters that Max uses for ChucK globals
    - Includes parameters used only in Max/MSP

- `play.tidal`: Tidalcycles performance code

## Usage

*Requirements:* 

- A functioning Tidalcycles environment with Haskell (ghci) and a REPL editor configured for Tidal (Pulsar, VS Code, NeoVim, etc). 
- SuperDirt/SuperCollider is not required, but the OSC target can be configured to support OSC messages to multiple targets (Max/MSP and SuperCollider).

The Max patch can be used in standalone mode or together with Tidalcycles (a live coding platform - https://tidalcycles.org). 

For use with Tidalcycles you need to first initialize the OSC target and then use Tidal code with the specific parameter names defined in the `oscplay` array. 

### Initialize the OSC target

In your REPL editor, run the sections of the `osc-target.tidal` file. 
When executing the `stream <- startStream defaultConfig oscmap` command, you may get an error message. This can be ignored. 

```
Listening for external controls on 127.0.0.1:6010
That port isn't available, perhaps another Tidal instance is already listening on that port?
```

### use Tidal code with the ChucK - Max parameters

- See `play.tidal` for specific examples. 
- The sound parameter (s):
    - A value of `h` will trigger the event
    - Any other value won't trigger an event but will ensure that parameter changes are sent. For example: `P!24` below will send 24 OSC messages per cycle, which allows the frequency to change gradually during the duration.
- Note the use of parameter values `#fmGain 0.2`. These need to be the exact parameter names defined in the osc-target.

```haskell
x1 $ s "[h, P!24]" #orbit 0
  #fmGain 0.2
  #fmFreq (slow 8 $ range 100 120 $ sine)
  #fmHarm 1.5
  #fmModIndx (rand * 15 + 2)
  #envDur "<1750 2000 4000 500>"
  #envAtt 0.2 #envDec 0.7 #envSusLev 0.8 #envRel 2
  #envCurAtt 0.1 #envCurDec 0.1 #envCurRel 0.5
   #amOn 1 #amFreq 8 #amDuty 0.1 #amRDel 250

```



