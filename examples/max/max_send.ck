// Host.msg(name, value) -> chuck~ reply outlet: msg <name> <value>
// also Host.msg(name) -> msg <name>; Host.msg(name, "sym"); Host.msg(name, [1.0, 2.0])
// in Max: [chuck~] rightmost outlet -> [route msg] -> [route freq]

SinOsc s => dac;
0.2 => s.gain;

while( true )
{
    Math.random2f( 200, 800 ) => float f => s.freq;
    Host.msg( "freq", f );
    250::ms => now;
}
