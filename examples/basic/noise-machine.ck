// name: noise-machine.ck
// desc: a toddler noise machine
// created for Ava in Yosemite 2026.1.16

// patch
Noise n => LPF lpf => dac;

// time loop
while( true )
{
    // sweep center frequency of filter
    1800 + Math.sin(now/second*.25)*500 => lpf.freq;
    // advance time
    10::ms => now;
}
