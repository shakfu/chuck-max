// MaxIn: Max -> ChucK messages. in Max: [msg note 60 0.8( or
// [msg note 64 0.5 legato( or [msg off( -> [chuck~]. echoes each note back
// with Max.send: reply outlet -> [route msg] -> [route got]

SinOsc s => ADSR env => dac;
env.set( 5::ms, 100::ms, 0.6, 200::ms );

MaxIn min;
min.addAddress( "note" );
min.addAddress( "off" );
MaxMsg m;

while( true )
{
    min => now;
    while( min.recv( m ) )
    {
        if( m.name() == "off" ) { env.keyOff(); continue; }
        Std.mtof( m.getInt( 0 ) ) => s.freq;
        m.getFloat( 1 ) * 0.5 => s.gain;
        // legato: change pitch without retriggering the envelope
        if( m.getString( 2 ) != "legato" ) env.keyOn();
        Max.send( "got", m.getInt( 0 ) );
    }
}
