// HostBuffer write: fill buffer~ ckbuf with a decaying 220 Hz tone.
// in Max: [buffer~ ckbuf 1000]; double-click it to see the result.
// reply outlet -> [route msg] -> [route written]: frames written

HostBuffer b;
b.name( "ckbuf" );

// the buffer~ reference is made on the main thread, a few ms from now
now + 1::second => time limit;
while( !b.ready() && now < limit ) 1::ms => now;
if( !b.ready() ) { <<< "no buffer~ named", b.name() >>>; me.exit(); }

b.frames() => int n;
b.sampleRate() => float sr;
for( 0 => int i; i < n; i++ )
{
    Math.sin( 2 * pi * 220 * i / sr ) * Math.exp( -4.0 * i / n ) => float v;
    for( 0 => int c; c < b.channels(); c++ ) b.poke( i, c, v );
}
Host.send( "written", n );
