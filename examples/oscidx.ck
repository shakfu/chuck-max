global int oscIndex;
global float incrFreq;
global float incrSecond;

//0 => oscIndex;
20 => incrFreq;
2 => incrSecond;

Osc u[5];

TriOsc tri @=> u[0];
SinOsc sin @=> u[1];
SawOsc saw @=> u[2];
SqrOsc sqr @=> u[3];
PulseOsc pul @=> u[4];

u[oscIndex] => dac;

110 => u[oscIndex].freq;
0.4 => u[oscIndex].gain;

while(u[oscIndex].freq() < 2000)
{
    incrSecond::second => now;
    u[oscIndex].freq() + incrFreq => u[oscIndex].freq;
    <<< u[oscIndex].freq() >>>;
}

