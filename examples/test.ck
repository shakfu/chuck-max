
global float freq;


SinOsc s => dac;

.2 => s.gain; 


while(true)
{
    freq => s.freq;
    100::ms => now;
}


