// patch
SinOsc foo => dac;
// not so loud
.5 => foo.gain;

// time loop
while( true )
{
    Math.random2f( 30, 1000 ) => foo.freq;
    100::ms => now;
}
