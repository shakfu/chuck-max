// HostBufPlay: loop channel 0 of buffer~ ckbuf. follows the buffer~ as it is
// refilled, resized, or deleted (silence).
// in Max: [msg rate 0.5( -> [chuck~] sets the rate; negative plays backward

HostBufPlay p => dac;
0.5 => p.gain;
p.name( "ckbuf" );
1 => p.loop;

HostIn min;
min.addAddress( "rate" );
HostMsg m;
while( true )
{
    min => now;
    while( min.recv( m ) ) m.getFloat( 0 ) => p.rate;
}
