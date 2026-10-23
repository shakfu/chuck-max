// MaxClock accuracy: at each beat, read [phasor~ 4n @lock 1] on inlet 1.
// at a beat the phasor wraps, so its value is the timing error in beats.
// sends: msg err <samples>   (expect |err| <= 1)

MaxClock clk;
adc => blackhole;

while( true )
{
    if( !clk.running() ) { clk => now; continue; }
    clk.toNextBeat() => now;
    adc.last() => float p;
    if( p > 0.5 ) p - 1 => p;
    Max.send( "err", p * ( clk.beat() / samp ) );
}
