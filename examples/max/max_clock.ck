// MaxClock: click on each beat of the Max transport, accent on beat 1.
// the loop restarts on any transport change (start/stop, tempo, seek), so a
// wait computed at the old tempo never finishes late

MaxClock clk;
Impulse imp => ResonZ r => dac;
20 => r.Q;

fun void click()
{
    clk.beatInBar() => int b;
    ( b == 1 ? 1760 : 880 ) => r.freq;
    5 => imp.next;
    Max.send( "beat", b );
}

fun void beats()
{
    // toNextBeat() is never zero, so a start exactly on a beat (e.g. the
    // transport starting at 1.1.0) would skip it
    if( clk.unit() < 0.01 ) click();
    while( true )
    {
        clk.toNextBeat() => now;
        click();
    }
}

Shred @ loop;
while( true )
{
    if( loop != null ) loop.exit();
    null @=> loop;
    if( clk.running() ) spork ~ beats() @=> loop;
    clk => now;
}
