// Max.send(name, value) -> chuck~ reply outlet: msg <name> <value>
// in Max: [chuck~] rightmost outlet -> [route msg] -> [route freq]

SinOsc s => dac;
0.2 => s.gain;

while( true )
{
    Math.random2f( 200, 800 ) => float f => s.freq;
    Max.send( "freq", f );
    250::ms => now;
}
