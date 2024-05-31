# FM envelopes with Max patch adding OSC control

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

- `play.tidal`:
    - Tidalcycles performance code




